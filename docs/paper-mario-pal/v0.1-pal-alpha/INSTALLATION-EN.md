# Paper Mario PAL v0.1-pal-alpha — Installation (English)

For **Miyoo Mini Plus with OnionOS**. This is a ready-to-install native ARM port: **no compilation is required**. You need your own unmodified European PAL copy of Paper Mario. No ROM or extracted game assets are included, and no ROM download is provided.

## Download and install

1. On the [SunnyBoon/miyoo64 release page](https://github.com/SunnyBoon/miyoo64/releases), select **v0.1-pal-alpha** and download **PaperMarioPAL-0.1-pal-alpha-OnionOS.zip**. Verify its SHA-256 against the accompanying SHA256SUMS: `763c582d37bff03363a14b2d04fe0f3726f44354f3e1098bef6c83302ca4f794`. Also download and keep `LICENSE`, `NOTICE.md` and `THIRD-PARTY-LICENSES.txt` with the ZIP.
2. Extract the ZIP contents to the **root of the SD card**, preserving `App/PaperMarioPAL/`. The six app files must be together: `pm64`, `recipes.bin`, `launch.sh`, `config.json`, `icon.png`, `full_resolution`. Avoid an extra outer folder. If `App/PaperMarioPAL/` already exists, back it up and review the existing installation before replacing anything.
3. Supply your own **Paper Mario (Europe, PAL)** ROM as **`App/PaperMarioPAL/rom.z64`**. It must be a **64 MiB (67,108,864 bytes), big-endian Z64** file with SHA-1 **`2111d39265a317414d359e35a7d971c4dfa5f9e1`**. US/Japanese, modified or incorrectly ordered files do not match. Renaming a file does not convert its contents.
4. Put the SD card back in the Miyoo Mini Plus, open the application list in OnionOS and start **Paper Mario PAL 0.1-pal-alpha**.
5. On first startup, the app checks the ROM and creates the required data locally in **`App/PaperMarioPAL/data/`**. Wait for this to finish; no manual asset extraction or compilation is needed. Keep the ROM on the card for streamed data. Do not copy old caches or save states into this folder.
6. Use the game's language selection to choose **English, German, French or Spanish**. Follow the prompts shown by the game. Use this build's own regular saves and newly created save states; migration of states between builds is not supported.

## Troubleshooting

- **App missing or not starting:** check the exact folder nesting and all six app files. Keep `pm64` and `launch.sh` executable; the ZIP records 0755 for them and 0644 for the other files. Preserve permissions if your extraction tool asks. Use the complete matching ZIP.
- **ROM not found or rejected:** check the exact `rom.z64` location, size, big-endian format and SHA-1 above. There must not be a second filename extension. Use your own matching dump; no download link is supplied here.
- **First launch takes longer:** allow local data creation to finish. Ensure the SD card has free space and is writable. Do not interrupt a write or add another build's cache.
- **Cannot save/load, missing audio or persistent startup problem:** confirm the supported device/OnionOS, verify the ZIP and keep the original binary/recipes together. Keep your saves safe. Note the visible error and what you did for a support report; do not replace files with another build or share ROMs/saves/private data.

## Tested scope and known issues

The **exact end-user ZIP passed 5/5 manual user hardware smoke checks**: startup from this app folder, all four languages, main menu/new game, intro/first battle, regular save/load and save-state/load-state. Its M14.39b base was also played stably through the prologue to the beginning of Chapter 1. This is an **alpha**, not full-game validation; later scenes and all special abilities have not been exhaustively tested. Only the stated device/system is supported by this evidence.

Known, unfixed graphics issues:

- Some large trees have narrow straight segment seams through which the background is visible.
- Goombario's enlarged enemy display has an incomplete frame: two sides instead of four.
- A Star Piece incorrectly blinks although it should remain statically visible; its shadow remains visible while the Star Piece disappears.

The separate [upstream PR #7](https://github.com/mr-finster/miyoo64/pull/7) has its own **8/8** hardware result and different source/binary/recipes. That result does not replace this ZIP's **5/5** result. Do not mix the two installations or reuse their states/caches. Read the accompanying KNOWN-ISSUES.md and NOTICE.md.
