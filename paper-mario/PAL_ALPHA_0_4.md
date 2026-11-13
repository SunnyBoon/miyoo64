# Paper Mario PAL Alpha 0.4 release candidate

This branch prepares two ROM-free Onion OS packages from one source revision:

| Variant | Identity | App directory | Purpose |
|---|---|---|---|
| Standard | `PAL-0.4-STD` | `App/PaperMarioPAL/` | Normal gameplay, without trace instrumentation |
| Diagnostic | `PAL-0.4-DIAG` | `App/PaperMarioPALDiagnostic/` | The same gameplay/rendering fixes with the established manual trace capture |

Their ordinary game data are deliberately separate. Neither app imports, migrates, or synchronizes saves from another Paper Mario app.

## Included work

| Work | Source | Status in this branch |
|---|---|---|
| PAL identity, language and asset pipeline | existing PAL integration | included |
| TEXEL1 rebase used by Tattle | upstream `2d9ffae` | already upstream |
| Quizmo matrix correction | upstream `19ca173` | already upstream |
| Object rotation | upstream `9674e68` | already upstream |
| TEXEL1 SoftTex extent normalization | hardware-confirmed M1.4 work | `b688d9d` |
| Mirror/clamp and SoftTex sampler state | M18/M1.4 renderer work | `b688d9d`, `f23b572`, `f24ffa0` |
| Save-RAM path lifetime | hardware-confirmed SaveFix | `43cbef9` |

The SoftTex change is restricted to generic TEXEL1-only draws with a valid unit-1 SoftTex binding; all other normalization paths retain their previous basis. The historical M1.4 hardware observation was 984 alpha rejects / 0 writes before, and 517 rejects / 461 writes after for the affected ItemEntity trace. That evidence applies to its tested binary, not automatically to a newly linked Alpha 0.4 binary.

## Build and distribution constraints

The user supplies the canonical big-endian Paper Mario (Europe, PAL) ROM with SHA-1 `2111d39265a317414d359e35a7d971c4dfa5f9e1`. It is only a local, read-only build input. The packages contain the executable and numeric recipe table, never the ROM, `assets.bin`, extracted game assets, personal saves, or test evidence.

The Diagnostic binary starts from the same release commit and applies the tracked `miyoo/pal-alpha-0.4-diagnostic-overlay.patch` in an isolated build tree before compiling. The overlay is restricted to the pre-existing M1.7 trace capture, item/TEXEL1 observation, trace-file tee, and two menu entries; it has no Save-RAM, asset, game, or rendering-fix change. Its trace metadata identifies `PaperMarioPALDiagnostic`, `PAL-0.4-DIAG`, and the linked binary's runtime build stamp. The package manifest binds that binary to its SHA-256; no runtime hash is invented.

`tools/port/pal_build.py` produces a fresh asset/recipe pair for every final link. `tools/port/pal_release_gates.py` verifies the ELF, asset region, relocation targets, Recipe cache/stamp, package structure, and ROM-content leakage. A successful static gate is not a hardware acceptance.

## Diagnostic save-file handling

`pm64.sav` is ordinary 128 KiB Save-RAM, not an S6MP state. When a reporter sends one, archive and hash the original, inspect its size and header, assess PAL compatibility, and place only a working copy in the diagnostic app's isolated `data/` directory. A regular save does not promise an exact reported scene. Reject malformed or incompatible files; do not put untrusted saves into a personal gameplay installation or expose an upload endpoint.

## Hardware acceptance still required

Before a tag or GitHub release, test the exact Standard and Diagnostic ZIPs on a Miyoo Mini Plus. Both must boot, offer language selection, run gameplay, retain normal saves and manual save-states, and coexist with isolated data directories. The Diagnostic package additionally needs a real trace and readable logs. No release or tag is created by this branch preparation.
