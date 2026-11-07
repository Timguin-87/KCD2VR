// Extracted production alignment and queued-command predicates, with memory and
// copied ownership mailboxes substituted at the process boundary only.
#include <array>
#include <atomic>
#include <algorithm>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <vector>
#include "vrik/align_veto_receipts24.h"
#include "vrik/fist_action_h12.h"
static std::uint64_t nowD37=1000;
static std::uintptr_t baseD37=0x180000000ULL;
static std::uint64_t GetTickCount64(){return nowD37;}
static std::uintptr_t WhBase(){return baseD37;}
static std::vector<std::pair<std::uintptr_t,std::size_t>> memoryD37;
template<std::size_t N> struct AreaD37 {
    std::array<unsigned char,N> bytes{};
    AreaD37(){memoryD37.emplace_back(address(),N);}
    std::uintptr_t address()const{return reinterpret_cast<std::uintptr_t>(bytes.data());}
    void* ptr(){return bytes.data();}
    template<class T> void put(std::size_t offset,T value){std::memcpy(bytes.data()+offset,&value,sizeof(value));}
};
static bool ClockRead50(std::uintptr_t p,void* out,std::size_t n){
    for(auto [start,size]:memoryD37)if(p>=start&&p-start<=size&&n<=size-(p-start)){std::memcpy(out,reinterpret_cast<void*>(p),n);return true;}
    return false;
}
template<class T> static bool Read(const void* p,T& out){return ClockRead50(reinterpret_cast<std::uintptr_t>(p),&out,sizeof(out));}
static bool PhysVft6698(void* p,std::uintptr_t offset){std::uintptr_t v=0;return Read(p,v)&&v==baseD37+offset;}
struct ReadScope101R2{};
struct NativeOwner104{void* player=nullptr;void* entity=nullptr;void* actor=nullptr;void* state=nullptr;std::uint64_t epoch=1;};
static NativeOwner104 nativeD37;
static bool resolveD37=true,observeD37=true,selectedD37=true,claimD37=false,bodyRequiredD37=false,referenceD37=false;
static bool NativeResolve104(void* p,NativeOwner104& o){o=nativeD37;return resolveD37&&p==o.player;}
namespace nc104 {struct Observed{int phase=0,type=0;bool requestCandidate=true;};}
static bool NativeObserve104(const NativeOwner104& o,nc104::Observed& s){return observeD37&&Read(static_cast<unsigned char*>(o.state)+0x38,s.phase);}
struct FistOwner101{void* player=nullptr;void* manager=nullptr;std::uint64_t epoch=1,request=1;bool empty=true;};
static FistOwner101 handsD37;
static bool FistReadOwner101(FistOwner101& o){o=handsD37;return o.player!=nullptr;}
static bool FistSelected101(){return selectedD37;}
struct TrackingSnapshot{bool allowed=true,head_valid=true,right_valid=true,left_valid=true,head_held=false,right_held=false,left_held=false;std::uint64_t tick=1000;float right_grip=1,right_trigger=1,left_grip=1,left_trigger=1;};
static TrackingSnapshot trackingD37;
static bool ReadTrackingSnapshot(TrackingSnapshot& t){t=trackingD37;return true;}
static bool HandClaimBlockedT24(int,std::uintptr_t,std::uint64_t,std::uint64_t){return claimD37;}
namespace kcd2vr::contact176 {static bool fresh(std::uint64_t t,std::uint64_t n,std::uint64_t max){return t&&n>=t&&n-t<=max;}}
template<class T>struct MailD37{T value{};bool available=true;bool read(T& out){out=value;return available;}};
struct FistContactFrameH11{void* player=nullptr;bool completedH31=true,valid[2]{true,true};std::uint64_t epoch=1,request=1,tick=1000,sample=7;std::uintptr_t character=123;};
struct CommittedFrame6695{struct{std::uint64_t epoch=1;std::uintptr_t actor=123;std::uint64_t sample=7;}stamp;bool valid[2]{true,true};};
static MailD37<FistContactFrameH11> g_fist_contact_h11;
static MailD37<CommittedFrame6695> g_committed_frame6695;
struct PhysFrame6698{
    struct{bool allowed=true,attacks=true,tracked[2]{true,true};std::uint64_t tick=1000,sample=7,bodyEpoch=1;}input;
    struct{bool owned[2]{},active[2]{},handlingOnly55c1[2]{};}output;
    struct Profile{bool primary=false;std::uint64_t key=0;bool melee()const{return false;}}profiles[2];
    std::uint64_t request=1;void* player=nullptr;void* manager=nullptr;
};
static MailD37<PhysFrame6698> g_phys_frame6698;
static bool NativeHandKey6697(void*,int,std::uint64_t&){return false;}
static bool CombatSynchronized59c10r9(){return true;}
namespace combat_diag_probe {static bool GeometryLiveAction178r35(){return true;}}
namespace diagnostics::lab{enum class Key{F3};static int Get(Key){return 3;}static bool NativeReference(){return referenceD37;}}
namespace retail_build112{static bool Ready(std::uintptr_t){return true;}}
namespace animation_filter{
    static std::atomic<bool> nativeBodyOwned112{false};static std::atomic<std::uint64_t> combatPoseTick112{1000};
    static bool PlayerNativeBodyRequired112(){return bodyRequiredD37;}
}
static std::atomic<bool> g_combat_experiment175{true},g_physical_combat177{true},g_phys_armed6698{true},g_phys_fault6698{false},g_native_fault104{false},g_fist_fault101{false};
static std::atomic<bool> g_gameplay_allowed{true},g_ctx_dialogue{false},g_ctx_cutscene{false},g_ctx_scripted{false},g_ctx_menu{false};
static std::atomic<bool> g_head_valid{true},g_right_valid{true},g_left_valid{true},g_no_step55{true};
static std::atomic<bool> g_no_turn_d40{true}; // D40 optional policy; existing tests use default no-step path.
static std::atomic<int> g_weapon_control_mode{7};
static std::atomic<std::uintptr_t> g_motion_contract112{baseD37};
static std::atomic<std::uint64_t> g_weapon_request_epoch6694{1},g_pose_epoch6689{1},g_fist_epoch101{1},g_attack_timing_policy22{1},g_position_veto36{0};
static std::atomic<void*> g_last_player_entity{nullptr},g_henry_item_attach_manager{nullptr};
static unsigned originalCallsD37=0,checksD37=0;
static void OriginalD37(void*,const void*){++originalCallsD37;}
static void(*g_position_original36)(void*,const void*)=&OriginalD37;
#include "generated_fist_alignment_d37.inc"
static void ck(bool ok,const char* name){++checksD37;if(!ok){std::fprintf(stderr,"FAIL %u: %s\n",checksD37,name);std::exit(1);}}
int main(){
    AreaD37<0xB00> player,actor;AreaD37<0xD00> state;AreaD37<0x100> manager,slots,alignment,clip,records,commands,entity;
    AreaD37<0x200> action;AreaD37<0x280> row;AreaD37<0x60> helper,type;
    nativeD37={player.ptr(),entity.ptr(),actor.ptr(),state.ptr(),1};handsD37.player=player.ptr();g_last_player_entity=entity.ptr();
    player.put(0,baseD37+0x3AA4B98);player.put(0x38,entity.ptr());player.put(0x30,std::uint32_t(77));
    actor.put(0,baseD37+0x3A80610);actor.put(0x1F8,player.ptr());actor.put(0x200,manager.ptr());actor.put(0x250,alignment.ptr());
    manager.put(0x38,slots.ptr());slots.put(0,action.ptr());action.put(0,baseD37+0x3A61C58);
    action.put(0x40,manager.ptr());action.put(0x70,actor.ptr());action.put(0x3C,0);action.put(0x28,std::uint8_t(1));
    action.put(0xA0,helper.ptr());action.put(0x88,baseD37+0x3A61440);action.put(0x58,row.ptr());
    helper.put(0,baseD37+0x3A61040);helper.put(8,actor.ptr());helper.put(0x20,action.address()+0x88);
    row.put(0x148,type.ptr());type.put(8,5);state.put(0x38,4);
    alignment.put(0,baseD37+0x3A633B0);alignment.put(8,actor.ptr());
    alignment.put(0x58,records.ptr());alignment.put(0x60,records.address()+0x20);alignment.put(0x68,records.address()+0x100);
    records.put(0xC,std::uint32_t(77));records.put(0x10,std::uint8_t(2));records.put(0x18,clip.ptr());clip.put(0,baseD37+0x3BAD328);
    alignment.put(0x30,commands.ptr());alignment.put(0x38,commands.address()+0x30);alignment.put(0x40,commands.address()+0x100);commands.put(0,std::uint8_t(2));
    g_fist_contact_h11.value.player=player.ptr();
    auto bindPunch=[&](int side,int nativeHand,int kind){
        namespace fh=kcd2vr::fist_action_h12;
        type.put(8,kind);row.put(0x224,nativeHand);
        fh::Binding b{};b.action=action.address();b.helper=helper.address();b.row=row.address();b.nativeType=kind;b.nativeHand=nativeHand;
        b.intent.player=player.address();b.intent.actor=actor.address();b.intent.state=state.address();
        b.intent.kind=kcd2vr::attack_timing178r22c2::Kind::Punch;b.intent.side=side;
        b.intent.epoch=1;b.intent.request=1;b.intent.policy=1;b.intent.sample=7;b.intent.tick=1000;b.intent.serial=1;
        const auto token=fh::begin(actor.address(),action.address(),true);
        if(!fh::finish(b,token,true)){std::fprintf(stderr,"cannot publish actual H12 fixture binding\n");std::exit(2);}
        // Production palms.valid is a per-hand closed Grip+Trigger bit. Keep
        // the other hand open, while both body/tracking hands remain valid.
        g_fist_contact_h11.value.valid[0]=side==0;g_fist_contact_h11.value.valid[1]=side==1;
        trackingD37.right_grip=trackingD37.right_trigger=side==0?1.f:0.f;
        trackingD37.left_grip=trackingD37.left_trigger=side==1?1.f:0.f;
    };
    for(int side:{0,1})for(int punch:{5,6,7})for(int phase:{4,8,16}){
        bindPunch(side,0,punch);state.put(0x38,phase);
        ck(NativeAlignmentOwned112(alignment.ptr()),"single tracked fist owns no-step with opposite hand open, including left VR/native right shape");
    }
    bindPunch(1,1,5);ck(NativeAlignmentOwned112(alignment.ptr()),"verified native left shape can retain the admitted left VR punch");
    bindPunch(0,0,5);g_fist_contact_h11.value.valid[0]=false;g_fist_contact_h11.value.valid[1]=true;
    ck(!NativeAlignmentOwned112(alignment.ptr()),"opposite valid palm cannot own a right-hand admitted punch");
    bindPunch(1,0,5);g_fist_contact_h11.value.valid[1]=false;g_fist_contact_h11.value.valid[0]=true;
    ck(!NativeAlignmentOwned112(alignment.ptr()),"native right shape cannot borrow right VR palm for admitted left punch");
    bindPunch(0,0,5);trackingD37.right_trigger=0;
    ck(!NativeAlignmentOwned112(alignment.ptr()),"current punch-hand release overrides a still-fresh closed palm packet");
    bindPunch(0,0,5);row.put(0x224,1);
    ck(!NativeAlignmentOwned112(alignment.ptr()),"changed native row hand cannot reuse previous fist binding");
    bindPunch(0,0,5);kcd2vr::fist_action_h12::recycled(action.address());
    ck(!NativeAlignmentOwned112(alignment.ptr()),"recycled native action cannot retain fist alignment ownership");
    bindPunch(0,0,5);g_attack_timing_policy22=2;
    ck(!NativeAlignmentOwned112(alignment.ptr()),"retired input policy cannot retain fist alignment ownership");g_attack_timing_policy22=1;
    bindPunch(0,0,5);g_fist_epoch101=2;
    ck(!NativeAlignmentOwned112(alignment.ptr()),"new fist request epoch rejects original action binding");g_fist_epoch101=1;
    bindPunch(0,0,5);slots.put(0,helper.ptr());
    ck(!NativeAlignmentOwned112(alignment.ptr()),"a different current action cannot borrow a retained punch");slots.put(0,action.ptr());
    bindPunch(0,0,5);
    NativePositionDispatch36(alignment.ptr(),commands.ptr(),baseD37+0xA70FA8);
    ck(originalCallsD37==0&&g_position_veto36==1,"owned queued step is consumed without original mover side effect");
    NativePositionDispatch36(alignment.ptr(),commands.ptr(),baseD37+0xA70FA9);ck(originalCallsD37==1,"foreign command caller remains native");
    for(int special:{0,1,8,55,62}){type.put(8,special);ck(!NativeAlignmentOwned112(alignment.ptr()),"non-punch/paired action cannot borrow fist owner");}bindPunch(0,0,5);
    handsD37.empty=false;ck(!NativeAlignmentOwned112(alignment.ptr()),"held equipment revokes boxing alignment");handsD37.empty=true;
    g_fist_contact_h11.value.completedH31=false;ck(!NativeAlignmentOwned112(alignment.ptr()),"uncommitted hand packet rejected");g_fist_contact_h11.value.completedH31=true;
    g_fist_contact_h11.value.tick=919;ck(!NativeAlignmentOwned112(alignment.ptr()),"stale palm rejected");g_fist_contact_h11.value.tick=1000;
    g_committed_frame6695.value.stamp.actor=124;ck(!NativeAlignmentOwned112(alignment.ptr()),"other character body packet rejected");g_committed_frame6695.value.stamp.actor=123;
    trackingD37.right_held=true;ck(!NativeAlignmentOwned112(alignment.ptr()),"estimated tracking cannot own no-step");trackingD37.right_held=false;
    claimD37=true;ck(!NativeAlignmentOwned112(alignment.ptr()),"competing hand claim rejected");claimD37=false;
    bodyRequiredD37=true;ck(!NativeAlignmentOwned112(alignment.ptr()),"protected native body retains alignment");bodyRequiredD37=false;
    g_fist_fault101=true;ck(!NativeAlignmentOwned112(alignment.ptr()),"fist fault revokes no-step ownership");g_fist_fault101=false;
    g_no_step55=false;ck(!NativeAlignmentOwned112(alignment.ptr()),"explicit no-step OFF remains respected");g_no_step55=true;
    actor.put(0x250,entity.ptr());ck(!NativeAlignmentOwned112(alignment.ptr()),"native alignment backlink required");actor.put(0x250,alignment.ptr());
    records.put(0xC,std::uint32_t(78));ck(!NativeAlignmentOwned112(alignment.ptr()),"other actor alignment record remains native");records.put(0xC,std::uint32_t(77));
    AreaD37<0x300> npcActor,npcPlayer;AreaD37<0x50> npcEntity;
    npcActor.put(0,baseD37+0x3A80610);npcActor.put(0x1F8,npcPlayer.ptr());npcPlayer.put(0,baseD37+0x3AA4B98);npcPlayer.put(0x38,npcEntity.ptr());
    alignment.put(8,npcActor.ptr());ck(!NativeAlignmentOwned112(alignment.ptr()),"fully typed foreign player/actor cannot suppress NPC alignment");alignment.put(8,actor.ptr());
    ck(NativeAlignmentOwned112(alignment.ptr()),"restoring exact current owner recovers ordinary punch no-step");
    commands.put(0,std::uint8_t(3));NativePositionDispatch36(alignment.ptr(),commands.ptr(),baseD37+0xA70FA8);ck(originalCallsD37==2,"non-position command remains native");
    std::printf("D37 actual alignment predicates: %u checks passed; native game execution NOT_RUN\n",checksD37);
}
