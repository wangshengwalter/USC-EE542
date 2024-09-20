#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <libgen.h>

#define MAX_PACKET_SIZE 8500 // 1KB
#define ACK_TIMEOUT 1 // 1 second timeout for ACK
#define MAX_FILENAME_SIZE 256

typedef struct {
    int seq_num;
    int is_last;
    int data_size;
    char filename[MAX_FILENAME_SIZE];
    char data[MAX_PACKET_SIZE - 3 * sizeof(int) - MAX_FILENAME_SIZE];
} Packet;

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

    // Wait for ACK
    fd_set readfds;
    struct timeval tv;
    FD_ZERO(&readfds);
    FD_SET(sock, &readfds);
    tv.tv_sec = ACK_TIMEOUT;
    tv.tv_usec = 0;

    int activity = select(sock + 1, &readfds, NULL, NULL, &tv);
    if (activity < 0) {
        perror("select error");
        exit(EXIT_FAILURE);
    } else if (activity == 0) {
        printf("Timeout waiting for ACK, resending packet %d\n", packet->seq_num);
        send_packet(sock, packet, server_addr); // Recursively resend
    } else {
        int ack;
        if (recv(sock, &ack, sizeof(ack), 0) < 0) {
            perror("recv failed");
            exit(EXIT_FAILURE);
        }
        if (ack != packet->seq_num) {
            printf("Received wrong ACK, resending packet %d\n", packet->seq_num);
            send_packet(sock, packet, server_addr); // Recursively resend
        }
    }
}

void send_file(const char* filename, const char* server_ip, int server_port) {
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
    size_t bytes_read;
    char* base_filename = basename((char*)filename); // Get filename without path

    while ((bytes_read = fread(packet.data, 1, sizeof(packet.data), file)) > 0) {
        packet.seq_num = seq_num++;
        packet.is_last = feof(file) ? 1 : 0;
        packet.data_size = bytes_read;
        strncpy(packet.filename, base_filename, MAX_FILENAME_SIZE - 1);
        packet.filename[MAX_FILENAME_SIZE - 1] = '\0'; // Ensure null-termination

        send_packet(sock, &packet, &server_addr);
        printf("Sent packet %d (%zu bytes)\n", packet.seq_num, bytes_read);
    }

    fclose(file);
    close(sock);
    printf("File transfer complete\n");
}

int main(int argc, char* argv[]) {
    if (argc != 4) {
        fprintf(stderr, "Usage: %s <filename> <server_ip> <server_port>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    const char* filename = argv[1];
    const char* server_ip = argv[2];
    int server_port = atoi(argv[3]);

    send_file(filename, server_ip, server_port);

    return 0;
}