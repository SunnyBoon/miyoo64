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
The segment command generates metadata only; it does not build or run the game.

## Native build graph generation (US and PAL)

`gen_build.py [pc|handheld|miyoo] --region us|pal` selects the PMRET region.
The default platform remains `pc`, the default region `us`. Existing invocations
and US-generated file contents and paths are preserved. The generator only reads
source/build metadata; it does not open a ROM, invoke a compiler, or start Docker.

```sh
python3 tools/port/gen_build.py                       # existing US desktop graph
python3 tools/port/gen_build.py handheld --region us  # existing US handheld graph
python3 tools/port/gen_build.py pc --region pal
python3 tools/port/gen_build.py handheld --region pal
python3 tools/port/gen_build.py miyoo --region pal
python3 -B -m unittest discover -s tools/port -p 'test_*build.py' -v
python3 -B -m unittest discover -s tools/port -p test_rom_segments.py -v
```

Inputs must already exist in `refs/papermario` at the port's pin `ee321921d`
plus `game/patches/decomp.patch`. PAL uses `ver/pal/papermario.ld`, selected
`ver/pal/build/...` objects from the shared `build.ninja`, `ver/pal/undefined_syms.txt`,
`ver/pal/build/include`, and `assets/pal`, including its generated effect headers.
Shared sources such as `src/effects.yaml` and `src/effect_shims.yaml` stay shared.
The game-side definitions become `VERSION=pal` and `VERSION_PAL`; host-side
platform definitions stay as before. PAL diagnostic segment classes use generic
`fixed`/`follow`/`effect` categories rather than US-specific VRAM window labels.

The corresponding PMRET split and the `generated_code_pal` and `inc_img_bins_pal`
prerequisites must have completed first. This includes generated C files, headers,
image includes and `message_ids.h`/`ld_addrs.h`. The generator checks prerequisite
file existence without reading generated binary data. It consumes
`build/game/pal/rom_segments.json` from `rom_segments.py --region pal` and refuses
a table without the four PAL language segments or native segment ranges.
Required language/code ranges must have a known end, fit in the 32-bit ROM address
space and contain ROM bytes. Duplicate PAL segment names are rejected. Zero-length
native BSS ranges remain supported for the pinned units `heaps`, `heaps2`, `heaps3`,
`heaps4`, `texture_memory` and `model_clear_render_tasks`: both the segment name
and its source path must match the pinned BSS-only unit. A `type: bss` tag or a
BSS-like name alone is insufficient. Other nonrequired zero-length BSS entries
remain valid. Additional BSS-only native units would need an explicit review.
It does not fall back to US inputs. Mixed regions, missing object rules, empty
native unit lists, malformed effect headers and unsupported symbolic linker
section addresses abort with an error. Numeric, omitted and NOLOAD section
addresses are supported; arbitrary linker expressions are not evaluated here.
The actual PAL split's linker output still needs validation once it is available.

All PAL artifacts are isolated under `build/game/pal/`: the platform Ninja files,
`pc_game.mk`/`handheld_game.mk`, `segments.json`, `port_effects.c`, `port_segtab.c`,
`rom_syms.ld`, asset/dontsave linker scripts, build stamps, platform object trees,
dependency/response files, executables and asset ELF links. Miyoo reuses only the
PAL handheld game objects. Ninja's `builddir` also isolates its logs/dependency
cache there. Existing stamps are preserved. US paths remain under `build/game/`.
PAL paths resolving outside their own directory are rejected, including symlink
aliases of US artifacts. Do not concurrently retarget output paths or run competing
generators; these checks are not a filesystem security boundary.

Before rendering and again before publication, every planned output is resolved
through relative links and symlink chains. Different logical outputs must have
different final targets, and no output may target a required input. For PAL,
known US artifact paths and default Ninja logs are protected as well. Existing
hardlink aliases are detected by `(st_dev, st_ino)` and conservatively rejected
between outputs or between outputs and protected files. Atomic replacement alone
would normally break a hardlink rather than change the old inode, but future
compiler/log writes need not do so. No generated binary is opened for these checks.
All final publication destinations are fixed before the first replacement.

Checks cover the declared graph and required inputs; they cannot guard aliases
created after validation, concurrent renames, or arbitrary build commands run
elsewhere. Files outside the declared input/output and known US artifact sets
are not inventoried; future compiler dependencies still need separate validation.
Paths and file identities must remain stable during generation/build.

All generator inputs are checked before rendering. Files are rendered in a
private staging directory, then replaced individually, with build entry points
published last. Input/rendering failures leave previous artifacts intact. This
is not a transaction across the entire file set: a disk/permission failure during
publication can leave a mixed set and requires a successful rerun before building.
The command then reports failure and does not print the final success message.
Regression tests inject a failure at the second replacement and a permission
failure during publication; detected validation collisions publish nothing.

At `ee321921d`, PAL Splat, PAL source branches, language menus, message segments
and region-aware generated-code rules already exist. The upstream PAL Splat
still refers to US auto-symbol files in two options; those are versioned upstream
inputs to the split, not substitutes for this generator's PAL symbol selection.
No upstream update is required for this graph-generation step. Tests cover only
synthetic fixtures, including frozen US output hashes from the original generator.
Those references cover generated contents and paths for `pc`, `handheld` and
`miyoo` on the synthetic fixtures. They detect changes relative to the original
implementation, but do not establish parser coverage for every real PMRET linker
script, complete dependency closure, native compilation or game correctness.

**A generated PAL graph is not a verified PAL executable.** A complete build still
needs the user's explicitly provided PAL ROM for the split/generated data, the
Docker/toolchain environment, native compile/link validation, PAL runtime endian
and timing fixes, PAL asset recipes, and later controlled device validation.
The existing `miyoo.sh` packaging pipeline remains US-only and is not changed here.

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
