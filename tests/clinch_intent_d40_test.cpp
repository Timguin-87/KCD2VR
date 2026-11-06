// Actual contact producer + complete production clinch callback. Native memory
// and the already-qualified frame readers are explicit test boundaries.
#include "vrik/clinch_intent_d40.h"
#include <array>
#include <atomic>
#include <cassert>
#include <cstdio>
#include <cstring>
#include <limits>
#include <vector>
#include <functional>
namespace ci=kcd2vr::clinch_intent_d40;
namespace npc172=kcd2vr::npc_contact172;
namespace pm=kcd2vr::pose_core;
namespace ph {using Vec3=pm::Vec3;}
static unsigned checks=0,failures=0;
static void check(bool value,const char* what){++checks;if(!value){++failures;std::printf("FAIL %s\n",what);}}
static std::uint64_t now=1000,publication=20;
static std::uintptr_t base=0;
static auto GetTickCount64() noexcept{return now;}
static auto WhBase() noexcept{return base;}
static std::vector<std::pair<std::uintptr_t,std::size_t>> ranges;
static std::function<void(std::uintptr_t)> readObserver;
template<class T> static bool Read(const void* p,T& value) noexcept {
    const auto a=reinterpret_cast<std::uintptr_t>(p);
    if(readObserver)readObserver(a);
    for(const auto& r:ranges)if(a>=r.first&&a-r.first<=r.second&&sizeof(T)<=r.second-(a-r.first)){
        std::memcpy(&value,p,sizeof(T));return true;}
    return false;
}
template<class T> static void put(unsigned char* p,const T& v){std::memcpy(p,&v,sizeof(v));}
template<class T> static void region(T& v){ranges.emplace_back(reinterpret_cast<std::uintptr_t>(v.data()),v.size());}
struct Matrix34V6550 {float m[12]{};};
static bool FiniteTmV6552(const Matrix34V6550& m){for(float x:m.m)if(!std::isfinite(x))return false;return true;}
namespace DetourModKit::hook {
enum class Gpr {Rax,Rbx};struct MidContext {std::uintptr_t registers[2]{},stack=0;};
static std::uintptr_t& gpr(MidContext& c,Gpr r){return c.registers[unsigned(r)];}
}
static std::atomic<bool> g_close_combat_ready175{true},g_gameplay_allowed{true},g_head_valid{true},g_right_valid{true},g_left_valid{true};
static std::atomic<bool> g_ctx_dialogue{false},g_ctx_menu{false},g_ctx_cutscene{false},g_ctx_scripted{false},g_fist_fault101{false};
static std::atomic<std::uint64_t> g_pose_epoch6689{2},g_fist_epoch101{9},g_weapon_request_epoch6694{5},g_phys_ready_sample6698{99},g_contact_control_epoch6695{17};
static std::atomic<void*> g_committed_character6689{};
static std::atomic<std::uintptr_t> g_player_char37{};
static std::atomic<void*> g_henry_char_attachment_manager{};
static std::atomic<std::uint64_t> g_native_use_until107[2]{};
static bool selectedFist=false,modePermits=true;static int rung=4;
static bool FistSelected101(){return selectedFist;}static bool FistModePermits178r22c2(){return modePermits;}
static int F4Rung178r10(){return rung;}
namespace retail_build112 {static bool ready=true;static bool Ready(std::uintptr_t){return ready;}}
namespace diagnostics::lab {static bool reference=false;static bool NativeReference(){return reference;}}
namespace animation_filter {static std::atomic<bool> nativeBodyOwned112{false};static bool kick=false;static bool NativeKickActive175(){return kick;}}
namespace diagnostics::capture168 {enum class Domain{Combat};static bool active(Domain,std::uint64_t){return false;}}
namespace b168 {struct Event{int kind=0,type=0,origin=0;std::uint64_t tick=0,sample=0;std::uintptr_t actor=0,opponent=0;bool nativeResult=false,allowed=false;};}
static void BridgeEvent168(b168::Event){}
struct FistSample {bool valid=true,empty[2]{true,true};std::uint64_t epoch=2,sequence=99,tick=1000,actor=0;double grip[2]{},trigger[2]{};};
struct FistFrame101 {void* player=nullptr;std::uint64_t request=9;FistSample sample{};};
struct FistOwner101 {void* player=nullptr;bool empty=true;std::uint64_t request=9,epoch=2;};
struct PhysFrame6698 {
    struct In {std::uint64_t sample=99,tick=1000,bodyEpoch=2;bool allowed=true,coherentMelee119=true,tracked[2]{true,true};} input;
    struct Profile {bool meleeValue=true;bool melee() const{return meleeValue;}} profiles[2];
    struct Out {bool active[2]{true,false},owned[2]{true,false};} output;
    void* player=nullptr;std::uint64_t request=5;
};
static kcd2vr::core::SnapshotMailbox<FistFrame101> g_fist_frame101;
static kcd2vr::core::SnapshotMailbox<PhysFrame6698> g_phys_frame6698;
struct NativeOwner104 {void* player=nullptr,*entity=nullptr,*actor=nullptr,*state=nullptr;std::uint64_t epoch=2;};
static NativeOwner104 henry;
static std::function<void()> resolveObserver;
static bool NativeResolve104(void* p,NativeOwner104& out){if(resolveObserver)resolveObserver();if(p!=henry.player)return false;out=henry;return true;}
static bool FistReadOwner101(FistOwner101& out){out.player=henry.player;return true;}
namespace nc104 {struct Observed{};}
static bool NativeObserve104(const NativeOwner104&,nc104::Observed&){return true;}
namespace native_admission147 {static bool guardWritable(const nc104::Observed&){return true;}}
struct ReadScope101R2{};
static bool CurrentOpponent168(void* actor,void*& state){void* back=nullptr;std::uintptr_t vf=0;
    return Read(actor,vf)&&(vf==base+0x3E3FAC8||vf==base+0x3A80610)&&Read(static_cast<unsigned char*>(actor)+0x210,state)&&
        state&&Read(static_cast<unsigned char*>(state)+0xCE0,back)&&back==actor;}
