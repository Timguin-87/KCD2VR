// Existing Henry controller owner only. Release yields the CURRENT native grind
// cycle, waits for its actual READY/ended state, then retires only UseMortar.
// PlaceMortar is separately requested through current native contextual input.
namespace mortar69=kcd2vr::subsystems::alchemy::mortar69;
struct MortarStop69 {mortar69::Request r{};bool active=false,attempted=false;
 int phaseD67=-1;std::uintptr_t childD67=0;std::uint64_t phaseTickD67=0;
 float activeD85=-1.f;std::uint64_t progressTickD85=0,sourceTickR96=0;}; // D85 craft: native child active-time progress
static MortarStop69 g_mortar_stop69;
static std::uint64_t g_mortar_retired69=0;
static void TickMortarStop69(void* ctrl,std::uint64_t now) noexcept {
 if(!ctrl||ctrl!=g_henry_ctrl.load()||!OnAlchemyUpdateOwnerR27()||!IsHenryController(ctrl))return;
 static thread_local bool entered69=false;if(entered69)return;
 struct Entry69 {bool& value;Entry69(bool& v):value(v){value=true;}~Entry69(){value=false;}} entry69(entered69);
 auto& s=g_mortar_stop69;
 const auto finish=[&](mortar69::Status status){
  (void)mortar69::result.publish({s.r.serial,now,s.r.left.instance,s.r.left.lifetime,status,
   s.attempted?mortar69::StopEntry72::Entered:mortar69::StopEntry72::NotEntered});
  if(DiagOn()&&kcd2vr::diagnostics::capture168::active(kcd2vr::diagnostics::capture168::Domain::Interaction,now)){char line69[800]{};std::snprintf(line69,sizeof(line69),
   "G2 MORTAR_HANDOFF serial=%llu status=%u nativeStopAttempt=%d detach=NOT_SYNTHESIZED next=NATIVE_CONTEXTUAL_PLACE_OR_EXPLICIT_A session=%llu tick=%llu instance=%llX lifetime=%llu parent=%llX child=%llX phase=%d phaseAgeMs=%lld requestAgeMs=%lld leftEntity=%u rightEntity=%u readyIsNotContentsTransfer=1",
   (unsigned long long)s.r.serial,unsigned(status),s.attempted,
   (unsigned long long)kcd2vr::diagnostics::capture168::gate.session[unsigned(kcd2vr::diagnostics::capture168::Domain::Interaction)].load(),
   (unsigned long long)now,(unsigned long long)s.r.left.instance,(unsigned long long)s.r.left.lifetime,
   (unsigned long long)s.r.parent,(unsigned long long)s.childD67,s.phaseD67,
   s.phaseTickD67&&now>=s.phaseTickD67?static_cast<long long>(now-s.phaseTickD67):-1LL,
   s.r.tick&&now>=s.r.tick?static_cast<long long>(now-s.r.tick):-1LL,s.r.left.entity,s.r.right.entity);diagnostic_trace::Write(line69);}
  g_mortar_retired69=s.r.serial;s={};mortar69::interested.store(false,std::memory_order_release);
  mortar69::Request newer{};if(mortar69::request.read(newer)&&newer.serial&&newer.serial!=g_mortar_retired69)
   mortar69::interested.store(true,std::memory_order_release);
 };
 const bool startingD78R2=!s.active;
 if(startingD78R2){mortar69::Request r{};if(!mortar69::request.read(r)||!r.serial||r.serial==g_mortar_retired69)return;
  s.r=r;s.active=true;s.sourceTickR96=r.tick;}
 now=std::uint64_t(GetTickCount64());
 // A held tilt can cease while its native grind cycle is still running. Only
 // the unchanged original request can withdraw, and only BEFORE native Stop
 // entry. An entered/refused Stop, retired pair or newer request is never
 // replayed, detached, or called a successful placement by this acknowledgment.
 const auto withdrawnD78R2=[&]() noexcept {
  mortar69::Request latest{};
  return !s.attempted&&mortar69::request.read(latest)&&mortar69::sameRequestD78R2(s.r,latest)&&
   latest.withdrawTickD78R2&&latest.withdrawTickD78R2>=s.r.tick&&now>=latest.withdrawTickD78R2;
 };
 if(withdrawnD78R2()){finish(mortar69::Status::WithdrawnD78R2);return;}
 if(startingD78R2&&(!s.r.tick||now<s.r.tick||now-s.r.tick>80)){finish(mortar69::Status::Revoked);return;}
 const auto& r=s.r;const auto b=g_base.load();namespace act=kcd2vr::activity_input163;const auto& w=act::witness175;
 kcd2vr::workstation_sample46::Frame f{};
 constexpr unsigned char nativeDestination69[]{0x83,0xfe,0x06,0x0f,0x84,0x4b,0x01,0x00,0x00,0x85,0xf6,0x0f,0x84,0x57,0x01,0x00,0x00};
 constexpr unsigned char completedD67[]{0x41,0xff,0x80,0x88,0x00,0x00,0x00,0x41,0xc7,0x40,0x70,0x02,0x00,0x00,0x00};
 constexpr unsigned char stopChildD67[]{0x48,0x8d,0xbb,0x90,0x00,0x00,0x00,0x48,0x83,0x3f,0x00,0x74,0x12,0x48,0x8b,0xcb,0xe8,0x79,0xb7,0xc2,0xff};
 if(!AlchemyReturnPins67()||!MatchBytes(b+0x6C3046,completedD67,sizeof(completedD67))||
    !MatchBytes(b+0xA97AAA,stopChildD67,sizeof(stopChildD67))||!MatchBytes(b+0x8D20DC,nativeDestination69,sizeof(nativeDestination69))||!w.ready.load()||w.transitionDepth175.load()||w.lifetime175.load()!=r.left.lifetime||
    w.active[unsigned(act::Kind175::Alchemy)].load()!=r.left.instance||!r.parent||
    r.left.side!=1||r.left.type!=2||r.right.side!=0||r.right.type!=3||r.left.character!=r.right.character||
    r.left.player!=r.right.player||r.left.instance!=r.right.instance||r.left.lifetime!=r.right.lifetime||
    r.modeEpoch!=kcd2vr::minigame_hands45::modeEpoch67.load()||!kcd2vr::workstation178r20c1::fullVrAlchemy63(now)||
    now<r.tick||(now-r.tick>15000&&!s.phaseTickD67)){finish(mortar69::Status::Revoked);return;}
 const bool haveFrameR96=kcd2vr::frik_pose_probe::WorkstationFrame46(f);now=std::uint64_t(GetTickCount64());
 if(!haveFrameR96||!f.hand[0].valid||!f.hand[1].valid){
  // Missing tracking is not a station/retained-pair replacement. Wait inside
  // the original source age only, without Stop entry or watchdog renewal.
  if(s.sourceTickR96&&now>=s.sourceTickR96&&now-s.sourceTickR96<=80)return;
  finish(mortar69::Status::Revoked);return;
 }
 if(!f.allowed||!kcd2vr::workstation_sample46::fresh(f,now)||f.poseEpoch!=r.poseEpoch||f.sourceEpoch!=r.sourceEpoch||
    !kcd2vr::workstation_sample46::finite(f.hand[0])||!kcd2vr::workstation_sample46::finite(f.hand[1])){finish(mortar69::Status::Revoked);return;}
 s.sourceTickR96=f.tick;
 // D85 craft: the old fixed 15 s from the request also cut a still-running
 // native grind cycle. It now only bounds a request whose native phase was
 // never read; after that the phase watchdog below governs, and a working
 // phase-3 child must keep showing native progress.
 const auto rd=[](std::uintptr_t at,auto& v){return SehCopy(reinterpret_cast<void*>(at),&v,sizeof(v));};
 auto* instance=reinterpret_cast<void*>(r.left.instance);std::uintptr_t first=0,last=0,parent=0;
 if(!rd(r.left.instance+0xB0,first)||!rd(r.left.instance+0xB8,last)||!first||last<first+16||last-first>256||!rd(first+8,parent))return;
 if(parent&&parent!=r.parent){finish(mortar69::Status::Revoked);return;}
 // Actual current pair is re-read even after parent retirement; this includes
 // the exact native retained-pair membership, entity and attachment objects.
 kcd2vr::alchemy_tool18::Binding left{},right{};
 const auto samePair69=[&]() noexcept {
  return kcd2vr::alchemy_tool18::read(rd,b,r.left.instance,r.left.player,r.left.character,r.left.attachment,r.left.entity,left)&&
   kcd2vr::alchemy_tool18::read(rd,b,r.right.instance,r.right.player,r.right.character,r.right.attachment,r.right.entity,right)&&
   left.side==1&&left.type==2&&right.side==0&&right.type==3&&left.object==r.left.object&&right.object==r.right.object;
 };
 if(!samePair69()){finish(mortar69::Status::Revoked);return;}
 if(!parent){
  const auto retiredChild=s.childD67?s.childD67:r.child;
  if(retiredChild&&AlchemyReturnChildMembership67(ctrl,retiredChild)!=0)return;
  finish(mortar69::Status::ReadyForNativePlace);return;
 }
 if(left.action!=r.parent||right.action!=r.parent||left.actionClass!=b+0x3F60770||right.actionClass!=b+0x3F60770){finish(mortar69::Status::Revoked);return;}
 const int phaseD67=Rd<int>(reinterpret_cast<void*>(parent),0x70,-1);
 std::uintptr_t childD67=0;if(!rd(parent+0x90,childD67)||phaseD67<0||phaseD67>4)return;
 if(phaseD67!=s.phaseD67||childD67!=s.childD67){s.phaseD67=phaseD67;s.childD67=childD67;s.phaseTickD67=now;
  s.activeD85=-1.f;s.progressTickD85=now;}
 // D85 craft: IAction active time (kActActiveTime) of THIS child, the field
 // the D81 pre-Update speed drives, is the progress witness of a long grind.
 if(phaseD67==3&&childD67){const float activeD85=Rd<float>(reinterpret_cast<void*>(childD67),kActActiveTime,-1.f);
  if(std::isfinite(activeD85)&&activeD85>=0.f&&activeD85>s.activeD85){if(s.activeD85>=0.f)s.progressTickD85=now;s.activeD85=activeD85;}}
 // D78 capture: this exact native phase3 still ran after 5.0 seconds and
 // retired around 6.5 seconds. Only that current working child may outlive
 // the shorter phase watchdog while it keeps progressing (D85). All
 // owner/tracking checks and other phases' existing timeout remain.
 if(s.phaseTickD67&&now>=s.phaseTickD67&&now-s.phaseTickD67>5000){
  void* childContextD78R2=nullptr;void* controllerContextD78R2=nullptr;
  const bool currentWorkD78R2=phaseD67==3&&childD67&&
   Rd<std::uintptr_t>(reinterpret_cast<void*>(childD67),0,0)==b+0x3A47948&&
   Rd<std::uintptr_t>(reinterpret_cast<void*>(childD67),0x268,0)==r.left.instance&&
   Rd<int>(reinterpret_cast<void*>(childD67),kActStatus,-1)==2&&
   rd(childD67+kActContext,childContextD78R2)&&childContextD78R2&&
   rd(reinterpret_cast<std::uintptr_t>(ctrl)+kCtrlContext,controllerContextD78R2)&&
   controllerContextD78R2==childContextD78R2;
  const bool progressingD85=s.progressTickD85&&now>=s.progressTickD85&&now-s.progressTickD85<=5000;
  if(!currentWorkD78R2||!progressingD85){finish(mortar69::Status::Revoked);return;}
 }
 // Native completion6C3046 publishes phase2 AFTER its work/counter, but keeps
 // the child at parent+90. Native Stop A97AAA itself retires that child through
 // 6C3238. Requiring child membership to disappear BEFORE Stop deadlocks READY.
 // We still never Stop phase1/3 work, and never steal an outstanding mod lease.
 if(phaseD67!=2||g_semi_alchemy46.written){
  (void)mortar69::result.publish({r.serial,now,r.left.instance,r.left.lifetime,mortar69::Status::WaitingNativeCycle});return;
 }
 if(s.attempted){finish(mortar69::Status::Refused);return;}
 if(withdrawnD78R2()){finish(mortar69::Status::WithdrawnD78R2);return;}
 now=std::uint64_t(GetTickCount64());
 if(!kcd2vr::workstation_sample46::fresh(f,now)||!kcd2vr::workstation178r20c1::fullVrAlchemy63(now)||
    !w.ready.load()||w.transitionDepth175.load()||w.lifetime175.load()!=r.left.lifetime||
    w.active[unsigned(act::Kind175::Alchemy)].load()!=r.left.instance||
    r.modeEpoch!=kcd2vr::minigame_hands45::modeEpoch67.load()){finish(mortar69::Status::Revoked);return;}
 s.attempted=true;
 // EXACT local slot1 cancellation, same retail implementation used by native
 // controls. A refused Stop is not an accepted placement and is never retried.
 if(!InvokeAlchemyCancel67(static_cast<unsigned char*>(instance)+0x78)){finish(mortar69::Status::Refused);return;}
 // Native Stop can synchronously leave/re-enter a station or load another
 // activity. Never dereference its old instance after that lifetime changes.
 if(!w.ready.load()||w.transitionDepth175.load()||w.lifetime175.load()!=r.left.lifetime||
    w.active[unsigned(act::Kind175::Alchemy)].load()!=r.left.instance||
    r.modeEpoch!=kcd2vr::minigame_hands45::modeEpoch67.load()){finish(mortar69::Status::Revoked);return;}
 const bool postFrameR96=kcd2vr::frik_pose_probe::WorkstationFrame46(f);now=std::uint64_t(GetTickCount64());
 if(!postFrameR96)return; // already entered: await observation, never repeat native Stop
 if(!f.allowed||!kcd2vr::workstation_sample46::fresh(f,now)||
    f.poseEpoch!=r.poseEpoch||f.sourceEpoch!=r.sourceEpoch){finish(mortar69::Status::Revoked);return;}
 if(!rd(r.left.instance+0xB0,first)||!first||!rd(first+8,parent))return;
 if(parent==r.parent){finish(mortar69::Status::Refused);return;}
 if(parent){finish(mortar69::Status::Revoked);return;}
 // Stop may synchronously detach or replace either attachment. A zero parent
 // proves action retirement, not that the original retained pair survived it.
 if(!samePair69()||left.action||right.action||!w.ready.load()||w.transitionDepth175.load()||
    w.lifetime175.load()!=r.left.lifetime||w.active[unsigned(act::Kind175::Alchemy)].load()!=r.left.instance){
  finish(mortar69::Status::Revoked);return;}
 const auto retiredChild=s.childD67?s.childD67:r.child;
 if(!retiredChild||AlchemyReturnChildMembership67(ctrl,retiredChild)==0)finish(mortar69::Status::ReadyForNativePlace);
}
