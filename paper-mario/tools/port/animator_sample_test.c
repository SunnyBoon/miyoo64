#include <assert.h>
#include "animator_sample.h"
unsigned port_frame;
int main(void) {
 int script=0,model=0,anim=0,node=0,other=0; float xyz[3]={1,2,3},m[16]={0}; const struct AnimatorSamples *p;
 animator_profile_select(&script,3); animator_profile_node(&model,&anim,&node,7,0,1,1,1,xyz,xyz,m); animator_profile_unselect(); assert(animator_profile_result()->count==0 && !animator_hook_enabled);
 animator_profile_arm(4);
 for(unsigned i=0;i<16;i++) {
  port_frame=100+i;
  if(i){animator_hook_begin(7); animator_hook_resolve(7,&anim,0,0,2,1); animator_hook_timer(7,1); animator_hook_command(7); animator_hook_end(7,ANIM_HOOK_COMPLETE,&anim,0,1);}
  animator_profile_select(&script,3); animator_profile_node(&model,&anim,&node,7,0,1,1,1,xyz,xyz,m); animator_profile_unselect(); animator_profile_head(&script,3,xyz);
 }
 p=animator_profile_result(); assert(p->count==16 && !p->incomplete && p->samples[0].marker==0 && p->samples[1].marker==ANIM_HOOK_COMPLETE && p->samples[15].head[1]==2);
 animator_profile_arm(5); port_frame=1; animator_profile_select(&script,3); animator_profile_node(&model,&anim,&node,7,0,1,1,1,xyz,xyz,m); animator_profile_unselect(); animator_profile_head(&script,3,xyz);
 port_frame=2; animator_profile_select(&script,3); animator_profile_node(&model,&other,&node,7,0,1,1,1,xyz,xyz,m); assert(animator_profile_result()->incomplete);
 animator_profile_arm(6); animator_profile_reload(); assert(animator_profile_result()->incomplete);
 animator_profile_stop(); return 0;
}
