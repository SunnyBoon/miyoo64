# Paper Mario for the Miyoo Mini

A native port of Paper Mario, built from the [pmret/papermario](https://github.com/pmret/papermario) decompilation, with a software renderer that shares the 3D between the Mini's two cores. The decompilation is fetched at build time at a pinned commit; this folder holds the port and its patch. It runs on the Miyoo Mini family (Mini, Mini Plus, Mini v4, Mini Flip).

**No game data is included**, here or in the zip. You need your own ROM.

Download `PaperMario-miyoo.zip` from https://miyoo-n64.pages.dev, and install it as the [top-level README](../README.md#install) says.

## Your ROM

- **Paper Mario (USA)**
- SHA-1 `3837f44cda784b466c9a2d99df70d77c322b97a0`
- .z64, .v64 or .n64, renamed to `App/PaperMario/rom.z64` (the game also looks in `Roms/N64/` and anywhere on the card down to two folders deep)
- The first start builds the game data from your ROM (`App/PaperMario/data/assets.bin`, about 11 MB). The game also reads music, maps and sprites from the ROM while it runs, so keep it on the card.

## Controls

| Miyoo | Does |
|---|---|
| D-pad | Walk, menus |
| A | Jump, talk, confirm |
| B | Hammer, back |
| X | Partner's ability |
| Y | Spin dash |
| L1 | Items |
| R1 | Partners |
| L2 | N64 R |
| R2 or SELECT | Status bar |
| START | Pause |
| MENU | Game menu: save, load, exit |

In the menu, up and down pick an entry, left and right change the save slot, A or START selects, and B or MENU closes it. The power button saves the auto save on the way out; the next start offers RESUME GAME or NEW GAME.

## Settings

Optional: a text file `App/PaperMario/data/debug.cfg`, one `key=value` per line. The ones a player might use:

| Setting | Effect |
|---|---|
| `PM64_STRETCH=0` | keep the N64's black frame around the field and battle scenes |
| `PM64_THREADS=1` | draw on one core only (default `2`) |
| `PM64_CPUCLOCK=<MHz>` | the CPU clock while the game runs (default 1500 / 1800; `0` leaves it alone) |
| `PM64_FIRECHEAP=0` | Bowser's fire breath with all its flames, as on the N64 (slower) |
| `PM64_ROTATE=0` | the picture unturned, for an SDL that turns the panel itself |

One `PM64_NAME=value` per line (`#` starts a comment).

## Building the Miyoo zip

Needs Docker (`./dev.sh` runs in `pm64hh:build`, `docker/Dockerfile`, built on `mk64hh:build` from `../mario-kart-64/docker/Dockerfile`, which has the Miyoo Mini toolchain at `/opt/miyoomini-toolchain`), the decomp image `pm64hh:decomp` (`docker/Dockerfile.decomp`), and your ROM at this folder's root as `Paper Mario (USA).z64`:

```sh
docker build --platform linux/amd64 -t pm64hh:decomp -f docker/Dockerfile.decomp docker
sh tools/port/decomp_patch.sh apply      # refs/papermario: pmret/papermario at the pinned commit + game/patches/decomp.patch
cp "Paper Mario (USA).z64" refs/papermario/ver/us/baserom.z64
docker run --rm -v "$PWD":/work -w /work/refs/papermario pm64hh:decomp ./configure us   # the decomp's split, from your ROM
docker run --rm -v "$PWD":/work -w /work/refs/papermario pm64hh:decomp ninja generated_code_us inc_img_bins_us   # the generated headers and image includes the game side reads
docker run --rm -v "$PWD":/work -w /work pm64hh:decomp python3 tools/port/rom_segments.py
./dev.sh 'sh tools/port/miyoo.sh'        # PaperMario-miyoo.zip
```

`tools/port/miyoo.sh` builds the game, derives new recipes from the ROM only if the asset layout differs from the shipped ones (numbers only), and runs the release gates: the decomp is the pinned commit plus exactly `game/patches/decomp.patch`, every other compiled source is tracked, no game data in the executable, only the game's files in the zip, the leak scan, no host paths. `NO_LEAK_SCAN=1` without a ROM writes an `-UNSCANNED` zip, which is not a release.

**A card-ready copy for yourself**, with your ROM inside as `App/PaperMario/rom.z64`:

```sh
./dev.sh 'PERSONAL=1 sh tools/port/miyoo.sh'
```

It writes `PaperMario-miyoo-PERSONAL.zip`. It contains your ROM, so keep it to yourself.

## ROM segment metadata (US and PAL)

`tools/port/rom_segments.py` reads only the selected region's
`refs/papermario/ver/<region>/splat.yaml`; it never opens a ROM.
With Python and PyYAML available, run from this folder:

```sh
python3 tools/port/rom_segments.py                 # US: build/game/rom_segments.json (unchanged)
python3 tools/port/rom_segments.py --region us     # explicit US, same output
python3 tools/port/rom_segments.py --region pal    # PAL: build/game/pal/rom_segments.json
python3 tools/port/rom_segments.py --region pal -o /tmp/pal-segments.json
python3 -B -m unittest discover -s tools/port -p test_rom_segments.py -v
```

Custom relative output paths are relative to the working directory. Keep custom
US and PAL destinations separate; PAL is explicitly forbidden from targeting the
US default path, including resolved symlink aliases. Naming, JSON fields, segment
order and the final `rom_end: null` are unchanged. Missing or `auto` starts are
skipped; numeric starts must be nonnegative integers and must not decrease.
Shared starts (BSS-only segments with zero ROM length) retain their original order.
Invalid input or write failures preserve an existing result through atomic file
replacement. Existing file symlinks (including relative targets and chains) stay
intact: the final referenced regular file is replaced atomically in its own
directory. A dangling file symlink creates its target if that target's parent
already exists; missing target directories, loops and nonregular targets are
rejected. The PAL guard compares the final resolved output and US target paths.
New files use `0666 & ~umask`; existing targets retain their POSIX permission bits.
Temporary files never start with permissions broader than the requested mode.

Path resolution and replacement are not protected against concurrent directory
or symlink changes. Do not retarget paths or run competing writers during
generation; this is a local build tool, not a filesystem security boundary.
Atomic replacement creates a new inode: ownership, ACLs and other hard-link
aliases are not preserved. File synchronization does not guarantee directory
entry durability across power loss. Tests use synthetic configurations in
temporary directories only.
This generates metadata only: `gen_build.py` and the game still support US only.

## Layout

- `game/port/`: the port: ROM access, audio, the N64 OS layer, the renderer glue
- `game/patches/decomp.patch`: the port's changes to the decompilation, against the pinned commit
- `kit/`: the shared platform kit (menu, save states, the panel, the second core, the CPU clock)
- `tools/port/`: the build (`gen_build.py`, `miyoo.sh`), the release gates and tests
- `tools/assets/`: the asset recipes (numbers only) and the leak scan
- `bench/`: the renderer benchmark used during development
- `README_MIYOO.md`: the Miyoo build in full (every switch, how the second core works, what to check on a new OS)

## Reporting a bug

Open an issue in this repository and attach `App/PaperMario/data/console.log` from the run that went wrong (the logs of earlier runs are kept next to it).

## Credits

Built on:

- [pmret/papermario](https://github.com/pmret/papermario), the Paper Mario decompilation (fetched at build time at a pinned commit);
- the platform kit shared with the other ports here.

Not affiliated with or endorsed by Nintendo or any other rights holder. You need your own copy of the game.
