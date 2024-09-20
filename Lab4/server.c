#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/time.h>

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

typedef struct {
    struct timeval start_time;
    struct timeval end_time;
    size_t total_bytes;
    char filename[MAX_FILENAME_SIZE];
} TransferStats;

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

void calculate_bandwidth(TransferStats* stats) {
    double duration = (stats->end_time.tv_sec - stats->start_time.tv_sec) +
                      (stats->end_time.tv_usec - stats->start_time.tv_usec) / 1000000.0;
    double bandwidth = (stats->total_bytes * 8) / (duration * 1000000); // Mbps

    printf("Transfer completed for file: %s\n", stats->filename);
    printf("Transfer completed in %.2f seconds\n", duration);
    printf("Total bytes transferred: %zu\n", stats->total_bytes);
    printf("Bandwidth: %.2f Mbps\n", bandwidth);
}

void run_server(const char* ip, int port) {
    int sock = create_socket(ip, port);
    struct sockaddr_in client_addr;
    Packet packet;
    FILE* file = NULL;
    int seq_num = 0;
    TransferStats stats = {0};
    char current_filename[MAX_FILENAME_SIZE] = {0};

    printf("Server listening on %s:%d\n", ip, port);

    while (1) {
        if (receive_packet(sock, &packet, &client_addr) == 0) {
            if (seq_num == packet.seq_num) {
                if (file == NULL || strcmp(current_filename, packet.filename) != 0) {
                    if (file != NULL) {
                        fclose(file);
                        gettimeofday(&stats.end_time, NULL);
                        calculate_bandwidth(&stats);
                    }
                    
                    strncpy(current_filename, packet.filename, MAX_FILENAME_SIZE);
                    file = fopen(current_filename, "wb");
                    if (file == NULL) {
                        perror("Failed to create file");
                        continue;
                    }
                    printf("Creating new file: %s\n", current_filename);
                    gettimeofday(&stats.start_time, NULL);
                    stats.total_bytes = 0;
                    strncpy(stats.filename, current_filename, MAX_FILENAME_SIZE);
                    seq_num = 0;
                }

                fwrite(packet.data, 1, packet.data_size, file);
                stats.total_bytes += packet.data_size;
                printf("Received packet %d for file %s (%d bytes)\n", seq_num, current_filename, packet.data_size);
                seq_num++;

                if (packet.is_last) {
                    gettimeofday(&stats.end_time, NULL);
                    printf("File transfer complete: %s\n", current_filename);
                    fclose(file);
                    file = NULL;
                    calculate_bandwidth(&stats);
                    memset(current_filename, 0, MAX_FILENAME_SIZE);
                    seq_num = 0;
                }
            } else {
                printf("Received out-of-order packet for %s. Expected %d, got %d\n", 
                       current_filename, seq_num, packet.seq_num);
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