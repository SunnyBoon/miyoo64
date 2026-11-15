/* The frame for the native port (M2): the game's display lists go to the M1 renderer (bench/kit, without
 * PM64_REPLAY), and a finished frame is shown. Host side (system headers and the kit's). On the desktop proxy a
 * frame is written as a PPM when PM64_SHOTS=<dir> is set, as a square (the area-filtered 4->3 squeeze the
 * bench uses); the device presents through SDL 1.2 (port_sdl.c). */
#include <stdbool.h>
#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <errno.h>
#include <PR/ultratypes.h>
#include <PR/gbi.h>
#include "gfx_pc.h"
#include "gfx_window_manager_api.h"
#include "gfx_rendering_api.h"
#include "port.h"
#include "savestate.h"
#include "boss_diag.h"
#include "animator_sample.h"

#define PANEL 240

extern struct GfxRenderingAPI gfx_soft_api;
extern uint32_t *gfx_soft_get_framebuffer(int *w, int *h);
extern uint16_t gFrameBuf0[], gFrameBuf1[], gFrameBuf2[];
extern const uint32_t port_build_stamp;

float identity_matrix[4][4] = { { 1, 0, 0, 0 }, { 0, 1, 0, 0 }, { 0, 0, 1, 0 }, { 0, 0, 0, 1 } };

/* the window manager: the port presents by itself, after each game frame */
static void wm_init(const char *n, bool fs) { (void) n; (void) fs; }
static void wm_set_kb(bool (*a)(int), bool (*b)(int), void (*c)(void)) { (void) a; (void) b; (void) c; }
static void wm_set_fs_cb(void (*cb)(bool)) { (void) cb; }
static void wm_set_fs(bool e) { (void) e; }
static void wm_main_loop(void (*f)(void)) { (void) f; }
static void wm_dims(uint32_t *w, uint32_t *h) { *w = 320; *h = 240; }
static void wm_events(void) {}
static bool wm_start(void) { return true; }
static void wm_swap(void) {}
static double wm_time(void) { return port_time_us() / 1e6; }
static struct GfxWindowManagerAPI sWapi = { wm_init, wm_set_kb, wm_set_fs_cb, wm_set_fs, wm_main_loop, wm_dims,
                                            wm_events, wm_start, wm_swap, wm_swap, wm_time };

/* one 240-px panel row from one 320-px row: every 4 columns -> 3 (3:1, 2:2, 1:3), as the bench's fit_row */
static void fit_row(uint32_t *dst, const uint32_t *src, int sw) {
    if (sw != 320) { memcpy(dst, src, PANEL * 4); return; }
    for (int i = 0; i < 80; i++, src += 4, dst += 3) {
        const uint32_t a = src[0], b = src[1], c = src[2], d = src[3];
#define CH(p, sh) (((p) >> (sh)) & 255u)
#define MIX(p, q, wp, wq) ((((CH(p, 16) * wp + CH(q, 16) * wq + 2) >> 2) << 16) | (((CH(p, 8) * wp + CH(q, 8) * wq + 2) >> 2) << 8) | ((CH(p, 0) * wp + CH(q, 0) * wq + 2) >> 2) | 0xFF000000u)
        dst[0] = MIX(a, b, 3, 1); dst[1] = MIX(b, c, 2, 2); dst[2] = MIX(c, d, 1, 3);
#undef MIX
#undef CH
    }
}

/* The square picture is a rectangle of the 320x240 frame resampled to the square:
 *   the whole frame (0, 0, 320, 240) -- the squeeze, fit_row, as always;
 *   during the intro logos the middle (40, 0, 240, 240) 1:1, widening to the whole frame with the last logo's fade;
 *   in the field the scene inside its black frame (12, 20, 296, 200), stretched (port_main.c). A change between the
 *   whole frame and the stretch (a battle, the curtains, a map without the frame) moves the rectangle over 8 frames,
 *   so the picture zooms rather than jumps. PM64_STRETCH=0 keeps the black frame, PM64_LOGOCROP=0 the logos squeezed.
 * Other than the squeeze and the copy, a panel pixel is a bilinear sample (7-bit weights, rounded) of the source at
 * its centre: across first, each source row once (a two-row cache of planar rows), then down. The device does the same
 * integer math in NEON (port_panel_565); this plain C makes the desktop's and the tests' frames, pixel for pixel the
 * same (PM64_PANELCHECK=1 compares the two on the device build). The stretch's first version averaged areas with up to
 * three taps across in plain C: 2.0 M ARM instructions a frame against the squeeze's 0.3 M (qemu, dgb_09). */
static uint16_t sHL[PANEL] SAVESTATE_EXCLUDE; /* the left source column */
static uint8_t sHW[PANEL] SAVESTATE_EXCLUDE;  /* the right one's weight, 0..127 of 128 */
static uint8_t sHIdx[PANEL] SAVESTATE_EXCLUDE; /* sHL less its group of 8's first (NEON's table lookups) */
static uint16_t sVR[PANEL] SAVESTATE_EXCLUDE; /* the upper source row */
static uint8_t sVW[PANEL] SAVESTATE_EXCLUDE;  /* the lower one's weight, 0..127 of 128 */
static float sRect[4] SAVESTATE_EXCLUDE = { -1, -1, -1, -1 };
static void rect_tables(const float r[4]) {
    const float kx = r[2] / PANEL, ky = r[3] / PANEL;
    for (int x = 0; x < PANEL; x++) {
        float c = r[0] + (x + 0.5f) * kx - 0.5f;
        if (c < 0) c = 0;
        if (c > 318) c = 318;
        int l = (int) c, w = (int) ((c - l) * 128.0f + 0.5f);
        if (w >= 128) { l++; w = 0; }
        sHL[x] = (uint16_t) l; sHW[x] = (uint8_t) w;
    }
    for (int x = 0; x < PANEL; x++) sHIdx[x] = (uint8_t) (sHL[x] - sHL[x & ~7]);
    for (int y = 0; y < PANEL; y++) {
        float c = r[1] + (y + 0.5f) * ky - 0.5f;
        if (c < 0) c = 0;
        if (c > 238) c = 238;
        int t = (int) c, w = (int) ((c - t) * 128.0f + 0.5f);
        if (w >= 128) { t++; w = 0; }
        sVR[y] = (uint16_t) t; sVW[y] = (uint8_t) w;
    }
    memcpy(sRect, r, sizeof sRect);
}
static inline uint32_t lerp7(uint32_t a, uint32_t b, uint32_t w) { return (a * (128 - w) + b * w + 64) >> 7; }
/* a source row across, into planar r, g, b */
static void rect_hrow(uint8_t *pr, uint8_t *pg, uint8_t *pb, const uint32_t *src) {
    for (int x = 0; x < PANEL; x++) {
        const uint32_t a = src[sHL[x]], b = src[sHL[x] + 1], w = sHW[x];
        pr[x] = (uint8_t) lerp7((a >> 16) & 255, (b >> 16) & 255, w);
        pg[x] = (uint8_t) lerp7((a >> 8) & 255, (b >> 8) & 255, w);
        pb[x] = (uint8_t) lerp7(a & 255, b & 255, w);
    }
}

static int logos_view(void) { /* 0 = 1:1, 255 = the whole frame (port_main.c) */
    static int on = -1;
    if (on < 0) { const char *e = getenv("PM64_LOGOCROP"); on = !(e && e[0] == '0'); }
    return on ? port_logos_view() : 255;
}

