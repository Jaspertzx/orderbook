#include "Server.h"

#include <csignal>
#include <cstdlib>
#include <iostream>

static Server* gServer = nullptr;

static void onSignal(int) {
    if (gServer) gServer->stop();
}

int main(int argc, char** argv) {
    int port = (argc > 1) ? std::atoi(argv[1]) : 9000;

    Server server(port);
    gServer = &server;
    std::signal(SIGINT, onSignal);   // Ctrl+C
    std::signal(SIGTERM, onSignal);  // kill

    try {
        server.run();
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << "\n";
        return 1;
    }
    return 0;
}
