// SPDX-License-Identifier: GPL-3.0-only
// Copyright (C) 2026 Tommy McLeroy. Part of Fighters Megamix Recomp: https://github.com/tmcleroy/fighters-megamix-recomp
// Fighters Megamix: the routines of the game that are replaced by C++ (see PROGRESS.md).
#include "game.h"
#include "lift.h"
#include "shared_lifts.h"
#include "state.h"

namespace satrecomp {

namespace {

// Routines Fighters Megamix shares byte for byte with Fighting Vipers (Sega's libraries, and AM2's engine):
// the same replacements, at this game's addresses (satrecomp/runtime/shared_lifts.h)
void RegisterShared() {
    using namespace shared;
    Register(kAm2LoadFile, 0x0602DE3C);
    Register(kAm2ReadPads, 0x0602F168);
    Register(kGfsOpen, 0x060483BC);
    Register(kGfsStart, 0x06048664);
    Register(kGfsPoll, 0x060489D8);
    Register(kGfsClose, 0x0604894C);
    Register(kCdPlay, 0x06047F50);
    Register(kCdSeek, 0x06048014);
    Register(kCdGetCurStat, 0x060479C8);
    Register(kPerSendCommand, 0x0605C458);
    // The opening movie's player (a second program, loaded at 060C0000) has its own copy of the peripheral
    // library: the same routine, so Start skips the movie
    Register(kPerSendCommand, 0x060D4510, "SendCommand (movie player)");
}

// Unlock everything (a cheat): what the game unlocks is in its save file FIGHTERS_MM (1,396 bytes, read to
// 0x0609FA00 and kept there), and the toolkit shows the game that file with these fields raised, keeping the
// player's own (Game::saveUnlocks). Found from the code that reads them:
// - 0x0609FAA8, a longword, one bit for each character or course unlocked: the screens that choose them take all
//   ones for all (character select, 0x0603DE30, offers 28 characters instead of 20, and 0x0603CB4C 42 places
//   instead of 35); a new file has 0x07FF85FF.
// - 0x0609FB02, a word of extras the game adds as it is played (the routine at 0x06037B58): 0x0080, 0x0600,
//   0x0100, 0x0040 and 0x0030, each when every course is cleared (0x0609FB04 = 0x01FF) and a count is reached.
// - 0x0609FB85 to 0x0609FB88, a byte each for the characters the game gives for time played and times started
//   (Mr. Meat and Palm Tree: 0x0609FB86 when it has been started 29 times, 0x0609FB87 when 0x0609FA80 passes
//   5,039) and the extra the same routine gives last.
void RegisterUnlocks() {
    Game &game = TheGame();
    game.features.push_back({"unlock_all", "Cheats", "Unlock everything",
                             "Every character, course and extra option, without playing for them. Your own save is kept as it is: "
                             "turned off, the game has what you have unlocked. From the next start or reset."});
    game.saveUnlocks = {"unlock_all", "FIGHTERS_MM", {
        {0xA8, 4, SaveField::kBits, 0xFFFFFFFF},
        {0x102, 2, SaveField::kBits, 0x07F0},
        {0x185, 1, SaveField::kBits, 1},
        {0x186, 1, SaveField::kBits, 1},
        {0x187, 1, SaveField::kBits, 1},
        {0x188, 1, SaveField::kBits, 1},
    }};
}

} // namespace

// The disc's music, tracks 2 to 42, by the names the game's own list of them gives (beside the stages' music in its
// table at 0x06082024: a stage's track is the first halfword of its 8-byte entry, by the stage at 0x060E1005),
// for a custom soundtrack's files (the toolkit's port/soundtrack.h); and the other discs that have some of it
static void RegisterSongs() {
    TheGame().songs = {
        {2, "you_win_1", "Winner's fanfare 1"},
        {3, "you_win_2", "Winner's fanfare 2"},
        {4, "you_win_3", "Winner's fanfare 3"},
        {5, "you_win_4", "Winner's fanfare 4"},
        {6, "you_lose", "Loser's tune"},
        {7, "continue", "Continue"},
        {8, "selector", "Character select"},
        {9, "last_wave", "Last Wave (Out Run)"},
        {10, "ab2_city202", "After Burner II's City 202"},
        {11, "fv_bahn", "Fighting Vipers, Bahn"},
        {12, "fv_grace", "Fighting Vipers, Grace"},
        {13, "fv_picky", "Fighting Vipers, Picky"},
        {14, "fv_tokio", "Fighting Vipers, Tokio"},
        {15, "fv_sanman", "Fighting Vipers, Sanman"},
        {16, "fv_candy", "Fighting Vipers, Candy"},
        {17, "fv_raxel", "Fighting Vipers, Raxel"},
        {18, "fv_jane", "Fighting Vipers, Jane"},
        {19, "fv_bm", "Fighting Vipers, B.M."},
        {20, "vf1_sarah", "Virtua Fighter, Sarah"},
        {21, "vf1_kage", "Virtua Fighter, Kage"},
        {22, "vf1_dural", "Virtua Fighter, Dural"},
        {23, "vf2_lau", "Virtua Fighter 2, Lau"},
        {24, "vf2_sarah", "Virtua Fighter 2, Sarah"},
        {25, "vf2_shun", "Virtua Fighter 2, Shun"},
        {26, "vf2_pai", "Virtua Fighter 2, Pai"},
        {27, "vf2_lion", "Virtua Fighter 2, Lion"},
        {28, "vf2_jeffry", "Virtua Fighter 2, Jeffry"},
        {29, "vf2_kage", "Virtua Fighter 2, Kage"},
        {30, "vf2_jacky", "Virtua Fighter 2, Jacky"},
        {31, "vf2_wolf", "Virtua Fighter 2, Wolf"},
        {32, "vf2_akira", "Virtua Fighter 2, Akira"},
        {33, "vf2_dural", "Virtua Fighter 2, Dural"},
        {34, "vf2_chicago", "Virtua Fighter 2, Chicago"},
        {35, "vf3_jeffry", "Virtua Fighter 3, Jeffry"},
        {36, "vf3_advertise", "Virtua Fighter 3 advertisement"},
        {37, "kids_akira", "Kids Akira"},
        {38, "sonic_stage1", "Sonic the Fighters, stage 1"},
        {39, "sonic_stage2", "Sonic the Fighters, stage 2"},
        {40, "virtua_city", "Virtua City"},
        {41, "rolling_start", "Rolling Start"},
        {42, "rent_a_hero", "Rent-A-Hero"},
    };
    // Fighting Vipers' discs have the Vipers' stage music as recorded here, at full length (141-176 seconds; here
    // 78-94): Megamix's tracks 11-19 are the US disc's 7-15 and the Japanese disc's 9-17, the same from the start
    std::vector<std::pair<int, int>> us, japan;
    for (int track = 11; track <= 19; track++) {
        us.push_back({track, track - 4});
        japan.push_back({track, track - 2});
    }
    TheGame().songDiscs = {{"MK-81041", us}, {"GS-9101", japan}};
}

void RegisterGame() {
    RegisterShared();
    RegisterSongs();
    RegisterUnlocks();
    // The stage's music is asked for again at every round (CdPlay of the track already playing); the option
    // lets it play on through the fight
    shared::MusicCarriesOn();
    // Widescreen: the stages' panels (the cages, fences and walls) are listed by four of the engine's routines,
    // each with its own copy of a limit on how far off the picture's middle a panel may be (the toolkit's
    // shared::WidenAm2PanelLimits): two shaped as in Fighting Vipers (the limit loaded at 0x06020BC4 and
    // 0x06020CD0) and two more (0x06021F76, 0x060220E6). (The gauges stay in the middle: Fighting Vipers' way of
    // moving them out, a split of NBG2's rows, would split the title screen's text here too, which is on that
    // screen in the same rows; it needs the game's "a fight is on", not found yet.)
    TheGame().frame = [] { shared::WidenAm2PanelLimits({0x06020BC4, 0x06020CD0, 0x06021F76, 0x060220E6}); };
    TheGame().state = [](Archive &ar) { shared::State(ar); };
}

} // namespace satrecomp