enum { PANEL_SQUEEZE, PANEL_COPY, PANEL_RECT, PANEL_OTHER };
static const uint32_t *sPanelFb SAVESTATE_EXCLUDE;
static int sPanelW SAVESTATE_EXCLUDE, sPanelH SAVESTATE_EXCLUDE, sPanelMode SAVESTATE_EXCLUDE;
static float sStretchT SAVESTATE_EXCLUDE = -1; /* 0 = the whole frame, 1 = the stretch */
static uint8_t sHRow[2][3][PANEL] __attribute__((aligned(16))) SAVESTATE_EXCLUDE; /* planar r, g, b */
static int sHRowSrc[2] SAVESTATE_EXCLUDE;
void port_panel_begin(const uint32_t *fb, int w, int h) {
    sPanelFb = fb; sPanelW = w; sPanelH = h;
    sHRowSrc[0] = sHRowSrc[1] = -1;
    if (w != 320 || h != 240) { sPanelMode = PANEL_OTHER; return; }
    const float target = port_letterbox_standard() ? 1.0f : 0.0f;
    if (sStretchT < 0) sStretchT = target;
    else if (sStretchT < target) sStretchT = sStretchT + 0.125f > target ? target : sStretchT + 0.125f;
    else if (sStretchT > target) sStretchT = sStretchT - 0.125f < target ? target : sStretchT - 0.125f;
    const int view = logos_view();
    float r[4];
    if (view < 255) { /* the logos: (40, 0, 240, 240) widening to the whole frame */
        const float t = view / 255.0f;
        r[0] = 40 * (1 - t); r[1] = 0; r[2] = 240 + 80 * t; r[3] = 240;
    } else { /* the whole frame, or the scene inside its black frame */
        const float t = sStretchT;
        r[0] = 12 * t; r[1] = 20 * t; r[2] = 320 - 24 * t; r[3] = 240 - 40 * t;
    }
    if (r[0] == 0 && r[1] == 0 && r[2] == 320 && r[3] == 240) { sPanelMode = PANEL_SQUEEZE; return; }
    if (r[0] == 40 && r[1] == 0 && r[2] == 240 && r[3] == 240) { sPanelMode = PANEL_COPY; return; }
    if (memcmp(r, sRect, sizeof r) != 0) rect_tables(r);
    sPanelMode = PANEL_RECT;
}
int port_panel_plain(void) { return sPanelMode == PANEL_SQUEEZE; }
int port_panel_rect(void) { return sPanelMode == PANEL_RECT; }
static uint8_t (*hrow(int r))[PANEL] { /* source row r across (planar), from the two-row cache */
    const int slot = r & 1;
    if (sHRowSrc[slot] != r) {
        rect_hrow(sHRow[slot][0], sHRow[slot][1], sHRow[slot][2], sPanelFb + (size_t) r * sPanelW);
        sHRowSrc[slot] = r;
    }
    return sHRow[slot];
}
void port_panel_row(uint32_t *dst, int y) {
    const uint32_t *src = sPanelFb + (size_t) (y < sPanelH ? y : sPanelH - 1) * sPanelW;
    switch (sPanelMode) {
        case PANEL_OTHER: memcpy(dst, src, PANEL * 4); return;
        case PANEL_SQUEEZE: fit_row(dst, src, sPanelW); return;
        case PANEL_COPY: memcpy(dst, src + 40, PANEL * 4); return;
    }
    uint8_t (*a)[PANEL] = hrow(sVR[y]), (*b)[PANEL] = hrow(sVR[y] + 1);
    const uint32_t w = sVW[y];
    for (int x = 0; x < PANEL; x++)
        dst[x] = 0xFF000000u | lerp7(a[0][x], b[0][x], w) << 16 | lerp7(a[1][x], b[1][x], w) << 8 | lerp7(a[2][x], b[2][x], w);
}

#if defined(__ARM_NEON) || defined(__ARM_NEON__)
#include <arm_neon.h>
/* The device's rectangle (PANEL_RECT): the same integer math as port_panel_row, 8 panel pixels at a time, straight to
 * RGB565 (the row stride in pixels). Across, a group of 8 panel pixels reads at most 11 source pixels (the rectangle is
 * 240..320 wide), so one de-interleaving load of 16 and table lookups gather them. */
static void neon_hrow(uint8_t *pr, uint8_t *pg, uint8_t *pb, const uint32_t *src) {
    const uint8x8_t k128 = vdup_n_u8(128), one = vdup_n_u8(1);
    for (int x = 0; x < PANEL; x += 8) {
        const int base = sHL[x] > 304 ? 304 : sHL[x]; /* 16 pixels from base stay inside the row */
        const uint8x16x4_t p = vld4q_u8((const uint8_t *) (src + base));
        uint8x8_t il = vld1_u8(&sHIdx[x]);
        if (base != sHL[x]) il = vadd_u8(il, vdup_n_u8((uint8_t) (sHL[x] - base)));
        const uint8x8_t ir = vadd_u8(il, one), w = vld1_u8(&sHW[x]), nw = vsub_u8(k128, w);
        uint8_t *const out[3] = { pr, pg, pb };
        for (int c = 0; c < 3; c++) {
            const uint8x16_t q = p.val[2 - c]; /* memory order b, g, r, a */
            const uint8x8x2_t t = { { vget_low_u8(q), vget_high_u8(q) } };
            const uint16x8_t acc = vmlal_u8(vmull_u8(vtbl2_u8(t, il), nw), vtbl2_u8(t, ir), w);
            vst1_u8(out[c] + x, vrshrn_n_u16(acc, 7));
        }
    }
}
void port_panel_565(uint16_t *dst, int stride) {
    static uint8_t cache[2][3][PANEL] __attribute__((aligned(16)));
    int src_of[2] = { -1, -1 };
    const uint8x8_t k128 = vdup_n_u8(128);
    for (int y = 0; y < PANEL; y++, dst += stride) {
        uint8_t (*ab[2])[PANEL];
        for (int k = 0; k < 2; k++) {
            const int r = sVR[y] + k, slot = r & 1;
            if (src_of[slot] != r) { neon_hrow(cache[slot][0], cache[slot][1], cache[slot][2], sPanelFb + (size_t) r * sPanelW); src_of[slot] = r; }
            ab[k] = cache[slot];
        }
        const uint8x8_t w = vdup_n_u8(sVW[y]), nw = vsub_u8(k128, w);
        for (int x = 0; x < PANEL; x += 8) {
            const uint8x8_t r = vrshrn_n_u16(vmlal_u8(vmull_u8(vld1_u8(ab[0][0] + x), nw), vld1_u8(ab[1][0] + x), w), 7);
            const uint8x8_t g = vrshrn_n_u16(vmlal_u8(vmull_u8(vld1_u8(ab[0][1] + x), nw), vld1_u8(ab[1][1] + x), w), 7);
            const uint8x8_t b = vrshrn_n_u16(vmlal_u8(vmull_u8(vld1_u8(ab[0][2] + x), nw), vld1_u8(ab[1][2] + x), w), 7);
            const uint16x8_t px = vorrq_u16(vorrq_u16(vandq_u16(vshll_n_u8(r, 8), vdupq_n_u16(0xF800)),
                                                      vshrq_n_u16(vandq_u16(vshll_n_u8(g, 8), vdupq_n_u16(0xFC00)), 5)),
                                            vshrq_n_u16(vshll_n_u8(b, 8), 11));
            vst1q_u16(dst + x, px);
        }
    }
}
#endif

static const char *sShotDir;
static uint32_t sShot, sShotEvery = 1; /* PM64_SHOTS_EVERY=<n>: every nth frame only (the map sweep) */
static void save_ppm(void) {
    char path[600];
    int w, h;
    /* PM64_SHOTS_GAMEFRAME=1: named and picked (every nth) by the game frame (port_frame), not by the shown frame: the
     * frames a slow host leaves undrawn differ from run to run, so two runs' shots then pair by name (renderer A/Bs) */
    static int byGame = -1;
    if (byGame < 0) { const char *e = getenv("PM64_SHOTS_GAMEFRAME"); byGame = e && e[0] == '1'; }
    const uint32_t n = byGame ? port_frame : sShot;
    sShot++;
    if (n % sShotEvery != 0) return;
    const uint32_t *fb = gfx_soft_get_framebuffer(&w, &h);
    snprintf(path, sizeof path, "%s/frame_%05u.ppm", sShotDir, n);
    FILE *f = fopen(path, "wb");
    if (!f) { port_log("gfx: cannot write %s\n", path); return; }
    static int raw = -1; /* PM64_SHOTS_RAW=1: the renderer's own frame (320x240), before the panel's squeeze */
    if (raw < 0) { const char *e = getenv("PM64_SHOTS_RAW"); raw = e && e[0] == '1'; }
    const int ow = raw ? w : PANEL;
    port_panel_begin(fb, w, h);
    fprintf(f, "P6\n%d %d\n255\n", ow, h);
    uint32_t row[640];
    for (int y = 0; y < h; y++) {
        if (ow == w) memcpy(row, fb + y * w, (size_t) w * 4);
        else port_panel_row(row, y);
        for (int x = 0; x < ow; x++) { const uint8_t px[3] = { row[x] >> 16, row[x] >> 8, row[x] }; fwrite(px, 3, 1, f); }
    }
    fclose(f);
}

