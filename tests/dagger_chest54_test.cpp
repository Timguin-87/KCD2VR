#include "vrik/dagger_chest54.h"
#include <cassert>
#include <cstdio>
#include <cstring>
int main(){namespace d=kcd2vr::dagger54;d::State s{};d::Input i{0x123,2,3,1000,77,true,false};
 auto p=d::advance(s,i);assert(p.run&&p.scan&&p.enabled&&p.generation==1);
 for(unsigned t=1001;t<2000;++t){i.tick=t;p=d::advance(s,i);assert(!p.run);}
 i.tick=2000;p=d::advance(s,i);assert(p.run&&!p.scan&&p.enabled);
 i.menu=true;++i.tick;p=d::advance(s,i);assert(p.run&&!p.scan&&!p.enabled);
 ++i.tick;p=d::advance(s,i);assert(!p.run);
 i.menu=false;++i.tick;p=d::advance(s,i);assert(p.run&&p.scan&&p.generation==2);
 ++i.request;++i.tick;p=d::advance(s,i);assert(p.run&&p.scan&&p.generation==3);
 ++i.epoch;++i.tick;p=d::advance(s,i);assert(p.run&&p.scan&&p.generation==4);
 i.owner=0;i.entity=0;++i.tick;p=d::advance(s,i);assert(p.run&&!p.enabled);
 ++i.tick;p=d::advance(s,i);assert(!p.run);
 i.owner=0x456;i.entity=88;++i.tick;p=d::advance(s,i);assert(p.run&&p.scan&&p.generation==5);
 char small[12]{},cmd[800]{};assert(!d::command(i,p,small,sizeof(small)));
 assert(d::command(i,p,cmd,sizeof(cmd))&&std::strstr(cmd,"refresh,88,5,'kcd2vr_chest54_5'"));
 assert(std::strstr(d::script,"GetInventoryTable")&&std::strstr(d::script,"ItemManager.GetItem"));
 assert(!std::strstr(d::script,"SpawnEntity")&&!std::strstr(d::script,"AddItem")&&!std::strstr(d::script,"Physicalize"));
 std::puts("dagger54 edge-only discovery,1Hz selected-item validation, ownership retirement PASS");
}
