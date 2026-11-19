# PAL v80 development configuration

Build in this checkout with `make PLAT=pc VERSION=pal_v80` or `make PLAT=miyoo VERSION=pal_v80`. The US v80 configuration remains the default. Output goes to `build/pc-pal-v80` or `build/miyoo-pal-v80`; the corresponding US paths remain unchanged. The supported ROM is Europe Rev 1, canonical SHA-1 `b7f628073237b3d211d40406aa0884ff8fdd70d5`. PAL v77 is not supported.

The PAL and US Miyoo ARM builds have both completed successfully in Docker. The resulting binaries were inspected as ELF32 ARM EABI5 hard-float executables with interpreter `/lib/ld-linux-armhf.so.3`; both require SDL 1.2, zlib, libm, libpthread and libc. The build flag stamps select the corresponding version and independent output directories. This is build verification only: no PAL ROM boot or hardware smoke test has been completed.

The PAL loader requires the matching ROM at every start. It verifies the whole normalized ROM before deriving the SHA-1 of its asset region, then verifies the PAL cache against that digest. The cache is not trusted on size alone. A 12 MiB ROM is expected, as in the existing loader; this assumption has not been verified with a PAL ROM here. `DKR_ROM` can name the ROM, or the normal search can find it.

All PAL data goes into a `pal-v80` subdirectory of `DKR_DATA` (or of the executable's `data` directory), including EEPROM, states, configuration, and the PAL-specific asset cache. Packaging uses `miyoo/game-pal-v80.json` and `App/DiddyKongRacingPAL`; run `VERSION=pal_v80 sh tools/build_miyoo.sh /path/to/canonical-pal.z64` after the ARM build. Its normal release checks still require the ROM and a tracked source tree. No package or hardware installation has been validated here.

PAL selects the game's 50 Hz TV path and 20 ms fields. The game's video initialization requests 320x264, whereas the port renderer is designed around 320x240; it has not been visually verified, and clipping or incorrect framing is possible. The renderer and asset swap logic are unchanged. PAL asset layouts, timing, audio, menus, save handling and hardware performance require tests with the verified ROM and device before this is considered playable.
