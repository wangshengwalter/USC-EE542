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
#include <thread>
#include <mutex>

#define MAX_PACKET_SIZE 1500
#define MAX_FILENAME_SIZE 256
#define DEFAULT_WINDOW_SIZE 20
#define DEFAULT_TIMEOUT 40.0

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
    std::mutex lock;
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

int receive_ack(int sock, struct timeval* timeout) {
    fd_set readfds;
    FD_ZERO(&readfds);
    FD_SET(sock, &readfds);

    int activity = select(sock + 1, &readfds, NULL, NULL, timeout);
    if (activity < 0) {
        perror("select error");
        exit(EXIT_FAILURE);
    } else if (activity == 0) {
        return -1;  // Timeout
    } else {
        int ack;
        if (recv(sock, &ack, sizeof(ack), 0) < 0) {
            perror("recv failed");
            exit(EXIT_FAILURE);
        }
        return ack;
    }
}

int window_size = 0;

WindowSlot* window = nullptr;

int base = 0;
std::mutex base_lock;

int next_seq_num = 0;
std::mutex nextseq_lock;

int file_finished = 0;
std::mutex end_lock;

void base_increment() {
    std::lock_guard<std::mutex> lock(base_lock);
    base++;
}

void nextseq_increment() {
    std::lock_guard<std::mutex> lock(nextseq_lock);
    next_seq_num++;
}

void send_thread(const char* filename, int sock, struct sockaddr_in* server_addr) {
    FILE* file = fopen(filename, "rb");
    if (file == NULL) {
        printf("Failed to open file '%s': %s\n", filename, strerror(errno));
        exit(EXIT_FAILURE);
    }
    printf("Successfully opened file: %s\n", filename);

    while (base < next_seq_num || !file_finished) {
        while (next_seq_num < base + window_size && !file_finished) {
            std::lock_guard<std::mutex> lock(window[next_seq_num % window_size].lock);

            Packet* packet = &window[next_seq_num % window_size].packet;
            packet->seq_num = next_seq_num;
            packet->data_size = fread(packet->data, 1, sizeof(packet->data), file);
            packet->is_last = feof(file);
            strncpy(packet->filename, basename((char*)filename), MAX_FILENAME_SIZE - 1);
            packet->filename[MAX_FILENAME_SIZE - 1] = '\0';

            send_packet(sock, packet, server_addr);
            gettimeofday(&window[next_seq_num % window_size].send_time, NULL);
            window[next_seq_num % window_size].acked = 0;

            nextseq_increment();
            if (packet->is_last) {
                std::lock_guard<std::mutex> lock(end_lock);
                file_finished = 1;
                break;
            }
        }
    }
}


void receive_thread(int sock, float timeout, struct sockaddr_in* server_addr) {
    while (base < next_seq_num || !file_finished) {

        struct timeval tv;
        tv.tv_sec = 0;
        tv.tv_usec = timeout * 1000; // Convert ms to μs

        int ack = receive_ack(sock, &tv);
        if (ack >= base && ack < next_seq_num) {
            printf("Received ACK %d\n", ack);
            int index = ack % window_size;
            std::lock_guard<std::mutex> lock(window[index].lock);
            window[index].acked = 1;
            while (base < next_seq_num && window[base % window_size].acked) {
                printf("Received ACK %d, advancing base\n", base);
                base_increment();
            }
        }
        else if (ack == -1) {
            printf("Timeout occurred. Resending unacked packets...\n");
            for (int i = base; i < next_seq_num; i++) {
                int index = i % window_size;
                std::lock_guard<std::mutex> lock(window[index].lock);
                if (!window[index].acked) {
                    Packet* packet = &window[index].packet;
                    send_packet(sock, packet, server_addr);
                    gettimeofday(&window[index].send_time, NULL);
                }
            }
        } else if (ack == -2) {
            printf("Completed file transfer\n");
            break;
        } else {
            printf("Received ACK %d outside window [%d, %d], discarding\n", ack, base, next_seq_num);
        }
    }
}





void send_file(const char* filename, const char* server_ip, int server_port, float timeout) {
    
    int sock = create_socket();
    if (sock < 0) {
        perror("Failed to create socket");
        return;
    }

    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr(server_ip);
    server_addr.sin_port = htons(server_port);

    window = (WindowSlot*)malloc(window_size * sizeof(WindowSlot));
    if (window == NULL) {
        perror("Failed to allocate memory for window");
        return;
    }

    char* base_filename = basename((char*)filename);
    if (base_filename == NULL)
    {
        perror("Failed to get base filename");
        free(window);
        return;
    }
    
    
    printf("creating threads\n");
    std::thread sendthread(send_thread, base_filename, sock, &server_addr);
    printf("created send thread\n");
    std::thread recvthread(receive_thread, sock, timeout, &server_addr);
    printf("threads started\n");

    sendthread.join();
    recvthread.join();
    printf("threads complete\n");
}


int main(int argc, char* argv[]) {
    if (argc != 4 && argc != 6) {
        fprintf(stderr, "Usage: %s <filename> <server_ip> <server_port> [window_size] [timeout]\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    const char* filename = argv[1];
    const char* server_ip = argv[2];
    int server_port = atoi(argv[3]);
    window_size = DEFAULT_WINDOW_SIZE;
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

    auto start = std::chrono::high_resolution_clock::now();
    send_file(filename, server_ip, server_port, timeout);
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = end - start;
    std::cout << "Elapsed time: " << elapsed.count() << " s\n";

    return 0;
}