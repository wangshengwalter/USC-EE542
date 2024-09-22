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
#define DEFAULT_WINDOW_SIZE 300
#define DEFAULT_TIMEOUT 260.0


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
    std::chrono::high_resolution_clock::time_point send_time;
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


    int timeout = 0;
    struct timeval tv;
    

    int create_socket() {
        int sock = socket(AF_INET, SOCK_DGRAM, 0);
        if (sock < 0) {
            perror("socket creation failed");
            exit(EXIT_FAILURE);
        }
        return sock;
    }


    void set_timeout(int new_timeout) {
        timeout = new_timeout;
        tv.tv_sec = 0;
        tv.tv_usec = new_timeout * 1000; // Convert ms to μs
    }



    void send_packet(const Packet* packet) {
        if (packet == nullptr){
            return;
        }

        
        if (sendto(sock, packet, sizeof(Packet), 0, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
            perror("sendto failed");
            exit(EXIT_FAILURE);
        }

        printf("Sent packet %d (%d bytes)\n", packet->seq_num, packet->data_size);
    }

    int receive_ack() {
        fd_set readfds;
        FD_ZERO(&readfds);
        FD_SET(sock, &readfds);

        printf("tv.tv_usec: %ld\n", &tv.tv_usec);


        int activity = select(sock + 1, &readfds, NULL, NULL, &tv);
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

    //send and receive threads
    void send_thread() {
        FILE* file = fopen(base_filename, "rb");
        if (file == NULL) {
            printf("Failed to open file '%s': %s\n", base_filename, strerror(errno));
            exit(EXIT_FAILURE);
        }
        printf("Successfully opened file: %s\n", base_filename);

        while (!file_finished) {
            while (next_seq_num < base + window_size && !file_finished) {
                std::lock_guard<std::mutex> lock(window[next_seq_num % window_size].lock);

                Packet* packet = &window[next_seq_num % window_size].packet;
                packet->seq_num = next_seq_num;
                packet->data_size = fread(packet->data, 1, sizeof(packet->data), file);
                packet->is_last = feof(file);
                strncpy(packet->filename, basename((char*)base_filename), MAX_FILENAME_SIZE - 1);
                packet->filename[MAX_FILENAME_SIZE - 1] = '\0';

                send_packet(packet);
                window[next_seq_num % window_size].acked = 0;
                window[next_seq_num % window_size].send_time = std::chrono::system_clock::now();

                next_seq_num++;
                if (packet->is_last) {
                    file_finished = true;
                    break;
                }
            }
            std::this_thread::sleep_for(std::chrono::microseconds(1)); //let reveive thread run
        }
    }


    void receive_thread() {
        while (base < next_seq_num || !file_finished) {

            int ack = receive_ack();
            if (ack >= base) {
                int index = ack % window_size;
                printf("Received ACK %d  window[%d, %d] with index %d\n", ack, base.load(), next_seq_num.load(), index);
                
                for (int i = base; i <= ack; i++) {
                    std::lock_guard<std::mutex> lock(window[i % window_size].lock);
                    window[i % window_size].acked = 1;

                    auto now = std::chrono::system_clock::now();
                    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(now - window[i % window_size].send_time);
                    //change timeout
                    timeout = (timeout + duration.count()) / 2;
                    printf("Currently Timeout: %d\n", timeout);

                    printf("Received ACK %d, advancing base\n", base.load());
                    base++;
                }
                
                // while (base < next_seq_num && window[base % window_size].acked) {
                //     printf("Received ACK %d, advancing base\n", base.load());
                //     base++;
                // }
            }
            else if (ack == -1) {
                printf("Timeout occurred. Resending unacked packets...\n");
                for (int i = base; i < next_seq_num; i++) {
                    int index = i % window_size;
                    std::lock_guard<std::mutex> lock(window[index].lock);
                    if (!window[index].acked) {
                        Packet* packet = &window[index].packet;
                        send_packet(packet);
                    }
                }
            } else if (ack == -2) {
                printf("Completed file transfer\n");
                break;
            } else {
                printf("Received ACK %d outside window [%d, %d], discarding\n", ack, base.load(), next_seq_num.load());
            }
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
        close(sock);
    }

    void sendFile(const char* filename) {
        window = (WindowSlot*)malloc(window_size * sizeof(WindowSlot));
        if (window == NULL) {
            perror("Failed to allocate memory for window");
            return;
        }

        base_filename = basename((char*)filename);
        if (base_filename == NULL) {
            perror("Failed to get base filename");
            free(window);
            return;
        }

        std::thread send_thread1(&UDPSender::send_thread, this);
        // std::thread send_thread2(&UDPSender::send_thread, this);
        // std::thread send_thread3(&UDPSender::send_thread, this);
        printf("send_thread\n");
        std::thread ack_thread(&UDPSender::receive_thread, this);
        printf("ack_thread\n");

        send_thread1.join();
        // send_thread2.join();
        // send_thread3.join();
        ack_thread.join();

        printf("File transfer complete\n");
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

    auto start = std::chrono::high_resolution_clock::now();
    sender.sendFile(filename);
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = end - start;
    std::cout << "Elapsed time: " << elapsed.count() << "s" << std::endl;

    return 0;
}