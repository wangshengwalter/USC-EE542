#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <pthread.h>
#include <errno.h>
#include <libgen.h>

#define MAX_PACKET_SIZE 8192
#define WINDOW_SIZE 64
#define INITIAL_TIMEOUT_MS 100
#define MAX_FILENAME_SIZE 256

typedef struct {
    uint32_t seq_num;
    uint32_t ack_num;
    uint16_t flags;
    uint16_t checksum;
    uint16_t data_size;
    char filename[MAX_FILENAME_SIZE];
    char data[MAX_PACKET_SIZE];
} Packet;

typedef struct {
    Packet packets[WINDOW_SIZE];
    int is_acked[WINDOW_SIZE];
    int base;
    int next_seq_num;
    pthread_mutex_t mutex;
    pthread_cond_t cv;
} SlidingWindow;

typedef struct {
    int sock;
    struct sockaddr_in server_addr;
    SlidingWindow* window;
    FILE* file;
    volatile int transfer_complete;
} ThreadArgs;

// Function prototypes
int create_socket();
void setup_server_addr(struct sockaddr_in* server_addr, const char* server_ip, int server_port);
void init_sliding_window(SlidingWindow* window);
void* send_packets_thread(void* arg);
void* receive_acks_thread(void* arg);
void send_packet(int sock, const Packet* packet, const struct sockaddr_in* server_addr);
uint16_t calculate_checksum(const Packet* packet);
void fill_window_from_file(SlidingWindow* window, FILE* file, const char* filename);
void wait_for_window_space(SlidingWindow* window);
void send_fin_packet(int sock, const struct sockaddr_in* server_addr);

int main(int argc, char* argv[]) {
    if (argc != 4) {
        fprintf(stderr, "Usage: %s <filename> <server_ip> <server_port>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    const char* filename = argv[1];
    const char* server_ip = argv[2];
    int server_port = atoi(argv[3]);

    int sock = create_socket();
    struct sockaddr_in server_addr;
    setup_server_addr(&server_addr, server_ip, server_port);

    FILE* file = fopen(filename, "rb");
    if (file == NULL) {
        perror("Failed to open file");
        exit(EXIT_FAILURE);
    }

    SlidingWindow window;
    init_sliding_window(&window);

    ThreadArgs args = {
        .sock = sock,
        .server_addr = server_addr,
        .window = &window,
        .file = file,
        .transfer_complete = 0
    };

    pthread_t send_thread, ack_thread;
    pthread_create(&send_thread, NULL, send_packets_thread, &args);
    pthread_create(&ack_thread, NULL, receive_acks_thread, &args);

    char* base_filename = basename((char*)filename);
    while (!args.transfer_complete) {
        fill_window_from_file(&window, file, base_filename);
        wait_for_window_space(&window);
    }

    send_fin_packet(sock, &server_addr);

    pthread_join(send_thread, NULL);
    pthread_join(ack_thread, NULL);

    fclose(file);
    close(sock);
    printf("File transfer complete\n");

    return 0;
}

int create_socket() {
    int sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock < 0) {
        perror("socket creation failed");
        exit(EXIT_FAILURE);
    }
    return sock;
}

void setup_server_addr(struct sockaddr_in* server_addr, const char* server_ip, int server_port) {
    memset(server_addr, 0, sizeof(*server_addr));
    server_addr->sin_family = AF_INET;
    server_addr->sin_addr.s_addr = inet_addr(server_ip);
    server_addr->sin_port = htons(server_port);
}

void init_sliding_window(SlidingWindow* window) {
    window->base = 0;
    window->next_seq_num = 0;
    memset(window->is_acked, 0, sizeof(window->is_acked));
    pthread_mutex_init(&window->mutex, NULL);
    pthread_cond_init(&window->cv, NULL);
}

void* send_packets_thread(void* arg) {
    ThreadArgs* args = (ThreadArgs*)arg;
    while (!args->transfer_complete) {
        pthread_mutex_lock(&args->window->mutex);
        while (args->window->next_seq_num < args->window->base + WINDOW_SIZE &&
               !args->window->is_acked[args->window->next_seq_num % WINDOW_SIZE]) {
            send_packet(args->sock, &args->window->packets[args->window->next_seq_num % WINDOW_SIZE], &args->server_addr);
            args->window->next_seq_num++;
        }
        pthread_mutex_unlock(&args->window->mutex);
        usleep(1000); // Sleep for 1ms to avoid busy waiting
    }
    return NULL;
}

void* receive_acks_thread(void* arg) {
    ThreadArgs* args = (ThreadArgs*)arg;
    Packet ack_packet;
    struct sockaddr_in server_addr;
    socklen_t addr_len = sizeof(server_addr);

    while (!args->transfer_complete) {
        ssize_t received = recvfrom(args->sock, &ack_packet, sizeof(Packet), 0,
                                    (struct sockaddr*)&server_addr, &addr_len);
        if (received < 0) {
            if (errno != EAGAIN && errno != EWOULDBLOCK) {
                perror("recvfrom failed");
            }
            continue;
        }

        if (ack_packet.flags & 0x0010) { // ACK flag
            pthread_mutex_lock(&args->window->mutex);
            for (int i = args->window->base; i <= ack_packet.ack_num; i++) {
                args->window->is_acked[i % WINDOW_SIZE] = 1;
            }
            args->window->base = ack_packet.ack_num + 1;
            pthread_cond_signal(&args->window->cv);
            pthread_mutex_unlock(&args->window->mutex);
        }

        if (ack_packet.flags & 0x0001) { // FIN flag
            args->transfer_complete = 1;
        }
    }
    return NULL;
}

void send_packet(int sock, const Packet* packet, const struct sockaddr_in* server_addr) {
    if (sendto(sock, packet, sizeof(Packet), 0,
               (struct sockaddr*)server_addr, sizeof(*server_addr)) < 0) {
        perror("sendto failed");
    }
}

uint16_t calculate_checksum(const Packet* packet) {
    // Implement a proper checksum algorithm here
    return 0;
}

void fill_window_from_file(SlidingWindow* window, FILE* file, const char* filename) {
    pthread_mutex_lock(&window->mutex);
    while (window->next_seq_num < window->base + WINDOW_SIZE) {
        Packet* packet = &window->packets[window->next_seq_num % WINDOW_SIZE];
        packet->seq_num = window->next_seq_num;
        packet->data_size = fread(packet->data, 1, MAX_PACKET_SIZE, file);
        if (packet->data_size == 0) {
            break; // End of file
        }
        packet->flags = 0x0000; // No flags set
        strncpy(packet->filename, filename, MAX_FILENAME_SIZE - 1);
        packet->filename[MAX_FILENAME_SIZE - 1] = '\0';
        packet->checksum = calculate_checksum(packet);
        window->next_seq_num++;
    }
    pthread_mutex_unlock(&window->mutex);
}

void wait_for_window_space(SlidingWindow* window) {
    pthread_mutex_lock(&window->mutex);
    while (window->next_seq_num >= window->base + WINDOW_SIZE) {
        pthread_cond_wait(&window->cv, &window->mutex);
    }
    pthread_mutex_unlock(&window->mutex);
}

void send_fin_packet(int sock, const struct sockaddr_in* server_addr) {
    Packet fin_packet = {0};
    fin_packet.flags = 0x0001; // FIN flag
    fin_packet.checksum = calculate_checksum(&fin_packet);
    send_packet(sock, &fin_packet, server_addr);
}