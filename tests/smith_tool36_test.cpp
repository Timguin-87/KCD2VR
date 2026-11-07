// C37: smithing tool membership reader (pure, fake memory image) and the ACTUAL
// production tool_attachment59c10.inl smithing auto-hold path with native leaves
// doubled. Host validation only: no game calls, no claim to emulate WHGame ABI.
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
namespace st=kcd2vr::smith_tool36;
struct QuatT28 {float q[4]{0,0,0,1},t[3]{};};
struct ToolSocket59c10 {void*self{},*attachment{},*character{},*entity{};std::uint32_t id{};int side=-1,joint=-1;QuatT28 relative{};std::uint64_t epoch{},tick{},request{};bool nativeGrasp13=false;kcd2vr::alchemy_tool18::Binding alchemy18{};st::Binding smith36{};QuatT28 grip67{};bool gripReady67=false;std::uint64_t modeEpoch67=0;};
struct ToolPose59c10 {ToolSocket59c10 socket{};QuatT28 model{};bool valid=false;kcd2vr::grasp13::FingerReference55 fingers55{};};
struct CommittedFrame6695 {pc::Stamp stamp{};QuatT28 native[2]{},allowed[2]{},propRequested5916c1[2]{};float world[8]{0,0,0,1,0,0,0,1};bool valid[2]{},propRequestedValid5916c1[2]{};ToolPose59c10 tools59c10[2]{};std::uint64_t doorTrackingEpoch66=0;};
static kcd2vr::core::SnapshotMailbox<ToolSocket59c10> g_tool_socket59c10[2];
static kcd2vr::core::SnapshotMailbox<kcd2vr::subsystems::smithing::contact68::HammerFrame> g_smith_placed68;
static std::uint64_t now=5000;
static std::uint64_t GetTickCount64(){return now;}
static std::uintptr_t WhBase(){return 0;}
namespace retail_build112 {static bool ready=true;static bool Ready(std::uintptr_t){return ready;}}
namespace diagnostics { namespace capture168=kcd2vr::diagnostics::capture168; }
namespace diagnostics::lab {static bool reference=false;static bool NativeReference(){return reference;}}
static std::vector<std::string> receipts;
namespace diagnostic_trace {static void Write(const char* text){receipts.emplace_back(text);}}
static std::atomic<bool>g_gameplay_allowed{false},g_pose_restore_fault6689{false};
static std::atomic<bool>g_dialogue_body178r9{false},g_ctx_dialogue{false},g_ctx_menu{false},g_ctx_cutscene{false},g_ctx_reading{false};
namespace animation_filter {static std::atomic<bool>nativeBodyOwned112{false};static bool NativeKickActive175(){return false;}}
// Ordinary Picking is absent in these smithing cases; the shared production
// attachment code must never receive an invented ordinary-item owner here.
namespace kcd2vr::action_time_owner59c11 {
 bool ReadAcquisition54(grasp13::Acquisition54& out) noexcept {out={};return false;}
}
namespace action_time_owner59c11=kcd2vr::action_time_owner59c11;
static std::atomic<std::uint64_t>g_tool_context19c4r2{0};
static std::atomic<std::uint64_t>g_pose_epoch6689{7}; // copied native hand witness epoch
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
static std::atomic<std::uint64_t>g_native_use_serial107[2]{},g_native_wrist_retired107[2]{};
static std::atomic<std::uint32_t>g_native_use_target59c10[2]{};
static std::atomic<float>g_left_grip{0},g_right_grip{0}; // C37: NO Grip anywhere unless a case sets it
static std::uint32_t held[2]{};
static bool tracking=true,placement=true,identity=true;
static int writes=0;
static CommittedFrame6695 frame{};
static std::array<unsigned char,0x100>attachment{},attachmentL{},attachmentE{},manager{},npcManager{},character{},npcCharacter{};
static std::array<unsigned char,0x100>hammerObject{},tongsObject{},workpieceObject{},entityStore{};
struct ReadScope101R2{};
template<class T>static bool Read(const void*p,T& out){if(!p)return false;std::memcpy(&out,p,sizeof(out));return true;}
template<class M,class T>static void Put(M& memory,std::size_t offset,const T& v){assert(offset+sizeof(v)<=memory.size());std::memcpy(memory.data()+offset,&v,sizeof(v));}
static bool NativeCandidateItemId6699(int s,std::uint32_t& id){id=held[s];return id!=0;}
static bool NativeHeldItemId6694(int s,std::uint32_t& id){id=held[s];return id!=0;}
static bool TrackingHeadUsable111(std::uint64_t){return tracking;}
static bool TrackingHandUsable111(int,std::uint64_t){return tracking;}
static bool ReadCommittedPacket6695(CommittedFrame6695& out){out=frame;return now>=frame.stamp.tick&&now-frame.stamp.tick<=80;}
static bool ReadCommittedPacketFor6695(CommittedFrame6695& out,bool workstation,
    const kcd2vr::grasp13::Acquisition54* picking55=nullptr){
 if(picking55)return false; // this fixture has no ordinary Picking service
 if(!g_gameplay_allowed.load()&&!(workstation&&g_dialogue_body178r9.load()))return false;return ReadCommittedPacket6695(out);}