static bool PhysEntityIdentity135(void* entity,std::uint32_t id){std::uint32_t actual=0;std::uintptr_t vf=0;
    return Read(entity,vf)&&vf==base+0x4072C78&&Read(static_cast<unsigned char*>(entity)+0xC,actual)&&actual==id;}
struct ModelWorld6692 {pm::Quat q{};pm::Vec3 t{};float scale=1;};
static ModelWorld6692 liveRoot;static bool rootReadable=true;
static bool ReadModelWorld6692(void* character,ModelWorld6692& out){out=liveRoot;return rootReadable&&reinterpret_cast<std::uintptr_t>(character)==g_player_char37.load();}
struct CommittedFrame6695 {pm::Stamp stamp{};float world[8]{0,0,0,1,0,0,0,1};std::uint64_t publicationR24=20,doorTrackingEpoch66=3;bool valid[2]{true,true};};
struct TrackingSnapshot {std::uint64_t tick=1000,source_epoch=3;bool allowed=true,head_valid=true,head_held=false,left_held=false,right_held=false;float left_grip=1,right_grip=0,left_trigger=0,right_trigger=0;};
static CommittedFrame6695 committed;static TrackingSnapshot tracking;static bool committedOK=true,trackingOK=true;
static bool ReadCommittedPacket6695(CommittedFrame6695& out){out=committed;return committedOK;}
static bool ReadTrackingSnapshot(TrackingSnapshot& out){out=tracking;return trackingOK;}
static std::uint64_t ReadCompletedPublicationT23(){return publication;}
namespace tracking59c6 {static std::atomic<std::uint64_t> epoch{3};}
static bool handUsable[2]{true,true};static int itemState[2]{1,0},physicalState[2]{1,0};
static bool TrackingHandUsable111(int side,std::uint64_t){return handUsable[side];}
static int ExactHandItemState(int side){return itemState[side];}
static int ReadPhysicalHandHeld55(int side,std::uint64_t){return physicalState[side];}
#include "clinch_adapter_d40_production.inc"

