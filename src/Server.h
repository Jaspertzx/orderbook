#pragma once
#include "OrderBook.h"
#include <atomic>
#include <mutex>
#include <string>

class Server {
public:
    explicit Server(int port);
    void run();   // blocks until stop() is called
    void stop();

private:
    void handleClient(int clientFd);
    std::string handleCommand(const std::string& line);

    int port_;
    std::atomic<bool> running_{false};
    OrderBook book_;
    std::mutex bookMutex_;
};
