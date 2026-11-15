#ifndef PM64_BOSS_DIAG_H
#define PM64_BOSS_DIAG_H
#include <stdint.h>

enum { BOSS_NODE_HEAD = 1, BOSS_PART = 2, BOSS_NODE_VINE = 3, BOSS_VINE = 4, BOSS_EVENT_LIMIT = 4 };
struct BossEvent {
    unsigned kind;
    float input[3], matrix[16], world[3];
    int32_t integer[3];
};
struct BossCapture {
    unsigned enabled, finished, count, overflow, mismatch, update_id;
    int32_t script_id;
    uintptr_t script;
    unsigned game_frame;
    struct BossEvent events[BOSS_EVENT_LIMIT];
};
void boss_diag_arm(void);
void boss_diag_cancel(void);
void boss_diag_select(const void *script, int32_t id, unsigned kind);
unsigned boss_diag_selected(const void *script, int32_t id);
void boss_diag_unselect(void);
void boss_diag_fail(void);
void boss_diag_check_frame(unsigned frame);
void boss_diag_begin(const void *script, int32_t id, unsigned frame);
int boss_diag_wants(const void *script, int32_t id, unsigned kind);
void boss_diag_node(const void *script, int32_t id, unsigned kind, const float input[3], const float matrix[16], const float world[3], const int32_t integer[3]);
void boss_diag_position(const void *script, int32_t id, unsigned kind, const float position[3]);
const struct BossCapture *boss_diag_result(void);
const char *boss_diag_reason(void);
#endif