static bool PhysVft6698(void*p,std::uintptr_t expected){std::uintptr_t v=0;return Read(p,v)&&v==expected;}
static bool PhysSocketContract101(std::uintptr_t){return true;}
static void*EntityById6694(std::uint32_t id){return id>=42&&id<=45?entityStore.data()+id:nullptr;}
struct ModelWorld6692 {ph::Pose pose{};float scale=1;};
static bool PhysAttachmentWorld164(void*,std::uintptr_t,ModelWorld6692& x){x={};x.pose.p={10,20,30};return true;}
static bool ReadAttachmentQs6689(void*,float* out){out[7]=1;return true;}
static ph::Pose PhysPose6698(const QuatT28& t){return {{t.q[0],t.q[1],t.q[2],t.q[3]},{t.t[0],t.t[1],t.t[2]}};}
static ph::Pose PhysCurrentWorld164(ph::Pose p,const ModelWorld6692& x){p.p=p.p*x.scale+x.pose.p;return p;}
static bool PhysEntityIdentity135(void*p,std::uint32_t id){return identity&&p==EntityById6694(id);}
static float lastWanted[8]{};static void* lastPlaced=nullptr;
static bool PlaceBothConsumers6689(void* self,void*,const float* wanted,bool& child){if(!placement)return false;++writes;lastPlaced=self;std::memcpy(lastWanted,wanted,sizeof(lastWanted));child=true;return true;}
static bool PlacementReadback6689(void*,void*,std::uint32_t,const float*,bool& checked){checked=true;return true;}
static pc::Vec3 FromT(const QuatT28& t){return {t.t[0],t.t[1],t.t[2]};}
static double Len(pc::Vec3 p){return pc::length(p);}
static QuatT28 Make(pc::Quat q,pc::Vec3 p){return {{float(q.x),float(q.y),float(q.z),float(q.w)},{float(p.x),float(p.y),float(p.z)}};}
static pc::Quat Q(const QuatT28& t){return {t.q[0],t.q[1],t.q[2],t.q[3]};}
static QuatT28 AbsoluteFromRelative(const QuatT28&a,const QuatT28&b){return Make(pc::multiply(Q(a),Q(b)),FromT(a)+pc::rotate(Q(a),FromT(b)));}
static QuatT28 RelativeFromAbsolute(const QuatT28&a,const QuatT28&b){const auto qa=Q(a);const pc::Quat inv{-qa.x,-qa.y,-qa.z,qa.w};return Make(pc::multiply(inv,Q(b)),pc::rotate(inv,FromT(b)-FromT(a)));}
static bool ValidPose6689(const QuatT28&t) noexcept {return ph::finite(PhysPose6698(t));}
static bool (*g_graspBinding13)(kcd2vr::grasp13::Binding&) noexcept=nullptr;
static void (*g_graspSocket28)(const kcd2vr::grasp13::SocketProof28&) noexcept=nullptr;
static void (*g_graspPlaced13)(const kcd2vr::grasp13::Placement&) noexcept=nullptr;
static bool ReadAlchemyTool18(void*,std::uint32_t,std::uintptr_t,kcd2vr::alchemy_tool18::Binding& out) noexcept {out={};return false;}
static std::atomic<void*> g_henry_char_attachment_manager{nullptr};