void port_gfx_init(void) {
    sShotDir = getenv("PM64_SHOTS");
    const char *every = getenv("PM64_SHOTS_EVERY");
    if (every && atoi(every) > 0) sShotEvery = (uint32_t) atoi(every);
    gfx_init(&sWapi, &gfx_soft_api, "Paper Mario", false);
    port_log("gfx: renderer ready%s%s\n", sShotDir ? ", frames to " : "", sShotDir ? sShotDir : "");
}

/* Paper Mario's depth queries (model.c is_point_visible / is_model_center_visible, decomp.patch): the N64 copies one
 * pixel of nuGfxZBuffer into DepthCopyBuffer[id] with a texture rectangle drawn into that buffer, and the next frame
 * decodes it (DepthFloatLookupTable) and compares it with the point's own depth: in front, the bulb glow (the star rod's
 * bloom, Watt, the lightning's flash, the lantern ghost) is drawn. This renderer keeps its depth in its own buffer, so
 * that memory never held a depth and every glow tested as hidden. Now the query is noted while the list is built
 * and answered when the frame is drawn, from the renderer's depth in the RDP's format: 18-bit z (0..32704 x 8 for
 * z/w * 0.5 + 0.5 = 0..1, the viewport's G_MAXZ range) as a 3-bit exponent and 11-bit mantissa, dz 0.
 * PM64_DEPTHQUERY=0: the old way (the texrect, which never wrote that memory). */
int gfx_soft_depth_at(int x, int y);
static struct { uint16_t *dst; int x, y; } sDq[16] SAVESTATE_EXCLUDE;
static int sDqN SAVESTATE_EXCLUDE;
int32_t port_depth_query(uint16_t *dst, int32_t x, int32_t y) {
    static int on SAVESTATE_EXCLUDE = -1;
    if (on < 0) { const char *e = getenv("PM64_DEPTHQUERY"); on = !(e && e[0] == '0'); }
    if (!on) return 0;
    if (sDqN < 16) { sDq[sDqN].dst = dst; sDq[sDqN].x = x; sDq[sDqN].y = y; sDqN++; }
    return 1;
}
static uint16_t n64_depth_encode(int z16) {
    static const uint32_t bias[8] = { 0, 0x20000, 0x30000, 0x38000, 0x3C000, 0x3E000, 0x3F000, 0x3F800 };
    static const uint8_t shift[8] = { 6, 5, 4, 3, 2, 1, 0, 0 };
    const uint32_t z18 = (uint32_t) ((uint64_t) (uint32_t) z16 * (32704u * 8u) / 65535u);
    int e = 7;
    while (e > 0 && z18 < bias[e]) e--;
    uint32_t m = (z18 - bias[e]) >> shift[e];
    if (m > 0x7FF) m = 0x7FF;
    return (uint16_t) (e << 13 | m << 2);
}
static void depth_query_resolve(void) {
    for (int i = 0; i < sDqN; i++) {
        const int z = gfx_soft_depth_at(sDq[i].x, sDq[i].y);
        if (z >= 0) *sDq[i].dst = n64_depth_encode(z);
    }
    sDqN = 0;
}

/* M2.2 has one bounded capture context.  It is entirely SAVESTATE_EXCLUDE: a
 * load never restores a half-open host file or stale renderer pointer. */
enum { PM64_DIAG_RENDER_FOCUS = 1, PM64_DIAG_TIMELINE = 2, PM64_DIAG_RAW_COMPAT = 3 };
enum { PM64_DIAG_MAX_RECORDS = 768, PM64_DIAG_TIMELINE_SWAPS = 360, PM64_DIAG_TEX_HISTORY = 16 };
enum { PM64_DIAG_READY, PM64_DIAG_ARMED, PM64_DIAG_RECORDING, PM64_DIAG_BUSY, PM64_DIAG_SAVED, PM64_DIAG_FAILED };
static struct {
    int armed, active, stop_requested;
    int event_start_pending, event_end_pending, event_end_wait;
    struct PM64EventSnapshot event_start, event_end;
    unsigned mode, state, seq, records, swap_start, game_start, last_game;
    unsigned bind_records, hit_records, texrect_records, critical_records, dropped_records;
    unsigned reload_generation, reload_game, reload_swap, cache_epoch;
    unsigned baseline_seq, baseline_reload_generation, baseline_game, last_swap;
    unsigned import_records, rect_records, history_head, history_count;
    struct { char kind[16]; struct PM64DiagTextureInfo info; } history[PM64_DIAG_TEX_HISTORY];
    char dir[512], group[32], role[32], reason[32];
    uint8_t seen_hits[512];
    unsigned last_bind[2];
    FILE *log;
} sDiag SAVESTATE_EXCLUDE;
int pm64_diag_event_pending SAVESTATE_EXCLUDE;
static struct { unsigned count; struct PM64ProbeRow rows[PM64_PROBE_LIMIT]; } sProbe SAVESTATE_EXCLUDE;
/* Merlee fix candidate: bounded observations, written only at TIMELINE end. */
static struct { unsigned count, dropped, next; struct { unsigned frame; int state, card, phase, alpha, width, height, x, y, result; } row[64]; } sMerlee SAVESTATE_EXCLUDE;
void port_merlee_sample(int state, int card, int phase, int alpha, int width, int height, int x, int y, int result) {
    if (!sDiag.active || sDiag.mode != PM64_DIAG_TIMELINE) return;
    unsigned i = sMerlee.next++ % 64;
    if (sMerlee.count < 64) sMerlee.count++; else sMerlee.dropped++;
    sMerlee.row[i].frame = port_frame;
    sMerlee.row[i].state = state; sMerlee.row[i].card = card; sMerlee.row[i].phase = phase;
    sMerlee.row[i].alpha = alpha; sMerlee.row[i].result = result;
    sMerlee.row[i].width = width; sMerlee.row[i].height = height; sMerlee.row[i].x = x; sMerlee.row[i].y = y;
}
int pm64_probe_active(void) { return sDiag.active && sDiag.mode == PM64_DIAG_TIMELINE && sProbe.count < PM64_PROBE_LIMIT; }
void pm64_probe_store(const struct PM64ProbeRow *row) {
    if (!pm64_probe_active() || !row) return;
    sProbe.rows[sProbe.count] = *row;
    sProbe.rows[sProbe.count].call = sProbe.count + 1;
    sProbe.count++;
}
static void pm64_probe_value(FILE *f, const struct PM64ProbeValues *v) {
    if (!v->valid) { fputs("null", f); return; }
    fprintf(f, "{\"id\":%d,\"ptrCurLine\":\"0x%llx\",\"ptrNextLine\":\"0x%llx\",\"curOpcode\":%d,\"stateFlags\":%d,\"frameCounter\":", v->id, (unsigned long long)v->cur, (unsigned long long)v->next, v->opcode, v->flags);
    if (isfinite(v->counter)) fprintf(f, "%.9g", (double)v->counter); else fputs("null", f);
    fputs(",\"timeScale\":", f);
    if (isfinite(v->scale)) fprintf(f, "%.9g", (double)v->scale); else fputs("null", f);
    fprintf(f, ",\"LVar1\":%d,\"LVarE\":%d,\"LVarF\":%d}", v->lvar1, v->lvare, v->lvarf);
}
static void pm64_probe_write(void) {
    char path[600]; snprintf(path, sizeof path, "%s/scheduler_probe.json", sDiag.dir);
    FILE *f = fopen(path, "w"); if (!f) return;
    fprintf(f, "{\"schema\":1,\"limit\":16,\"scheduler_reached\":%s,\"samples\":[", sProbe.count ? "true" : "false");
    for (unsigned i = 0; i < sProbe.count && i < PM64_PROBE_LIMIT; i++) {
        const struct PM64ProbeRow *r = &sProbe.rows[i];
        const char *reasons[] = { "none", "debug_no_update", "id_changed", "flags", "insufficient_frame_counter", "multiple_targets" };
        fprintf(f, "%s{\"scheduler_call\":%u,\"present\":%s,\"reached\":%s,\"invoked\":%s,\"error_before_target\":%s,\"skip_reason\":\"%s\",\"before\":", i ? "," : "", r->call, r->present ? "true":"false", r->reached ? "true":"false", r->invoked ? "true":"false", r->error_before ? "true":"false", r->reason >= 0 && r->reason < 6 ? reasons[r->reason] : "unknown");
        pm64_probe_value(f, &r->before); fputs(",\"after\":", f); pm64_probe_value(f, &r->after); fputc('}', f);
    }
    fputs("]}\n", f); fclose(f);
}
extern int gfx_trace_frames;
extern void port_trace_file(FILE *f);

