// C37 VR leg KICK TRANSACTION: one R3 click -> one serial -> at most one contact -> at most one application.
// Pure ledger/effort logic (no native call, no WHGame image). The game-thread drain is kick_drain37.inl.
#include "../src/vrik/kick_hit34.h"
#include <cmath>
#include <cstdio>
namespace k=kcd2vr::kick34;
static int failures=0,checks=0;
#define CHECK(c,msg) do{++checks;if(!(c)){++failures;std::printf("FAIL %s\n",msg);}else std::printf("PASS %s\n",msg);}while(0)
static constexpr std::uintptr_t kHenry=0x1000,kState=0x2000,kPlayer=0x3000;
static k::Binding bind(std::uint64_t serial,std::uint64_t tick,std::uint64_t epoch=7){k::Binding b{};b.serial=serial;b.tick=tick;b.poseEpoch=epoch;b.actor=kHenry;b.state=kState;b.player=kPlayer;return b;}
static k::Contact foot(std::uint64_t serial,std::uint64_t tick,std::uint64_t epoch=7,bool extend=true,bool living=true,std::uint32_t entity=55){
    k::Contact c{};c.serial=serial;c.tick=tick;c.poseEpoch=epoch;c.entity=entity;c.part=3;c.body=2;c.bodyMaterial=8;c.extend=extend;c.living=living;return c;}
