#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <thread>
#include <mutex>
#include <atomic>
#include <iostream>

#define MAX_PACKET_SIZE 8500
#define MAX_FILENAME_SIZE 50
#define WINDOW_SIZE 300
#define BUFFER_ZONE 5

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

int sock;
struct sockaddr_in client_addr;


WindowSlot window[WINDOW_SIZE];
std::atomic<int> base{0};
std::atomic<bool> file_finished{false};

char current_filename[MAX_FILENAME_SIZE] = {0};


void receive_thread(){
    Packet packet;
    FILE* file = NULL;

    while (!file_finished)
    {
        socklen_t client_len = sizeof(client_addr);
        ssize_t received = recvfrom(sock, &packet, sizeof(Packet), 0, (struct sockaddr*)&client_addr, &client_len);

        if (received < 0) {
            perror("recvfrom failed");
            continue;
        }

        printf("Received packet %d for file %s (%d bytes)\n", packet.seq_num, packet.filename, packet.data_size);

        if (packet.seq_num >= base && packet.seq_num < base + WINDOW_SIZE + BUFFER_ZONE) {
            int index = packet.seq_num % WINDOW_SIZE;
            window[index].packet = packet;
            window[index].received = 1;

            printf("Accepted packet %d (base: %d, window: [%d, %d])\n", 
                   packet.seq_num, base.load(), base.load(), base.load() + WINDOW_SIZE - 1);

            // Process packets in order, stopping at the first gap
            while (window[base % WINDOW_SIZE].received) {
                Packet* current_packet = &window[base % WINDOW_SIZE].packet;

                if (strcmp(current_filename, current_packet->filename) != 0) {
                    if (file != NULL) {
                        fclose(file);
                    }
                    strncpy(current_filename, current_packet->filename, MAX_FILENAME_SIZE);
                    file = fopen(current_filename, "wb");
                    if (file == NULL) {
                        perror("Failed to create file");
                        exit(EXIT_FAILURE);
                    }
                    printf("Creating new file: %s\n", current_filename);
                }

                fwrite(current_packet->data, 1, current_packet->data_size, file);
                
                if (current_packet->is_last == 1) {
                    printf("File transfer complete: %s\n", current_filename);
                    fclose(file);
                    file = NULL;
                    memset(current_filename, 0, MAX_FILENAME_SIZE);

                    // Send ACK for last packet
                    send_ack(sock, -2, &client_addr);

                    file_finished = true;
                } 

                printf("Processing packet %d, advancing base\n", base.load());
                window[base % WINDOW_SIZE].received = 0;
                base++;

                send_ack(sock, base - 1, &client_addr);
            }
        } else {
            printf("Packet %d outside window [%d, %d], discarding\n", 
                   packet.seq_num, base.load(), base.load() + WINDOW_SIZE + BUFFER_ZONE - 1);
        }
    }
}
void send_thread(){
    while (!file_finished){
        if(client_addr.sin_addr.s_addr == 0){
            continue;
        }
        // Send ACK for last packet
        if(base > 0){
            send_ack(sock, base-1, &client_addr);
        } 
        //sleep 1ms
        usleep(1000);
    }
}

void run_server(const char* ip, int port) {
    sock = create_socket(ip, port); 
    
    // Initialize window
    for (int i = 0; i < WINDOW_SIZE; i++) {
        window[i].received = 0;
    }
    printf("Server listening on %s:%d\n", ip, port);


    std::thread send_thread1(receive_thread);
    std::thread ack_thread(send_thread);

    send_thread1.join();
    ack_thread.join();

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