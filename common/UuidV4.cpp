#include <random>
#include <array>
#include <string>
#include <cstdint>

class UuidV4 {
public:
    static std::string generate() {
        // Gerador inicializado uma única vez com seed de alta entropia
        static std::mt19937_64 gen([]() {
            std::random_device rd;
            // Combina múltiplas leituras para maior entropia no seed
            std::seed_seq seed{ rd(), rd(), rd(), rd() };
            return std::mt19937_64(seed);
            }());

        static std::uniform_int_distribution<uint64_t> dist(
            0, std::numeric_limits<uint64_t>::max()
        );

        // Gera 128 bits aleatórios (2 x uint64)
        uint64_t hi = dist(gen);
        uint64_t lo = dist(gen);

        // RFC 4122, Seção 4.4:
        // Bits 12-15 do time_hi_and_version → versão 4 (0100)
        hi = (hi & 0xFFFFFFFFFFFF0FFFull) | 0x0000000000004000ull;

        // Bits 6-7 do clock_seq_hi → variante 1 (10xxxxxx)
        lo = (lo & 0x3FFFFFFFFFFFFFFFull) | 0x8000000000000000ull;

        // Formata como string no padrão xxxxxxxx-xxxx-xxxx-xxxx-xxxxxxxxxxxx
        const char* hex = "0123456789abcdef";
        std::array<char, 37> buf; // 32 hex + 4 hífens + '\0'
        buf[36] = '\0';

        // Escreve os 64 bits superiores (8 + 4 + 4 chars)
        auto write = [&](int pos, uint64_t val, int nibbles) {
            for (int i = nibbles - 1; i >= 0; --i) {
                buf[pos + i] = hex[val & 0xF];
                val >>= 4;
            }
            };

        write(0, hi >> 32, 8);  // time_low
        buf[8] = '-';
        write(9, hi >> 16, 4);  // time_mid
        buf[13] = '-';
        write(14, hi, 4);  // time_hi_and_version
        buf[18] = '-';
        write(19, lo >> 48, 4);  // clock_seq
        buf[23] = '-';
        write(24, lo & 0xFFFFFFFFFFFFull, 12); // node

        return std::string(buf.data(), 36);
    }
};