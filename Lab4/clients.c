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




class UDPSender {
private:
    int sock;
    struct sockaddr_in server_addr;

    int window_size = 0;

    int timeout = 0;

    const char* filename = nullptr;
    int file_separator = 0;


    int create_socket() {
        int sock = socket(AF_INET, SOCK_DGRAM, 0);
        if (sock < 0) {
            perror("socket creation failed");
            return -1;
        }

        return sock;
    }

public:
    ~UDPSender() {
        close(sock);
        if (filename != nullptr) {
            free((void*)filename);
        }
    }

    UDPSender(const char* ip, int port, int window_size, float timeout, const char* filename, int file_separator) {
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

        this->filename = basename((char*)filename);
        if (this->filename == NULL) {
            perror("Failed to get base filename");
            return;
        }

        this->file_separator = file_separator;
    }

    void sendFile() {

        std::ifstream input(this->filename, std::ios::binary);
        if (!input) {
            std::cerr << "Error opening input file." << std::endl;
            return;
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
                return;
            }

            std::streamsize bytes_to_read = (i == file_separator - 1) ? (file_size - i * part_size) : part_size;
            input.read(buffer.data(), bytes_to_read);
            output.write(buffer.data(), bytes_to_read);
        }

        std::cout << "File split successfully." << std::endl;
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
    sender.sendFile();

    sender.combine_files(filename, file_separator);
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = end - start;
    std::cout << "Elapsed time: " << elapsed.count() << "s" << std::endl;

    return 0;
}