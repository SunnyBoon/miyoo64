#include <assert.h>
#include <stdarg.h>
#include <stdint.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>
#include <signal.h>
#include "port_segments.c"
unsigned char Vine0Base[0x4000], Vine1Base[0x3000], Vine2Base[0x3000], Vine3Base[0x2000];
unsigned char D_80200000[0x4000], D_80204000[0x3000], D_80207000[0x3000], D_8020A000[0x6000];
static int16_t data[]={5,1,-22,8,10,102,100,-10,1,1,0};
static int16_t large[0x2800];
#define ENTRY(i,n) {0x1000*(i+1),0x1000*(i+1)+24,"world_model_anim_kzn_" n,(uint8_t*)data,(uint8_t*)(data+11)}
const typeof(*port_code_segs) port_code_segs[]={
 ENTRY(0,"00"),ENTRY(1,"0F"),ENTRY(2,"11"),ENTRY(3,"1F"),ENTRY(4,"20"),ENTRY(5,"22"),ENTRY(6,"24"),
 {0x8000,0x8018,"normal_overlay",(uint8_t*)data,(uint8_t*)(data+11)},
 {0x10000,0x15000,"world_model_anim_kzn_24",(uint8_t*)large,(uint8_t*)(large+0x2800)}
};
const int port_code_segs_n=9;
void port_log(const char *fmt, ...) {(void)fmt;}
int main(void) {
 unsigned char *v[]={Vine0Base,Vine1Base,Vine2Base,Vine3Base,D_80200000,D_80204000,D_80207000,D_8020A000};
 size_t sizes[]={sizeof Vine0Base,sizeof Vine1Base,sizeof Vine2Base,sizeof Vine3Base,sizeof D_80200000,sizeof D_80204000,sizeof D_80207000,sizeof D_8020A000};
 for (unsigned i=0;i<8;i++) {
  assert(sizes[i]==(i==0||i==4 ? 0x4000 : i==3 ? 0x2000 : i==7 ? 0x6000 : 0x3000));
  for (unsigned j=0;j<7;j++) {
   uint32_t start=0x1000*(j+1); memset(v[i],0xcc,32);
   assert(port_vine_dma(j,start,v[i],16)); assert(port_vine_dma(j,start+16,v[i]+16,8));
   int16_t *x=(int16_t*)v[i]; assert(x[0]==5 && x[2]==-22 && x[3]==8 && x[5]==102 && x[7]==-10 && x[8]==1);
   assert(v[i][22]==0 && v[i][23]==0 && v[i][24]==0xcc);
   data[2]=-77; assert(port_vine_dma(j,start,v[i],24)); assert(x[2]==-77); data[2]=-22;
   assert(!port_vine_dma(j,start+1,v[i],2)); assert(!port_vine_dma(j,start+23,v[i]+23,2));
  }
  assert(!port_vine_dma(7,0x8000,v[i],24));
 }
 large[0]=123; large[0x27ff]=-123;
 assert(port_vine_dma(8,0x10000,D_8020A000,0x5000));
 assert(((int16_t*)D_8020A000)[0]==123 && ((int16_t*)D_8020A000)[0x27ff]==-123);
 pid_t p=fork(); assert(p>=0); if (!p) {port_vine_dma(8,0x10000,D_80200000,0x100); _exit(0);}
 int st; assert(waitpid(p,&st,0)==p); assert(WIFSIGNALED(st) && WTERMSIG(st)==SIGABRT);
 return 0;
}
