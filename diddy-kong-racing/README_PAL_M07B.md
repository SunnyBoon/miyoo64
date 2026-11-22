# PAL M0.7b viewport experiment

M0.7b is an isolated hardware A/B test, not a verified fix or a release. It
retains M0.7a's 320x264 PAL target and display scaling. The PAL one-player
viewport produced by `viewport_main` has raw x scale 640, x translation 624,
y scale 580 and y translation 508 (RSP quarter-pixel units). The renderer
normally maps it to x=[-4,316); this build moves that exact positive-scale
viewport to [0,320), before output scaling, without changing its width,
height or scissor. Other raw geometries, including ordinary split views and
the mirrored negative-scale viewport, are unchanged. An unrelated special
view with exactly the same four values will also be affected. As some HUD or
menu triangles use the same viewport, they can move by four source pixels;
texture rectangles bypass the viewport. The original camera, HUD and minimap
coordinates are not edited. The test may leave the edge unchanged if its
cause lies elsewhere; it does not horizontally stretch the framebuffer.

The test uses `App/DiddyKongRacingPALM07b`, its own `data/pal-v80-m07b`
subdirectory, and the separate Onion shortcut `Diddy Kong Racing PAL
M07b.port`. The other package launchers are also uniquely named. Do not copy
saves or save states from the other builds; each test needs independent game
progress and the matching Europe Rev 1 ROM. Do not install without approval.

Build and package in the existing toolchain container:

    make PLAT=miyoo VERSION=pal_v80 PAL264=1 M07B=1 -j3
    mkdir -p build/m07b-package
    cp build/miyoo-pal-v80-m07b/dkr build/m07b-package/dkr
    /opt/miyoomini-toolchain/usr/bin/arm-linux-gnueabihf-strip --remove-section=.comment build/m07b-package/dkr
    python3 miyoo/tools/miyoo_pack.py miyoo/game-pal-v80-m07b.json --build build/m07b-package -o build/DiddyKongRacingPALM07b-UNSCANNED.zip
    bash tools/check_m07b_package.sh /path/to/canonical-pal-rom.z64

The checker validates paths against the M0.6 and checked M0.7a archives,
source tracking, ELF/save-region properties, package metadata and the ROM
leak gate before creating the HARDWARE-TEST copy. The manifest must be tracked
or staged for the provenance gate; source tracking does not prove that a dirty
working tree produced the binary. The ROM is read only and is not packaged.

For A/B, compare the same title, options, hub and race scenes and matching
settings, HUD, minimap, split-screen, menus and performance. Capture the
existing `DKR_DUMP` and `DKR_PANEL_DUMP` outputs for the same static scene;
their frame numbers come from different counters and need not represent the
same moment. Check source columns 316–319 in the framebuffer dump and the
visible panel edge. A result on one scene does not prove all views are fixed.

## Build and checks (2026-11-22)

The M0.7a backup branch is `diddy-pal-m07a-backup` at
`31cfafa0da8bb6d30deebfc71c8e0bc3c8bba4f6`. The separate M0.7b working
branch is `diddy-pal-m07b-test`; its changes remain uncommitted. The M0.6 and
M0.7a packages, M0.6 reference binary, the M0.7a unstripped binary, and the
pre-experiment source snapshot are additionally preserved in
`build/m07a-preserved/` with `SHA256SUMS`.

The ARM build and `tools/raster_test.sh 10` passed (eight kinds, two seeds;
ARM QEMU against x86). A small standalone check of the actual viewport-match
predicate verified the intended coordinates and rejected representative split,
mirrored and reset viewports; it does not replace a running-game test. The
package checker passed ELF32/ARM/hard-float, exact stripping, `.game_bss`
NOBITS, no COPY relocations inside save-state regions, source tracking,
metadata, archive CRC and non-overlapping package files. No save-state
round-trip or hardware execution was performed.

The optimized scanner checked the supplied canonical ROM: 3359 inflated
assets, zero leak windows, 91 code-region windows (2.8 KiB against a 48 KiB
budget). Measured separately on the full M0.7b archive, peak RSS was 70012
KiB; the matching-ROM M0.6 and M0.7a scans also passed (91 and 94 code windows
respectively). The checked package contains no ROM.

- Packaged stripped `dkr` SHA-256:
  `a6213f470755c01d621127fcf85e51aa09dc77017b715df3ed4f09dc2e363951`
- `build/DiddyKongRacingPALM07b-HARDWARE-TEST.zip` SHA-256:
  `7710913f716a3d7a37bc9c9c63fa014423093107be0a36f3874543bd263d17f8`
- Preserved M0.7a hardware package SHA-256:
  `3e87a93a57c85d89c7405dee1670157efdf01d6fe6c1d7fe0ec3416cec82aa00`
