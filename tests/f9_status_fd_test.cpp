#include "subsystems/physics/f9_status_fd.h"
#include "vrik/visible_failure.h"
#include <cstdio>
#include <cstring>
int main(){using namespace kcd2vr::f9_status_fd;int n=0,f=0;auto ck=[&](bool b,const char* w){++n;if(!b){++f;std::printf("FAIL %s\n",w);}};
 for(int r=0;r<=3;++r)for(int a=0;a<2;++a)for(int b=0;b<2;++b)for(int c=0;c<2;++c){const char* t=rungText(r,a,b,c);ck(t&&kcd2vr::visible_failure::safe(t),"rung text safe");}
 ck(std::strstr(rungText(3,false,true,true),"corpse grab OFF")!=nullptr,"rung3 drag off says so");
 for(int m=0;m<16;++m){const char* t=rung3Text(m&1,m&2,m&4,m&8);ck(t&&kcd2vr::visible_failure::safe(t),"rung3 list safe");
  ck(std::strstr(t,(m&1)?"corpse grab ON":"corpse grab OFF")&&std::strstr(t,"living reactions PAUSED")&&
     !std::strstr(t,"npc touch ON")&&!std::strstr(t,"hit impacts ON")&&std::strstr(t,(m&8)?"prop wake ON":"prop wake OFF"),"rung3 reports suspended living actuation and independent corpse/prop selections");}
 ck(std::strstr(rungText(2,1,1,1),"right hand")!=nullptr,"rung2 tells the hand rule");
 for(unsigned s=0;s<16;++s){const char* t=heldRefusalText(s);if(t)ck(kcd2vr::visible_failure::safe(t),"refusal safe");}
 ck(heldRefusalText(0)==nullptr&&heldRefusalText(11)==nullptr,"disabled/retiring silent");
 ck(std::strstr(heldRefusalText(3),"draw")!=nullptr,"NO_HELD_SWORD = draw");
 ck(heldRefusalText(13)!=nullptr,"ready says active");
 ck(!keepCapture(0)&&keepCapture(1)&&keepCapture(2)&&keepCapture(3)&&!keepCapture(4),"capture policy");
 std::printf("f9_status_fd %d checks, %d failures\n",n,f);return f?1:0;}