int main(){
    k::Ledger l;
    // one click -> one contact -> one claim -> one application
    CHECK(l.open(bind(5,1000)),"R3 click opens transaction 5 bound to Henry/epoch");
    CHECK(l.wants(5)&&!l.wants(4),"pose fast path wants only this click's kick");
    CHECK(l.offer(foot(5,1120,7,false),1121)==k::Why::NotExtend&&l.wants(5),"knee coming up is not the kick and does not consume the click");
    CHECK(l.offer(foot(5,1130,7,true,false),1131)==k::Why::NotBody&&l.wants(5),"a wall/prop touch is not a body and does not consume the click");
    CHECK(l.offer(foot(5,1200),1201)==k::Why::None&&!l.wants(5),"first foot body contact during EXTEND is taken");
    CHECK(l.offer(foot(5,1210),1211)==k::Why::Duplicate,"sustained intersection: second contact of the same click refused");
    auto c=l.claim(kHenry,kState,1250);
    CHECK(c.ready&&c.contact.entity==55&&c.contact.bodyMaterial==8&&c.binding.serial==5,"game thread claims the exact contact once");
    CHECK(!l.claim(kHenry,kState,1251).ready,"no second claim of the same click");
    CHECK(l.finish(5,k::Why::Applied)&&l.stage()==k::Stage::Applied&&!l.finish(5,k::Why::Applied),"one application, finished once");
    CHECK(l.offer(foot(5,1300),1301)==k::Why::Duplicate,"a contact after the application is refused");
    std::uint64_t seen=0;k::Note n{};
    CHECK(l.takeNote(seen,n)&&n.serial==5&&n.why==k::Why::Applied&&!l.takeNote(seen,n),"outcome edge is read once");
    // replay / monotonic serial
    CHECK(!l.open(bind(5,1400))&&!l.open(bind(4,1400)),"an old or replayed click never reopens");
    CHECK(!l.open(bind(6,1400,0))&&!l.open(k::Binding{}),"no pose epoch / no Henry -> no transaction");
    // expiry
    CHECK(l.open(bind(6,2000)),"next click");
    CHECK(l.offer(foot(6,2000+k::kExpiryMs+1),2000+k::kExpiryMs+1)==k::Why::Expired&&l.stage()==k::Stage::Expired,"contact after the bounded click window expires the click");
    CHECK(!l.claim(kHenry,kState,2000+k::kExpiryMs+2).ready,"expired click cannot be claimed");
    CHECK(l.open(bind(7,3000)),"click 7");
    CHECK(l.offer(foot(7,3100),3100)==k::Why::None,"contact inside the window");
    c=l.claim(kHenry,kState,3000+k::kExpiryMs+1);CHECK(!c.ready&&c.why==k::Why::Expired,"claim after the bounded click window is refused as expired");
    CHECK(l.open(bind(8,4000)),"click 8");l.sweep(4500);CHECK(l.stage()==k::Stage::Open,"sweep leaves a live click alone");
    l.sweep(4000+k::kExpiryMs+1);CHECK(l.stage()==k::Stage::Expired&&l.takeNote(seen,n)&&n.why==k::Why::Expired,"a click with no contact closes once as EXPIRED");
    // stale contact
    CHECK(l.open(bind(9,5000)),"click 9");CHECK(l.offer(foot(9,5100),5100)==k::Why::None,"contact taken");
    c=l.claim(kHenry,kState,5261);CHECK(!c.ready&&c.why==k::Why::ContactStale,"contact older than 160 ms at the game thread is refused");
    // epoch / request change
    CHECK(l.open(bind(10,6000)),"click 10");
    CHECK(l.offer(foot(10,6100,8),6100)==k::Why::EpochChanged&&l.stage()==k::Stage::Refused,"contact from a newer pose epoch refuses the click");
    CHECK(l.open(bind(11,6200)),"click 11");l.retire(k::Why::EpochChanged);
    CHECK(l.stage()==k::Stage::Refused&&l.offer(foot(11,6300),6300)==k::Why::Duplicate,"recenter/body replacement retires an open click");
    CHECK(l.open(bind(12,7000)),"click 12");CHECK(l.open(bind(13,7050)),"a new click before contact");
    CHECK(l.offer(foot(12,7100),7100)==k::Why::SerialMismatch,"the old click's foot contact cannot feed the new transaction");
    CHECK(l.offer(foot(13,7110),7110)==k::Why::None,"the new click takes its own contact");
    // non-Henry
    c=l.claim(0x9999,kState,7120);CHECK(!c.ready&&c.why==k::Why::NotHenry,"another actor never claims Henry's kick");
    CHECK(l.open(bind(14,8000)),"click 14");CHECK(l.offer(foot(14,8100),8100)==k::Why::None,"contact");
    c=l.claim(kHenry,0x7777,8110);CHECK(!c.ready&&c.why==k::Why::HenryStateChanged,"replaced combat state refuses");
    CHECK(!l.finish(14,k::Why::Applied),"a refused click cannot be finished as applied");
    // game-thread refusal (no stamina) after a claim
    CHECK(l.open(bind(15,9000)),"click 15");CHECK(l.offer(foot(15,9100),9100)==k::Why::None,"contact");
    c=l.claim(kHenry,kState,9110);CHECK(c.ready,"claimed");
    CHECK(l.finish(15,k::Why::NoStamina)&&l.stage()==k::Stage::Refused&&l.takeNote(seen,n)&&n.why==k::Why::NoStamina,"no stamina refuses the click once");
    const auto counts=l.counters();CHECK(counts.applied==1&&counts.claimed==2,"counters");
    // native effort rules
    CHECK(!k::CanAttack(0.f,31.2f)&&k::CanAttack(.1f,31.2f)&&k::CanAttack(0.f,0.f),"686F40: refuses only at zero stamina (ratio 0.0)");
    CHECK(!k::CanAttack(std::nanf(""),31.2f),"unknown stamina refuses");
    CHECK(k::AttackStamMod(40.f,31.2f)==1.f&&std::fabs(k::AttackStamMod(7.8f,31.2f)-.75f)<1e-5f,"4C7484: full above the cost, 0.5+0.5*sqrt below");
    // own raise effect: exactly one, exactly the native kick cost
    {float d=-7.02f;CHECK(!k::scaleKickEffect(77,0xC5BBEB,1,d)&&d==-7.02f,"no scope: native effect unchanged");
     k::KickRaiseScope s(15,77,31.2f);
     float wrong=-7.02f;CHECK(!k::scaleKickEffect(77,0xC64DCA,2,wrong)&&!k::scaleKickEffect(78,0xC5BBEB,1,wrong)&&!k::scaleKickEffect(77,0xC5BBEB,2,wrong),"other call site / soul / flag untouched");
     float pos=1.f;CHECK(!k::scaleKickEffect(77,0xC5BBEB,1,pos),"a gain is never scaled");
     CHECK(k::scaleKickEffect(77,0xC5BBEB,1,d)&&d==-31.2f&&s.value.entered&&s.value.nativeDelta==-7.02f,"our raise effect = -native type-4 cost");
     float again=-7.02f;CHECK(!k::scaleKickEffect(77,0xC5BBEB,1,again)&&again==-7.02f,"one effect per scope");}
    CHECK(k::kickRaise==nullptr,"scope restored");
    // repeat-target guard
    {k::RepeatGuard g{};g.note(55,1000);CHECK(g.blocked(55,2499)&&!g.blocked(55,2500)&&!g.blocked(56,1200),"D53610 rule: same target within 1.5 s refused");}
    {bool ray=false;CHECK(k::LocationMaterial(8,ray)==8&&ray&&k::LocationMaterial(-1,ray)==6&&!ray,"ray idmatOrg first, zone-5 approximation labelled otherwise");}
    std::printf("kick_tx37 %d/%d\n",checks-failures,checks);
    return failures?1:0;
}
