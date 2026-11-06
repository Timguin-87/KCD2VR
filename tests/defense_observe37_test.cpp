// C37 read-only defense layouts: exact type checks, no allocation path, edge comparison and reaction landing.
#include "vrik/defense_observe37.h"
#include <array>
#include <cassert>
#include <cstring>
#include <string>
using namespace kcd2vr::defense37;
struct Mem {alignas(16) std::array<unsigned char,0x1000> b{};std::uintptr_t at(std::size_t o){return reinterpret_cast<std::uintptr_t>(b.data())+o;}};
static Mem henryState,npcActor,npcState,ai,owner,automation,mgr,slotArray,attack,reaction;static unsigned copies=0;
static Mem* all[]{&henryState,&npcActor,&npcState,&ai,&owner,&automation,&mgr,&slotArray,&attack,&reaction};
static bool copy(std::uintptr_t p,void* out,std::size_t n){++copies;
 for(auto* m:all){const auto s=m->at(0);if(p>=s&&n<=m->b.size()&&p-s<=m->b.size()-n){std::memcpy(out,reinterpret_cast<const void*>(p),n);return true;}}return false;}
template<class T> void put(Mem& m,std::size_t o,T v){std::memcpy(m.b.data()+o,&v,sizeof(v));}
constexpr std::uintptr_t base=0x180000000ull;
int main(){
 const auto hs=henryState.at(0),na=npcActor.at(0),ns=npcState.at(0),henryActor=std::uintptr_t(0x7777);
 // Henry: phase int +38, +4B0/+4E0 exact bool property type.
 put(henryState,0x38,4);put(henryState,0x4B0,base+kBoolVft);put(henryState,0x4B8,std::uint8_t(1));
 put(henryState,0x4E0,base+kBoolVft);put(henryState,0x4E8,std::uint8_t(0));
 auto h=ReadHenry(copy,base,hs);assert(h.known&&h.phase==4&&h.reach==1&&h.almost==0&&h.blockDowngrade390==-1);
 put(henryState,0x390,base+kBoolVft);put(henryState,0x398,std::uint8_t(1));h=ReadHenry(copy,base,hs);assert(h.blockDowngrade390==1);
 put(henryState,0x4E0,base+0x1234);h=ReadHenry(copy,base,hs);assert(h.almost==-1&&!h.known); // wrong property type is UNKNOWN, not false
 put(henryState,0x4E0,base+kBoolVft);
 // NPC without AI object: never treated as "no automation" proof beyond the read, and no allocation route exists.
 put(npcState,0x38,1);auto n=ReadNpc(copy,base,na,ns,henryActor);
 assert(n.known&&n.automation==0&&n.guardMode==-1&&n.slotCount==-1&&n.blockMode==-1&&n.refuse680==-1&&n.almost510==-1);
 assert(std::string(automationName(n.automation,n.automationVft))=="NONE");
 // Typed properties.
 put(npcState,0x680,base+kBoolVft);put(npcState,0x688,std::uint8_t(0));
 put(npcState,0x510,base+kRefBoolVft);put(npcState,0x518,2);
 put(npcState,0x630,base+kBlockModeVft);put(npcState,0x630+0x19,std::uint8_t(1));put(npcState,0x630+0x1D,std::uint8_t(1));
 // AI object chain: actor+248 -> ai; ai+48 -> owner; owner+108 -> Block automation (+8 npc actor, +30 Henry).
 put(npcActor,0x248,ai.at(0));put(ai,0x4C8+0x88,std::uint8_t(2));put(ai,0x48,owner.at(0));put(owner,0x108,automation.at(0));
 put(automation,0,base+kAutoBlock);put(automation,8,na);put(automation,0x30,henryActor);
 // Action manager actor+200 with two slots: slot0 = active attack (priority 5), slot1 empty.
 put(npcActor,0x200,mgr.at(0));put(mgr,0x38,slotArray.at(0));put(mgr,0x40,slotArray.at(16));
 put(slotArray,0,attack.at(0));put(slotArray,8,std::uintptr_t(0));
 put(attack,0,base+0x3A61C58);put(attack,0x28,std::uint8_t(1));put(attack,0x38,5);put(attack,0x3C,0);
 n=ReadNpc(copy,base,na,ns,henryActor);
 put(npcActor,0x280,std::uint8_t(0));n=ReadNpc(copy,base,na,ns,henryActor);
 assert(n.free280==0&&n.refuse680==0&&n.almost510==2&&n.blockMode==1&&n.blockAuto==1&&n.guardMode==2);
 assert(n.automationVft==kAutoBlock&&n.automationOwner==1&&n.automationTargetsHenry==1);
 assert(std::string(automationName(n.automation,n.automationVft))=="Block");
 assert(n.slotCount==2&&n.slots[0].vft==0x3A61C58&&n.slots[0].priority==5&&n.slots[0].slot==0&&n.slots[0].active==1&&!n.slots[1].ptr);
 char text[600]{};FormatNpc(text,sizeof(text),n);assert(std::string(text).find("automation=Block")!=std::string::npos);
 assert(std::string(text).find("slots=2[0:3A61C58/p5/s0/a1,1:-]")!=std::string::npos);
 // Edge comparison.
 auto same=ReadNpc(copy,base,na,ns,henryActor);assert(Same(n,same));
 put(npcState,0x630+0x1D,std::uint8_t(0));auto changed=ReadNpc(copy,base,na,ns,henryActor);assert(!Same(n,changed));
 // Unknown automation class: +8/+30 are not interpreted.
 put(automation,0,base+0x3A8A5F0);n=ReadNpc(copy,base,na,ns,henryActor);assert(n.automationOwner==-1&&std::string(automationName(n.automation,n.automationVft))=="PerfectBlock");
 // Implausible slot array is not walked.
 put(mgr,0x40,slotArray.at(3));n=ReadNpc(copy,base,na,ns,henryActor);assert(n.slotCount==-1);
 put(mgr,0x40,slotArray.at(16));
 // Landing classification around one 90C850 call.
 put(reaction,0,base+0x3A00000);put(reaction,0x28,std::uint8_t(1));put(reaction,0x38,9);put(reaction,0x3C,0);
 Dispatch d{};d.captured=true;d.result=1;d.before=ReadNpc(copy,base,na,ns,henryActor);d.reaction=ReadAction(copy,base,reaction.at(0));
 put(slotArray,0,reaction.at(0));d.after=ReadNpc(copy,base,na,ns,henryActor);int at=-2;
 assert(Classify(d,&at)==Landing::ReplacedSameSlot&&at==0);
 // Reaction installed in slot1 while the attack keeps slot0: parallel, the manager stopped nothing.
 put(slotArray,0,attack.at(0));put(slotArray,8,reaction.at(0));d.after=ReadNpc(copy,base,na,ns,henryActor);
 assert(Classify(d,&at)==Landing::ParallelToActive&&at==1);
 // Not installed (e.g. queued/refused) and empty-slot cases.
 put(slotArray,8,std::uintptr_t(0));d.after=ReadNpc(copy,base,na,ns,henryActor);assert(Classify(d,&at)==Landing::NotInstalled&&at==-1);
 put(attack,0x28,std::uint8_t(0));d.before=ReadNpc(copy,base,na,ns,henryActor);put(slotArray,0,reaction.at(0));d.after=ReadNpc(copy,base,na,ns,henryActor);
 assert(Classify(d,&at)==Landing::EmptySlot&&at==0);
 Dispatch none{};assert(Classify(none)==Landing::Unknown);
 // Null inputs perform no reads.
 const auto before=copies;(void)ReadNpc(copy,base,0,ns,0);(void)ReadHenry(copy,base,0);assert(copies==before);
 return 0;
}
