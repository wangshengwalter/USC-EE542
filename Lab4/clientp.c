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

#define MAX_PACKET_SIZE 8500
#define MAX_FILENAME_SIZE 50


const int WINDOW_SIZE = 5;

struct Packet {
    int sequence_number;
    int is_last;
    int size;
    char filename[MAX_FILENAME_SIZE];
    char data[MAX_PACKET_SIZE - 3 * sizeof(int) - MAX_FILENAME_SIZE];
};

class SlidingWindow {
private:
    std::vector<Packet> window;
    std::mutex mtx;
    std::condition_variable cv;
    int base;
    int next_sequence;
    bool finished;

public:
    SlidingWindow() : base(0), next_sequence(0), finished(false) {}

    void addPacket(const Packet& packet) {
        std::unique_lock<std::mutex> lock(mtx);
        cv.wait(lock, [this] { return window.size() < WINDOW_SIZE; });
        window.push_back(packet);
        cv.notify_all();
    }

    Packet getPacket() {
        std::unique_lock<std::mutex> lock(mtx);
        cv.wait(lock, [this] { return !window.empty(); });
        Packet packet = window.front();
        cv.notify_all();
        return packet;
    }

    void ackPacket(int ack) {
        std::unique_lock<std::mutex> lock(mtx);
        while (!window.empty() && window.front().sequence_number <= ack) {
            window.erase(window.begin());
            base++;
        }
        cv.notify_all();
    }

    bool isFinished() const {
        return finished;
    }

    void setFinished() {
        finished = true;
        cv.notify_all();
    }

    int getNextSequence() {
        return next_sequence++;
    }
};

class UDPSender {
private:
    int sockfd;
    struct sockaddr_in server_addr;
    SlidingWindow window;

public:
    UDPSender(const char* ip, int port) {
        sockfd = socket(AF_INET, SOCK_DGRAM, 0);
        if (sockfd < 0) {
            std::cerr << "Error creating socket" << std::endl;
            exit(1);
        }

        memset(&server_addr, 0, sizeof(server_addr));
        server_addr.sin_family = AF_INET;
        server_addr.sin_port = htons(port);
        inet_pton(AF_INET, ip, &server_addr.sin_addr);
    }

    ~UDPSender() {
        close(sockfd);
    }

    void sendFile(const std::string& filename) {
        std::ifstream file(filename, std::ios::binary);
        if (!file) {
            std::cerr << "Error opening file" << std::endl;
            return;
        }

        std::thread send_thread(&UDPSender::sendThread, this);
        printf("send_thread\n");
        std::thread ack_thread(&UDPSender::ackThread, this);
        printf("ack_thread\n");

        Packet packet;

        while(file) {
            file.read(packet.data, sizeof(packet.data));
            packet.size = file.gcount();
            packet.sequence_number = window.getNextSequence();
            packet.is_last = file.eof();
            strncpy(packet.filename, filename.c_str(), MAX_FILENAME_SIZE);
            window.addPacket(packet);

            if(file.eof()) {
                break;
            }
        }

        window.setFinished();
        send_thread.join();
        ack_thread.join();
    }

private:
    void sendThread() {
        while (!window.isFinished() || window.getPacket().size > 0) {
            Packet packet = window.getPacket();
            sendto(sockfd, &packet, sizeof(Packet), 0, (struct sockaddr*)&server_addr, sizeof(server_addr));
            printf("Sent packet %d\n", packet.sequence_number);
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
    }

    void ackThread() {
        char buffer[sizeof(int)];
        struct sockaddr_in client_addr;
        socklen_t client_len = sizeof(client_addr);

        while (!window.isFinished()) {
            ssize_t received = recvfrom(sockfd, buffer, sizeof(int), 0, (struct sockaddr*)&client_addr, &client_len);
            if (received == sizeof(int)) {
                int ack;
                memcpy(&ack, buffer, sizeof(int));
                printf("Received ACK %d\n", ack);
                window.ackPacket(ack);
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

    UDPSender sender(ip, port);
    sender.sendFile(filename);

    return 0;
}