int main(){
    std::vector<unsigned char> image(0x5400000);base=reinterpret_cast<std::uintptr_t>(image.data());region(image);
    std::array<unsigned char,0x500> actor{},enemy{};
    std::array<unsigned char,0xE00> state{},enemyState{};
    std::array<unsigned char,0x300> enemyOwner{},player{};
    std::array<unsigned char,0x100> entity{},other{},factory{},body{},character{};
    region(actor);region(enemy);region(state);region(enemyState);region(enemyOwner);region(player);region(entity);region(other);region(factory);region(body);region(character);
    henry={player.data(),entity.data(),actor.data(),state.data(),2};g_committed_character6689=character.data();
    g_player_char37=reinterpret_cast<std::uintptr_t>(character.data());g_henry_char_attachment_manager=character.data()+8;
    put(actor.data(),base+0x3A80610);put(enemy.data(),base+0x3E3FAC8);
    put(actor.data()+0x210,state.data());put(enemy.data()+0x210,enemyState.data());
    put(state.data()+0xCE0,actor.data());put(enemyState.data()+0xCE0,enemy.data());
    put(state.data()+0xCF8,enemy.data());put(enemyState.data()+0xCF8,actor.data());
    put(state.data()+8,std::uint8_t(1));put(enemyState.data()+8,std::uint8_t(1));
    put(actor.data()+0x3F0,factory.data());put(enemy.data()+0x3F0,factory.data());put(factory.data()+8,actor.data());
    put(enemy.data()+0x1F8,enemyOwner.data());put(enemyOwner.data()+0x278,enemy.data());put(enemyOwner.data()+0x38,other.data());
    put(entity.data(),base+0x4072C78);put(other.data(),base+0x4072C78);put(entity.data()+0xC,std::uint32_t(123));put(other.data()+0xC,std::uint32_t(456));
    Matrix34V6550 tm{};tm.m[0]=tm.m[5]=tm.m[10]=1;put(entity.data()+0x58,tm);tm.m[7]=.4f;put(other.data()+0x58,tm);
    put(image.data()+0x53204E4,35);put(body.data(),base+0x3A82570);put(body.data()+0x20,other.data());
    put(image.data()+0x3A82570+0x50,base+0x5D6F44);
    PhysFrame6698 f{};f.player=player.data();g_phys_frame6698.publish(f);
    FistFrame101 fist{};fist.player=player.data();fist.sample.actor=reinterpret_cast<std::uintptr_t>(character.data());g_fist_frame101.publish(fist);
    committed.stamp.actor=reinterpret_cast<std::uintptr_t>(character.data());committed.stamp.epoch=2;committed.stamp.sample=2;committed.stamp.tick=1000;
    npc172::Witness witness{};witness.ownerRead=true;witness.owner=reinterpret_cast<std::uintptr_t>(other.data());witness.collider=reinterpret_cast<std::uintptr_t>(body.data());
    witness.entityId=456;witness.part=0;witness.body=npc172::Body::Living;
    kcd2vr::passive169r1::Sample s{};s.actor=committed.stamp.actor;s.epoch=2;s.request=17;s.side=1;s.key=0x48414e4400000001ull;
    s.source=kcd2vr::passive169r1::Source::HandWorld;s.status=kcd2vr::passive169r1::Status::Contact;s.eligible=s.worldCoherent=true;s.point={0,.4,1};s.normal={0,-1,0};
    auto press=[&](bool fistClosed=false){ci::setMode(0);ci::setMode(1);s.sequence=1;s.tick=990;ci::publish(s,witness,0,18);s.sequence=2;s.tick=1000;ci::publish(s,witness,1,20,fistClosed);};
    auto gate=[&](bool reject,const char* message,bool native=true){
        using namespace DetourModKit::hook;MidContext c{};c.registers[0]=native?0xAABB01:0xAABB00;c.registers[1]=reinterpret_cast<std::uintptr_t>(factory.data());c.stack=0x9876;
        const auto before=c;CloseCombatAutoGate175(c);
        check(c.registers[0]==(reject?0xAABB00:before.registers[0]),message);
        check(c.registers[1]==before.registers[1]&&c.stack==before.stack,"only native result may change");};
    ci::setMode(2);gate(true,"automatic OFF rejects inside radius");gate(false,"native false preserved",false);
    put(factory.data()+8,enemy.data());gate(false,"D41 enemy-initiated clinch stays native when player automatic clinch is OFF");put(factory.data()+8,actor.data());
    ci::setMode(0);gate(false,"legacy distance preserves inside native candidate");tm.m[7]=.8f;put(other.data()+0x58,tm);gate(true,"legacy radius still rejects outside");tm.m[7]=.4f;put(other.data()+0x58,tm);
    ci::setMode(1);gate(true,"contact mode requires a deliberate current grip");
    press();gate(false,"native false cannot consume grip",false);gate(false,"free left hand contact admits native candidate with sword in right");gate(true,"same grip consumed once");
    ci::setMode(1);put(factory.data()+8,enemy.data());gate(false,"D41 enemy initiation needs no player grip ticket");put(factory.data()+8,actor.data());
    press();put(factory.data()+8,enemy.data());gate(false,"D41 enemy initiation preserves a pending player grip");put(factory.data()+8,actor.data());
    gate(false,"D41 player may still use the grip after the enemy candidate");gate(true,"D41 only the player candidate consumes that grip");
    press();tracking.left_grip=0;gate(true,"live release revokes grip");tracking.left_grip=1;
    press();tracking.left_trigger=1;gate(true,"live punching trigger cannot become clinch");tracking.left_trigger=0;
    press(true);gate(true,"closed-fist producer cannot create clinch ticket");
    press();itemState[1]=1;gate(true,"occupied selected hand refused");itemState[1]=0;
    press();physicalState[1]=1;gate(true,"other physical hand owner refused");physicalState[1]=0;
    press();publication=22;gate(true,"another body publication cannot lend contact");publication=20;
    press();liveRoot.t.x=.01;gate(true,"root translation after publication invalidates contact");liveRoot={};
    press();liveRoot.q={0,0,std::sin(.01),std::cos(.01)};gate(true,"root yaw after publication invalidates contact");liveRoot={};
    press();liveRoot.scale=1.01f;gate(true,"root scale after publication invalidates contact");liveRoot={};
    press();rootReadable=false;gate(true,"unreadable current root cannot admit old contact");rootReadable=true;
    press();committed.world[4]=std::numeric_limits<float>::quiet_NaN();gate(true,"non-finite committed root cannot admit contact");committed.world[4]=0;
    press();liveRoot.t.x=.001;gate(false,"existing two-millimetre contact tolerance retained");liveRoot={};
    auto notConsumed=[&](const char* what){ci::Ticket t{};ci::tickets[1].read(t);check(t.serial&&ci::used[1].load()<t.serial,what);};
    const auto foreignAddress=reinterpret_cast<std::uintptr_t>(body.data()+0x20);
    unsigned foreignReads=0;
    press();readObserver=[&](std::uintptr_t address){if(address==foreignAddress&&++foreignReads==2)liveRoot.t.x=.01;};
    gate(true,"root shift during final foreign-owner read revokes contact");readObserver={};notConsumed("root refusal does not consume deliberate press");liveRoot={};
    press();foreignReads=0;readObserver=[&](std::uintptr_t address){if(address==foreignAddress&&++foreignReads==2)animation_filter::nativeBodyOwned112=true;};
    gate(false,"protected paired takeover during final intent read remains native");readObserver={};notConsumed("protected takeover consumes no press");animation_filter::nativeBodyOwned112=false;
    press();foreignReads=0;readObserver=[&](std::uintptr_t address){if(address==foreignAddress&&++foreignReads==2)g_ctx_menu=true;};
    gate(false,"menu takeover during final intent read remains native");readObserver={};notConsumed("context takeover consumes no press");g_ctx_menu=false;
    press();foreignReads=0;readObserver=[&](std::uintptr_t address){if(address==foreignAddress&&++foreignReads==2)g_pose_epoch6689=3;};
    gate(false,"pose epoch takeover during final intent read remains native");readObserver={};notConsumed("epoch takeover consumes no press");g_pose_epoch6689=2;
    unsigned resolves=0;
    press();resolveObserver=[&](){if(++resolves==3)publication=22;};
    gate(true,"publication changed after final intent read cannot consume previous contact");resolveObserver={};notConsumed("new publication consumes no retired press");publication=20;
    press();resolves=0;resolveObserver=[&](){if(++resolves==3)tracking59c6::epoch=4;};
    gate(false,"tracking epoch takeover during last candidate recheck remains native");resolveObserver={};notConsumed("tracking epoch takeover consumes no press");tracking59c6::epoch=3;


    press();g_player_char37=reinterpret_cast<std::uintptr_t>(character.data())+16;gate(true,"old committed frame cannot borrow replaced player character");g_player_char37=reinterpret_cast<std::uintptr_t>(character.data());
    press();g_henry_char_attachment_manager=nullptr;gate(true,"cleared attachment owner revokes old current contact");g_henry_char_attachment_manager=character.data()+8;
    press();g_henry_char_attachment_manager=character.data()+16;gate(true,"changed attachment owner cannot lend old contact");g_henry_char_attachment_manager=character.data()+8;
    press();put(body.data()+0x20,entity.data());gate(true,"changed collider foreign owner refused");put(body.data()+0x20,other.data());
    press();put(body.data(),base+0x3A48DC0);gate(true,"rigid prop cannot become NPC contact");put(body.data(),base+0x3A82570);
    press();now=1081;f.input.tick=now;g_phys_frame6698.publish(f);tracking.tick=now;gate(true,"stale contact refused even with fresh weapon frame");now=1000;f.input.tick=1000;g_phys_frame6698.publish(f);tracking.tick=1000;
    press();tm.m[7]=.8f;put(other.data()+0x58,tm);gate(true,"physical intent does not bypass distance");tm.m[7]=.4f;put(other.data()+0x58,tm);gate(false,"distance refusal does not consume grip");
    press();animation_filter::nativeBodyOwned112=true;gate(false,"active protected action unchanged");animation_filter::nativeBodyOwned112=false;
    ci::setMode(2);rung=0;gate(false,"outside selected VR combat native result unchanged");rung=4;
    retail_build112::ready=false;gate(false,"unverified build untouched");retail_build112::ready=true;
    ci::setMode(1);s.sequence=3;s.tick=1010;ci::publish(s,witness,1,22);ci::Ticket t{};ci::tickets[1].read(t);check(!t.serial,"mode change does not manufacture a new grip");
    press();ci::tickets[1].read(t);check(t.serial!=0,"physical ticket exists before expiry");s.sequence=3;s.tick=1251;ci::publish(s,witness,1,22);ci::tickets[1].read(t);check(!t.serial,"held grip cannot refresh original press forever");
    ci::setMode(0);tm.m[7]=.8f;put(other.data()+0x58,tm);put(factory.data()+8,enemy.data());
    gate(false,"D41 player distance setting does not veto enemy initiation");put(factory.data()+8,actor.data());
    gate(true,"D41 player distance setting still rejects the player's candidate");tm.m[7]=.4f;put(other.data()+0x58,tm);
    selectedFist=true;fist.sample.grip[0]=fist.sample.trigger[0]=1;g_fist_frame101.publish(fist);
    put(factory.data()+8,enemy.data());gate(false,"D41 legacy closed fists cannot veto enemy clinch");put(factory.data()+8,actor.data());
    gate(true,"D41 legacy closed-fist veto remains player-only");
    // Native distance disables the modern distance gate; the legacy fist
    // callback must independently retain cutscene/context ownership.
    g_clinch_distance_meters32=0;
    g_ctx_cutscene=true;gate(false,"D41 legacy cutscene preserves native candidate");g_ctx_cutscene=false;
    g_ctx_menu=true;gate(false,"D41 legacy menu preserves native candidate");g_ctx_menu=false;
    g_ctx_dialogue=true;gate(false,"D41 legacy dialogue preserves native candidate");g_ctx_dialogue=false;
    g_ctx_scripted=true;gate(false,"D41 legacy scripted owner preserves native candidate");g_ctx_scripted=false;
    rung=0;gate(false,"D41 legacy unselected combat preserves native candidate");rung=4;
    resolveObserver=[&](){g_ctx_cutscene=true;};
    gate(false,"D41 legacy takeover during native owner read preserves candidate");resolveObserver={};g_ctx_cutscene=false;
    resolveObserver=[&](){now=1081;};
    gate(false,"D41 legacy aged fist proof cannot veto after native read");resolveObserver={};now=1000;
    resolveObserver=[&](){g_fist_epoch101=10;};
    gate(false,"D41 legacy changed fist request cannot veto after native read");resolveObserver={};g_fist_epoch101=9;
    resolveObserver=[&](){ci::setMode(2);};
    gate(false,"D41 legacy policy change cannot veto after native read");resolveObserver={};ci::setMode(0);
    gate(true,"D41 legacy fresh gameplay still vetoes player candidate");g_clinch_distance_meters32=.55f;
    selectedFist=false;ci::setMode(2);g_ctx_cutscene=true;
    gate(false,"D41 cutscene keeps native player initiation");put(factory.data()+8,enemy.data());
    gate(false,"D41 cutscene keeps native enemy initiation");put(factory.data()+8,actor.data());g_ctx_cutscene=false;
    std::printf("D41 actual clinch producer/adapter: %u checks, %u failures; native engine leaves substituted\n",checks,failures);
    return failures?1:0;
}
