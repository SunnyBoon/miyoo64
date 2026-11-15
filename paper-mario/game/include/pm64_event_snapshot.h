#ifndef PM64_EVENT_SNAPSHOT_H
#define PM64_EVENT_SNAPSHOT_H
/* Shared by the game scheduler and the host-side capture writer. */
enum { PM64_EVENT_SCRIPT_LIMIT = 32 };
struct PM64EventScript {
    int id, opcode, opcode_known, blocked, blocked_known;
    int parent_id, parent_known, child_id, child_known;
    unsigned int state_flags;
    __UINTPTR_TYPE__ first_line, cur_line;
};
enum { PM64_KZN_SETUP, PM64_KZN_HIT, PM64_KZN_KOLORADO, PM64_KZN_SCRIPT_COUNT };
struct PM64EventSnapshot {
    int valid, area, map, mode, transition, input_disabled;
    unsigned int player_flags, override_flags;
    int action, partner_action;
    int scripts_valid, script_count, script_captured;
    int vulkan_map, vulkan_flags_valid, vulkan_stop_player, vulkan_rolling_done;
    int vulkan_scripts_valid, vulkan_matches[PM64_KZN_SCRIPT_COUNT];
    struct PM64EventScript vulkan_scripts[PM64_KZN_SCRIPT_COUNT];
    struct PM64EventScript scripts[PM64_EVENT_SCRIPT_LIMIT];
};
extern int pm64_diag_event_pending;
void pm64_diag_event_scheduler(void);
int pm64_event_kzn17_ready(const struct PM64EventSnapshot *out);
void pm64_event_collect_scripts(struct PM64EventSnapshot *out);
enum { PM64_PROBE_LIMIT = 16 };
struct PM64ProbeValues { int valid, id, opcode, flags, lvar1, lvare, lvarf; __UINTPTR_TYPE__ cur, next; float counter, scale; };
struct PM64ProbeRow { unsigned int call; int present, reached, invoked, error_before, reason; struct PM64ProbeValues before, after; };
int pm64_probe_active(void);
int pm64_probe_map(void);
void pm64_probe_store(const struct PM64ProbeRow *row);
#endif
