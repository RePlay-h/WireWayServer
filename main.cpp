#include "network/server.hpp"

#include <boost/asio/signal_set.hpp>

#include <thread>

int main() {
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