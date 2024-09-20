#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <libgen.h>
#include <errno.h>

#define MAX_PACKET_SIZE 1500 // Changed to 1KB
#define ACK_TIMEOUT_MS 40 // Changed to 40ms
#define MAX_RETRIES 5 // Added max retries for packet loss
#define MAX_FILENAME_SIZE 

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

int send_packet(int sock, const Packet* packet, const struct sockaddr_in* server_addr) {
    int retries = 0;
    while (retries < MAX_RETRIES) {
        if (sendto(sock, packet, sizeof(Packet), 0, (struct sockaddr*)server_addr, sizeof(*server_addr)) < 0) {
            perror("sendto failed");
            return -1;
        }

        // Wait for ACK
        fd_set readfds;
        struct timeval tv;
        FD_ZERO(&readfds);
        FD_SET(sock, &readfds);
        tv.tv_sec = 0;
        tv.tv_usec = ACK_TIMEOUT_MS * 1000; // Convert ms to μs

        int activity = select(sock + 1, &readfds, NULL, NULL, &tv);
        if (activity < 0) {
            if (errno == EINTR) continue; // Interrupted system call, try again
            perror("select error");
            return -1;
        } else if (activity == 0) {
            printf("Timeout waiting for ACK, retrying packet %d (attempt %d)\n", packet->seq_num, retries + 1);
            retries++;
        } else {
            int ack;
            if (recv(sock, &ack, sizeof(ack), 0) < 0) {
                perror("recv failed");
                return -1;
            }
            if (ack == packet->seq_num) {
                return 0; // ACK received successfully
            }
            printf("Received wrong ACK, retrying packet %d (attempt %d)\n", packet->seq_num, retries + 1);
            retries++;
        }
    }
    printf("Max retries reached for packet %d, giving up\n", packet->seq_num);
    return -1;
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

        if (send_packet(sock, &packet, &server_addr) == 0) {
            printf("Sent packet %d (%zu bytes)\n", packet.seq_num, bytes_read);
        } else {
            printf("Failed to send packet %d after max retries\n", packet.seq_num);
            // You might want to implement additional error handling here
        }
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