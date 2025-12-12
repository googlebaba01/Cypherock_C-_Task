#include <iostream>
#include <iomanip>
#include "asio.hpp"
#include "server/network/TcpServer.hpp"
#include "server/crypto/CryptoUtils.hpp"

using namespace server::crypto;

// Print a 32-byte scalar as hex
static void print_hex(const Scalar32& s) {
    for (uint8_t b : s) {
        std::cout << std::hex
                  << std::setw(2)
                  << std::setfill('0')
                  << (int)b;
    }
    std::cout << std::dec << "\n";
}

int main() {
    try {
        // =====================================================
        // Scalar sanity test
        // =====================================================
        std::cout << "=== Scalar Test (C++ server) ===\n";

        Scalar32 a = CryptoUtils::random_scalar();
        Scalar32 b = CryptoUtils::random_scalar();

        Scalar32 sum = CryptoUtils::add_mod_n(a, b);
        Scalar32 mul = CryptoUtils::mul_mod_n(a, b);

        std::cout << "a      = "; print_hex(a);
        std::cout << "b      = "; print_hex(b);
        std::cout << "a + b  = "; print_hex(sum);
        std::cout << "a * b  = "; print_hex(mul);

        std::cout << "=== Scalar Test Completed ===\n\n";

        // =====================================================
        // Start server
        // =====================================================
        asio::io_context io_context;

        const unsigned short port = 9000;
        server::network::TcpServer server(io_context, port);

        std::cout << "Cypherock C++ server listening on port " << port << std::endl;
        server.run_blocking();  // never returns

    } catch (const std::exception& ex) {
        std::cerr << "Server exception: " << ex.what() << std::endl;
        return 1;
    }

    return 0;
}
