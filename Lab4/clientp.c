#include <iostream>
#include <fstream>
#include <vector>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <cstring>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>
#include <atomic>

#define MAX_PACKET_SIZE 8500
#define MAX_FILENAME_SIZE 50
#define DEFAULT_WINDOW_SIZE 294
#define DEFAULT_TIMEOUT 240.0


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


// class SlidingWindow {
// private:
    

// public:
    
// };



class UDPSender {
private:
    int sock;
    struct sockaddr_in server_addr;

    char* base_filename = nullptr;

    int window_size = 0;
    WindowSlot* window = nullptr;

    std::atomic<bool> file_finished{false};
    std::atomic<int> base{0};
    std::atomic<int> next_seq_num{0};


    struct timeval tv;

    int create_socket() {
        int sock = socket(AF_INET, SOCK_DGRAM, 0);
        if (sock < 0) {
            perror("socket creation failed");
            exit(EXIT_FAILURE);
        }
        return sock;
    }

    void set_timeout(float timeout) {
        tv.tv_sec = 0;
        tv.tv_usec = timeout * 1000; // Convert ms to μs
    }


    void send_packet(const Packet* packet) {
        if (sendto(sock, packet, sizeof(Packet), 0, (struct sockaddr*)server_addr, sizeof(*server_addr)) < 0) {
            perror("sendto failed");
            exit(EXIT_FAILURE);
        }
        printf("Sent packet %d (%d bytes)\n", packet->seq_num, packet->data_size);
    }

    int receive_ack() {
        fd_set readfds;
        FD_ZERO(&readfds);
        FD_SET(sock, &readfds);

        int activity = select(sock + 1, &readfds, NULL, NULL, tv);
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

public:
    UDPSender(const char* ip, int port, int window_size, float timeout) {
        sock = create_socket();
        if (sock < 0) {
            perror("Failed to create socket");
            return;
        }
        
        memset(&server_addr, 0, sizeof(server_addr));
        server_addr.sin_family = AF_INET;
        server_addr.sin_port = htons(port);
        inet_pton(AF_INET, ip, &server_addr.sin_addr);

        this->window_size = window_size;
        set_timeout(timeout);
    }

    ~UDPSender() {
        close(sockfd);
    }

    void sendFile(const std::string& filename) {

        window = (WindowSlot*)malloc(sliding_window.window_size * sizeof(WindowSlot));
        if (sliding_window.window == NULL) {
            perror("Failed to allocate memory for window");
            return;
        }

        base_filename = basename((char*)filename);
        if (sliding_window.base_filename == NULL) {
            perror("Failed to get base filename");
            free(window);
            return;
        }


        std::thread send_thread(&UDPSender::sendThread, this);
        printf("send_thread\n");
        std::thread ack_thread(&UDPSender::ackThread, this);
        printf("ack_thread\n");

        send_thread.join();
        ack_thread.join();

        printf("File transfer complete\n");
    }

private:
    void send_thread() {
        FILE* file = fopen(filename, "rb");
        if (file == NULL) {
            printf("Failed to open file '%s': %s\n", filename, strerror(errno));
            exit(EXIT_FAILURE);
        }
        printf("Successfully opened file: %s\n", filename);

        while (!file_finished) {
            while (get_nextseq() < get_base() + window_size && !file_finished) {
                std::lock_guard<std::mutex> lock(window[get_nextseq() % window_size].lock);

                Packet* packet = &window[get_nextseq() % window_size].packet;
                packet->seq_num = get_nextseq();
                packet->data_size = fread(packet->data, 1, sizeof(packet->data), file);
                packet->is_last = feof(file);
                strncpy(packet->filename, basename((char*)filename), MAX_FILENAME_SIZE - 1);
                packet->filename[MAX_FILENAME_SIZE - 1] = '\0';

                send_packet(sock, packet, server_addr);
                gettimeofday(&window[get_nextseq() % window_size].send_time, NULL);
                window[get_nextseq() % window_size].acked = 0;

                nextseq_increment();
                if (packet->is_last) {
                    std::lock_guard<std::mutex> lock(end_lock);
                    file_finished = 1;
                    break;
                }
            }
        }
    }


    void receive_thread() {
        while (get_base() < get_nextseq() || !file_finished) {

            int ack = receive_ack(sock, &tv);
            if (ack >= get_base() && ack < get_nextseq()) {
                int index = ack % window_size;
                printf("Received ACK %d  window[%d, %d] with index %d\n", ack, get_base(), get_nextseq(), index);
                
                for (int i = get_base(); i <= ack; i++) {
                    std::lock_guard<std::mutex> lock(window[i % window_size].lock);
                    window[i % window_size].acked = 1;
                }
                
                while (get_base() < get_nextseq() && window[get_base() % window_size].acked) {
                    printf("Received ACK %d, advancing base\n", get_base());
                    base_increment();
                }
            }
            else if (ack == -1) {
                printf("Timeout occurred. Resending unacked packets...\n");
                for (int i = get_base(); i < get_nextseq(); i++) {
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
                printf("Received ACK %d outside window [%d, %d], discarding\n", ack, get_base(), get_nextseq());
            }
        }
    }
};

int main(int argc, char* argv[]) {
    if (argc != 4) {
        std::cerr << "Usage: " << argv[0] << " <IP> <Port> <Filename>" << std::endl;
        return 1;
    }

    const char* ip = argv[1];
    int port = std::stoi(argv[2]);
    const char* filename = argv[3];

    UDPSender sender(ip, port, DEFAULT_WINDOW_SIZE, DEFAULT_TIMEOUT);
    sender.sendFile(filename);

    return 0;
}