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
#include <cmath>

#define DEFAULT_WINDOW_SIZE 300
#define DEFAULT_TIMEOUT 260.0
#define MAX_FILENAME_SIZE 50
#define MAX_PACKET_SIZE 8500

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



//sliding window control
class SlidingWindowClient {
private:
    int sock;
    struct sockaddr_in server_addr;

    const char* fileFragName = nullptr;
    FILE* file = nullptr;

    int window_size = 0;
    WindowSlot* window = nullptr;
    std::atomic<bool> finished{false};
    std::atomic<int> base{0};
    std::atomic<int> next_seq_num{0};

    int timeout = 0;


public:
    SlidingWindowClient(const char* filename, const char* ip, int port, int window_size, int time_out) {
        //create socket
        sock = create_socket();
        if (sock < 0) {
            perror("Failed to create socket");
            return;
        }
        
        memset(&server_addr, 0, sizeof(server_addr));
        server_addr.sin_family = AF_INET;
        server_addr.sin_port = htons(port);
        inet_pton(AF_INET, ip, &server_addr.sin_addr);

        //store the filename and open the file
        fileFragName = basename((char*)filename);
        if (fileFragName == NULL) {
            perror("Failed to get base filename");
            return;
        }

        file = fopen(fileFragName, "rb");
        if (file == NULL) {
            printf("Failed to open file '%s': %s\n", fileFragName, strerror(errno));
            exit(EXIT_FAILURE);
        }

        //create the window
        this->window_size = window_size;
        window = (WindowSlot*)malloc(window_size * sizeof(WindowSlot));
        if (window == NULL) {
            perror("Failed to allocate memory for window");
            exit(EXIT_FAILURE);
        }

        //set the timeout
        timeout = time_out;
    }

    ~SlidingWindowClient() {
        close(sock);
        if (file != NULL) {
            fclose(file);
            file = NULL;
        }
        free(window);
    }

    void run() {
        std::thread send_thread(&SlidingWindowClient::send_packets, this);
        std::thread receive_thread(&SlidingWindowClient::receive_acks, this);

        while (!finished) {
            while (next_seq_num < base + window_size && !finished) {
                setPacket(next_seq_num);
                Packet* packet = &window[next_seq_num % window_size].packet;
                send_packet(packet);
                next_seq_num++;
            }
        }

        send_thread.join();
        receive_thread.join();

        printf("Sub File transfer complete\n");
    }

private:
    int create_socket() {
        int sock = socket(AF_INET, SOCK_DGRAM, 0);
        if (sock < 0) {
            perror("socket creation failed");
            return -1;
        }
        return sock;
    }

    void setPacket(int next_seq_num) {
        std::lock_guard<std::mutex> lock(window[next_seq_num % window_size].lock);
        Packet* packet = &window[next_seq_num % window_size].packet;
        packet->seq_num = next_seq_num;
        packet->data_size = fread(packet->data, 1, sizeof(packet->data), file);
        packet->is_last = feof(file);
        strncpy(packet->filename, fileFragName, MAX_FILENAME_SIZE - 1);
        packet->filename[MAX_FILENAME_SIZE - 1] = '\0';
        window[next_seq_num % window_size].acked = 0;
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

        struct timeval tv;
        tv.tv_sec = 0;
        tv.tv_usec = 1000*timeout; // 1ms

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

    void send_packets() {
        while (!finished) {
            for (int i = base; i < next_seq_num; i++) {
                int index = i % window_size;
                std::lock_guard<std::mutex> lock(window[index].lock);
                if (!window[index].acked) {
                    Packet* packet = &window[index].packet;
                    send_packet(packet);
                }
            }
        }
    }

    void receive_acks() {
        while (!finished) {
            int ack = receive_ack();
            int index = ack % window_size;
            printf("Received ACK %d  window[%d, %d] with index %d\n", ack, base.load(), next_seq_num.load(), index);
            
            if (ack >= base) {
                for (int i = base; i <= ack; i++) {
                    std::lock_guard<std::mutex> lock(window[i % window_size].lock);
                    window[i % window_size].acked = 1;
                    printf("Received ACK %d, advancing base\n", base.load());
                    base++;
                }
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
                finished = true;
            } else {
                printf("Received ACK %d outside window [%d, %d], discarding\n", ack, base.load(), next_seq_num.load());
            }
        }
    }
};




class UDPSender {
private:
    const char* ip = nullptr;
    int port = 0;