const char *pm64_diag_mode_name(unsigned mode) {
    switch (mode) {
        case PM64_DIAG_RENDER_FOCUS: return "RENDER FOCUS";
        case PM64_DIAG_TIMELINE: return "TIMELINE";
        case PM64_DIAG_RAW_COMPAT: return "RAW COMPAT";
        default: return "UNKNOWN";
    }
}
const char *pm64_diag_state_name(void) {
    switch (sDiag.state) {
        case PM64_DIAG_ARMED: return "ARMED";
        case PM64_DIAG_RECORDING: return "RECORDING";
        case PM64_DIAG_BUSY: return "BUSY";
        case PM64_DIAG_SAVED: return "SAVED";
        case PM64_DIAG_FAILED: return "FAILED";
        default: return "READY";
    }
}
int pm64_diag_request(unsigned mode) {
    if (mode < PM64_DIAG_RENDER_FOCUS || mode > PM64_DIAG_RAW_COMPAT) {
        sDiag.state = PM64_DIAG_FAILED; snprintf(sDiag.reason, sizeof sDiag.reason, "bad_mode"); return 0;
    }
    if (sDiag.armed || sDiag.active || sDiag.event_end_pending || gfx_trace_frames) {
        sDiag.state = PM64_DIAG_BUSY; snprintf(sDiag.reason, sizeof sDiag.reason, "capture_active"); return 0;
    }
    sDiag.mode = mode;
    sDiag.armed = 1;
    sDiag.stop_requested = 0;
    sDiag.state = PM64_DIAG_ARMED;
    snprintf(sDiag.reason, sizeof sDiag.reason, "armed");
    return 1;
}
/* A second A on an active timeline is an explicit, safe finish request. */
int pm64_diag_stop_request(void) {
    if (!sDiag.active || sDiag.mode != PM64_DIAG_TIMELINE) return 0;
    sDiag.stop_requested = 1;
    sDiag.state = PM64_DIAG_BUSY;
    snprintf(sDiag.reason, sizeof sDiag.reason, "stop_requested");
    return 1;
}
/* Compatibility for existing out-of-tree menu patches. */
int pm64_qtrace_request(void) { return pm64_diag_request(PM64_DIAG_RAW_COMPAT); }
int pm64_qtrace_request_profile(unsigned profile) { (void) profile; return pm64_diag_request(PM64_DIAG_RENDER_FOCUS); }

#if defined(PM64_MERLEE_FIX_TEST)
#define DIAG_SESSION_PREFIX "merlee-test"
#define DIAG_APP_NAME "PaperMarioPALMerleeFixTest"
#define DIAG_BUILD_NAME "PAL-0.4.5-MERLEE-FIX-TEST"
#elif defined(PM64_VINE_FIX_M26)
#define DIAG_SESSION_PREFIX "m26"
#define DIAG_APP_NAME "PaperMarioPALAlpha045VineFixM26"
#define DIAG_BUILD_NAME "PAL-0.4.5-VINE-FIX-M26"
#elif defined(PM64_VINE_FIX_M25)
#define DIAG_SESSION_PREFIX "m25"
#define DIAG_APP_NAME "PaperMarioPALAlpha045VineFixM25"
#define DIAG_BUILD_NAME "PAL-0.4.5-VINE-FIX-M25"
#elif defined(PM64_ANIMATOR_DIAG_M24B)
#define DIAG_SESSION_PREFIX "m24b"
#define DIAG_APP_NAME "PaperMarioPALAlpha045AnimatorDiagM24B"
#define DIAG_BUILD_NAME "PAL-0.4.5-ANIMATOR-DIAG-M24B"
#elif defined(PM64_BOSS_DIAG_M24)
#define DIAG_SESSION_PREFIX "m24a"
#define DIAG_APP_NAME "PaperMarioPALAlpha045BossDiagM24"
#define DIAG_BUILD_NAME "PAL-0.4.5-BOSS-DIAG-M24"
#else
#define DIAG_SESSION_PREFIX "m221"
#define DIAG_APP_NAME "PaperMarioPALAlpha045DiagM221"
#define DIAG_BUILD_NAME "PAL-0.4.5-DIAG-M221"
#endif
static void diag_write_metadata(uint32_t swap, const char *status, const char *reason, int screenshot) {
    char path[600];
    snprintf(path, sizeof path, "%s/metadata.json", sDiag.dir);
    FILE *m = fopen(path, "w");
    if (!m) return;
    fprintf(m, "{\"diag_schema\":4,\"trace_id\":%u,\"diag_mode\":\"%s\",\"session_id\":\"%s-%08u\",\"comparison_group\":\"%s\",\"capture_role\":\"%s\",\"app\":\"%s\",\"build\":\"%s\",\"build_stamp\":\"%08X\",\"swap_frame_start\":%u,\"swap_frame_end\":%u,\"game_frame_start\":%u,\"game_frame_end\":%u,\"reload_generation\":%u,\"reload_game_frame\":%u,\"cache_epoch\":%u,\"records\":%u,\"dropped_records\":%u,\"trace\":\"trace.log\",\"framebuffer\":%s,\"capture_status\":\"%s\",\"completion_reason\":\"%s\"}\n",
            sDiag.seq, pm64_diag_mode_name(sDiag.mode), DIAG_SESSION_PREFIX, sDiag.seq, sDiag.group, sDiag.role, DIAG_APP_NAME, DIAG_BUILD_NAME,
            (unsigned)port_build_stamp, sDiag.swap_start, swap, sDiag.game_start, (unsigned)port_frame,
            sDiag.reload_generation, sDiag.reload_game, sDiag.cache_epoch, sDiag.records, sDiag.dropped_records,
            screenshot ? "\"frame.ppm\"" : "null", status, reason);
    fclose(m);
}
static int diag_begin(uint32_t swap) {
    const char *base = getenv("PM64_TRACE_DIR");
    if (!base || !*base) { snprintf(sDiag.reason, sizeof sDiag.reason, "no_trace_dir"); return 0; }
    for (int i = 0; i < 100000; i++) {
        unsigned n = ++sDiag.seq;
        if (snprintf(sDiag.dir, sizeof sDiag.dir, "%s/trace_%06u", base, n) >= (int)sizeof sDiag.dir) { snprintf(sDiag.reason, sizeof sDiag.reason, "path_too_long"); return 0; }
        if (mkdir(sDiag.dir, 0755) == 0) break;
        if (errno != EEXIST || i == 99999) { snprintf(sDiag.reason, sizeof sDiag.reason, "mkdir_failed"); return 0; }
    }
    char path[600]; snprintf(path, sizeof path, "%s/trace.log", sDiag.dir);
    sDiag.log = fopen(path, "wx");
    if (!sDiag.log) { snprintf(sDiag.reason, sizeof sDiag.reason, "log_open_failed"); return 0; }
    setvbuf(sDiag.log, NULL, _IOLBF, 0);
    port_trace_file(sDiag.log);
    sDiag.active = 1; sDiag.armed = 0; sDiag.stop_requested = 0; sDiag.records = 0;
    sDiag.bind_records = sDiag.hit_records = sDiag.texrect_records = sDiag.critical_records = sDiag.dropped_records = 0;
    memset(sDiag.seen_hits, 0, sizeof sDiag.seen_hits); sDiag.last_bind[0] = sDiag.last_bind[1] = ~0u;
    sDiag.state = PM64_DIAG_RECORDING;
    sDiag.swap_start = swap; sDiag.last_swap = swap; sDiag.game_start = port_frame; sDiag.last_game = port_frame;
    if (sDiag.mode == PM64_DIAG_RENDER_FOCUS) {
        if (sDiag.baseline_seq && sDiag.reload_generation == sDiag.baseline_reload_generation + 1 &&
            port_frame >= sDiag.reload_game && port_frame - sDiag.reload_game <= 900) {
            snprintf(sDiag.group, sizeof sDiag.group, "focus-%08u", sDiag.baseline_seq);
            snprintf(sDiag.role, sizeof sDiag.role, "after_reload_observed");
        } else {
            snprintf(sDiag.group, sizeof sDiag.group, "focus-%08u", sDiag.seq);
            snprintf(sDiag.role, sizeof sDiag.role, "before_reload_candidate");
        }
    } else { sDiag.group[0] = 0; snprintf(sDiag.role, sizeof sDiag.role, "manual"); }
    if (sDiag.mode == PM64_DIAG_TIMELINE) {
        memset(&sMerlee, 0, sizeof sMerlee);
        boss_diag_arm();
        animator_profile_arm(sDiag.reload_generation);
        memset(&sProbe, 0, sizeof sProbe);
        memset(&sDiag.event_start, 0, sizeof sDiag.event_start);
        memset(&sDiag.event_end, 0, sizeof sDiag.event_end);
        sDiag.event_start_pending = 1;
        pm64_diag_event_pending = 1;
    }
    diag_write_metadata(swap, "started", "armed_swap", 0);
    port_log("M22 BEGIN session=%s-%08u mode=%s build_stamp=%08X swap=%u game=%u reload=%u epoch=%u\n", DIAG_SESSION_PREFIX, sDiag.seq,
             pm64_diag_mode_name(sDiag.mode), (unsigned)port_build_stamp, swap, (unsigned)port_frame, sDiag.reload_generation, sDiag.cache_epoch);
    if (sDiag.mode == PM64_DIAG_RENDER_FOCUS) {
        unsigned n = sDiag.history_count;
        unsigned first = (sDiag.history_head + PM64_DIAG_TEX_HISTORY - n) % PM64_DIAG_TEX_HISTORY;
        for (unsigned i = 0; i < n; i++) {
            const unsigned p = (first + i) % PM64_DIAG_TEX_HISTORY;
            const struct PM64DiagTextureInfo *x = &sDiag.history[p].info;
            port_log("M22 HISTORY kind=%s id=%u key=%08X source=%08X import=%ux%u soft=%ux%u tile=%08X result=%u epoch=%u\n", sDiag.history[p].kind, x->id, x->key, x->source_hash, x->import_wh >> 16, x->import_wh & 0xffff, x->soft_wh >> 16, x->soft_wh & 0xffff, x->tile_params, x->result, sDiag.cache_epoch);
        }
    }
    if (sDiag.mode == PM64_DIAG_RAW_COMPAT) gfx_trace_frames = 1;
    return 1;
}
static int diag_screenshot(void) {
    char path[600]; snprintf(path, sizeof path, "%s/frame.ppm", sDiag.dir);
    FILE *f = fopen(path, "wx");
    int w=0, h=0, ok=0;
    const uint32_t *fb = gfx_soft_get_framebuffer(&w, &h);
    if (f && fb && w == 320 && h == 240) {
        ok = fprintf(f, "P6\n%d %d\n255\n", w, h) > 0;
        for (int y=0; y<h && ok; y++) for (int x=0; x<w; x++) {
            uint32_t p=fb[y*w+x]; unsigned char rgb[3]={p>>16,p>>8,p};
            if (fwrite(rgb, 1, 3, f) != 3) { ok=0; break; }
        }
    }
    if (f && fclose(f)) ok=0;
    return ok;
}
/* At most 32 entries per section; integer widths and pointer widths are fixed.
 * Including Vulkan details, the document is below 32768 bytes on a 64-bit host. */
