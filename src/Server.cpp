#include "Server.h"

#include <arpa/inet.h>
#include <netinet/in.h>
#include <poll.h>
#include <sys/socket.h>
#include <unistd.h>

#include <iostream>
#include <sstream>
#include <stdexcept>
#include <thread>

Server::Server(int port) : port_(port) {}

void Server::stop() { running_ = false; }

void Server::run() {
    int listenFd = socket(AF_INET, SOCK_STREAM, 0);
    if (listenFd < 0) throw std::runtime_error("socket() failed");

    int opt = 1;
    setsockopt(listenFd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    addr.sin_port = htons(port_);

    if (bind(listenFd, reinterpret_cast<sockaddr*>(&addr), sizeof(addr)) < 0)
        throw std::runtime_error("bind() failed");
    if (listen(listenFd, 16) < 0)
        throw std::runtime_error("listen() failed");

    std::cout << "Listening on port " << port_ << " (Ctrl+C to stop)\n";
    running_ = true;

    while (running_) {
        // Wake up every 500ms so we notice stop() requests.
        pollfd pfd{listenFd, POLLIN, 0};
        if (poll(&pfd, 1, 500) <= 0) continue;

        int clientFd = accept(listenFd, nullptr, nullptr);
        if (clientFd < 0) continue;

        std::thread(&Server::handleClient, this, clientFd).detach();
    }

    close(listenFd);
    std::cout << "Server stopped\n";
}

void Server::handleClient(int clientFd) {
    std::cout << "Client connected\n";
    std::string buffer;
    char chunk[1024];

    while (running_) {
        ssize_t n = recv(clientFd, chunk, sizeof(chunk), 0);
        if (n <= 0) break;  // disconnected or error
        buffer.append(chunk, n);

        // Process each complete line.
        size_t pos;
        while ((pos = buffer.find('\n')) != std::string::npos) {
            std::string line = buffer.substr(0, pos);
            buffer.erase(0, pos + 1);
            if (!line.empty() && line.back() == '\r') line.pop_back();

            std::string reply = handleCommand(line);
            send(clientFd, reply.data(), reply.size(), 0);
        }
    }

    close(clientFd);
    std::cout << "Client disconnected\n";
}

// Protocol (one command per line):
//   BUY <qty> <price>
//   SELL <qty> <price>
//   CANCEL <id>
//   BOOK
std::string Server::handleCommand(const std::string& line) {
    std::istringstream in(line);
    std::string cmd;
    in >> cmd;

    std::lock_guard<std::mutex> lock(bookMutex_);

    if (cmd == "BUY" || cmd == "SELL") {
        uint32_t qty;
        double price;
        if (!(in >> qty >> price)) return "ERR usage: " + cmd + " <qty> <price>\n";
        Side side = (cmd == "BUY") ? Side::Buy : Side::Sell;
        uint64_t id = book_.addOrder(side, price, qty);
        return "OK " + std::to_string(id) + "\n";
    }
    if (cmd == "CANCEL") {
        uint64_t id;
        if (!(in >> id)) return "ERR usage: CANCEL <id>\n";
        return book_.cancelOrder(id) ? "OK\n" : "ERR not found\n";
    }
    if (cmd == "BOOK") {
        return book_.toString();
    }
    return "ERR unknown command\n";
}
