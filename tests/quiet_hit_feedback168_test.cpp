// Actual complete combat diagnostic TU; synthetic native leaves/identity memory.
// GCC/Clang host only: override return-address intrinsic, not game code.
#include <cstdint>
#include <vector>
#include <string>
#include <functional>
static void* testCaller168=nullptr;
#define __builtin_return_address(n) testCaller168
#include "../src/combat_diag_probe.cpp"
#undef __builtin_return_address
static void* playerEntity168=nullptr;
static unsigned notifications168=0,originalCalls168=0,checks175=0,failures175=0;
static bool nativeResult168=true,inputOwned175=true,geometryOwner18=true;
static kcd2vr::user175::Configuration gameplay175=kcd2vr::user175::configuration(kcd2vr::user175::Mode::Weapons);
static std::vector<std::string> records168;
struct Arguments175 {void* rpg;void* hit;const float* strength;bool* post;const void* request;bool secondary;int type;void* details;};
static Arguments175 originalArgs175{};
static kcd2vr::answer59c4r2::Lineage answerLineage19c4r2{};
static bool answerKnown19c4r2=false,replaceAnswerDuringNative19c4r2=false;
static unsigned answerSnapshots19c4r2=0;
static std::function<void()> nestedSubmit19c4r3;
namespace kcd2vr::diagnostic_trace {void Write(const char* m) noexcept {records168.emplace_back(m);}}
namespace kcd2vr::frik_pose_probe {
// This passive native probe fixture has no physical fist owner.
int FistContactAdmissionH12(const void*,const float*,int,const void*) noexcept {return -1;}

// Diagnostic-only native namespace reader is absent in this passive combat fixture.
void ObserveHitPartNamespace62(const kcd2vr::part_namespace62::Ticket&) noexcept {}

bool GeometryNativeAnswer19c4r2(const void*,const void*,int,kcd2vr::answer59c4r2::Lineage& out) noexcept {
    ++answerSnapshots19c4r2;out=answerKnown19c4r2?answerLineage19c4r2:kcd2vr::answer59c4r2::Lineage{};return answerKnown19c4r2;
}
void* LastPlayerEntity() noexcept {return playerEntity168;}
bool GeometryOwnerValid178r30(std::uint64_t,std::uint64_t,std::uint64_t,std::uint64_t) noexcept {return geometryOwner18;}
int GeometryOwnerStage49() noexcept {return 0;}
bool CombatExperiment175Enabled() noexcept {return gameplay175.experiment;}
bool ReadF4ContactLive176(kcd2vr::contact176::Live&) noexcept {return false;}
// Only this external gameplay owner is replaced. SubmitHitDetour, actor
// qualification, result handling and capture gates below are production code.
bool PhysicalWeaponInputOwned6698() noexcept {return inputOwned175&&FullWeaponMode120(gameplay175.weapon);}
void NotifyNativeWeaponImpact136R2(float,float,float,bool,bool,bool,std::uint32_t) noexcept {++notifications168;}
}
using namespace kcd2vr::combat_diag_probe;
static void check175(bool ok,const char* message){++checks175;if(!ok){++failures175;std::printf("FAIL %s\n",message);}}
static bool __fastcall Native168(void* rpg,void* hit,const float* strength,bool* post,const void* request,bool secondary,int type,void* details) {
    if(replaceAnswerDuringNative19c4r2){++answerLineage19c4r2.release.serial;++answerLineage19c4r2.release.gesture59c3;++answerLineage19c4r2.action;}
    ++originalCalls168;originalArgs175={rpg,hit,strength,post,request,secondary,type,details};
    if(nestedSubmit19c4r3){auto callback=std::move(nestedSubmit19c4r3);nestedSubmit19c4r3={};callback();}
    return nativeResult168;
}
static std::array<std::uint64_t,11> counters175(){
    std::array<std::uint64_t,11> out{};for(unsigned i=0;i<out.size();++i)out[i]=nhp134::g_counts[i].load();return out;
}
int main(){
    namespace n=nhp134;namespace c=kcd2vr::diagnostics::capture168;namespace u=kcd2vr::user175;
    {namespace power=n::geom27::power59c8;namespace riposte=n::geom27::riposte59c8;
     n::geom27::Pending p{};p.entityId=555;p.powerRatio59c8=1;
     for(auto& entry:power::g_repeat)entry={};
     auto first=power::Compute(p,30000,false),refused=power::Compute(p,30001,false);
     check175(first.repeat==0&&refused.repeat==0&&first.power==refused.power,"undelivered preflight does not reduce next strike power");
     power::RecordDelivered(p,30002);
     check175(power::Compute(p,30003,false).repeat==1,"only delivered adjudication advances repeat count once");
     auto other=p;other.entityId=556;check175(power::Compute(other,30003,false).repeat==0,"repeat bookkeeping remains target-specific");
     const void* target=reinterpret_cast<void*>(0x1234);riposte::NotePerfectBlock(30000,target);
     check175(riposte::PeekFor(30001,target)&&!riposte::PeekFor(30001,reinterpret_cast<void*>(0x4321)),"counter preview requires the exact blocked attacker");
     (void)power::Compute(p,30001,riposte::PeekFor(30001,target));
     check175(riposte::PeekFor(30002,target),"preview/refused native transport does not consume counter window");
     check175(riposte::TakeFor(30003,target)&&!riposte::TakeFor(30004,target),"accepted counter consumes at most once");
     riposte::NotePerfectBlock(30005,target);const auto token=riposte::TokenForC2(30006,target);
     riposte::NotePerfectBlock(30007,target); // callback renewed it before the original hit returned
     check175(!riposte::TakeTokenC2(30008,target,token)&&riposte::PeekFor(30008,target),"native callback renewal cannot be consumed by an older contact");
     const auto replacement=riposte::TokenForC2(30008,target);
     check175(riposte::TakeTokenC2(30009,target,replacement)&&!riposte::TakeTokenC2(30010,target,replacement),"exact replacement window is consumed once");
     for(auto& entry:power::g_repeat)entry={};riposte::g_until=0;
    }
    n::g_base=0x180000000ull;n::g_submit_original=Native168;n::g_vr_weapon_log136=false;
    std::array<unsigned char,0xD00> actor{},state{},player{};std::array<unsigned char,0x80> entity{},request{},hit{},details{},rpg{};
    auto put=[](auto& a,std::size_t off,const auto& v){std::memcpy(a.data()+off,&v,sizeof(v));};
    put(actor,0,n::g_base+n::kCombatPlayerVft);put(actor,0x1F8,player.data());put(actor,0x210,state.data());
    put(state,0xCE0,actor.data());
    put(player,0,n::g_base+n::kPlayerVft);put(player,0x38,entity.data());put(player,0x278,actor.data());
    put(entity,0,n::g_base+n::kEntityVft);put(entity,0xC,std::uint32_t(100));playerEntity168=entity.data();
    put(rpg,8,actor.data());put(hit,0x28,std::uint32_t(200));put(hit,0x30,1.f);
    { // C54: actual application hook shares the physical attempt terminal claim.
     namespace m=kcd2vr::motion59c4;
     const auto oldCaller=testCaller168;const bool oldSync=n::geom27::g_sync59c10r9.load();
     const bool oldMotion=m::selected.load();const auto oldGameplay=gameplay175;
     gameplay175=u::configuration(u::Mode::Weapons);gameplay175.experiment=true;
     n::geom27::g_sync59c10r9=true;m::selected=false;
     m::hybridAttempts54.clear();const auto now=std::uint64_t(GetTickCount64());
     m::Key key{991,71,72,73,74,75,reinterpret_cast<std::uintptr_t>(player.data())};
     check175(m::hybridAttempts54.event(key,m::Event::Prepare,now,now,2,0),"C54 physical preparation exists independently of release");
     answerKnown19c4r2=true;answerLineage19c4r2={};auto& ticket=answerLineage19c4r2.release;
     ticket.gesture59c3=key.gesture;ticket.key=key.weapon;ticket.generation=key.generation;ticket.epoch=key.body;
     ticket.request=key.request;ticket.recovery59c3=key.recovery;ticket.player=key.player;ticket.serial=999;ticket.tick=now;
     answerLineage19c4r2.action=123;
     testCaller168=reinterpret_cast<void*>(n::g_base+0x173BF58ull);const auto calls=originalCalls168;
     float force=1;bool post=false;
     (void)n::SubmitHitDetour(rpg.data(),hit.data(),&force,&post,request.data(),true,1,details.data());
     check175(originalCalls168==calls+1,"C54 exact native application remains native on first entry");
     (void)n::SubmitHitDetour(rpg.data(),hit.data(),&force,&post,request.data(),true,1,details.data());
     check175(originalCalls168==calls+1,"C54 same ordinary native application cannot repeat an entered physical attempt");
     (void)n::SubmitHitDetour(rpg.data(),hit.data(),&force,&post,request.data(),false,1,details.data());
     check175(originalCalls168==calls+2,"C54 native candidate evaluation remains untouched after contact");
     testCaller168=reinterpret_cast<void*>(n::g_base+0xC6248Full);
     (void)n::SubmitHitDetour(rpg.data(),hit.data(),&force,&post,request.data(),true,1,details.data());
     check175(originalCalls168==calls+3,"C54 unrelated producer is never denied by the ordinary-followup gate");
     testCaller168=oldCaller;answerKnown19c4r2=false;answerLineage19c4r2={};
     n::geom27::g_sync59c10r9=oldSync;m::selected=oldMotion;gameplay175=oldGameplay;m::hybridAttempts54.clear();
    }
    {namespace g=n::geom27;g::Pending p{};p.serial=7;p.gesture59c3=9;p.entityId=100;p.tick=hostTickCount64;
     p.bodyMaterial=38;p.surface=116;p.part=4;p.energy=.8f;p.metadata59c7g.bodyMaterial=38;
     g::InjectResult35 result{};result.seen=result.ok=result.fields=true;result.target=100;result.death=1;
     result.blocked=result.perfect=result.parry=0;
     const auto before=records168.size();
     g::PublishContact59c7g(p,result,hostTickCount64,entity.data(),1,false,true,true,8,64);
     check175(records168.size()==before,"finisher diagnostics OFF adds no formatting or logging");
     g::decap59c7e::g_enabled=true;
     for(bool direct:{false,true}){
      g::PublishContact59c7g(p,result,hostTickCount64,entity.data(),1,direct,true,true,8,64);
      check175(records168.back().find("decapCandidate=1 failures=0x0")!=std::string::npos,
       "live and direct routes evaluate completed lethal contact despite native post-hit phase 64");
     }
     g::PublishContact59c7g(p,result,hostTickCount64,entity.data(),1,true,true,false,8,64);
     check175(records168.back().find("decapCandidate=0 failures=0x10")!=std::string::npos,
      "missing native continuation is an explicit refusal, never a successful sever");
     result.target=999;
     const auto mismatch=g::PublishContact59c7g(p,result,hostTickCount64,entity.data(),1,true,true,true,8,64);
     check175(!mismatch.fields&&!mismatch.applied&&!mismatch.continuation&&records168.back().find("decapCandidate=0")!=std::string::npos,
      "a mismatched native target cannot manufacture applied or continued finisher evidence");
     result.target=100;result.ok=false;
     const auto refused=g::PublishContact59c7g(p,result,hostTickCount64,entity.data(),1,false,true,true,8,64);
     check175(!refused.accepted&&!refused.fields&&!refused.applied&&!refused.continuation,
      "native refusal remains refused despite process/apply arguments");result.ok=true;
     p.metadata59c7g.bodyMaterial=3;
     g::PublishContact59c7g(p,result,hostTickCount64,entity.data(),1,true,true,true,8,64);
     check175(records168.back().find("bodyMaterialCandidate=3 submittedBodyMaterial=38")!=std::string::npos&&
      records168.back().find("failures=0x80")!=std::string::npos,
      "contact material stays distinct from submitted or approximated native material");
     g::decap59c7e::g_enabled=false;records168.clear();kcd2vr::contact58c1::latest.publish({});
    }
    testCaller168=reinterpret_cast<void*>(n::g_base+0xD536BC);c::gate.mask=0;
    float strength=1;bool post=false;
    auto invoke=[&](bool notify,const char* label){
        const auto calls=originalCalls168,pulses=notifications168;
        const bool result=n::SubmitHitDetour(rpg.data(),hit.data(),&strength,&post,request.data(),true,1,details.data());
        check175(result==nativeResult168&&originalCalls168==calls+1&&notifications168==pulses+unsigned(notify),label);
        check175(originalArgs175.rpg==rpg.data()&&originalArgs175.hit==hit.data()&&originalArgs175.strength==&strength&&
            originalArgs175.post==&post&&originalArgs175.request==request.data()&&originalArgs175.secondary&&
            originalArgs175.type==1&&originalArgs175.details==details.data(),"original arguments forwarded unchanged exactly once");
    };
    invoke(true,"F12 quiet accepted collision notifies with F3 recorder and legacy log flag OFF");
    gameplay175=u::configuration(u::Mode::Combat);
    check175(gameplay175.experiment&&gameplay175.weapon==7,"fixture selects real F4 weapon/experiment configuration");
    invoke(true,"F4 quiet accepted collision notifies without enabling a recorder");
    check175(records168.empty()&&counters175()==std::array<std::uint64_t,11>{},"quiet positives have zero formatting and diagnostic counters");

    nativeResult168=false;invoke(false,"native rejection never pulses");nativeResult168=true;
    testCaller168=reinterpret_cast<void*>(n::g_base+0xC6248F);invoke(false,"native follow-up producer never pulses");
    testCaller168=reinterpret_cast<void*>(n::g_base+0xD536BC);
    put(hit,0x28,std::uint32_t(0));invoke(false,"environment target zero never pulses");
    put(hit,0x28,std::uint32_t(100));invoke(false,"self target never pulses");
    put(hit,0x28,std::uint32_t(200));
    playerEntity168=nullptr;invoke(false,"unmatched Henry entity cannot pulse");playerEntity168=entity.data();
    put(state,0xCE0,static_cast<void*>(nullptr));invoke(false,"broken combat-state backlink cannot pulse");put(state,0xCE0,actor.data());
    put(player,0x278,static_cast<void*>(nullptr));invoke(false,"broken player-to-actor backlink cannot pulse");put(player,0x278,actor.data());
    put(actor,0,std::uintptr_t(0));invoke(false,"unknown combat actor vtable cannot pulse");put(actor,0,n::g_base+n::kCombatPlayerVft);
    // Exercise the quiet unknown-NPC path too: it used to update diagnostic
    // identity counters and format NPC_OWNER_UNRESOLVED outside recording.
    put(actor,0,n::g_base+n::kCombatActorVft);put(player,0x38,static_cast<void*>(nullptr));
    invoke(false,"unresolved NPC owner stays quiet and cannot pulse");
    put(actor,0,n::g_base+n::kCombatPlayerVft);put(player,0x38,entity.data());
    inputOwned175=false;invoke(false,"lost or stale physical gameplay owner cannot pulse");inputOwned175=true;
    gameplay175=u::configuration(u::Mode::Off);invoke(false,"all VR modes OFF cannot acquire hit feedback");
    n::g_vr_weapon_log136=true;invoke(false,"legacy logging flag alone cannot acquire feedback or quiet diagnostics");n::g_vr_weapon_log136=false;
    gameplay175=u::configuration(u::Mode::Weapons);
    c::gate.select(c::Domain::Hud,true,GetTickCount64());c::gate.select(c::Domain::Interaction,true,GetTickCount64());c::gate.select(c::Domain::Reload,true,GetTickCount64());
    invoke(true,"F2/F5/F11 recorders do not change accepted weapon feedback");
    check175(records168.empty()&&counters175()==std::array<std::uint64_t,11>{},"quiet negatives and other-domain recorders update no combat counters or formatting");

    c::gate.select(c::Domain::Combat,true,GetTickCount64());n::g_vr_weapon_log136=true;
    invoke(true,"F3 ON adds recording without changing accepted feedback");
    const auto firstCaptured=counters175();
    check175(!records168.empty()&&firstCaptured[1]==1&&firstCaptured[2]==1,"F3 ON owns the submit/player diagnostic counters and output");
    const auto evalCalls=originalCalls168,evalPulses=notifications168;
    check175(n::SubmitHitDetour(rpg.data(),hit.data(),&strength,&post,request.data(),false,1,details.data())&&
      originalCalls168==evalCalls+1&&notifications168==evalPulses,"F3 evaluation still calls native once without application feedback");
    n::services59c5::Drain();bool evaluationRecord=false,applicationRecord=false;
    for(const auto& line:records168){
      if(line.find("NATIVE_SERVICE seq=")!=std::string::npos&&line.find("hitInfoHealthDamage=")!=std::string::npos){
        evaluationRecord|=line.find("hitEffect=EVALUATION")!=std::string::npos;
        applicationRecord|=line.find("hitEffect=APPLICATION_REQUEST")!=std::string::npos;
      }}
    check175(evaluationRecord&&applicationRecord,"actual service journal preserves evaluation and application with explicit computed-HitInfo labels");
    const auto captured=counters175();const auto recordCount=records168.size();
    c::gate.select(c::Domain::Combat,false,GetTickCount64());n::g_vr_weapon_log136=false;
    invoke(true,"F3 OFF immediately retains the same qualified native-hit notification");
    check175(records168.size()==recordCount&&counters175()==captured,"F3 OFF freezes diagnostic output and counters");
    inputOwned175=false;const auto calls=originalCalls168,pulses=notifications168;
    check175(n::SubmitHitDetour(nullptr,nullptr,nullptr,nullptr,nullptr,false,1,nullptr)&&
        originalCalls168==calls+1&&notifications168==pulses,"inactive early path forwards null fixture arguments exactly once without qualification");
    {   // C4R3: preserve native defensive evaluation; application-only refusal
        // remains limited to our exact current DIRECT owner.
        namespace vs=n::geom27::vrstate596;inputOwned175=true;
        kcd2vr::motion59c4::selected=true;vs::g_consequence=true;vs::g_fault=false;vs::g_henry_actor=actor.data();vs::g_own_hits18=true;
        std::array<unsigned char,0xD00> opponent18{};
        put(actor,0x388,rpg.data());put(state,0xCF8,opponent18.data());put(request,0x10,opponent18.data());
        put(state,n::geom27::kActionPhaseProp39+8,int(8));put(state,n::kAttackType+8,int(1));
        vs::g_owner_state=state.data();vs::g_key5912={1,77,4,1,1,kcd2vr::contact176::recoveryEpoch55c1.load(),reinterpret_cast<std::uintptr_t>(player.data())};
        vs::g_stage=8;vs::g_gesture=1;vs::g_deadline=GetTickCount64()+600;vs::g_hit_at5910c1=0;vs::SnapshotAttack(GetTickCount64());
        auto call=[&](){return n::SubmitHitDetour(rpg.data(),hit.data(),&strength,&post,request.data(),false,1,details.data());};
        testCaller168=reinterpret_cast<void*>(n::g_base+0x173BF58);auto before=originalCalls168;const auto refused=vs::g_followup_suppressed18.load();
        check175(call()&&originalCalls168==before+1&&vs::g_followup_suppressed18.load()==refused,"C4R3 VR direct preserves native FOLLOWUP_C evaluation of Henry's attack");
        testCaller168=reinterpret_cast<void*>(n::g_base+0xD536BC);before=originalCalls168;(void)call();check175(originalCalls168==before+1,"C10R18 a real collision is forwarded");
        testCaller168=reinterpret_cast<void*>(n::g_base+0x173BF58);
        vs::g_stage=0;vs::SnapshotAttack(GetTickCount64());before=originalCalls168;(void)call();check175(originalCalls168==before+1,"no current direct attempt preserves native followup");
        vs::g_stage=8;vs::SnapshotAttack(GetTickCount64());
        geometryOwner18=false;before=originalCalls168;(void)call();check175(originalCalls168==before+1,"replaced/stale weapon owner preserves native followup");geometryOwner18=true;
        ++kcd2vr::contact176::recoveryEpoch55c1;before=originalCalls168;(void)call();check175(originalCalls168==before+1,"new collision recovery epoch preserves native followup");--kcd2vr::contact176::recoveryEpoch55c1;
        before=originalCalls168;(void)n::SubmitHitDetour(rpg.data(),hit.data(),&strength,&post,request.data(),false,2,details.data());
        check175(originalCalls168==before+1,"native special attack type preserves native followup");
        before=originalCalls168;(void)n::SubmitHitDetour(rpg.data(),hit.data(),&strength,&post,request.data(),true,1,details.data());
        check175(originalCalls168==before,"unexpected application-only followup remains denied for the exact current DIRECT contact owner");
        put(state,0xCF8,static_cast<void*>(nullptr));before=originalCalls168;(void)call();check175(originalCalls168==before+1,"changed opponent preserves native followup");put(state,0xCF8,opponent18.data());
        put(state,n::geom27::kActionPhaseProp39+8,int(64));before=originalCalls168;(void)call();check175(originalCalls168==before+1,"native reaction phase preserves native followup");put(state,n::geom27::kActionPhaseProp39+8,int(8));
        testCaller168=reinterpret_cast<void*>(n::g_base+0x173BF58);vs::g_henry_actor=nullptr;before=originalCalls168;(void)call();
        check175(originalCalls168==before+1,"C10R18 an NPC's parry-in-place (attacker not Henry) is forwarded");vs::g_henry_actor=actor.data();
        vs::g_own_hits18=false;before=originalCalls168;(void)call();check175(originalCalls168==before+1,"C10R18 switch OFF forwards");vs::g_own_hits18=true;
        kcd2vr::motion59c4::selected=false;before=originalCalls168;(void)call();check175(originalCalls168==before+1,"C10R18 only the VR direct backend refuses");
        vs::g_consequence=false;vs::g_henry_actor=nullptr;testCaller168=reinterpret_cast<void*>(n::g_base+0xD536BC);}
    #include "answer_lineage19c4r2_cases.inl"
    std::printf("quiet_hit_feedback175=%u/%u production_submit=YES native_leaves=REPLACED retail_execution=NONE\n",checks175-failures175,checks175);
    return failures175?1:0;
}
