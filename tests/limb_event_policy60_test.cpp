// C60 limb event policy: fail-closed dismember dispatch (pure logic of src/subsystems/gore/limb_event_policy60.h)
#include "../src/subsystems/gore/limb_event_policy60.h"
#include <cassert>
#include <cmath>
#include <cstdio>
#include <limits>
#include <string>
using namespace kcd2vr::gore32;
namespace gp=kcd2vr::gorepak50;
static unsigned n=0;static void ck(bool b){++n;assert(b);}
// A candidate that passes every receipt, minus whatever the test overrides.
static CutCandidate ok(uint64_t now=1000){CutCandidate c{};c.target=45104;c.identity=0x1000;c.tick=now;
    c.corpse=true;c.nativeLethal42=true;c.bodyMaterial=16;c.part44=380;c.rawPart47=380;return c;}
int main(){
    gp::dismember.store(true);limbCorpseMinSpeed.store(5.0f);
    // the fully qualified receipt produces the event request (kind + limb id)
    {LimbCutLedger60 l{};ReceiptBudget60 b{};const auto d=limbEvent60(ok(),6.5,gp::Cut::None,1000,1,l,b);
     ck(d.fire()&&d.request.kind==gp::Cut::RightHand&&d.request.limbId==380&&d.refusal==LimbEventRefusal60::None);
     ck(d.request.kind!=gp::Cut::Head);}
    // the struck part id: rawPart47 first, part44 fallback (the native receipt lane's own choice)
    {LimbCutLedger60 l{};ReceiptBudget60 b{};auto c=ok();c.part44=-1;c.rawPart47=381;
     ck(limbEvent60(c,6.,gp::Cut::None,1000,1,l,b).request.limbId==381);
     c.rawPart47=-1;c.part44=382;ck(limbEvent60(c,6.,gp::Cut::None,1000,1,l,b).request.limbId==382);}
    // every limb material kind fires as itself
    {LimbCutLedger60 l{};ReceiptBudget60 b{};
     const struct{int m;gp::Cut k;}t[]{{16,gp::Cut::RightHand},{17,gp::Cut::RightHand},{18,gp::Cut::RightHand},
      {23,gp::Cut::LeftHand},{24,gp::Cut::LeftHand},{25,gp::Cut::LeftHand},
      {28,gp::Cut::RightFoot},{29,gp::Cut::RightFoot},{30,gp::Cut::RightFoot},{31,gp::Cut::RightFoot},
      {34,gp::Cut::LeftFoot},{35,gp::Cut::LeftFoot},{36,gp::Cut::LeftFoot},{37,gp::Cut::LeftFoot}};
     for(const auto&x:t){auto c=ok();c.bodyMaterial=x.m;ck(limbEvent60(c,6.,gp::Cut::None,1000,1,l,b).request.kind==x.k);}}
    // a living actor is NEVER dismembered: corpse flag missing, even with a lethal receipt
    {LimbCutLedger60 l{};ReceiptBudget60 b{};
     auto c=ok();c.corpse=false;c.nativeLethal42=true;
     ck(limbEvent60(c,6.,gp::Cut::None,1000,1,l,b).refusal==LimbEventRefusal60::Living);
     c=ok();c.corpse=true;c.nativeLethal42=false; // no death proof at all
     ck(limbEvent60(c,6.,gp::Cut::None,1000,1,l,b).refusal==LimbEventRefusal60::Living);
     c=ok();c.nativeLethal42=false;c.nonLethalLimb60=true; // the living-limb receipt lane is a living actor
     ck(limbEvent60(c,6.,gp::Cut::None,1000,1,l,b).refusal==LimbEventRefusal60::Living);}
    // dead-by-game proof stands in for the lethal receipt, same entity identity, within an hour
    {LimbCutLedger60 l{};ReceiptBudget60 b{};NoteDead(9,0xB,500);
     auto c=ok(1000);c.target=9;c.identity=0xB;c.nativeLethal42=false;
     ck(limbEvent60(c,6.,gp::Cut::None,1000,1,l,b).fire());           // game reported him killed
     c.identity=0xC;                                                  // ... but a different entity now
     ck(limbEvent60(c,6.,gp::Cut::None,1000,1,l,b).refusal==LimbEventRefusal60::Living);
     c.identity=0xB;ck(limbEvent60(c,6.,gp::Cut::None,1000+3600001,1,l,b).refusal==LimbEventRefusal60::Living);} // older than an hour
    // no material limb witness = refused (unknown/0/-1 and non-limb body surfaces)
    {LimbCutLedger60 l{};ReceiptBudget60 b{};for(const int m:{0,-1,6,2,38,12}){
      auto c=ok();c.bodyMaterial=m;ck(limbEvent60(c,6.,gp::Cut::None,1000,1,l,b).refusal==LimbEventRefusal60::NoMaterial);}}
    // two disagreeing witnesses fail closed; an unresolved joint witness leaves the material kind alone
    {LimbCutLedger60 l{};ReceiptBudget60 b{};
     ck(limbEvent60(ok(),6.,gp::Cut::LeftHand,1000,1,l,b).refusal==LimbEventRefusal60::Mismatch); // material says RIGHT hand
     ck(limbEvent60(ok(),6.,gp::Cut::RightHand,1000,1,l,b).fire());
     ck(limbEvent60(ok(),6.,gp::Cut::None,1000,1,l,b).fire());}
    // the min blade speed gate (equal to the threshold counts)
    {LimbCutLedger60 l{};ReceiptBudget60 b{};
     ck(limbEvent60(ok(),4.999,gp::Cut::None,1000,1,l,b).refusal==LimbEventRefusal60::NoSpeed);
     ck(limbEvent60(ok(),5.0,gp::Cut::None,1000,1,l,b).fire());
     limbCorpseMinSpeed.store(8.f);ck(limbEvent60(ok(),7.9,gp::Cut::None,1000,1,l,b).refusal==LimbEventRefusal60::NoSpeed);
     limbCorpseMinSpeed.store(5.0f);}
    // the gorepak50 event switch off = nothing fires
    {LimbCutLedger60 l{};ReceiptBudget60 b{};gp::dismember.store(false);
     ck(limbEvent60(ok(),6.,gp::Cut::None,1000,1,l,b).refusal==LimbEventRefusal60::SwitchOff);
     gp::dismember.store(true);ck(limbEvent60(ok(),6.,gp::Cut::None,1000,1,l,b).fire());}
    // dedupe: the policy only INSPECTS the ledger; the caller admits after the queue succeeds
    {LimbCutLedger60 l{};ReceiptBudget60 b{};
     ck(limbEvent60(ok(),6.,gp::Cut::None,1000,1,l,b).fire());       // policy does not consume
     ck(limbEvent60(ok(),6.,gp::Cut::None,1000,1,l,b).fire());       // same candidate still allowed (retry before queue)
     ck(l.admit(45104,0x1000,1001,int(gp::Cut::RightHand)));         // caller: queue succeeded
     ck(limbEvent60(ok(),6.,gp::Cut::None,1002,1,l,b).refusal==LimbEventRefusal60::Dedupe);
     ck(limbEvent60(ok(),6.,gp::Cut::None,1000+2000,1,l,b).refusal==LimbEventRefusal60::Dedupe); // tick=1001: 3000 is still inside (1999 ms)
     ck(limbEvent60(ok(),6.,gp::Cut::None,1000+2001,1,l,b).refusal==LimbEventRefusal60::Dedupe); // exact last refused instant (2000 ms)
     ck(limbEvent60(ok(),6.,gp::Cut::None,1000+2002,1,l,b).fire());  // 2001 ms: cooldown over
     auto c=ok();c.target=45105;ck(limbEvent60(c,6.,gp::Cut::None,1000,1,l,b).fire());          // other target independent
     c=ok();c.bodyMaterial=34;ck(limbEvent60(c,6.,gp::Cut::None,1000,1,l,b).fire());            // other kind independent
     ck(!l.available(45104,0x1000,1500,int(gp::Cut::RightHand)));}   // a refusal never consumed the ledger (tick still 1001)
    // budget: saturating, session-keyed, consumed only by a complete pass
    {LimbCutLedger60 l{};ReceiptBudget60 b{};b.cap=3;
     auto c=ok();c.identity=0xE00;
     ck(limbEvent60(c,6.,gp::Cut::None,1000,9,l,b).fire());
     ck(limbEvent60(c,6.,gp::Cut::None,1000,9,l,b).fire());
     ck(limbEvent60(c,6.,gp::Cut::None,1000,9,l,b).fire());
     ck(limbEvent60(c,6.,gp::Cut::None,1000,9,l,b).refusal==LimbEventRefusal60::Budget);
     ck(limbEvent60(c,6.,gp::Cut::None,1000,10,l,b).fire());         // a new F3 session rearms
     ReceiptBudget60 b2{};b2.cap=1;c.bodyMaterial=12;                 // refused candidates never consume the budget
     ck(limbEvent60(c,6.,gp::Cut::None,1000,9,l,b2).refusal==LimbEventRefusal60::NoMaterial&&b2.count==0);
     c.bodyMaterial=16;ck(limbEvent60(c,4.,gp::Cut::None,1000,9,l,b2).refusal==LimbEventRefusal60::NoSpeed&&b2.count==0);
     ck(limbEvent60(c,6.,gp::Cut::LeftFoot,1000,9,l,b2).refusal==LimbEventRefusal60::Mismatch&&b2.count==0);
     ck(limbEvent60(c,6.,gp::Cut::None,1000,9,l,b2).fire()&&b2.count==1);}
    // structural refusals
    {LimbCutLedger60 l{};ReceiptBudget60 b{};auto c=ok();
     ck(limbEvent60(c,6.,gp::Cut::None,0,1,l,b).refusal==LimbEventRefusal60::BadRequest);        // no clock
     ck(limbEvent60(c,6.,gp::Cut::None,1000,0,l,b).refusal==LimbEventRefusal60::BadRequest);     // no session key
     c=ok();c.target=0;ck(limbEvent60(c,6.,gp::Cut::None,1000,1,l,b).refusal==LimbEventRefusal60::BadRequest);
     c=ok();c.identity=0;ck(limbEvent60(c,6.,gp::Cut::None,1000,1,l,b).refusal==LimbEventRefusal60::BadRequest);
     c=ok();c.part44=-1;c.rawPart47=-1;ck(limbEvent60(c,6.,gp::Cut::None,1000,1,l,b).refusal==LimbEventRefusal60::BadRequest);
     c=ok();ck(limbEvent60(c,std::nan(""),gp::Cut::None,1000,1,l,b).refusal==LimbEventRefusal60::BadRequest);
     ck(limbEvent60(c,std::numeric_limits<double>::infinity(),gp::Cut::None,1000,1,l,b).refusal==LimbEventRefusal60::BadRequest);
     ck(limbEvent60(c,-3.,gp::Cut::None,1000,1,l,b).refusal==LimbEventRefusal60::NoSpeed);}      // finite, just slow
    // refusal names and the decision contract
    ck(std::string(name(LimbEventRefusal60::Living))=="LIVING"&&std::string(name(LimbEventRefusal60::None))=="NONE");
    {LimbEventDecision60 d{};ck(!d.fire());d.request.kind=gp::Cut::LeftFoot;d.request.limbId=5;ck(d.fire());}
    std::printf("PASS %u limb_event_policy60 checks\n",n);
}
