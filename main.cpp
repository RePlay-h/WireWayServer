#include "network/server.hpp"

#include <boost/asio/signal_set.hpp>

#include <thread>

int main() {

    std::cout <<"                  _    _  _              _    _                 _____                                \n \
                | |  | |(_)            | |  | |               /  ___|                               \n \
                | |  | | _  _ __   ___ | |  | |  __ _  _   _  \\ `--.   ___  _ __ __   __  ___  _ __ \n \
                | |/\\| || || '__| / _ \\| |/\\| | / _` || | | |  `--. \\ / _ \\| '__|\\ \\ / / / _ \\| '__|\n \
                \\  /\\  /| || |   |  __/\\  /\\  /| (_| || |_| | /\\__/ /|  __/| |    \\ V / |  __/| |   \\ \n \
                 \\/  \\/ |_||_|    \\___| \\/  \\/  \\__,_| \\__, | \\____/  \\___||_|     \\_/   \\___||_|   \n \
                                                       __/ |                                                        \
                                                                                     |___/ \n";                                    


    asio::io_context io;

    tcp::acceptor acc{io, tcp::endpoint{tcp::v4(), 8888}};

    asio::io_context::work work{io};


    asio::signal_set signals(io, SIGTERM, SIGINT);

    asio::co_spawn(io, network::Listener(std::move(acc)), asio::detached);

    signals.async_wait([&](auto, auto){
        Log::Print(INFO, "Server is down", std::cout);

        io.stop();
    });
    
    io.run();
}