static void diag_vulkan_script(FILE *f, const struct PM64EventSnapshot *x, int which) {
    const struct PM64EventScript *e = &x->vulkan_scripts[which];
    const int count = x->vulkan_matches[which];
    if (!x->vulkan_scripts_valid) { fputs("{\"status\":\"unavailable\"}", f); return; }
    if (count == 0) { fputs("{\"status\":\"not_active\",\"count\":0}", f); return; }
    if (count != 1) { fprintf(f, "{\"status\":\"ambiguous\",\"count\":%d}", count); return; }
    fprintf(f, "{\"status\":\"active\",\"count\":1,\"id\":%d,"
            "\"ptr_first_line\":\"0x%llx\",\"ptr_cur_line\":\"0x%llx\",\"opcode\":",
            e->id, (unsigned long long)e->first_line, (unsigned long long)e->cur_line);
    if (e->opcode_known) fprintf(f, "%d", e->opcode); else fputs("null", f);
    fprintf(f, ",\"state_flags\":%u,\"paused\":%s,\"suspended\":%s,"
            "\"blocked_by_child\":%s,\"blocked\":", e->state_flags,
            (e->state_flags & 0x02) ? "true" : "false",
            (e->state_flags & 0x80) ? "true" : "false",
            (e->state_flags & 0x10) ? "true" : "false");
    if (e->blocked_known) fputs(e->blocked ? "true" : "false", f); else fputs("null", f);
    fputc('}', f);
}
static void diag_event_vulkan(FILE *f, const struct PM64EventSnapshot *x) {
    if (x->vulkan_map < 0) { fputs("{\"status\":\"unavailable\"}", f); return; }
    if (!x->vulkan_map) { fputs("{\"status\":\"not_applicable\"}", f); return; }
    fputs("{\"status\":\"applicable\",\"AF_KZN_Tromp2_StopPlayer\":", f);
    if (x->vulkan_flags_valid) fputs(x->vulkan_stop_player ? "true" : "false", f); else fputs("null", f);
    fputs(",\"AF_KZN_TrompRollingDone\":", f);
    if (x->vulkan_flags_valid) fputs(x->vulkan_rolling_done ? "true" : "false", f); else fputs("null", f);
    fputs(",\"scripts\":{\"EVS_SetupSpinyTromp\":", f);
    diag_vulkan_script(f, x, PM64_KZN_SETUP);
    fputs(",\"D_80240D10_C7EE90\":", f);
    diag_vulkan_script(f, x, PM64_KZN_HIT);
    fputs(",\"EVS_Kolorado_TrompImpact\":", f);
    diag_vulkan_script(f, x, PM64_KZN_KOLORADO);
    fputs(",\"stop_player_thread\":{\"status\":\"not_uniquely_identifiable\"}}}", f);
}
static void diag_event_section(FILE *f, const struct PM64EventSnapshot *x) {
    if (!x->valid) { fputs("null", f); return; }
    fprintf(f, "{\"area_id\":%d,\"map_id\":%d,\"game_mode\":%d,"
            "\"map_transition_state\":%d,\"input_disabled_count\":%d,"
            "\"player_flags\":%u,\"override_flags\":%u,\"action_state\":%d,"
            "\"partner_action_state\":%d,\"vulkan\":",
            x->area, x->map, x->mode, x->transition, x->input_disabled,
            (unsigned)x->player_flags, (unsigned)x->override_flags, x->action, x->partner_action);
    diag_event_vulkan(f, x);
    fputs(",\"scripts\":", f);
    if (!x->scripts_valid) { fputs("null}", f); return; }
    int n = x->script_captured;
    if (n < 0) n = 0;
    if (n > PM64_EVENT_SCRIPT_LIMIT) n = PM64_EVENT_SCRIPT_LIMIT;
    fprintf(f, "{\"count\":%d,\"truncated\":%s,\"entries\":[", x->script_count,
            x->script_count > n ? "true" : "false");
    for (int i = 0; i < n; i++) {
        const struct PM64EventScript *e = &x->scripts[i];
        fprintf(f, "%s{\"id\":%d,\"ptr_first_line\":\"0x%llx\",\"ptr_cur_line\":\"0x%llx\","
                "\"opcode\":", i ? "," : "", e->id,
                (unsigned long long)e->first_line, (unsigned long long)e->cur_line);
        if (e->opcode_known) fprintf(f, "%d", e->opcode); else fputs("null", f);
        fprintf(f, ",\"state_flags\":%u,\"parent_id\":", e->state_flags);
        if (e->parent_known) fprintf(f, "%d", e->parent_id); else fputs("null", f);
        fputs(",\"child_id\":", f);
        if (e->child_known) fprintf(f, "%d", e->child_id); else fputs("null", f);
        fprintf(f, ",\"paused\":%s,\"suspended\":%s,\"blocked_by_child\":%s,\"blocked\":",
                (e->state_flags & 0x02) ? "true" : "false",
                (e->state_flags & 0x80) ? "true" : "false",
                (e->state_flags & 0x10) ? "true" : "false");
        if (e->blocked_known) fputs(e->blocked ? "true" : "false", f); else fputs("null", f);
        fputc('}', f);
    }
    fputs("]}}", f);
}
static void diag_write_event_state(void) {
    char path[600];
    snprintf(path, sizeof path, "%s/event_state.json", sDiag.dir);
    FILE *f = fopen(path, "w");
    if (!f) return;
    fputs("{\"schema\":3,\"sampling\":\"next_game_tick\",\"script_sampling\":\"next_script_update\",\"start\":", f);
    diag_event_section(f, &sDiag.event_start);
    fputs(",\"end\":", f);
    diag_event_section(f, &sDiag.event_end);
    fputs("}\n", f);
    fclose(f);
}
/* Called only by the scheduler after its loop, on the game thread. */
void pm64_diag_event_scheduler(void) {
    struct PM64EventSnapshot *out;
    if (pm64_diag_event_pending == 1 && sDiag.active && sDiag.event_start_pending) {
        out = &sDiag.event_start;
        sDiag.event_start_pending = 0;
    } else if (pm64_diag_event_pending == 2 && sDiag.event_end_pending) {
        out = &sDiag.event_end;
    } else {
        pm64_diag_event_pending = 0;
        return;
    }
    if (!out->valid) pm64_event_snapshot(out);
    if (out->valid) pm64_event_collect_scripts(out);
    pm64_diag_event_pending = 0;
}
/* Write only on session completion, outside boss animation/position functions. */
static void diag_boss_write(void) {
    const struct BossCapture *c = boss_diag_result();
    FILE *f = sDiag.log;
    if (!f) { boss_diag_cancel(); return; }
    const char *reason = boss_diag_reason();
    fprintf(f, "M24A status=%s reason=%s capture=%u update=%u script_id=%d game_frame=%u reload=%u swap_start=%u build_stamp=%08X count=%u limit=%u\n",
        strcmp(reason, "none") ? "incomplete" : "complete", reason, sDiag.seq, c->update_id,
        c->script_id, c->game_frame, sDiag.reload_generation, sDiag.swap_start,
        (unsigned)port_build_stamp, c->count, BOSS_EVENT_LIMIT);
    for (unsigned i = 0; i < c->count; ++i) {
        const struct BossEvent *e = &c->events[i];
        fprintf(f, "M24A event=%u kind=%u world=%.9g,%.9g,%.9g", i + 1, e->kind,
            (double)e->world[0], (double)e->world[1], (double)e->world[2]);
        if (e->kind == BOSS_NODE_HEAD || e->kind == BOSS_NODE_VINE) {
            fprintf(f, " input=%.9g,%.9g,%.9g integer=%d,%d,%d matrix=", (double)e->input[0],
                (double)e->input[1], (double)e->input[2], e->integer[0], e->integer[1], e->integer[2]);
            for (unsigned j = 0; j < 16; ++j) fprintf(f, "%s%.9g", j ? "," : "", (double)e->matrix[j]);
        }
        fputc('\n', f);
    }
    boss_diag_cancel();
}

