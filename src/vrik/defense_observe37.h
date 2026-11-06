#pragma once
#include <cstdint>
#include <cstddef>
#include <cstdio>
// C37 DEFENSE OBSERVATION (F3 only, read-only). WHGame 6A350E20 retail layouts, see REPORT_DEFENSE.md:
//  Henry state +30 phase int (93A0C8), +4B0/+4E0 bool C_ModelProperty<bool> vft3B0A6A0 (72F790 writes via 72FA34).
//  NPC automation = *(*(*(npcActor+248)+48)+108) (7302CC; C55D84/17A41B0/C56368 read +108). Block vft 3A8A8C0.
//  NPC state +630 C_ModelContextProperty<E_BlockModeContext> vft3B0A968: per-context bytes +18..+1C, max +1D (395D9C).
//  NPC state +680 bool vft3B0A6A0 (C56314 refuses defense when set); +510 C_ModelRefProperty<bool> vft3B0A7E0 count +8 (188B004).
//  NPC guard mode byte = *(npcActor+248)+4C8+88 (72F644 -> 72FB58 -> 72FC2C). NPC actor +280 plain byte: automation ctor
//  C57EC8 clears it, dtor C57FBC sets it; 9089AC only builds the phase-8 block trigger action while it is set.
//  Action manager = *(actor+200), slot array [+38,+40) of action pointers (9CC3D4/4A7658); base action +28 active,
//  +38 priority, +3C slot (90D31C ctor; Attack getters 1A731D0/1A73A40/1A72810). Other classes: raw fields only.
// No virtual call, no write, no allocation (7302CC's lazy 10EE974 allocation is never reached from here).
namespace kcd2vr::defense37 {
constexpr std::uintptr_t kBoolVft=0x3B0A6A0,kRefBoolVft=0x3B0A7E0,kBlockModeVft=0x3B0A968;
constexpr std::uint32_t kAutoBlock=0x3A8A8C0,kAutoNoAction=0x3A8A840,kAutoDefense=0x3A8A730,kAutoPerfect=0x3A8A5F0,
    kAutoSyncPerfect=0x3C40DA0,kAutoMaster=0x3A8A560,kAutoDodge=0x3C3A298,kNoVft=0xFFFFFFFFu;
constexpr unsigned kSlots=6;
struct Slot {std::uintptr_t ptr=0;std::uint32_t vft=kNoVft;int priority=-99,slot=-99,active=-1;};
struct Henry {bool known=false;int phase=-1,reach=-1,almost=-1,blockDowngrade390=-1;}; // +390: C55DF0 turns a type-5 Block roll into NoAction
struct Npc {
    bool known=false;int phase=-1,free280=-1,refuse680=-1,almost510=-1,blockMode=-1,blockAuto=-1,guardMode=-1;
    std::uintptr_t automation=0;std::uint32_t automationVft=kNoVft;int automationOwner=-1,automationTargetsHenry=-1;
    int slotCount=-1;Slot slots[kSlots]{};
};
inline const char* automationName(std::uintptr_t automation,std::uint32_t vft) noexcept {
    if(!automation)return "NONE";
    switch(vft){case kAutoBlock:return "Block";case kAutoNoAction:return "NoAction";case kAutoDefense:return "DefenseAction";
        case kAutoPerfect:return "PerfectBlock";case kAutoSyncPerfect:return "SyncPerfectBlock";case kAutoMaster:return "MasterStrike";
        case kAutoDodge:return "Dodge";default:return "OTHER";}
}
inline std::uint32_t Rva(std::uintptr_t base,std::uintptr_t vft) noexcept {return vft>base&&vft-base<0x6000000?std::uint32_t(vft-base):kNoVft;}
template<class Copy> int BoolProp(Copy copy,std::uintptr_t base,std::uintptr_t prop) noexcept {
    std::uintptr_t vft=0;unsigned char v=255;
    return prop&&copy(prop,&vft,sizeof(vft))&&vft==base+kBoolVft&&copy(prop+8,&v,1)&&v<=1?int(v):-1;
}
template<class Copy> Slot ReadAction(Copy copy,std::uintptr_t base,std::uintptr_t action) noexcept {
    Slot s{};s.ptr=action;if(!action)return s;std::uintptr_t vft=0;std::int32_t pr=0,sl=0;unsigned char a=255;
    if(copy(action,&vft,sizeof(vft)))s.vft=Rva(base,vft);
    if(copy(action+0x38,&pr,4))s.priority=pr;
    if(copy(action+0x3C,&sl,4))s.slot=sl;
    if(copy(action+0x28,&a,1)&&a<=1)s.active=a;
    return s;
}
template<class Copy> Henry ReadHenry(Copy copy,std::uintptr_t base,std::uintptr_t state) noexcept {
    Henry h{};if(!base||!state)return h;std::int32_t phase=0;
    if(copy(state+0x38,&phase,4))h.phase=phase;
    h.reach=BoolProp(copy,base,state+0x4B0);h.almost=BoolProp(copy,base,state+0x4E0);h.blockDowngrade390=BoolProp(copy,base,state+0x390);
    h.known=h.phase>=0&&h.reach>=0&&h.almost>=0;return h;
}
template<class Copy> Npc ReadNpc(Copy copy,std::uintptr_t base,std::uintptr_t actor,std::uintptr_t state,std::uintptr_t henryActor) noexcept {
    Npc n{};if(!base||!actor||!state)return n;
    std::int32_t phase=0,count=0;std::uintptr_t vft=0,ai=0,owner=0,automation=0;
    if(copy(state+0x38,&phase,4))n.phase=phase;
    {unsigned char f=255;if(copy(actor+0x280,&f,1)&&f<=1)n.free280=f;}
    n.refuse680=BoolProp(copy,base,state+0x680);
    if(copy(state+0x510,&vft,sizeof(vft))&&vft==base+kRefBoolVft&&copy(state+0x518,&count,4)&&count>=0&&count<256)n.almost510=count;
    unsigned char mode[6]{};
    if(copy(state+0x630,&vft,sizeof(vft))&&vft==base+kBlockModeVft&&copy(state+0x630+0x18,mode,sizeof(mode))&&mode[5]<=2&&mode[1]<=2){
        n.blockMode=mode[5];n.blockAuto=mode[1];} // context 1 = automation (C58C8C/C57FBC pass r8b=1)
    if(copy(actor+0x248,&ai,sizeof(ai))&&ai){ // never allocate: a null +248 means no AI combat object yet
        unsigned char g=255;if(copy(ai+0x4C8+0x88,&g,1))n.guardMode=g;
        if(copy(ai+0x48,&owner,sizeof(owner))&&owner&&copy(owner+0x108,&automation,sizeof(automation))){
            n.automation=automation;
            if(automation&&copy(automation,&vft,sizeof(vft)))n.automationVft=Rva(base,vft);
            // +8 owner / +30 attacker: DefenseAction-derived layout (C577F8 ctor: Block, NoAction) only.
            std::uintptr_t a8=0,a30=0;
            if(automation&&(n.automationVft==kAutoBlock||n.automationVft==kAutoNoAction)&&
               copy(automation+8,&a8,sizeof(a8))&&copy(automation+0x30,&a30,sizeof(a30))){
                n.automationOwner=a8==actor?1:0;n.automationTargetsHenry=henryActor&&a30==henryActor?1:0;}
        }
    }
    std::uintptr_t mgr=0,begin=0,end=0;
    if(copy(actor+0x200,&mgr,sizeof(mgr))&&mgr&&copy(mgr+0x38,&begin,sizeof(begin))&&copy(mgr+0x40,&end,sizeof(end))&&
       end>=begin&&(end-begin)%8==0&&(end-begin)/8<=64){
        n.slotCount=int((end-begin)/8);
        for(unsigned i=0;i<kSlots&&i<unsigned(n.slotCount);++i){std::uintptr_t a=0;if(copy(begin+8*i,&a,sizeof(a)))n.slots[i]=ReadAction(copy,base,a);}
    }
    n.known=n.phase>=0;return n;
}
inline bool Same(const Slot& a,const Slot& b) noexcept {return a.ptr==b.ptr&&a.vft==b.vft&&a.priority==b.priority&&a.slot==b.slot&&a.active==b.active;}
inline bool Same(const Henry& a,const Henry& b) noexcept {return a.known==b.known&&a.phase==b.phase&&a.reach==b.reach&&a.almost==b.almost&&a.blockDowngrade390==b.blockDowngrade390;}
inline bool Same(const Npc& a,const Npc& b) noexcept {
    if(a.known!=b.known||a.phase!=b.phase||a.free280!=b.free280||a.refuse680!=b.refuse680||a.almost510!=b.almost510||a.blockMode!=b.blockMode||
       a.blockAuto!=b.blockAuto||a.guardMode!=b.guardMode||a.automation!=b.automation||a.automationVft!=b.automationVft||
       a.automationTargetsHenry!=b.automationTargetsHenry||a.slotCount!=b.slotCount)return false;
    for(unsigned i=0;i<kSlots;++i){if(!Same(a.slots[i],b.slots[i]))return false;}
    return true;
}
// Where did an accepted reaction land, relative to the actions that were installed before 90C850?
// 9CC3D4 installs into slots[action+3C]; same-slot replacement needs new.priority > current.priority
// (C_CombatActorActionAttack vf+38 = 14962EC). Other slots are never stopped by the manager itself.
enum class Landing {Unknown,NotInstalled,EmptySlot,ReplacedSameSlot,ParallelToActive};
struct Dispatch {bool captured=false;int result=-1;std::uintptr_t henryActor=0,target=0;Henry henry{};Npc before{},after{};Slot reaction{};};
inline Landing Classify(const Dispatch& d,int* index=nullptr) noexcept {
    if(index)*index=-1;
    if(!d.captured||!d.reaction.ptr||d.before.slotCount<0||d.after.slotCount<0)return Landing::Unknown;
    int at=-1;for(unsigned i=0;i<kSlots;++i){if(d.after.slots[i].ptr==d.reaction.ptr){at=int(i);break;}}
    if(index)*index=at;
    if(at<0)return d.after.slotCount>int(kSlots)?Landing::Unknown:Landing::NotInstalled;
    const auto& old=d.before.slots[at];
    if(old.ptr&&old.ptr!=d.reaction.ptr&&old.active==1)return Landing::ReplacedSameSlot;
    for(unsigned i=0;i<kSlots;++i)if(int(i)!=at&&d.before.slots[i].ptr&&d.before.slots[i].active==1&&d.after.slots[i].ptr==d.before.slots[i].ptr)
        return Landing::ParallelToActive;
    return Landing::EmptySlot;
}
inline const char* LandingName(Landing l) noexcept {
    switch(l){case Landing::NotInstalled:return "NOT_INSTALLED";case Landing::EmptySlot:return "EMPTY_SLOT";
        case Landing::ReplacedSameSlot:return "REPLACED_SAME_SLOT";case Landing::ParallelToActive:return "PARALLEL_TO_ACTIVE_SLOT";default:return "UNKNOWN";}
}
inline int FormatSlots(char* out,std::size_t n,const Npc& s) noexcept {
    if(!out||!n)return 0;
    std::size_t used=0;out[0]=0;
    for(unsigned i=0;i<kSlots&&int(i)<s.slotCount;++i){
        const auto& a=s.slots[i];const int w=a.ptr?std::snprintf(out+used,n-used,"%s%u:%X/p%d/s%d/a%d",used?",":"",i,a.vft,a.priority,a.slot,a.active):
            std::snprintf(out+used,n-used,"%s%u:-",used?",":"",i);
        if(w<0||std::size_t(w)>=n-used)break;
        used+=std::size_t(w);}
    return int(used);
}
inline int FormatNpc(char* out,std::size_t n,const Npc& s) noexcept {
    char slots[200]{};FormatSlots(slots,sizeof(slots),s);
    return std::snprintf(out,n,"npcPhase=%d automation=%s automationVft=%X automationOwner=%d automationTargetsHenry=%d blockMode630=%d blockModeAutomation=%d actorFree280=%d refuse680=%d almost510=%d guardMode88=%d slots=%d[%s]",
        s.phase,automationName(s.automation,s.automationVft),s.automationVft,s.automationOwner,s.automationTargetsHenry,s.blockMode,s.blockAuto,s.free280,
        s.refuse680,s.almost510,s.guardMode,s.slotCount,slots);
}
}
