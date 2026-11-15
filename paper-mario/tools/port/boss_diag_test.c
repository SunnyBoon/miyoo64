#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "boss_diag.h"

static void node(const void *s, int id, unsigned kind) {
    const float in[3]={1,2,3}, m[16]={1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1}, out[3]={4,5,6};
    const int32_t ints[3]={4,5,6};
    boss_diag_node(s,id,kind,in,m,out,ints);
}
static void position(const void *s, int id, unsigned kind) {
    const float p[3]={4,5,6}; boss_diag_position(s,id,kind,p);
}
int main(void) {
    int a = 0, b = 0; const struct BossCapture *c;
    boss_diag_begin(&a,7,42); node(&a,7,BOSS_NODE_HEAD);
    assert(!boss_diag_result()->enabled && !boss_diag_result()->count);
    boss_diag_arm(); assert(!strcmp(boss_diag_reason(), "boss_update_not_seen"));
    boss_diag_begin(&a,7,42); assert(!strcmp(boss_diag_reason(), "missing_events"));
    boss_diag_select(&a,7,BOSS_NODE_HEAD); assert(boss_diag_selected(&a,7)==BOSS_NODE_HEAD);
    node(&a,7,BOSS_NODE_HEAD); boss_diag_unselect(); assert(!boss_diag_selected(&a,7));
    position(&a,7,BOSS_PART); node(&a,7,BOSS_NODE_VINE); position(&a,7,BOSS_VINE);
    c=boss_diag_result(); assert(c->finished && c->count==4 && !c->mismatch && !c->overflow);
    assert(!strcmp(boss_diag_reason(), "none"));
    assert(c->script_id==7 && c->game_frame==42 && c->update_id && c->events[0].matrix[5]==1);
    unsigned update=c->update_id;
    for (int i=0;i<100;i++) { node(&a,7,BOSS_NODE_HEAD); position(&a,7,BOSS_VINE); }
    assert(c->count==4 && c->update_id==update && !c->overflow);
    boss_diag_fail(); assert(c->mismatch && c->finished);
    assert(!strcmp(boss_diag_reason(), "context_or_order_mismatch"));
    boss_diag_arm(); boss_diag_begin(&a,8,43); node(&a,8,BOSS_NODE_HEAD);
    position(&b,8,BOSS_PART); assert(c->finished && c->mismatch && c->count==1);
    boss_diag_arm(); boss_diag_begin(&a,8,43); node(&a,8,BOSS_NODE_HEAD);
    node(&a,8,BOSS_NODE_VINE); assert(c->finished && c->mismatch && c->count==1);
    boss_diag_arm(); boss_diag_begin(&a,8,43); boss_diag_check_frame(44);
    assert(c->finished && c->mismatch && !c->count);
    boss_diag_arm(); boss_diag_begin(&a,8,43); boss_diag_begin(&a,8,44);
    assert(c->finished && c->mismatch && !c->count);
    boss_diag_arm(); boss_diag_begin(&a,8,43); node(&a,8,BOSS_NODE_HEAD);
    assert(!c->finished && c->count==1); boss_diag_cancel(); position(&a,8,BOSS_PART); assert(c->count==1);
    puts("boss diagnostic bounded capture: OK");
}