static void diag_animator_write(void) {
    const struct AnimatorSamples *s = animator_profile_result(); unsigned missing=0;
    for (unsigned i=1;i<s->count;i++) if (!s->samples[i].marker) missing++;
    if (sDiag.log) {
        fprintf(sDiag.log,"M24B status=%s reason=%s count=%u limit=%u ambiguous_updates=%u first_update=unobserved reload=%u\n",(!s->incomplete && s->count==ANIMATOR_SAMPLE_LIMIT && !missing) ? "complete":"incomplete", s->reason ? s->reason : s->count<ANIMATOR_SAMPLE_LIMIT ? "missing_samples" : missing ? "unmatched_update" : "none",s->count,ANIMATOR_SAMPLE_LIMIT,missing,sDiag.reload_generation);
        for (unsigned i=0;i<s->count;i++) {
            const struct AnimatorSample *v=&s->samples[i];
            fprintf(sDiag.log,"M24B sample=%u frame=%u reload=%u update=%u model=%llx animator=%llx animator_id=%d node=%llx animation=%llx read=%llx flags=%u hook_frame=%u hook_calls=%u cause=%u commands=%u timer_changed=%u transforms=%u timer_before=%.9g timer=%.9g speed=%.9g local=%.9g,%.9g,%.9g world=%.9g,%.9g,%.9g head=%.9g,%.9g,%.9g matrix=",i+1,v->frame,v->reload,v->update_id,(unsigned long long)v->model,(unsigned long long)v->animator,v->animator_id,(unsigned long long)v->node,(unsigned long long)v->animation,(unsigned long long)v->read_pos,v->flags,v->hook_frame,v->hook_calls,v->marker,v->commands,v->timer_updated,v->transforms,(double)v->timer_before,(double)v->timer,(double)v->speed,(double)v->local[0],(double)v->local[1],(double)v->local[2],(double)v->world[0],(double)v->world[1],(double)v->world[2],(double)v->head[0],(double)v->head[1],(double)v->head[2]);
            for (unsigned j=0;j<16;j++) fprintf(sDiag.log,"%s%.9g",j ? ",":"",(double)v->matrix[j]);
            fputc('\n',sDiag.log);
        }
    }
    animator_profile_stop();
}

