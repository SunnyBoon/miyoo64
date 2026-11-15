#include <assert.h>
#include <stdarg.h>
#include <stdint.h>
#include <string.h>
#include "port_segments.c"
unsigned char Vine0Base[0x4000], Vine1Base[0x3000], Vine2Base[0x3000], Vine3Base[0x2000];
static int16_t data[] = {5, 1, -22, 8, 10, 102, 100, -10, 1, 1, 0};
const typeof(*port_code_segs) port_code_segs[] = {
 {0x100,0x118,"world_model_anim_kzn_00",(uint8_t*)data,(uint8_t*)(data+11)},
 {0x200,0x218,"normal_overlay",(uint8_t*)data,(uint8_t*)(data+11)}
};
const int port_code_segs_n=2;
void port_log(const char *fmt, ...) {(void)fmt;}
int main(void) {
 for (int i=0;i<4;i++) {
  unsigned char *v[]={Vine0Base,Vine1Base,Vine2Base,Vine3Base};
  memset(v[i],0xcc,32); assert(port_vine_dma(0,0x100,v[i],16)); assert(port_vine_dma(0,0x110,v[i]+16,8));
  int16_t *x=(int16_t*)v[i]; assert(x[0]==5 && x[2]==-22 && x[3]==8 && x[5]==102 && x[7]==-10 && x[8]==1);
  assert(v[i][22]==0 && v[i][23]==0 && v[i][24]==0xcc);
  data[2]=-77; assert(port_vine_dma(0,0x100,v[i],24)); assert(x[2]==-77); data[2]=-22;
  assert(!port_vine_dma(1,0x200,v[i],24)); assert(!port_vine_dma(0,0x101,v[i],2)); assert(!port_vine_dma(0,0x117,v[i]+23,2));
 }
 return 0;
}
