#include "vrik/physical_weapon.h"
#include "core/snapshot_mailbox.h"
#include "vrik/contact_authority176.h"
#include "subsystems/vrmap/map_hand_owner_m34.h"
#include <atomic>
#include <cstdio>
using namespace kcd2vr;
namespace ph=kcd2vr::physical;
struct TrackingSnapshot {bool allowed=true,head_valid=true,head_held=false,left_valid=true,right_valid=true,left_held=false,right_held=false;std::uint64_t tick=1000;float left_grip=0,left_trigger=0,right_grip=0,right_trigger=0;};
std::atomic<bool> g_gameplay_allowed{true};
namespace animation_filter {std::atomic<bool> nativeBodyOwned112{false};}
namespace diagnostics::lab {bool reference=false;bool NativeReference(){return reference;}}
bool selected=true,mode=true;bool FistSelected101(){return selected;}bool FistModePermits178r22c2(){return mode;}
struct FistOwner101 {bool empty=false;void* player=nullptr;void* manager=nullptr;std::uint64_t epoch=0,request=0,tick=0;};
core::SnapshotMailbox<FistOwner101> g_fist_owner101;std::atomic<std::uint64_t>g_fist_epoch101{3};
#include <map_owner_m34_fist.inc>
struct Tx {ph::State physicalState{};std::array<ph::Profile,2>physicalProfiles{};ph::Output physicalOutput{};bool physicalActive=false;std::uint64_t doorSerial66[2]{};std::uint32_t mapPresentationOwnerM34[2]{~0u,~0u};};
static unsigned checks=0,failed=0;
void ck(bool v,const char* w){++checks;if(!v){++failed;std::printf("FAIL %s\n",w);}}
std::uint32_t producer(Tx&tx,int side,bool prop=false,bool door=false,bool passenger=false,bool readingVisual32=false,bool stringFingers169r2=false,bool nativeFingerCapture=false,bool objectFingers=false,bool combatFistH34=false){
 bool propHeld5916c1[2]{},doorPose66[2]{},nativePassengerHand46[2]{};propHeld5916c1[side]=prop;doorPose66[side]=door;nativePassengerHand46[side]=passenger;
#include <map_owner_m34_producer.inc>
 return tx.mapPresentationOwnerM34[side];
}
int main(){
 Tx tx{};ck(tx.mapPresentationOwnerM34[1]==map_hand_owner_m34::Unknown,"absent producer data is not free");
 ck(producer(tx,1)==0,"ordinary free left hand including free-hand proxy presence remains available");
 ph::Input in{};in.allowed=in.tracked[0]=in.tracked[1]=true;in.epoch=in.bodyEpoch=1;in.tick=1000;in.grip[0]=1;
 in.contact=true;in.dynamics=in.attacks=in.bowActions=false;in.reach[0]=in.reach[1]=1;in.shoulders[0]={.1,0,0};in.shoulders[1]={-.1,0,0};in.hands[1].p={0,-.18,0};
 auto&p=tx.physicalProfiles[0];p.key=p.generation=1;p.side=0;p.classId=4;p.primary=p.support=p.twoHanded=p.geometry=true;
 p.grip[1].p={0,-.09,0};p.tip={0,.9,0};const double lo[]{-.03,-.20,-.03},hi[]{.03,.9,.03};p.supportRegion169r2=kcd2vr::hilt169r2::qualify({0,0,0},p.tip,p.grip[1].p,lo,hi);
 auto next=[&]{++in.sample;in.tick+=10;in.timeNs=in.tick*1000000;tx.physicalOutput=ph::step(tx.physicalState,in,tx.physicalProfiles,[](const ph::Profile&,ph::Pose,ph::Pose,bool){return ph::Contact103{};});tx.physicalActive=true;};
 next();next();ck(tx.physicalOutput.owned[0]&&!tx.physicalOutput.owned[1]&&producer(tx,1)==0,"actual right weapon never claims a genuinely free left");
 in.grip[1]=1;for(int n=0;n<5;++n)next();ck(tx.physicalOutput.support[0]&&tx.physicalOutput.owned[1]&&producer(tx,1)!=0,"actual acquired twohand support yields paper");
 // Native physical output may use anatomical fallback for one frame while the
 // same confirmed support grasp remains retained; the paper must still yield.
 tx.physicalOutput.owned[1]=false;ck((producer(tx,1)&512)!=0,"retained same-resource support outlives one unowned display fallback");
 ++tx.physicalProfiles[0].generation;ck(producer(tx,1)==0,"old support from replaced generation does not hide paper");--tx.physicalProfiles[0].generation;
 in.grip[1]=0;next();ck(producer(tx,1)==0,"actual support release restores free left");
 for(int which=0;which<8;++which){tx={};bool flags[8]{};flags[which]=true;ck(producer(tx,1,flags[0],flags[1],flags[2],flags[3],flags[4],flags[5],flags[6],flags[7])==(2u<<which),"actual producer maps only explicit current left owner bit");}
 tx={};tx.doorSerial66[1]=7;ck(producer(tx,1)==4,"current door lease yields paper without a new native poll");
 TrackingSnapshot ts{};FistOwner101 owner{true,reinterpret_cast<void*>(1),reinterpret_cast<void*>(2),7,3,1000};g_fist_owner101.publish(owner);
 for(float v:{.1f,.6f}){ts.left_grip=ts.left_trigger=v;ck(!QualifiedCombatFistH34(ts,7,1,1000),"partial squeeze is not native-qualified fist");}
 ts.left_grip=ts.left_trigger=.61f;ck(QualifiedCombatFistH34(ts,7,1,1000)&&producer(tx,1,false,false,false,false,false,false,false,QualifiedCombatFistH34(ts,7,1,1000))!=0,"actual qualified closed fist yields paper");
 ts.left_held=true;ck(!QualifiedCombatFistH34(ts,7,1,1000),"held stale tracking cannot qualify fist");ts.left_held=false;
 owner.empty=false;g_fist_owner101.publish(owner);ck(!QualifiedCombatFistH34(ts,7,1,1000),"occupied native owner is not an empty fist");
 owner.empty=true;owner.request=2;g_fist_owner101.publish(owner);ck(!QualifiedCombatFistH34(ts,7,1,1000),"replaced fist request cannot qualify");owner.request=3;owner.tick=919;g_fist_owner101.publish(owner);ck(!QualifiedCombatFistH34(ts,7,1,1000),"expired fist owner cannot qualify");
 std::printf("map_hand_owner_m34=%u/%u retail_calls=NONE\n",checks-failed,checks);return failed?1:0;
}