static void diag_end_ex(uint32_t swap, const char *reason, int screenshot) {
    const int ok = screenshot ? diag_screenshot() : 0;
    if (sDiag.mode == PM64_DIAG_TIMELINE) {
        if (sDiag.log) {
            fprintf(sDiag.log, "MERLEE variant=fix count=%u dropped=%u limit=64\n", sMerlee.count, sMerlee.dropped);
            for (unsigned j = 0; j < sMerlee.count; j++) {
                unsigned i = (sMerlee.next - sMerlee.count + j) % 64;
                fprintf(sDiag.log, "MERLEE frame=%u state=%d card_index=0 card=%d phase=%d alpha=%d width=%d height=%d x=%d y=%d result=%d\n", sMerlee.row[i].frame, sMerlee.row[i].state, sMerlee.row[i].card, sMerlee.row[i].phase, sMerlee.row[i].alpha, sMerlee.row[i].width, sMerlee.row[i].height, sMerlee.row[i].x, sMerlee.row[i].y, sMerlee.row[i].result);
            }
        }
        diag_boss_write();
        diag_animator_write();
        pm64_probe_write();
        sDiag.event_start_pending = 0; /* no later scheduler: scripts stay unknown */
        sDiag.event_end_pending = 1;
        sDiag.event_end_wait = 1;
        pm64_diag_event_pending = 2;
    }
    port_log("M22 END session=%s-%08u mode=%s records=%u dropped=%u swap=%u game=%u reason=%s ppm=%s\n", DIAG_SESSION_PREFIX, sDiag.seq,
             pm64_diag_mode_name(sDiag.mode), sDiag.records, sDiag.dropped_records, swap, (unsigned)port_frame, reason, ok ? "ok" : "error");
    diag_write_metadata(swap, screenshot ? (ok ? "complete" : "error") : "partial", reason, ok);
    port_trace_file(NULL);
    if (sDiag.log) fclose(sDiag.log);
    sDiag.log = NULL; sDiag.active = 0; sDiag.armed = 0; sDiag.stop_requested = 0;
    sDiag.state = ok ? PM64_DIAG_SAVED : PM64_DIAG_FAILED;
    snprintf(sDiag.reason, sizeof sDiag.reason, "%s", reason);
    if (sDiag.mode == PM64_DIAG_RENDER_FOCUS && strcmp(sDiag.role, "before_reload_candidate") == 0 && ok) {
        sDiag.baseline_seq = sDiag.seq;
        sDiag.baseline_reload_generation = sDiag.reload_generation;
        sDiag.baseline_game = port_frame;
    }
}
static void diag_end(uint32_t swap, const char *reason) { diag_end_ex(swap, reason, 1); }
void pm64_diag_app_exit(void) {
    if (!sDiag.active && !sDiag.event_end_pending) return;
    if (sDiag.event_end_pending) {
        sDiag.event_end_pending = 0;
        pm64_diag_event_pending = 0;
        diag_write_event_state();
        return;
    }
    /* The video backend may already be winding down: close evidence faithfully
     * without claiming that a final framebuffer was captured. */
    diag_end_ex(sDiag.last_swap, "app_exit", 0);
    if (sDiag.event_end_pending) {
        sDiag.event_end_pending = 0;
        pm64_diag_event_pending = 0;
        diag_write_event_state();
    }
}
void pm64_diag_state_loaded(void) {
    /* Keep a tiny history across sessions: a following focus capture is linked
     * only to one observed load after one completed baseline, never guessed. */
    sDiag.reload_generation++; sDiag.reload_game = port_frame; sDiag.reload_swap = sDiag.swap_start;
    if (sDiag.active && sDiag.mode == PM64_DIAG_TIMELINE) { boss_diag_fail(); animator_profile_reload(); }
    if (sDiag.active && sDiag.mode == PM64_DIAG_TIMELINE && sDiag.records < PM64_DIAG_MAX_RECORDS) {
        sDiag.records++;
        port_log("M21 TIMELINE n=%u event=savestate_load generation=%u game=%u\n", sDiag.records, sDiag.reload_generation, (unsigned)port_frame);
    }
    if (sDiag.active) pm64_diag_render_event("savestate_load", sDiag.reload_generation, 0, 0, 0);
}
int pm64_diag_render_focus_active(void) { return sDiag.active && sDiag.mode == PM64_DIAG_RENDER_FOCUS; }
static int diag_focus_admit(const char *kind, unsigned a, unsigned b) {
    if (strcmp(kind, "texrect") == 0) {
        if (sDiag.texrect_records >= 320) return 0;
        sDiag.texrect_records++; return 1;
    }
    if (strcmp(kind, "bind") == 0) {
        unsigned unit = b & 1u;
        if (sDiag.last_bind[unit] == a || sDiag.bind_records >= 96) return 0;
        sDiag.last_bind[unit] = a; sDiag.bind_records++; return 1;
    }
    if (strcmp(kind, "cache_hit") == 0) {
        unsigned slot = a & 4095u, mask = 1u << (slot & 7u);
        if ((sDiag.seen_hits[slot >> 3] & mask) || sDiag.hit_records >= 128) return 0;
        sDiag.seen_hits[slot >> 3] |= mask; sDiag.hit_records++; return 1;
    }
    /* Resets, imports and uploads establish state; reserve room for them. */
    if (sDiag.critical_records >= 192) return 0;
    sDiag.critical_records++; return 1;
}
void pm64_diag_render_event(const char *kind, unsigned a, unsigned b, unsigned c, unsigned d) {
    if (strcmp(kind, "cache_reset") == 0) sDiag.cache_epoch = a;
    if (!pm64_diag_render_focus_active()) return;
    if (sDiag.records >= PM64_DIAG_MAX_RECORDS || !diag_focus_admit(kind, a, b)) { sDiag.dropped_records++; return; }
    sDiag.records++;
    port_log("M21 RENDER n=%u kind=%s a=%u b=%u c=%u d=%u game=%u epoch=%u\n", sDiag.records, kind, a, b, c, d, (unsigned)port_frame, sDiag.cache_epoch);
}
static int diag_texture_admit(const char *kind) {
    if (strcmp(kind, "texrect") == 0) { if (sDiag.rect_records >= 320) return 0; sDiag.rect_records++; return 1; }
    if (sDiag.import_records >= 192) return 0;
    sDiag.import_records++; return 1;
}
void pm64_diag_texture_event(const char *kind, const struct PM64DiagTextureInfo *x) {
    if (!x) return;
    if (strcmp(kind, "cache_reset") == 0) sDiag.cache_epoch = x->result;
    /* A tiny rolling import/reset history bridges a state load that happens
     * before the operator opens the following Focus capture. */
    if (strcmp(kind, "cache_hit") != 0) {
        unsigned p = sDiag.history_head++ % PM64_DIAG_TEX_HISTORY;
        snprintf(sDiag.history[p].kind, sizeof sDiag.history[p].kind, "%s", kind);
        sDiag.history[p].info = *x;
        if (sDiag.history_count < PM64_DIAG_TEX_HISTORY) sDiag.history_count++;
    }
    if (!pm64_diag_render_focus_active()) return;
    if (sDiag.records >= PM64_DIAG_MAX_RECORDS || !diag_texture_admit(kind)) { sDiag.dropped_records++; return; }
    sDiag.records++;
    port_log("M22 TEX n=%u kind=%s id=%u key=%08X source=%08X import=%ux%u soft=%ux%u tile=%08X result=%u game=%u epoch=%u\n", sDiag.records, kind, x->id, x->key, x->source_hash, x->import_wh >> 16, x->import_wh & 0xffff, x->soft_wh >> 16, x->soft_wh & 0xffff, x->tile_params, x->result, (unsigned)port_frame, sDiag.cache_epoch);
}
void pm64_diag_texrect_event(uint32_t tile, int32_t ulx, int32_t uly, int32_t lrx, int32_t lry, int16_t uls, int16_t ult, int16_t dsdx, int16_t dtdy, const struct PM64DiagTextureInfo *a, const struct PM64DiagTextureInfo *b, uint32_t combine, uint32_t other_h, uint32_t other_l, uint32_t cms, uint32_t cmt) {
    if (!pm64_diag_render_focus_active()) return;
    if (sDiag.records >= PM64_DIAG_MAX_RECORDS || !diag_texture_admit("texrect")) { sDiag.dropped_records++; return; }
    sDiag.records++;
    port_log("M22 RECT n=%u tile=%u xy=%d,%d,%d,%d uv=%d,%d,%d,%d u0=id:%u,key:%08X,src:%08X,import:%ux%u,soft:%ux%u,result:%u u1=id:%u,key:%08X,src:%08X,import:%ux%u,soft:%ux%u,result:%u combine=%08X other=%08X/%08X sampler=%08X/%08X game=%u epoch=%u\n", sDiag.records, tile, ulx, uly, lrx, lry, uls, ult, dsdx, dtdy, a ? a->id : 0, a ? a->key : 0, a ? a->source_hash : 0, a ? a->import_wh >> 16 : 0, a ? a->import_wh & 0xffff : 0, a ? a->soft_wh >> 16 : 0, a ? a->soft_wh & 0xffff : 0, a ? a->result : 0, b ? b->id : 0, b ? b->key : 0, b ? b->source_hash : 0, b ? b->import_wh >> 16 : 0, b ? b->import_wh & 0xffff : 0, b ? b->soft_wh >> 16 : 0, b ? b->soft_wh & 0xffff : 0, b ? b->result : 0, combine, other_h, other_l, cms, cmt, (unsigned)port_frame, sDiag.cache_epoch);
}
void pm64_diag_game_tick(void) {
    if (sDiag.event_start_pending && sDiag.active && !sDiag.event_start.valid)
        pm64_event_snapshot(&sDiag.event_start);
    if (sDiag.event_end_pending && !sDiag.event_end.valid)
        pm64_event_snapshot(&sDiag.event_end);
    if (sDiag.event_end_pending) {
        /* One more game frame permits an update after a render-path finish.
         * If no scheduler runs, preserve an explicitly unknown end state. */
        if (pm64_diag_event_pending != 2 || !sDiag.event_end_wait) {
            sDiag.event_end_pending = 0;
            pm64_diag_event_pending = 0;
            diag_write_event_state();
        } else {
            sDiag.event_end_wait--;
        }
    }
    if (sDiag.active && sDiag.mode == PM64_DIAG_TIMELINE && sDiag.records < PM64_DIAG_MAX_RECORDS && port_frame != sDiag.last_game) {
        sDiag.last_game = port_frame; sDiag.records++;
    port_log("M21 TIMELINE n=%u event=game_tick game=%u\n", sDiag.records, (unsigned)port_frame);
    }
}
void pm64_diag_merlee_phase(const char *phase, int a, int b, int c) {
    if (!sDiag.active || sDiag.mode != PM64_DIAG_TIMELINE || sDiag.records >= PM64_DIAG_MAX_RECORDS) return;
    sDiag.records++;
    port_log("M21 TIMELINE n=%u event=merlee phase=%s a=%d b=%d c=%d game=%u\n", sDiag.records, phase, a, b, c, (unsigned)port_frame);
    if (strcmp(phase, "SCRIPT_DONE") == 0) diag_end(sDiag.swap_start, "script_done");
}

