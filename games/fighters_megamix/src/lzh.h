// SPDX-License-Identifier: GPL-3.0-only
// Copyright (C) 2026 Tommy McLeroy. Part of Fighters Megamix Recomp: https://github.com/tmcleroy/fighters-megamix-recomp
// Reading an LZH archive (LHA, the format of the Japanese disc's OMAKE___.LZH): its files stored (-lh0-) or
// compressed with -lh5- (an 8 KB window, Huffman-coded), each checked against the CRC-16 its header gives.
#pragma once

#include <cstdint>
#include <string>
#include <vector>

namespace satrecomp::lzh {

struct File {
    std::string name;
    std::vector<uint8_t> data;
};

// The archive's files that could be read and passed their CRC (the others are left out)
std::vector<File> Read(const std::vector<uint8_t> &archive);

} // namespace satrecomp::lzh
