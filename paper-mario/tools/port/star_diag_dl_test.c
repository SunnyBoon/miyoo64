/* Run actual DL interpreter with a body marker and adjacent unrelated tris. */
#define main nearw_test_unused_main
#include "nearw_test.c"
#undef main
#include <assert.h>
uint32_t port_frame = 77;
static unsigned begins, ends, end_id, end_frame;
int pm64_diag_render_focus_active(void) { return 1; }
void gfx_star_diag_begin(int probe) { assert(probe == 1); begins++; }
void gfx_star_diag_end(unsigned id, unsigned frame) { ends++; end_id = id; end_frame = frame; }
int main(int argc, char **argv) {
    Gfx commands[6] = {0};
    const int enabled = argc == 1;
    (void)argv;
    if (enabled) setenv("PM64_STAR_DIAG", "1", 1);
    else unsetenv("PM64_STAR_DIAG");
    /* An invalid vertex index is rejected before the graphics backend is used. */
    commands[0].words.w0 = ((uint32_t)G_TRI1 << 24) | 0x00fefefeu;
    commands[1].words.w0 = (uint32_t)G_NOOP << 24; commands[1].words.w1 = 0xa300002au;
    commands[2].words.w0 = ((uint32_t)G_TRI2 << 24) | 0x00fefefeu;
    commands[2].words.w1 = 0x00fefefeu;
    commands[3].words.w0 = (uint32_t)G_NOOP << 24; commands[3].words.w1 = 0xa400002au;
    commands[4].words.w0 = ((uint32_t)G_TRI1 << 24) | 0x00fefefeu;
    commands[5].words.w0 = (uint32_t)G_ENDDL << 24;
    gfx_run_dl(commands);
    if (!enabled) {
        assert(begins == 0 && ends == 0 && sStarDiag.id == 0 && sStarDiag.interpreted == 0);
        puts("star_diag_dl_test: PASS (disabled)");
        return 0;
    }
    assert(begins == 1 && ends == 1 && end_id == 42 && end_frame == 77);
    assert(sStarDiag.id == 0 && sStarDiag.interpreted == 2 && sStarDiag.early == 2);
    assert(sStarDiag.cull_clip == 0 && sStarDiag.emitted == 0);
    puts("star_diag_dl_test: PASS (body isolation and early reject)");
    return 0;
}
