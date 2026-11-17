# HeartBlock yaw reset M3.1 hardware milestone

Status: hardware-tested on Miyoo Mini Plus; the user confirmed the post-refill heart looked satisfactory. This is a scene-specific visual observation, not a pixel-registered comparison or a general renderer validation.

## Change and cause

The M2.9 coverage candidate changes the software renderer's alpha rejection for the exact one-cycle `AA_ZB_TEX_EDGE` render mode. It approximates full geometric coverage with threshold 32 instead of 0x55, in `bench/kit/gfx_pc.c` and `bench/kit/gfx_soft.c`. This renderer approximation addresses coverage/alpha handling and does not change HeartBlock animation state.

M3.1 resets the two entries read by the Happy HeartBlock draws after healing finishes. In `entity_HeartBlockContent__anim_heal`, state 5 clears yaw slots 8 and 7, then sets `yawBufferPos` to zero. With that position the two lagged reads select those slots. Healing states 2–4 retain their original rotation updates. The change is in the canonical `game/patches/decomp.patch` and is guarded by `PM64_PORT`.

These are distinct mechanisms. The M3.1 hardware build includes the M2.9 renderer change, but the M3.0 and M3.1 captures use the same renderer baseline; the M3.1 delta under test is the post-heal yaw reset and its diagnostic identity.

## Hardware result

M3.0 reference: build `047C6674`, trace at game frame 1296 (map entry) measured `yaw2=0`, `yaw3=0`; after refill at frame 1618 measured `yaw2=270.203918`, `yaw3=240.203918`. The heart was reported persistently too bright after refill.

M3.1: build stamp `5AAEB27E`; map-entry capture at game frame 1862 and post-refill capture at frame 2328 both measured `yaw2=0`, `yaw3=0`. Both were complete single-swap RENDER-FOCUS captures with `status=sampled`, `count=1`, `dropped=0`. The user visually checked the Miyoo Mini Plus result and judged the heart satisfactory.

For both versions, `task_link=unverified` remains a diagnostic limitation. The visual result was not quantified with a pixel-aligned comparison.

## Provenance and retained evidence

- Decomp pin: `ee321921df2debdaae4c9c00426b6ea84f26adb5`.
- Canonical patch check: `git diff --binary <pin> -- .` exactly matches `game/patches/decomp.patch` (107,889 bytes).
- M3.1 binary SHA-256: `4441d09b6e5cec677d974612d268ceddb75b1a9db20921b8f9fe0df3d99cf872`.
- M3.1 AppOnly ZIP SHA-256: `b977fdfeed6f636ab8297130349fe2cb137752807b993398548c53e51cb9465c`.
- The binary hash was also checked against the `pm64` member inside that ZIP.
- The six files in each M3.0 and M3.1 capture set passed both local and remote SHA-256 manifests.
- The PAL release-gates record reports `PASS`; its fields `hardware_tested=false` and `publication_authorized=false` show that it records structural gates, not the hardware observation or permission to publish.

Original evidence remains outside the repository at `/home/daniel/projects/healing-block-heart-evidence/m30-yaw/` and `/home/daniel/projects/healing-block-heart-evidence/m31-yaw-reset/`. Do not copy or regenerate those originals as part of this source milestone.

## Commit and tag proposal

Suggested local milestone commit: `test(paper-mario): record M3.1 HeartBlock yaw hardware result`.

Suggested annotated tag: `m31-heart-yaw-reset-hw-5aaeb27e`, pointing at that milestone commit and carrying the build stamp and binary digest above. Keep the AppOnly ZIP, release-gates JSON, builds, ROMs, saves, and raw evidence out of Git.
