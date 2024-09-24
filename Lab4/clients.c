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




class UDPSender {
private:
    int sock;
    struct sockaddr_in server_addr;

    int window_size = 0;

    int timeout = 0;

    const char* filename = nullptr;


    int create_socket() {
        int sock = socket(AF_INET, SOCK_DGRAM, 0);
        if (sock < 0) {
            perror("socket creation failed");
            return -1;
        }

        return sock;
    }

public:
    UDPSender(const char* ip, int port, int window_size, float timeout, const char* filename) {
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

        this->timeout = timeout;

        this->filename = filename;
    }

    ~UDPSender() {
        close(sock);
    }

    void sendFile() {

        //seperate the file into smaller parts
        int num_subfiles = 8;

        std::ifstream input(this->filename, std::ios::binary);
        if (!input) {
            std::cerr << "Error opening input file." << std::endl;
            return;
        }

        input.seekg(0, std::ios::end);
        std::streamsize file_size = input.tellg();
        input.seekg(0, std::ios::beg);

        std::streamsize part_size = std::ceil(static_cast<double>(file_size) / NUM_PARTS);

        std::vector<char> buffer(part_size);

        for (int i = 0; i < NUM_PARTS; ++i) {
            std::string output_filename = this->filename + ".part" + std::to_string(i + 1);
            std::ofstream output(output_filename, std::ios::binary);
            
            if (!output) {
                std::cerr << "Error creating output file: " << output_filename << std::endl;
                return;
            }

            std::streamsize bytes_to_read = (i == NUM_PARTS - 1) ? (file_size - i * part_size) : part_size;
            input.read(buffer.data(), bytes_to_read);
            output.write(buffer.data(), bytes_to_read);
        }

        std::cout << "File split successfully." << std::endl;
    }



    void combine_files() {
        std::ofstream output(this->filename, std::ios::binary);
        if (!output) {
            std::cerr << "Error creating output file." << std::endl;
            return;
        }

        for (int i = 0; i < NUM_PARTS; ++i) {
            std::string input_filename = this->filename + ".part" + std::to_string(i + 1);
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

    UDPSender sender(ip, port, DEFAULT_WINDOW_SIZE, DEFAULT_TIMEOUT, filename);

    auto start = std::chrono::high_resolution_clock::now();
    sender.sendFile();

    sender.combine_files();
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = end - start;
    std::cout << "Elapsed time: " << elapsed.count() << "s" << std::endl;

    return 0;
}