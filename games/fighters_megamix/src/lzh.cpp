// SPDX-License-Identifier: GPL-3.0-only
// Copyright (C) 2026 Tommy McLeroy. Part of Fighters Megamix Recomp: https://github.com/tmcleroy/fighters-megamix-recomp
// LZH archives (lzh.h). The -lh5- format, as LHA writes it: blocks, each starting with its size in codes and three
// Huffman tables (for the code lengths of the next, for the 510 literal-or-length codes, for the 14 distance
// classes) given as code lengths; then codes: a byte, or a match of 3 to 256 bytes at a distance in the last 8 KB.
#include "lzh.h"

#include <cstring>

namespace satrecomp::lzh {

namespace {

constexpr int kDicBits = 13;                    // the window: 8 KB
constexpr int kNC = 255 + 256 + 2 - 3;          // literals and match lengths: 510 codes
constexpr int kNP = kDicBits + 1;               // distance classes
constexpr int kNT = 16 + 3;                     // code length codes
constexpr int kCBits = 9, kTBits = 5, kPBits = 4; // the bits that give each table's count
constexpr int kThreshold = 3;                   // the shortest match

class Decoder {
public:
    Decoder(const uint8_t *in, size_t size)
        : m_in(in), m_size(size) {
        Fill(16);
    }

    bool Decode(size_t outSize, std::vector<uint8_t> &out) {
        out.clear();
        out.reserve(outSize);
        while (out.size() < outSize) {
            const int c = DecodeC();
            if (m_bad) {
                return false;
            }
            if (c < 256) {
                out.push_back(static_cast<uint8_t>(c));
                continue;
            }
            const size_t length = static_cast<size_t>(c - (256 - kThreshold));
            const size_t distance = static_cast<size_t>(DecodeP()) + 1;
            if (m_bad || distance > out.size()) {
                return false;
            }
            for (size_t i = 0; i < length && out.size() < outSize; i++) {
                out.push_back(out[out.size() - distance]);
            }
        }
        return true;
    }

private:
    // The next 16 bits are in m_bits (the top 16 of it); taking n of them brings n more in
    void Fill(int n) {
        while (n > 0) {
            if (m_count == 0) {
                m_byte = m_pos < m_size ? m_in[m_pos++] : 0;
                m_count = 8;
            }
            const int take = n < m_count ? n : m_count;
            m_bits = static_cast<uint16_t>((m_bits << take) | ((m_byte >> (m_count - take)) & ((1 << take) - 1)));
            m_count -= take;
            n -= take;
        }
    }
    unsigned Get(int n) {
        if (n == 0) {
            return 0;
        }
        const unsigned v = static_cast<unsigned>(m_bits >> (16 - n));
        Fill(n);
        return v;
    }

