# Build and source provenance — v0.1.1-pal-alpha

This is a packaging update of the historical M14.39b-based alpha. No C source, ARM objects, gameplay, renderer, binary, recipe payload, buildstamp or cache identity changed. The exact hardware-tested DualMenu ZIP is copied without repacking, only renamed to PaperMarioPAL-0.1.1-pal-alpha-OnionOS.zip for a future release.

ZIP SHA-256: d65494e451820223279bc2fa442d1a165c861649bc0312a03ecb938e73430f31

pm64 SHA-256: 9efd71f707ef6297397bfed55f569c8fa1fc9b614b822a4135b2b13ebd508ed4

recipes.bin SHA-256: 55ab55c7698e93b8d64241e62d7261c5e4ca6e58f5e6bddb11e13648ddaf9aca

Buildstamp **18810FBA**, layout **0A0D92C4**, recipe CRC **ED95F0ED**, cache-ID **200078C8**. Identity is bound to the unchanged original program/recipes, not a fresh build. No new ROM extraction or cache reconstruction was run.

The original release source is **3666fbde3106f7215ecb9ab56cf8f1d0c4135203** on release/0.1-pal-alpha. A packaging/documentation source commit is prepared as its child on release/0.1.1-pal-alpha. Its exact object identity is recorded in the local final review; the proposed public version is identified by the future tag, avoiding a self-referential hash in its own tree. The future annotated tag must be **v0.1.1-pal-alpha**, targeting that new commit, not the old tag, main, docs/pal-alpha or PR head. The source profile must preserve all eight payload hashes and exact icon/cover bytes, rather than silently rerun the old generic icon/cover generator.

M14.51 reports six manual user hardware passes for this DualMenu package. The earlier M14.39b engine test scope extends to the beginning of Chapter 1. The separate upstream PR #7 is based on **9a867e0720744215b36a1fe9fa74ef994ffd243e** and the newer overlay architecture; its 8/8 test result is a different build. No new full-game or exhaustive save-state validation is implied.

Only currently present M14.41 release sources, M14.48 public notices and M14.50 compliance results were used after workspace cleanup. Missing historical paths were not replaced by unverified alternatives. The package includes no ROM or user runtime data. Artwork provenance is separate; see ARTWORK-PROVENANCE.md.

The source additions are the six exact frozen packaging files, their verified layout metadata and versioned public documents. Runtime C, renderer, buildgenerator and generic packager remain byte-identical to the parent. The profile is declarative, not a newly implemented generic-packager feature. The unchanged generic packager is not the release path and is not claimed to regenerate this ZIP. Publication uses the existing verified ZIP copy. Future builds or generic-packager enhancements need separate review.
