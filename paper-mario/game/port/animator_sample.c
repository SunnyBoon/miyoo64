#include "animator_sample.h"
#include <string.h>
#include "savestate.h"
void animator_samples_arm(struct AnimatorSamples *s) {
    memset(s, 0, sizeof *s);
    s->enabled = 1;
}
void animator_samples_store(struct AnimatorSamples *s, const struct AnimatorSample *sample) {
    if (!s->enabled || s->incomplete) return;
    if (!sample || !sample->model || !sample->animator || !sample->node) { s->incomplete = 1; s->reason="missing_identity"; return; }
    if (s->count == ANIMATOR_SAMPLE_LIMIT) { s->enabled = 0; return; }
    if (s->count) {
        const struct AnimatorSample *p = &s->samples[s->count - 1];
        if (sample->model != p->model || sample->animator != p->animator || sample->node != p->node ||
            sample->reload != p->reload || sample->frame < p->frame || sample->update_id != p->update_id + 1) {
            s->incomplete = 1; s->reason = sample->reload != p->reload ? "reload_changed" : "identity_or_order_changed"; return;
        }
    }
    s->samples[s->count++] = *sample;
    if (s->count == ANIMATOR_SAMPLE_LIMIT) s->enabled = 0;
}

/* One explicitly selected instance; hook calls are inert until bound. */
int animator_hook_enabled SAVESTATE_EXCLUDE;
static struct AnimatorSamples *hook_samples SAVESTATE_EXCLUDE;
static int32_t hook_id SAVESTATE_EXCLUDE;
static uintptr_t hook_instance SAVESTATE_EXCLUDE;
static int hook_current SAVESTATE_EXCLUDE;
void animator_hook_bind(struct AnimatorSamples *s, int32_t id, const void *instance) {
    hook_samples = s; hook_id = id; hook_instance = (uintptr_t)instance;
    hook_current = 0; animator_hook_enabled = s && s->enabled && instance;
}
void animator_hook_unbind(void) { animator_hook_enabled = 0; hook_samples = 0; hook_current = 0; }
void animator_hook_begin(int32_t id) {
    hook_current = animator_hook_enabled && hook_samples && hook_samples->enabled && id == hook_id;
    if (!hook_current) return;
    unsigned calls = hook_samples->hook.calls + 1;
    memset(&hook_samples->hook, 0, sizeof hook_samples->hook);
    hook_samples->hook.calls = calls; hook_samples->hook.requested_id = id;
    { extern unsigned port_frame; hook_samples->hook.frame = port_frame; }
}
void animator_hook_resolve(int32_t id, const void *instance, const void *read_pos, const void *buffer, float timer, float speed) {
    if (!hook_current || id != hook_id) return;
    if (instance && (uintptr_t)instance != hook_instance) { hook_samples->hook.cause = ANIM_HOOK_INSTANCE_CHANGED; hook_samples->incomplete = 1; hook_samples->reason="animator_instance_changed"; hook_current = 0; return; }
    hook_samples->hook.animator = (uintptr_t)instance;
    hook_samples->hook.read_before = (uintptr_t)read_pos; hook_samples->hook.animation_buffer = (uintptr_t)buffer;
    hook_samples->hook.timer_before = timer; hook_samples->hook.speed = speed;
}
void animator_hook_timer(int32_t id, float timer) { if (hook_current && id == hook_id) hook_samples->hook.timer_updated = timer != hook_samples->hook.timer_before; }
void animator_hook_command(int32_t id) { if (hook_current && id == hook_id) hook_samples->hook.commands++; }
void animator_hook_end(int32_t id, unsigned cause, const void *instance, const void *read_pos, float timer) {
    if (!hook_current || id != hook_id) return;
    hook_samples->hook.cause = cause; hook_samples->hook.animator = (uintptr_t)instance;
    hook_samples->hook.read_after = (uintptr_t)read_pos; hook_samples->hook.timer_after = timer;
    hook_samples->hook.transforms = cause == ANIM_HOOK_COMPLETE;
    hook_current = 0;
}

