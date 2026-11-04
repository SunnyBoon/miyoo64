# Paper Mario PAL v0.1-pal-alpha

Ready-to-install native ARM alpha for **Miyoo Mini Plus with OnionOS**. No compilation required. Supported game languages: **English, German, French, Spanish**.

## Download and installation

**[Download PaperMarioPAL-0.1-pal-alpha-OnionOS.zip](https://github.com/SunnyBoon/miyoo64/releases/download/v0.1-pal-alpha/PaperMarioPAL-0.1-pal-alpha-OnionOS.zip)**

| Installation guide | Language |
| --- | --- |
| [INSTALLATION-EN.md](INSTALLATION-EN.md) | English |
| [INSTALLATION-DE.md](INSTALLATION-DE.md) | Deutsch |
| [INSTALLATION-FR.md](INSTALLATION-FR.md) | Français |
| [INSTALLATION-ES.md](INSTALLATION-ES.md) | Español |

Supply your own European PAL ROM at **App/PaperMarioPAL/rom.z64**: **64 MiB**, big-endian Z64, SHA-1 **2111d39265a317414d359e35a7d971c4dfa5f9e1**. No ROM or extracted game assets are supplied. Follow the appropriate guide, preserve App/PaperMarioPAL/ and let the app create local runtime data at first startup.

ZIP SHA-256: **763c582d37bff03363a14b2d04fe0f3726f44354f3e1098bef6c83302ca4f794**.

## Documentation and distribution companions

- [Release Notes](RELEASE-NOTES.md)
- [Known Issues](KNOWN-ISSUES.md)
- [Build and Source Provenance](BUILD-PROVENANCE.md)
- [LICENSE](LICENSE), [NOTICE.md](NOTICE.md), [complete third-party licenses](THIRD-PARTY-LICENSES.txt)
- [Current documentation checksums](SHA256SUMS)
- [Original M14.48 release-asset manifest](original-release/RELEASE-MANIFEST.json) and [original release checksums](original-release/SHA256SUMS)
- [Original release-note text](original-release/RELEASE-NOTES.txt), preserved byte-for-byte as a historical record; its former attachment links are archival text, not current installation links. Use the current Release Notes above.

Keep the ZIP with the downloadable **[NOTICE.md](https://github.com/SunnyBoon/miyoo64/releases/download/v0.1-pal-alpha/NOTICE.md)** and **[THIRD-PARTY-LICENSES.txt](https://github.com/SunnyBoon/miyoo64/releases/download/v0.1-pal-alpha/THIRD-PARTY-LICENSES.txt)**. These two files stay alongside the binary to preserve the existing library-use notices and complete license accompaniment. The exact MIT text in [LICENSE](LICENSE) is already inside the unchanged ZIP's config.json; save an additional copy from this repository if following the original guide's instruction to keep LICENSE with the ZIP. SDL 1.2 and the GNU C library are system-provided shared libraries, not bundled DSOs. This distribution must not restrict modification for the user's own use or reverse engineering for debugging such modifications with those libraries.

The installation guides and other original public documents are copied without content changes. Their references to accompanying documentation now resolve through this directory. SHA256SUMS binds this current documentation tree; original-release/ records the original M14.48 publication set and does not describe the later documentation commit or reduced attachment list. The ZIP is downloaded separately and its hash is printed above and in the original checksums.

## Tested scope

The exact alpha ZIP passed **5/5 manual user hardware checks**. Its M14.39b base was played through the prologue to the **beginning of Chapter 1**. This is not full-game validation. The separate [upstream PR #7](https://github.com/mr-finster/miyoo64/pull/7) has a different build and its own **8/8** result. Known graphics defects remain; see Known Issues.

Release source/tag remain **3666fbde3106f7215ecb9ab56cf8f1d0c4135203 / v0.1-pal-alpha**. This branch adds documentation only. It does not rebuild, repackage or change the program, recipes, release source branch, release tag or PR.
