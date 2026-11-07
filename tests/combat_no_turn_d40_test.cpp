// Exact production queued-rotation adapter, ordinary action gates, native pins,
// public controls and hook-arm routines are extracted by the runner. Native
// dispatch, copied ownership mailboxes, clock, memory and hook-library leaves
// are explicit substitutes. A native rotation leaf increments a side-effect
// counter and yaw witness; it does NOT execute or model the retail mover ABI.
#include <array>
#include <atomic>
#include <algorithm>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <vector>
#include <optional>
#include <functional>
#include <utility>
#include <string>
#include <cmath>
#define __fastcall
static std::uintptr_t callerD40=0;
#define _ReturnAddress FixtureReturnAddressD40
static void* _ReturnAddress(){return reinterpret_cast<void*>(callerD40);}
#include "vrik/align_veto_receipts24.h"
#include "subsystems/experimental/free_targeting_d42.h"
#include "diagnostics/capture168.h"
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
struct CodeSegmentD40{std::uintptr_t rva;std::vector<unsigned char> bytes;};
static std::vector<CodeSegmentD40> codeD40;
static std::function<void(std::uintptr_t,std::size_t)> readObserverD40;
static bool ClockRead50(std::uintptr_t p,void* out,std::size_t n){
    if(readObserverD40)readObserverD40(p,n);
    for(const auto& block:codeD40)if(p>=baseD37+block.rva&&p-(baseD37+block.rva)<=block.bytes.size()&&n<=block.bytes.size()-(p-(baseD37+block.rva))){
        std::memcpy(out,block.bytes.data()+p-(baseD37+block.rva),n);return true;}
    for(auto [start,size]:memoryD37)if(p>=start&&p-start<=size&&n<=size-(p-start)){std::memcpy(out,reinterpret_cast<void*>(p),n);return true;}
    return false;
}
template<class T> static bool Read(const void* p,T& out){return ClockRead50(reinterpret_cast<std::uintptr_t>(p),&out,sizeof(out));}
static bool PhysVft6698(void* p,std::uintptr_t offset){std::uintptr_t v=0;return Read(p,v)&&v==baseD37+offset;}
struct ReadScope101R2{};
struct NativeOwner104{void* player=nullptr;void* entity=nullptr;void* actor=nullptr;void* state=nullptr;std::uint64_t epoch=1;};
static NativeOwner104 nativeD37;
static bool resolveD37=true,observeD37=true,selectedD37=true,claimD37=false,bodyRequiredD37=false,referenceD37=false;
static bool retailD40=true,supportedD40=true,synchronizedD40=true,liveActionD40=true;
static bool SupportedWhBuild(std::uintptr_t b){return supportedD40&&b==baseD37;}
static bool NativeResolve104(void* p,NativeOwner104& o){o=nativeD37;return resolveD37&&p==o.player;}
namespace nc104 {struct Observed{int phase=0,type=0;bool requestCandidate=true;};}
static bool NativeObserve104(const NativeOwner104& o,nc104::Observed& s){return observeD37&&Read(static_cast<unsigned char*>(o.state)+0x38,s.phase)&&Read(static_cast<unsigned char*>(o.state)+0x218,s.type);}
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
    struct Profile{bool primary=false,isMelee=false;std::uint64_t key=0;bool melee()const{return isMelee;}}profiles[2];
    std::uint64_t request=1;void* player=nullptr;void* manager=nullptr;
};
static MailD37<PhysFrame6698> g_phys_frame6698;
static std::uint64_t nativeHandKeyD40=123;
static bool NativeHandKey6697(void*,int,std::uint64_t& key){key=nativeHandKeyD40;return true;}
static bool CombatSynchronized59c10r9(){return synchronizedD40;}
static bool CombatIntegrated178r22c2Enabled(){return true;} // R113: position-command veto gate (integrated covers synchronized + direct)
namespace combat_diag_probe {static bool GeometryLiveAction178r35(){return liveActionD40;}}
namespace diagnostics {namespace capture168=kcd2vr::diagnostics::capture168;}
namespace diagnostics::lab{enum class Key{F3};static int Get(Key){return 3;}static bool NativeReference(){return referenceD37;}}
namespace retail_build112{static bool Ready(std::uintptr_t b){return retailD40&&b==baseD37;}}
namespace animation_filter{
    static std::atomic<bool> nativeBodyOwned112{false};static std::atomic<std::uint64_t> combatPoseTick112{1000};
    static bool PlayerNativeBodyRequired112(){return bodyRequiredD37;}
}
static std::atomic<bool> g_combat_experiment175{true},g_physical_combat177{true},g_phys_armed6698{true},g_phys_fault6698{false},g_native_fault104{false},g_fist_fault101{false};
static std::atomic<bool> g_gameplay_allowed{true},g_ctx_dialogue{false},g_ctx_cutscene{false},g_ctx_scripted{false},g_ctx_menu{false};
static std::atomic<bool> g_head_valid{true},g_right_valid{true},g_left_valid{true},g_no_step55{true};
static std::atomic<int> g_weapon_control_mode{7};
static std::atomic<std::uint64_t> g_weapon_request_epoch6694{1},g_pose_epoch6689{1},g_fist_epoch101{1},g_attack_timing_policy22{1};
static std::atomic<std::uint64_t> g_pose_policy_generation57{2};
static std::atomic<void*> g_last_player_entity{nullptr},g_henry_item_attach_manager{nullptr};

