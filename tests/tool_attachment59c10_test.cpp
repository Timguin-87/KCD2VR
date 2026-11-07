#include "../src/vrik/static_grab_r89.h"
// Actual production F5 attachment code with explicit native-memory/call leaves.
// Host validation only: no game calls, no claim to emulate WHGame ABI.
#include <array>
#include <atomic>
#include <cassert>
#include <cmath>
#include <cstdint>
#include <cstring>
#include <cstdio>
#include <string>
#include <vector>
#include "diagnostics/interaction_receipt59c10.h"
#include "../src/core/snapshot_mailbox.h"
#include "../src/core/native_grasp13.h"
#include "../src/action_time_owner59c11.h"
#include "../src/vrik/physical_weapon.h"
#include "../src/vrik/workstation178r20c1.h"
#include "../src/vrik/alchemy_tool18.h"
#include "../src/vrik/smith_tool36.h"
#include "../src/subsystems/smithing/bridge67.h"
#include "../src/subsystems/smithing/smith_contact68.h"
#include "../src/vrik/alchemy_stage19c4.h"
#include "../src/activity_input163.h"
#include "../src/frik_pose_probe.h"
#include "../src/vrik/visual_selection59c7f.h"
#include "../src/vrik/presentation_context55.h"
namespace ph=kcd2vr::physical;
namespace pc=kcd2vr::pose_core;
struct QuatT28 {float q[4]{0,0,0,1},t[3]{};};
struct ToolSocket59c10 {void*self{},*attachment{},*character{},*entity{};std::uint32_t id{};int side=-1,joint=-1;QuatT28 relative{};std::uint64_t epoch{},tick{},request{};bool nativeGrasp13=false;kcd2vr::alchemy_tool18::Binding alchemy18{};kcd2vr::smith_tool36::Binding smith36{};QuatT28 grip67{};bool gripReady67=false;std::uint64_t modeEpoch67=0;};
struct ToolPose59c10 {ToolSocket59c10 socket{};QuatT28 model{},nativeObserved28{};bool valid=false,nativeObservedKnown28=false;kcd2vr::grasp13::FingerReference55 fingers55{};QuatT28 parkedWorldD47{};bool parkedD47=false;};
#include "../src/vrik/alchemy_contact_result_d80.h"
struct CommittedFrame6695 {AlchemyContactResultD80 alchemyContactD80[2]{};pc::Stamp stamp{};QuatT28 native[2]{},allowed[2]{},propRequested5916c1[2]{};float world[8]{0,0,0,1,0,0,0,1};bool valid[2]{},propRequestedValid5916c1[2]{};ToolPose59c10 tools59c10[2]{};std::uint64_t doorTrackingEpoch66=0;float mapGripM39[2]{};
 QuatT28 originalToolSlotD71[2]{};bool originalToolSlotValidD71[2]{};
 std::uint64_t itemHandoverRevisionD71=0,publicationR24=0;std::uint32_t fistJointCountH31=0;
 int fistWristBoneH31[2]{-1,-1},fistSlotBoneH31[2]{-1,-1};kcd2vr::subsystems::smithing::bridge67::FixturePose smithFixture[2]{};};
static kcd2vr::core::SnapshotMailbox<ToolSocket59c10> g_tool_socket59c10[2];
static kcd2vr::core::SnapshotMailbox<kcd2vr::subsystems::smithing::contact68::HammerFrame> g_smith_placed68;
static std::uint64_t now=1000;
static std::uint64_t GetTickCount64(){return now;}
static std::uintptr_t WhBase(){return 0;}
namespace retail_build112 {static bool ready=true;static bool Ready(std::uintptr_t){return ready;}}
namespace diagnostics { namespace capture168=kcd2vr::diagnostics::capture168; }
namespace diagnostics::lab {static bool reference=false;static bool NativeReference(){return reference;}}
static std::vector<std::string> receipts;
namespace diagnostic_trace {static void Write(const char* text){receipts.emplace_back(text);}}
static std::atomic<bool>g_gameplay_allowed{true},g_pose_restore_fault6689{false};
static std::atomic<bool>g_dialogue_body178r9{false},g_ctx_dialogue{false},g_ctx_menu{false},g_ctx_cutscene{false},g_ctx_reading{false};
namespace animation_filter {static std::atomic<bool>nativeBodyOwned112{false};static bool kick175=false;static bool NativeKickActive175(){return kick175;}}
static std::atomic<std::uint64_t>g_tool_context19c4r2{0};
static std::atomic<std::uint64_t>g_pose_epoch6689{7};
static bool calibrated19c4r2=true;static int census19c4r2=3,skin19c4r2=0;
namespace kcd2vr::frik_pose_probe {
 bool CalibrationReady() noexcept{return calibrated19c4r2;}
 int CombatCensusMode() noexcept{return census19c4r2;}
 int FinalSkinTestMode() noexcept{return skin19c4r2;}
}
namespace activity_input163=kcd2vr::activity_input163;
namespace workstation178r20c1=kcd2vr::workstation178r20c1;
namespace hands59c7f=kcd2vr::hands59c7f;
namespace visual_selection59c7f=kcd2vr::visual_selection59c7f;
namespace frik_pose_probe=kcd2vr::frik_pose_probe;
// Both snippets are the actual Mod producer and the first production context
// publication; this fixture does not invent menu=false for native minigames.
static kcd2vr::frik_pose_probe::GameStateContext ToolContext19c4r2(bool nativeControls=false,bool realUi=false,
    bool vrMenu=false,bool theater=false,bool matrix=false,bool uiBlocks=false,bool dialogue=false,bool cutscene=false,bool* bodyAdmitted=nullptr){
    const struct Activity {bool nativeControls;} activity163{nativeControls};
    const struct UI {bool blocked;bool BlocksWorldInteraction()const{return blocked;}} ui89_policy{realUi};
    const struct Input {bool blocks_gameplay;} ui88_input_result{uiBlocks};
    const bool mis_menu_after_frame=vrMenu,safe_ui_fit=theater,matrix_active=matrix;
    const bool dialogue_active=dialogue,cutscene_flat_ui=cutscene;
    kcd2vr::frik_pose_probe::GameStateContext gs{};
#include "../src/vrik/workstation_pose_context19c4r2_producer.inl"
    const auto& state=gs;
#include "../src/vrik/workstation_pose_context19c4r2_publish.inl"
#include "../src/vrik/workstation_pose_body19c4r2_producer.inl"
    if(bodyAdmitted)*bodyAdmitted=workstationBody178r13;
    return gs;
}
static std::atomic<std::uint32_t>g_phys_relevant_id101[2]{};
static std::atomic<std::uint64_t>g_native_use_until107[2]{};
static std::atomic<std::uint64_t>g_native_use_serial107[2]{},g_native_wrist_until107[2]{},g_native_wrist_retired107[2]{};
static std::atomic<unsigned>g_page_memo_ms178r7{};
static std::atomic<std::uint64_t>g_read_faults178r7{};
#include "../src/subsystems/interactions/native_hand_use_control.inl"
static std::atomic<std::uint32_t>g_native_use_target59c10[2]{};
static std::atomic<float>g_left_grip{1},g_right_grip{1};
static std::uint32_t held[2]{42,0};
static bool tracking=true,placement=true,identity=true,childPresent=true;
static int writes=0,readbacks=0;
static CommittedFrame6695 frame{};
static std::uint64_t completedPublicationD71=0;
static std::uint64_t ReadCompletedPublicationT23() noexcept {return completedPublicationD71;}
static const CommittedFrame6695* committedReadOverrideD47=nullptr;
static std::array<unsigned char,0x100>self{},attachment{},manager{},character{},entity{};
struct ReadScope101R2{};
// Controlled publication interleaving at the native-read leaf; production adapter is unchanged.
static void(*beforeNativeFixtureReadR95)(const void*)=nullptr;
template<class T>static bool Read(const void*p,T& out){if(beforeNativeFixtureReadR95)beforeNativeFixtureReadR95(p);if(!p)return false;std::memcpy(&out,p,sizeof(out));return true;}
template<class M,class T>static void Put(M& memory,std::size_t offset,const T& v){assert(offset+sizeof(v)<=memory.size());std::memcpy(memory.data()+offset,&v,sizeof(v));}
static bool NativeCandidateItemId6699(int s,std::uint32_t& id){id=held[s];return id!=0;}
static bool NativeHeldItemId6694(int s,std::uint32_t& id){id=held[s];return id!=0;}
static std::uint64_t finishingHandKeysR94[2]{};
static bool NativeHandKey6697(void* m,int side,std::uint64_t& key){key=0;if(m!=manager.data()||side<0||side>1)return false;key=finishingHandKeysR94[side];return true;}
static bool TrackingHeadUsable111(std::uint64_t){return tracking;}
static bool TrackingHandUsable111(int,std::uint64_t){return tracking;}
static bool ReadCommittedPacket6695(CommittedFrame6695& out){out=committedReadOverrideD47?*committedReadOverrideD47:frame;return now>=out.stamp.tick&&now-out.stamp.tick<=80;}
static unsigned bodyReads19=0;
static bool acceptVisualPacket55=true;
// Controlled committed-packet leaf. The complete actual C55 typed packet reader
// is exercised separately by tracked_frame51_camera_test; this fixture invokes
// the actual attachment admission, socket join and final writer path below.
static bool ReadCommittedPacketFor6695(CommittedFrame6695& out,bool workstation,
    const kcd2vr::grasp13::Acquisition54* picking55=nullptr,
    const kcd2vr::grasp13::FinishingPresentationR94* finishingR94=nullptr){
 ++bodyReads19;if(!g_gameplay_allowed.load()&&!(workstation&&g_dialogue_body178r9.load())&&
    !(picking55&&acceptVisualPacket55)&&!(finishingR94&&acceptVisualPacket55))return false;return ReadCommittedPacket6695(out);}
