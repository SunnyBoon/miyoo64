# Build/source provenance — v0.1-pal-alpha

Release source branch: release/0.1-pal-alpha. Expected source commit: 3666fbde3106f7215ecb9ab56cf8f1d0c4135203. Planned tag: v0.1-pal-alpha, explicitly targeting that release commit; main and the PR head are different and must not be used accidentally.

The program/recipes were built and paired in M14.39b from the hardwarevalidated M14.36 lineage with the verified selective gameplay patch. That build recompiled five affected units (four gameplay consumers plus stamp) and reused 3,391 unchanged objects only after the documented source/header/compiler dependency closure check; 3,396 objects were linked. Do not confuse this with the later PR's 3,396-unit fresh build.

M14.41 assembled the release-source commit and packaged the exact existing pair for App/PaperMarioPAL. Its source audit binds the decomp and port inputs to M14.39b; packaging/build-plan/documentation metadata was added. The supplied ZIP is a tested repackaging of that exact pair, not a new build from the later release metadata. M14.48 only verifies and copies the original ZIP, without repacking or recompiling.

pm64 SHA-256: 9efd71f707ef6297397bfed55f569c8fa1fc9b614b822a4135b2b13ebd508ed4

recipes.bin SHA-256: 55ab55c7698e93b8d64241e62d7261c5e4ca6e58f5e6bddb11e13648ddaf9aca

ZIP SHA-256: 763c582d37bff03363a14b2d04fe0f3726f44354f3e1098bef6c83302ca4f794

Buildstamp 18810FBA; layout 0A0D92C4; recipe CRC ED95F0ED; cache-ID 200078C8. The shipped ELF has a NOBITS asset region. Prior complete reconstruction and leak gates are bound to the unchanged identities; no new ROM/cache access in staging.

Pinned game source: pmret/papermario ee321921df2debdaae4c9c00426b6ea84f26adb5 plus release game/patches/decomp.patch. Documented offline build image sha256:073270ce6da0489cfc139bfad75a6302e6931d63290ea1f5d6af52b4cfc8fe2b; decomp/provider image sha256:0c42a871ebcd90da2a7cde0ce981fd02dbd53683f9ff771c86af7b21ad1b0289. The recorded generators, flags, source/closure hashes and recipe process establish provenance. No fresh rebuild from the final tag, nor independent bit-for-bit reproduction from that tag, is asserted. A newly executed build normally gets its own stamp/pair and needs its own verification.

The separate public source PR is https://github.com/mr-finster/miyoo64/pull/7 with tested head 9a867e0720744215b36a1fe9fa74ef994ffd243e. Its M14.44/M14.45 pair and 8/8 result do not replace the alpha pair or its own 5/5 end-user smoke result. Game/save-state compatibility between those builds is not offered.

Original source licenses and attribution remain in their original scope; use the companion notices. No ROM, game-asset payload, save, cache or private log is included in the release set.
