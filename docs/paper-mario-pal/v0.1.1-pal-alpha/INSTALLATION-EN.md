# Paper Mario PAL v0.1.1-pal-alpha — Installation (English)

For **Miyoo Mini Plus with OnionOS**. Native ARM port; **no compilation required**. This packaging update adds a Ports entry, a new App icon and a Ports cover. The program and recipes are unchanged.

## Install and import into Ports

1. When published, download **PaperMarioPAL-0.1.1-pal-alpha-OnionOS.zip** from the **v0.1.1-pal-alpha** pre-release on SunnyBoon/miyoo64. ZIP SHA-256: **d65494e451820223279bc2fa442d1a165c861649bc0312a03ecb938e73430f31**. Keep the accompanying **NOTICE.md** and **THIRD-PARTY-LICENSES.txt** with the ZIP; LICENSE and the full documentation belong to the repository documentation set.
2. Exit the game before updating. Extract at the **SD-card root**, merging the existing **App/** and **Roms/** directories. Preserve **App/PaperMarioPAL/** and **Roms/PORTS/** exactly. Do not replace or delete the entire App or Roms directory. This package does not target the original US App/PaperMario installation.
3. Supply your own **Paper Mario (Europe, PAL)** ROM as **App/PaperMarioPAL/rom.z64**. Required: **64 MiB (67,108,864 bytes), big-endian Z64**, SHA-1 **2111d39265a317414d359e35a7d971c4dfa5f9e1**. No ROM, extracted game assets, saves or runtime caches are included. Renaming another ROM does not convert it.
4. For Apps, start **Paper Mario PAL 0.1-pal-alpha**. This remains the actual App label inside the unchanged hardware-tested package; the download release version is v0.1.1-pal-alpha.
5. For Ports, use the **Ports import function supplied by your OnionOS installation**, then choose **Paper Mario PAL** in Ports. The shortcut is **Roms/PORTS/Shortcuts/Paper Mario PAL.port**, the import target **App/PaperMarioPAL/launch.sh**, and the cover **Roms/PORTS/Imgs/Paper Mario PAL.png**. **Roms/PORTS/Games/** must exist; it is included as an empty directory. Do not put another ROM or game installation there. No global Emu configuration is supplied or replaced.
6. On first game startup, the app checks the ROM and creates the data it needs locally. Choose **English, German, French or Spanish** in the game. Both Apps and Ports enter the same application and use **App/PaperMarioPAL/data/**, the same ROM, regular saves and build-specific save-state location. The Ports entry is a forwarder, not a second game installation.
7. Updating an existing matching alpha installation uses its existing App/PaperMarioPAL folder. Merge the packaged files and retain **rom.z64** and **data/** in place; neither is included in the ZIP. No manual save migration or cache deletion is required by this packaging update. Exit a session before starting through the other menu.

## Troubleshooting

- Missing App: check folder nesting and all six files: pm64, recipes.bin, launch.sh, config.json, icon.png, full_resolution. Keep **pm64**, **launch.sh** and the **.port** shortcut executable (0755); ordinary files are 0644.
- Missing Ports entry/cover: check the exact PAL shortcut/cover names, the empty Games directory and the App launcher, then use your installed OnionOS Ports import function again. If Ports/import support itself is absent, consult the documentation for that OnionOS version; this ZIP does not install the system importer.
- Rejected ROM: verify location, size, Z64 byte order and SHA-1. Do not add a second ROM under Roms/PORTS. There are no ROM-download links here.
- First launch: allow local data creation to finish; ensure writable free space. Do not mix binaries, recipes, caches or states from the separate PR build.
- Different saves between entries: verify both launchers still target this same App folder. Do not move or overwrite existing saves to troubleshoot.

## Evidence and known limitations

The maintainer reports **6/6 PASS** for M14.51 on a real Miyoo Mini Plus: Apps/new icon, Ports import, cover, Ports start, shared saves, save/load and return to OnionOS. This was a **manual user hardware test**, not automated measurement or a full playthrough. The unchanged M14.39b base was previously tested through the prologue to the beginning of Chapter 1; its separate earlier save-state evidence does not imply new exhaustive DualMenu testing. The newer upstream PR has a different build.

Known, unfixed: narrow straight seams exposing the background on some large trees; Goombario's enlarged enemy display has two frame sides instead of four; a Star Piece incorrectly blinks instead of remaining statically visible while its shadow stays visible. See KNOWN-ISSUES.md, NOTICE.md and ARTWORK-PROVENANCE.md. The two images were generated with ChatGPT from text prompts, according to the maintainer; possible third-party character/logo rights remain a disclosed uncertainty expressly accepted by the publishing project operator (ARTWORK_PUBLISHER_DECISION: ACCEPTED), not externally cleared rights. See ARTWORK-PUBLISHER-DECISION.md. This document is a release-preparation draft, not evidence that v0.1.1-pal-alpha is already published.