    // A table of codes: the first `tableBits` bits of a code index `table`; longer codes go on down a tree (left,
    // right) a bit at a time
    void MakeTable(int nchar, const uint8_t *bitLength, int tableBits, uint16_t *table) {
        uint16_t count[17] = {}, weight[17] = {}, start[18] = {};
        for (int i = 0; i < nchar; i++) {
            if (bitLength[i] > 16) {
                m_bad = true;
                return;
            }
            count[bitLength[i]]++;
        }
        unsigned total = 0;
        for (int i = 1; i <= 16; i++) {
            start[i] = static_cast<uint16_t>(total);
            total += static_cast<unsigned>(count[i]) << (16 - i);
        }
        if (total != (1u << 16)) {
            m_bad = true;
            return;
        }
        const int jut = 16 - tableBits;
        for (int i = 1; i <= tableBits; i++) {
            start[i] = static_cast<uint16_t>(start[i] >> jut);
            weight[i] = static_cast<uint16_t>(1u << (tableBits - i));
        }
        for (int i = tableBits + 1; i <= 16; i++) {
            weight[i] = static_cast<uint16_t>(1u << (16 - i));
        }
        start[17] = 0;
        {
            unsigned i = static_cast<unsigned>(start[tableBits + 1]) >> jut;
            if (tableBits + 1 > 16 || start[tableBits + 1] == 0) {
                i = 1u << tableBits; // (all of the table is used)
            }
            for (; i < (1u << tableBits); i++) {
                table[i] = 0;
            }
        }
        int avail = nchar;
        const unsigned mask = 1u << (15 - tableBits);
        for (int ch = 0; ch < nchar; ch++) {
            const int len = bitLength[ch];
            if (len == 0) {
                continue;
            }
            const unsigned next = static_cast<unsigned>(start[len]) + weight[len];
            if (len <= tableBits) {
                for (unsigned i = start[len]; i < next; i++) {
                    table[i] = static_cast<uint16_t>(ch);
                }
            } else {
                unsigned k = start[len];
                uint16_t *p = &table[k >> jut];
                for (int i = len - tableBits; i != 0; i--) {
                    if (*p == 0) {
                        if (avail >= 2 * kNC) {
                            m_bad = true;
                            return;
                        }
                        m_right[avail] = m_left[avail] = 0;
                        *p = static_cast<uint16_t>(avail++);
                    }
                    p = (k & mask) ? &m_right[*p] : &m_left[*p];
                    k <<= 1;
                }
                *p = static_cast<uint16_t>(ch);
            }
            start[len] = static_cast<uint16_t>(next);
        }
    }

    void ReadPtLength(int nn, int nbit, int special) {
        const int n = static_cast<int>(Get(nbit));
        if (n == 0) {
            const uint16_t c = static_cast<uint16_t>(Get(nbit));
            std::memset(m_ptLength, 0, sizeof m_ptLength);
            for (uint16_t &t : m_ptTable) {
                t = c;
            }
            return;
        }
        int i = 0;
        while (i < n && i < nn) {
            int c = m_bits >> 13;
            if (c == 7) {
                unsigned mask = 1u << 12;
                while (mask & m_bits) {
                    mask >>= 1;
                    c++;
                }
            }
            Fill(c < 7 ? 3 : c - 3);
            m_ptLength[i++] = static_cast<uint8_t>(c);
            if (i == special) {
                for (int z = static_cast<int>(Get(2)); z > 0 && i < nn; z--) {
                    m_ptLength[i++] = 0;
                }
            }
        }
        while (i < nn) {
            m_ptLength[i++] = 0;
        }
        MakeTable(nn, m_ptLength, 8, m_ptTable);
    }

    void ReadCLength() {
        const int n = static_cast<int>(Get(kCBits));
        if (n == 0) {
            const uint16_t c = static_cast<uint16_t>(Get(kCBits));
            std::memset(m_cLength, 0, sizeof m_cLength);
            for (uint16_t &t : m_cTable) {
                t = c;
            }
            return;
        }
        int i = 0;
        while (i < n && i < kNC) {
            int c = m_ptTable[m_bits >> 8];
            if (c >= kNT) {
                unsigned mask = 1u << 7;
                do {
                    c = (m_bits & mask) ? m_right[c] : m_left[c];
                    mask >>= 1;
                } while (c >= kNT && mask);
            }
            Fill(m_ptLength[c]);
            if (c <= 2) {
                int zeros = c == 0 ? 1 : c == 1 ? static_cast<int>(Get(4)) + 3 : static_cast<int>(Get(kCBits)) + 20;
                while (zeros-- > 0 && i < kNC) {
                    m_cLength[i++] = 0;
                }
            } else {
                m_cLength[i++] = static_cast<uint8_t>(c - 2);
            }
        }
        while (i < kNC) {
            m_cLength[i++] = 0;
        }
        MakeTable(kNC, m_cLength, 12, m_cTable);
    }