static bool PhysVft6698(void*p,std::uintptr_t expected){std::uintptr_t v=0;return Read(p,v)&&v==expected;}
static bool PhysSocketContract101(std::uintptr_t){return true;}
static void*EntityById6694(std::uint32_t id){return id==42?entity.data():nullptr;}
struct ModelWorld6692 {ph::Pose pose{};pc::Quat q{};pc::Vec3 t{};float scale=1;};
static float worldScale35=1;
static ph::Vec3 attachmentRootD47{10,20,30};
static void(*onAttachmentWorld55)()=nullptr;
static bool PhysAttachmentWorld164(void*,std::uintptr_t,ModelWorld6692& x){x={};x.pose.p=attachmentRootD47;x.scale=worldScale35;if(onAttachmentWorld55)onAttachmentWorld55();return true;}
static bool ReadAttachmentQs6689(void*,float* out){out[7]=1;return true;}
static ph::Pose PhysPose6698(const QuatT28& t){return {{t.q[0],t.q[1],t.q[2],t.q[3]},{t.t[0],t.t[1],t.t[2]}};}
static ph::Pose PhysCurrentWorld164(ph::Pose p,const ModelWorld6692& x){p.p=p.p*x.scale+x.pose.p;return p;}
static bool PhysEntityIdentity135(void*p,std::uint32_t id){return identity&&p==entity.data()&&id==42;}
static float lastWanted[8]{};
static void (*placementNotify107)()=nullptr;
static bool PlaceBothConsumers6689(void*,void*,const float* wanted,bool& child){if(!placement)return false;++writes;std::memcpy(lastWanted,wanted,sizeof(lastWanted));child=true;if(placementNotify107)placementNotify107();return true;}
static bool PlacementReadback6689(void*,void*,std::uint32_t,const float*,bool& checked){++readbacks;checked=childPresent;return true;}
static pc::Vec3 FromT(const QuatT28& t){return {t.t[0],t.t[1],t.t[2]};}
static double Len(pc::Vec3 p){return pc::length(p);}
static QuatT28 Make(pc::Quat q,pc::Vec3 p){return {{float(q.x),float(q.y),float(q.z),float(q.w)},{float(p.x),float(p.y),float(p.z)}};}
static pc::Quat Q(const QuatT28& t){return {t.q[0],t.q[1],t.q[2],t.q[3]};}
static QuatT28 AbsoluteFromRelative(const QuatT28&a,const QuatT28&b){return Make(pc::multiply(Q(a),Q(b)),FromT(a)+pc::rotate(Q(a),FromT(b)));}
static QuatT28 RelativeFromAbsolute(const QuatT28&a,const QuatT28&b){const auto qa=Q(a);const pc::Quat inv{-qa.x,-qa.y,-qa.z,qa.w};return Make(pc::multiply(inv,Q(b)),pc::rotate(inv,FromT(b)-FromT(a)));}
static bool ValidPose6689(const QuatT28&t) noexcept {return ph::finite(PhysPose6698(t));}
static kcd2vr::grasp13::Binding graspBinding{};
static kcd2vr::grasp13::Placement receivedPlacement{};static unsigned nativePlacements=0;
static bool ReadGrasp(kcd2vr::grasp13::Binding& b) noexcept {b=graspBinding;return b.attached||b.socketOnly28;}
static kcd2vr::grasp13::Acquisition54 acquisition55{};static bool acquisitionAvailable55=false;
static kcd2vr::grasp13::FinishingPresentationR94 finishingReceiptR94{};
static bool finishingAvailableR94=false;
namespace kcd2vr::action_time_owner59c11 {
 bool ReadFinishingAttachmentPresentationR94(grasp13::FinishingPresentationR94& out) noexcept {
  out={};if(!finishingAvailableR94||now<finishingReceiptR94.requestedAt||now-finishingReceiptR94.requestedAt>80)return false;
  out=finishingReceiptR94;return true;
 }
 bool ReadAdoptedAttachmentPresentationR95(grasp13::FinishingPresentationR94& out) noexcept {
  out={};
  if(finishingAvailableR94||now<finishingReceiptR94.requestedAt||!graspBinding.attached||!graspBinding.place||
     !grasp13::sameHeldResource54(finishingReceiptR94.binding,graspBinding)||!graspBinding.gripReady55||
     std::memcmp(&finishingReceiptR94.binding.grip55,&graspBinding.grip55,sizeof(graspBinding.grip55)))return false;
  out=finishingReceiptR94;return true;
 }
 bool FinishingOrHeldPresentationCurrentR94(const grasp13::FinishingPresentationR94& wanted) noexcept {
  if(now<wanted.requestedAt||!grasp13::sameFinishingPresentationR94(wanted,finishingReceiptR94))return false;
  if(finishingAvailableR94)return now-wanted.requestedAt<=80;
  grasp13::FinishingPresentationR94 adopted{};return ReadAdoptedAttachmentPresentationR95(adopted);
 }
 bool ReadAcquisition54(grasp13::Acquisition54& out) noexcept {out=acquisition55;return acquisitionAvailable55;}
 bool ReadPendingAttachmentPresentationD72(grasp13::Binding& out) noexcept {
  out={};if(!acquisitionAvailable55||!grasp13::pendingAttachmentPresentationD72(graspBinding,acquisition55,now))return false;
  out=graspBinding;return true;
 }
}
namespace action_time_owner59c11=kcd2vr::action_time_owner59c11;
static void ReceivePlacement(const kcd2vr::grasp13::Placement& p) noexcept {receivedPlacement=p;++nativePlacements;}
static bool (*g_graspBinding13)(kcd2vr::grasp13::Binding&) noexcept=nullptr;
static kcd2vr::grasp13::SocketProof28 socketProof28{};static unsigned socketProofs28=0;
static void ReceiveSocket28(const kcd2vr::grasp13::SocketProof28& p) noexcept {socketProof28=p;++socketProofs28;}
static void (*g_graspSocket28)(const kcd2vr::grasp13::SocketProof28&) noexcept=&ReceiveSocket28;
static void (*g_graspPlaced13)(const kcd2vr::grasp13::Placement&) noexcept=nullptr;
static std::array<unsigned char,0x700> alchemy{},alchemyPlayer{};
static std::array<unsigned char,0x100> alchemyAction{};
static std::array<unsigned char,0x300> alchemyChild19{};
static std::array<std::uintptr_t,2> alchemySlots{};
static bool alchemyActive=false;
static bool ReadAlchemyTool18(void* att,std::uint32_t id,std::uintptr_t ch,kcd2vr::alchemy_tool18::Binding& out) noexcept {
 const auto rd=[](std::uintptr_t at,auto& value){return Read(reinterpret_cast<void*>(at),value);};
 const auto lifetime=kcd2vr::activity_input163::witness175.lifetime175.load();
 if(kcd2vr::activity_input163::witness175.transitionDepth175.load())return false;
 const bool valid=kcd2vr::alchemy_tool18::read(rd,0,alchemyActive?reinterpret_cast<std::uintptr_t>(alchemy.data()):0,
     reinterpret_cast<std::uintptr_t>(alchemyPlayer.data()),ch,reinterpret_cast<std::uintptr_t>(att),id,out);
 if(!valid||lifetime!=kcd2vr::activity_input163::witness175.lifetime175.load())return false;
 out.lifetime=lifetime;return true;
}
// C37: smithing never runs in this fixture; its exact reader is covered by smith_tool36_test.
static std::atomic<void*> g_henry_char_attachment_manager{nullptr};
static bool smithActive37=false;
static kcd2vr::smith_tool36::Binding smithBinding37{};
static bool ReadSmithTool36(void* at,std::uint32_t id,std::uintptr_t ch,kcd2vr::smith_tool36::Binding& out) noexcept {
 out={};if(!smithActive37||smithBinding37.attachment!=reinterpret_cast<std::uintptr_t>(at)||
  smithBinding37.entity!=id||smithBinding37.character!=ch)return false;
 out=smithBinding37;return true;
}
// Native passenger owner is an explicit external leaf in this retained fixture.
static bool nativePassengerR94=false;
static bool NativePassengerCurrentA34(std::uint64_t) noexcept {return nativePassengerR94;}
// Diagnostics are explicit inactive host leaves; placement ownership above
// and the actual complete production adapter below are executed unchanged.
namespace kcd2vr::diagnostics::hands28 {static bool active(std::uint64_t){return false;}}
template<class... Args>static void TraceToolItem28(const Args&...){ }
#include "../src/vrik/tool_attachment59c10.inl"

