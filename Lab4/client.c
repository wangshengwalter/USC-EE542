#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <libgen.h>
#include <errno.h>
#include <iostream>
#include <chrono>
#include <math.h>

#define MAX_PACKET_SIZE 8500
#define MAX_FILENAME_SIZE 50
#define DEFAULT_WINDOW_SIZE 25
#define DEFAULT_TIMEOUT 300.0

#define ALPHA 0.125
#define BETA 0.25
#define K 4

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
    printf("Sent packet %d (%d bytes)\n", packet->seq_num, packet->data_size);
}

double estimated_rtt = 1.0;
double dev_rtt = 0.0;

double calculate_timeout(struct timeval* send_time, struct timeval* ack_time) {
    struct timeval diff;
    timersub(ack_time, send_time, &diff);
    double sample_rtt = diff.tv_sec + diff.tv_usec / 1000000.0;

    estimated_rtt = (1 - ALPHA) * estimated_rtt + ALPHA * sample_rtt;
    dev_rtt = (1 - BETA) * dev_rtt + BETA * fabs(sample_rtt - estimated_rtt);

    return estimated_rtt + K * dev_rtt;
}

void update_window(WindowSlot* window, int window_size, int ack, int* base, int next_seq_num) {
    for (int i = *base; i <= ack; i++) {
        window[i % window_size].acked = 1;
    }
    // Move the base of the window
    while (*base < next_seq_num && window[*base % window_size].acked) {
        (*base)++;
    }
    printf("Updated window base to: %d\n", *base);
}

void receive_ack(int sock, int* ack, struct timeval* timeout, WindowSlot* window, int window_size, int* base, int next_seq_num) {
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
        // Update the window based on the received ACK
        update_window(window, window_size, *ack, base, next_seq_num);
    }
}

void send_file(const char* filename, const char* server_ip, int server_port, int window_size, float initial_timeout) {
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
    double current_timeout = initial_timeout;

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

            next_seq_num