// ---- fake retail smithing image (WhBase()==0, so vft values are the RVAs) ----
static std::array<unsigned char,0x400> smith{},model{};
static std::array<unsigned char,0xA00> player{};
static std::array<unsigned char,0x300> slots{};
static std::array<unsigned char,0x40> hammer{},tongs{},workpiece{},forgeBag{};
static std::uintptr_t P(const void* p){return reinterpret_cast<std::uintptr_t>(p);}
static void Image(){
 Put(smith,0,std::uintptr_t(st::kInstanceVft));Put(smith,0x18,P(player.data()));Put(smith,0x2C,std::uint8_t(1));Put(smith,st::kModel,P(model.data()));
 Put(player,0,std::uintptr_t(st::kPlayerVft));Put(player,st::kPlayerEntity,std::uint32_t(900));Put(player,st::kPlayerSlots,P(slots.data()));
 Put(model,0,std::uintptr_t(st::kModelVft));
 Put(model,st::kHammerSlot,P(hammer.data()));Put(model,st::kTongsSlot,P(tongs.data()));Put(model,0x2F0,P(forgeBag.data()));Put(model,st::kWorkpieceSlot,P(workpiece.data()));
 const auto holder=[](auto& h,std::uintptr_t vft,int hand,std::uint32_t id,std::uint64_t key){
  Put(h,0,vft);Put(h,st::kHand,hand);Put(h,st::kHost,std::uint32_t(900));Put(h,st::kEntity,id);Put(h,st::kAttached,std::uint8_t(1));
  Put(h,st::kAssetSlot,std::uint8_t(0));Put(h,st::kKey,key);};
 holder(hammer,st::kSlotToolVft,0,42,0xAAAA000000000001ull);holder(tongs,st::kSlotToolVft,1,43,0xBBBB000000000002ull);
 holder(workpiece,st::kWorkpieceVft,1,44,0);holder(forgeBag,0x3A7BAA8,0,45,0);
 Put(slots,st::kHandKeys,0xAAAA000000000001ull);Put(slots,st::kHandKeys+8,0xBBBB000000000002ull);
 const auto att=[](auto& a,const char* name,auto& object,std::uint32_t id,void* mgr){
  Put(a,0,std::uintptr_t(st::kAttachmentVft));Put(a,0x10,P(name));Put(a,0x20,P(object.data()));Put(a,0x28,mgr);
  Put(object,0,std::uintptr_t(st::kObjectVft));Put(object,8,id);
  Put(a,0x78,std::int16_t(0));Put(a,0x7A,std::uint8_t(0));Put(a,0x9C,QuatT28{});};
 att(attachment,"hand_right",hammerObject,42,static_cast<void*>(manager.data()));
 att(attachmentL,"hand_left",tongsObject,43,static_cast<void*>(manager.data()));
 att(attachmentE,"entity_left",workpieceObject,44,static_cast<void*>(manager.data()));
 Put(manager,0x70,static_cast<void*>(character.data()));Put(npcManager,0x70,static_cast<void*>(npcCharacter.data()));
 Put(character,0,std::uintptr_t(0x40602F8));
}
static int phase36=11;
static void Phase(int phase){namespace ai=kcd2vr::activity_input163;phase36=phase;
 ai::witness175.smithSampleC1=(ai::witness175.smithEpochC1.load()<<8)|unsigned(phase<0?255:phase);}