    int window_size = 0;
    int timeout = 0;

    const char* filename = nullptr;
    int file_separator = 0;
public:
    ~UDPSender() {
    }

    UDPSender(const char* ip, int port, int window_size, float timeout, const char* filename, int file_separator) {

        this->ip = ip;
        this->port = port;
        
        this->window_size = window_size;
        this->timeout = timeout;

        this->filename = basename((char*)filename);
        if (this->filename == NULL) {
            perror("Failed to get base filename");
            return;
        }

        this->file_separator = file_separator;
    }

    std::vector<std::string> seperate() {
        std::vector<std::string> filePart_names = {};

        std::ifstream input(this->filename, std::ios::binary);
        if (!input) {
            std::cerr << "Error opening input file." << std::endl;
            return filePart_names;
        }

        input.seekg(0, std::ios::end);
        std::streamsize file_size = input.tellg();
        input.seekg(0, std::ios::beg);

        std::streamsize part_size = std::ceil(static_cast<double>(file_size) / file_separator);

        std::vector<char> buffer(part_size);

        for (int i = 0; i < file_separator; ++i) {
            std::string output_filename = std::string(this->filename) + ".part" + std::to_string(i + 1);
            std::ofstream output(output_filename, std::ios::binary);
            
            if (!output) {
                std::cerr << "Error creating output file: " << output_filename << std::endl;
                return filePart_names;
            }

            std::streamsize bytes_to_read = (i == file_separator - 1) ? (file_size - i * part_size) : part_size;
            input.read(buffer.data(), bytes_to_read);
            output.write(buffer.data(), bytes_to_read);

            filePart_names.push_back(output_filename);
        }

        std::cout << "File split successfully." << std::endl;
        return filePart_names;
    }


    std::string modifyFilename(const char* filename) {
        size_t lastdot = std::string(filename).find_last_of(".");
        size_t firstdot = std::string(filename).find_first_of(".");

        if (lastdot == std::string::npos) {
            return std::string(filename) + "_copy";
        } else if (firstdot != lastdot) {
            return std::string(filename) + "_copy";
        } else {
            std::string name = std::string(filename).substr(0, lastdot);
            std::string extension = std::string(filename).substr(lastdot);
            return name + "_copy" + extension;
        }
    }



    void combine_files(const char* filename, int file_separator) {
        std::string output_filename = modifyFilename(filename);
        std::ofstream output(output_filename, std::ios::binary);
        if (!output) {
            std::cerr << "Error creating output file." << std::endl;
            return;
        }

        for (int i = 0; i < file_separator; ++i) {
            std::string input_filename = std::string(filename) + ".part" + std::to_string(i + 1);
            std::ifstream input(input_filename, std::ios::binary);
            
            if (!input) {
                std::cerr << "Error opening input file: " << input_filename << std::endl;
                return;
            }

            output << input.rdbuf();
        }

        std::cout << "Files combined successfully." << std::endl;
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

    int file_separator = 8;

    UDPSender sender(ip, port, DEFAULT_WINDOW_SIZE, DEFAULT_TIMEOUT, filename, file_separator);

    auto start = std::chrono::high_resolution_clock::now();
    std::vector<std::string> fileNames = sender.seperate();

    // sender.combine_files(filename, file_separator);
    //create the sliding window for each file part
    std::vector<std::thread> threads;
    for (int i = 0; i < fileNames.size(); i++) {
        threads.push_back(std::thread([ip, port, &fileNames, i]() {
            SlidingWindowClient client(fileNames[i].c_str(), ip, port, DEFAULT_WINDOW_SIZE, DEFAULT_TIMEOUT);
            client.run();
        }));
    }

    // Join all threads
    for (std::thread& thread : threads) {
        if (thread.joinable()) {
            thread.join();
        }
    }

    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = end - start;
    std::cout << "Elapsed time: " << elapsed.count() << "s" << std::endl;

    return 0;
}