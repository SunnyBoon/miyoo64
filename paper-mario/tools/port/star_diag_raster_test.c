/* Exercise the real scalar rasterizer's star counters without a ROM or ARM build. */
#define main fill_test_unused_main
#include "fill_test.c"
#undef main
#include <assert.h>
void pm64_diag_texture_event(const char *kind, const PM64DiagTextureInfo *info) { (void)kind; (void)info; }

int main(void) {
    gfx_soft_set_render_res(W, H, W, H);
    gfx_soft_init(); ensure_targets();
    gTex = gfx_soft_new_texture(); upload_uniform(0xffffffffu);
    gNT = 1;
    gT[0] = vtx(10, 10, 255, 255, 255, 255);
    gT[1] = vtx(40, 10, 255, 255, 255, 255);
    gT[2] = vtx(10, 40, 255, 255, 255, 255);
    gfx_star_diag_begin(1); draw(true, true);
    unsigned painted = 0;
    for (int i = 0; i < W * H; i++) if (sFB[i] != 0xff000000u) painted++;
    assert(painted && sStarPixels.calls == 1 && !sStarPixels.incomplete);
    assert(sStarPixels.writes == painted && sStarPixels.z == 0 && sStarPixels.alpha == 0);
    for (int i = 0; i < 4; i++) {
        assert(sStarPixels.samples[1][i].valid && sStarPixels.samples[1][i].fa == 255);
        assert(sStarPixels.samples[1][i].amin == 8 && sStarPixels.samples[1][i].t0a == 255);
        assert(sStarPixels.samples[1][i].t1a == -1 && sStarPixels.samples[1][i].shade == 255);
    }
    uint32_t expected[W * H]; memcpy(expected, sFB, sizeof expected);
    gfx_star_diag_begin(0); draw(true, true);
    assert(memcmp(expected, sFB, sizeof expected) == 0 && !sStarPixels.probe);
    for (int i = 0; i < 4; i++) assert(!sStarPixels.samples[0][i].valid && !sStarPixels.samples[1][i].valid);
    for (int i = 0; i < W * H; i++) sZB[i] = 0;
    gfx_star_diag_begin(1);
    gfx_soft_draw_triangles((float *)gT, 3 * sizeof(SoftVtx) / 4, 1);
    assert(sStarPixels.calls == 1 && sStarPixels.z == painted && sStarPixels.writes == 0);
    gT[0].a = gT[1].a = gT[2].a = 0;
    gfx_star_diag_begin(1); draw(true, false);
    assert(sStarPixels.calls == 1 && sStarPixels.alpha == painted && sStarPixels.writes == 0);
    for (int i = 0; i < 4; i++) {
        assert(sStarPixels.samples[0][i].valid && sStarPixels.samples[0][i].fa == 0);
        assert(sStarPixels.samples[0][i].amin == 8 && sStarPixels.samples[0][i].shade == 0 && sStarPixels.samples[0][i].filter == 0);
        assert(sStarPixels.samples[0][i].t0a == -1 && sStarPixels.samples[0][i].t1a == -1);
        assert(!sStarPixels.samples[1][i].valid);
    }
    gProg.cc.opt_texture_edge = true; sEdgeFullCoverage = true;
    gfx_star_diag_begin(1); gfx_soft_draw_triangles((float *)gT, 3 * sizeof(SoftVtx) / 4, 1);
    assert(sStarPixels.samples[0][0].valid && sStarPixels.samples[0][0].amin == 32);
    sEdgeFullCoverage = false;
    gfx_star_diag_begin(1); gfx_soft_draw_triangles((float *)gT, 3 * sizeof(SoftVtx) / 4, 1);
    assert(sStarPixels.samples[0][0].valid && sStarPixels.samples[0][0].amin == 0x55);
    gProg.cc.opt_texture_edge = false;
    gT[0].a = 0; gT[1].a = gT[2].a = 100;
    gfx_star_diag_begin(1); draw(true, false);
    assert(sStarPixels.alpha > 0 && sStarPixels.writes > 0);
    for (int i = 0; i < 4; i++) {
        assert(sStarPixels.samples[0][i].valid && sStarPixels.samples[1][i].valid);
        assert(sStarPixels.samples[0][i].fa < sStarPixels.samples[0][i].amin);
        assert(sStarPixels.samples[1][i].fa >= sStarPixels.samples[1][i].amin);
    }
    memcpy(expected, sFB, sizeof expected);
    gfx_star_diag_end(1, 1); draw(true, false);
    assert(memcmp(expected, sFB, sizeof expected) == 0 && !sStarPixels.active && !sStarPixels.probe);
    /* Real two-cycle generic combiner: verify both sampled texel alphas. */
    static uint8_t tex1[32 * 32 * 4];
    for (int i = 0; i < 32 * 32; i++) { tex1[4*i]=255; tex1[4*i+1]=255; tex1[4*i+2]=255; tex1[4*i+3]=128; }
    uint32_t id1 = gfx_soft_new_texture(); gfx_soft_select_texture(1, id1);
    gfx_soft_upload_texture(tex1, 32, 32, GU_PSM_8888); gfx_soft_set_sampler_parameters(1, false, 0, 0);
    const uint8_t prim[4] = {255,255,255,255}, env[4] = {127,127,127,127}; const int32_t map[6] = {0};
    gfx_soft_pm_generic(true, 0xFFFFFFu, 0xFFFD7438u, true, prim, env, 0, 0, 0, map);
    assert(!sGen.t0 && sGen.t1);
    gfx_star_diag_begin(1); draw(true, true);
    StarAlphaSample *q = sStarPixels.samples[0][0].valid ? &sStarPixels.samples[0][0] : &sStarPixels.samples[1][0];
    assert(q->valid && q->t0a == -1 && q->t1a == 128 && q->amin == 8 && q->fa == 128 && q->filter == 1);
    sGen.t0 = true; /* force both already-bound texels to be fetched */
    gfx_star_diag_begin(1); draw(true, true);
    q = sStarPixels.samples[0][0].valid ? &sStarPixels.samples[0][0] : &sStarPixels.samples[1][0];
    assert(q->valid && q->t0a == 255 && q->t1a == 128);
    gfx_soft_pm_generic(false, 0, 0, false, 0, 0, 0, 0, 0, 0);
    gT[0].x = gT[1].x = gT[2].x = -100;
    gfx_star_diag_begin(1); draw(true, false);
    assert(sStarPixels.calls == 1 && sStarPixels.writes == 0 && sStarPixels.alpha == 0);
    gfx_star_diag_end(1, 1);
    puts("star_diag_raster_test: PASS (writes, z, alpha, rejected triangle)");
    return 0;
}