    int DecodeC() {
        if (m_blockSize == 0) {
            m_blockSize = Get(16);
            ReadPtLength(kNT, kTBits, 3);
            ReadCLength();
            ReadPtLength(kNP, kPBits, -1);
            if (m_bad) {
                return 0;
            }
        }
        m_blockSize--;
        int j = m_cTable[m_bits >> 4];
        if (j >= kNC) {
            unsigned mask = 1u << 3;
            do {
                j = (m_bits & mask) ? m_right[j] : m_left[j];
                mask >>= 1;
            } while (j >= kNC && mask);
        }
        Fill(m_cLength[j]);
        return j;
    }

    int DecodeP() {
        int j = m_ptTable[m_bits >> 8];
        if (j >= kNP) {
            unsigned mask = 1u << 7;
            do {
                j = (m_bits & mask) ? m_right[j] : m_left[j];
                mask >>= 1;
            } while (j >= kNP && mask);
        }
        Fill(m_ptLength[j]);
        if (j != 0) {
            j = static_cast<int>((1u << (j - 1)) + Get(j - 1));
        }
        return j;
    }

    const uint8_t *m_in;
    size_t m_size, m_pos = 0;
    uint16_t m_bits = 0;
    uint8_t m_byte = 0;
    int m_count = 0;
    unsigned m_blockSize = 0;
    bool m_bad = false;
    uint8_t m_cLength[kNC] = {}, m_ptLength[kNT + kNP] = {};
    uint16_t m_cTable[4096] = {}, m_ptTable[256] = {};
    uint16_t m_left[2 * kNC] = {}, m_right[2 * kNC] = {};
};

uint16_t Crc16(const std::vector<uint8_t> &data) { // (CRC-16, polynomial 0xA001 reflected, as LHA's)
    uint16_t crc = 0;
    for (const uint8_t b : data) {
        crc ^= b;
        for (int i = 0; i < 8; i++) {
            crc = static_cast<uint16_t>(crc & 1 ? (crc >> 1) ^ 0xA001 : crc >> 1);
        }
    }
    return crc;
}

uint32_t Le32(const uint8_t *p) {
    return static_cast<uint32_t>(p[0] | p[1] << 8 | p[2] << 16 | static_cast<uint32_t>(p[3]) << 24);
}
uint16_t Le16(const uint8_t *p) {
    return static_cast<uint16_t>(p[0] | p[1] << 8);
}

} // namespace

std::vector<File> Read(const std::vector<uint8_t> &archive) {
    std::vector<File> files;
    size_t at = 0;
    while (at + 24 <= archive.size() && archive[at] != 0) {
        const uint8_t *h = &archive[at];
        const size_t headerSize = h[0];
        const std::string method(reinterpret_cast<const char *>(h + 2), 5);
        uint32_t packed = Le32(h + 7);
        const uint32_t size = Le32(h + 11);
        const int level = h[20];
        if (level != 0 && level != 1) {
            break; // (levels 2 and 3 have another layout; this archive has neither)
        }
        const size_t nameLength = h[21];
        if (at + 22 + nameLength + 2 > archive.size()) {
            break;
        }
        File file;
        file.name.assign(reinterpret_cast<const char *>(h + 22), nameLength);
        const uint16_t crc = Le16(h + 22 + nameLength);
        size_t data = at + 2 + headerSize;
        if (level == 1) {
            // (extended headers follow the header, their sizes counted in `packed`; the last says 0)
            for (size_t next = Le16(&archive[data - 2]); next != 0 && data + next <= archive.size(); next = Le16(&archive[data - 2])) {
                data += next;
                packed -= static_cast<uint32_t>(next);
            }
        }
        if (data + packed > archive.size()) {
            break;
        }
        bool ok = false;
        if (method == "-lh0-") {
            file.data.assign(archive.begin() + static_cast<std::ptrdiff_t>(data), archive.begin() + static_cast<std::ptrdiff_t>(data + packed));
            ok = file.data.size() == size;
        } else if (method == "-lh5-") {
            Decoder decoder(&archive[data], packed);
            ok = decoder.Decode(size, file.data);
        }
        if (ok && Crc16(file.data) == crc) {
            files.push_back(std::move(file));
        }
        at = data + packed;
    }
    return files;
}

} // namespace satrecomp::lzh
