#include "server/network/TcpServer.hpp"

#include <iostream>
#include <vector>
#include <cstring>

#include "pb_encode.h"
#include "pb_decode.h"
#include "message.pb.h"

using asio::ip::tcp;

namespace server::network {

TcpServer::TcpServer(asio::io_context& io_context, unsigned short port)
    : io_context_(io_context),
      acceptor_(io_context_, tcp::endpoint(tcp::v4(), port)) {}


// Helper: read exactly N bytes
static bool read_exact(tcp::socket& socket, void* data, size_t len) {
    size_t total = 0;
    uint8_t* ptr = static_cast<uint8_t*>(data);

    while (total < len) {
        size_t n = socket.read_some(asio::buffer(ptr + total, len - total));
        if (n == 0) return false;
        total += n;
    }
    return true;
}

// Helper: write exactly N bytes
static void write_exact(tcp::socket& socket, const void* data, size_t len) {
    asio::write(socket, asio::buffer(data, len));
}


void TcpServer::run_blocking() {
    for (;;) {
        tcp::socket socket(io_context_);
        std::cout << "[server] Waiting for client...\n";
        acceptor_.accept(socket);

        std::cout << "[server] Client connected\n";

        // 1️⃣ Read 4-byte message length
        uint32_t msg_len = 0;
        if (!read_exact(socket, &msg_len, sizeof(msg_len))) {
            std::cout << "[server] Failed to read length\n";
            continue;
        }

        msg_len = ntohl(msg_len);
        std::cout << "[server] Expecting message of size: " << msg_len << "\n";

        // 2️⃣ Read protobuf body
        std::vector<uint8_t> buf(msg_len);
        if (!read_exact(socket, buf.data(), msg_len)) {
            std::cout << "[server] Failed to read message body\n";
            continue;
        }

        // 3️⃣ Decode protobuf into PlainMessage struct
        cot_PlainMessage incoming_msg = cot_PlainMessage_init_zero;
        pb_istream_t istream = pb_istream_from_buffer(buf.data(), buf.size());

        if (!pb_decode(&istream, cot_PlainMessage_fields, &incoming_msg)) {
            std::cout << "[server] Decode error: " << PB_GET_ERROR(&istream) << "\n";
            continue;
        }

        std::cout << "\n[server] Received Message:" << "\n";
        std::cout << " type = " << incoming_msg.type << "\n";
        std::cout << " payload_size = " << incoming_msg.payload.size << " bytes\n";

        // 4️⃣ Prepare reply message
        cot_PlainMessage reply = cot_PlainMessage_init_zero;
        strncpy(reply.type, "server_reply", sizeof(reply.type)-1);

        const char* reply_payload = "world";
        reply.payload.size = strlen(reply_payload);
        memcpy(reply.payload.bytes, reply_payload, reply.payload.size);

        // 5️⃣ Encode reply protobuf
        uint8_t outbuf[256];
        pb_ostream_t ostream = pb_ostream_from_buffer(outbuf, sizeof(outbuf));

        if (!pb_encode(&ostream, cot_PlainMessage_fields, &reply)) {
            std::cout << "[server] Encode error: " << PB_GET_ERROR(&ostream) << "\n";
            continue;
        }

        uint32_t out_len = ostream.bytes_written;

        // 6️⃣ Send length prefix
        uint32_t net_len = htonl(out_len);
        write_exact(socket, &net_len, sizeof(net_len));

        // 7️⃣ Send encoded protobuf message
        write_exact(socket, outbuf, out_len);

        std::cout << "[server] Reply sent (" << out_len << " bytes)\n\n";
    }
}

} // namespace server::network