#ifndef KCD2VR_TOOL_ATTACHMENT_NO_MAIN
int main(){
 namespace ws=kcd2vr::workstation178r20c1;namespace cr=kcd2vr::combat_rules178r10;
 cr::f5Rung=2;cr::workstationEnabled=true;ws::setExperiment36(true);
 ws::heldToolActivity59c10=1;ws::heldToolActivityTick59c10=now;
 Put(self,8,std::uint32_t(42));Put(attachment,0,std::uintptr_t(0x3A657C0));Put(attachment,0x28,static_cast<void*>(manager.data()));
 Put(manager,0x70,static_cast<void*>(character.data()));Put(character,0,std::uintptr_t(0x40602F8));
 Put(attachment,0x78,std::int16_t(0));Put(attachment,0x9C,QuatT28{});
 frame.stamp.actor=reinterpret_cast<std::uintptr_t>(character.data());frame.stamp.epoch=7;frame.stamp.tick=now;frame.valid[0]=true;
 frame.native[0].t[0]=1;frame.allowed[0].t[0]=2;
 std::array<QuatT28,1>original{};original[0].t[0]=1.1f;
 for(unsigned unknown:{2u,255u}){Put(attachment,0x7A,std::uint8_t(unknown));
  assert(!ToolAttachment59c10(self.data(),attachment.data())&&writes==0);
  ToolSocket59c10 notPublished{};assert(!g_tool_socket59c10[0].read(notPublished)||!notPublished.self);
 }
 Put(attachment,0x7A,std::uint8_t(1));
 assert(!ToolAttachment59c10(self.data(),attachment.data())&&writes==0);
 // Native keeps publishing another pending transform before each attachment
 // callback. The current+9C socket is still valid; discovery alone never writes.
 assert(!ToolAttachment59c10(self.data(),attachment.data())&&writes==0);
 BuildToolPoses59c10(frame,original.data(),original.size());
 assert(frame.tools59c10[0].valid&&std::abs(frame.tools59c10[0].model.t[0]-2.1f)<.00001);
 // +7A stays1: only a committed SAME current socket may now place the item.
 assert(ToolAttachment59c10(self.data(),attachment.data())&&writes==1&&std::abs(lastWanted[4]-12.1f)<.00001);
 assert(receipts.empty()&&readbacks==0); // recorder-off adds no receipt/readback work
 Put(attachment,0x7A,std::uint8_t(1));
 assert(ToolAttachment59c10(self.data(),attachment.data())&&writes==2);
 {const auto saved=frame.stamp.epoch;frame.stamp.epoch=saved+1;
  assert(!ToolAttachment59c10(self.data(),attachment.data())&&writes==2);frame.stamp.epoch=saved;}
 {const auto saved=frame.tools59c10[0].socket.relative;frame.tools59c10[0].socket.relative.t[0]+=.1f;
  assert(!ToolAttachment59c10(self.data(),attachment.data())&&writes==2);frame.tools59c10[0].socket.relative=saved;}
 // The engine consumes a DIFFERENT pending relative transform between body and
 // attachment callbacks. The old committed grip must not place through it.
 {QuatT28 changed{};changed.t[0]=.2f;Put(attachment,0x9C,changed);
  assert(!ToolAttachment59c10(self.data(),attachment.data())&&writes==2);
  const auto saved=frame;auto next=frame;next.tools59c10[0]={};
  BuildToolPoses59c10(next,original.data(),original.size());
  assert(next.tools59c10[0].valid&&std::abs(next.tools59c10[0].model.t[0]-2.3f)<.00001);
  frame=saved;frame.tools59c10[0]={};Put(attachment,0x9C,QuatT28{});
  assert(!ToolAttachment59c10(self.data(),attachment.data())&&writes==2); // republish current socket only
 }
 Put(attachment,0x7A,std::uint8_t(0));
 // Reuse always derives from original native joints; no cumulative delta.
 for(int i=0;i<10;++i)BuildToolPoses59c10(frame,original.data(),original.size());
 assert(std::abs(frame.tools59c10[0].model.t[0]-2.1f)<.00001);
 g_right_grip=.1f;assert(!ToolAttachment59c10(self.data(),attachment.data())&&writes==2);g_right_grip=1;
 tracking=false;assert(!ToolAttachment59c10(self.data(),attachment.data()));tracking=true;
 held[0]=0;assert(!ToolAttachment59c10(self.data(),attachment.data()));held[0]=42;
 g_phys_relevant_id101[0]=42;assert(!ToolAttachment59c10(self.data(),attachment.data()));g_phys_relevant_id101[0]=0;
 identity=false;assert(!ToolAttachment59c10(self.data(),attachment.data()));identity=true;
 auto candidate=frame;candidate.stamp.epoch=8;candidate.tools59c10[0]={};BuildToolPoses59c10(candidate,original.data(),1);assert(!candidate.tools59c10[0].valid);
 candidate=frame;candidate.tools59c10[0]={};candidate.stamp.actor=123;BuildToolPoses59c10(candidate,original.data(),1);assert(!candidate.tools59c10[0].valid);
 candidate=frame;candidate.tools59c10[0]={};BuildToolPoses59c10(candidate,original.data(),0);assert(!candidate.tools59c10[0].valid);
 now+=81;assert(!ToolAttachment59c10(self.data(),attachment.data()));candidate=frame;candidate.tools59c10[0]={};BuildToolPoses59c10(candidate,original.data(),1);assert(!candidate.tools59c10[0].valid);now-=81;
 ws::setExperiment36(false);assert(!ToolAttachment59c10(self.data(),attachment.data())&&writes==2);
 ws::setExperiment36(true);assert(!ToolAttachment59c10(self.data(),attachment.data())&&writes==2); // old request cannot resume writes
 retail_build112::ready=false;assert(!ToolAttachment59c10(self.data(),attachment.data()));
 // Active capture explains the exact ownership gate without changing writes.
 retail_build112::ready=true;now+=1000;
 diagnostics::capture168::gate.select(diagnostics::capture168::Domain::Interaction,true,now);
 g_gameplay_allowed=false;assert(!ToolAttachment59c10(self.data(),attachment.data()));
 assert(!receipts.empty()&&receipts.back().find("WORKSTATION_CONTEXT_STALE")!=std::string::npos&&writes==2&&readbacks==0);
 g_gameplay_allowed=true;frame.stamp.tick=now;ws::heldToolActivityTick59c10=now;
 assert(!ToolAttachment59c10(self.data(),attachment.data()));BuildToolPoses59c10(frame,original.data(),1);
 now+=600;frame.stamp.tick=now;ws::heldToolActivityTick59c10=now;
 assert(ToolAttachment59c10(self.data(),attachment.data())&&writes==3&&readbacks==1);
 assert(receipts.back().find("CONTROLLER")!=std::string::npos);
 // The native grasp works without F5, a temporary Use lease, or the old hand census.
 diagnostics::capture168::gate.select(diagnostics::capture168::Domain::Interaction,false,now);
 ws::setExperiment36(false);kcd2vr::combat_rules178r10::f5Rung=0;g_native_use_until107[0]=0;held[0]=0;childPresent=false;
 frame.tools59c10[0]={};frame.stamp.tick=now;g_graspBinding13=&ReadGrasp;g_graspPlaced13=&ReceivePlacement;
 graspBinding={frame.stamp.actor,11,12,13,700,42,frame.stamp.epoch,42,99,0,true,true};
 NotifyNativeHandUse(0,1800);const auto wristBefore107=g_native_use_serial107[0].load();
 assert(!ToolAttachment59c10(self.data(),attachment.data()));BuildToolPoses59c10(frame,original.data(),1);
 assert(g_native_wrist_retired107[0]==0&&"socket bootstrap cannot retire an accepted native animation");
 assert(frame.tools59c10[0].valid&&ToolAttachment59c10(self.data(),attachment.data()));
 assert(nativePlacements==1&&receivedPlacement.sample==frame.stamp.sample&&receivedPlacement.world.p[0]==lastWanted[4]);
 assert(g_native_wrist_retired107[0]==wristBefore107&&g_native_use_until107[0]==now+80&&
        g_native_wrist_until107[0]==now+1800&&"actual controller placement retires only earlier wrist provenance, not fingers/contact/native action");
 // A new accepted action delivered inside the actual placement leaf has the
 // SAME millisecond/deadline, but a distinct existing serial and must survive.
 placementNotify107=+[](){NotifyNativeHandUse(0,1800);};
 assert(ToolAttachment59c10(self.data(),attachment.data()));placementNotify107=nullptr;
 assert(g_native_use_serial107[0]==wristBefore107+1&&g_native_wrist_retired107[0]==wristBefore107&&
        g_native_wrist_until107[0]==now+1800&&"stale placement cannot retire a newer same-millisecond accepted action");
 placement=false;assert(!ToolAttachment59c10(self.data(),attachment.data()));placement=true;
 assert(g_native_wrist_retired107[0]==wristBefore107&&"failed placement never changes wrist provenance");
 assert(ToolAttachment59c10(self.data(),attachment.data())&&g_native_wrist_retired107[0]==wristBefore107+1);
 // D40: bootstrap and canonical item presentation share the native grip.
 // Legacy clearance preferences cannot create a temporary outward offset and
 // a second visible inward step when the same canonical grip is captured.
 assert(std::abs(lastWanted[4]-12.1f)<.00001f);
 const auto nativeRelative35=frame.tools59c10[0].socket.relative;
 for(int i=0;i<20;++i)BuildToolPoses59c10(frame,original.data(),1);
 assert(std::abs(frame.tools59c10[0].model.t[0]-2.1f)<.00001f&&
    std::memcmp(&nativeRelative35,&frame.tools59c10[0].socket.relative,sizeof(nativeRelative35))==0&&frame.allowed[0].t[0]==2);
 kcd2vr::grasp13::setPresentation35(.06f,.28f);assert(ToolAttachment59c10(self.data(),attachment.data()));
 assert(std::abs(lastWanted[4]-12.1f)<.00001f);
 kcd2vr::grasp13::setPresentation35(0,.28f);assert(ToolAttachment59c10(self.data(),attachment.data()));
 assert(std::abs(lastWanted[4]-12.1f)<.00001f);
 kcd2vr::grasp13::setPresentation35(.02f,.28f);assert(ToolAttachment59c10(self.data(),attachment.data()));
 worldScale35=2;assert(ToolAttachment59c10(self.data(),attachment.data()));
 assert(std::abs(lastWanted[4]-14.2f)<.00001f); // native grip scales with the character; no bootstrap-only clearance
 worldScale35=1;assert(ToolAttachment59c10(self.data(),attachment.data()));
 // Canonical native grip remains fixed through the native pocket/exit
 // trajectory. It is learned from the actual original-joint socket proof,
 // never from a generic item offset or from the controller-moved entity.
 {const auto saved=graspBinding;const auto savedFrame=frame;const auto savedOriginal=original;
  assert(socketProof28.gripReady55&&std::abs(socketProof28.grip55.p[0]-.1f)<.00001f);
  // Carry native fingers WITH the original tool pose. Reusing the XR clocks
  // must still select the later actual animation transaction's payload.
  kcd2vr::grasp13::FingerReference55 references[2]{};auto& ref=references[0];
  ref.sample=frame.stamp.sample=701;ref.tick=frame.stamp.tick;ref.count=2;
  ref.mapping.skeleton=0x1234;ref.mapping.generation=1;ref.mapping.joints=4;ref.mapping.wrist=1;ref.mapping.count=2;
  ref.mapping.ids[0]=2;ref.mapping.ids[1]=3;ref.mapping.parents[0]=1;ref.mapping.parents[1]=2;
  auto& a=ref.acquisition;a.character=graspBinding.character;a.epoch=graspBinding.epoch;a.side=graspBinding.side;
  a.action=101;a.scope=102;a.serial=103;a.startedAt=900;a.tick=now;a.active=true;
  a.graspToken=graspBinding.token;a.graspKey=graspBinding.key;a.graspItem=graspBinding.item;a.graspEntity=graspBinding.entity;
  for(unsigned i=0;i<ref.count;++i)ref.rotations[i]={.1f,0,0,std::sqrt(.99f)};
  BuildToolPoses59c10(frame,original.data(),1,references);
  assert(frame.tools59c10[0].fingers55.rotations[0][0]==.1f);
  for(unsigned i=0;i<ref.count;++i)ref.rotations[i]={.3f,0,0,std::sqrt(.91f)};
  BuildToolPoses59c10(frame,original.data(),1,references);
  assert(ToolAttachment59c10(self.data(),attachment.data()));
  assert(socketProof28.fingers55.sample==701&&socketProof28.fingers55.rotations[0][0]==.3f&&
      socketProof28.fingers55.acquisition.graspKey==graspBinding.key);
  const auto paired=socketProof28.fingers55;
  // A stale or foreign finger payload cannot cross the current native socket;
  // its absence never invents a new blocker for item ownership/placement.
  ++ref.sample;BuildToolPoses59c10(frame,original.data(),1,references);
  assert(!frame.tools59c10[0].fingers55.count&&ToolAttachment59c10(self.data(),attachment.data())&&!socketProof28.fingers55.count);
  --ref.sample;++ref.acquisition.graspKey;BuildToolPoses59c10(frame,original.data(),1,references);
  assert(!frame.tools59c10[0].fingers55.count&&ToolAttachment59c10(self.data(),attachment.data())&&!socketProof28.fingers55.count);
  graspBinding.fingers55=paired;
  graspBinding.grip55=socketProof28.grip55;graspBinding.gripReady55=true;
  original[0].t[0]=1.5f;BuildToolPoses59c10(frame,original.data(),1);
  assert(frame.tools59c10[0].valid&&std::abs(frame.tools59c10[0].model.t[0]-2.1f)<.00001f);
  QuatT28 exitRelative{};exitRelative.t[0]=.3f;Put(attachment,0x9C,exitRelative);
  assert(ToolAttachment59c10(self.data(),attachment.data())&&std::abs(lastWanted[4]-12.1f)<.00001f);
  // D40 reproduces the first callback after native cleanup. The SAME current
  // attached resource already owns its canonical grip, but the intervening
  // committed packet legitimately has no tool transaction. No new original
  // joint sample is required to compose that known grip on the current wrist.
  {const auto savedToolD40=frame.tools59c10[0];const auto savedLogicD40=graspBinding.logic;
   frame.tools59c10[0]={};graspBinding.logic=0;
   assert(ToolAttachment59c10(self.data(),attachment.data())&&std::abs(lastWanted[4]-12.1f)<.00001f);
   const auto beforeD40=writes;graspBinding.place=false;
   assert(!ToolAttachment59c10(self.data(),attachment.data())&&writes==beforeD40);
   graspBinding.place=true;tracking=false;
   assert(!ToolAttachment59c10(self.data(),attachment.data())&&writes==beforeD40);tracking=true;
   graspBinding.grip55.p[0]=.81f;
   assert(!ToolAttachment59c10(self.data(),attachment.data())&&writes==beforeD40);graspBinding.grip55.p[0]=.1f;
   frame.tools59c10[0]=savedToolD40;graspBinding.logic=savedLogicD40;
  }
  // A captured native grip is authoritative even when an old saved clearance
  // is nonzero. Both prop and fingers remain on the same canonical transform.
  kcd2vr::grasp13::setPresentation35(.08f,.28f);
  assert(ToolAttachment59c10(self.data(),attachment.data())&&std::abs(lastWanted[4]-12.1f)<.00001f);
  worldScale35=2;
  assert(ToolAttachment59c10(self.data(),attachment.data())&&std::abs(lastWanted[4]-14.2f)<.00001f);
  worldScale35=1;kcd2vr::grasp13::setPresentation35(.02f,.28f);
  BuildToolPoses59c10(frame,original.data(),1);
  assert(std::abs(frame.tools59c10[0].model.t[0]-2.1f)<.00001f);
  graspBinding=saved;frame=savedFrame;original=savedOriginal;Put(attachment,0x9C,QuatT28{});
 }
 // C55: the real current attached ordinary item may follow the committed VR
 // wrist while its SAME native Picking temporarily blocks gameplay. Exercise
 // the actual attachment writer, original-joint join and final mutation gate.
 {const auto savedBinding=graspBinding;const auto savedFrame=frame;
  const auto savedContext=g_tool_context19c4r2.load();
  acquisition55.character=graspBinding.character;acquisition55.action=0x1234;acquisition55.scope=0x2345;
  acquisition55.serial=14;acquisition55.epoch=graspBinding.epoch;acquisition55.startedAt=now-20;
  acquisition55.tick=now;acquisition55.side=graspBinding.side;acquisition55.nativeToken=55;acquisition55.active=true;
  acquisition55.graspToken=graspBinding.token;acquisition55.graspKey=graspBinding.key;
  acquisition55.graspItem=graspBinding.item;acquisition55.graspEntity=graspBinding.entity;
  acquisitionAvailable55=true;(void)ToolContext19c4r2();
  const bool witnessReady=kcd2vr::activity_input163::witness175.ready.exchange(true);
  g_gameplay_allowed=false;g_dialogue_body178r9=false;frame.stamp.sample=99;
  const int before=writes;
  assert(GraspAcquisitionVisual55(graspBinding,now));
  assert(ToolAttachment59c10(self.data(),attachment.data())&&writes==before+1);
  assert(!g_gameplay_allowed); // pose permission never enables gameplay
  auto refusal=[&](){const int w=writes;assert(!ToolAttachment59c10(self.data(),attachment.data())&&writes==w);};
  acceptVisualPacket55=false;refusal();acceptVisualPacket55=true; // typed body publication unavailable
  acquisition55.tick=now-81;refusal();acquisition55.tick=now;
  acquisition55.tick=now+1;refusal();acquisition55.tick=now;
  acquisition55.active=false;refusal();acquisition55.active=true; // queued is not active
  acquisition55.scope=0;refusal();acquisition55.scope=0x2345;
  ++acquisition55.graspToken;refusal();--acquisition55.graspToken;
  ++acquisition55.graspKey;refusal();--acquisition55.graspKey;
  ++acquisition55.graspItem;refusal();--acquisition55.graspItem;
  ++acquisition55.graspEntity;refusal();--acquisition55.graspEntity;
  ++acquisition55.epoch;refusal();--acquisition55.epoch;
  acquisition55.side=1;refusal();acquisition55.side=0;
  acquisition55.character=123;refusal();acquisition55.character=graspBinding.character;
  frame.stamp.epoch=8;refusal();frame.stamp.epoch=7;
  frame.stamp.tick=now-81;refusal();frame.stamp.tick=now;
  g_ctx_menu=true;refusal();g_ctx_menu=false;
  g_ctx_dialogue=true;refusal();g_ctx_dialogue=false;
  g_ctx_cutscene=true;refusal();g_ctx_cutscene=false;
  g_ctx_reading=true;refusal();g_ctx_reading=false;
  animation_filter::nativeBodyOwned112=true;refusal();animation_filter::nativeBodyOwned112=false;
  animation_filter::kick175=true;refusal();animation_filter::kick175=false;
  kcd2vr::activity_input163::witness175.active[unsigned(kcd2vr::activity_input163::Kind175::Alchemy)]=true;
  refusal();kcd2vr::activity_input163::witness175.active[unsigned(kcd2vr::activity_input163::Kind175::Alchemy)]=false;
  (void)ToolContext19c4r2(false,true);refusal();(void)ToolContext19c4r2();
  g_tool_context19c4r2=((now-81)<<8);refusal();(void)ToolContext19c4r2();
  graspBinding.attached=graspBinding.place=graspBinding.socketOnly28=false;refusal();graspBinding=savedBinding;
  // A pending native socket is observation only, even with an exact Picking
  // permit and a correlated original pose. It cannot publish an item transform.
  graspBinding.attached=graspBinding.place=false;graspBinding.socketOnly28=true;frame.tools59c10[0]={};
  const int pendingWrites=writes;const unsigned pendingProofs=socketProofs28;
  assert(!ToolAttachment59c10(self.data(),attachment.data())&&writes==pendingWrites);
  BuildToolPoses59c10(frame,original.data(),1);assert(frame.tools59c10[0].valid);
  assert(!ToolAttachment59c10(self.data(),attachment.data())&&writes==pendingWrites&&socketProofs28==pendingProofs+1);
  graspBinding=savedBinding;frame=savedFrame;frame.stamp.sample=99;
  // A changed current item or newly opened UI during native attachment reads
  // must still lose at the final gate, not inherit the earlier permit.
  onAttachmentWorld55=[](){++graspBinding.key;};refusal();onAttachmentWorld55=nullptr;graspBinding=savedBinding;
  onAttachmentWorld55=[](){g_ctx_menu=true;};refusal();onAttachmentWorld55=nullptr;g_ctx_menu=false;
  onAttachmentWorld55=[](){now+=81;};refusal();onAttachmentWorld55=nullptr;now-=81;
  assert(ToolAttachment59c10(self.data(),attachment.data()));
  acquisitionAvailable55=false;acquisition55={};g_gameplay_allowed=true;g_dialogue_body178r9=false;
  kcd2vr::activity_input163::witness175.ready=witnessReady;
  graspBinding=savedBinding;frame=savedFrame;g_tool_context19c4r2=savedContext;
  socketProofs28=0;socketProof28={};
 }
 // Dirty real-world pickup discovery is separate from HELD/place. Even after
 // the actual next-original-pose join it may publish a proof, never item writes.
 {const auto saved=graspBinding;const auto savedFrame=frame;const int before=writes;socketProofs28=0;
  graspBinding.attached=graspBinding.place=false;graspBinding.socketOnly28=true;++graspBinding.token;
  frame.tools59c10[0]={};frame.stamp.sample=100;Put(attachment,0x7A,std::uint8_t(1));
  assert(!ToolAttachment59c10(self.data(),attachment.data())&&writes==before&&socketProofs28==0);
  BuildToolPoses59c10(frame,original.data(),1);assert(frame.tools59c10[0].valid);
  assert(std::abs(frame.tools59c10[0].model.t[0]-2.1f)<.00001f); // discovery never gets clearance/place capability
  assert(!ToolAttachment59c10(self.data(),attachment.data())&&writes==before&&socketProofs28==1);
  assert(socketProof28.binding.token==graspBinding.token&&socketProof28.sample==100&&
      socketProof28.self==reinterpret_cast<std::uintptr_t>(self.data())&&socketProof28.joint==0&&socketProof28.gripReady55);
  // Revocation/replacement between the two stages cannot inherit old proof.
  const auto pending=graspBinding;graspBinding.socketOnly28=false;
  assert(!ToolAttachment59c10(self.data(),attachment.data())&&writes==before&&socketProofs28==1);
  graspBinding=pending;++graspBinding.token;
  assert(!ToolAttachment59c10(self.data(),attachment.data())&&writes==before&&socketProofs28==1);
  graspBinding=saved;frame=savedFrame;
 }
 // No animated child is required to validate the actual entity world transform.
 assert(receivedPlacement.binding.token==700);
 g_right_grip=.1f;assert(ToolAttachment59c10(self.data(),attachment.data())); // release owner, not renderer, detaches
 graspBinding.place=false;assert(!ToolAttachment59c10(self.data(),attachment.data()));
 graspBinding.place=true;graspBinding.token=701;assert(!ToolAttachment59c10(self.data(),attachment.data())); // replaced token cannot reuse pose
 // Owning another object must not enable old F5 tool writes for this one.
 graspBinding.entity=77;held[0]=42;ws::heldToolActivityTick59c10=now;ws::heldToolActivity59c10=1;
 g_native_use_until107[0]=now+100;g_native_use_target59c10[0]=42;
 const int beforeOther=writes;assert(!ToolAttachment59c10(self.data(),attachment.data())&&writes==beforeOther);
 auto legacySocket=frame.tools59c10[0].socket;legacySocket.nativeGrasp13=false;
 legacySocket.request=ws::experimentEpoch36.load();g_tool_socket59c10[0].publish(legacySocket);
 auto legacyFrame=frame;legacyFrame.tools59c10[0]={};BuildToolPoses59c10(legacyFrame,original.data(),1);
 assert(!legacyFrame.tools59c10[0].valid);

 // Temporary resource-left and mortar-left/pestle-right attachments do not
 // publish matching ordinary held types for every hand.
 g_graspBinding13=nullptr;held[0]=held[1]=0;cr::f5Rung=2;ws::setExperiment36(true);
 ws::heldToolActivity59c10=1;ws::heldToolActivityTick59c10=now;alchemyActive=true;g_right_grip=g_left_grip=1;
 Put(alchemy,0,std::uintptr_t(0x3F60B08));Put(alchemy,0x18,reinterpret_cast<std::uintptr_t>(alchemyPlayer.data()));
 Put(alchemy,0x2C,std::uint8_t(1));Put(alchemyPlayer,0,std::uintptr_t(0x3AA4B98));
 Put(alchemy,0x78,std::uintptr_t(0x3BFA5B0));alchemySlots[1]=reinterpret_cast<std::uintptr_t>(alchemyAction.data());
 Put(alchemy,0xB0,reinterpret_cast<std::uintptr_t>(alchemySlots.data()));Put(alchemy,0xB8,reinterpret_cast<std::uintptr_t>(alchemySlots.data()+2));
 Put(alchemyAction,0,std::uintptr_t(0x3A5C670));Put(alchemyAction,0x28,std::uint8_t(1));Put(alchemyAction,0x3C,int(1));
 Put(alchemyAction,0x40,reinterpret_cast<std::uintptr_t>(alchemy.data()+0x78));Put(alchemyAction,0x58,reinterpret_cast<std::uintptr_t>(alchemy.data()));
 Put(alchemyAction,0x78,reinterpret_cast<std::uintptr_t>(alchemyChild19.data()));
 Put(alchemyChild19,0,std::uintptr_t(0x3A47948));Put(alchemyChild19,0x268,reinterpret_cast<std::uintptr_t>(alchemy.data()));
 Put(alchemyChild19,0x28,int(2));Put(alchemyChild19,0x54,std::uint32_t(55));frame.stamp.sample=71;
 Put(alchemy,0x304,std::uint32_t(14));Put(alchemy,0x150+4*14,std::uint32_t(42));
 Put(alchemy,0x2A0,std::uint32_t(21));Put(alchemy,0x2A4,std::uint32_t(21));
 Put(self,0,std::uintptr_t(0x3A68F68));Put(attachment,0x10,reinterpret_cast<std::uintptr_t>("entity_left"));
 Put(attachment,0x20,reinterpret_cast<std::uintptr_t>(self.data()));
 frame.valid[1]=true;frame.native[1]=frame.native[0];frame.allowed[1]=frame.allowed[0];frame.tools59c10[1]={};frame.stamp.tick=now;
 g_gameplay_allowed=false;assert(!ToolAttachment59c10(self.data(),attachment.data()));
 namespace ai=kcd2vr::activity_input163;ai::witness175.ready=true;
 ai::witness175.Begin(ai::Kind175::Alchemy,reinterpret_cast<std::uintptr_t>(alchemy.data()));
 // No retained ordinary packet may bootstrap a workstation tool. The actual
 // Mod body selector does not depend on a tool socket or committed packet.
 const auto nextAlchemyFrame=frame;frame=CommittedFrame6695{};bool bodyAdmitted=false;
 auto activityContext=ToolContext19c4r2(true,false,false,false,false,false,false,false,&bodyAdmitted);
 assert(bodyAdmitted&&activityContext.menu&&activityContext.workstation_controls_only);
 g_dialogue_body178r9=bodyAdmitted;const int bootWrites=writes;
 assert(!ToolAttachment59c10(self.data(),attachment.data())&&writes==bootWrites);
 frame=nextAlchemyFrame;frame.tools59c10[0]={};frame.tools59c10[1]={};frame.stamp.tick=now-81;
 assert(!ToolAttachment59c10(self.data(),attachment.data())&&writes==bootWrites);
 // Only a NEW valid committed wrist reaches the native attachment discovery.
 // The actual carafe must bootstrap even if the native dirty flag is set on
 // every callback, without bypassing its real binding or original-pose join.
 Put(attachment,0x7A,std::uint8_t(1));
 frame.stamp.tick=now;frame.stamp.sample=72;
 assert(!ToolAttachment59c10(self.data(),attachment.data())&&writes==bootWrites);
 BuildToolPoses59c10(frame,original.data(),1);assert(frame.tools59c10[1].valid);
 assert(ToolAttachment59c10(self.data(),attachment.data())&&writes==bootWrites+1);
 frame.tools59c10[1]={}; // subsequent cases start with uncorrelated tool again
 calibrated19c4r2=false;(void)ToolContext19c4r2(true,false,false,false,false,false,false,false,&bodyAdmitted);assert(!bodyAdmitted);
 calibrated19c4r2=true;census19c4r2=0;(void)ToolContext19c4r2(true,false,false,false,false,false,false,false,&bodyAdmitted);assert(!bodyAdmitted);
 census19c4r2=3;(void)ToolContext19c4r2(true,false,false,false,false,false,false,false,&bodyAdmitted);assert(bodyAdmitted);
 assert(!g_gameplay_allowed&&WorkstationToolPose18(now));
 // Fresh real-menu policy wins before the preceding gameplay/body snapshots
 // change: neither the old body permission nor old gameplay=true may allow it.
 for(int nativeControls=0;nativeControls<2;++nativeControls)for(int reason=0;reason<7;++reason){
  // First publish an admitted activity; then publish a new real-UI owner while
  // all old body/general gameplay fields deliberately remain unchanged.
  (void)ToolContext19c4r2(true);
  (void)ToolContext19c4r2(nativeControls!=0,reason==0,reason==1,reason==2,reason==3,reason==4,reason==5,reason==6);
  assert(!WorkstationToolPose18(now));
  const int before=writes;assert(!ToolAttachment59c10(self.data(),attachment.data())&&writes==before);
  {g_gameplay_allowed=true;
   assert(!ToolAttachment59c10(self.data(),attachment.data())&&writes==before);g_gameplay_allowed=false;}
 }
 (void)ToolContext19c4r2(true);assert(WorkstationToolPose18(now));
 now+=81;assert(!WorkstationToolPose18(now));now-=81;
 g_tool_context19c4r2=0;assert(!WorkstationToolPose18(now));(void)ToolContext19c4r2(true);
 // A callback cannot reuse a future publication or a prior session's body frame.
 ++now;(void)ToolContext19c4r2(true);--now;assert(!WorkstationToolPose18(now));(void)ToolContext19c4r2(true);
 ai::trackedHands59c7f[0]=false;assert(!WorkstationToolPose18(now));ai::trackedHands59c7f[0]=true;
 {auto saved=graspBinding;graspBinding={frame.stamp.actor,11,12,13,700,42,frame.stamp.epoch,42,99,0,true,true};
  g_graspBinding13=&ReadGrasp;const int w=writes;assert(!ToolAttachment59c10(self.data(),attachment.data())&&writes==w);g_graspBinding13=nullptr;graspBinding=saved;}
 {held[0]=42;alchemyActive=false;const int w=writes;assert(!ToolAttachment59c10(self.data(),attachment.data())&&writes==w);held[0]=0;alchemyActive=true;}
 g_dialogue_body178r9=false;{const int w=writes;assert(!ToolAttachment59c10(self.data(),attachment.data())&&writes==w);}g_dialogue_body178r9=true;
 const int beforeAlchemy=writes;assert(!ToolAttachment59c10(self.data(),attachment.data()));
 assert(NativeAlchemyFingers18(frame.stamp.actor,frame.stamp.epoch,1,now));
 BuildToolPoses59c10(frame,original.data(),1);assert(frame.tools59c10[1].valid);
 assert(ToolAttachment59c10(self.data(),attachment.data())&&writes==beforeAlchemy+1);
 {kcd2vr::alchemy_stage19c4::Proof proof{};assert(kcd2vr::alchemy_stage19c4::attached.read(proof));
  assert(proof.child==reinterpret_cast<std::uintptr_t>(alchemyChild19.data())&&proof.token==55&&proof.sample==72&&
      proof.binding.action==reinterpret_cast<std::uintptr_t>(alchemyAction.data())&&proof.binding.side==1);}
 Put(attachment,0x7A,std::uint8_t(1));
 assert(ToolAttachment59c10(self.data(),attachment.data())&&writes==beforeAlchemy+2);
 const auto oldLifetime=frame.tools59c10[1].socket.alchemy18.lifetime;
 ai::witness175.End(reinterpret_cast<std::uintptr_t>(alchemy.data()));ai::witness175.Begin(ai::Kind175::Alchemy,reinterpret_cast<std::uintptr_t>(alchemy.data()));
 assert(ai::witness175.lifetime175.load()!=oldLifetime);
 assert(!NativeAlchemyFingers18(frame.stamp.actor,frame.stamp.epoch,1,now));
 const int beforeRestart=writes;assert(!ToolAttachment59c10(self.data(),attachment.data())&&writes==beforeRestart);
 Put(attachment,0x7A,std::uint8_t(0)); // restarted action must acquire a clean socket again
 assert(!ToolAttachment59c10(self.data(),attachment.data())&&writes==beforeRestart);
 BuildToolPoses59c10(frame,original.data(),1);assert(frame.tools59c10[1].socket.alchemy18.lifetime!=oldLifetime);
 assert(ToolAttachment59c10(self.data(),attachment.data())&&writes==beforeRestart+1);
 ai::witness175.transitionDepth175=1;assert(!ToolAttachment59c10(self.data(),attachment.data()));ai::witness175.transitionDepth175=0;
 Put(attachment,0x20,reinterpret_cast<std::uintptr_t>(entity.data()));assert(!ToolAttachment59c10(self.data(),attachment.data()));
 Put(attachment,0x20,std::uintptr_t(0));assert(!ToolAttachment59c10(self.data(),attachment.data()));Put(attachment,0x20,reinterpret_cast<std::uintptr_t>(self.data()));
 g_left_grip=.1f;assert(!ToolAttachment59c10(self.data(),attachment.data()));g_left_grip=1;
 // Action slot stopped/replaced, wrong native owner or wrong side cannot qualify.
 diagnostics::capture168::gate.select(diagnostics::capture168::Domain::Interaction,true,now);
 alchemySlots[1]=0;assert(!ToolAttachment59c10(self.data(),attachment.data()));
 assert(receipts.back().find("ALCHEMY_ACTION_OR_CURRENT_ATTACHMENT_UNQUALIFIED")!=std::string::npos);
 alchemySlots[1]=reinterpret_cast<std::uintptr_t>(alchemyAction.data());
 Put(alchemyAction,0x28,std::uint8_t(0));assert(!ToolAttachment59c10(self.data(),attachment.data()));Put(alchemyAction,0x28,std::uint8_t(1));
 Put(alchemyAction,0x58,std::uintptr_t(0));assert(!ToolAttachment59c10(self.data(),attachment.data()));Put(alchemyAction,0x58,reinterpret_cast<std::uintptr_t>(alchemy.data()));
 Put(attachment,0x10,reinterpret_cast<std::uintptr_t>("entity_right"));assert(!ToolAttachment59c10(self.data(),attachment.data()));
 Put(attachment,0x10,reinterpret_cast<std::uintptr_t>("entity_left"));
 Put(alchemy,0x304,std::uint32_t(2));assert(!ToolAttachment59c10(self.data(),attachment.data()));Put(alchemy,0x304,std::uint32_t(14));
 Put(alchemy,0x150+4*14,std::uint32_t(43));assert(!ToolAttachment59c10(self.data(),attachment.data()));Put(alchemy,0x150+4*14,std::uint32_t(42));
 Put(self,8,std::uint32_t(43));assert(!ToolAttachment59c10(self.data(),attachment.data()));Put(self,8,std::uint32_t(42));
 Put(alchemy,0x2C,std::uint8_t(0));assert(!ToolAttachment59c10(self.data(),attachment.data()));Put(alchemy,0x2C,std::uint8_t(1));
 Put(alchemy,0x18,std::uintptr_t(0));assert(!ToolAttachment59c10(self.data(),attachment.data()));Put(alchemy,0x18,reinterpret_cast<std::uintptr_t>(alchemyPlayer.data()));
 // Mortar action publishes hand0=type2, but attaches mortar LEFT/pestle RIGHT.
 Put(alchemyAction,0,std::uintptr_t(0x3F60770));Put(alchemy,0x2A0,std::uint32_t(2));Put(alchemy,0x2A4,std::uint32_t(21));
 Put(alchemy,0x158,std::uint32_t(42));Put(alchemy,0x15C,std::uint32_t(42));
 assert(!ToolAttachment59c10(self.data(),attachment.data()));BuildToolPoses59c10(frame,original.data(),1);
 assert(frame.tools59c10[1].valid&&ToolAttachment59c10(self.data(),attachment.data()));
 assert(frame.tools59c10[1].socket.alchemy18.type==2);
 Put(alchemy,0x158,std::uint32_t(43)); // right-hand entity42 is the pestle, not mortar43
 Put(attachment,0x10,reinterpret_cast<std::uintptr_t>("entity_right"));frame.tools59c10[0]={};
 assert(!ToolAttachment59c10(self.data(),attachment.data()));BuildToolPoses59c10(frame,original.data(),1);
 assert(frame.tools59c10[0].valid&&ToolAttachment59c10(self.data(),attachment.data()));assert(frame.tools59c10[0].socket.alchemy18.type==3);
 assert(!NativeAlchemyFingers18(frame.stamp.actor+1,frame.stamp.epoch,0,now));
 assert(!NativeAlchemyFingers18(frame.stamp.actor,frame.stamp.epoch+1,0,now));
 // Ordinary membership survives the completed TakeObject action. No current
 // director action or selected verb is required for an already-held native tool.
 alchemySlots[1]=0;Put(alchemyAction,0x28,std::uint8_t(0));
 Put(alchemy,0x2A0,std::uint32_t(0));Put(alchemy,0x150,std::uint32_t(42));
 assert(!ToolAttachment59c10(self.data(),attachment.data()));BuildToolPoses59c10(frame,original.data(),1);
 assert(ToolAttachment59c10(self.data(),attachment.data()));
 assert(frame.tools59c10[0].socket.alchemy18.action==0&&frame.tools59c10[0].socket.alchemy18.type==0);
 Put(alchemy,0x2A0,std::uint32_t(21));assert(!ToolAttachment59c10(self.data(),attachment.data()));
 Put(alchemy,0x2A0,std::uint32_t(0));Put(self,8,std::uint32_t(43));assert(!ToolAttachment59c10(self.data(),attachment.data()));Put(self,8,std::uint32_t(42));
 now+=81;assert(!ToolAttachment59c10(self.data(),attachment.data()));
 assert(!NativeAlchemyFingers18(frame.stamp.actor,frame.stamp.epoch,0,now));
 // A new context alone cannot refresh a stale committed pose / native activity.
 (void)ToolContext19c4r2(true);assert(!ToolAttachment59c10(self.data(),attachment.data()));

 // C37C1: exercise the ACTUAL placement include with an explicit reader mock.
 // Separate smith_tool36_test exercises the production memory reader itself.
 ai::witness175.End(reinterpret_cast<std::uintptr_t>(alchemy.data()));alchemyActive=false;
 ai::witness175.Begin(ai::Kind175::Smithing,999);
 ws::heldToolActivity59c10=int(ws::Kind::Anvil);ws::heldToolActivityTick59c10=now;
 ws::setSmithAutoHold36(true);ws::setExperiment36(true);cr::f5Rung=2;
 (void)ToolContext19c4r2(true);g_gameplay_allowed=false;g_dialogue_body178r9=true;
 frame.stamp.tick=now;frame.valid[0]=true;frame.tools59c10[0]={};frame.tools59c10[1]={};
 g_henry_char_attachment_manager=manager.data();g_right_grip=0;held[0]=0;
 Put(attachment,0x7A,std::uint8_t(0));Put(attachment,0x20,reinterpret_cast<std::uintptr_t>(self.data()));
 smithBinding37.instance=999;smithBinding37.player=888;smithBinding37.character=frame.stamp.actor;
 smithBinding37.attachment=reinterpret_cast<std::uintptr_t>(attachment.data());
 smithBinding37.object=reinterpret_cast<std::uintptr_t>(self.data());smithBinding37.holder=777;
 smithBinding37.entity=42;smithBinding37.side=0;smithBinding37.tool=0;smithBinding37.phase=11;smithBinding37.lifetime=88;
 smithActive37=true;g_phys_relevant_id101[0]=42;
 const auto beforeSmith37=writes;
 assert(!ToolAttachment59c10(self.data(),attachment.data()));
 BuildToolPoses59c10(frame,original.data(),1);
 assert(frame.tools59c10[0].valid&&ToolAttachment59c10(self.data(),attachment.data())&&writes==beforeSmith37+1);
 assert(NativeSmithFingers36(frame.stamp.actor,frame.stamp.epoch,0,now));
 // Drop of Grip has no effect, but detach, stale stage and settings must stop it.
 smithActive37=false;assert(!ToolAttachment59c10(self.data(),attachment.data())&&writes==beforeSmith37+1);
 smithActive37=true;smithBinding37.phase=14; // real native raise acknowledgement keeps the exact grip
 assert(ToolAttachment59c10(self.data(),attachment.data()));
 BuildToolPoses59c10(frame,original.data(),1);
 assert(ToolAttachment59c10(self.data(),attachment.data()));
 ws::setSmithAutoHold36(false);assert(!ToolAttachment59c10(self.data(),attachment.data()));
 ws::setSmithAutoHold36(true);assert(!ToolAttachment59c10(self.data(),attachment.data()));
 BuildToolPoses59c10(frame,original.data(),1);assert(ToolAttachment59c10(self.data(),attachment.data()));
 tracking=false;assert(!ToolAttachment59c10(self.data(),attachment.data()));tracking=true;
 // Classification alone is never enough: wrong object must not cross the weapon exclusion.
 smithBinding37.object=123;const int stoppedWrites37=writes;
 assert(!ToolAttachment59c10(self.data(),attachment.data())&&writes==stoppedWrites37);
 std::puts("PASS actual F5 tool placement: original socket, exact Picking visual permit with gameplay OFF, PendingAttach observation only, UI/native-owner/tracking/item/epoch refusal, final stale/replacement gate, unchanged ordinary held and workstation routes");
 return 0;
}

#endif