// Same contract as the production wrapper (smith_tool36.inl) minus the retail pins.
static bool ReadSmithTool36(void* att,std::uint32_t id,std::uintptr_t ch,st::Binding& out) noexcept {
 namespace ai=kcd2vr::activity_input163;out={};
 if(!ai::witness175.ready.load()||ai::witness175.transitionDepth175.load())return false;
 const auto lifetime=ai::witness175.lifetime175.load();
 const auto instance=ai::witness175.active[unsigned(ai::Kind175::Smithing)].load();
 const int phase=ai::witness175.SmithPhaseC1();
 const auto rd=[](std::uintptr_t at,auto& value){return Read(reinterpret_cast<void*>(at),value);};
 if(!instance||phase<0||!st::read(rd,0,instance,P(player.data()),ch,P(att),id,phase,out))return false;
 if(lifetime!=ai::witness175.lifetime175.load()||instance!=ai::witness175.active[unsigned(ai::Kind175::Smithing)].load()||
    phase!=ai::witness175.SmithPhaseC1()){out={};return false;}
 out.lifetime=lifetime;return true;
}
// Native passenger owner is an explicit external leaf in this retained fixture.
static bool NativePassengerCurrentA34(std::uint64_t) noexcept {return false;}
#include "../src/vrik/tool_attachment59c10.inl"

static unsigned checks=0;static void ck(bool v,const char* what){++checks;if(!v){std::printf("FAIL %s\n",what);std::fflush(stdout);}assert(v);}
static bool pure(const void* att,std::uint32_t id,int phase,st::Binding& out,const void* ch=nullptr){
 const auto rd=[](std::uintptr_t at,auto& value){return Read(reinterpret_cast<void*>(at),value);};
 return st::read(rd,0,P(smith.data()),P(player.data()),P(ch?ch:character.data()),P(att),id,phase,out);}
static bool pureEvidence(const void* att,std::uint32_t id,int phase,st::Binding& out,st::ReadEvidenceI1& e){
 const auto rd=[](std::uintptr_t at,auto& value){return Read(reinterpret_cast<void*>(at),value);};
 return st::read(rd,0,P(smith.data()),P(player.data()),P(character.data()),P(att),id,phase,out,&e);}
