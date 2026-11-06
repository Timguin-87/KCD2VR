// CB1 physical combo finisher: table join + verdicts (pure header, no native).
#include "../src/subsystems/masterstrike_combos/physical_finisher_cb1.h"
#include <cstdio>
#include <set>
#include <string>
namespace cb=kcd2vr::finisher_cb1;using R=kcd2vr::bodycombo59c7g::Region;
static int checks=0,fails=0;static void check(bool b,const char* m){++checks;fails+=!b;std::printf("%s %s\n",b?"PASS":"FAIL",m);}
static void bytes(const char* hex,std::uint8_t out[16]){for(int i=0;i<16;++i)out[i]=std::uint8_t(cb::hex(hex[2*i])*16+cb::hex(hex[2*i+1]));}
static const cb::Row* by(const char* n){for(const auto& r:cb::rows)if(std::string(r.name)==n)return &r;return nullptr;}
int main(){
 bool all=true;std::set<std::string> uniq;
 for(const auto& r:cb::rows){std::uint8_t g[16]{};all=all&&cb::parseGuid(r.technique,g)&&cb::findRow(g)==&r;uniq.insert(r.technique);}
 check(all&&cb::rows.size()==51&&uniq.size()==51,"all 51 finisher GUIDs parse, are unique and join back to their row");
 std::uint8_t n[16]{};
 // native recipe+0x28 bytes copied from Miki's 24 Oct headset logs (V66.91 COMBO_CANDIDATE techniqueBytesHex)
 bytes("04886FA0612A66A41829FD0E8B1E4F9E",n);check(cb::findRow(n)==by("Longsword_C07"),"native C07 bytes == table Longsword_C07 (MS GUID byte order)");
 bytes("94EE27FDD309A9362093E7845B498F58",n);check(cb::findRow(n)==by("Longsword_C02"),"native C02 bytes == table Longsword_C02");
 bytes("8E06CA10CD5931518B727AA2E115609C",n);check(cb::findRow(n)==by("Longsword_C06"),"native C06 bytes == table Longsword_C06");
 n[15]^=1;check(cb::findRow(n)==nullptr,"one flipped byte joins nothing");
 const auto* c12=by("Longsword_C12"); // Face / Pommel / BluntStrike
 check(cb::judge(c12,cb::Kind::Pommel,true,1.62)==cb::Verdict::Ok,"C12 pommel to the face -> finisher");
 check(cb::judge(c12,cb::Kind::Cut,true,1.62)==cb::Verdict::WrongMotion,"C12 cut to the face -> refused (needs pommel)");
 check(cb::judge(c12,cb::Kind::Pommel,true,1.00)==cb::Verdict::WrongRegion,"C12 pommel to the belly -> refused");
 check(cb::judge(c12,cb::Kind::Pommel,false,0)==cb::Verdict::WrongRegion,"C12 unknown blade height -> refused, never guessed");
 check(cb::judge(c12,cb::Kind::None,true,1.6)==cb::Verdict::NoRelease,"no VR strike in flight -> native rule (flat input)");
 // D62: physical labels describe the striking part. A swung mace head is
 // not a pommel, even though both effects are blunt.
 for(const char* name:{"Blunt_C09","BluntShield_C09"}){
  const auto* head=by(name);
  check(cb::judge(head,cb::Kind::Cut,true,1.62)==cb::Verdict::Ok,"C09 swung weapon head can satisfy face blunt-strike finisher");
  check(cb::judge(head,cb::Kind::Pommel,true,1.62)==cb::Verdict::WrongMotion,"C09 handle/pommel cannot impersonate weapon-head blow");
  check(cb::judge(head,cb::Kind::Thrust,true,1.62)==cb::Verdict::WrongMotion,"C09 thrust cannot impersonate swung weapon-head blow");
  check(cb::judge(head,cb::Kind::Cut,true,1.0)==cb::Verdict::WrongRegion,"C09 weapon-head correction keeps face-region demand");
  check(cb::judge(head,cb::Kind::Cut,false,0)==cb::Verdict::WrongRegion,"C09 unknown region still refuses extra physical match");
 }
 const auto* c09=by("Longsword_C09"); // Abdomen / Blade / Cut
 check(cb::judge(c09,cb::Kind::Cut,true,1.0)==cb::Verdict::Ok&&cb::judge(c09,cb::Kind::Cut,true,1.6)==cb::Verdict::WrongRegion&&
       cb::judge(c09,cb::Kind::Thrust,true,1.0)==cb::Verdict::WrongMotion,"C09 abdomen CUT only");
 const auto* hal7=by("Halberd_C07"); // Leg / WeaponHead / Trip
 check(cb::judge(hal7,cb::Kind::Cut,true,.45)==cb::Verdict::Ok&&cb::judge(hal7,cb::Kind::Cut,true,1.3)==cb::Verdict::WrongRegion,"halberd C07 trip = low cut that lands on the leg");
 const auto* c01=by("Longsword_C01"); // Any / Blade / Cut
 check(cb::judge(c01,cb::Kind::Cut,false,0)==cb::Verdict::Ok&&cb::judge(c01,cb::Kind::Thrust,false,0)==cb::Verdict::WrongMotion,"Any-region finisher needs only its motion, no height");
 check(cb::judge(by("BluntShield_C16"),cb::Kind::Cut,true,1.3)==cb::Verdict::Unsupported,"shield bash finisher: no VR shield bash -> refused with reason");
 check(cb::judge(by("Unarmed_C02"),cb::Kind::None,true,1.6)==cb::Verdict::NativeOnly,"fist finisher keeps the native rule");
 check(cb::judge(nullptr,cb::Kind::Cut,true,1)==cb::Verdict::NativeOnly,"unknown recipe keeps the native rule");
 check(cb::regionOk(R::Knee,.5)&&!cb::regionOk(R::Knee,1.)&&cb::regionOk(R::Face,1.6)&&!cb::regionOk(R::Face,1.2)&&
       cb::regionOk(R::Chest,1.3)&&cb::regionOk(R::Neck,1.45)&&!cb::regionOk(R::Abdomen,1.4),"height bands");
 cb::release.store(0);check(cb::currentRelease(1000)==cb::Kind::None,"no release published");
 cb::publishRelease(cb::Kind::Thrust,1000);check(cb::currentRelease(1200)==cb::Kind::Thrust&&cb::currentRelease(1300)==cb::Kind::None&&
       cb::currentRelease(999)==cb::Kind::None,"release kind valid 250 ms, never from the future");
 char w[64]{};cb::want(c12,w,sizeof(w));check(std::string(w)=="POMMEL BLUNT_STRIKE to FACE","visible want text");
 // CB4: region judged on the stroke line to the opponent's body, not a fixed time ahead (25 Oct C06 knee case).
 {const auto* c06=by("Longsword_C06"); // knee cut
  const double p0[3]{1.0,0.0,1.40},q0[3]{1.0,0.0,1.50}; // tip 1.4 m up, 1 m from him, moving straight down 10 cm/frame
  check(cb::ContactHeightCB4(p0,q0,0,0,0)==1.40,"stroke going straight down beside him never reaches his body: release height");
  const double p1[3]{0.7,0.0,1.39},q1[3]{0.8,0.0,1.49}; // diagonal downward cut towards him
  const double hc=cb::ContactHeightCB4(p1,q1,0,0,0);check(hc>.9&&hc<1.0,"diagonal cut meets his body at 0.30 m: contact height ~0.99 m");
  const double p2[3]{0.7,0.0,0.95},q2[3]{0.8,0.0,1.05}; // steep downward cut
  const double hk=cb::ContactHeightCB4(p2,q2,0,0,0);check(hk>.5&&hk<.6,"steeper cut meets him at the knee (~0.55 m)");
  check(cb::judgePath(c06,cb::Kind::Cut,true,0.95,hk)==cb::Verdict::Ok,"C06 knee finisher: wind-up 0.95 m, stroke reaches the knee band -> OK");
  check(cb::judgePath(c06,cb::Kind::Cut,true,1.40,1.40)==cb::Verdict::WrongRegion,"C06: stroke that stays high (log 1.39/1.40) stays refused");
  const double p3[3]{2.5,0.0,0.5},q3[3]{2.6,0.0,1.5};check(cb::ContactHeightCB4(p3,q3,0,0,0)==0.5,"body farther than one sweep (1 m): undecided -> release height");
  const double p4[3]{0.2,0.0,1.0},q4[3]{0.3,0.0,1.1};check(cb::ContactHeightCB4(p4,q4,0,0,0)==1.0,"already inside his body radius: release height");
  check(cb::enabled.load(),"VrPhysicalComboFinisher default ON");}
 std::printf("%d/%d\n",checks-fails,checks);return fails?1:0;
}
