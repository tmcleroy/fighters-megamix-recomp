# A custom soundtrack

The music on the *Fighters Megamix* disc is cut short: most stage tunes repeat after 80 to 95 seconds. This port can
play other music in their place, through the game's own sound mixing, so its volume and fades still apply:

- **Your own files**: an arranged soundtrack, a full-length rip of an album you own, anything as Ogg Vorbis, FLAC,
  MP3 or WAV.
- **Fighting Vipers' disc**: if you have *Fighting Vipers* on the Saturn too (USA or Japan), its disc has the Vipers'
  stage music as Megamix has it, at full length (141 to 176 seconds). Name the disc and the nine Vipers stages
  use it.
- **Any other disc's tracks**: one song at a time, by track number.

## Setting it up

1. Switch on **Custom soundtrack** (the Features page, under Sound; it is on in Enhanced).
2. On the launcher's first page, in the Sound card, **Choose…** a folder for it. With none chosen, the game uses the
   `soundtrack` folder in [its own folder](LAUNCHER.md#your-files), which it makes.

The game writes `songs.txt` in the folder: the name of every piece of music and its track on the disc. **Name a
file after the song it replaces**: `vf2_akira.flac`, `fv_grace.ogg`, `selector.mp3` (character select),
`you_win_1.wav`. The track's number works too (`32.flac`, `track32.flac`). A song you have no file for plays as the
disc has it.

## soundtrack.ini

For anything more, put `soundtrack.ini` in the folder:

```ini
volume = 80                     ; every song, in percent
disc = Fighting Vipers (USA) (5S).cue   ; another disc (this one gives the Vipers' stages their full music)

[vf2_akira]                     ; a song by its name (or its track: [32])
file = 05 Akira.flac
loop = 0:12.5 - 2:47.25         ; the part that repeats: start - end (minutes:seconds, or seconds); "no": once
volume = 90

[selector]
disc = Another game.cue         ; a disc for this song alone
track = 4                       ; its audio track 4
```

A file or disc is named by its full path, or by its name alone when it is in the folder.

A song repeats as the game repeats its music (a stage's tune over and over, a fanfare once). A file repeats whole
unless `loop` says otherwise, or it has the loop tags many soundtrack packs carry (`LOOPSTART`, with `LOOPEND` or
`LOOPLENGTH`). A song longer than the disc's track plays to its end.

Your files and discs are only ever read. Switching the feature off in the in-game menu brings back the disc's music
from the next frame.
