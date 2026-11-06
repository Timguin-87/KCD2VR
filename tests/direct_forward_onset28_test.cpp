// Actual physical command producer: no mirrored recognizer or invented NPC outcome.
#include "vrik/physical_weapon.h"
#include "vrik/contact_authority176.h"
#include <cstdio>
#include <vector>
using namespace kcd2vr::physical;
static unsigned checks=0,failures=0;
static void Check(bool ok,const char* label){++checks;if(!ok){++failures;std::printf("FAIL %s\n",label);}}
namespace ph=kcd2vr::physical;
namespace mo28=kcd2vr::motion59c4;
struct ConsumerFrame28 {};
struct ConsumerEquipment28 {void* player=reinterpret_cast<void*>(0x777);};
static struct ConsumerOwner28 {bool attackPrepared=false;} consumerOwner28;
static struct Commands28 {unsigned popped=0;bool pop(ph::Command&){++popped;return true;}} g_phys_commands6698;
static unsigned cancelCalls28=0,guardHandoffs28=0;
static void PhysCancelAttack117(){++cancelCalls28;consumerOwner28.attackPrepared=false;}
static void NativeDirectPrepareGuardHandoff19c4r2(const ph::Command& c,const ConsumerFrame28&,std::uint64_t){
    Check(c.kind==CommandKind::Prepare&&!c.held57,"actual DIRECT consumer sees qualified forward Prepare, never rear held provenance");++guardHandoffs28;}
