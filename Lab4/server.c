#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/time.h>

#define MAX_PACKET_SIZE 8500
#define MAX_FILENAME_SIZE 256
#define WINDOW_SIZE 10

typedef struct {
    int seq_num;
    int is_last;
    int data_size;
    char filename[MAX_FILENAME_SIZE];
    char data[MAX_PACKET_SIZE - 3 * sizeof(int) - MAX_FILENAME_SIZE];
} Packet;

typedef struct {
    Packet packet;
    int received;
} WindowSlot;

int create_socket(const char* ip, int port) {
    int sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock < 0) {
        perror("socket creation failed");
        exit(EXIT_FAILURE);
    }

    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr(ip);
    addr.sin_port = htons(port);

    if (bind(sock, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    return sock;
}

void send_ack(int sock, int ack, struct sockaddr_in* client_addr) {
    if (sendto(sock, &ack, sizeof(ack), 0, (struct sockaddr*)client_addr, sizeof(*client_addr)) < 0) {
        perror("sendto (ACK) failed");
    }
    printf("Sent ACK %d\n", ack);
}

void run_server(const char* ip, int port) {
    int sock = create_socket(ip, port);
    struct sockaddr_in client_addr;
    Packet packet;
    FILE* file = NULL;
    WindowSlot window[WINDOW_SIZE];
    int base = 0;
    char current_filename[MAX_FILENAME_SIZE] = {0};

    printf("Server listening on %s:%d\n", ip, port);

    while (1) {
        socklen_t client_len = sizeof(client_addr);
        ssize_t received = recvfrom(sock, &packet, sizeof(Packet), 0, (struct sockaddr*)&client_addr, &client_len);

        if (received < 0) {
            perror("recvfrom failed");
            continue;
        }

        if (packet.seq_num >= base && packet.seq_num < base + WINDOW_SIZE) {
            int index = packet.seq_num % WINDOW_SIZE;
            window[index].packet = packet;
            window[index].received = 1;

            printf("Received packet %d for file %s (%d bytes)\n", packet.seq_num, packet.filename, packet.data_size);

            while (window[base % WINDOW_SIZE].received) {
                if (strcmp(current_filename, packet.filename) != 0) {
                    if (file != NULL) {
                        fclose(file);
                    }
                    strncpy(current_filename, packet.filename, MAX_FILENAME_SIZE);
                    file = fopen(current_filename, "wb");
                    if (file == NULL) {
                        perror("Failed to create file");
                        exit(EXIT_FAILURE);
                    }
                    printf("Creating new file: %s\n", current_filename);
                }

                fwrite(window[base % WINDOW_SIZE].packet.data, 1, window[base % WINDOW_SIZE].packet.data_size, file);
                
                if (window[base % WINDOW_SIZE].packet.is_last) {
                    printf("File transfer complete: %s\n", current_filename);
                    fclose(file);
                    file = NULL;
                    memset(current_filename, 0, MAX_FILENAME_SIZE);
                }

                printf("Cur base is %d\n", base);
                window[base % WINDOW_SIZE].received = 0;
                base++;

                send_ack(sock, base - 1, &client_addr);
            }
        }
    }

    close(sock);
}

int main(int argc, char* argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <ip> <port>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    const char* ip = argv[1];
    int port = atoi(argv[2]);

    run_server(ip, port);

    return 0;
}