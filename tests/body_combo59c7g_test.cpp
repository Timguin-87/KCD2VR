#include "../src/vrik/body_combo_recipes59c7g.h"
#include "../src/vrik/receipt_build59c7g.h"
#include "../src/vrik/contact_metadata59c7g.h"
#include <cassert>
using namespace kcd2vr::bodycombo59c7g;
int main(){
 const std::array<Recipe,1> r{{{"abdomen","example","Longsword",{{
  {Region::Any,Feature::Any,Motion::Any,-1,false,true},
  {Region::Abdomen,Feature::Tip,Motion::Thrust,-1,true,false}}},2,1000}}};
 Contact c{};c.attempt=1;c.tick=100;c.epoch=1;c.request=2;c.key=3;c.generation=4;
 c.target=5;c.targetIdentity=6;c.weaponClass=4;c.continuation=true;c.bodyContact=true;
 c.outcome=Outcome::Hit;c.region=Region::Head;c.feature=Feature::Tip;c.motion=Motion::Thrust;c.origin=0;
 Matcher<1> m;assert(m.observe(r,c).advanced==1);
 assert(m.observe(r,c).duplicate&&m.progress[0].next==1);
 c.target=7;c.targetIdentity=8;assert(m.observe(r,c).duplicate); // one swing, two victims
 c.target=5;c.targetIdentity=6;c.attempt=2;c.tick=300;c.region=Region::Abdomen;c.origin=2;c.twoHands=true;
 assert(m.observe(r,c).completed==1); // origin and actual hit region are independent
 for(unsigned i=3;i<25;++i){c.attempt=i;c.tick+=10;m.observe(r,c);}
 c.attempt=1;assert(m.observe(r,c).duplicate); // no small dedupe-ring wrap
 m={};c.attempt=30;c.tick=500;c.region=Region::Head;c.outcome=Outcome::Block;c.bodyContact=false;
 assert(m.observe(r,c).advanced==1);
 ++c.attempt;c.tick+=10;c.region=Region::Abdomen;assert(!m.observe(r,c).completed);
 ++c.attempt;c.outcome=Outcome::PerfectBlock;assert(m.observe(r,c).reset&&m.progress[0].next==0);
 ++c.attempt;c.outcome=Outcome::Hit;c.bodyContact=true;assert(m.observe(r,c).advanced==1);
 ++c.attempt;c.tick+=1001;assert(!m.observe(r,c).completed); // timeout starts a new chain
 ++c.attempt;c.continuation=false;assert(m.observe(r,c).reset);
 assert(!regionMatches(Region::Knee,humanMaterial(26))); // a leg is not proof of a knee
 assert(humanMaterial(3)==Region::Face&&humanMaterial(8)==Region::Abdomen&&humanMaterial(99)==Region::Unknown);
 c={};c.weaponClass=4;assert(equipmentMatches("Longsword",c));c.shield=true;assert(!equipmentMatches("Longsword",c));
 // Failed/evaluation submissions cannot turn into physical sequence acceptance.
 kcd2vr::contact58c1::Receipt receipt{};receipt.kind=1;receipt.bodyMaterial=8;receipt.bodyContact=true;
 receipt.fields=receipt.accepted=true;receipt.applied=false;receipt.damage=10;
 assert(kcd2vr::contact59c7g::physicalCandidate(receipt).outcome==Outcome::Rejected);
 receipt.applied=true;receipt.blocked=receipt.perfect=receipt.parry=receipt.master=0;
 assert(kcd2vr::contact59c7g::physicalCandidate(receipt).outcome==Outcome::Hit);
 receipt.blocked=1;assert(kcd2vr::contact59c7g::physicalCandidate(receipt).outcome==Outcome::Block);
 receipt.perfect=1;assert(kcd2vr::contact59c7g::physicalCandidate(receipt).outcome==Outcome::PerfectBlock);
 assert(recipes.size()==19);
}
