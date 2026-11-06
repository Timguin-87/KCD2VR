#include "subsystems/dialogues/lively_npc_d73.h"
#include <cassert>
#include <cstdio>
#include <limits>
int main(){
 using namespace kcd2vr::lively_npc_d73;
 assert(!Enabled()&&!cleanup.load()&&!ready.load());
 SetListeningGestures(true);SetListeningGestures(false);assert(!cleanup.load());
 ready.store(true); // explicit session preparation succeeds before enabling
 SetEnabled(true);auto gen=mode.load();
 Frame f{100,gen,17,23,false,true,true,1,2,3,0,1,0};
 assert(Current(f,180,gen)&&!Current(f,181,gen)&&!Current(f,99,gen));
 Speaker s{41,17,23,gen,100,true};assert(Speaking(s,f,200));
 auto wrong=f;wrong.scene=24;assert(!Speaking(s,wrong,200));
 wrong=f;wrong.actor=18;assert(!Speaking(s,wrong,200));
 wrong=f;wrong.generation+=2;assert(!Speaking(s,wrong,200));
 s.henry=false;assert(!Speaking(s,f,200));s.henry=true;
 assert(!Speaking(s,f,60101));
 assert(ListeningD81(s,f,200));s.henry=false;assert(!ListeningD81(s,f,200));
 s.clearedD81=true;assert(ListeningD81(s,f,200)&&!Speaking(s,f,200));
 assert(!ListeningD81(s,wrong,200));s.hud=0;assert(!ListeningD81(s,f,200));
 s.hud=41;assert(!ListeningD81(s,f,60101)&&!ListeningD81(s,f,99));
 wrong=f;wrong.x=std::numeric_limits<float>::quiet_NaN();assert(!Current(wrong,100,gen));
 SetEnabled(false);assert(!Current(f,100,mode.load())&&cleanup.load());
 SetEnabled(true);assert(!Current(f,100,mode.load()));
 auto same=mode.load();SetEnabled(true);assert(mode.load()==same);
 SetEnabled(false);cleanup.store(false); // retirement acknowledged
 SetListeningGestures(true);SetListeningGestures(false);assert(!cleanup.load());
 std::puts("D73 NPC generation, actor/scene speaker join, stale frame and OFF retirement PASS");
}