/* Lava-Piranha selection is established only by the boss script wrapper. */
static struct AnimatorSamples profile SAVESTATE_EXCLUDE;
static struct AnimatorSample pending SAVESTATE_EXCLUDE;
static uintptr_t profile_script SAVESTATE_EXCLUDE;
static int32_t profile_script_id SAVESTATE_EXCLUDE;
static unsigned profile_reload SAVESTATE_EXCLUDE;
static unsigned pending_valid SAVESTATE_EXCLUDE;
int animator_profile_selected SAVESTATE_EXCLUDE;
void animator_profile_arm(unsigned reload) { animator_hook_unbind(); animator_samples_arm(&profile); profile_reload=reload; profile_script=0; pending_valid=0; animator_profile_selected=0; }
void animator_profile_select(const void *script, int32_t id) {
    animator_profile_selected=0;
    if (!profile.enabled || profile.incomplete) return;
    if (pending_valid) { profile.incomplete=1; profile.reason="head_position_missing"; return; }
    if (!profile_script) { profile_script=(uintptr_t)script; profile_script_id=id; }
    if (profile_script!=(uintptr_t)script || profile_script_id!=id) { profile.incomplete=1; profile.reason="script_changed"; return; }
    animator_profile_selected=1;
}
void animator_profile_unselect(void) { animator_profile_selected=0; }
void animator_profile_node(const void *model, const void *animator, const void *node, int32_t id, const void *animation, unsigned flags, float timer, float speed, const float *local, const float *world, const float *matrix) {
    extern unsigned port_frame;
    if (!animator_profile_selected || !profile.enabled || profile.incomplete) return;
    if (!model || !animator || !node) { profile.incomplete=1; profile.reason="missing_identity"; return; }
    memset(&pending,0,sizeof pending); pending.model=(uintptr_t)model; pending.animator=(uintptr_t)animator; pending.node=(uintptr_t)node;
    pending.animator_id=id; pending.animation=(uintptr_t)animation; pending.flags=flags; pending.timer=timer; pending.speed=speed;
    pending.frame=port_frame; pending.reload=profile_reload; pending.update_id=profile.count+1;
    memcpy(pending.local,local,sizeof pending.local); memcpy(pending.world,world,sizeof pending.world); memcpy(pending.matrix,matrix,sizeof pending.matrix);
    if (!profile.count) { animator_hook_bind(&profile,id,animator); pending.marker=ANIM_HOOK_NONE; }
    else {
        const struct AnimatorSample *first=&profile.samples[0];
        if (first->model!=(uintptr_t)model || first->animator!=(uintptr_t)animator || first->animator_id!=id || first->node!=(uintptr_t)node) { profile.incomplete=1; profile.reason="instance_changed"; return; }
        pending.hook_frame=profile.hook.frame; pending.hook_calls=profile.hook.calls;
        if (profile.hook.calls==profile.samples[profile.count-1].hook_calls+1 && profile.hook.animator==(uintptr_t)animator && profile.hook.frame==port_frame) {
            pending.marker=profile.hook.cause; pending.commands=profile.hook.commands; pending.timer_updated=profile.hook.timer_updated;
            pending.transforms=profile.hook.transforms; pending.timer_before=profile.hook.timer_before; pending.read_pos=profile.hook.read_after;
        }
    }
    pending_valid=1;
}
void animator_profile_head(const void *script, int32_t id, const float *head) {
    if (!profile.enabled || profile.incomplete || !pending_valid) return;
    if (profile_script!=(uintptr_t)script || profile_script_id!=id) { profile.incomplete=1; profile.reason="script_changed"; return; }
    memcpy(pending.head,head,sizeof pending.head); animator_samples_store(&profile,&pending); pending_valid=0;
    if (!profile.enabled) animator_hook_unbind();
}
void animator_profile_reload(void) { if (profile.enabled) { profile.incomplete=1; profile.reason="reload_changed"; animator_hook_unbind(); } }
const struct AnimatorSamples *animator_profile_result(void) { return &profile; }
void animator_profile_stop(void) { animator_hook_unbind(); profile.enabled=0; animator_profile_selected=0; }

int animator_profile_pending(void) { return profile.enabled && !profile.incomplete && pending_valid; }
