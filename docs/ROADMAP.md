# Roadmap

Roughly in order. [games/fighters_megamix/PROGRESS.md](../games/fighters_megamix/PROGRESS.md) has the
detail.

1. Hand play on the Mac: every mode, versus with two pads, sound.
2. Widescreen: the renderer's switch, then the HUD at the picture's edges (Fighting Vipers' way).
3. The fight rules as readable code, from Fighting Vipers' rewrites (round state machine, damage, input),
   then a training mode like Fighting Vipers' (dummy, input display, frame data, hit view, reset point).
4. A training mode's worth of readable code, then Linux, SteamOS and Windows downloads played by hand.
5. From players (issues #17-#19): a custom soundtrack (done: [SOUNDTRACK.md](SOUNDTRACK.md)), move lists in the
   menu, more modes (a random or chosen arcade ladder, survival with health carried over).

Later, and large:

- Saturn Virtua Fighter 2's character models in Megamix's place: its meshes and textures made into Megamix's
  format and fitted to Megamix's animations.
- Model 2 assets (the arcade games' stages, then characters): a second renderer beside the Saturn picture.
- New characters: room for more than the 32 in the character select and the per-character tables, and each one's
  moves, animations and hit boxes made from nothing.
