# Changelog

## Unreleased

## 0.2.1

- .chd disc images: the game, the launcher's disc check and picker, the custom soundtrack's other discs and the Android app read a
  `.chd` file as well as a `.cue` with its `.bin` files. A `.chd` holds every track in one file, the music's too (an
  `.iso` has only the data track, so no music). libchdr's license is in the download's licenses folder.
- The in-game menu is laid out as in the other recomp ports: Display and Game first, Speed and Mute on the Game
  page (with Cheats, Sound and Pictures under it), and a new **Reset all options** (every option back to the
  Enhanced preset, and full speed). Speeds from 1/8 to 8x. The mouse works the menu; held arrows repeat;
  the gamepad's Guide button opens it (Back alone no longer does; Start and Back together still do). **P** pauses
  and **O** goes on a frame (F4 and F8 still do), **F12** saves a screenshot.
- Toolkit at satrecomp 0756c55.

## 0.2.0

- Android: an app for ARM64 phones and tablets, Android 9 and later (`FightersMegamix-android.apk`). Touch controls beside
  the picture (a stick, the buttons and their pairs, Start and Menu) hide while a gamepad is used; Back opens the
  in-game menu. The disc is chosen with Android's picker, its .cue and all its .bin files at once, and copied into
  the app's folder. Played on a Pixel 9, widescreen and high resolution included (the GPU renderer no longer asks
  for GPU features it doesn't use, which Mali GPUs like the Pixel's lack).
- Export pictures writes PNG files instead of JPEG: each picture exactly as the Saturn shows it (the Japanese
  disc's bonus pictures are still the JPEG files it holds). Pictures exported before stay as they were; export
  again for the PNG files.
- Toolkit at satrecomp 9616b00.

## 0.1.0-beta.5

- A custom soundtrack (the Features page, under Sound; issue #17): a *Fighting Vipers* or *Virtua Fighter 2* Saturn
  disc image put in the soundtrack folder gives their stages their full-length music (the Vipers' nine, VF2's
  twelve, as the arcade game has them); your own music files, named after the songs, play in place of any.
  `songs.txt` in the folder says what plays for each song. [docs/SOUNDTRACK.md](docs/SOUNDTRACK.md).
- Save states made by earlier versions can no longer be loaded (the toolkit's save states now keep the music's place).
- Export pictures (in-game menu, Game → Pictures): the 30 ending pictures as JPEG files, and with the Japanese disc beside yours
  (Fighters Megamix (Japan), found by the launcher) its four original ending pictures that the American release
  changed and the 13 bonus pictures from its OMAKE archive; to the pictures folder in your files, which Show pictures
  folder opens.
- The winner's close-up after a fight (issue #13): the camera looked too high, the winner's head at the
  bottom of the screen. The game draws that shot in double interlace (twice the lines), which the GPU renderer
  drew at twice its height; it is drawn at its height now, as the software renderer and the Saturn draw it.
- Music carries on between rounds (an option on the Game page, on in the Enhanced preset): the stage's music
  plays through the whole fight instead of starting again at every round.
- Reset game at the bottom of the in-game menu: the game starts again as when switched on; your saves are kept.
- Toolkit at satrecomp 67a5fd4.

## 0.1.0-beta.4

- Sound effects in fights (issue #8): after the opening movie had played, or been skipped, fights had only
  the music. The game waits a set number of reads for its sound driver to take a request and then turns
  effects off for good; the machine answered those reads faster than a Saturn, so the wait ran out. Reads of
  the sound chip now take as long as on the hardware. Toolkit at satrecomp 2086530.

## 0.1.0-beta.3

- Unlock everything (a cheat on the Features page): every character, Mr. Meat and Palm Tree among them, every
  course and the extra options, without playing for them. The game is shown its save with them unlocked; the
  save kept is your own, so with it off the game has what you have unlocked. From the next start or reset.
  Toolkit at satrecomp a7b9c65 (save unlocks).
- Sound effects in fights: they stopped after "Round 1, Fight!", a hiss in their place, while the music went
  on. The file library's Fread is the game's own again, reading through the CD drive (the toolkit's
  replacement read at once, and the game loads its sound through it); loading before a fight takes about two
  seconds longer.
- Widescreen: the stages' cages and fences reach the picture's edges (they ended a little past the 4:3
  picture's sides). The gauges stay in the middle for now.
- README: screenshots (a fight in 16:9 and in 4:3, the character select, the launcher).
- README: a short section per feature, with the picture and the launcher in docs/PICTURE.md and docs/LAUNCHER.md.

## 0.1.0-beta.2

- The opening movie plays (it froze on its first frames), and skipping it with Start works (it crashed);
  the attract loop goes round. Toolkit at satrecomp's fix for both.

## 0.1.0-beta.1

- First release: Fighters Megamix boots, its menus work and arcade fights play on satrecomp's own machine,
  with Fighting Vipers' file, CD music and controller routines (shared in the toolkit). The opening
  movie freezes on its first frame (games/fighters_megamix/PROGRESS.md).
