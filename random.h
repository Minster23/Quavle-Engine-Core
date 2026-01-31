#pragma once
#include <string>
#include <random>
#include <sstream>
#include <chrono>

inline std::string GenerateRandomID()
{
    static std::mt19937_64 rng(
        static_cast<uint64_t>(
            std::chrono::high_resolution_clock::now()
            .time_since_epoch()
            .count()
            )
    );

    std::uniform_int_distribution<uint64_t> dist;

    uint64_t a = dist(rng);
    uint64_t b = dist(rng);

    std::stringstream ss;
    ss << std::hex << a << b;

    return ss.str();
}
