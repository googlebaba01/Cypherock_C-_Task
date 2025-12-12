cmake_minimum_required(VERSION 3.20)
project(cypherock_server LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(ASIO_ROOT "C:/libs/asio-1.36.0/include")

# === Nanopb ===
set(NANOPB_DIR ${CMAKE_SOURCE_DIR}/external/nanopb)
set(GENERATED_DIR ${CMAKE_SOURCE_DIR}/generated)

set(NANOPB_SOURCES
    ${NANOPB_DIR}/pb_common.c
    ${NANOPB_DIR}/pb_encode.c
    ${NANOPB_DIR}/pb_decode.c
)

set(PROTO_SOURCES
    ${GENERATED_DIR}/message.pb.c
)

# === Trezor Minimal ECC Subset ===
set(TREZOR_CRYPTO_DIR ${CMAKE_SOURCE_DIR}/external/trezor-crypto)

set(TREZOR_SOURCES
    ${TREZOR_CRYPTO_DIR}/bignum.c
    ${TREZOR_CRYPTO_DIR}/curves.c
    ${TREZOR_CRYPTO_DIR}/secp256k1.c
)

# === Build executable ===
add_executable(cypherock_server
    src/main.cpp
    src/network/TcpServer.cpp
    src/crypto/CryptoUtils.cpp
    ${NANOPB_SOURCES}
    ${PROTO_SOURCES}
    ${TREZOR_SOURCES}
)

# === Include paths ===
target_include_directories(cypherock_server
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${ASIO_ROOT}
        ${NANOPB_DIR}
        ${GENERATED_DIR}
        ${TREZOR_CRYPTO_DIR}
)

# === Windows networking ===
target_link_libraries(cypherock_server PRIVATE ws2_32)
