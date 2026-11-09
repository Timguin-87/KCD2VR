// C32: impale hang + decapitation policy (pure logic of src/vrik/gore32.h)
#include "../src/vrik/gore32.h"
#include <cassert>
#include <cstdio>
using namespace kcd2vr::gore32;
static unsigned n=0;static void ck(bool b){++n;assert(b);}
int main(){
    // head attachment names
    ck(headAttachment("head")&&headAttachment("HEAD")&&headAttachment("BascinetVisorZizka_m01-head")&&headAttachment("BascinetVisorZizka_m01-Visor"));
    ck(headAttachment("hair_03")&&headAttachment("m_beard_12")&&headAttachment("eye_left")&&headAttachment("CoifMail_m01-coif"));
    ck(!headAttachment("body")&&!headAttachment("CoatZizkaSword_m01-torso")&&!headAttachment("GorgetPlate_m01-neck")&&!headAttachment("weapon_bow")&&!headAttachment("")&&!headAttachment(nullptr));
    ck(!headAttachment("headband_belt-torso")&&!headAttachment("hand_left"));
    ck(!headAttachment("kcd2vr_stump_head")&&!headAttachment("KCD2VR_head_part")&&!headAttachment("kcd2vr_rh_head_face"));
    // D73: helmet-only optional equipment; natural head/hair/beard remain.
    ck(detachedHeadPartD73("head","objects/characters/humans/male/head/m_head_034/m_head_034.skin"));
    ck(detachedHeadPartD73("head","objects/characters/humans/female/head/f_head_000/f_head_000.skin"));
    ck(detachedHeadPartD73("beard","objects/characters/humans/male/head/m_head_034/beards/m_beard_08.skin"));
    ck(detachedHeadPartD73("BascinetOpen02_m01-head","objects/characters/humans/male/clothing/head/helmet/bascinet/bascinet_open/bascinetopen02/bascinetopen02.skin"));
    ck(detachedHeadPartD73("BascinetOpen02_m01-head","OBJECTS\\CHARACTERS\\HUMANS\\MALE\\CLOTHING\\HEAD\\HELMET\\BASCINET\\BASCINETOPEN02.SKIN"));
    ck(!detachedHeadPartD73("generic-head","objects/characters/humans/male/clothing/head/coif/coif_mail/coifmail01/coifmail01.skin"));
    ck(!detachedHeadPartD73("Hood09-head","objects/characters/humans/male/clothing/head/hood/hood09/hood09.skin"));
    ck(!detachedHeadPartD73("helmet-neck-head","objects/characters/humans/male/clothing/head/helmet/bascinet/bascinetopen02.skin"));
    ck(detachedHeadPartD73("cap27-head","objects/characters/humans/male/clothing/head/cap/cap27/cap27.skin"));
    ck(!detachedHeadPartD73("unknown-head","objects/characters/humans/male/clothing/head/unknown/part.skin"));
    ck(!detachedHeadPartD73("Cowl-head",nullptr)&&!detachedHeadPartD73("CoifMail_m01-coif",nullptr));
    ck(detachedHeadPartD73("eye_left",nullptr)&&!detachedHeadPartD73("helmet",nullptr));
    ck(!detachedHeadPartD73("kcd2vr_stump_head","objects/characters/humans/male/head/m_head_034/m_head_034.skin"));
    // neck gate (model space, feet at 0)
    ck(neckHit({0,.05,1.55}));ck(neckHit({.1,-.1,1.42}));
    ck(neckHit({.25,0,1.45}));    // C41: top of the shoulder is within +/-15 cm of the neck now (Miki)
    ck(!neckHit({0,.1,1.10}));    // chest
    // C43T death proof: only an npc the game reported killed, same entity identity, within an hour
    NoteDead(7,0x1000,1000);ck(KnownDead(7,0x1000,2000)&&!KnownDead(7,0x2000,2000)&&!KnownDead(8,0x1000,2000)&&!KnownDead(7,0x1000,1000+3600001));
    ck(!KnownDead(0,0x1000,2000)&&!KnownDead(7,0,2000));
    // C46: three decap styles 0 HIDE / 1 LOOSE NECK / 2 STUMP+HEAD, default 2, anything else clamped
    ck(decapStyle44.load()==2&&kDecapStyleCount46==3&&clampDecapStyle46(-5)==0&&clampDecapStyle46(0)==0&&clampDecapStyle46(1)==1&&clampDecapStyle46(2)==2&&clampDecapStyle46(3)==2&&clampDecapStyle46(99)==2);
    for(unsigned i=0;i<16;++i)NoteDead(100+i,0x5000+i,5000);ck(!KnownDead(7,0x1000,6000)&&KnownDead(115,0x500F,6000)); // ring of 16
    // R155: a save load forgets every death (8 Nov 12:57: men killed before the load were cut/stabbed as corpses while fighting); the next kill counts again
    ForgetDead();ck(!KnownDead(115,0x500F,6000)&&!KnownDead(100,0x5000,6000));
    NoteDead(115,0x500F,7000);ck(KnownDead(115,0x500F,7500)); // no latch: the new world's kills fill the table again
    ck(!neckHit({.35,0,1.45}));   // C41: arm / outer shoulder still not
    ck(headMaterial(1)&&headMaterial(5)&&headMaterial(38)&&!headMaterial(6)&&!headMaterial(0)&&!headMaterial(-1)&&!headMaterial(12)); // C41 surfaces
    ck(headAttachment("HoodLinen_m02")&&headAttachment("chaperon_hood_m01")&&headAttachment("CapLeather_m01")&&!headAttachment("CapeWool_m01")&&!headAttachment("CapeWool_m01-torso")); // C41 names
    ck(neckHit({0,0,1.95})&&!neckHit({0,0,2.10})); // C41: a head cut counts, above the head does not
    ck(neckHit({0,0,1.55*1.1},1.1)&&!neckHit({0,0,1.55},.2));
    // hang: follows the blade
    Hang h;h.start(1000,.20);ck(h.active&&h.depth==.20);
    HangIn in{};in.now=1011;in.bladeAxis={0,1,0};in.bladePoint={0,1,1.2};in.wound={0,1,1.0};
    auto o=h.step(in);ck(o.drive&&o.velocity.z>0&&o.release==Release::None);
    // blade moves sideways: body is dragged, not released
    for(int i=0;i<10;++i){in.now+=11;in.bladePoint.x+=.02;in.wound.x+=.015;o=h.step(in);ck(o.drive);}
    // pull straight back along the blade -> slides out after ~depth
    unsigned guard=0;while(o.release==Release::None&&guard<200){in.now+=11;in.bladePoint.y-=.011;o=h.step(in);++guard;}
    ck(o.release==Release::PullOut&&gush(o.release)&&!h.active&&guard<40);
    // Zero-net 2mm axial oscillation must not consume penetration over time.
    {Hang k;k.start(1000,.20);HangIn q{};q.now=1000;q.bladeAxis={0,1,0};q.bladePoint={0,1,1};q.wound=q.bladePoint;
     (void)k.step(q);bool retained=true;
     for(int i=0;i<1000;++i){q.now+=10;q.bladePoint.y=1.002;auto a=k.step(q);
      q.now+=10;q.bladePoint.y=1;auto b=k.step(q);retained=retained&&k.active&&a.release==Release::None&&b.release==Release::None;}
     ck(retained&&k.depth>.195&&k.depth<=k.initialDepth);
     // A subsequent deliberate pull still releases, rather than a long grace.
     HangOut r{};for(int i=0;i<25&&k.active;++i){q.now+=10;q.bladePoint.y-=.01;r=k.step(q);}
     ck(r.release==Release::PullOut&&!k.active);}
    // slow backward drift is not a pull
    h.start(5000,.2);in.now=5011;in.bladePoint={0,1,1};in.wound={0,1,1};o=h.step(in);
    for(int i=0;i<50;++i){in.now+=11;in.bladePoint.y-=.001;in.wound.y-=.001;o=h.step(in);}
    ck(h.active&&o.release==Release::None);
    // hand push
    in.freePalm=true;in.palm={.2,1,1};in.palmVelocity={-1.5,0,0};in.now+=11;o=h.step(in);
    ck(o.release==Release::HandPush&&o.releaseVelocity.x<0);
    // palm moving away does not push
    h.start(9000,.2);in.now=9011;in.palm={.2,1,1};in.palmVelocity={1.5,0,0};o=h.step(in);ck(h.active);
    // kick
    in.freePalm=false;in.kick=true;in.kickDirection={0,1,0};in.now+=11;o=h.step(in);ck(o.release==Release::Kick&&o.releaseVelocity.y>3);
    in.kick=false;
    // stuck: wound stays 0.6 m away for 400 ms
    h.start(20000,.2);in.now=20011;in.bladePoint={0,0,0};in.wound={.6,0,0};o=h.step(in); // sideways: along the blade is no gap
    for(int i=0;i<40&&h.active;++i){in.now+=11;o=h.step(in);}
    ck(o.release==Release::Stuck&&!gush(o.release));
    // timeout
    h.start(30000,.2);in.now=30011;in.wound=in.bladePoint;o=h.step(in);in.now=30000+31000;o=h.step(in);ck(o.release==Release::Timeout);
    // invalid input / time reversal
    h.start(40000,.2);in.now=39000;o=h.step(in);ck(o.release==Release::Lost);
    // review case: the body really follows the drive (tracking 50..100 %), pulled back at 0.5 / 1 / 2 / 4 m/s
    for(double track:{.5,1.0})for(double speed:{.5,1.0,2.0,4.0}){
        Hang k;k.start(50000,.20);HangIn q{};q.now=50000;q.bladeAxis={0,1,0};q.bladePoint={0,1,1};q.wound={0,1,1};
        HangOut r{};unsigned steps=0;
        while(k.active&&steps<400){q.now+=11;q.bladePoint.y-=speed*.011;r=k.step(q);if(r.drive)q.wound=q.wound+r.velocity*(.011*track);++steps;}
        ck(r.release==Release::PullOut);
    }
    // sideways carry with a following body is never a release
    {Hang k;k.start(60000,.2);HangIn q{};q.now=60000;q.bladeAxis={0,1,0};q.bladePoint={0,1,1};q.wound={0,1,1};HangOut r{};
     for(int i=0;i<200;++i){q.now+=11;q.bladePoint.x+=.011;r=k.step(q);if(r.drive)q.wound=q.wound+r.velocity*.011;}
     ck(k.active&&r.release==Release::None);}
    // weight (C44: a quarter of VrImpaleHangMassKg at the stuck point, 2..20 kg, at most 8 x the weapon)
    hangMassKg=40.f;ck(hangLoadKg(1.5,true)==10.0&&hangLoadKg(1.5,false)==0&&std::fabs(hangLoadKg(.3,true)-2.4)<1e-9);
    hangMassKg=80.f;ck(hangLoadKg(3,true)==20.0&&hangLoadKg(1.5,true)==12.0);
    hangMassKg=20.f;ck(hangLoadKg(1.5,true)==5.0);
    hangMassKg=500.f;ck(hangLoadKg(3,true)==20.0);                       // slider range clamped
    ck(clampHangMass44(5)==20.&&clampHangMass44(100)==80.&&clampHangMass44(std::nan(""))==40.);
    // C44 sag of the weapon target: 40 kg at the 180 N design profile = 6 cm drop, 0.175 rad droop; a stronger arm sags less
    {const auto s=hangSag(40,180);ck(std::fabs(s.drop-.06)<1e-9&&std::fabs(s.droop-.175)<1e-9&&std::fabs(s.pullN-40*9.81*.35)<1e-6);}
    {const auto s=hangSag(80,180);ck(std::fabs(s.drop-.12)<1e-9&&std::fabs(s.droop-.35)<1e-9);}
    {const auto a=hangSag(40,360),b=hangSag(40,90);ck(std::fabs(a.drop-.03)<1e-9&&std::fabs(b.drop-.09)<1e-9);} // 180/force clamped .5..1.5
    {const auto s=hangSag(40,std::nan(""));ck(std::fabs(s.drop-.06)<1e-9);}                                     // unknown authority = design profile
    {double r=0;for(int i=0;i<40;++i)r=sagRamp(r,true,.011);ck(r>.99);r=sagRamp(r,true,.011);ck(r==1.);
     for(int i=0;i<40;++i)r=sagRamp(r,false,.011);ck(r<.01);r=sagRamp(r,false,.011);ck(r==0.);
     ck(sagRamp(0,true,std::nan(""))==0.&&std::fabs(sagRamp(0,true,5)-1./3)<1e-9);}                                                   // bad dt = no step; dt capped 100 ms
    {namespace g=kcd2vr::gore32; // C49: C48 log cases
     ck(g::decapZone47(false,6,false,{-0.14,0.04,1.30})==g::DecapBy47::None);   // 01:39:24 chest kill, no head
     ck(g::decapZone47(false,6,false,{0.02,0.21,1.28})==g::DecapBy47::None);    // 01:39:59 chest kill
     ck(g::decapZone47(false,2,false,{0.09,0.19,1.48})==g::DecapBy47::Material);// 01:40:05 the real head kill
     ck(g::decapZone47(false,0,false,{-0.18,0.30,1.41})==g::DecapBy47::None);   // H18: C46 00:21:34 material0 head cut is a miss with the band opt-in OFF (needs a joint/head-surface witness)
     g::decapAllowBand.store(true);
     ck(g::decapZone47(false,0,false,{-0.18,0.30,1.41})==g::DecapBy47::Band);   // H18 opt-in restores the legacy band for the same hit
     g::decapAllowBand.store(false);
     ck(g::decapZone47(false,6,true,{0,0,1.0})==g::DecapBy47::Joint);}          // a head/neck joint always counts
    {namespace g=kcd2vr::gore32;ck(g::limbCorpseMinSpeed.load()==5.0f);}           // C60: strong-cut receipt threshold seed (INI VrLimbCorpseMinSpeed overrides)
    {namespace g=kcd2vr::gore32; // C60 budget: saturating, rearmed per capture session
     g::ReceiptBudget60 b{};unsigned n=0;for(unsigned i=0;i<200;++i)if(b.admit(1))++n;
     ck(n==64&&!b.admit(1));                                                     // saturates at 64 in session 1
     ck(b.admit(2)&&b.count==1);                                                  // new session rearms
     g::ReceiptBudget60 b2{};b2.cap=3;ck(b2.admit(9)&&b2.admit(9)&&b2.admit(9)&&!b2.admit(9));}
    {namespace g=kcd2vr::gore32;using C=kcd2vr::gorepak50::Cut;
     g::LimbCutLedger60 l{};const auto t=1000ull;
     // Use actual production enum values: Head=1 is NOT a limb, RightFoot=5 IS.
     for(C c:{C::LeftHand,C::RightHand,C::LeftFoot,C::RightFoot}){
      const int k=int(c);ck(l.available(5,0xA,t,k));
      ck(l.available(5,0xA,t+1,k)); // pin/attachment/queue failures consume nothing
      ck(l.admit(5,0xA,t+2,k)&&!l.available(5,0xA,t+500,k));
      ck(!l.admit(5,0xA,t+2002,k)&&l.admit(5,0xA,t+2003,k)); // exact original deadline
     }
     ck(!l.available(5,0xA,t,int(C::Head))&&!l.admit(5,0xA,t,int(C::Head)));
     ck(!l.available(5,0xA,t,int(C::None))&&!l.admit(5,0xA,t,6));
     ck(l.admit(6,0xB,t,int(C::RightFoot))); // other target independent
     ck(l.admit(5,0xB,t,int(C::LeftHand))); // same ID, replaced entity independent
     ck(!l.admit(0,0xA,t,int(C::LeftHand))&&!l.admit(5,0,t,int(C::LeftHand)));
     ck(!l.admit(8,0xC,0,int(C::LeftHand)));
     ck(l.admit(9,0xD,t,int(C::LeftFoot))&&!l.available(9,0xD,t-1,int(C::LeftFoot))); // clock regression refuses
    }
    // DS-F23 material classifier: race-0 human mapping, fail closed
    ck(LimbFromMaterial60(16)==kcd2vr::gorepak50::Cut::RightHand&&LimbFromMaterial60(18)==kcd2vr::gorepak50::Cut::RightHand);
    ck(LimbFromMaterial60(23)==kcd2vr::gorepak50::Cut::LeftHand&&LimbFromMaterial60(25)==kcd2vr::gorepak50::Cut::LeftHand);
    ck(LimbFromMaterial60(28)==kcd2vr::gorepak50::Cut::RightFoot&&LimbFromMaterial60(31)==kcd2vr::gorepak50::Cut::RightFoot);
    ck(LimbFromMaterial60(34)==kcd2vr::gorepak50::Cut::LeftFoot&&LimbFromMaterial60(37)==kcd2vr::gorepak50::Cut::LeftFoot);
    ck(LimbFromMaterial60(0)==kcd2vr::gorepak50::Cut::None&&LimbFromMaterial60(-1)==kcd2vr::gorepak50::Cut::None&&LimbFromMaterial60(12)==kcd2vr::gorepak50::Cut::None); // shoulder/unknown: never a limb
    // DS-F36 human-path matchers: EXACT canonical prefixes, fail closed
    ck(humanRacePath61("objects/characters/humans/male/body/male_body.skin"));
    ck(humanRacePath61("characters/humans/female/head/female_head.skin")); // non-objects-rooted variant
    ck(!humanRacePath61("objects/characters/horses/humans/male/body/foo.skin")); // horses prefix refused
    ck(!humanRacePath61("xobjects/characters/humans/male/body.skin")); // arbitrary segment before the family
    ck(!humanRacePath61("objects/characters/humans/malebody.skin"));
    ck(humanBodyPath61("objects/characters/humans/male/body/male_body.skin"));
    ck(humanBodyPath61("characters/humans/female/body/female_body.skin"));
    ck(!humanBodyPath61("objects/characters/humans/male/head/male_head.skin")); // race ok, not a body skin
    ck(!humanBodyPath61("objects/characters/humans/male/bodyguard_arm.skin")); // '/body' must be a bounded segment
    ck(!humanBodyPath61("objects/characters/horses/humans/male/body/foo.skin")); // horses prefix refused
    std::printf("PASS %u gore32 checks\n",n);
}
