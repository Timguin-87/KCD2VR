// C49 BLOODY FIGHTS pure parts: which receipts qualify (Henry's applied, undefended, wounding body hits only), the once-per-hit
// gate, weapon -> game blood row, struck material -> BloodMask zone, the extra amount (native 0.3..0.5), the kill FIFO and the
// 4 s / 4 pulse gush scheduler with kill-move upgrades (LETHAL -> IMPALE / DECAP).
#include "vrik/blood49.h"
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cmath>
namespace b=kcd2vr::blood49;
static unsigned checks=0;
static void ck(bool ok,const char* label){++checks;if(!ok){std::printf("FAIL %s\n",label);std::exit(1);}std::printf("PASS %s\n",label);}
static kcd2vr::contact58c1::Receipt hit(std::uint64_t serial){
    kcd2vr::contact58c1::Receipt r{};r.serial=serial;r.tick=1000;r.target=45104;r.targetIdentity=0x1000;r.seen=r.accepted=r.applied=r.fields=r.bodyContact=true;
    r.blocked=r.perfect=r.parry=0;r.damage=12.5f;r.kind=0;r.weaponClass=4;r.bodyMaterial=6;r.nativePart=400;r.energy=.8f;r.death=0;r.point[0]=1;r.point[1]=2;r.point[2]=1.3f;return r;}
