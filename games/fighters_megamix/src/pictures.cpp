// SPDX-License-Identifier: GPL-3.0-only
// Copyright (C) 2026 Tommy McLeroy. Part of Fighters Megamix Recomp: https://github.com/tmcleroy/fighters-megamix-recomp
// Fighters Megamix's pictures for the menu's Export pictures (satrecomp Game::exportPictures).
//
// - The endings' pictures are in FMM_DATA.DAT, 30 of them, each 112 blocks of 2 KB: 512 by 224 dots of the Saturn's
//   16-bit colour, the picture in the left 352. The program reads the file by blocks from a table of (blocks,
//   first block) pairs, words in 1ST_READ.DAT (at 0x06081C46 in memory): the pictures are its one run of 112-block
//   entries, each after the last (the American disc's from block 10070, the Japanese disc's from 9670).
// - The Japanese disc (--extra-disc) has four of them as they were before the American release changed them
//   (Honey's "Taste of Honey", a fighter's open shirt, Rent-A-Hero's with the credit for its sung theme, Dural's):
//   written as the American ones' names with "-japan".
// - The Japanese disc also carries OMAKE___.LZH, an archive of 13 bonus pictures (1024 by 768 JPEG files) to be
//   looked at on a computer: written as they are (lzh.h).
#include "game.h"
#include "lift.h"
#include "lzh.h"

#include <algorithm>
#include <string>
#include <vector>

namespace satrecomp {

namespace {

constexpr size_t kBlock = 2048, kPictureBlocks = 112;
constexpr int kStride = 512, kWidth = 352, kHeight = 224;

// The first block of each ending picture, from a disc's program: the longest run of (112, first) pairs whose firsts
// follow one another
std::vector<uint32_t> EndingPictures(const std::vector<uint8_t> &program) {
    std::vector<uint32_t> best;
    for (size_t parity = 0; parity < 4; parity += 2) {
        std::vector<uint32_t> run;
        for (size_t at = parity; at + 4 <= program.size(); at += 4) {
            const uint32_t blocks = static_cast<uint32_t>(program[at] << 8 | program[at + 1]);
            const uint32_t first = static_cast<uint32_t>(program[at + 2] << 8 | program[at + 3]);
            if (blocks == kPictureBlocks && (run.empty() || first == run.back() + kPictureBlocks)) {
                run.push_back(first);
            } else {
                if (run.size() > best.size()) {
                    best = run;
                }
                run.clear();
                if (blocks == kPictureBlocks) {
                    run.push_back(first);
                }
            }
        }
        if (run.size() > best.size()) {
            best = run;
        }
    }
    return best.size() >= 8 ? best : std::vector<uint32_t>{}; // (a short run is not the table)
}

std::string Number(size_t n) {
    return (n < 10 ? "0" : "") + std::to_string(n);
}

void ExportPictures(PictureOut &out) {
    // The endings' pictures, from the player's disc
    std::vector<uint8_t> program, data;
    std::vector<uint32_t> ours;
    if (ReadDiscFile("1ST_READ.DAT", program) && ReadDiscFile("FMM_DATA.DAT", data)) {
        ours = EndingPictures(program);
        for (size_t i = 0; i < ours.size(); i++) {
            const size_t at = ours[i] * kBlock;
            if (at + kPictureBlocks * kBlock <= data.size()) {
                out.picture("ending-" + Number(i + 1), &data[at], kWidth, kHeight, kStride);
            }
        }
    }
    // The Japanese disc's own, where they differ (its program and its archive have the same names)
    std::vector<uint8_t> theirProgram, theirData;
    if (ReadExtraDiscFile("1ST_READ.DAT", theirProgram) && ReadExtraDiscFile("FMM_DATA.DAT", theirData)) {
        const std::vector<uint32_t> theirs = EndingPictures(theirProgram);
        for (size_t i = 0; i < theirs.size(); i++) {
            const size_t at = theirs[i] * kBlock, size = kPictureBlocks * kBlock;
            if (at + size > theirData.size()) {
                continue;
            }
            const bool differs = i >= ours.size() || ours[i] * kBlock + size > data.size() ||
                                 !std::equal(theirData.begin() + static_cast<std::ptrdiff_t>(at),
                                             theirData.begin() + static_cast<std::ptrdiff_t>(at + size),
                                             data.begin() + static_cast<std::ptrdiff_t>(ours[i] * kBlock));
            if (differs) {
                out.picture("ending-" + Number(i + 1) + "-japan", &theirData[at], kWidth, kHeight, kStride);
            }
        }
    }
    // The bonus pictures (the Japanese disc's; ReadDiscFile finds a file the player's disc does not have there)
    std::vector<uint8_t> archive;
    if (ReadDiscFile("OMAKE___.LZH", archive)) {
        for (const lzh::File &file : lzh::Read(archive)) {
            const size_t dot = file.name.rfind('.');
            const std::string stem = file.name.substr(0, dot);
            const std::string extension = dot == std::string::npos ? "bin" : file.name.substr(dot + 1);
            out.file("bonus-" + stem, file.data, extension);
        }
    }
}

} // namespace

void RegisterPictures() {
    TheGame().exportPictures = ExportPictures;
}

} // namespace satrecomp
