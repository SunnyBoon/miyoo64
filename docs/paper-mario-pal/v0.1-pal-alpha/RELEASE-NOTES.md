# Paper Mario PAL – 0.1-pal-alpha

An **alpha pre-release of the native ARM European PAL port** for **Miyoo Mini Plus with OnionOS**. Supported game languages: **English, German, French and Spanish**.

## Ready-to-install OnionOS download

**[Download PaperMarioPAL-0.1-pal-alpha-OnionOS.zip](https://github.com/SunnyBoon/miyoo64/releases/download/v0.1-pal-alpha/PaperMarioPAL-0.1-pal-alpha-OnionOS.zip) — no compilation required.**

Keep the accompanying **[NOTICE.md](https://github.com/SunnyBoon/miyoo64/releases/download/v0.1-pal-alpha/NOTICE.md)** and **[THIRD-PARTY-LICENSES.txt](https://github.com/SunnyBoon/miyoo64/releases/download/v0.1-pal-alpha/THIRD-PARTY-LICENSES.txt)** with the ZIP. They remain downloadable distribution companions; the program's own complete MIT notice is already inside the ZIP and also [in the repository](https://github.com/SunnyBoon/miyoo64/blob/docs/pal-alpha/docs/paper-mario-pal/v0.1-pal-alpha/LICENSE).

## Installation instructions

- **[English](https://github.com/SunnyBoon/miyoo64/blob/docs/pal-alpha/docs/paper-mario-pal/v0.1-pal-alpha/INSTALLATION-EN.md)**
- **[Deutsch](https://github.com/SunnyBoon/miyoo64/blob/docs/pal-alpha/docs/paper-mario-pal/v0.1-pal-alpha/INSTALLATION-DE.md)**
- **[Français](https://github.com/SunnyBoon/miyoo64/blob/docs/pal-alpha/docs/paper-mario-pal/v0.1-pal-alpha/INSTALLATION-FR.md)**
- **[Español](https://github.com/SunnyBoon/miyoo64/blob/docs/pal-alpha/docs/paper-mario-pal/v0.1-pal-alpha/INSTALLATION-ES.md)**

See the **[documentation overview](https://github.com/SunnyBoon/miyoo64/blob/docs/pal-alpha/docs/paper-mario-pal/v0.1-pal-alpha/README.md)** for installation, troubleshooting, checksums and provenance. Extract the unchanged ZIP to the SD-card root, preserving **App/PaperMarioPAL/**.

## Your own European PAL ROM is required

Supply your own **Paper Mario (Europe, PAL)** ROM as **App/PaperMarioPAL/rom.z64**: canonical **64 MiB (67,108,864 bytes), big-endian Z64**, SHA-1 **2111d39265a317414d359e35a7d971c4dfa5f9e1**. No ROM, extracted game assets or ROM download is included. Required runtime data is created locally on first startup.

## Confirmed hardware scope and limitations

This **exact end-user ZIP passed 5/5 manual user hardware checks** on Miyoo Mini Plus/OnionOS: start from the final app folder, all four PAL languages, main menu/new game, intro/first battle, regular save/load and save-state/load-state. The underlying **M14.39b** build was played stably through the prologue to the **beginning of Chapter 1**. These are user-reported tests, not automated measurements; full-game stability and later scenes are not established.

Known, **unfixed** graphics issues:

- Narrow straight segment seams on some large trees expose the background.
- Goombario's enlarged enemy display has an incomplete frame: two sides instead of four.
- A Star Piece incorrectly blinks although it should remain statically visible; its shadow remains visible while the Star Piece disappears.

Read **[Known Issues](https://github.com/SunnyBoon/miyoo64/blob/docs/pal-alpha/docs/paper-mario-pal/v0.1-pal-alpha/KNOWN-ISSUES.md)**. Keep existing data safe; do not mix binary/recipe pairs, caches or save states between builds.

## Integrity, source and upstream PR

ZIP SHA-256:

`763c582d37bff03363a14b2d04fe0f3726f44354f3e1098bef6c83302ca4f794`

Release source: [3666fbde3106f7215ecb9ab56cf8f1d0c4135203](https://github.com/SunnyBoon/miyoo64/commit/3666fbde3106f7215ecb9ab56cf8f1d0c4135203); unchanged annotated tag **v0.1-pal-alpha**. Binary buildstamp **18810FBA**, layout **0A0D92C4**, recipe CRC **ED95F0ED**, cache-ID **200078C8**. See **[Build and Source Provenance](https://github.com/SunnyBoon/miyoo64/blob/docs/pal-alpha/docs/paper-mario-pal/v0.1-pal-alpha/BUILD-PROVENANCE.md)** and **[original release checksums](https://github.com/SunnyBoon/miyoo64/blob/docs/pal-alpha/docs/paper-mario-pal/v0.1-pal-alpha/original-release/SHA256SUMS)**.

The separate **[upstream PAL PR #7](https://github.com/mr-finster/miyoo64/pull/7)** uses the newer general overlay architecture and its own **8/8** manual hardware result. It is a different source/binary version; this download remains the standalone M14.39b-based alpha.

## License and attribution

**[LICENSE](https://github.com/SunnyBoon/miyoo64/blob/docs/pal-alpha/docs/paper-mario-pal/v0.1-pal-alpha/LICENSE)** · **[Notices and credits](https://github.com/SunnyBoon/miyoo64/blob/docs/pal-alpha/docs/paper-mario-pal/v0.1-pal-alpha/NOTICE.md)** · **[Complete third-party license texts](https://github.com/SunnyBoon/miyoo64/blob/docs/pal-alpha/docs/paper-mario-pal/v0.1-pal-alpha/THIRD-PARTY-LICENSES.txt)**

Credits include mr-finster/miyoo64, PMRET and original game contributors/rightsholders, MK64/Beckerd and SM64-derived components, Emill/MaikelChan, the attributed Irrlicht clipping authors, SDL and GNU runtime contributors. MIT covers the project's own port code, not all inherited game/SDK/third-party material. Original attribution is retained; no blanket relicensing or affiliation with Nintendo is claimed. Library-use notices and complete license copies remain available as the two companion release downloads above.