int main(){
    // ------------------------------------------------ qualify
    {auto r=hit(1);ck(b::qualify(r,true)==b::Skip::None,"applied undefended wounding body hit qualifies");
     ck(b::qualify(r,false)==b::Skip::Off,"switch off: nothing");
     auto x=r;x.applied=false;ck(b::qualify(x,true)==b::Skip::NotApplied,"not applied by the game: nothing");
     x=r;x.bodyContact=false;ck(b::qualify(x,true)==b::Skip::NotBody,"blade on his weapon/shield: nothing");
     x=r;x.blocked=1;ck(b::qualify(x,true)==b::Skip::Defended,"blocked: nothing");
     x=r;x.parry=1;ck(b::qualify(x,true)==b::Skip::Defended,"parried: nothing");
     x=r;x.damage=0;ck(b::qualify(x,true)==b::Skip::NoDamage,"no damage (armour stopped it): nothing");
     x=r;x.damage=NAN;ck(b::qualify(x,true)==b::Skip::NoDamage,"NaN damage: nothing");
     x=r;x.point[1]=INFINITY;ck(b::qualify(x,true)==b::Skip::BadPoint,"bad contact point: nothing");
     x=r;x.target=0;ck(b::qualify(x,true)==b::Skip::NoReceipt,"no target: nothing");
     x=r;x.targetIdentity=0;ck(b::qualify(x,true)==b::Skip::NoReceipt,"no target identity: nothing");
     const auto h=b::fromReceipt(r);ck(h.serial==1&&h.target==45104&&h.part==400&&h.weaponClass==4&&h.bodyMaterial==6&&h.death==0&&std::fabs(h.point[2]-1.3f)<1e-6,"hit copy");
     x=r;x.nativePart=-1;x.rawPart=379;ck(b::fromReceipt(x).part==379,"raw part when the native part is unknown");
     x=r;x.death=1;ck(b::fromReceipt(x).death==1,"death travels");}
    // ------------------------------------------------ offer -> mailbox
    {b::on.store(true);auto r=hit(7);ck(b::offer(r),"offer publishes a qualified hit");b::Hit h{};ck(b::hits.read(h)&&h.serial==7,"mailbox holds it");
     auto bad=hit(8);bad.blocked=1;ck(!b::offer(bad),"blocked hit is not published");ck(b::hits.read(h)&&h.serial==7,"mailbox still the last good hit");
     b::on.store(false);ck(!b::offer(hit(9)),"switch off: not published");b::on.store(true);}
    // ------------------------------------------------ once per hit
    {b::Once<4> o;ck(o.take(5)&&!o.take(5),"serial acted on once");ck(!o.take(0),"serial 0 never");
     ck(o.take(6)&&o.take(7)&&o.take(8),"other serials pass");ck(!o.take(6),"still remembered within the window");
     ck(o.take(9)&&o.take(5),"oldest forgotten after N new ones (receipt serials never repeat)");}
    // ------------------------------------------------ row / zone / amount
    {ck(!std::strcmp(b::row(4,false),"hit_passed_sword_slash_player")&&!std::strcmp(b::row(4,true),"hit_passed_sword_stab_player"),"longsword rows");
     ck(!std::strcmp(b::row(0,true),"hit_passed_sword_stab_player"),"dagger = blade row");
     ck(!std::strcmp(b::row(3,false),"hit_passed_axe_player")&&!std::strcmp(b::row(5,false),"hit_passed_mace_player")&&!std::strcmp(b::row(6,true),"hit_passed_mace_player")&&!std::strcmp(b::row(7,true),"hit_passed_halberd_player"),"axe/mace/flail/halberd rows");
     ck(!b::row(-1,false)&&!b::row(9,false)&&!b::row(10,true),"fists / bows / unknown: no row, no extra blood");
     ck(!std::strcmp(b::zone(1),"head")&&!std::strcmp(b::zone(38),"head")&&!std::strcmp(b::zone(6),"torso")&&!std::strcmp(b::zone(11),"torso"),"head/neck, torso");
     ck(!std::strcmp(b::zone(12),"arm_right")&&!std::strcmp(b::zone(18),"arm_right")&&!std::strcmp(b::zone(19),"arm_left")&&!std::strcmp(b::zone(25),"arm_left"),"arms");
     ck(!std::strcmp(b::zone(26),"leg_right")&&!std::strcmp(b::zone(31),"leg_right")&&!std::strcmp(b::zone(32),"leg_left")&&!std::strcmp(b::zone(37),"leg_left"),"legs");
     ck(!b::zone(0)&&!b::zone(-1)&&!b::zone(39),"unknown material: no zone");
     ck(std::fabs(b::extraAmount(0)-.3f)<1e-6&&std::fabs(b::extraAmount(1)-.5f)<1e-6&&std::fabs(b::extraAmount(3)-.5f)<1e-6&&std::fabs(b::extraAmount(-1)-.4f)<1e-6&&std::fabs(b::extraAmount(NAN)-.4f)<1e-6,"amount inside the native 0.3..0.5 transfer");}
    // ------------------------------------------------ kill FIFO
    {b::KillQueue<2> q;b::KillEvent e{};const double p[3]{1,2,3},d[3]{0,0,1};
     ck(b::makeKill(b::Kill::Decap,5,0x10,p,d,38,100,e)&&e.kill==b::Kill::Decap&&e.target==5&&e.point[2]==3.f,"kill event");
     ck(!b::makeKill(b::Kill::None,5,0x10,p,d,38,100,e)&&!b::makeKill(b::Kill::Lethal,0,0x10,p,d,38,100,e)&&!b::makeKill(b::Kill::Lethal,5,0,p,d,38,100,e),"no kill / no target / no identity refused");
     const double np[3]{NAN,0,0};ck(!b::makeKill(b::Kill::Lethal,5,0x10,np,d,-1,100,e),"NaN point refused");
     b::KillEvent a{},c{};a.target=1;c.target=2;ck(q.push(a)&&q.push(c)&&!q.push(a)&&q.dropped==1,"FIFO bounded");
     b::KillEvent o{};ck(q.pop(o)&&o.target==1&&q.pop(o)&&o.target==2&&!q.pop(o),"FIFO order (decap raised before the lethal hit in the same frame stays first)");
     b::on.store(false);ck(!b::offerKill(b::Kill::Lethal,5,0x10,p,d,6,100),"switch off: no kill offered");b::on.store(true);
     ck(b::offerKill(b::Kill::Lethal,5,0x10,p,d,6,100)&&b::kills.pop(o)&&o.kill==b::Kill::Lethal,"offer -> FIFO");}
    // ------------------------------------------------ gush scheduler
    {b::Gushes<2> gs;b::Gush in{};in.target=5;in.identity=0x10;in.kill=b::Kill::Lethal;b::Gush* s=nullptr;
     ck(gs.start(in,1000,s)==b::Start::Started&&s&&s->active&&s->start==1000&&s->pulses==0,"lethal kill starts a gush");
     ck(gs.start(in,1100,s)==b::Start::Duplicate&&s->start==1000,"the same kill again: duplicate, schedule kept");
     int fired=0;std::uint64_t last=0;
     for(std::uint64_t t=1000;t<=1000+b::kGushMs+50;t+=10){auto& g=*gs.find(5,0x10);if(b::Gushes<2>::due(g,t)){++g.pulses;++fired;last=t;}}
     ck(fired==b::kPulses,"4 pulses");ck(last==1000+b::kPulseMs[b::kPulses-1],"last pulse at 3.3 s");
     ck(!b::Gushes<2>::done(*gs.find(5,0x10),1000+b::kGushMs-1)&&b::Gushes<2>::done(*gs.find(5,0x10),1000+b::kGushMs),"done after 4 s");
     b::Gushes<2> g2;b::Gush l=in;g2.start(l,1000,s);s->pulses=1;
     b::Gush imp=in;imp.kill=b::Kill::Impale;imp.local[0]=9;
     ck(g2.start(imp,1600,s)==b::Start::Upgraded&&s->kill==b::Kill::Impale&&s->start==1600&&s->pulses==0&&s->local[0]==9,"impale latch upgrades the lethal gush: new wound, 4 s again");
     b::Gush dec=in;dec.kill=b::Kill::Decap;ck(g2.start(dec,1700,s)==b::Start::Upgraded&&s->kill==b::Kill::Decap,"decap is the strongest kill move");
     ck(g2.start(l,1800,s)==b::Start::Duplicate&&s->kill==b::Kill::Decap,"a later lethal receipt never downgrades");
     b::Gush other=in;other.target=6;ck(g2.start(other,1900,s)==b::Start::Started&&s->target==6,"second victim, second slot");
     b::Gush third=in;third.target=7;ck(g2.start(third,2000,s)==b::Start::Started&&s->target==7&&!g2.find(5,0x10),"slots full: the oldest gives way");
     b::Gush same=in;same.target=6;same.identity=0x99;ck(g2.start(same,2100,s)==b::Start::Started&&s->identity==0x99,"same id, other entity (reused id): its own gush");
     b::Gush none=in;none.kill=b::Kill::None;ck(g2.start(none,2200,s)==b::Start::Refused&&!s,"no kill: refused");
     ck(std::strcmp(b::name(b::Kill::Decap),"DECAP")==0&&std::strcmp(b::name(b::Kill::Impale),"IMPALE")==0&&std::strcmp(b::name(b::Kill::Lethal),"LETHAL")==0,"receipt names");}
    {b::PartWound55 p{};p.serial=7;p.target=42;p.identity=0x1234;p.tick=1000;p.epoch=2;p.request=3;
     ck(b::freshPart55(p,42,0x1234,7,1080,2,3),"C55 copied exact wound valid within original80ms");
     ck(!b::freshPart55(p,42,0x1234,7,1081,2,3),"C55 stale wound refuses repeated world fountain");
     ck(!b::freshPart55(p,42,0x1234,8,1000,2,3)&&!b::freshPart55(p,43,0x1234,7,1000,2,3)&&
        !b::freshPart55(p,42,0x1235,7,1000,2,3)&&!b::freshPart55(p,42,0x1234,7,1000,3,3)&&
        !b::freshPart55(p,42,0x1234,7,1000,2,4),"C55 serial/target/entity/epoch/request changes refuse wound");
     p.point[0]=NAN;ck(!b::freshPart55(p,42,0x1234,7,1000,2,3),"C55 invalid wound refuses");}
    std::printf("blood49: %u checks PASS\n",checks);
}
