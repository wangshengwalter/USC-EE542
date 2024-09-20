// client.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/time.h>

#define MAX_PACKET_SIZE 1500 
#define ACK_TIMEOUT 10000 // 10000 msecond timeout for ACK
#define MAX_RETRIES 3

typedef struct {
    int seq_num;
    int is_last;
    int data_size;
    char data[MAX_PACKET_SIZE - 3 * sizeof(int)];
} Packet;

int create_socket() {
    int sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock < 0) {
        perror("socket creation failed");
        exit(EXIT_FAILURE);
    }
    return sock;
}

int send_packet(int sock, Packet* packet, struct sockaddr_in* dest_addr) {
    for (int attempt = 0; attempt < MAX_RETRIES; attempt++) {
        if (sendto(sock, packet, sizeof(Packet), 0, (struct sockaddr*)dest_addr, sizeof(*dest_addr)) < 0) {
            perror("sendto failed");
            return -1;
        }

        // Wait for ACK
        fd_set readfds;
        struct timeval tv;
        FD_ZERO(&readfds);
        FD_SET(sock, &readfds);
        tv.tv_sec = 0;
        tv.tv_usec = ACK_TIMEOUT;

        int ready = select(sock + 1, &readfds, NULL, NULL, &tv);
        if (ready > 0) {
            int ack;
            struct sockaddr_in sender_addr;
            socklen_t sender_len = sizeof(sender_addr);
            if (recvfrom(sock, &ack, sizeof(ack), 0, (struct sockaddr*)&sender_addr, &sender_len) < 0) {
                perror("recvfrom failed");
                return -1;
            }
            if (ack == packet->seq_num) {
                return 0;
            }
        }
        printf("Timeout, retrying... (Attempt %d)\n", attempt + 1);
    }

    printf("Failed to send packet after multiple attempts\n");
    return -1;
}

void run_client(const char* server_ip, int server_port, const char* filename) {
    int sock = create_socket();
    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr(server_ip);
    server_addr.sin_port = htons(server_port);

    FILE* file = fopen(filename, "rb");
    if (file == NULL) {
        perror("Failed to open file");
        exit(EXIT_FAILURE);
    }

    Packet packet;
    int seq_num = 0;
    int is_last = 0;

    while (!is_last) {
        packet.seq_num = seq_num;
        packet.data_size = fread(packet.data, 1, sizeof(packet.data), file);
        is_last = feof(file);
        packet.is_last = is_last;

        if (send_packet(sock, &packet, &server_addr) == 0) {
            printf("Sent packet %d (%d bytes)\n", seq_num, packet.data_size);
            seq_num++;
        } else {
            printf("Failed to send packet %d\n", seq_num);
            // Optionally, you might want to implement a retry mechanism here
        }
    }

    fclose(file);
    close(sock);
    printf("File transfer complete\n");
}

int main(int argc, char* argv[]) {
    if (argc != 4) {
        fprintf(stderr, "Usage: %s <server_ip> <server_port> <filename>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    const char* server_ip = argv[1];
    int server_port = atoi(argv[2]);
    const char* filename = argv[3];

    run_client(server_ip, server_port, filename);

    return 0;
}
