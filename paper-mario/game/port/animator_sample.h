#ifndef PM64_ANIMATOR_SAMPLE_H
#define PM64_ANIMATOR_SAMPLE_H
#include <stdint.h>
#define ANIMATOR_SAMPLE_LIMIT 16
struct AnimatorSample {
    uintptr_t model, animator, node, animation, read_pos;
    uint32_t frame, reload, update_id, flags, marker;
    uint32_t hook_frame, hook_calls, commands, timer_updated, transforms;
    float timer_before;
    int32_t animator_id;
    float timer, speed, local[3], world[3], matrix[16], head[3];
};
enum AnimatorHookCause { ANIM_HOOK_NONE, ANIM_HOOK_CONTEXT, ANIM_HOOK_MISSING, ANIM_HOOK_INACTIVE, ANIM_HOOK_FROZEN, ANIM_HOOK_END, ANIM_HOOK_COMPLETE, ANIM_HOOK_INSTANCE_CHANGED };
struct AnimatorHookRecord {
    uintptr_t animator, read_before, read_after, animation_buffer;
    int32_t requested_id;
    uint32_t calls, cause, commands, transforms, timer_updated, frame;
    float timer_before, timer_after, speed;
};
struct AnimatorSamples {
    struct AnimatorSample samples[ANIMATOR_SAMPLE_LIMIT];
    uint32_t enabled, count, incomplete;
    const char *reason;
    struct AnimatorHookRecord hook;
};
extern int animator_hook_enabled;
extern int animator_profile_selected;
void animator_profile_arm(unsigned reload);
void animator_profile_select(const void *script, int32_t id);
void animator_profile_unselect(void);
void animator_profile_node(const void *model, const void *animator, const void *node, int32_t id, const void *animation, unsigned flags, float timer, float speed, const float *local, const float *world, const float *matrix);
void animator_profile_head(const void *script, int32_t id, const float *head);
void animator_profile_reload(void);
const struct AnimatorSamples *animator_profile_result(void);
void animator_profile_stop(void);
void animator_hook_bind(struct AnimatorSamples *s, int32_t id, const void *instance);
void animator_hook_unbind(void);
void animator_hook_begin(int32_t id);
void animator_hook_resolve(int32_t id, const void *instance, const void *read_pos, const void *buffer, float timer, float speed);
void animator_hook_timer(int32_t id, float timer);
void animator_hook_command(int32_t id);
void animator_hook_end(int32_t id, unsigned cause, const void *instance, const void *read_pos, float timer);
void animator_samples_arm(struct AnimatorSamples *s);
void animator_samples_store(struct AnimatorSamples *s, const struct AnimatorSample *sample);
#endif
