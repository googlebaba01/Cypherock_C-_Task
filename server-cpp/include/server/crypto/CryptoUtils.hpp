#pragma once

#include <array>
#include <cstdint>

namespace server::crypto {

using Scalar32 = std::array<uint8_t, 32>;

class CryptoUtils {
public:
    // Return a random scalar in [1, n-1] where n = order(secp256k1)
    static Scalar32 random_scalar();

    // (a + b) mod n
    static Scalar32 add_mod_n(const Scalar32& a, const Scalar32& b);

    // (a * b) mod n
    static Scalar32 mul_mod_n(const Scalar32& a, const Scalar32& b);
};

} // namespace server::crypto