static void PureReader(){
 st::Binding b{};
 ck(pure(attachment.data(),42,11,b)&&b.tool==0&&b.side==0&&b.key==0xAAAA000000000001ull&&b.holder==P(hammer.data())&&b.host==900,"hammer hand_right");
 ck(pure(attachmentL.data(),43,11,b)&&b.tool==1&&b.side==1&&b.holder==P(tongs.data()),"tongs hand_left");
 ck(pure(attachmentE.data(),44,11,b)&&b.tool==2&&b.side==1&&b.key==0&&b.holder==P(workpiece.data()),"sword workpiece entity_left");
 st::Binding again{};ck(pure(attachment.data(),42,11,again)&&pure(attachment.data(),42,11,b)&&st::same(again,b),"stable read is same");
 ck(pure(attachment.data(),42,14,b)&&!st::same(again,b),"phase change is a new stage binding");
 // Refusals: every exact field is required.
 ck(!pure(attachment.data(),43,11,b),"entity mismatch");
 ck(!pure(attachment.data(),42,-1,b)&&!pure(attachment.data(),42,21,b),"unknown phase");
 ck(!pure(attachment.data(),42,11,b,npcCharacter.data()),"NPC character");
 {Put(workpiece,st::kAssetSlot,std::uint8_t(1));ck(!pure(attachmentE.data(),44,11,b),"workpiece inside tongs Assetslot is not Henry's attachment");Put(workpiece,st::kAssetSlot,std::uint8_t(0));}
 {Put(attachmentE,0x10,P("entity_right"));Put(workpieceObject,8,std::uint32_t(45));ck(!pure(attachmentE.data(),45,11,b),"forge bag entity_right never binds");
  Put(attachmentE,0x10,P("entity_left"));Put(workpieceObject,8,std::uint32_t(44));}
 {Put(slots,st::kHandKeys,std::uint64_t(0));ck(!pure(attachment.data(),42,11,b),"hand slot cleared (DetachHammer/AttachOnly not done)");Put(slots,st::kHandKeys,0xAAAA000000000001ull);}
 {Put(slots,st::kHandKeys,0xAAAA000000000009ull);ck(!pure(attachment.data(),42,11,b),"other item in hand slot");Put(slots,st::kHandKeys,0xAAAA000000000001ull);}
 {Put(hammer,st::kAttached,std::uint8_t(0));ck(!pure(attachment.data(),42,11,b),"holder detached");Put(hammer,st::kAttached,std::uint8_t(1));}
 {Put(hammer,st::kHost,std::uint32_t(901));ck(!pure(attachment.data(),42,11,b),"holder host is not this player");Put(hammer,st::kHost,std::uint32_t(900));}
 {Put(hammer,st::kHand,1);ck(!pure(attachment.data(),42,11,b),"holder hand differs from attachment hand");Put(hammer,st::kHand,0);}
 {Put(hammer,0,std::uintptr_t(0x3A7BAA8));ck(!pure(attachment.data(),42,11,b),"forge-bag class in hammer slot");Put(hammer,0,std::uintptr_t(st::kSlotToolVft));}
 {Put(model,st::kHammerSlot,P(tongs.data()));ck(!pure(attachment.data(),42,11,b),"wrong holder in slot");Put(model,st::kHammerSlot,P(hammer.data()));}
 {Put(model,0,std::uintptr_t(0x1234));ck(!pure(attachment.data(),42,11,b),"model class");Put(model,0,std::uintptr_t(st::kModelVft));}
 {Put(smith,0x2C,std::uint8_t(0));ck(!pure(attachment.data(),42,11,b),"inactive minigame");Put(smith,0x2C,std::uint8_t(1));}
 {Put(smith,0,std::uintptr_t(0x3F60B08));ck(!pure(attachment.data(),42,11,b),"alchemy instance");Put(smith,0,std::uintptr_t(st::kInstanceVft));}
 {Put(player,st::kPlayerEntity,std::uint32_t(0));ck(!pure(attachment.data(),42,11,b),"player id unknown");Put(player,st::kPlayerEntity,std::uint32_t(900));}
 {Put(hammerObject,0,std::uintptr_t(0x3A68F00));ck(!pure(attachment.data(),42,11,b),"attachment object class");Put(hammerObject,0,std::uintptr_t(st::kObjectVft));}
 {Put(attachment,0x10,P("hand_rightx"));ck(!pure(attachment.data(),42,11,b),"attachment name");Put(attachment,0x10,P("hand_right"));}
 {Put(attachment,0x20,P(tongsObject.data()));ck(!pure(attachment.data(),42,11,b),"attachment carries another object");Put(attachment,0x20,P(hammerObject.data()));}
 // The headset run recognized the sword workpiece but not its real hammer.
 // Distinguish a station-link/runtime-entity mismatch from a stale hand WUID,
 // while keeping BOTH cases refused. Raw diagnostic fields are not ownership.
 st::ReadEvidenceI1 e{};
 {Put(hammer,st::kEntity,std::uint32_t(901));
  ck(!pureEvidence(attachment.data(),42,11,b,e)&&!b&&
     std::strcmp(e.refusal,"SMITH_LINKED_ENTITY_DIFFERS_FROM_BOUND_ENTITY")==0&&
     e.linkedEntity==901&&e.boundEntity==42&&e.holder==P(hammer.data())&&e.holderFields&&
     e.keysRead&&e.holderKey==e.handKey&&e.holderHand==0&&e.attached==1,
     "source entity mismatch remains refused and exposes the otherwise matching hand key");
  Put(hammer,st::kEntity,std::uint32_t(42));}
 {Put(slots,st::kHandKeys,0xAAAA000000000009ull);
  ck(!pureEvidence(attachment.data(),42,11,b,e)&&!b&&
     std::strcmp(e.refusal,"SMITH_TOOL_KEY_DIFFERS_FROM_CURRENT_HAND")==0&&
     e.linkedEntity==e.boundEntity&&e.keysRead&&e.holderKey!=e.handKey,
     "different hand key is distinguishable from entity mismatch");
  Put(slots,st::kHandKeys,0xAAAA000000000001ull);}
 {Put(hammer,st::kAttached,std::uint8_t(0));
  ck(!pureEvidence(attachment.data(),42,11,b,e)&&!b&&
     std::strcmp(e.refusal,"SMITH_HOLDER_NOT_ATTACHED")==0&&e.attached==0,
     "native attach completion is distinct from a permanent binding mismatch");
  Put(hammer,st::kAttached,std::uint8_t(1));}
 ck(pureEvidence(attachment.data(),42,11,b,e)&&
    std::strcmp(e.refusal,"ACCEPTED_CURRENT_BINDING")==0&&e.boundEntity==42,
    "a fresh accepted binding replaces refusal evidence");
 ck(!pureEvidence(attachment.data(),42,-1,b,e)&&!b&&
    std::strcmp(e.refusal,"UNKNOWN_NATIVE_PHASE")==0&&!e.holder&&!e.keysRead,
    "early refusal clears previously collected holder and key evidence");
}
static std::array<QuatT28,1> original{};
// Production builds a FRESH packet each frame: old tool poses never survive a skipped side.
static void Correlate(){frame.tools59c10[0]={};frame.tools59c10[1]={};BuildToolPoses59c10(frame,original.data(),original.size());}
static void Fresh(){frame.stamp.tick=now;workstation178r20c1::heldToolActivityTick59c10=now;(void)ToolContext19c4r2(true);}
int main(){
 namespace ws=kcd2vr::workstation178r20c1;namespace cr=kcd2vr::combat_rules178r10;namespace ai=kcd2vr::activity_input163;
 Image();PureReader();
 // Smithing minigame with the tracked workstation wrist (F10 hands rung 2), F5 OFF, NO Grip.
 cr::f5Rung=0;cr::workstationEnabled=true;ws::setExperiment36(false);
 hands59c7f::rung=2;hands59c7f::admitted=true;hands59c7f::pickupReady=true;
 g_henry_char_attachment_manager=manager.data();
 ai::witness175.ready=true;ai::witness175.Begin(ai::Kind175::Smithing,P(smith.data()));Phase(11);
 ws::heldToolActivity59c10=int(ws::Kind::Anvil);
 frame.stamp.actor=P(character.data());frame.stamp.epoch=7;frame.valid[0]=frame.valid[1]=true;
 frame.native[0].t[0]=1;frame.allowed[0].t[0]=2;frame.native[1].t[0]=1;frame.allowed[1].t[0]=3;original[0].t[0]=1.1f;
 bool body=false;(void)ToolContext19c4r2(true,false,false,false,false,false,false,false,&body);ck(body,"workstation body admitted");g_dialogue_body178r9=body;
 Fresh();ck(SmithAutoHoldReason36(now)==nullptr,"smith gate open without F5 or Grip");
 ck(ToolExperimentReason59c10(now)!=nullptr,"legacy F5 route closed");
 // Acquisition: socket first, then the original-joint transaction, then CONTROLLER placement. No Grip.
 ck(!ToolAttachment59c10(hammerObject.data(),attachment.data())&&writes==0,"first callback only publishes the socket");
 Correlate();ck(frame.tools59c10[0].valid&&bool(frame.tools59c10[0].socket.smith36)&&frame.tools59c10[0].socket.smith36.tool==0,"hammer socket correlated");
 ck(ToolAttachment59c10(hammerObject.data(),attachment.data())&&writes==1&&lastPlaced==hammerObject.data(),"hammer auto-held at grip 0");
 ck(std::abs(lastWanted[4]-12.1f)<.00001f,"controller wrist places the native tool (allowed*grip), not the native wrist");
 ck(NativeSmithFingers36(frame.stamp.actor,frame.stamp.epoch,0,now)&&!NativeSmithFingers36(frame.stamp.actor,frame.stamp.epoch,1,now),"native finger grip only on the holding hand");
 ck(ToolAttachment59c10(hammerObject.data(),attachment.data())&&writes==2,"continuing hold");
 // Tongs (left) acquire independently; one owner per hand.
 ck(!ToolAttachment59c10(tongsObject.data(),attachmentL.data()),"tongs socket");Correlate();
 ck(ToolAttachment59c10(tongsObject.data(),attachmentL.data())&&writes==3&&std::abs(lastWanted[4]-13.1f)<.00001f,"tongs auto-held at grip 0");
 // Receipts explain the owner when the Interaction recorder is on.
 diagnostics::capture168::gate.select(diagnostics::capture168::Domain::Interaction,true,now);
 ck(ToolAttachment59c10(hammerObject.data(),attachment.data())&&writes==4,"hold with recorder");
 ck(!receipts.empty()&&receipts.back().find("pose=CONTROLLER_SMITH_AUTO_HOLD")!=std::string::npos&&
    receipts.back().find("smithTool36=HAMMER")!=std::string::npos&&receipts.back().find("grip=NOT_REQUIRED")!=std::string::npos,"hold receipt");
 diagnostics::capture168::gate.select(diagnostics::capture168::Domain::Interaction,false,now);
 // RELEASE on phase change: the same callback stops placing; then re-acquires for the new stage.
 Phase(14);int w=writes;
 ck(!ToolAttachment59c10(hammerObject.data(),attachment.data())&&writes==w,"phase change releases immediately");
 {auto stale=frame;stale.tools59c10[0]={};ToolSocket59c10 s{};ck(g_tool_socket59c10[0].read(s)&&s.smith36.phase==14,"new stage socket published");}
 Correlate();ck(ToolAttachment59c10(hammerObject.data(),attachment.data())&&writes==w+1,"re-acquired for phase 14 without Grip");
 // RELEASE on DetachHammer (holder +14 cleared, hand slot key cleared by the native detach).
 Put(hammer,st::kAttached,std::uint8_t(0));Put(slots,st::kHandKeys,std::uint64_t(0));w=writes;
 ck(!ToolAttachment59c10(hammerObject.data(),attachment.data())&&writes==w,"detach releases");
 Correlate();ck(!frame.tools59c10[0].valid,"detached socket never correlates");
 ck(!NativeSmithFingers36(frame.stamp.actor,frame.stamp.epoch,0,now),"fingers released on detach");
 // RE-ACQUISITION after AttachHammer.
 Put(hammer,st::kAttached,std::uint8_t(1));Put(slots,st::kHandKeys,0xAAAA000000000001ull);
 ck(!ToolAttachment59c10(hammerObject.data(),attachment.data()),"re-attach publishes again");Correlate();
 ck(ToolAttachment59c10(hammerObject.data(),attachment.data())&&writes==w+1,"re-acquired after AttachHammer");
 // ABORT: the setting OFF revokes at once; ON again needs a fresh socket (new request epoch).
 ws::setSmithAutoHold36(false);w=writes;
 ck(!ToolAttachment59c10(hammerObject.data(),attachment.data())&&writes==w,"menu OFF releases");
 Correlate();ck(!frame.tools59c10[0].valid,"OFF socket never correlates");
 ws::setSmithAutoHold36(true);
 ck(!ToolAttachment59c10(hammerObject.data(),attachment.data())&&writes==w,"ON again: old socket cannot resume");
 Correlate();ck(ToolAttachment59c10(hammerObject.data(),attachment.data())&&writes==w+1,"ON again: re-acquired");
 // One owner: with the setting OFF and F5 rung 2 armed, the old Grip route is the only one (Grip 0 => nothing).
 ws::setSmithAutoHold36(false);cr::f5Rung=2;ws::setExperiment36(true);held[0]=42;Fresh();w=writes;
 ck(!ToolAttachment59c10(hammerObject.data(),attachment.data())&&writes==w,"legacy route still needs Grip");
 // ...and with the setting ON the pinned tool belongs to the auto-hold even while F5 is armed.
 ws::setSmithAutoHold36(true);Fresh();
 ck(!ToolAttachment59c10(hammerObject.data(),attachment.data()),"auto-hold socket under F5");Correlate();
 ck(ToolAttachment59c10(hammerObject.data(),attachment.data())&&writes==w+1,"auto-hold owns the pinned tool under F5");
 {ToolSocket59c10 s{};ck(g_tool_socket59c10[0].read(s)&&bool(s.smith36)&&s.request==ws::smithHoldEpoch36.load(),"socket owner is the smith request");}
 cr::f5Rung=0;ws::setExperiment36(false);held[0]=0;Fresh();
 // Unpinned attachments: forge bag (entity_right) and an NPC attachment are never read/placed.
 {Put(attachmentE,0x10,P("entity_right"));Put(workpieceObject,8,std::uint32_t(45));w=writes;
  ck(!ToolAttachment59c10(workpieceObject.data(),attachmentE.data())&&writes==w,"forge bag stays native");
  Put(attachmentE,0x10,P("entity_left"));Put(workpieceObject,8,std::uint32_t(44));}
 {Put(attachment,0x28,static_cast<void*>(npcManager.data()));w=writes;
  ck(!ToolAttachment59c10(hammerObject.data(),attachment.data())&&writes==w,"NPC attachment ignored");
  Put(attachment,0x28,static_cast<void*>(manager.data()));}
 // Sword workpiece on entity_left (no tongs) is auto-held by the left controller.
 Put(model,st::kTongsSlot,std::uintptr_t(0));
 ck(!ToolAttachment59c10(workpieceObject.data(),attachmentE.data()),"workpiece socket");Correlate();
 ck(ToolAttachment59c10(workpieceObject.data(),attachmentE.data())&&lastPlaced==workpieceObject.data(),"sword workpiece auto-held");
 Put(model,st::kTongsSlot,P(tongs.data()));
 // Tracking loss, real UI and native reference refuse every write.
 tracking=false;w=writes;ck(!ToolAttachment59c10(hammerObject.data(),attachment.data())&&writes==w,"tracking loss");tracking=true;
 diagnostics::lab::reference=true;ck(!ToolAttachment59c10(hammerObject.data(),attachment.data())&&writes==w,"native reference");diagnostics::lab::reference=false;
 (void)ToolContext19c4r2(true,true);ck(!ToolAttachment59c10(hammerObject.data(),attachment.data())&&writes==w,"real UI owns pose");Fresh();
 // Lost wrist ownership (F10 off) releases.
 hands59c7f::rung=0;ck(SmithAutoHoldReason36(now)!=nullptr&&!ToolAttachment59c10(hammerObject.data(),attachment.data())&&writes==w,"wrist not admitted");
 hands59c7f::rung=2;Fresh();
 (void)ToolAttachment59c10(hammerObject.data(),attachment.data());Correlate();ck(ToolAttachment59c10(hammerObject.data(),attachment.data()),"held again");
 // COMPLETION: minigame end releases at once and nothing correlates afterwards.
 ai::witness175.End(P(smith.data()));w=writes;
 ck(SmithAutoHoldReason36(now)!=nullptr&&!ToolAttachment59c10(hammerObject.data(),attachment.data())&&writes==w,"minigame end releases");
 Correlate();ck(!frame.tools59c10[0].valid&&!NativeSmithFingers36(frame.stamp.actor,frame.stamp.epoch,0,now),"no pose after the end");
 // A NEW minigame (same address) is a new lifetime: re-acquire from scratch.
 ai::witness175.Begin(ai::Kind175::Smithing,P(smith.data()));Phase(16);Fresh();
 ck(!ToolAttachment59c10(hammerObject.data(),attachment.data()),"new minigame publishes");Correlate();
 ck(ToolAttachment59c10(hammerObject.data(),attachment.data())&&writes==w+1,"new minigame re-acquired");
 // Unknown phase (observer unavailable) fails closed.
 Phase(-1);w=writes;ck(!ToolAttachment59c10(hammerObject.data(),attachment.data())&&writes==w,"unknown phase never binds");
 std::printf("PASS %u smith_tool36 checks: pinned hammer/tongs/workpiece membership, auto-hold acquire/release/abort/re-acquire without Grip\n",checks);
return 0;
}