static unsigned checksD40=0,failuresD40=0,rotationCallsD40=0,positionCallsD40=0;
static double nativeYawD40=0;
static void RotationLeafD40(void*,const void*){++rotationCallsD40;nativeYawD40+=.25;}
static void PositionLeafD40(void*,const void*){++positionCallsD40;}
static bool AlignmentLeafD40(void*){return true;}
static bool AlignVetoReceiptsActive25(){return false;}
static void AlignVetoReceipt25(void*,std::uintptr_t,bool,bool,std::uint8_t){}
static std::atomic<std::uint64_t> g_alignment_skips55{0};
namespace kcd2vr::hookcost178r22{enum Kind{Alignment,AlignmentPolicy};struct Scope{explicit Scope(Kind){}};struct Original{explicit Original(Kind){}};}
namespace DetourModKit{
struct Address{std::uintptr_t value;};
namespace hook{
struct Target{Address address;};struct Options{bool fail_if_already_hooked=false;};
struct InlineRequest{const char* name;Target target;Options options;};
static unsigned installsD40=0,enablesD40=0,midInstallsD125=0,midEnablesD125=0;
static std::uintptr_t failedTargetD40=0,failedEnableD40=0;
struct Hook{
    std::uintptr_t address=0;
    template<class T>T original(){
        if(address==baseD37+0x14097F0)return reinterpret_cast<T>(&RotationLeafD40);
        if(address==baseD37+0x14095E8)return reinterpret_cast<T>(&PositionLeafD40);
        return reinterpret_cast<T>(&AlignmentLeafD40);
    }
    bool enable(){if(address==baseD37+0x83A1A0){++midEnablesD125;return address!=failedEnableD40;}++enablesD40;return address!=failedEnableD40;}
    bool disable(){return true;}
};
template<class T>std::optional<Hook> inline_at(InlineRequest&& req,T){
    ++installsD40;if(req.target.address.value==failedTargetD40)return {};
    return Hook{req.target.address.value};
}
// R125 mid hook leaf: the register file the production callback reads (rdi = CAnimatedCharacter, rbx = out QuatT).
enum class Gpr{Rax,Rbx,Rcx,Rdx,Rsi,Rdi,Rbp};
struct MidContext{std::uintptr_t r[7]{};};
static std::uintptr_t gpr(MidContext& c,Gpr g){return c.r[int(g)];}
struct MidRequest{const char* name;Target target;Options options;};
static void (*midCallbackD125)(MidContext&)=nullptr;
static std::optional<Hook> mid_at(MidRequest&& req,void (*fn)(MidContext&)){
    ++midInstallsD125;if(req.target.address.value==failedTargetD40)return {};
    midCallbackD125=fn;return Hook{req.target.address.value};
}
}}
namespace diagnostic_trace{static unsigned linesD125=0;static std::string lastD125;static void Write(const char* m){++linesD125;lastD125=m;}}
#include "generated_combat_no_turn_d40.inc"
static void ck(bool ok,const char* name){++checksD40;if(!ok)++failuresD40;std::printf("%s %u: %s\n",ok?"PASS":"FAIL",checksD40,name);}
static void RotationAtQueueD40(void* manager,const void* command,std::uintptr_t caller){
#if D40_HAS_ROTATION
    callerD40=caller;NativeRotationDetourD40(manager,command);
#else
    // D39 has no hook at14097F0. This absent-hook boundary calls the explicit
    // native leaf; no rewritten/duplicated production policy is substituted.
    (void)caller;RotationLeafD40(manager,command);
#endif
}
static void expectD40(void* manager,const void* command,bool veto,const char* label,std::uintptr_t caller=0){
    const auto before=rotationCallsD40;const auto yaw=nativeYawD40;
    RotationAtQueueD40(manager,command,caller?caller:baseD37+0x14097E2);
    ck(rotationCallsD40==before+unsigned(!veto)&&nativeYawD40==yaw+(veto?0:.25),label);
}
int main(){
#include "generated_combat_rotation_pins_d40.inc"
    g_motion_contract112=baseD37;g_position_original36=&PositionLeafD40;
#if D40_HAS_ROTATION
    g_rotation_contract_d40=baseD37;g_rotation_original_d40=&RotationLeafD40;
#endif
    AreaD37<0xB00> player,actor;AreaD37<0xD00> state;AreaD37<0x100> manager,slots,alignment,clip,records,entity;AreaD37<0x1000> commands;AreaD37<0x100> equipment;
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
    alignment.put(0x30,commands.ptr());alignment.put(0x38,commands.address()+0x30);alignment.put(0x40,commands.address()+0x100);commands.put(0,std::uint8_t(3));
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

    commands.put(0,std::uint8_t(3));
    const auto phase=[&](int p){state.put(0x38,p);};
    const auto queue=[&](){alignment.put(0x30,commands.address());alignment.put(0x38,commands.address()+0x30);alignment.put(0x40,commands.address()+0x100);commands.put(0,std::uint8_t(3));};
    auto melee=[&](int kind){
        selectedD37=false;type.put(8,kind);phase(4);
        player.put(0x9D0,equipment.ptr());equipment.put(0,baseD37+0x3EC9958);equipment.put(0x38,player.ptr());
        g_henry_item_attach_manager=equipment.ptr();
        auto& f=g_phys_frame6698.value;f={};f.player=player.ptr();f.manager=equipment.ptr();
        f.input.allowed=f.input.attacks=f.input.tracked[0]=f.input.tracked[1]=true;
        f.output.owned[0]=f.output.active[0]=true;f.profiles[0].primary=f.profiles[0].isMelee=true;f.profiles[0].key=nativeHandKeyD40;
    };
    for(int kind:{0,1})for(int p:{4,8,16}){
        melee(kind);phase(p);expectD40(alignment.ptr(),commands.ptr(),true,"ordinary committed slash/thrust phase blocks early rotation");
    }
    selectedD37=true;
    for(int side:{0,1})for(int punch:{5,6,7})for(int p:{4,8,16}){
        bindPunch(side,0,punch);phase(p);expectD40(alignment.ptr(),commands.ptr(),true,"ordinary current tracked punch blocks early rotation, opposite palm open");
    }
    bindPunch(0,0,5);phase(4);
    const auto beforeYaw=nativeYawD40;
    RotationAtQueueD40(alignment.ptr(),commands.ptr(),baseD37+0x14097E2);
    const bool late=NativeAlignmentResult112(alignment.ptr(),baseD37+0x9E2D55,true);
    ck(!late,"existing late query still vetoes an owned ordinary alignment");
    alignment.put(0x60,records.address()); // R112: R106/R111 logs - the query answers true before any record exists
    ck(!NativeAlignmentResult112(alignment.ptr(),baseD37+0x9E2D55,true),"R112 empty alignment record vector still vetoes the late step query (punch)");
    alignment.put(0x60,records.address()+0x20);
    ck(nativeYawD40==beforeYaw,"early hook prevents the side effect which a later query cannot undo");
    const auto priorPosition=positionCallsD40;
    NativePositionDispatch36(alignment.ptr(),commands.ptr(),baseD37+0x14097E2);
    ck(positionCallsD40==priorPosition+1,"D39 position adapter cannot intercept rotation caller or command kind");
    melee(0);
    const auto stateBefore=state.bytes;const auto actorBefore=actor.bytes;const auto actionBefore=action.bytes;const auto rowBefore=row.bytes;
    expectD40(alignment.ptr(),commands.ptr(),true,"qualified ordinary melee before semantic-memory preservation check");
    alignment.put(0x60,records.address());
    ck(!NativeAlignmentResult112(alignment.ptr(),baseD37+0x9E2D55,true),"R112 empty alignment record vector vetoes the ordinary slash/thrust step");
    alignment.put(0x58,std::uintptr_t(0));alignment.put(0x60,std::uintptr_t(0));alignment.put(0x68,std::uintptr_t(0));
    ck(!NativeAlignmentResult112(alignment.ptr(),baseD37+0x9E2D55,true),"R112 never-allocated vector (all null) vetoes the ordinary step");
    alignment.put(0x58,records.ptr());alignment.put(0x60,records.address()+0x10);alignment.put(0x68,records.address()+0x100);
    ck(NativeAlignmentResult112(alignment.ptr(),baseD37+0x9E2D55,true),"R112 malformed (partial) vector stays native");
    alignment.put(0x60,records.address()+0x20);
    ck(state.bytes==stateBefore&&actor.bytes==actorBefore&&action.bytes==actionBefore&&row.bytes==rowBefore,
       "adapter leaves opponent/phase/combo/action/actor memory untouched");
    g_no_step55=false;
    expectD40(alignment.ptr(),commands.ptr(),true,"early rotation control works while the existing step control is OFF");
    ck(NativeAlignmentResult112(alignment.ptr(),baseD37+0x9E2D55,true),"step OFF retains native late query");
    commands.put(0,std::uint8_t(2));const auto p0=positionCallsD40;
    NativePositionDispatch36(alignment.ptr(),commands.ptr(),baseD37+0xA70FA8);
    ck(positionCallsD40==p0+1,"rotation policy does not claim an ordinary position command");
    commands.put(0,std::uint8_t(3));g_no_step55=true;
#if D40_HAS_ROTATION
    SetCombatNoTurnD40(false);ck(!CombatNoTurnD40(),"public setter immediately changes early-turn preference");
    expectD40(alignment.ptr(),commands.ptr(),false,"early turn OFF forwards original native routine");
    ck(CombatNoTurnReadyD40(),"readiness remains separate from the OFF preference");
    SetCombatNoTurnD40(true);ck(CombatNoTurnD40(),"public getter reflects turn ON");
    SetCombatNoStep55(false);ck(!CombatNoStep55(),"existing step policy exposes live OFF state");
    SetCombatNoStep55(true);ck(CombatNoStep55(),"existing step policy exposes live ON state");
#endif
    for(std::uintptr_t wrong:{baseD37+0xA70FA8,baseD37+0x14097E1,baseD37+0x14097E3,baseD37+0xA70F9B,baseD37+0xA71577,baseD37+0x14095DB})
        expectD40(alignment.ptr(),commands.ptr(),false,"foreign caller retains original rotation routine",wrong);
    for(unsigned char kind:{0,1,2,4,5,6,255}){commands.put(0,kind);expectD40(alignment.ptr(),commands.ptr(),false,"other queue kinds including combined/paired stay native");}queue();
    expectD40(nullptr,commands.ptr(),false,"null manager stays native");expectD40(alignment.ptr(),nullptr,false,"null command stays native");
    expectD40(alignment.ptr(),commands.bytes.data()+1,false,"misaligned queue entry stays native");
    expectD40(alignment.ptr(),commands.bytes.data()+0x30,false,"entry at exclusive queue end stays native");
    alignment.put(0x30,std::uintptr_t(0));expectD40(alignment.ptr(),commands.ptr(),false,"missing queue vector stays native");queue();
    alignment.put(0x38,commands.address());expectD40(alignment.ptr(),commands.ptr(),false,"empty queue stays native");queue();
    alignment.put(0x40,commands.address()+0x20);expectD40(alignment.ptr(),commands.ptr(),false,"end beyond queue capacity stays native");queue();
    alignment.put(0x38,commands.address()+0x31);expectD40(alignment.ptr(),commands.ptr(),false,"malformed queue stride stays native");queue();
    alignment.put(0x38,commands.address()+65*0x30);alignment.put(0x40,commands.address()+66*0x30);
    expectD40(alignment.ptr(),commands.ptr(),false,"oversized queue stays native");queue();
#if D40_HAS_ROTATION
    unsigned queueReads=0;
    readObserverD40=[&](std::uintptr_t p,std::size_t){if(p==alignment.address()+0x30&&++queueReads==2)alignment.put(0x30,commands.address()+0x30);};
    expectD40(alignment.ptr(),commands.ptr(),false,"queue replacement during ownership read revokes early veto");readObserverD40={};queue();
    readObserverD40=[&](std::uintptr_t p,std::size_t){if(p==commands.address())SetCombatNoTurnD40(false);};
    expectD40(alignment.ptr(),commands.ptr(),false,"live OFF during command inspection is rechecked before veto");readObserverD40={};SetCombatNoTurnD40(true);
    readObserverD40=[&](std::uintptr_t p,std::size_t){if(p==commands.address())g_ctx_menu=true;};
    expectD40(alignment.ptr(),commands.ptr(),false,"D40R1: menu takeover after context read must retain native rotation");readObserverD40={};g_ctx_menu=false;
    readObserverD40=[&](std::uintptr_t p,std::size_t){if(p==commands.address())g_weapon_request_epoch6694=2;};
    expectD40(alignment.ptr(),commands.ptr(),false,"D40R1: weapon owner epoch revoked after context read must retain native rotation");readObserverD40={};g_weapon_request_epoch6694=1;
    g_pose_policy_generation57=3;
    expectD40(alignment.ptr(),commands.ptr(),false,"D40R1: in-progress context publication stays native");g_pose_policy_generation57=2;
    readObserverD40=[&](std::uintptr_t p,std::size_t){if(p==commands.address())g_pose_policy_generation57.fetch_add(2);};
    expectD40(alignment.ptr(),commands.ptr(),false,"D40R1: completed context ABA after admission stays native");readObserverD40={};g_pose_policy_generation57=2;
    readObserverD40=[&](std::uintptr_t p,std::size_t){if(p==commands.address())g_pose_epoch6689=2;};
    expectD40(alignment.ptr(),commands.ptr(),false,"D40R1: body epoch retirement after admission stays native");readObserverD40={};g_pose_epoch6689=1;
    g_pose_policy_generation57=10;
    expectD40(alignment.ptr(),commands.ptr(),true,"D40R1: stable later context generation retains ordinary ownership");g_pose_policy_generation57=2;
    readObserverD40=[&](std::uintptr_t p,std::size_t){if(p==commands.address())g_fist_epoch101=2;};
    expectD40(alignment.ptr(),commands.ptr(),true,"D40R1: unrelated fist epoch cannot block stable weapon ownership");readObserverD40={};g_fist_epoch101=1;
    g_rotation_contract_d40=0;expectD40(alignment.ptr(),commands.ptr(),false,"unverified rotation hook cannot claim command");g_rotation_contract_d40=baseD37;
#endif
    g_motion_contract112=0;expectD40(alignment.ptr(),commands.ptr(),false,"lost existing motion contract retains native rotation");g_motion_contract112=baseD37;
    retailD40=false;expectD40(alignment.ptr(),commands.ptr(),false,"unverified retail SHA stays native");retailD40=true;
    referenceD37=true;expectD40(alignment.ptr(),commands.ptr(),false,"native-reference mode stays native");referenceD37=false;
    g_combat_experiment175=false;expectD40(alignment.ptr(),commands.ptr(),false,"F4 off stays native");g_combat_experiment175=true;
    synchronizedD40=false;expectD40(alignment.ptr(),commands.ptr(),true,"F4 DIRECT with a proven active ordinary action also blocks forced turn");synchronizedD40=true;
    g_phys_fault6698=true;expectD40(alignment.ptr(),commands.ptr(),false,"physical subsystem fault stays native");g_phys_fault6698=false;
    g_native_fault104=true;expectD40(alignment.ptr(),commands.ptr(),false,"native owner fault stays native");g_native_fault104=false;
    g_weapon_control_mode=6;expectD40(alignment.ptr(),commands.ptr(),false,"non-hybrid weapon mode stays native");g_weapon_control_mode=7;
    g_gameplay_allowed=false;expectD40(alignment.ptr(),commands.ptr(),false,"no gameplay authority stays native");g_gameplay_allowed=true;
    for(auto* flag:{&g_ctx_dialogue,&g_ctx_cutscene,&g_ctx_scripted,&g_ctx_menu}){*flag=true;expectD40(alignment.ptr(),commands.ptr(),false,"dialogue/cutscene/scripted/menu ownership stays native");*flag=false;}
    for(auto* flag:{&g_head_valid,&g_right_valid,&g_left_valid}){*flag=false;expectD40(alignment.ptr(),commands.ptr(),false,"missing live head or controller stays native");*flag=true;}
    animation_filter::nativeBodyOwned112=true;expectD40(alignment.ptr(),commands.ptr(),false,"protected native body owner retains rotation");animation_filter::nativeBodyOwned112=false;
    bodyRequiredD37=true;expectD40(alignment.ptr(),commands.ptr(),false,"protected clip census retains rotation");bodyRequiredD37=false;
    animation_filter::combatPoseTick112=919;expectD40(alignment.ptr(),commands.ptr(),false,"stale completed body pose retains rotation");animation_filter::combatPoseTick112=1000;
    for(int p:{0,1,2,32,64,128,256}){phase(p);expectD40(alignment.ptr(),commands.ptr(),false,"nonordinary action phase retains rotation");}phase(4);
    for(int kind:{5,8,35,55,62}){type.put(8,kind);expectD40(alignment.ptr(),commands.ptr(),false,"special/kick/clinch/masterstrike row cannot borrow melee owner");}type.put(8,0);
    state.put(0x218,35);expectD40(alignment.ptr(),commands.ptr(),true,"pending request type does not replace the proven current ordinary action");state.put(0x218,0);
    slots.put(0,helper.ptr());expectD40(alignment.ptr(),commands.ptr(),false,"replaced current main action retains native rotation");slots.put(0,action.ptr());
    actor.put(0x250,entity.ptr());expectD40(alignment.ptr(),commands.ptr(),false,"actor/manager backlink mismatch stays native");actor.put(0x250,alignment.ptr());
    AreaD37<0x300> npcActor,npcPlayer;AreaD37<0x50> npcEntity;
    npcActor.put(0,baseD37+0x3A80610);npcActor.put(0x1F8,npcPlayer.ptr());npcPlayer.put(0,baseD37+0x3AA4B98);npcPlayer.put(0x38,npcEntity.ptr());
    alignment.put(8,npcActor.ptr());expectD40(alignment.ptr(),commands.ptr(),false,"fully typed NPC manager retains native rotation");alignment.put(8,actor.ptr());
    auto& f=g_phys_frame6698.value;
    f.input.tick=919;expectD40(alignment.ptr(),commands.ptr(),false,"stale physical frame stays native");f.input.tick=1000;
    f.request=2;expectD40(alignment.ptr(),commands.ptr(),false,"retired weapon request stays native");f.request=1;
    f.input.bodyEpoch=2;expectD40(alignment.ptr(),commands.ptr(),false,"retired body epoch stays native");f.input.bodyEpoch=1;
    f.output.owned[0]=false;expectD40(alignment.ptr(),commands.ptr(),false,"unowned weapon stays native");f.output.owned[0]=true;
    f.output.active[0]=false;expectD40(alignment.ptr(),commands.ptr(),false,"inactive output without admitted recovery stays native");
    f.output.handlingOnly55c1[0]=true;expectD40(alignment.ptr(),commands.ptr(),true,"qualified admitted collision recovery retains early-turn veto");
    synchronizedD40=false;expectD40(alignment.ptr(),commands.ptr(),false,"F4 DIRECT cannot claim recovery-only output without an active ordinary owner");synchronizedD40=true;
    f.output.active[0]=true;f.output.handlingOnly55c1[0]=false;
    f.profiles[0].key=999;expectD40(alignment.ptr(),commands.ptr(),false,"changed equipment identity stays native");f.profiles[0].key=nativeHandKeyD40;
    trackingD37.head_held=true;expectD40(alignment.ptr(),commands.ptr(),false,"estimated head tracking stays native");trackingD37.head_held=false;
    trackingD37.left_held=true;expectD40(alignment.ptr(),commands.ptr(),false,"estimated controller tracking stays native");trackingD37.left_held=false;
    trackingD37.tick=919;expectD40(alignment.ptr(),commands.ptr(),false,"stale live tracking stays native");trackingD37.tick=1000;
    selectedD37=true;bindPunch(0,0,5);
    trackingD37.right_trigger=0;expectD40(alignment.ptr(),commands.ptr(),false,"released punch-hand intent cannot own rotation");bindPunch(0,0,5);
    claimD37=true;expectD40(alignment.ptr(),commands.ptr(),false,"competing hand claim retains native punch rotation");claimD37=false;
    handsD37.empty=false;expectD40(alignment.ptr(),commands.ptr(),false,"equipped hand cannot borrow fist action ownership");handsD37.empty=true;
    g_fist_contact_h11.value.completedH31=false;expectD40(alignment.ptr(),commands.ptr(),false,"uncommitted fist contact packet stays native");g_fist_contact_h11.value.completedH31=true;
    g_fist_contact_h11.value.tick=919;expectD40(alignment.ptr(),commands.ptr(),false,"stale fist contact packet stays native");g_fist_contact_h11.value.tick=1000;
    kcd2vr::fist_action_h12::recycled(action.address());expectD40(alignment.ptr(),commands.ptr(),false,"retired fist binding retains native rotation");bindPunch(0,0,5);
#if D40_HAS_ROTATION
    readObserverD40=[&](std::uintptr_t p,std::size_t){if(p==commands.address())g_fist_epoch101=2;};
    expectD40(alignment.ptr(),commands.ptr(),false,"D40R1: original fist request retirement after admission stays native");readObserverD40={};g_fist_epoch101=1;
    ck(CombatNoTurnVetoCountD40()>0,"public receipt count observes actual adapter vetoes");
    ck(NativeRotationContractD40(baseD37),"all rotation pins match bytes read independently from the supplied DLL");
    for(auto& block:codeD40){
        if(block.rva!=0x14097F0&&block.rva!=0x14097CE&&block.rva!=0x14097AC&&block.rva!=0xA70F44&&block.rva!=0xA70F90&&block.rva!=0x14097D7&&block.rva!=0x1409816&&block.rva!=0x14098B0&&block.rva!=0x1409885&&block.rva!=0xA71ACC&&block.rva!=0xA71AE4&&block.rva!=0xA71B2E&&block.rva!=0xA71B60)continue;
        block.bytes[0]^=1;ck(!NativeRotationContractD40(baseD37),"each required native route pin fails closed when changed");block.bytes[0]^=1;
    }
    supportedD40=false;ck(!NativeRotationContractD40(baseD37),"unsupported build cannot verify rotation contract");supportedD40=true;
    retailD40=false;ck(!NativeRotationContractD40(baseD37),"unverified full retail fingerprint cannot verify rotation contract");retailD40=true;
    using namespace DetourModKit::hook;
    const auto resetHooks=[&](){
        g_alignment_hook112.reset();g_position_hook36.reset();g_rotation_hook_d40.reset();
        g_alignment_original112=nullptr;g_position_original36=nullptr;g_rotation_original_d40=nullptr;
        g_motion_contract112=0;g_rotation_contract_d40=0;installsD40=enablesD40=0;failedTargetD40=failedEnableD40=0;
#if D125_HAS_ROOT_MOTION
        g_root_motion_hook125.reset();g_root_motion_contract125=0;midInstallsD125=midEnablesD125=0;
#endif
    };
    resetHooks();ck(NativeMotionArm112()&&CombatNoTurnReadyD40()&&installsD40==3&&enablesD40==3,"actual arm routine installs core plus optional verified rotation hook");
#if D125_HAS_ROOT_MOTION
    ck(g_root_motion_contract125==baseD37&&midInstallsD125==1&&midEnablesD125==1,"R125 arm installs the verified root-motion exit hook with the core");
    ck(NativeMotionArm112()&&midInstallsD125==1,"R125 repeated arm reuses the root-motion hook");
#endif
    ck(NativeMotionArm112()&&installsD40==3,"repeated arm reuses installed hooks");
    auto* rotationEntry=static_cast<CodeSegmentD40*>(nullptr);for(auto& b:codeD40)if(b.rva==0x14097F0)rotationEntry=&b;
    ck(rotationEntry!=nullptr,"independent DLL fixture includes rotation entry");
    resetHooks();rotationEntry->bytes[0]^=1;
    ck(NativeMotionArm112()&&!CombatNoTurnReadyD40()&&g_motion_contract112==baseD37&&installsD40==2,"rotation pin failure preserves verified legacy no-step hooks");
    rotationEntry->bytes[0]^=1;ck(NativeMotionArm112()&&CombatNoTurnReadyD40()&&installsD40==3,"uninstalled optional hook can arm after its contract is restored");
    resetHooks();failedEnableD40=baseD37+0x14097F0;
    ck(NativeMotionArm112()&&!CombatNoTurnReadyD40()&&g_motion_contract112==baseD37,"rotation enable failure leaves core ownership intact and turn native-only");
    failedEnableD40=0;const auto partial=installsD40;
    ck(NativeMotionArm112()&&!CombatNoTurnReadyD40()&&installsD40==partial,"partial optional trampoline is retained without hot recreation");
    resetHooks();retailD40=false;ck(!NativeMotionArm112()&&installsD40==0&&!CombatNoTurnReadyD40(),"unverified retail never installs any motion hook");retailD40=true;
#if D125_HAS_ROOT_MOTION
    ck(midInstallsD125==0&&g_root_motion_contract125==0,"R125 unverified retail never installs the root-motion hook");
    resetHooks();failedEnableD40=baseD37+0x83A1A0;
    ck(NativeMotionArm112()&&g_motion_contract112==baseD37&&g_root_motion_contract125==0,"R125 root-motion enable failure keeps no-step/turn hooks");
    failedEnableD40=0;ck(NativeRootMotionArm125(baseD37)&&g_root_motion_contract125==baseD37&&midInstallsD125==1,"R125 failed enable is retried later on the same hook (no latch)");
#endif
#else
    ck(NativeMotionContract112(baseD37),"D39 legacy motion contract still verifies against actual DLL bytes");
#endif
#if D42_HAS_FREE_TARGETING
    // Same complete producer/admission used above. F3 counters cannot modify
    // the native-call decision, and all unqualified/protected owners remain.
    g_rotation_contract_d40=baseD37;g_motion_contract112=baseD37;g_rotation_original_d40=&RotationLeafD40;g_position_original36=&PositionLeafD40;
    g_no_turn_d40=false;selectedD37=true;bindPunch(0,0,5);
    kcd2vr::free_targeting_d42::set(true);
    expectD40(alignment.ptr(),commands.ptr(),true,"free-targeting overrides only the no-turn preference for owned ordinary punches");
    referenceD37=true;expectD40(alignment.ptr(),commands.ptr(),false,"free-targeting never overrides native reference");referenceD37=false;
    bodyRequiredD37=true;expectD40(alignment.ptr(),commands.ptr(),false,"free-targeting never overrides protected paired body ownership");bodyRequiredD37=false;
    readObserverD40=[&](std::uintptr_t p,std::size_t){if(p==commands.address()){kcd2vr::free_targeting_d42::set(false);kcd2vr::free_targeting_d42::set(true);}};
    expectD40(alignment.ptr(),commands.ptr(),false,"free-targeting switch off and on during admission cannot revive a captured policy");readObserverD40={};
    // R114 (R113 log, fists: 15 of 19 Henry turns kept as NOT_OWNED): with free targeting a live ordinary native
    // punch row owns the turn veto without the per-hand steering proof; kicks/paired rows and protected bodies stay native.
    for(int punch:{5,6,7})for(int p:{4,8,16}){
        bindPunch(0,0,punch);phase(p);
        claimD37=true;expectD40(alignment.ptr(),commands.ptr(),true,"R114 free targeting: hand claim on a hand does not give the punch turn back");claimD37=false;
        trackingD37.right_trigger=0;expectD40(alignment.ptr(),commands.ptr(),true,"R114 free targeting: opened punch hand keeps the turn vetoed");trackingD37.right_trigger=1;
        g_fist_contact_h11.value.tick=919;expectD40(alignment.ptr(),commands.ptr(),true,"R114 free targeting: stale palm packet keeps the turn vetoed");g_fist_contact_h11.value.tick=1000;
        g_committed_frame6695.value.stamp.sample=6;expectD40(alignment.ptr(),commands.ptr(),true,"R114 free targeting: older body packet keeps the turn vetoed");g_committed_frame6695.value.stamp.sample=7;
        kcd2vr::fist_action_h12::recycled(action.address());expectD40(alignment.ptr(),commands.ptr(),true,"R114 free targeting: live native punch without a tracked binding keeps the turn vetoed");
    }
    bindPunch(0,0,5);
    for(int kind:{0,1,4,8,35,55,62})for(int p:{4,8,16}){type.put(8,kind);phase(p);expectD40(alignment.ptr(),commands.ptr(),false,"R114 free targeting fists: kick/clinch/masterstrike/non-punch row keeps the native turn");}
    type.put(8,5);phase(4);
    phase(64);expectD40(alignment.ptr(),commands.ptr(),false,"R114 free targeting fists: own stagger keeps the native turn");phase(4);
    action.put(0x3C,1);expectD40(alignment.ptr(),commands.ptr(),false,"R114 free targeting fists: punch row off the main channel stays native");action.put(0x3C,0);
    bodyRequiredD37=true;expectD40(alignment.ptr(),commands.ptr(),false,"R114 free targeting fists: protected paired body stays native");bodyRequiredD37=false;
    trackingD37.left_held=true;expectD40(alignment.ptr(),commands.ptr(),false,"R114 free targeting fists: estimated controller tracking stays native");trackingD37.left_held=false;
    alignment.put(8,npcActor.ptr());expectD40(alignment.ptr(),commands.ptr(),false,"R114 free targeting fists: NPC manager stays native");alignment.put(8,actor.ptr());
    kcd2vr::free_targeting_d42::set(false);g_no_turn_d40=true;
    claimD37=true;expectD40(alignment.ptr(),commands.ptr(),false,"R114: without free targeting the full punch proof still decides (claim)");claimD37=false;
    kcd2vr::fist_action_h12::recycled(action.address());expectD40(alignment.ptr(),commands.ptr(),false,"R114: without free targeting a punch without binding stays native");
    g_no_turn_d40=false;kcd2vr::free_targeting_d42::set(true);bindPunch(0,0,5);phase(4);
    ck(CombatFreeViewContextD42(),"ordinary current HMD ownership permits camera-only mode independently of F4");
    g_combat_experiment175=false;g_physical_combat177=false;liveActionD40=false;
    ck(CombatFreeViewContextD42(),"F4 OFF does not block camera-only free targeting");
    expectD40(alignment.ptr(),commands.ptr(),true,"F4 OFF free-targeting owns qualified tracked punch rotation without enabling contact experiment");
    melee(0);expectD40(alignment.ptr(),commands.ptr(),true,"F4 OFF free-targeting owns qualified tracked ordinary melee rotation");
    g_weapon_control_mode=6;expectD40(alignment.ptr(),commands.ptr(),true,"free-targeting permits coherent weapon mode6 without F4");g_weapon_control_mode=7;
    PhysFrame6698 noStepFrame{};NativeOwner104 noStepOwner{};
    ck(!NativeMotionContext112(noStepFrame,noStepOwner,alignment.ptr()),"free-targeting cannot grant position or late-alignment ownership");
    const auto positionBefore=positionCallsD40;commands.put(0,std::uint8_t(2));
    NativePositionDispatch36(alignment.ptr(),commands.ptr(),baseD37+0xA70FA8);
    ck(positionCallsD40==positionBefore+1,"F4 OFF free-targeting does not suppress native step command");commands.put(0,std::uint8_t(3));
    alignment.put(8,npcActor.ptr());expectD40(alignment.ptr(),commands.ptr(),false,"free-targeting rejects fully typed NPC before Henry tracking or census");alignment.put(8,actor.ptr());
    for(int kind:{8,35,55,62}){type.put(8,kind);expectD40(alignment.ptr(),commands.ptr(),false,"free-targeting preserves kick/clinch/masterstrike/paired native row");}type.put(8,0);
    f.output.active[0]=false;f.output.handlingOnly55c1[0]=true;
    expectD40(alignment.ptr(),commands.ptr(),false,"free-targeting alone cannot borrow contact-recovery ownership");f.output.active[0]=true;f.output.handlingOnly55c1[0]=false;
    referenceD37=true;expectD40(alignment.ptr(),commands.ptr(),false,"F4 OFF free-targeting preserves native reference");referenceD37=false;
    bodyRequiredD37=true;expectD40(alignment.ptr(),commands.ptr(),false,"F4 OFF free-targeting preserves protected body census");bodyRequiredD37=false;
    g_combat_experiment175=true;g_physical_combat177=true;liveActionD40=true;
    selectedD37=true;bindPunch(0,0,5);
    trackingD37.right_valid=trackingD37.left_valid=false;ck(CombatFreeViewContextD42(),"camera-only lease does not require tracked hands");trackingD37.right_valid=trackingD37.left_valid=true;
    trackingD37.tick=940;ck(CombatFreeViewContextD42(),"exact 60ms head snapshot boundary retained");trackingD37.tick=939;ck(!CombatFreeViewContextD42(),"61ms head snapshot cannot own free camera");trackingD37.tick=1000;
    trackingD37.head_held=true;ck(!CombatFreeViewContextD42(),"estimated head tracking yields");trackingD37.head_held=false;
    for(auto* flag:{&g_ctx_dialogue,&g_ctx_menu,&g_ctx_cutscene,&g_ctx_scripted,&animation_filter::nativeBodyOwned112}){
        flag->store(true);ck(!CombatFreeViewContextD42(),"published protected context yields camera lease");flag->store(false);
    }
    g_pose_policy_generation57=3;ck(!CombatFreeViewContextD42(),"in-progress context publication yields");g_pose_policy_generation57=2;
    const auto before=kcd2vr::free_targeting_d42::rotationCalls.load();
    expectD40(alignment.ptr(),commands.ptr(),true,"recorder OFF retains qualified native veto");
    ck(kcd2vr::free_targeting_d42::rotationCalls.load()==before,"F3 OFF adds no rotation diagnostic record");
    kcd2vr::diagnostics::capture168::gate.select(kcd2vr::diagnostics::capture168::Domain::Combat,true,1000);
    expectD40(alignment.ptr(),commands.ptr(),true,"recorder ON has identical qualified native decision");
    ck(kcd2vr::free_targeting_d42::rotationCalls.load()==before+1&&kcd2vr::free_targeting_d42::rotationVetoes.load()>0,"F3 counts actual admitted rotation veto");
    g_ctx_menu=true;expectD40(alignment.ptr(),commands.ptr(),false,"F3 records retained native protected command");g_ctx_menu=false;
    ck(kcd2vr::free_targeting_d42::rotationRefused[kcd2vr::align24::GameContext].load()>0,"refusal retains exact first failing owner gate");
    kcd2vr::diagnostics::capture168::gate.select(kcd2vr::diagnostics::capture168::Domain::Combat,false,1000);
    kcd2vr::free_targeting_d42::set(false);
#endif
#if D125_HAS_ROOT_MOTION
    {
    // R125 (R124 log: alignment vetoed 49..128x and position command vetoed per attack, Henry still C38 forwardCm
    // 45..126): the exit of CalculateWantedEntityMovement keeps Henry's requested horizontal movement in his ordinary
    // attack phases 4/8/16 and drops the clip/forced root translation. Rotation and height stay native.
    g_motion_contract112=baseD37;g_combat_experiment175=true;g_physical_combat177=true;liveActionD40=true;
    kcd2vr::free_targeting_d42::set(false);g_no_step55=true;
    AreaD37<0x800> animated;AreaD37<0x40> out;
    animated.put(0,baseD37+0x3A49468);animated.put(0x38,entity.ptr());animated.put(0x708,std::uint8_t(0));
    animated.put(0x68C,.01f);animated.put(0x690,-.02f);animated.put(0x694,.5f);
    player.put(0x268,animated.ptr());
    const auto prime=[&](){out.put(0,.1f);out.put(4,.2f);out.put(8,.3f);out.put(12,.9f);out.put(0x10,.30f);out.put(0x14,.40f);out.put(0x18,-.05f);};
    const auto fire=[&](std::uintptr_t a){DetourModKit::hook::MidContext c{};c.r[int(DetourModKit::hook::Gpr::Rdi)]=a;c.r[int(DetourModKit::hook::Gpr::Rbx)]=out.address();RootMotionMid125(c);};
    const auto f32=[&](std::size_t o){float v=0;std::memcpy(&v,out.bytes.data()+o,4);return v;};
    const auto vetoed=[&](){return f32(0x10)==.01f&&f32(0x14)==-.02f&&f32(0x18)==-.05f&&f32(0)==.1f&&f32(12)==.9f;};
    const auto untouched=[&](){return f32(0x10)==.30f&&f32(0x14)==.40f&&f32(0x18)==-.05f&&f32(0)==.1f&&f32(12)==.9f;};
    const auto expectRoot=[&](bool veto,const char* label,std::uintptr_t a=0){prime();fire(a?a:animated.address());ck(veto?vetoed():untouched(),label);};
    g_root_motion_contract125=baseD37;
    for(int kind:{0,1})for(int p:{4,8,16}){melee(kind);phase(p);expectRoot(true,"R125 ordinary slash/thrust phase 4/8/16: horizontal = requested movement, clip translation removed, rotation and height native");}
    melee(0);
    for(int p:{0,1,2,32,64,128,256}){phase(p);expectRoot(false,"R125 guard/after-strike/stagger/block phases keep native movement");}phase(8);
    for(int kind:{4,5,8,35,55,62}){type.put(8,kind);expectRoot(false,"R125 kick/clinch/masterstrike/special row keeps native clip motion");}type.put(8,0);
    expectRoot(true,"R125 baseline restored");
    g_no_step55=false;expectRoot(false,"R125 Attack step veto OFF = native clip motion");g_no_step55=true;
    expectRoot(true,"R125 switch back ON works again at once (no latch)");
    bodyRequiredD37=true;expectRoot(false,"R125 protected paired/masterstrike body census keeps native motion");bodyRequiredD37=false;
    animation_filter::nativeBodyOwned112=true;expectRoot(false,"R125 native body owner keeps native motion");animation_filter::nativeBodyOwned112=false;
    g_ctx_menu=true;expectRoot(false,"R125 menu context keeps native motion");g_ctx_menu=false;
    g_combat_experiment175=false;expectRoot(false,"R125 F4 off keeps native motion");g_combat_experiment175=true;
    animated.put(0x708,std::uint8_t(1));expectRoot(false,"R125 native no-movement (identity) path untouched");animated.put(0x708,std::uint8_t(0));
    AreaD37<0x800> npc;AreaD37<0x50> npcEnt;npc.put(0,baseD37+0x3A49468);npc.put(0x38,npcEnt.ptr());
    expectRoot(false,"R125 NPC animated character keeps its native attack motion",npc.address());
    animated.put(0,baseD37+0x3A49470);expectRoot(false,"R125 wrong animated-character vtable stays native");animated.put(0,baseD37+0x3A49468);
    player.put(0x268,npc.ptr());expectRoot(false,"R125 animated character not linked from Henry's player stays native");player.put(0x268,animated.ptr());
    animated.put(0x68C,NAN);expectRoot(false,"R125 non-finite requested movement stays native");animated.put(0x68C,.01f);
    g_root_motion_contract125=0;expectRoot(false,"R125 unverified contract cannot write");g_root_motion_contract125=baseD37;
    selectedD37=true;bindPunch(0,0,5);phase(8);expectRoot(true,"R125 tracked ordinary punch phase 8 also loses the clip step");
    type.put(8,8);expectRoot(false,"R125 kick with fists keeps its native step");type.put(8,5);
    selectedD37=false;melee(0);phase(8);
    ck(NativeRootMotionContract125(baseD37),"R125 root-motion pins match bytes read from the supplied DLL");
    for(auto& block:codeD40){
        if(block.rva!=0x839FF4&&block.rva!=0x83A016&&block.rva!=0x83A06B&&block.rva!=0x83A0D6&&block.rva!=0x83A195&&block.rva!=0x83A7A2&&block.rva!=0x83A72B&&block.rva!=0x3A49468+0x2F0)continue;
        block.bytes[0]^=1;ck(!NativeRootMotionContract125(baseD37),"R125 each root-motion pin fails closed when changed");block.bytes[0]^=1;
    }
    kcd2vr::diagnostics::capture168::gate.select(kcd2vr::diagnostics::capture168::Domain::Combat,true,1000);
    const auto lines=diagnostic_trace::linesD125;
    expectRoot(true,"R125 F3 recorder ON: identical decision");expectRoot(true,"R125 F3 second frame");
    phase(32);expectRoot(false,"R125 window closes at phase 32");phase(8);
    ck(diagnostic_trace::linesD125==lines+1&&diagnostic_trace::lastD125.find("R125 ROOT_MOTION_VETO frames=2")==0,"R125 one F3 row per attack window with the removed travel");
    kcd2vr::diagnostics::capture168::gate.select(kcd2vr::diagnostics::capture168::Domain::Combat,false,1000);
    }
#endif
    std::printf("D40 actual queued-rotation adapter: %u checks, %u failures; feature_present=%d; native/Windows/headset execution NOT_RUN\n",checksD40,failuresD40,D40_HAS_ROTATION);
    return failuresD40?1:0;
}
