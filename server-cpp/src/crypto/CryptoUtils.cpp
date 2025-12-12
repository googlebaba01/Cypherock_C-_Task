#include "server/crypto/CryptoUtils.hpp"

#include "bignum.h"
#include "secp256k1.h"

#include <random>
#include <cstring>

namespace server::crypto {

// --- Helper: fill buffer with cryptographic random bytes ---
static void random_bytes(uint8_t* out, size_t len) {
    // Use random_device + mt19937 to get reasonably good entropy for tests.
    // For production, replace with a proper CSPRNG.
    static thread_local std::random_device rd;
    static thread_local std::mt19937 rng(rd());
    std::uniform_int_distribution<uint32_t> dist(0, 0xFFFFFFFFu);

    size_t i = 0;
    while (i + 4 <= len) {
        uint32_t v = dist(rng);
        out[i + 0] = static_cast<uint8_t>(v >> 24);
        out[i + 1] = static_cast<uint8_t>(v >> 16);
        out[i + 2] = static_cast<uint8_t>(v >> 8);
        out[i + 3] = static_cast<uint8_t>(v);
        i += 4;
    }
    if (i < len) {
        uint32_t v = dist(rng);
        for (; i < len; ++i) {
            out[i] = static_cast<uint8_t>(v >> 24);
            v <<= 8;
        }
    }
}

// --- Helper: compare two bignum256 values
// returns -1 if a < b, 0 if equal, +1 if a > b
static int bn_cmp_words(const bignum256* a, const bignum256* b) {
    // Note: our bignum stores words with val[7] = most-significant word
    for (int i = 7; i >= 0; --i) {
        if (a->val[i] < b->val[i]) return -1;
        if (a->val[i] > b->val[i]) return 1;
    }
    return 0;
}

// static bool bn_is_zero(const bignum256* a) {
//     for (int i = 0; i < 8; ++i) {
//         if (a->val[i] != 0) return false;
//     }
//     return true;
// }

// --- Convert bignum256 -> Scalar32 (big-endian 32 bytes)
static void bignum_to_buf(const bignum256* x, Scalar32& out) {
    bn_write_be(x, out.data());
}

// --- Convert Scalar32 -> bignum256
static void buf_to_bignum(const Scalar32& in, bignum256* out) {
    bn_read_be(in.data(), out);
}

// === CryptoUtils implementation ===

Scalar32 CryptoUtils::random_scalar() {
    Scalar32 out{};
    bignum256 k;
    const bignum256* order = &secp256k1_info.order;

    for (;;) {
        random_bytes(out.data(), out.size());
        buf_to_bignum(out, &k);

        // Accept only 1 <= k < order
        if (!bn_is_zero(&k) && (bn_cmp_words(&k, order) < 0)) {
            return out;
        }
        // otherwise try again
    }
}

Scalar32 CryptoUtils::add_mod_n(const Scalar32& a, const Scalar32& b) {
    Scalar32 out{};
    bignum256 A, B, R;
    buf_to_bignum(a, &A);
    buf_to_bignum(b, &B);

    // bn_addmod(a, b, mod, out)
    bn_addmod(&A, &B, &secp256k1_info.order, &R);

    bignum_to_buf(&R, out);
    return out;
}

Scalar32 CryptoUtils::mul_mod_n(const Scalar32& a, const Scalar32& b) {
    Scalar32 out{};
    bignum256 A, B, R;
    buf_to_bignum(a, &A);
    buf_to_bignum(b, &B);

    // bn_multiply(a, b, mod, out)  -- (A * B) mod order
    bn_multiply(&A, &B, &secp256k1_info.order, &R);

    bignum_to_buf(&R, out);
    return out;
}

} // namespace server::crypto