namespace diagnostic_trace {static void Write(const char*) noexcept {}}
namespace combat_diag_probe {
struct Event28 {mo28::Key key;std::uint64_t tick;int kind,zone,technique;};static std::vector<Event28> events28;
static bool VrStateConsequence596(){return true;}
static bool VrStateEvent5912(const mo28::Key& key,std::uint64_t tick,int kind,int zone,int technique){events28.push_back({key,tick,kind,zone,technique});return true;}
}
static void TraceHitCommandC54R3(const ph::Command&,const char*,const char*,std::uint64_t,bool){} // diagnostic sink only
#include "direct_consumer28.inc"
struct Replay28 {
    Input in{};State state{};std::array<Profile,2> profiles{};Output out{};std::vector<Command> commands;
    bool blocked=false,actor=false,known=true;
    Replay28(bool direct=true){
        in.epoch=in.bodyEpoch=1;in.tick=1000;in.timeNs=1000000000;in.sample=1;
        in.allowed=in.tracked[0]=in.tracked[1]=in.intentTracked[0]=true;in.grip[0]=1;
        in.requirePrimaryGrip114=in.heldMotionAttacks=in.coherentMelee119=in.controllerAnchor119=true;
        in.naturalCombat117=in.physicalCombat177=in.forwardOnly19=in.thrustAsSwing55=true;
        in.motionOwned59c5=direct;in.windupHold57=in.telegraphWindup59c10=true;
        in.earlyWarning33=true; // C35: execute actual C34 ON configuration, never hide its regression behind a struct default
        in.dynamics=false;in.contact=true;in.reach[0]=1.9;in.strikeMinScaleGate59c14=false;
        in.shoulders[0]={.2,0,1.3};in.shoulders[1]={-.2,0,1.3};
        in.hands[0].p={.3,.1,1.25};in.intentHands[0]=in.hands[0];
        auto& p=profiles[0];p.key=p.generation=1;p.classId=4;p.primary=p.geometry=true;p.axis={0,0,1};p.tip={0,0,.6};
        Rest(200);commands.clear();
    }
    void Move(Vec3 d={},bool model=true,bool relative=true){
        ++in.sample;in.tick+=10;in.timeNs+=10000000;
        if(model)in.hands[0].p=in.hands[0].p+d;if(relative)in.intentHands[0].p=in.intentHands[0].p+d;
        out=step(state,in,profiles,[&](const Profile&,Pose from,Pose,bool){Contact103 c{};c.known=known;
            if(blocked){c.fraction=0;c.plane=true;c.normal={0,-1,0};c.point=from.p;
                if(actor){c.currentOpponent175=true;c.native172.mask=kcd2vr::npc_contact172::BodiesAndWorld;
                    c.native172.ownerRead=true;c.native172.owner=23;c.native172.collider=24;c.native172.entityId=42;
                    c.native172.part=2;c.native172.body=kcd2vr::npc_contact172::Body::Articulated;}}
            return c;});
        for(unsigned j=0;j<out.commandCount;++j)commands.push_back(out.commands[j]);
    }
    void Rest(unsigned ms){for(unsigned j=0;j<ms/10;++j)Move();}
    unsigned Count(CommandKind kind)const{unsigned n=0;for(const auto& c:commands)n+=c.kind==kind;return n;}
    unsigned Hits()const{return Count(CommandKind::Slash)+Count(CommandKind::Thrust)+Count(CommandKind::Pommel);}
    std::uint64_t First(CommandKind kind)const{for(const auto& c:commands)if(c.kind==kind)return c.tick;return 0;}
    void RearAndHold(unsigned ms){for(unsigned j=0;j<16;++j)Move({0,-.01,0});Rest(ms);}
};
int main(){
    for(bool thrust:{false,true})for(unsigned hold:{300u,1100u,2500u}){
        Replay28 r;if(thrust){r.profiles[0].axis={0,1,0};r.profiles[0].tip={0,.6,0};r.Rest(150);r.commands.clear();}
        r.RearAndHold(hold);
        Check(r.Count(CommandKind::Prepare)==0&&r.Hits()==0&&!r.state.weapon[0].cocked57.active,
            "DIRECT rear cue and settled hold never start/expire the forward attack clock");
        const auto forward=r.in.tick+10;
        for(unsigned j=0;j<24;++j)r.Move(thrust?Vec3{0,.025,0}:Vec3{.025,.012,0});
        const auto kind=thrust?CommandKind::Thrust:CommandKind::Slash;
        Check(r.Count(CommandKind::Prepare)==1&&r.Count(kind)==1&&r.Hits()==1,"long-held windup gives one fresh forward preparation and one cut/stab");
        Check(r.First(CommandKind::Prepare)>=forward&&r.First(CommandKind::Prepare)<=forward+20,
            "native preparation uses the actual qualified forward onset, not the old rear cue");
        Check(r.First(kind)>r.First(CommandKind::Prepare),"free-space forward onset precedes commitment");
        std::uint64_t gesture=0;for(const auto& c:r.commands)if(c.kind==CommandKind::Prepare||c.kind==kind){
            if(!gesture)gesture=c.gesture59c3;Check(gesture==c.gesture59c3&&!c.held57,"one current gesture, no held-native preparation provenance");}
    }
    {Replay28 r(false);r.RearAndHold(1100);
        Check(r.Count(CommandKind::Prepare)==1&&r.state.weapon[0].cocked57.active&&r.Hits()==0,"HYBRID keeps its proven native held preparation");
        bool held=false;for(const auto& c:r.commands)held|=c.kind==CommandKind::Prepare&&c.held57;
        Check(held&&r.Count(CommandKind::Abort)==0,"HYBRID held preparation survives with original provenance");}
    for(unsigned invalid=0;invalid<4;++invalid){Replay28 r;
        if(invalid==0)r.in.allowed=false;if(invalid==1)r.in.tracked[0]=false;if(invalid==2)r.in.grip[0]=0;
        for(unsigned j=0;j<16;++j)r.Move({0,-.01,0},invalid!=3,true);r.Rest(400);
        Check(r.Count(CommandKind::Prepare)==0&&r.Hits()==0,"ineligible or one-family rear movement cannot create an attack");}
    for(bool thrust:{false,true}){Replay28 r;
        if(thrust){r.profiles[0].axis={0,1,0};r.profiles[0].tip={0,.6,0};r.Rest(150);r.commands.clear();}
        r.RearAndHold(1100);const auto forward=r.in.tick+10;
        for(unsigned j=0;j<24;++j)r.Move(thrust?Vec3{0,.025,0}:Vec3{.025,.012,0});
        mo28::selected=true;mo28::attempts.clear();combat_diag_probe::events28.clear();guardHandoffs28=cancelCalls28=0;
        ConsumerFrame28 frame{};ConsumerEquipment28 equipment{};
        for(const auto& c:r.commands)Check(ConsumeDirect28(c,frame,equipment,c.tick+20),"actual native command conversion accepts current DIRECT command");
        Check(combat_diag_probe::events28.size()==2&&combat_diag_probe::events28[0].kind==1&&combat_diag_probe::events28[1].kind==2,
            "actual native command conversion publishes exactly one Prepare and one Commit");
        const auto& p=combat_diag_probe::events28[0];const auto& c=combat_diag_probe::events28[1];
        Check(p.tick==forward&&mo28::same(p.key,c.key)&&p.tick<c.tick&&guardHandoffs28==1&&cancelCalls28==0,
            "physical-to-native handoff preserves fresh forward tick, full key and no ordinary attack request");
        Check(mo28::attempts.beginForKey(p.key,c.tick+20)==forward,"actual mo ledger uses forward onset after a held windup, with no deadline refresh");
        mo28::Attempt hit{};Check(mo28::attempts.claim(c.key,c.tick+10,c.tick+20,42,hit)==mo28::Claim::Ready&&
            mo28::attempts.claim(c.key,c.tick+10,c.tick+20,42,hit)==mo28::Claim::Duplicate,
            "actual once-only contact ledger still permits one adjudication per forward attempt");
        mo28::attempts.clear();combat_diag_probe::events28.clear();
        for(const auto& cmd:r.commands)if(cmd.kind==CommandKind::Prepare){
            Check(ConsumeDirect28(cmd,frame,equipment,cmd.tick+101)&&combat_diag_probe::events28.empty(),
                "stale native command cannot renew a forward onset or publish a threat");break;}
        mo28::selected=false;
    }
    {Replay28 r;r.Rest(2000);Check(!r.Count(CommandKind::Prepare)&&!r.Hits(),"idle shoulder pose is not an attack");}
    for(bool thrust:{false,true}){Replay28 r;r.in.positionGuard176=true;r.in.guardEnterMaxSpeed178r29=2.5;r.in.guardExitSpeed178r45=2.9;
        r.profiles[0].axis=thrust?Vec3{.5,.866025403784,0}:Vec3{1,0,0};r.profiles[0].tip=r.profiles[0].axis*.6;
        r.in.hands[0].p={-.15,.25,1.35};r.in.intentHands[0]=r.in.hands[0];r.Rest(200);r.commands.clear();
        Check(r.out.guard117,"fixture starts in a settled native-compatible weapon guard");const auto forward=r.in.tick+10;
        const auto delta=thrust?r.profiles[0].axis*.022:Vec3{.012,.035,0};
        for(unsigned j=0;j<24;++j)r.Move(delta);
        Check(r.Count(CommandKind::Prepare)==1&&r.Count(thrust?CommandKind::Thrust:CommandKind::Slash)==1,
            "settled guard hands off to one forward cut/stab without a rear preparation");
        Check(r.First(CommandKind::Prepare)>=forward&&r.First(CommandKind::Prepare)<=forward+40,
            "guard handoff preserves early forward input rather than waiting for contact");}
    for(bool thrust:{false,true}){Replay28 r;
        if(thrust){r.profiles[0].axis={0,1,0};r.profiles[0].tip={0,.6,0};r.Rest(150);r.commands.clear();}
        r.RearAndHold(1100);const auto forward=r.in.tick+10;
        const auto delta=thrust?Vec3{0,.025,0}:Vec3{.025,.012,0};
        for(unsigned j=0;j<4;++j)r.Move(delta);
        Check(r.Count(CommandKind::Prepare)==1&&r.First(CommandKind::Prepare)==forward&&r.Hits()==0,
            "qualified forward preparation reaches the consumer before physical enemy contact");
        r.blocked=r.actor=true;const auto contact=r.in.tick+10;r.Move(delta);
        Check(r.Count(thrust?CommandKind::Thrust:CommandKind::Slash)==1&&r.First(CommandKind::Prepare)<contact,
            "first qualified actor contact completes the same forward cut/stab");
        for(unsigned j=0;j<60;++j)r.Move(delta);
        Check(r.Hits()==1&&r.Count(CommandKind::Prepare)==1,"continued contact pressure cannot spend a second attempt");
    }
    {Replay28 r;const auto onset=r.in.tick;for(unsigned j=0;j<3;++j)r.Move({.025,.012,0});
        const auto prepare=r.First(CommandKind::Prepare);r.Rest(150);
        Check(prepare>onset&&r.Count(CommandKind::Prepare)==1&&r.Count(CommandKind::Abort)==1&&r.Hits()==0,
            "a cancelled short forward stroke still cancels once and cannot bank a future hit");}
    #include "postguard_onset32_cases.inl"
    std::printf("direct_forward_onset28=%u/%u actual_physical_step=YES native_outcomes=NOT_EMULATED\n",checks-failures,checks);return failures?1:0;
}
