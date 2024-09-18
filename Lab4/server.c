// server.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/time.h>

#define MAX_PACKET_SIZE 1024
#define ACK_TIMEOUT 1 // 1 second timeout for ACK

typedef struct {
    int seq_num;
    int is_last;
    int data_size;
    char data[MAX_PACKET_SIZE - 3 * sizeof(int)];
} Packet;

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

int receive_packet(int sock, Packet* packet, struct sockaddr_in* sender_addr) {
    socklen_t sender_len = sizeof(*sender_addr);
    
    ssize_t received = recvfrom(sock, packet, sizeof(Packet), 0, (struct sockaddr*)sender_addr, &sender_len);
    if (received < 0) {
        perror("recvfrom failed");
        return -1;
    }

    // Send ACK
    if (sendto(sock, &packet->seq_num, sizeof(packet->seq_num), 0, (struct sockaddr*)sender_addr, sender_len) < 0) {
        perror("sendto (ACK) failed");
        return -1;
    }

    return 0;
}

void run_server(const char* ip, int port) {
    int sock = create_socket(ip, port);
    struct sockaddr_in client_addr;
    Packet packet;
    FILE* file = NULL;
    int seq_num = 0;

    printf("Server listening on %s:%d\n", ip, port);

    while (1) {
        if (receive_packet(sock, &packet, &client_addr) == 0) {
            if (seq_num == packet.seq_num) {
                if (file == NULL) {
                    char filename[256];
                    snprintf(filename, sizeof(filename), "received_file_%d", seq_num);
                    file = fopen(filename, "wb");
                    if (file == NULL) {
                        perror("Failed to create file");
                        continue;
                    }
                    printf("Creating new file: %s\n", filename);
                }

                fwrite(packet.data, 1, packet.data_size, file);
                printf("Received packet %d (%d bytes)\n", seq_num, packet.data_size);
                seq_num++;

                if (packet.is_last) {
                    printf("File transfer complete\n");
                    fclose(file);
                    file = NULL;
                    seq_num = 0;
                }
            } else {
                printf("Received out-of-order packet. Expected %d, got %d\n", seq_num, packet.seq_num);
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
