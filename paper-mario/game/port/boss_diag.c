/* A single bounded boss update, owned by the host diagnostic session. */
#include "boss_diag.h"
#include "savestate.h"
#include <string.h>
static struct BossCapture capture SAVESTATE_EXCLUDE;
static unsigned next_update SAVESTATE_EXCLUDE;
static unsigned selected SAVESTATE_EXCLUDE;
void boss_diag_arm(void) { memset(&capture, 0, sizeof capture); selected = 0; capture.enabled = 1; }
void boss_diag_cancel(void) { capture.enabled = 0; selected = 0; }
void boss_diag_fail(void) { if (capture.enabled) { capture.mismatch = 1; capture.finished = 1; } }
void boss_diag_check_frame(unsigned frame) { if (capture.enabled && !capture.finished && capture.script && capture.game_frame != frame) boss_diag_fail(); }
void boss_diag_select(const void *script, int32_t id, unsigned kind) { selected = boss_diag_wants(script,id,kind) ? kind : 0; }
unsigned boss_diag_selected(const void *script, int32_t id) { return selected && boss_diag_wants(script,id,selected) ? selected : 0; }
void boss_diag_unselect(void) { selected = 0; }
void boss_diag_begin(const void *script, int32_t id, unsigned frame) {
    if (!capture.enabled || capture.finished) return;
    if (capture.script) { capture.mismatch = 1; capture.finished = 1; return; }
    capture.script = (uintptr_t)script; capture.script_id = id; capture.game_frame = frame;
    capture.update_id = ++next_update;
}
int boss_diag_wants(const void *script, int32_t id, unsigned kind) {
    if (!capture.enabled || capture.finished || !capture.script) return 0;
    if (capture.script != (uintptr_t)script || capture.script_id != id) { capture.mismatch = 1; capture.finished = 1; return 0; }
    if (capture.count >= BOSS_EVENT_LIMIT) { capture.overflow = 1; capture.finished = 1; return 0; }
    if (kind != capture.count + 1) { capture.mismatch = 1; capture.finished = 1; return 0; }
    return 1;
}
void boss_diag_node(const void *script, int32_t id, unsigned kind, const float input[3], const float matrix[16], const float world[3], const int32_t integer[3]) {
    struct BossEvent *e;
    if (!boss_diag_wants(script,id,kind)) return;
    e = &capture.events[capture.count++]; e->kind = kind;
    memcpy(e->input,input,sizeof e->input); memcpy(e->matrix,matrix,sizeof e->matrix);
    memcpy(e->world,world,sizeof e->world); memcpy(e->integer,integer,sizeof e->integer);
}
void boss_diag_position(const void *script, int32_t id, unsigned kind, const float position[3]) {
    struct BossEvent *e;
    if (!boss_diag_wants(script,id,kind)) return;
    e = &capture.events[capture.count++]; e->kind = kind; memcpy(e->world,position,sizeof e->world);
    if (capture.count == BOSS_EVENT_LIMIT) capture.finished = 1;
}
const struct BossCapture *boss_diag_result(void) { return &capture; }

const char *boss_diag_reason(void) {
    if (capture.overflow) return "overflow";
    if (capture.mismatch) return "context_or_order_mismatch";
    if (!capture.script) return "boss_update_not_seen";
    if (capture.count != BOSS_EVENT_LIMIT) return "missing_events";
    return "none";
}
