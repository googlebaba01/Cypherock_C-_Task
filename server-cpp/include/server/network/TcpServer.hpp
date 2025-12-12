#pragma once

#include <string>
#include "asio.hpp"

namespace server::network {

class TcpServer {
public:
    TcpServer(asio::io_context& io_context, unsigned short port);

    // Blocks and handles one client at a time in a loop.
    void run_blocking();

private:
    asio::io_context& io_context_;
    asio::ip::tcp::acceptor acceptor_;
};

} // namespace server::network
