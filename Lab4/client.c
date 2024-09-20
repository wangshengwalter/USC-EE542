#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <libgen.h>
#include <errno.h>

#define MAX_PACKET_SIZE 1024
#define MAX_FILENAME_SIZE 256
#define DEFAULT_WINDOW_SIZE 10
#define DEFAULT_TIMEOUT 1.0

typedef struct {
    int seq_num;
    int is_last;
    int data_size;
    char filename[MAX_FILENAME_SIZE];
    char data[MAX_PACKET_SIZE - 3 * sizeof(int) - MAX_FILENAME_SIZE];
} Packet;

typedef struct {
    Packet packet;
    int acked;
    struct timeval send_time;
} WindowSlot;

int create_socket() {
    int sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock < 0) {
        perror("socket creation failed");
        exit(EXIT_FAILURE);
    }
    return sock;
}

void send_packet(int sock, const Packet* packet, const struct sockaddr_in* server_addr) {
    if (sendto(sock, packet, sizeof(Packet), 0, (struct sockaddr*)server_addr, sizeof(*server_addr)) < 0) {
        perror("sendto failed");
        exit(EXIT_FAILURE);
    }
}

void receive_ack(int sock, int* ack, struct timeval* timeout) {
    fd_set readfds;
    FD_ZERO(&readfds);
    FD_SET(sock, &readfds);

    int activity = select(sock + 1, &readfds, NULL, NULL, timeout);
    if (activity < 0) {
        perror("select error");
        exit(EXIT_FAILURE);
    } else if (activity == 0) {
        *ack = -1;  // Timeout
    } else {
        if (recv(sock, ack, sizeof(*ack), 0) < 0) {
            perror("recv failed");
            exit(EXIT_FAILURE);
        }
    }
}

void send_file(const char* filename, const char* server_ip, int server_port, int window_size, float timeout) {
    int sock = create_socket();
    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr(server_ip);
    server_addr.sin_port = htons(server_port);

    FILE* file = fopen(filename, "rb");
    if (file == NULL) {
        fprintf(stderr, "Failed to open file '%s': %s\n", filename, strerror(errno));
        exit(EXIT_FAILURE);
    }

    printf("Successfully opened file: %s\n", filename);

    WindowSlot* window = (WindowSlot*)malloc(window_size * sizeof(WindowSlot));
    if (window == NULL) {
        perror("Failed to allocate memory for window");
        exit(EXIT_FAILURE);
    }

    int base = 0;
    int next_seq_num = 0;
    char* base_filename = basename((char*)filename);
    int file_finished = 0;

    while (base < next_seq_num || !file_finished) {
        // Fill window
        while (next_seq_num < base + window_size && !file_finished) {
            Packet* packet = &window[next_seq_num % window_size].packet;
            packet->seq_num = next_seq_num;
            packet->data_size = fread(packet->data, 1, sizeof(packet->data), file);
            packet->is_last = feof(file);
            strncpy(packet->filename, base_filename, MAX_FILENAME_SIZE - 1);
            packet->filename[MAX_FILENAME_SIZE - 1] = '\0';

            send_packet(sock, packet, &server_addr);
            gettimeofday(&window[next_seq_num % window_size].send_time, NULL);
            window[next_seq_num % window_size].acked = 0;

            printf("Sent packet %d (%d bytes)\n", next_seq_num, packet->data_size);

            next_seq_num++;
            if (packet->is_last) {
                file_finished = 1;
            }
        }

        // Receive ACKs
        struct timeval tv;
        tv.tv_sec = (int)timeout;
        tv.tv_usec = (timeout - (int)timeout) * 1000000;

        int ack;
        receive_ack(sock, &ack, &tv);

        if (ack >= base && ack < next_seq_num) {
            window[ack % window_size].acked = 1;
            while (window[base % window_size].acked) {
                base++;
                if (base == next_seq_num) break;
            }
        } else if (ack == -1) {  // Timeout
            for (int i = base; i < next_seq_num; i++) {
                if (!window[i % window_size].acked) {
                    send_packet(sock, &window[i % window_size].packet, &server_addr);
                    gettimeofday(&window[i % window_size].send_time, NULL);
                    printf("Resent packet %d\n", i);
                }
            }
        }
    }

    free(window);
    fclose(file);
    close(sock);
    printf("File transfer complete\n");
}

int main(int argc, char* argv[]) {
    if (argc != 4 && argc != 6) {
        fprintf(stderr, "Usage: %s <filename> <server_ip> <server_port> [window_size] [timeout]\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    const char* filename = argv[1];
    const char* server_ip = argv[2];
    int server_port = atoi(argv[3]);
    int window_size = DEFAULT_WINDOW_SIZE;
    float timeout = DEFAULT_TIMEOUT;

    if (argc == 6) {
        window_size = atoi(argv[4]);
        timeout = atof(argv[5]);
        if (window_size <= 0 || timeout <= 0) {
            fprintf(stderr, "Invalid window size or timeout. Using defaults.\n");
            window_size = DEFAULT_WINDOW_SIZE;
            timeout = DEFAULT_TIMEOUT;
        }
    }

    send_file(filename, server_ip, server_port, window_size, timeout);

    return 0;
}