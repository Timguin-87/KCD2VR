#include "../src/vrik/own_shield_contact58.h"
#include "../src/frik_pose_probe.cpp"
namespace kcd2vr::frik_pose_probe { namespace {
#include "../src/subsystems/physics/own_shield_clang58.inl"
}}
#include <cassert>
#include <cstdio>
namespace kcd2vr::diagnostic_trace {void Write(const char*) noexcept {}}
using namespace kcd2vr::frik_pose_probe;
int main(){
 namespace sc=kcd2vr::self_contact54;namespace hs=kcd2vr::hand_shape59c7f;
 const auto oldCharacter=g_pose6689.character;const auto oldEpoch=g_pose_epoch6689.load();
 const auto oldRequest=g_weapon_request_epoch6694.load();const auto oldCommitted=g_committed_character6689.load();
 g_weapon_request_epoch6694=2;g_committed_character6689=reinterpret_cast<void*>(0x1234);
 g_pose6689.character=reinterpret_cast<void*>(0x1234);g_pose_epoch6689=1;g_pose6689.selfPat54={};
 CoherentTransaction6689 tx{};tx.epoch=1;tx.physicalActive=true;tx.tracking.allowed=tx.tracking.head_valid=true;
 tx.tracking.left_valid=tx.tracking.right_valid=true;tx.tracking.sequence=10;tx.tracking.tick=1000;tx.tracking.timeNs=1000000000;
 tx.physicalInput.epoch=2;auto& e=tx.physicalEquipment;e.valid=true;e.epoch=1;e.request=2;e.player=reinterpret_cast<void*>(0x2345);
 e.handKnown[0]=e.handKnown[1]=true;e.handKey[0]=77;e.count=1;e.heldIndex[0]=0;e.items[0].held=true;
 auto& p=tx.physicalProfiles[0];p.key=77;p.generation=88;p.entity=99;p.side=0;p.classId=4;
 p.primary=p.geometry=p.diagnosticBounds59c6=true;p.grip[0].p={0,0,0};p.tip={0,1,0};e.items[0].profile=p;
 tx.physicalOutput.active[0]=tx.physicalOutput.owned[0]=true;
 auto& correlation=tx.physicalCorrelation126[0];correlation.socket.valid=true;correlation.socket.key=77;correlation.socket.generation=88;
 correlation.socket.character=g_pose6689.character;correlation.stamp={1,9,990,0x1234};
 hs::Shape shape{};shape.valid=true;shape.count=1;shape.part[0]={{0,0,0},{0,.1,0},.006};
 V3 goal{.06f,.4f,0};const auto physicalBefore=tx.physicalOutput;
 assert(StageOwnBladeContact54(tx,1,true,shape,{0,0,0,1},goal));
 assert(!std::memcmp(&physicalBefore,&tx.physicalOutput,sizeof(physicalBefore))); // weapon/intent untouched
 CommittedFrame6695 packet{};packet.stamp={1,10,1000,0x1234};packet.valid[1]=true;packet.allowed[1].q[3]=1;
 ToT(packet.allowed[1],goal);PublishOwnBladeContact54(tx,packet,false);assert(!g_pose6689.selfPat54.have);
 PublishOwnBladeContact54(tx,packet,true);assert(g_pose6689.selfPat54.have);
 tx.tracking.sequence=11;tx.tracking.tick=1020;tx.tracking.timeNs+=20000000;goal={.010f,.4f,0};
 assert(StageOwnBladeContact54(tx,1,true,shape,{0,0,0,1},goal)&&goal.x>.010f&&goal.x<.071f);
 packet.stamp={1,11,1020,0x1234};ToT(packet.allowed[1],goal);PublishOwnBladeContact54(tx,packet,true);
 e.tick=1020;g_phys_equipment6698.publish(e);
 sc::Event edge{};assert(ReadSelfPat54(edge,1020)&&edge.serial&&edge.key==77&&edge.player==0x2345&&edge.hand==1);
 assert(!ReadSelfPat54(edge,1141));
 e.handKey[1]=55;g_phys_equipment6698.publish(e);assert(!ReadSelfPat54(edge,1020));e.handKey[1]=0;g_phys_equipment6698.publish(e);
 ++g_weapon_request_epoch6694;assert(!ReadSelfPat54(edge,1020));g_weapon_request_epoch6694=2;
 assert(ReadSelfPat54(edge,1020)&&edge.tick==1020);
 auto refuse=[&](){V3 q{.010f,.4f,0};return !StageOwnBladeContact54(tx,1,true,shape,{0,0,0,1},q)&&q.x==.010f;};
 tx.physicalOutput.support[0]=true;assert(refuse());tx.physicalOutput.support[0]=false;
 tx.physicalOutput.owned[1]=true;assert(refuse());tx.physicalOutput.owned[1]=false;
 tx.physicalOutput.bowHand169r2=true;assert(refuse());tx.physicalOutput.bowHand169r2=false;
 tx.physicalOutput.displayLatched[0]=true;assert(refuse());tx.physicalOutput.displayLatched[0]=false;
 tx.physicalOutput.handlingOnly55c1[0]=true;assert(refuse());tx.physicalOutput.handlingOnly55c1[0]=false;
 e.handKey[1]=55;assert(refuse());e.handKey[1]=0;
 tx.tracking.left_held=true;assert(refuse());tx.tracking.left_held=false;
 correlation.stamp.tick=800;assert(refuse());correlation.stamp.tick=1010;
 p.classId=8;assert(refuse());p.classId=4; // shield bounds never become a real face/block witness
 ++p.generation;assert(refuse());--p.generation;
 goal={.010f,.4f,0};assert(!StageOwnBladeContact54(tx,1,false,shape,{0,0,0,1},goal));
 // Actual staged shield path leaves both grips/weapon trajectories unchanged.
 g_pose6689.shieldPat54={};e.count=2;e.heldIndex[1]=1;e.handKey[1]=78;e.items[1].held=true;
 auto& sp=tx.physicalProfiles[1];sp.key=78;sp.generation=89;sp.entity=100;sp.side=1;sp.classId=8;
 sp.primary=sp.geometry=sp.diagnosticBounds59c6=true;sp.diagnosticBoundsMin59c6={-.01,-.3,-.4};sp.diagnosticBoundsMax59c6={.01,.3,.4};
 e.items[1].profile=sp;tx.physicalOutput.active[1]=tx.physicalOutput.owned[1]=true;
 auto& cs=tx.physicalCorrelation126[1];cs.socket.valid=true;cs.socket.key=78;cs.socket.generation=89;cs.socket.character=g_pose6689.character;
 tx.tracking.sequence=20;tx.tracking.tick=2000;tx.tracking.timeNs=2000000000;cs.stamp=correlation.stamp={1,19,1990,0x1234};
 tx.physicalOutput.weapons[0].p.x=.06;const auto beforeShield=tx.physicalOutput;
 assert(StageShieldPat54(tx)&&!std::memcmp(&beforeShield,&tx.physicalOutput,sizeof(beforeShield)));
 packet.stamp={1,20,2000,0x1234};PublishShieldPat54(tx,packet,false);assert(!g_pose6689.shieldPat54.have);
 PublishShieldPat54(tx,packet,true);assert(g_pose6689.shieldPat54.have);
 tx.tracking.sequence=21;tx.tracking.tick=2020;tx.tracking.timeNs+=20000000;tx.physicalOutput.weapons[0].p.x=.016;
 assert(StageShieldPat54(tx));packet.stamp={1,21,2020,0x1234};PublishShieldPat54(tx,packet,true);
 e.tick=2020;g_phys_equipment6698.publish(e);assert(ReadSelfPat54(edge,2020)&&edge.source==sc::Source::VisualShieldBounds&&edge.shieldKey==78);
 ++e.items[1].profile.generation;g_phys_equipment6698.publish(e);assert(!ReadSelfPat54(edge,2020));--e.items[1].profile.generation;
 sp.diagnosticBounds59c6=false;assert(!StageShieldPat54(tx));sp.diagnosticBounds59c6=true;
 tx.physicalOutput.owned[1]=false;assert(!StageShieldPat54(tx));tx.physicalOutput.owned[1]=true;
 // C58 actual Stage -> copied native bounds -> completed packet -> clang hook.
 // The production frik include/call seam is supplied separately to the parent.
 packet.valid[0]=packet.valid[1]=true;tx.tracking.source_epoch=kcd2vr::tracking59c6::epoch.load();
 g_gameplay_allowed=true;kcd2vr::haptics101::enabled=false;kcd2vr::haptics101::allowed=true;
 auto clangSample58=[&](std::uint64_t tick,double x,bool published=true){
  tx.tracking.tick=tick;tx.tracking.timeNs=tick*1000000;++tx.tracking.sequence;
  cs.stamp=correlation.stamp={1,tx.tracking.sequence,tick,0x1234};
  tx.physicalOutput.weapons[0].p.x=x;g_tool_context19c4r2.store(tick<<8);
  assert(StageShieldPat54(tx));packet.stamp={1,tx.tracking.sequence,tick,0x1234};
  return PublishOwnShieldClang58(tx,packet,published,tick);
 };
 const auto quiet58=kcd2vr::haptics101::eventSequence.load();
 assert(!clangSample58(2300,.06).tick);assert(clangSample58(2320,.016).tick);
 assert(kcd2vr::haptics101::eventSequence.load()==quiet58); // user haptics OFF
 kcd2vr::haptics101::enabled=true;
 assert(!clangSample58(2600,.06).tick);assert(clangSample58(2620,.016).tick);
 assert(kcd2vr::haptics101::eventSequence.load()==quiet58+2); // both actual held hands
 assert(!clangSample58(2640,.016).tick); // held overlap is quiet
 assert(!clangSample58(2660,.06,false).tick);assert(!clangSample58(2680,.016).tick); // failed publication retires
 assert(!clangSample58(2900,.06).tick);g_ctx_menu=true;assert(!clangSample58(2920,.016).tick);
 g_ctx_menu=false;assert(!clangSample58(2940,.016).tick); // no replay across menu
 kcd2vr::haptics101::enabled=false;kcd2vr::haptics101::allowed=false;
 g_pose6689.character=oldCharacter;g_pose_epoch6689=oldEpoch;
 g_weapon_request_epoch6694=oldRequest;g_committed_character6689=oldCommitted;
 std::puts("PASS C54 actual coherent own-sword correction/commit helper: exact owner, empty hand, support/string/native/grace yields, committed-only pat publication, unchanged weapon state");
}