void port_gfx_task(void *gfx_list, int swap) {
    /* PM64_TRACE_AT=<frame>[,<tasks>]: the renderer's command trace (PM64_TRILOG=1 adds every triangle) for that
     * frame's tasks, frames counted as the shots are (the file select's window flip, M2) */
    /* With PM64_SHOTS_GAMEFRAME=1 the frame is the game frame (port_frame), and the switch is read again after a state
     * load (these are kept out of states), so a trace can start from a PM64_STATE_LOAD */
    static uint32_t sFrame, sTraceAt SAVESTATE_EXCLUDE, sTraceN SAVESTATE_EXCLUDE = 1;
    static int sTraceInit SAVESTATE_EXCLUDE, sTraceGame SAVESTATE_EXCLUDE;
    if (!sTraceInit) {
        { const char *g = getenv("PM64_SHOTS_GAMEFRAME"); sTraceGame = g && g[0] == '1'; }
        const char *t = getenv("PM64_TRACE_AT");
        sTraceInit = 1;
        if (t && t[0]) {
            sTraceAt = (uint32_t) strtoul(t, NULL, 10);
            const char *c = strchr(t, ',');
            if (c) sTraceN = (uint32_t) strtoul(c + 1, NULL, 10);
        }
    }
    if (sTraceAt && (sTraceGame ? port_frame : sFrame) == sTraceAt) {
extern int gfx_trace_frames;
        gfx_trace_frames = (int) sTraceN;
        sTraceAt = 0;
    }
    const uint32_t diagFrame = sFrame;
    if (swap && sDiag.armed && !gfx_trace_frames) {
        if (!diag_begin(diagFrame)) {
            sDiag.armed = 0; sDiag.state = PM64_DIAG_FAILED;
            port_log("M21 ERROR cannot create trace output reason=%s\n", sDiag.reason);
        }
    }
    if (swap) { sDiag.last_swap = diagFrame; sFrame++; }
    const uint32_t perfT0 = port_time_us();
    gfx_start_frame();
    gfx_run((Gfx *) gfx_list);
    gfx_end_frame();
    if (swap && sDiag.active && sDiag.mode == PM64_DIAG_TIMELINE && sDiag.records < PM64_DIAG_MAX_RECORDS) {
        sDiag.records++;
        port_log("M21 TIMELINE n=%u event=swap swap=%u game=%u\n", sDiag.records, diagFrame, (unsigned)port_frame);
    }
    if (swap && sDiag.active && sDiag.mode == PM64_DIAG_RENDER_FOCUS)
        diag_end(diagFrame, "single_swap");
    else if (swap && sDiag.active && sDiag.mode == PM64_DIAG_RAW_COMPAT && !gfx_trace_frames)
        diag_end(diagFrame, "raw_complete");
    else if (swap && sDiag.active && sDiag.mode == PM64_DIAG_TIMELINE && sDiag.stop_requested)
        diag_end(diagFrame, "user_stop");
    else if (swap && sDiag.active && sDiag.mode == PM64_DIAG_TIMELINE &&
             (diagFrame - sDiag.swap_start >= PM64_DIAG_TIMELINE_SWAPS || sDiag.records >= PM64_DIAG_MAX_RECORDS))
        diag_end(diagFrame, sDiag.records >= PM64_DIAG_MAX_RECORDS ? "record_limit" : "timeout");
    if (!swap) { port_perf_gfx_us += port_time_us() - perfT0; return; }
#ifdef PM64_SDL12
    port_video_present();
#endif
    port_perf_gfx_us += port_time_us() - perfT0;
    depth_query_resolve();
    if (sShotDir) save_ppm();
}

/* The last frame as the game's RGBA16 framebuffer holds it (big-endian 5551, 320x240): the pause menu's backdrop
 * (background_gfx.c, BACKGROUND_RENDER_STATE_FILTER_PAUSED). */
void port_capture_frame_rgba16(uint16_t *dst) {
    int w, h;
    const uint32_t *fb = gfx_soft_get_framebuffer(&w, &h);
    if (!fb || w <= 0 || h <= 0) return;
    for (int y = 0; y < 240; y++)
        for (int x = 0; x < 320; x++) {
            const uint32_t p = fb[(y * h / 240) * w + (x * w / 320)];
            const uint16_t c = (uint16_t) (((p >> 19) & 31) << 11 | ((p >> 11) & 31) << 6 | ((p >> 3) & 31) << 1 | 1);
            dst[y * 320 + x] = (uint16_t) (c >> 8 | c << 8);
        }
}

/* The frame into the game's own framebuffer, for the effects that texture the screen with the frame before
 * (screen_render_util.c appendGfx_draw_prev_frame_buffer: the battle-start swirl, the blur overlay, the Dizzy Dial,
 * the firework). The renderer draws into its own buffer and never into that memory, so the swirl drew the frame
 * before as nothing: the black screen before the prologue's Bowser battle (the user, 29 Sep). A copy at the draw's
 * building is too early -- each frame's background task is drawn before its main list is built, so it held the sky
 * alone -- so, as on the N64, a finished frame goes into its own framebuffer (nuGfxTaskStart's), while the effects
 * ask for it: a frame that asked copies itself when drawn, for the next frame's draw. An effect's first frame finds
 * no copy of the frame before (it is gone by then): that buffer is cleared. PM64_PREVCOPY=0: the old way (A/B). */
static uint16_t *sPrevCopied; /* the framebuffer the last finished frame went to */
static int sPrevWanted;       /* this frame asked */
static int sPrevX0 = 320, sPrevY0 = 240, sPrevX1 = -1, sPrevY1 = -1;
bool port_is_framebuffer(const void *address);
void port_capture_prev_rows(uint16_t *dst, int x0, int y0, int x1, int y1) {
    static int on = -1; /* PM64_PREVCOPY=0: no copy (the A/B) */
    if (on < 0) { const char *e = getenv("PM64_PREVCOPY"); on = !(e && e[0] == '0'); }
    if (!on || !port_is_framebuffer(dst)) return;
    if (x0 < 0) x0 = 0;
    if (y0 < 0) y0 = 0;
    if (x1 > 319) x1 = 319;
    if (y1 > 239) y1 = 239;
    if (x0 > x1 || y0 > y1) return;
    if (dst != sPrevCopied)
        for (int y = y0; y <= y1; y++) memset(dst + y * 320 + x0, 0, (size_t) (x1 - x0 + 1) * 2);
    if (x0 < sPrevX0) sPrevX0 = x0;
    if (y0 < sPrevY0) sPrevY0 = y0;
    if (x1 > sPrevX1) sPrevX1 = x1;
    if (y1 > sPrevY1) sPrevY1 = y1;
    sPrevWanted = 1;
}
/* After a frame's main task (port_nusys.c nuGfxTaskStart): that frame into its framebuffer, if it asked */
void port_prev_frame_done(uint16_t *dst) {
    int w, h;
    const uint32_t *fb = gfx_soft_get_framebuffer(&w, &h);
    sPrevCopied = NULL;
    if (!sPrevWanted) return;
    sPrevWanted = 0;
    if (fb && w > 0 && h > 0 && port_is_framebuffer(dst)) {
        for (int y = sPrevY0; y <= sPrevY1; y++) {
            const uint32_t *row = fb + (size_t) (y * h / 240) * w;
            uint16_t *out = dst + y * 320;
            for (int x = sPrevX0; x <= sPrevX1; x++) {
                const uint32_t p = row[x * w / 320];
                const uint16_t c = (uint16_t) (((p >> 19) & 31) << 11 | ((p >> 11) & 31) << 6 | ((p >> 3) & 31) << 1 | 1);
                out[x] = (uint16_t) (c >> 8 | c << 8);
            }
        }
        sPrevCopied = dst;
    }
    sPrevX0 = 320, sPrevY0 = 240, sPrevX1 = -1, sPrevY1 = -1;
}

bool port_is_framebuffer(const void *address) {
    const uint8_t *a = address;
    const size_t n = 320 * 240 * 2;
    return (a >= (const uint8_t *) gFrameBuf0 && a < (const uint8_t *) gFrameBuf0 + n) ||
           (a >= (const uint8_t *) gFrameBuf1 && a < (const uint8_t *) gFrameBuf1 + n) ||
           (a >= (const uint8_t *) gFrameBuf2 && a < (const uint8_t *) gFrameBuf2 + n);
}

void *port_seg_to_ptr(uintptr_t addr) {
    static int sWarned;
    if (addr < 0x10000000u && sWarned < 20) {
        sWarned++;
        port_log("gfx: segmented address 0x%08X has no segment base (0x%08X)\n", (unsigned) addr, (unsigned) addr);
    }
    return (void *) addr;
}
