# Paper Mario PAL – 0.1.1-pal-alpha

An OnionOS **alpha pre-release** packaging update for the native ARM European PAL port on **Miyoo Mini Plus**. Supported game languages: **English, German, French and Spanish**.

## What changes

- Start the same game from **Apps or Ports**.
- New **74 × 74 App icon** and **250 × 360 Ports cover**.
- Ports import metadata points to the existing App/PaperMarioPAL application.
- One ROM, one program/recipe pair and shared App/PaperMarioPAL/data for saves and local runtime data. The original US App/PaperMario installation is not targeted.

## Ready-to-install ZIP and installation

**[Download PaperMarioPAL-0.1.1-pal-alpha-OnionOS.zip](https://github.com/SunnyBoon/miyoo64/releases/download/v0.1.1-pal-alpha/PaperMarioPAL-0.1.1-pal-alpha-OnionOS.zip)** when this release is published; no compilation required. Keep **NOTICE.md** and **THIRD-PARTY-LICENSES.txt** with the ZIP.

- [English](https://github.com/SunnyBoon/miyoo64/blob/v0.1.1-pal-alpha/docs/paper-mario-pal/v0.1.1-pal-alpha/INSTALLATION-EN.md)
- [Deutsch](https://github.com/SunnyBoon/miyoo64/blob/v0.1.1-pal-alpha/docs/paper-mario-pal/v0.1.1-pal-alpha/INSTALLATION-DE.md)
- [Français](https://github.com/SunnyBoon/miyoo64/blob/v0.1.1-pal-alpha/docs/paper-mario-pal/v0.1.1-pal-alpha/INSTALLATION-FR.md)
- [Español](https://github.com/SunnyBoon/miyoo64/blob/v0.1.1-pal-alpha/docs/paper-mario-pal/v0.1.1-pal-alpha/INSTALLATION-ES.md)

Merge App/ and Roms/ into the SD-card root. Preserve the exact App/PaperMarioPAL/ and Roms/PORTS/ paths. Supply your own European PAL ROM at **App/PaperMarioPAL/rom.z64**: **64 MiB, big-endian Z64**, SHA-1 **2111d39265a317414d359e35a7d971c4dfa5f9e1**. No ROM, extracted game-data payload, savegame or runtime cache is included. Use OnionOS's installed Ports import function, then choose Paper Mario PAL in Ports. Do not create a second ROM/game installation in Roms/PORTS/Games.

The actual App label inside the unchanged tested ZIP is still **Paper Mario PAL 0.1-pal-alpha**. The release version is **v0.1.1-pal-alpha**; this label difference is intentional preservation of the tested package.

## Hardware evidence

The maintainer reports **M14.51: 6/6 PASS**, manually tested on a real Miyoo Mini Plus: Apps start/new icon, Ports import, visible cover, Ports start, shared saves, save/load and return to OnionOS. This is user-reported hardware evidence, not an automated test or full playthrough. The unchanged M14.39b engine was previously played through the prologue to the beginning of Chapter 1. No new exhaustive late-scene or save-state validation is claimed.

## Unchanged engine and known issues

No gameplay, renderer or ARM-build changes. pm64: `9efd71f707ef6297397bfed55f569c8fa1fc9b614b822a4135b2b13ebd508ed4`. recipes.bin: `55ab55c7698e93b8d64241e62d7261c5e4ca6e58f5e6bddb11e13648ddaf9aca`. Buildstamp **18810FBA**, cache-ID **200078C8**.

Unfixed: narrow tree seams exposing the background; two instead of four frame sides in Goombario's enlarged enemy display; a Star Piece incorrectly blinks although it should remain statically visible, while its shadow remains visible. See [Known Issues](https://github.com/SunnyBoon/miyoo64/blob/v0.1.1-pal-alpha/docs/paper-mario-pal/v0.1.1-pal-alpha/KNOWN-ISSUES.md).

The separate [upstream PR #7](https://github.com/mr-finster/miyoo64/pull/7) has a different source/binary and its own earlier 8/8 result. This ZIP keeps the standalone historical alpha engine.

ZIP SHA-256: **d65494e451820223279bc2fa442d1a165c861649bc0312a03ecb938e73430f31**.

## Attribution and artwork disclosure

See [NOTICE.md](https://github.com/SunnyBoon/miyoo64/blob/v0.1.1-pal-alpha/docs/paper-mario-pal/v0.1.1-pal-alpha/NOTICE.md), [complete third-party license texts](https://github.com/SunnyBoon/miyoo64/blob/v0.1.1-pal-alpha/docs/paper-mario-pal/v0.1.1-pal-alpha/THIRD-PARTY-LICENSES.txt), [LICENSE](https://github.com/SunnyBoon/miyoo64/blob/v0.1.1-pal-alpha/docs/paper-mario-pal/v0.1.1-pal-alpha/LICENSE), [build/source provenance](https://github.com/SunnyBoon/miyoo64/blob/v0.1.1-pal-alpha/docs/paper-mario-pal/v0.1.1-pal-alpha/BUILD-AND-SOURCE-PROVENANCE.md) and [artwork provenance](https://github.com/SunnyBoon/miyoo64/blob/v0.1.1-pal-alpha/docs/paper-mario-pal/v0.1.1-pal-alpha/ARTWORK-PROVENANCE.md). The maintainer generated the new images in ChatGPT from text prompts without uploaded reference images. No affiliation with Nintendo or blanket relicensing is claimed.

The publishing project operator has accepted the documented remaining possible third-party character/logo rights uncertainty and chosen these unchanged ChatGPT-generated images: **ARTWORK_PUBLISHER_DECISION: ACCEPTED**. This is not an external rights clearance or blanket artwork license. Artwork provenance and all original component notices remain linked above.
