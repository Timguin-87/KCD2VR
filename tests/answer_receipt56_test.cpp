// C56 review (Claude): the native-answer ledger carries the struck part / surface material / producer
// of an applied native answer as RECEIPT ONLY. A wood/shield clash (material 144) is a completed native
// block when the game said so; the ledger must still answer Same for it (no material filter), and the
// receipt fields must travel with the exact match so the VR_HIT_ANSWERED_BY_NPC line can show what the
// NPC's answer physically was. Unknown/alias answers carry no part/material.
#include "../src/vrik/native_answer59c4r2.h"
#include <cassert>
#include <cstdio>
using namespace kcd2vr;
int main(){
    unsigned checks=0;auto ck=[&](bool ok,const char* what){++checks;std::printf("%s %s\n",ok?"PASS":"FAIL",what);assert(ok);};
    answer59c4r2::Ledger ledger;
    contact176::Ticket release{};release.player=0x1000;release.entity=0x2000;release.serial=81;release.tick=1000;release.sample=258850;
    release.epoch=9;release.request=21;release.key=0x18E23;release.generation=1;release.policy=1;release.gesture59c3=258844;release.recovery59c3=3;
    answer59c4r2::Record r{};r.attacker=0xA;r.target=0xB;r.tick=1031;r.kind=3;r.effect=answer59c4r2::Effect::Application;
    r.attackerId=30583;r.targetId=45104;r.lineage.action=0x5555;r.lineage.release=release;
    r.part=403;r.material=144;r.callerRva=0xD536BCull; // the C51R1 clang: sword on the NPC's wooden shield, perfect block
    ledger.note(r);
    auto q=ledger.query(0xA,0xB,release,1060);
    ck(q.match==answer59c4r2::Match::Same&&q.kind==3,"a material-144 clash with exact lineage is still an exact SAME perfect-block answer (no material filter)");
    ck(q.part==403&&q.material==144&&q.callerRva==0xD536BCull&&q.tick==1031,"receipt fields travel with the exact match");
    // Same physical gesture, different dispatch metadata = ambiguous alias: Unknown, and no receipt fields leak.
    auto alias=release;alias.serial=82;alias.sample=258860;
    auto u=ledger.query(0xA,0xB,alias,1060);
    ck(u.match==answer59c4r2::Match::Unknown&&u.part==-1&&u.material==-1&&u.callerRva==0,"alias of the same motion stays Unknown without receipt fields");
    // Another target: no answer at all for this contact.
    auto n=ledger.query(0xA,0xC,release,1060);
    ck(n.match==answer59c4r2::Match::None,"another target is not answered by this record");
    // Default-constructed Result/Record keep the old two-field brace initialisation valid.
    answer59c4r2::Result two{answer59c4r2::Match::Different,1};
    ck(two.part==-1&&two.material==-1&&two.callerRva==0&&two.tick==0,"appended receipt fields default");
    std::printf("answer_receipt56: %u checks\n",checks);
    return 0;
}
