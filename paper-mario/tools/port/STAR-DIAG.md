# PM-PAL-GFX-003 star-piece trace

Set `PM64_STAR_DIAG=1` before launching. Lines go to the normal port log
(`data/console.log` on Miyoo). The trace is disabled by default. It logs every
visible or hidden star-piece slot visited, so extended runs can produce large logs.

`frame` is the port's game-frame counter; `task` is a 24-bit serial assigned at
queue time. `task=0` means no identity was assigned. Match the item, queued,
append, draw_begin, triangles and raster lines by task. A queued task without
append was not observed executing; an append without draw_begin was not observed
by the display-list interpreter. Missing logs or an `error`/`untracked` line
make such inferences incomplete. Task serials can be reused after a savestate
restore; retain frame and surrounding log context.

The animation index is the image number 0,1,2,1 from the four-command
STAR_PIECE script. `anim_step` is its 0..3 sequence position;
`anim_next_offset` is the byte offset of the next script command. Index -1
means the script position was not one of the four expected positions.
Raster and palette indices are cache lookup indices, not ROM addresses.

The markers surround the body display-list call for either item size, excluding
other tasks and shadows. Marker boundaries flush buffered triangles. `early`
counts early front-end drops; `cull_clip` counts winding/clip drops; `emitted`
counts output triangles after clipping, which can exceed interpreted triangles.
`calls` counts triangles entering the scalar/vector rasterizer, and pixel
counts refer to the scalar path. `complete=0` means a vector row or the special
depth-only path was used and pixel counts are incomplete. To request scalar
rows for a future diagnostic run, set `PM64_CMNEON=0` at process start; the
special depth-only path still reports incomplete. Writes are framebuffer stores,
not a guarantee that a later draw leaves those pixels visible. Instrumentation
and forced flushes can change performance.

The host tests are `test_star_diag_task.py`, `star_diag_dl_test.c`, and
`star_diag_raster_test.c`. The task test extracts the real task wrapper and
checks delayed/reordered execution; the DL fixture links unexercised renderer
symbols as unresolved and only tests its isolated marker/early-reject path;
the raster fixture drives the real scalar rasterizer. None is an ARM build or
an end-to-end hardware validation.
