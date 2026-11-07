#include "../src/core/native_grasp13.h"
#include <cstdio>
using namespace kcd2vr::grasp13;
int main(){
 Binding b{};b.character=1;b.manager=2;b.item=3;b.logic=4;b.token=5;b.key=6;b.epoch=7;b.entity=8;b.actor=9;b.side=0;b.attached=b.place=true;
 Acquisition54 a{};a.character=b.character;a.action=10;a.scope=11;a.serial=12;a.epoch=b.epoch;a.startedAt=900;a.tick=1000;a.side=b.side;a.active=true;
 a.graspToken=b.token;a.graspKey=b.key;a.graspItem=b.item;a.graspEntity=b.entity;
 FingerRotations54 q{};for(auto& r:q)r={0,0,0,1};unsigned checks=0;
 auto check=[&](bool ok,const char* why){++checks;if(!ok){std::printf("FAIL %s\n",why);return false;}return true;};
 NativeFinger54 good;if(!check(good.capture(b,a,1000,q,15),"same native pickup resource captures"))return 1;
 // Same character/hand/time is insufficient: this may be another pickup action.
 for(int field=0;field<4;++field){auto wrong=a;
  if(field==0)++wrong.graspToken;if(field==1)++wrong.graspKey;if(field==2)++wrong.graspItem;if(field==3)++wrong.graspEntity;
  NativeFinger54 cache;if(!check(!cache.capture(b,wrong,1000,q,15)&&!cache.ready,"different pickup resource must not be learned as held fingers"))return 1;
 }
 auto absent=a;absent.graspToken=0;NativeFinger54 unjoined;if(!check(!unjoined.capture(b,absent,1000,q,15),"active action before grasp binding cannot capture"))return 1;
 b.logic=0;a={};if(!check(good.current(b,15),"same item retains inert native pose after Finish retires logic"))return 1;
 ++b.token;if(!check(!good.current(b,15)&&!good.ready,"new resource retires old pose"))return 1;
 std::printf("PASS native grasp finger exact resource identity: %u checks\n",checks);return 0;
}
