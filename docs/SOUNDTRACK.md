# A custom soundtrack

The music on the *Fighters Megamix* disc is cut short: most stage tunes repeat after 80 to 95 seconds. This port can
play other music in their place, through the game's own sound mixing, so its volume and fades still apply.

## The quick way: another game's disc

The Vipers' and the Virtua Fighter 2 characters' stage music in Megamix is the music of those games, cut down. Their
Saturn discs have it at full length:

| Disc | Gives | Length there (here) |
| --- | --- | --- |
| *Fighting Vipers* (USA, Europe or Japan) | the nine Vipers' stages | 141-176 seconds (78-94) |
| *Virtua Fighter 2* (any release) | the twelve VF2 stages, as the arcade game has them | 138-217 seconds (79) |

1. Switch on **Custom soundtrack** (the Features page, under Sound; it is on in Enhanced).
2. Open the soundtrack folder: on the launcher's first page, the Sound card's **Custom soundtrack** row (**Choose…**
   a folder of your own there, or leave it and use the `soundtrack` folder in
   [the game's own folder](LAUNCHER.md#your-files), which the game makes the first time it starts).
3. Put the disc image in it: the `.cue` file with its `.bin` files, or the whole folder they are in. Both discs can be
   there at once.
4. Start the game.

That is all. The disc image is only read, never changed, and needs no renaming.

## Your own files

Name a file after the song it replaces, with `.ogg`, `.flac`, `.mp3` or `.wav`: `vf2_akira.flac`, `fv_grace.ogg`,
`selector.mp3` (the character select), `you_win_1.wav`. The track's number works too (`32.flac`, `track32.flac`). A
file of your own goes before a disc's, and a song with neither plays as the Megamix disc has it.

## songs.txt: what plays

Every time it starts, the game writes `songs.txt` in the soundtrack folder: the name of every piece of music, the
discs it found there, and **what plays for each song now** (your file, a disc's track, or the game's own). If
something is not playing as you meant, look there first.

## soundtrack.ini

For anything more, put `soundtrack.ini` in the folder:

```ini
volume = 80                     ; every song, in percent

[vf2_akira]                     ; a song by its name (or its track: [32])
file = 05 Akira.flac
loop = 0:12.5 - 2:47.25         ; the part that repeats: start - end (minutes:seconds, or seconds); "no": once
volume = 90

[selector]
disc = Another game.cue         ; any disc's audio track, by number
track = 4
```

A file or disc is named by its full path, or by its name alone when it is in the folder. A song repeats as the game
repeats its music (a stage's tune over and over, a fanfare once). A file repeats whole unless `loop` says otherwise,
or it has the loop tags many soundtrack packs carry (`LOOPSTART`, with `LOOPEND` or `LOOPLENGTH`). A song longer
than Megamix's track plays to its end; turn on **Music carries on between rounds** (the Game page) to hear it go on
through a whole fight.

Switching the feature off in the in-game menu brings back the disc's music from the next frame.
