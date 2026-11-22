#!/usr/bin/env python3
"""Exercise the actual queued-task wrapper with delayed and reordered calls."""
from pathlib import Path
import subprocess
import tempfile

root = Path(__file__).resolve().parents[2]
src = (root / 'refs/papermario/src/item_entity.c').read_text()
start = src.index('#ifdef PM64_PORT\n/* The queued RenderTask owns')
end = src.index('\nvoid appendGfx_item_entity(void* data) {', start)
block = src[start:end]
# The two possible body DLs must each be surrounded by marker commands.
body = src[end:src.index('\nvoid draw_item_entities(void)', end)]
for name in ('D_8014C678', 'D_8014C6A0'):
    at = body.index('gSPDisplayList(gMainGfxPos++, ' + name + ')')
    before, after = body[:at], body[at:]
    assert '0xA3000000u | sStarDrawing->id' in before[-300:]
    assert '0xA4000000u | sStarDrawing->id' in after[:300]
assert 'gDPNoOpTag' not in body[body.index('draw_coin_sparkles'):]

prefix = r'''
#include <assert.h>
#include <stdarg.h>
#include <stdio.h>
#define PM64_PORT 1
typedef unsigned int u32;
typedef struct { float x, y, z; } Vec;
typedef struct ItemEntity { int n; int itemID, flags; Vec pos; int *readPos; unsigned nextUpdate, lookupRasterIndex, lookupPaletteIndex; } ItemEntity;
static int script[32]; static int *gItemEntityScripts[] = { script };
typedef struct RenderTask { void *appendGfxArg; void (*appendGfx)(void *); } RenderTask;
static void *gMainGfxPos;
unsigned int port_frame = 10;
int pm64_star_diag_enabled(void) { return 1; }
void port_log(const char *fmt, ...) { (void)fmt; }
void appendGfx_item_entity(void *);
'''
suffix = r'''
static unsigned ids[4], frames[4]; static int slots[4], calls;
void appendGfx_item_entity(void *p) {
    assert(sStarDrawing && sStarDrawing->item == p);
    ids[calls] = sStarDrawing->id; frames[calls] = sStarDrawing->frame;
    slots[calls++] = sStarDrawing->slot;
}
int main(void) {
    ItemEntity a = { .n = 1, .readPos = script + 6 }, b = { .n = 2, .readPos = script + 12 }; RenderTask t1 = {0}, t2 = {0}, t3 = {0};
    star_diag_queue(&t1, &a, 3);
    port_frame = 11; star_diag_queue(&t2, &a, 3);
    star_diag_queue(&t3, &b, 9); /* deliberately not executed */
    port_frame = 12; t2.appendGfx(t2.appendGfxArg); t1.appendGfx(t1.appendGfxArg);
    assert(calls == 2 && ids[0] == 2 && ids[1] == 1);
    assert(frames[0] == 11 && frames[1] == 10 && slots[0] == 3 && slots[1] == 3);
    assert(sStarDrawing == 0 && ((StarDiagTask *)t3.appendGfxArg)->id == 3);
    puts("star_diag_task_test: PASS (delayed, reordered, missing task; body markers)");
}
'''
with tempfile.TemporaryDirectory() as tmp:
    c = Path(tmp) / 'test.c'; exe = Path(tmp) / 'test'
    c.write_text(prefix + block + suffix)
    subprocess.run(['gcc', '-std=c11', '-Wall', '-Wextra', '-Werror', '-Wno-pointer-to-int-cast', str(c), '-o', str(exe)], check=True)
    subprocess.run([str(exe)], check=True)
