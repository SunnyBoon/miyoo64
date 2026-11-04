# Distribution companions — v0.1.1-pal-alpha

This packaging edition keeps the original program and recipes unchanged. Keep NOTICE.md and THIRD-PARTY-LICENSES.txt with the ZIP; the exact port MIT notice is already inside its config.json and is also supplied in the repository as LICENSE. Following M14.50, do not remove the downloadable full license/notice companions merely to shorten an asset list.

The new menu images are user-reported ChatGPT generations from text prompts without uploaded image references. This description is not an independent third-party character/logo rights grant or an assertion that the art is MIT-licensed. ARTWORK_PUBLISHER_DECISION: ACCEPTED. The publishing operator explicitly chooses these unchanged images and accepts the documented remaining possible third-party rights uncertainty. This is not verified external consent or legal clearance. See ARTWORK-PROVENANCE.md and ARTWORK-PUBLISHER-DECISION.md. Original game/SDK/renderer/component notices retain their scope.

The following original core notice is preserved verbatim for the unchanged alpha binary. Its historical Icon bullet describes the previous v0.1 package artwork; the two current images have the separate provenance and publisher decision stated above. No license condition is removed or weakened:

# Notices and attribution — Paper Mario PAL v0.1-pal-alpha

Keep this file, LICENSE and THIRD-PARTY-LICENSES.txt with the downloaded ZIP. They are distribution companions; the hardware-tested six-file ZIP is unchanged. The complete component texts are included in THIRD-PARTY-LICENSES.txt, not merely linked to a website.

- **miyoo64 / native port:** Copyright (c) 2026 mr-finster. The exact upstream MIT notice is supplied as LICENSE and is also present in the app's config.json. It covers the project's own port code; it is not a blanket license for decompiled game/SDK or other third-party material. PAL integration history is recorded under SunnyBoon; original upstream authorship is retained.
- **Paper Mario decompilation:** pmret/papermario, pinned ee321921df2debdaae4c9c00426b6ea84f26adb5. Original contributor, Nintendo and SGI/SDK notices remain applicable where present in the source. No general MIT relicensing of that material is asserted. Paper Mario and game content remain the property of their respective rights holders. This unofficial port is not affiliated with or endorsed by Nintendo.
- **MK64/Beckerd port/tool/platform lineage:** the original MK64 Portable license credits David Becker (2026), scopes MIT to its own enumerated parts and distinguishes inherited material. The full original notice is included in the collection. Credits to that port do not transfer authorship of inherited renderer/audio code.
- **Fast3D/SM64-derived graphics:** Copyright (c) 2020 Emill, MaikelChan. The collection includes the conditional license from Emill/n64-fast3d-engine at 881eb68bad1150f720433d75bccb4d26879c28ab. It is not described as MIT. Older snapshot notices and later source attribution are retained in the source history, not silently replaced.
- **Irrlicht clipping excerpt:** the port retains “Copyright (C) 2002-2006 Nikolaus Gebhardt/Alten Thomas”. The collection separately includes the official Irrlicht license text (Copyright © 2002-2010 Nikolaus Gebhardt); this is not an assertion that the complete Irrlicht engine or its JPEG component is bundled.
- **Audio and shared kit:** upstream credits the SM64 mixer/scalar algorithms and MK64/Beckerd-derived platform/save-state lineage. Existing attribution is preserved. No unrelated renderer/audio changes or new authorship claims are part of this alpha staging.
- **Icon:** retained upstream-supplied package artwork; no claim of a newly created image or a ROM-extracted icon.
- **System libraries:** the ELF dynamically requires libSDL-1.2.so.0, libm.so.6, libpthread.so.0 and libc.so.6; these DSOs are supplied by the device/system and are not included in the ZIP. SDL 1.2 and glibc license information is represented by the complete GNU LGPL 2.1 text. The launcher preserves the existing LD_LIBRARY_PATH and can use system libraries or compatible replacements. An optional system libpadsp audio shim is not bundled; its platform notices remain its own. Exact device-provider versions are not asserted by these notices.
- **Compiler/startup runtime:** GCC-derived runtime helper attribution remains with the GNU contributors. GPLv3 and the GCC Runtime Library Exception 3.1 texts are included for the relevant runtime context; their inclusion does not place all port/game material under GPL. Other original startup/source notices remain preserved in the source tree.

No additional distribution terms here restrict modification for the user's own use or reverse engineering to debug such modifications in connection with the linked LGPL libraries. Nothing in these notices limits permissions supplied by the applicable component licenses. No library binaries, ROMs, extracted assets, saves or caches accompany this download.

Sources and upstream projects:

- https://github.com/mr-finster/miyoo64
- https://github.com/pmret/papermario/tree/ee321921df2debdaae4c9c00426b6ea84f26adb5
- https://github.com/Emill/n64-fast3d-engine/blob/881eb68bad1150f720433d75bccb4d26879c28ab/LICENSE.txt
- https://irrlicht.sourceforge.io/?page_id=294
- https://www.libsdl.org/license.php
- https://raw.githubusercontent.com/bminor/glibc/glibc-2.28/COPYING.LIB
- https://raw.githubusercontent.com/gcc-mirror/gcc/releases/gcc-8.3.0/COPYING.RUNTIME

No ROM download is offered. The binary/recipe output and the source-code PR have separate provenance; technical testing alone is not presented as a new legal opinion.
