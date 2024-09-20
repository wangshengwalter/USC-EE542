#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <pthread.h>
#include <errno.h>

#define MAX_PACKET_SIZE 8192
#define WINDOW_SIZE 64
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
    int is_received[WINDOW_SIZE];
    uint32_t next_expected_seq;
    pthread_mutex_t mutex;
    pthread_cond_t cv;
} ReceiveWindow;

typedef struct {
    int sock;
    ReceiveWindow* window;
    volatile int transfer_complete;
} ThreadArgs;

// Function prototypes
int create_socket(const char* ip, int port);
void init_receive_window(ReceiveWindow* window);
void* receive_packets_thread(void* arg);
void* write_to_file_thread(void* arg);
void process_received_packets(ReceiveWindow* window, FILE* file);
void send_cumulative_ack(int sock, const struct sockaddr_in* client_addr, uint32_t ack_num);
uint16_t calculate_checksum(const Packet* packet);

int main(int argc, char* argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <ip> <port>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    const char* ip = argv[1];
    int port = atoi(argv[2]);

    int sock = create_socket(ip, port);
    ReceiveWindow window;
    init_receive_window(&window);

    ThreadArgs args = {
        .sock = sock,
        .window = &window,
        .transfer_complete = 0
    };

    pthread_t receive_thread, write_thread;
    pthread_create(&receive_thread, NULL, receive_packets_thread, &args);
    pthread_create(&write_thread, NULL, write_to_file_thread, &args);

    pthread_join(receive_thread, NULL);
    pthread_join(write_thread, NULL);

    close(sock);
    printf("Server shutting down\n");

    return 0;
}

int create_socket(const char* ip, int port) {
    int sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock < 0) {
        perror("socket creation failed");
        exit(EXIT_FAILURE);
    }

    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr(ip);
    server_addr.sin_port = htons(port);

    if (bind(sock, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    printf("Server listening on %s:%d\n", ip, port);
    return sock;
}

void init_receive_window(ReceiveWindow* window) {
    window->next_expected_seq = 0;
    memset(window->is_received, 0, sizeof(window->is_received));
    pthread_mutex_init(&window->mutex, NULL);
    pthread_cond_init(&window->cv, NULL);
}

void* receive_packets_thread(void* arg) {
    ThreadArgs* args = (ThreadArgs*)arg;
    struct sockaddr_in client_addr;
    socklen_t addr_len = sizeof(client_addr);
    Packet received_packet;

    while (!args->transfer_complete) {
        ssize_t received = recvfrom(args->sock, &received_packet, sizeof(Packet), 0,
                                    (struct sockaddr*)&client_addr, &addr_len);
        if (received < 0) {
            if (errno != EAGAIN && errno != EWOULDBLOCK) {
                perror("recvfrom failed");
            }
            continue;
        }

        if (calculate_checksum(&received_packet) != received_packet.checksum) {
            printf("Packet checksum mismatch, discarding packet %u\n", received_packet.seq_num);
            continue;
        }

        pthread_mutex_lock(&args->window->mutex);
        if (received_packet.seq_num >= args->window->next_expected_seq &&
            received_packet.seq_num < args->window->next_expected_seq + WINDOW_SIZE) {
            int index = received_packet.seq_num % WINDOW_SIZE;
            if (!args->window->is_received[index]) {
                args->window->packets[index] = received_packet;
                args->window->is_received[index] = 1;
                pthread_cond_signal(&args->window->cv);
            }
        }
        pthread_mutex_unlock(&args->window->mutex);

        send_cumulative_ack(args->sock, &client_addr, args->window->next_expected_seq - 1);

        if (received_packet.flags & 0x0001) { // FIN flag
            args->transfer_complete = 1;
            // Send FIN-ACK
            Packet fin_ack = {0};
            fin_ack.flags = 0x0011; // FIN-ACK flags
            fin_ack.ack_num = received_packet.seq_num;
            fin_ack.checksum = calculate_checksum(&fin_ack);
            sendto(args->sock, &fin_ack, sizeof(Packet), 0, (struct sockaddr*)&client_addr, addr_len);
        }
    }
    return NULL;
}

void* write_to_file_thread(void* arg) {
    ThreadArgs* args = (ThreadArgs*)arg;
    FILE* file = NULL;
    char current_filename[MAX_FILENAME_SIZE] = {0};

    while (!args->transfer_complete) {
        pthread_mutex_lock(&args->window->mutex);
        while (args->window->is_received[args->window->next_expected_seq % WINDOW_SIZE] == 0 &&
               !args->transfer_complete) {
            pthread_cond_wait(&args->window->cv, &args->window->mutex);
        }

        if (args->transfer_complete) {
            pthread_mutex_unlock(&args->window->mutex);
            break;
        }

        Packet* packet = &args->window->packets[args->window->next_expected_seq % WINDOW_SIZE];
        
        if (strcmp(current_filename, packet->filename) != 0) {
            if (file != NULL) {
                fclose(file);
            }
            strncpy(current_filename, packet->filename, MAX_FILENAME_SIZE);
            file = fopen(current_filename, "wb");
            if (file == NULL) {
                perror("Failed to create file");
                exit(EXIT_FAILURE);
            }
            printf("Creating new file: %s\n", current_filename);
        }

        fwrite(packet->data, 1, packet->data_size, file);
        printf("Wrote packet %u to file %s (%d bytes)\n", packet->seq_num, current_filename, packet->data_size);

        args->window->is_received[args->window->next_expected_seq % WINDOW_SIZE] = 0;
        args->window->next_expected_seq++;

        pthread_mutex_unlock(&args->window->mutex);
    }

    if (file != NULL) {
        fclose(file);
    }
    return NULL;
}

void send_cumulative_ack(int sock, const struct sockaddr_in* client_addr, uint32_t ack_num) {
    Packet ack_packet = {0};
    ack_packet.ack_num = ack_num;
    ack_packet.flags = 0x0010; // ACK flag
    ack_packet.checksum = calculate_checksum(&ack_packet);
    sendto(sock, &ack_packet, sizeof(Packet), 0, (struct sockaddr*)client_addr, sizeof(*client_addr));
}

uint16_t calculate_checksum(const Packet* packet) {
    // Implement a proper checksum algorithm here
    // For simplicity, we're using a basic sum of all bytes
    const uint8_t* data = (const uint8_t*)packet;
    uint32_t sum = 0;
    for (size_t i = 0; i < sizeof(Packet); i++) {
        sum += data[i];
    }
    return (uint16_t)(sum & 0xFFFF);
}