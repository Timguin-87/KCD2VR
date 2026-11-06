// Compile the COMPLETE current frik translation unit. Invoke its actual portable
// publication/grip/geometry bridge here. Native function-address leaves are NOT
// executed; Win32/Detour shims are explicitly non-native.
#include "../src/frik_pose_probe.cpp"
#include <cstdio>
static std::vector<std::string> observationLines59;
namespace kcd2vr::diagnostic_trace { void Write(const char* text) noexcept {if(text&&std::strstr(text,"observe59{"))observationLines59.emplace_back(text);} }
namespace kcd2vr::retail_build112 {extern bool hostRetailVerified112;}
namespace kcd2vr::action_time_owner59c11 {extern std::uintptr_t hostOwnedDrawEntry62;}
using namespace kcd2vr::frik_pose_probe;
static unsigned checks=0,failures=0;
static void Check(bool b,const char*n){++checks;if(!b)++failures;std::printf("%s %s\n",b?"PASS":"FAIL",n);}
int main(){
 { // Actual production equipment entry guard; owned-hook service tested in the full owner TU.
  const auto module=hostModuleHandleW;const auto pinned=g_wh_pinned104.exchange(0);
  const auto verified=kcd2vr::retail_build112::hostRetailVerified112;
  std::vector<unsigned char> native(0x8EEA24+32);hostModuleHandleW=native.data();const auto b=WhBase();
  kcd2vr::retail_build112::hostRetailVerified112=true;
  for(const auto& fp:ph::equipmentFingerprint114)std::memcpy(native.data()+fp.rva,fp.bytes.data(),16);
  Check(PhysEquipmentEntries62(),"equipment accepts pristine exact retail entries");
  native[0x8CEA48]=0xE9;
  Check(!PhysEquipmentEntries62(),"foreign draw jump cannot authorize holster/draw");
  kcd2vr::action_time_owner59c11::hostOwnedDrawEntry62=b;
  Check(PhysEquipmentEntries62(),"verified process-live own draw hook preserves equipment admission");
  native[0x8CF158]^=1;Check(!PhysEquipmentEntries62(),"own draw hook does not bypass holster fingerprint");native[0x8CF158]^=1;
  native[0x8EEA24]^=1;Check(!PhysEquipmentEntries62(),"own draw hook does not bypass extra-item fingerprint");native[0x8EEA24]^=1;
  kcd2vr::retail_build112::hostRetailVerified112=false;Check(!PhysEquipmentEntries62(),"owned entry cannot bypass retail build qualification");
  kcd2vr::action_time_owner59c11::hostOwnedDrawEntry62=0;
  hostModuleHandleW=module;g_wh_pinned104=pinned;kcd2vr::retail_build112::hostRetailVerified112=verified;
 }

 {const auto oldMode=g_census_mode.load();const auto oldCal=g_cal_state.load();
  const bool oldMenu=g_ctx_menu.load(),oldCut=g_ctx_cutscene.load();
  g_census_mode=3;g_cal_state=2;g_ctx_menu=false;g_ctx_cutscene=false;
  Check(StowedBowImmersive54(),"C54 virtual bow hide belongs to calibrated immersive body");
  g_ctx_menu=true;Check(!StowedBowImmersive54(),"C54 menu restores native stowed appearance");
  g_ctx_menu=false;g_ctx_cutscene=true;Check(!StowedBowImmersive54(),"C54 native cutscene restores stowed appearance");
  g_ctx_cutscene=false;g_cal_state=0;Check(!StowedBowImmersive54(),"C54 uncalibrated body cannot own stowed visibility");
  g_census_mode=oldMode;g_cal_state=oldCal;g_ctx_menu=oldMenu;g_ctx_cutscene=oldCut;}
 g_phys_state6698.weapon[0].ready=g_phys_state6698.weapon[0].prepared=true;
 g_phys_state6698.weapon[0].stroke118.active=true;g_phys_state6698.weapon[0].linearVelocity={3,0,0};
 g_phys_state6698.weapon[0].lastTick=10;g_phys_state6698.sample=20;
 PhysRetireMotion30c1();
 Check(!g_phys_state6698.sample&&!g_phys_state6698.weapon[0].ready&&!g_phys_state6698.weapon[0].prepared&&
       !g_phys_state6698.weapon[0].stroke118.active&&ph::m::length(g_phys_state6698.weapon[0].linearVelocity)==0,
       "native body handoff retires pre-hit pose, velocity and gesture history");
 g_phys_armed6698=true;g_phys_abort6698=true;g_gameplay_allowed=false;
 PhysNativeTickImpl6698();
 Check(!g_phys_abort6698.load(),"inactive native frame consumes abort instead of poisoning the recovered frame");
 g_phys_armed6698=false;

 {cc::Event event{};while(cc::pop(event)){};cc::until=0;cc::captureOnlyUntil168r1=0;cc::interactionOnlyUntil168r1=0;
  ph::Command edge{};edge.kind=ph::CommandKind::Abort;edge.tick=1001;edge.sample=71;edge.key=12;edge.generation=3;
  ph::Output snapshot{};snapshot.melee118.reason=kcd2vr::melee118::Reason::Blocked;snapshot.blocked[0]=true;
  snapshot.melee118.axisKnown=true;snapshot.melee118.axisCos=.92;snapshot.melee118.modelAxisCos=.94;
  snapshot.melee118.guardedOrigin59=true;snapshot.melee118.guardHandTravel59=.041;snapshot.melee118.guardCutPass59=false;
  ObserveMeleeCommand174r2(edge,snapshot,true);Check(!cc::pop(event),"command-edge recorder is quiet without finite capture");
  kcd2vr::diagnostics::capture168::gate.select(kcd2vr::diagnostics::capture168::Domain::Combat,true,1000);
  cc::selectSources(cc::CaptureOnly168R1,1000,true);ObserveMeleeCommand174r2(edge,snapshot,true);
  snapshot.melee118.reason=kcd2vr::melee118::Reason::Idle;snapshot.blocked[0]=false; // later mailbox replacement
  Check(cc::pop(event)&&event.kind==cc::Kind::MeleeCommand174r2&&event.sequence==71&&
        !std::strcmp(event.name,"BLOCKED")&&(event.flags&18)==18&&event.key[0]==12&&event.aliasHash==3,
        "terminal edge survives later idle snapshot with original identity and queue acknowledgement");
  Check(event.current[2]==1.f&&event.current[0]>.91f&&event.current[1]>.93f&&event.current[4]==1.f&&
        event.current[3]>.040f&&event.current[3]<.042f&&event.current[5]==0.f,
        "C59 immutable command edge retains exact two-family axial and guarded hand-travel qualification");
  ObserveMeleeCommand174r2(edge,snapshot,false);Check(cc::pop(event)&&!(event.flags&16),"failed enqueue is not reported as queued");
  edge.kind=ph::CommandKind::BowFire;ObserveMeleeCommand174r2(edge,snapshot,true);Check(!cc::pop(event),"bow commands are not mislabeled melee");
  cc::selectSources(0,1002);kcd2vr::diagnostics::capture168::gate.select(kcd2vr::diagnostics::capture168::Domain::Combat,false,1002);}

 { // Exact NativeObserve104 wrapper and existing refusal formatter, not a parallel logger.
  std::array<unsigned char,0x500> actor59{};std::array<unsigned char,0xe00> state59{};
  const auto put59=[&](unsigned off,auto value){std::memcpy(state59.data()+off,&value,sizeof(value));};
  auto* statePtr59=state59.data();auto* actorPtr59=actor59.data();std::memcpy(actor59.data()+0x210,&statePtr59,8);put59(0xce0,actorPtr59);
  put59(0x38,1);put59(0x218,-1);put59(0x248,-1);put59(0x278,2);put59(0x1b8,1);
  NativeOwner104 owner59{};owner59.actor=actorPtr59;owner59.state=statePtr59;nc104::Observed fast59{},captured59{};nc104::ReadReceipt59 receipt59{};
  Check(NativeObserve104(owner59,fast59)&&NativeObserve104(owner59,captured59,&receipt59)&&
        !nc104::changed(fast59,captured59)&&receipt59.readMask==0x3ff,"C59 captured native reads preserve the original noncapture valid result");
  for(int phaseH14:{128,256}){put59(0x38,phaseH14);put59(0x278,7);
   Check(NativeObserve104(owner59,fast59)&&NativeObserve104(owner59,captured59,&receipt59)&&
         nc104::blockMetadataH14(fast59)&&!nc104::valid(fast59)&&!nc104::changed(fast59,captured59)&&receipt59.readMask==0x3ff,
         "H14 real fast/captured wrappers identically classify native block metadata without granting an attack sector");}
  put59(0x38,1);
  put59(0x278,99);Check(!NativeObserve104(owner59,fast59)&&!NativeObserve104(owner59,captured59,&receipt59)&&
        receipt59.offset==0x278&&!std::strcmp(receipt59.why,"TYPED_RANGE"),"C59 both real wrapper paths refuse the same invalid native zone");
  ph::Command refusal59{};refusal59.tick=GetTickCount64();refusal59.stroke117=ph::CommandKind::Slash;
  cc::until=0;cc::captureOnlyUntil168r1=0;cc::interactionOnlyUntil168r1=0;observationLines59.clear();
  NativeInputReject117("NATIVE_STATE_READ",refusal59,&receipt59,&captured59);
  Check(observationLines59.empty(),"C59 diagnostic details never format outside finite F3 capture");
  kcd2vr::diagnostics::capture168::gate.select(kcd2vr::diagnostics::capture168::Domain::Combat,true,refusal59.tick);
  cc::selectSources(cc::CaptureOnly168R1,refusal59.tick,true);
  NativeInputReject117("NATIVE_STATE_READ",refusal59,&receipt59,&captured59);
  Check(observationLines59.size()==1&&observationLines59[0].find("offset=0x278 readMask=0x3FF")!=std::string::npos&&
        observationLines59[0].find("zone=99")!=std::string::npos&&observationLines59[0].find("extraReads=0}")!=std::string::npos,
        "C59 same existing refusal line records exact failed field and partial validity, with zero rereads");
  cc::selectSources(0,refusal59.tick);kcd2vr::diagnostics::capture168::gate.select(kcd2vr::diagnostics::capture168::Domain::Combat,false,refusal59.tick);
 }
 ph::Command c{};g_phys_commands6698.head=0;g_phys_commands6698.tail=0;
 for(unsigned n=0;n<31;++n){c.sample=n+1;Check(g_phys_commands6698.push(c),"bounded native request enqueue");}
 Check(!g_phys_commands6698.push(c)&&g_phys_abort6698,"queue exhaustion aborts ownership, never overwrites request");
 for(unsigned n=0;n<31;++n){Check(g_phys_commands6698.peek(c)&&c.sample==n+1,"native request order stable");g_phys_commands6698.pop(c);}
 Check(!g_phys_commands6698.pop(c),"request consumed once");
 PhysEquipmentItem6698 item{};item.profile.key=777;item.profile.generation=3;item.profile.side=0;item.profile.classId=4;item.profile.twoHanded=true;item.profile.weight=2;item.boundsValid=true;
 const float bbox[6]{-.06f,-.20f,-.05f,.06f,1.f,.05f};std::copy_n(bbox,6,item.bounds);item.held=true;
 std::array<unsigned char,0x30> attach{};const float one=1;for(int k=0;k<3;++k)std::memcpy(attach.data()+12+k*4,&one,4);
 CommittedFrame6695 body{};body.world[3]=body.world[7]=1;body.valid[0]=body.valid[1]=true;body.native[0]={{0,0,0,1},{0,0,1.2f}};body.native[1]={{0,0,0,1},{0,-.09f,1.2f}};
 float natural[8]{0,0,0,1,0,0,1.2f,1};
 g_support_synthetic178r9=true; // exercise the shipped INI value, not just host default false
 // Held capture requires the same native pose's qualified socket metadata.
 PhysSocket101 socket{};socket.valid=true;socket.key=item.profile.key;socket.generation=item.profile.generation;
 socket.character=attach.data();socket.joint=208;socket.relative={{0,0,0,1},{0,0,0}};
 socket.tick=9990;socket.callback=1;body.stamp.actor=reinterpret_cast<std::uintptr_t>(socket.character);
 body.stamp.sample=1;PhysCaptureGrip6698(item,natural,attach.data(),body);
 Check(!g_phys_grip_bank6698.grip[0].ready[0]&&!g_phys_grip_bank6698.grip[0].ready[1],
       "held transform without coherent binding cannot learn a grip");
 for(int n=2;n<=13;++n){body.stamp.sample=n;PhysCaptureGrip6698(item,natural,nullptr,body,socket.scale,&socket);}
 PhysGripBank6698 grips{};g_phys_grips6698.read(grips);
 Check(grips.grip[0].ready[0]&&grips.grip[0].ready[1],"native coherent samples automatically acquire both grip relations");
 { // C37 support source chain: PhysFillProfile6698 (selected socket) -> ph::step latch (Grip) -> final displayed wrist.
  // Asset data: weapon_slots.json (long_sword_* slt_1 10.46 cm, long_sword_henry slt_1 5.0 cm, polearms slt_0 61.3 / slt_1 27.3 cm);
  // runtime data: C34/C35/C36 logs, Henry's sword native right wrist (0.021,-0.066,0.012), native left (-0.042,-0.050,-0.174).
  const ph::Pose rightSlot{{0,0,0,1},{-.0661,.0239,.0161}}; // male.chr RightHandSlot in RightHand
  const ph::Quat s0q{-.70710678,.70710678,0,0},s1q{0,0,.70710678,.70710678}; // longsword slt_0 / slt_1 rotations (x,y,z,w)
  auto sword=[&](double slt1z){auto it=item;it.profile.key=4242;it.profile.generation=7;it.profile.side=0;it.profile.classId=4;it.profile.twoHanded=true;
   const float b[6]{-.02f,-.12f,-.16f,.02f,.12f,1.055f};std::copy_n(b,6,it.bounds);it.boundsValid=true;
   it.slotValid34[0]=it.slotValid34[1]=true;it.slot34[0]={s0q,{0,0,0}};it.slot34[1]={s1q,{0,0,slt1z}};return it;};
  auto nativeGrip=[&](const PhysEquipmentItem6698& it,float scale){PhysGrip6698 g{};g.key=it.profile.key;g.generation=it.profile.generation;
   for(int k=0;k<3;++k)g.geometryScale[k]=scale;auto s0=it.slot34[0];s0.p=s0.p*double(scale);
   g.hand[0]=ph::compose(s0,ph::inverse(rightSlot));g.ready[0]=true;g.ready[1]=false;g.stable[0]=6;return g;}; // native right wrist = slt_0 * inv(RightHandSlot)
  auto fill=[&](const PhysEquipmentItem6698& it,const PhysGrip6698& g){ph::Profile p{};PhysFillProfile6698(it,g,p);return p;};
  const bool savedSlot=g_support_slot34.load(),savedSynth=g_support_synthetic178r9.load();g_support_synthetic178r9=true;g_support_slot34=true;
  const auto common=sword(-.1046);const auto henry=sword(-.05003);
  auto wide=common;{const float b[6]{-.08f,-.12f,-.24f,.08f,.12f,1.055f};std::copy_n(b,6,wide.bounds);} // the earlier "logged" fixture bounds: room for the synthetic hilt point
  const auto gC=nativeGrip(common,1.f),gH=nativeGrip(henry,1.f);
  Check(pm::length(gH.hand[0].p-pm::Vec3{.021,-.066,.012})<.006,"C37 asset chain: slt_0 * inv(RightHandSlot) reproduces the logged native right wrist of Henry's sword within 6 mm");
  // 1) selection order
  const auto pc=fill(common,gC);
  Check(pc.support&&pc.supportSlot34&&pc.supportSource37==2&&pc.slotCheck37==1&&!pc.supportPositionOnly174r2&&pm::length(pc.grip[1].p-pm::Vec3{-.0239,-.0661,-.0885})<.002,
        "C37 no native left frame: a measured slt_1 (10.46 cm) gives the full left wrist frame");
  g_support_slot34=false;const auto pcOff=fill(common,gC);const auto phOff=fill(henry,gH);g_support_slot34=true;
  const auto ph5=fill(henry,gH);
  Check(ph5.slotCheck37==3&&!ph5.supportSlot34&&ph5.support==phOff.support&&ph5.supportSource37==phOff.supportSource37&&pm::length(ph5.grip[1].p-phOff.grip[1].p)<1e-12,
        "C37 Henry's own sword (slots 5 cm apart, palms would overlap) keeps exactly the existing path");
  Check(!phOff.support&&phOff.supportSource37==0&&pcOff.supportSource37==0&&pcOff.slotCheck37==0,
        "C37 real thin blade bounds (x +-2 cm): the synthetic hilt point is refused (logged support=0 before capture); the axis grip owns that case");
  g_support_slot34=false;const auto pwOff=fill(wide,gC);g_support_slot34=true;const auto pw=fill(wide,gC);
  Check(pwOff.supportSource37==3&&pwOff.supportPositionOnly174r2&&pw.supportSource37==2&&pw.slotCheck37==1,"C37 wide bounds: slot option off = synthetic fallback, on = measured slt_1");
  auto gN=gH;gN.ready[1]=true;gN.stable[1]=6;gN.hand[1]={{.1,.2,.3,.927},{-.042,-.050,-.174}};gN.hand[1].q=*pm::unit(gN.hand[1].q);
  for(const auto* it:{&common,&henry}){auto g=gN;g.key=it->profile.key;g.generation=it->profile.generation;const auto pn=fill(*it,g);
   Check(pn.supportSource37==1&&!pn.supportSlot34&&(pn.slotCheck37==5||pn.slotCheck37==3)&&pm::length(pn.grip[1].p-g.hand[1].p)<1e-12&&pm::angle(pn.grip[1].q,g.hand[1].q)<1e-12,
         "C37 a captured native left frame always wins over slt_1 (position AND rotation unchanged)");}
  {auto g=gC;g.hand[0].p.x+=.05;const auto pm1=fill(common,g);g_support_slot34=false;const auto pm0=fill(common,g);g_support_slot34=true;
   Check(pm1.slotCheck37==2&&!pm1.supportSlot34&&pm1.supportSource37==pm0.supportSource37&&pm1.supportPositionOnly174r2==pm0.supportPositionOnly174r2&&pm::length(pm1.grip[1].p-pm0.grip[1].p)<1e-12,
         "C37 failed right-wrist measurement falls back to the EXISTING frame (no bind-pose socket)");
   g.ready[1]=true;g.hand[1]=gN.hand[1];const auto pm2=fill(common,g);
   Check(pm2.slotCheck37==2&&pm2.supportSource37==1&&pm::length(pm2.grip[1].p-gN.hand[1].p)<1e-12,"C37 failed measurement with a native frame keeps the native frame");}
  {auto it=wide;it.slotValid34[1]=false;const auto pa=fill(it,gC);
   Check(pa.slotCheck37==0&&pa.supportSource37==3&&pm::length(pa.grip[1].p-pwOff.grip[1].p)<1e-12,"C37 absent slt_1 helper: synthetic fallback unchanged");
   it=wide;it.slot34[1].p={0,0,-.40};const auto pb=fill(it,gC);Check(pb.slotCheck37==4&&pb.supportSource37==3&&pm::length(pb.grip[1].p-pwOff.grip[1].p)<1e-12,"C37 slt_1 off the weapon bounds is refused");}
  // 2) no per-frame toggling while both sources exist / neither changes
  {unsigned changes=0;int last=-1;for(int n=0;n<40;++n){auto g=n<20?gC:gN;g.key=common.profile.key;g.generation=common.profile.generation;const auto p=fill(common,g);if(p.supportSource37!=last){++changes;last=p.supportSource37;}}
   Check(changes==2,"C37 source is a pure function of the captured state: slot until the native frame is captured, then native, no toggling");}
  // 3) geometryScale: polearm, helpers scaled exactly like bounds; an unscaled hand relation fails closed
  {auto pole=item;pole.profile.key=5151;pole.profile.generation=9;pole.profile.side=0;pole.profile.classId=7;pole.profile.twoHanded=true;
   const float b[6]{-.018f,-.07f,-.002f,.017f,.053f,2.039f};std::copy_n(b,6,pole.bounds);pole.boundsValid=true;
   pole.slotValid34[0]=pole.slotValid34[1]=true;pole.slot34[0]={{.70710678,-.70710678,0,0},{.0001,-.00007,.61337}};pole.slot34[1]={{0,0,0,1},{.00016,-.00002,.27256}};
   const auto g1=nativeGrip(pole,1.25f);const auto pp=fill(pole,g1);
   ph::Pose s1=pole.slot34[1];s1.p=s1.p*1.25;ph::Pose leftSlot{{0,0,0,1},{.0661,-.0239,-.0161}};
   Check(pp.supportSource37==2&&pm::length(pp.grip[1].p-ph::compose(s1,ph::inverse(leftSlot)).p)<1e-6&&std::abs(pp.extent-2.041*1.25)<.01,
         "C37 polearm at geometryScale 1.25: slt_0/slt_1 scale with the bounds; left = scaled slt_1 * inv(LeftHandSlot)");
   auto gu=nativeGrip(pole,1.f);for(int k=0;k<3;++k)gu.geometryScale[k]=1.25f;const auto pu=fill(pole,gu);
   Check(pu.slotCheck37==2&&!pu.supportSlot34,"C37 a hand relation that ignores the scale fails the measurement (fail-closed)");
   // chain: axis grip ON latches the polearm where the hand closes (slot irrelevant), axis OFF latches the slot frame
   for(bool axis:{true,false}){std::array<ph::Profile,2> prof{};prof[0]=pp;prof[1].side=1;
    ph::Input i{};i.allowed=true;i.epoch=i.bodyEpoch=1;i.sample=1;i.tick=1000;i.timeNs=1000000000ull;i.tracked[0]=i.tracked[1]=true;i.intentTracked[0]=i.intentTracked[1]=true;
    i.grip[0]=1;i.grip[1]=0;i.hands[0].p={.2,.25,1.3};i.hands[1].p={-.3,.1,1.0};i.shoulders[0]={.2,0,1.3};i.shoulders[1]={-.2,0,1.3};i.reach[0]=i.reach[1]=.9;
    i.dynamics=true;i.controllerAnchor119=true;i.constraintDrive129=true;i.supportConstraint130=true;i.supportMaxAngle=55*3.14159265358979323846/180.;i.supportV120_178r27=true;i.supportAxis59c16=axis;
    ph::State st{};auto clear=[](const ph::Profile&,ph::Pose,ph::Pose,bool){return 1.0;};auto o=ph::step(st,i,prof,clear);
    auto adv=[&](){++i.sample;i.tick+=10;i.timeNs+=10000000ull;};
    // axis on: the hand closes ON the shaft at the slt_1 palm point; axis off: the wrist comes 2 cm from the slot wrist frame
    auto place=[&](){i.hands[1]=ph::compose(o.weapons[0],pp.grip[1]);if(axis)i.hands[1].p=ph::point(o.weapons[0],s1.p);else i.hands[1].p=i.hands[1].p+pm::rotate(o.weapons[0].q,pm::Vec3{.02,0,0});};
    place();i.grip[1]=1;for(int n=0;n<8;++n){adv();place();o=ph::step(st,i,prof,clear);}
    const auto local=st.weapon[0].supportGrip169r2;
    Check(o.support[0]&&(axis?st.weapon[0].supportAxis59c16&&pm::length(local.p-pp.grip[1].p)>.015:!st.weapon[0].supportAxis59c16&&pm::length(local.p-pp.grip[1].p)<1e-9&&pm::angle(local.q,pp.grip[1].q)<1e-9),
          axis?"C37 polearm with VrTwoHandAxisGrip on: latched where the hand closes (slot/slider not used)":"C37 polearm with the axis grip off: latched exactly on the scaled slt_1 frame");}
  }
  // 4) the whole sword chain in the real producer: socket -> latch -> displayed wrist, outside/inside combat, rotation, equip, tracking, regrab
  for(int mode=0;mode<4;++mode){const bool axis=mode&1,combat=mode&2;
   std::array<ph::Profile,2> prof{};prof[0]=pc;prof[1].side=1;
   ph::Input i{};i.allowed=true;i.epoch=i.bodyEpoch=1;i.sample=1;i.tick=1000;i.timeNs=1000000000ull;i.tracked[0]=i.tracked[1]=true;i.intentTracked[0]=i.intentTracked[1]=true;
   i.grip[0]=1;i.grip[1]=0;i.hands[0].p={.2,.25,1.3};i.hands[1].p={-.3,.1,1.0};i.shoulders[0]={.2,0,1.3};i.shoulders[1]={-.2,0,1.3};i.reach[0]=i.reach[1]=.8;
   i.dynamics=true;i.controllerAnchor119=true;i.constraintDrive129=true;i.supportConstraint130=true;i.supportMaxAngle=55*3.14159265358979323846/180.;i.supportV120_178r27=true;
   i.supportAxis59c16=axis;i.physicalCombat177=combat;i.naturalCombat117=combat;
   ph::State st{};auto clear=[](const ph::Profile&,ph::Pose,ph::Pose,bool){return 1.0;};auto o=ph::step(st,i,prof,clear);
   auto adv=[&](){++i.sample;i.tick+=10;i.timeNs+=10000000ull;};
   auto shown=[&](int s){return o.displayLatched[s]?o.displayHands[s]:o.hands[s];};
   auto grab=[&](const ph::Profile& p){i.grip[1]=0;adv();o=ph::step(st,i,prof,clear);
    for(int n=0;n<8;++n){i.hands[1]=ph::compose(o.weapons[0],p.grip[1]);i.hands[1].p=i.hands[1].p+pm::rotate(o.weapons[0].q,pm::Vec3{.03,0,.01});i.grip[1]=1;adv();o=ph::step(st,i,prof,clear);}};
   auto onSocket=[&](const ph::Profile& p){const auto rel=ph::compose(ph::inverse(o.weapons[0]),shown(1));return o.support[0]&&o.owned[1]&&pm::length(rel.p-p.grip[1].p)<1e-6&&pm::angle(rel.q,p.grip[1].q)<1e-6;};
   const char* tag=mode==0?"[axis off, outside combat]":mode==1?"[axis on, outside combat]":mode==2?"[axis off, in combat]":"[axis on, in combat]";
   char name[200]{};
   grab(pc);std::snprintf(name,sizeof(name),"C37 chain %s: slot socket -> Grip latch -> displayed left wrist is exactly slt_1 * inv(LeftHandSlot)",tag);Check(onSocket(pc),name);
   // rotate the right controller 70 degrees while held: the left wrist stays rigid on the weapon
   for(int n=0;n<20;++n){i.hands[0].q=pm::from_rotation_vector({0,0,.0611*(n+1)});i.hands[1]=ph::compose(o.weapons[0],pc.grip[1]);adv();o=ph::step(st,i,prof,clear);}
   std::snprintf(name,sizeof(name),"C37 chain %s: weapon rotation keeps the displayed wrist on the same weapon-local frame",tag);Check(onSocket(pc),name);
   // the native frame is captured while the slot grasp is held
   ph::Profile nat=pc;nat.grip[1]=gN.hand[1];nat.supportSlot34=false;nat.supportSource37=1;nat.slotCheck37=5;prof[0]=nat;
   unsigned flips=0;bool was=o.support[0];
   for(int n=0;n<30;++n){i.hands[1]=ph::compose(o.weapons[0],pc.grip[1]);adv();o=ph::step(st,i,prof,clear);if(o.support[0]!=was){++flips;was=o.support[0];}}
   if(axis){std::snprintf(name,sizeof(name),"C37 chain %s: native frame captured mid-hold keeps the held grasp (no jump, no flicker)",tag);
    Check(flips==0&&onSocket(pc),name);}
   else {std::snprintf(name,sizeof(name),"C37 chain %s: native frame captured mid-hold releases once (existing same-socket invariant), no flicker",tag);
    Check(flips==1&&!o.support[0],name);}
   grab(nat);std::snprintf(name,sizeof(name),"C37 chain %s: re-grab adopts the native frame and stays latched",tag);
   bool stable=onSocket(nat);for(int n=0;n<30;++n){i.hands[1]=ph::compose(o.weapons[0],nat.grip[1]);adv();o=ph::step(st,i,prof,clear);stable=stable&&onSocket(nat);}
   Check(stable,name);
   // tracking loss: release; recovery needs a neutral Grip first
   i.tracked[1]=false;adv();o=ph::step(st,i,prof,clear);const bool lost=!o.support[0]&&!o.owned[1];
   i.tracked[1]=true;for(int n=0;n<10;++n){i.hands[1]=ph::compose(o.weapons[0],nat.grip[1]);adv();o=ph::step(st,i,prof,clear);}
   std::snprintf(name,sizeof(name),"C37 chain %s: tracking loss releases and does not silently reacquire",tag);Check(lost&&!o.support[0],name);
   // equip change (new generation): the held grasp of the old item never carries over
   grab(nat);const bool before=onSocket(nat);ph::Profile next=pc;next.generation=pc.generation+1;prof[0]=next;
   bool carried=false;for(int n=0;n<5;++n){i.hands[1]=ph::compose(o.weapons[0],next.grip[1]);adv();o=ph::step(st,i,prof,clear);
    if(o.support[0]&&!onSocket(next))carried=true;}
   std::snprintf(name,sizeof(name),"C37 chain %s: equip change (new generation) never carries the old item's grasp",tag);Check(before&&!carried&&st.weapon[0].generation==next.generation,name);
   grab(next);std::snprintf(name,sizeof(name),"C37 chain %s: fresh Grip on the new item latches its own selected socket",tag);Check(onSocket(next),name);}
  // synthetic (absent helpers) chain: position only, the controller wrist rotation captured once at the grab
  {std::array<ph::Profile,2> prof{};prof[0]=pwOff;prof[1].side=1;
   ph::Input i{};i.allowed=true;i.epoch=i.bodyEpoch=1;i.sample=1;i.tick=1000;i.timeNs=1000000000ull;i.tracked[0]=i.tracked[1]=true;i.intentTracked[0]=i.intentTracked[1]=true;
   i.grip[0]=1;i.hands[0].p={.2,.25,1.3};i.shoulders[0]={.2,0,1.3};i.shoulders[1]={-.2,0,1.3};i.reach[0]=i.reach[1]=.8;i.dynamics=true;i.controllerAnchor119=true;
   i.constraintDrive129=true;i.supportConstraint130=true;i.supportMaxAngle=1;i.supportV120_178r27=true;i.supportAxis59c16=true;
   ph::State st{};auto clear=[](const ph::Profile&,ph::Pose,ph::Pose,bool){return 1.0;};auto o=ph::step(st,i,prof,clear);
   const auto ctrlQ=pm::from_rotation_vector({.4,-.2,.3});
   for(int n=0;n<8;++n){i.hands[1].p=ph::point(o.weapons[0],pwOff.grip[1].p);i.hands[1].q=ctrlQ;i.grip[1]=1;++i.sample;i.tick+=10;i.timeNs+=10000000ull;o=ph::step(st,i,prof,clear);}
   const auto rel=ph::compose(ph::inverse(o.weapons[0]),o.hands[1]);
   Check(o.support[0]&&pm::length(rel.p-pwOff.grip[1].p)<1e-6&&pm::angle(st.weapon[0].supportGrip169r2.q,pm::multiply(pm::conjugate(o.weapons[0].q),ctrlQ))<.05,
         "C37 synthetic fallback chain: slider position, controller wrist rotation captured once (positionOnly)");}
  { // C39 GRIP: 17 Oct halberd/longsword two-hand support through the real profile producer (PhysFillProfile6698 -> ph::step -> displayed wrist).
   // Runtime data (diag.log 14:23:24): polearmBardiche native right wrist (-0.014,-0.098,0.620), native LEFT (-0.058,0.028,0.379), tip (0,0.010,2.005).
   auto pole=item;pole.profile.key=19661;pole.profile.generation=3;pole.profile.side=0;pole.profile.classId=7;pole.profile.twoHanded=true;pole.profile.weight=9.6;
   {const float b[6]{-.03f,-.04f,-.002f,.03f,.06f,2.005f};std::copy_n(b,6,pole.bounds);pole.boundsValid=true;pole.slotValid34[0]=pole.slotValid34[1]=false;}
   PhysGrip6698 gp{};gp.key=pole.profile.key;gp.generation=pole.profile.generation;for(int k=0;k<3;++k)gp.geometryScale[k]=1;
   gp.hand[0]={*pm::unit(pm::Quat{-.5,.5,.5,.5}),{-.014,-.098,.620}};gp.hand[1]={*pm::unit(pm::Quat{.5,.5,-.5,.5}),{-.058,.028,.379}};
   gp.ready[0]=gp.ready[1]=true;gp.stable[0]=gp.stable[1]=6;
   const auto pp=fill(pole,gp);const auto line=ph::shaft_line39(pp);
   const auto radial=[&](pm::Vec3 v){const auto d=v-line.o;return pm::length(d-line.u*pm::dot(d,line.u));};
   const auto oldAx=*pm::unit(pp.tip-pp.grip[0].p);const auto ol=pp.grip[1].p-pp.grip[0].p;
   const double oldPerp=pm::length(ol-oldAx*pm::dot(ol,oldAx));
   std::printf("C39 halberd: native left wrist %.1f cm from the R16 right-wrist line, %.1f cm from the shaft centre; right wrist %.1f cm\n",oldPerp*100,radial(pp.grip[1].p)*100,radial(pp.grip[0].p)*100);
   Check(pp.supportSource37==1&&line.valid&&std::abs(line.butt-.622)<.002&&oldPerp>.10&&radial(pp.grip[1].p)<.07,
         "C39 root cause: the game's own left halberd wrist is >10 cm from the R16 line (refused), 6 cm from the shaft centre line");
   auto chain=[&](const ph::Profile& prof0,auto&& body){std::array<ph::Profile,2> prof{};prof[0]=prof0;prof[1].side=1;
    ph::Input i{};i.allowed=true;i.epoch=i.bodyEpoch=1;i.sample=1;i.tick=1000;i.timeNs=1000000000ull;i.tracked[0]=i.tracked[1]=true;i.intentTracked[0]=i.intentTracked[1]=true;
    i.grip[0]=1;i.grip[1]=0;i.hands[0].p={.2,.25,1.3};i.hands[1].p={-.3,.1,1.0};i.shoulders[0]={.2,0,1.3};i.shoulders[1]={-.2,0,1.3};i.reach[0]=.9;i.reach[1]=1.9;
    i.dynamics=true;i.controllerAnchor119=true;i.constraintDrive129=true;i.supportConstraint130=true;i.supportMaxAngle=55*3.14159265358979323846/180.;i.supportV120_178r27=true;i.supportAxis59c16=true;
    ph::State st{};auto clear=[](const ph::Profile&,ph::Pose,ph::Pose,bool){return 1.0;};auto o=ph::step(st,i,prof,clear);
    body(prof,i,st,o,[&](){++i.sample;i.tick+=10;i.timeNs+=10000000ull;o=ph::step(st,i,prof,clear);});};
   const pm::Quat leftQ=*pm::unit(pm::Quat{.3,.6,-.4,.6});
   // a wrist on the shaft at station t (relative to the right hand), dr beside the centre line along +x
   auto onShaft=[&](const ph::Profile& p,double t,double dr){const auto l=ph::shaft_line39(p);return l.o+l.u*t+pm::Vec3{dr,0,0};};
   auto grabAt=[&](const ph::Profile& p,ph::Input& i,ph::Output& o,auto&& adv,pm::Vec3 local,float grip){
    i.grip[1]=0;adv();for(int n=0;n<8;++n){i.hands[1].p=ph::point(o.weapons[0],local);i.hands[1].q=pm::multiply(o.weapons[0].q,leftQ);i.grip[1]=grip;adv();}(void)p;};
   // 1) native-first: closing on the captured native frame takes that authored frame
   chain(pp,[&](auto& prof,auto& i,auto& st,auto& o,auto&& adv){
    grabAt(pp,i,o,adv,pp.grip[1].p,1.f);
    Check(o.support[0]&&!st.weapon[0].supportAxis59c16&&pm::length(st.weapon[0].supportGrip169r2.p-pp.grip[1].p)<1e-9&&pm::angle(st.weapon[0].supportGrip169r2.q,pp.grip[1].q)<1e-8,
          "C39 halberd: a grasp within 14 cm of the native left frame latches that native frame (position AND rotation)");(void)prof;});
   // 2) anywhere along the haft: butt end, below, above the right hand, high on the shaft; the fist ends on the wood
   for(double t:{-.60,-.35,.25,.90}){
    chain(pp,[&](auto& prof,auto& i,auto& st,auto& o,auto&& adv){
     grabAt(pp,i,o,adv,onShaft(pp,t,.09),1.f);
     const auto& g=st.weapon[0].supportGrip169r2;const double along=pm::dot(g.p-line.o,line.u);
     const auto shown=o.displayLatched[1]?o.displayHands[1]:o.hands[1];const auto rel=ph::compose(ph::inverse(o.weapons[0]),shown);
     char name[220]{};std::snprintf(name,sizeof(name),"C39 halberd grab at %+.0f cm along the haft: latched as shaft grip, wrist on the wood (radius = native 6.1 cm), shown there (rigid slide <1.2 cm)",t*100);
     Check(o.support[0]&&o.owned[1]&&st.weapon[0].supportAxis59c16&&std::abs(along-t)<.012&&std::abs(radial(g.p)-radial(pp.grip[1].p))<.002&&
           pm::length(rel.p-g.p)<1e-6&&g.p.x>0,name);(void)prof;});}
   // the same wrist offset the game uses for the left hand, 45 cm below the right hand: R16 refused it (perp > 10 cm)
   chain(pp,[&](auto& prof,auto& i,auto& st,auto& o,auto&& adv){
    const auto nat=pp.grip[1].p+line.u*(-.45-pm::dot(pp.grip[1].p-line.o,line.u));
    const auto ax=ph::axis_grip59c16(i,prof[0],o.weapons[0],ph::point(o.weapons[0],nat));
    grabAt(pp,i,o,adv,nat,1.f);
    Check(o.support[0]&&st.weapon[0].supportAxis59c16&&ax.centre39&&ax.perp<.07,"C39 halberd: the native left-hand wrist offset 45 cm down the shaft latches (R16 line: refused)");});
   // refusals: off the shaft, beyond the butt end, light Grip
   chain(pp,[&](auto& prof,auto& i,auto& st,auto& o,auto&& adv){grabAt(pp,i,o,adv,onShaft(pp,-.35,.17),1.f);
    Check(!o.support[0]&&o.supportProbe59c10.reason==6&&!(o.supportProbe59c10.perp39<o.supportProbe59c10.perpLimit39),"C39 halberd: a squeeze 17 cm beside the shaft centre is refused (OFF_SHAFT_CENTRE)");(void)prof;(void)st;});
   chain(pp,[&](auto& prof,auto& i,auto& st,auto& o,auto&& adv){grabAt(pp,i,o,adv,onShaft(pp,-.72,.07),1.f);
    Check(!o.support[0]&&o.supportProbe59c10.reason==6&&o.supportProbe59c10.along59c16<o.supportProbe59c10.tMin39-.05,"C39 halberd: a squeeze past the butt end is refused (BEYOND_BUTT_END)");(void)prof;(void)st;});
   for(float gr:{.5f,.6f})chain(pp,[&](auto& prof,auto& i,auto& st,auto& o,auto&& adv){grabAt(pp,i,o,adv,onShaft(pp,-.35,.07),gr);
    Check(gr<.55f?(!o.support[0]&&o.supportProbe59c10.reason==5):o.support[0],gr<.55f?"C39 halberd: Grip 0.50 does not latch (GRIP_BELOW_0.55)":"C39 halberd: Grip 0.60 latches");(void)prof;(void)st;});
   // 3) slide, steer, release, re-grab
   chain(pp,[&](auto& prof,auto& i,auto& st,auto& o,auto&& adv){
    grabAt(pp,i,o,adv,onShaft(pp,-.30,.08),1.f);const auto q0=st.weapon[0].supportGrip169r2.q;const double r0=radial(st.weapon[0].supportGrip169r2.p);
    for(int n=0;n<10;++n){i.hands[1].p=ph::point(o.weapons[0],onShaft(pp,-.30-.02*(n+1),.08));adv();}
    double along=pm::dot(st.weapon[0].supportGrip169r2.p-line.o,line.u);
    Check(o.support[0]&&std::abs(along+.30)<.01&&std::abs(radial(st.weapon[0].supportGrip169r2.p)-r0)<1e-6&&pm::angle(st.weapon[0].supportGrip169r2.q,q0)<1e-9,
          "C42 halberd: held grasp stays at selected station with radial/rotation continuity");
    for(int n=0;n<10;++n){i.hands[1].p=ph::point(o.weapons[0],onShaft(pp,-.9,.08));adv();}
    along=pm::dot(st.weapon[0].supportGrip169r2.p-line.o,line.u);
    Check(o.support[0]&&std::abs(along+.30)<.003,"C42 halberd: reaching toward butt cannot slide a held grasp");
    for(int n=0;n<20;++n){i.hands[1].p=ph::point(o.weapons[0],onShaft(pp,-.02,.08));adv();}
    along=pm::dot(st.weapon[0].supportGrip169r2.p-line.o,line.u);std::printf("C39 halberd slide toward the right hand: station %.1f cm\n",along*100);
    Check(o.support[0]&&std::abs(along+.30)<.003,"C42 halberd: reaching through primary hand does not move the selected station");
    (void)prof;});
   chain(pp,[&](auto& prof,auto& i,auto& st,auto& o,auto&& adv){
    grabAt(pp,i,o,adv,onShaft(pp,-.40,.08),1.f);double along=0;i.dynamics=false; // geometry only: weight sag (gravity/compliance, ~13 deg for 9.6 kg) is the dynamics' job
    // steer: both hands own the axis
    for(int n=0;n<10;++n){i.hands[1].p=ph::point(o.weapons[0],onShaft(pp,-.40,.08));adv();}
    const pm::Vec3 primary=i.hands[0].p;const pm::Vec3 arm=i.hands[1].p-primary;
    for(int n=0;n<80;++n){i.hands[1].p=primary+pm::rotate(pm::from_rotation_vector({0,0,1.2*std::min(1.0,(n+1)/20.0)}),arm);adv();
}
    const auto loc=st.weapon[0].supportGrip169r2.p-prof[0].grip[0].p;const auto w=pm::unit(pm::rotate(o.weapons[0].q,loc));const auto want=pm::unit(i.hands[1].p-primary);
    const double err=w&&want?std::acos(std::clamp(pm::dot(*w,*want),-1.0,1.0))*57.2958:180.;std::printf("C39 halberd steer error %.1f deg\n",err);
    Check(o.support[0]&&err<3,"C39 halberd: the two-hand axis follows the support hand (69 degree turn, rigid geometry)");
    i.grip[1]=0;adv();Check(!o.support[0]&&o.owned[0],"C39 halberd: opening the left Grip releases, the right hand keeps the weapon");
    grabAt(pp,i,o,adv,onShaft(pp,.40,.08),1.f);along=pm::dot(st.weapon[0].supportGrip169r2.p-line.o,line.u);
    Check(o.support[0]&&std::abs(along-.40)<.012,"C39 halberd: re-grab above the right hand latches the new point");});
   // 4) longsword with the native left frame (Radzig's sword, 14:06 capture): socket first, then anywhere on the hilt / half-sword
   auto ls=item;ls.profile.key=0x18E22;ls.profile.generation=1;ls.profile.side=0;ls.profile.classId=4;ls.profile.twoHanded=true;
   {const float b[6]{-.10f,-.03f,-.25f,.10f,.03f,1.055f};std::copy_n(b,6,ls.bounds);ls.boundsValid=true;ls.slotValid34[0]=ls.slotValid34[1]=false;}
   PhysGrip6698 gl=gp;gl.key=ls.profile.key;gl.generation=ls.profile.generation;gl.hand[0].p={.021,-.066,.012};gl.hand[1].p={-.062,-.037,-.146};
   const auto pl=fill(ls,gl);
   chain(pl,[&](auto& prof,auto& i,auto& st,auto& o,auto&& adv){grabAt(pl,i,o,adv,pl.grip[1].p+pm::Vec3{.03,0,.02},1.f);
    Check(o.support[0]&&!st.weapon[0].supportAxis59c16&&pm::length(st.weapon[0].supportGrip169r2.p-pl.grip[1].p)<1e-9,"C39 longsword: a grasp near the native hilt frame keeps the native frame");(void)prof;});
   for(double t:{-.20,.35})chain(pl,[&](auto& prof,auto& i,auto& st,auto& o,auto&& adv){grabAt(pl,i,o,adv,onShaft(pl,t,.07),1.f);
    const double along=pm::dot(st.weapon[0].supportGrip169r2.p-ph::shaft_line39(pl).o,ph::shaft_line39(pl).u);
    Check(o.support[0]&&st.weapon[0].supportAxis59c16&&std::abs(along-t)<.002,t<0?"C39 longsword: a grasp on the pommel end (20 cm below, 14+ cm from the socket) latches (refused before)":
          "C39 longsword: half-sword grasp 35 cm up the blade latches (refused before: socket only)");
    for(int n=0;n<10;++n){i.hands[1].p=ph::point(o.weapons[0],onShaft(pl,t+.01*(n+1),.07));adv();}
    Check(o.support[0]&&std::abs(pm::dot(st.weapon[0].supportGrip169r2.p-ph::shaft_line39(pl).o,ph::shaft_line39(pl).u)-along)<1e-9,"C39 longsword: the chosen sword grasp stays fixed (no slide, C19C3)");(void)prof;});
   // 5) unchanged: one-handed weapon, bow, shield; no bounds = R16 line
   {ph::Input i{};i.supportAxis59c16=true;auto one=pp;one.twoHanded=false;auto bow=pp;bow.classId=9;auto shield=pp;shield.classId=8;
    const ph::Pose w{{0,0,0,1},{0,0,0}};const pm::Vec3 h=onShaft(pp,-.3,.06);
    Check(!ph::axis_grip59c16(i,one,w,h).eligible&&!ph::axis_grip59c16(i,bow,w,h).eligible&&!ph::axis_grip59c16(i,shield,w,h).eligible,"C39 one-handed weapons, bows and shields never get a shaft grip");
    auto nb=pp;nb.diagnosticBounds59c6=false;const auto a=ph::axis_grip59c16(i,nb,w,h);Check(a.eligible&&!a.centre39&&a.perpLimit39==.10,"C39 without item bounds the R16 axis (10 cm) is used unchanged");}
  }
  g_support_slot34=savedSlot;g_support_synthetic178r9=savedSynth;}
 ph::Profile profile{};PhysFillProfile6698(item,grips.grip[0],profile);
 Check(profile.primary&&profile.support&&profile.geometry&&profile.extent>1,"native grip and real bounds form active profile");
 Check(!profile.supportPositionOnly174r2,"captured native support retains its anatomical wrist frame");
 auto missingSupport=grips.grip[0];missingSupport.ready[1]=false;
 ph::Profile fallbackSupport{};PhysFillProfile6698(item,missingSupport,fallbackSupport);
 Check(fallbackSupport.support&&fallbackSupport.supportV120_178r27&&fallbackSupport.supportPositionOnly174r2,"startup fallback proves hilt position only, never the opposite wrist frame");
 {const auto d=fallbackSupport.grip[1].p-fallbackSupport.grip[0].p;Check(d.y<-.06&&d.y>-.13,"startup support socket lies toward the pommel before any combat capture");}
 // C19C3 execute the real profile producer, not a mirrored slider formula.
 g_support_offset_sword178r9=.06f;ph::Profile sliderNear{};PhysFillProfile6698(item,missingSupport,sliderNear);
 g_support_offset_sword178r9=.14f;ph::Profile sliderFar{};PhysFillProfile6698(item,missingSupport,sliderFar);
 Check(sliderNear.support&&sliderFar.support&&pm::length(sliderFar.grip[1].p-sliderNear.grip[1].p)>.075,
       "C19C3 sword fallback slider changes real V120 support socket within bounded hilt region");
 ph::Profile nativeUnchanged{};PhysFillProfile6698(item,grips.grip[0],nativeUnchanged);
 Check(pm::length(nativeUnchanged.grip[1].p-profile.grip[1].p)<1e-9&&pm::angle(nativeUnchanged.grip[1].q,profile.grip[1].q)<1e-9,
       "C19C3 fallback slider never overwrites the captured native off-hand frame");
 g_support_offset_sword178r9=.10f;
 SetTwoHandV120_178r27(false);ph::Profile fallback166{};PhysFillProfile6698(item,missingSupport,fallback166);SetTwoHandV120_178r27(true);
 Check(fallback166.support&&fallback166.supportPositionOnly174r2&&!fallback166.supportV120_178r27,"VrTwoHandV120=false: synthetic hilt proves position only, never opposite wrist orientation");
 {auto logged=item;auto captured=missingSupport;
  // The real run's primary and learned left wrist; the bounds here deliberately
  // form a simple containing model, not an assertion about undocumented assets.
  const float zBounds[6]{-.08f,-.10f,-.24f,.08f,.10f,1.055f};std::copy_n(zBounds,6,logged.bounds);
  captured.hand[0].p={.021,-.066,.012};captured.hand[1].p={-.007,-.030,-.184};
  captured.ready[0]=true;captured.ready[1]=false;
  ph::Profile startup{};PhysFillProfile6698(logged,captured,startup);
  Check(startup.support&&startup.supportPositionOnly174r2&&kcd2vr::support_grip166::OnHilt(startup.grip[0].p,startup.tip,startup.grip[1].p),
        "logged Z-axis primary: missing support is on hilt before any native combat animation");
  captured.ready[1]=true;ph::Profile learned{};PhysFillProfile6698(logged,captured,learned);
  Check(learned.support&&!learned.supportPositionOnly174r2&&pm::length(learned.grip[1].p-captured.hand[1].p)<1e-12,
        "logged learned left wrist remains authoritative after combat capture");
  captured.ready[1]=false;logged.bounds[2]=0;ph::Profile noHilt{};PhysFillProfile6698(logged,captured,noHilt);
  Check(!noHilt.support,"no bounded hilt refuses synthetic support rather than falling through onto blade");}
 g_support_synthetic178r9=false;
 Check(profile.axis.y>.99&&profile.tip.y>.99,"feature axis follows actual weapon bounds, not donor coordinate guess");
 auto bowItem=item;bowItem.profile.classId=9;bowItem.profile.side=1;
 const float bowBounds[6]{-.1f,-.15f,-.6f,.1f,.15f,.6f};std::copy_n(bowBounds,6,bowItem.bounds);
 auto bowGrip=grips.grip[0];bowGrip.hand[1].p={0,0,0};bowGrip.hand[0].p={0,-.15,.10};
 ph::Profile bowProfile{};PhysFillProfile6698(bowItem,bowGrip,bowProfile);
 Check(bowProfile.support&&bowProfile.axis.y<-.99&&std::abs(bowProfile.axis.z)<1e-8,"native wrist height difference does not tilt the bow draw axis upwards");
 // Exercise immutable binding requalification through the REAL consumer.
 // Callback freshness does not erase learned grips; a topology/relative change does.
 const auto stablePrimary=grips.grip[0].stable[0],stableSupport=grips.grip[0].stable[1];
 ++socket.callback;++socket.tick;body.stamp.sample=14;
 PhysCaptureGrip6698(item,natural,nullptr,body,socket.scale,&socket);g_phys_grips6698.read(grips);
 Check(grips.grip[0].ready[0]&&grips.grip[0].ready[1]&&grips.grip[0].stable[0]==stablePrimary&&grips.grip[0].stable[1]==stableSupport,
       "same binding callback refresh preserves both learned grips");
 const auto priorPhysical=g_phys_state6698.weapon[0];
 g_phys_state6698.weapon[0].weapon.p={.31,.42,.53};g_phys_state6698.weapon[0].ready=true;
 ++socket.joint;socket.relative.t[0]=.02f;natural[4]=.02f;body.stamp.sample=15;
 PhysCaptureGrip6698(item,natural,nullptr,body,socket.scale,&socket);g_phys_grips6698.read(grips);
 Check(!grips.grip[0].ready[0]&&!grips.grip[0].ready[1]&&grips.grip[0].stable[0]==1&&grips.grip[0].stable[1]==0,
       "changed coherent native binding invalidates both grips before relearning");
 Check(g_phys_state6698.weapon[0].ready&&pm::length(g_phys_state6698.weapon[0].weapon.p-pm::Vec3{.31,.42,.53})<1e-12,
       "binding invalidation never teleports or clears the checked collision seed");
 for(int n=16;n<=27;++n){body.stamp.sample=n;PhysCaptureGrip6698(item,natural,nullptr,body,socket.scale,&socket);}
 g_phys_grips6698.read(grips);
 Check(grips.grip[0].ready[0]&&grips.grip[0].ready[1]&&std::abs(grips.grip[0].hand[0].p.x+.02)<1e-6&&
       grips.grip[0].captureBinding.joint==socket.joint,
       "new binding learns fresh primary and support relations from consecutive native poses");
 g_phys_state6698.weapon[0]=priorPhysical;
 auto unstable=item;unstable.profile.key=999;unstable.profile.generation=4;
 auto unstableSocket=socket;unstableSocket.key=999;unstableSocket.generation=4;
 for(int n=30;n<50;++n){body.stamp.sample=n;body.native[1].t[0]=n%2?.6f:-.6f;PhysCaptureGrip6698(unstable,natural,nullptr,body,unstableSocket.scale,&unstableSocket);}
 g_phys_grips6698.read(grips);bool reject=false;for(auto& g:grips.grip)if(g.key==999)reject=!g.ready[1];
 Check(reject,"distant changing offhand is not silently promoted to support grip");
 item.held=false;body.stamp.sample=100;natural[4]=-.18f;natural[5]=-.15f;natural[6]=.8f;
 PhysCaptureGrip6698(item,natural,attach.data(),body);g_phys_grips6698.read(grips);Check(grips.grip[0].mountReady,"native holstered attachment establishes actual mount");
 PhysEquipment6698 equipment{};equipment.valid=true;equipment.request=1;equipment.epoch=1;equipment.tick=9990;equipment.handKnown[0]=equipment.handKnown[1]=true;equipment.handKey[0]=777;equipment.heldIndex[0]=0;equipment.items[0]=item;equipment.count=1;
 g_phys_armed6698=true;g_phys_fault6698=false;g_weapon_control_mode=4;g_weapon_request_epoch6694=1;g_pose_epoch6689=1;g_phys_equipment6698.publish(equipment);
 equipment.manager=reinterpret_cast<void*>(0x1234);g_henry_item_attach_manager=equipment.manager;g_phys_equipment6698.publish(equipment);
 Check(ExactHandItemState(0)==1&&ExactHandItemState(1)==0,"fresh exact native equipment establishes occupied and empty hands without attach events");
 auto stale=equipment;stale.tick=1;g_phys_equipment6698.publish(stale);Check(ExactHandItemState(1)==-1,"stale equipment cannot authorize a free hand");g_phys_equipment6698.publish(equipment);
 ph::Input input{};input.tick=9980;std::array<ph::Profile,2> profiles{};PhysEquipment6698 read{};
 Check(PhysPrepare6698(input,profiles,read)&&profiles[0].key==777&&profiles[0].primary,"newer native metadata does not falsely reject slightly older tracking timestamp");
 equipment.request=2;g_phys_equipment6698.publish(equipment);Check(!PhysPrepare6698(input,profiles,read),"mode generation mismatch rejects old equipment packet");
 equipment.request=1;ph::Output output{};output.commandCount=1;output.commands[0].kind=ph::CommandKind::BowCancel;output.commands[0].sample=51;input.epoch=1;input.bodyEpoch=1;input.sample=51;ph::State state{};std::array<ph::HolsterState,8> holsters{};
 PhysPublish6698(input,output,profiles,equipment,body.world,state,holsters);
 Check(g_phys_ready_sample6698==51&&g_phys_commands6698.peek(c)&&c.sample==51,"frame ready marker follows its command publication");g_phys_commands6698.pop(c);
 g_weapon_control_mode=0;input.sample=52;PhysPublish6698(input,output,profiles,equipment,body.world,state,holsters);
 Check(g_phys_ready_sample6698==51&&!g_phys_commands6698.pop(c),"mode-off cannot publish physical command");
 g_weapon_control_mode=4;input.bodyEpoch=2;PhysPublish6698(input,output,profiles,equipment,body.world,state,holsters);
 Check(g_phys_ready_sample6698==51&&!g_phys_commands6698.pop(c),"recalibration epoch cannot publish stale physical command");
 // Exact pre-impact effort is visible no later than the contact frame. This is
 // the COMPLETE production publisher; no native stamina function is executed.
 {namespace mo=kcd2vr::motion59c4;namespace se=kcd2vr::swing_effort59c14;
  mo::selected=true;se::ledger.clear();g_weapon_control_mode=6;
  ph::Input pin{};pin.epoch=pin.bodyEpoch=1;pin.sample=900;pin.tick=9990;
  pin.allowed=pin.attacks=pin.tracked[0]=pin.motionOwned59c5=pin.physicalCombat177=true;
  ph::Output pout{};pout.active[0]=pout.owned[0]=pout.contactKnown[0]=true;
  auto packetProfiles=profiles;auto& pp=packetProfiles[0];pp.key=777;pp.generation=3;pp.classId=4;pp.tip={0,.8,0};pp.grip[0]={};
  auto gear=equipment;gear.valid=true;gear.player=reinterpret_cast<void*>(0x20000);gear.epoch=gear.request=1;
  gear.items[0].held=true;gear.items[0].profile=pp;
  auto& impact=pout.impactRecord178r30[0];impact.valid=true;impact.tick=pin.tick;impact.sample=pin.sample;
  impact.key=pp.key;impact.generation=pp.generation;impact.epoch=impact.request=1;impact.gesture59c3=800;
  impact.speed=2.;impact.entityId=123;impact.collider=456;impact.part=3;impact.kind=1;
  ph::impact_effort19(impact,pp,0,{}, {0,2,0},{},1.);
  const mo::Key identity{800,777,3,1,1,kcd2vr::contact176::recoveryEpoch55c1.load(),0x20000};
  PhysPublish6698(pin,pout,packetProfiles,gear,body.world,state,holsters);
  se::Record cost{};Check(g_phys_ready_sample6698==pin.sample&&se::ledger.reserve(identity,pin.tick,cost)==se::Claim::Ready&&
    std::abs(cost.factor-se::factor(2./3.))<1e-8,"actual frame publication carries same-contact immutable effort before frame readiness");
  se::ledger.clear();++pin.sample;pin.tick+=10;
  PhysPublish6698(pin,pout,packetProfiles,gear,body.world,state,holsters);
  Check(se::ledger.reserve(identity,pin.tick,cost)==se::Claim::Missing,"retained old impact cannot republish or refresh an effort record");
  se::ledger.clear();mo::selected=false;g_weapon_control_mode=4;
 }
 g_phys_armed6698=false;
 // C54: production item description accepts exact equipped metadata without a
 // world model. This does not synthesize a collider, grip, entity or attachment.
 {
  std::array<unsigned char,0xA0> nativeItem{};std::array<unsigned char,0x130> definition{};
  std::array<unsigned char,0x30> weaponClass{};std::array<unsigned char,0x38> worldLink{};
  auto put=[](unsigned char* p,const auto& value){std::memcpy(p,&value,sizeof(value));};
  const auto base=WhBase();const std::uintptr_t iv=base+0x3A2DA88,dv=base+0x3A4E390;
  put(nativeItem.data(),iv);put(definition.data(),dv);
  void* def=definition.data();void* cls=weaponClass.data();put(nativeItem.data()+0x48,def);put(definition.data()+0xE8,cls);
  const std::uint64_t key=725;put(nativeItem.data()+0x30,key);const int id=7;put(weaponClass.data()+8,id);
  const unsigned char two=1,right=1;put(weaponClass.data()+0x24,two);put(weaponClass.data()+0x25,right);
  const float mass=3.7f;put(definition.data()+0x58,mass);const int sr=15,ar=8;put(definition.data()+0x118,sr);put(definition.data()+0x11C,ar);
  PhysEquipmentItem6698 pole{};
  Check(PhysDescribeItem54(key,nativeItem.data(),pole)&&pole.profile.classId==7&&pole.profile.side==0&&
    pole.profile.twoHanded&&std::abs(pole.profile.weight-mass)<1e-6&&pole.profile.strReq59c17==15&&
    !pole.profile.entity&&!pole.entity&&!pole.boundsValid&&!pole.slotValid34[0]&&!pole.slotValid34[1],
    "C54 exact stowed polearm metadata survives null world link without inventing geometry");
  Check(PhysCachedItemMatches54(pole,key),"C54 metadata-only stowed cache remains reusable for the same item");
  Check(!PhysDescribeItem54(key+1,nativeItem.data(),pole),"C54 another WUID cannot borrow stowed metadata");
  Check(PhysDescribeItem54(key,nativeItem.data(),pole),"C54 exact metadata restored after refused foreign key");
  void* link=worldLink.data();const std::uint32_t entity=44;put(worldLink.data()+0x30,entity);put(nativeItem.data()+0x88,link);
  Check(!PhysCachedItemMatches54(pole,key),"C54 spawning the world link invalidates the metadata-only profile cache");
  Check(PhysDescribeItem54(key,nativeItem.data(),pole)&&pole.worldLink54==link&&pole.profile.entity==44,
    "C54 live world link retains its exact entity identity");
  Check(!PhysCachedItemMatches54(pole,key),"C54 existing entity ID without resolved model retries instead of caching missing geometry forever");
  pole.entity=worldLink.data();Check(PhysCachedItemMatches54(pole,key),"C54 resolved same-link model can reuse metadata cache");
  link=nullptr;put(nativeItem.data()+0x88,link);Check(!PhysCachedItemMatches54(pole,key),"C54 holstering invalidates previous live-model cache");
  const unsigned char invalidHand=2;put(weaponClass.data()+0x25,invalidHand);
  Check(!PhysDescribeItem54(key,nativeItem.data(),pole),"C54 invalid native hand field still rejects metadata");
 }
 // Equipment columns and anatomical hands differ for bows/crossbows. Exercise
 // the actual production preflight and gesture adapter, without native calls.
 {
  PhysEquipment6698 gear{};gear.count=1;gear.selected=0;gear.handKnown[0]=gear.handKnown[1]=true;
  auto& bow=gear.items[0];bow.set=1;bow.slot126=0;bow.profile.key=901;bow.profile.generation=1;bow.profile.classId=9;bow.profile.side=1;
  std::array<unsigned char,0x100> manager{};gear.manager=manager.data();
  std::memcpy(manager.data()+0x48+16,&bow.profile.key,8);
  ph::Command command{};command.key=901;command.generation=1;command.side=1;command.set=1;
  Check(PhysEquipmentSlot126(gear,command),"left-held bow validates actual main-column key, not empty off-column");
  command.side=0;Check(!PhysEquipmentSlot126(gear,command),"wrong native hand cannot borrow loadout slot proof");command.side=1;
  manager[0x48+16]=0;Check(!PhysEquipmentSlot126(gear,command),"changed native slot revokes equipment request");
  std::memcpy(manager.data()+0x48+16,&bow.profile.key,8);
  Check(PhysDrawCandidate126(gear,0),"unique stored bow can be selected from a different empty-hands loadout");
  gear.items[1]=bow;gear.items[1].profile.key=902;gear.items[1].set=2;gear.count=2;
  Check(!PhysDrawCandidate126(gear,0)&&!PhysDrawCandidate126(gear,1),"ambiguous stored shoulder weapons do not select arbitrarily");
  gear.selected=1;Check(PhysDrawCandidate126(gear,0)&&!PhysDrawCandidate126(gear,1),"selected matching weapon resolves shared gesture zone");
  gear.count=1;gear.selected=0;gear.handKey[0]=123;
  Check(!PhysDrawCandidate126(gear,0),"cross-set draw never sheaths a held opposite-hand item");gear.handKey[0]=0;
  ph::Input in{};in.epoch=in.bodyEpoch=1;in.tick=1000;in.sample=1;in.requirePrimaryGrip114=in.allowed=true;
  in.tracked[0]=in.tracked[1]=in.intentTracked[0]=in.intentTracked[1]=true;
  in.shoulders[0]={.2,0,1.3};in.shoulders[1]={-.2,0,1.3};
  ph::Vec3 bowMount{};(void)ph::holster_mount126(in,bow.profile,bowMount);in.hands[1].p=bowMount; // V66.147: bow over the RIGHT shoulder
  std::array<ph::HolsterState,8> state{};ph::Output out{};
  auto gesture=[&](){out={};++in.sample;in.tick+=20;PhysHolsterGestures6698(in,gear,out,state);};
  gesture();in.grip[1]=1;gesture();in.hands[1].p.y+=.45; // 30 cm zone: pull past 36 cm
  for(int n=0;n<4;++n)gesture();
  Check(out.commandCount==1&&out.commands[0].kind==ph::CommandKind::Draw&&out.commands[0].side==1&&out.commands[0].set==1,
    "production left-hand RIGHT-shoulder pull requests stored bow draw");
  gesture();Check(out.commandCount==0,"pending draw does not spam native requests");
  bow.held=true;gear.handKey[1]=901;gear.selected=1;gesture(); // native draw acknowledgement
  (void)ph::holster_mount126(in,bow.profile,bowMount);in.grip[1]=1;in.hands[1].p=bowMount;gesture();
  for(int n=0;n<3;++n)gesture(); // dwell inside physical shoulder zone
  in.hands[1].p.y+=.27;for(int n=0;n<4;++n)gesture(); // ordinary body/hand drift must not erase candidate
  in.grip[1]=0;unsigned stows=0;
  for(int n=0;n<8;++n){gesture();for(unsigned k=0;k<out.commandCount;++k)if(out.commands[k].kind==ph::CommandKind::Holster)++stows;} // V146R1: 120 ms release debounce
  Check(stows==1,"R1 sticky shoulder latch survives drift and Grip release sends one native holster intent");
  bow.held=false;gear.handKey[1]=0;gesture();Check(!state[0].comparison114.pending,"only changed native hand membership acknowledges holster");
  ph::Profile p{};ph::Vec3 mount{};
  p.classId=9;Check(ph::holster_hand126(p)==1&&ph::holster_mount126(in,p,mount)&&mount.x>0,"V147 bow: left hand, RIGHT shoulder");
  for(int id:{10,13,14,15}){p.classId=id;Check(ph::holster_hand126(p)==(id==13?0:1)&&ph::holster_mount126(in,p,mount)&&mount.x<0,"C45 ranged: native primary hand, lower LEFT back");}
  p.classId=7;Check(ph::holster_hand126(p)==0&&ph::holster_mount126(in,p,mount)&&mount.x>0,"C45 halberd: right hand, RIGHT shoulder");
  p.classId=8;Check(ph::holster_hand126(p)==1&&ph::holster_mount126(in,p,mount)&&mount.x<0,"R14B shield: left hand, LEFT shoulder");
  p.classId=4;p.twoHanded=true;Check(ph::holster_zone126(p)==ph::HolsterZone126::NativeBelt,"V146R1 long sword (game-flagged two-handed): native HIP scabbard");p.twoHanded=false;
  for(int id:{1,2,16}){p.classId=id;Check(ph::holster_zone126(p)==ph::HolsterZone126::NativeBelt&&!ph::holster_mount126(in,p,mount),"one-handed weapon never invents a fixed hip location");}
  PhysEquipmentItem6698 belt{};belt.profile=p;belt.profile.classId=1;belt.profile.key=903;belt.profile.generation=7;
  PhysGripBank6698 mounts{};auto& captured=mounts.grip[0];captured.key=903;captured.generation=7;captured.mountReady=true;
  captured.mountStamp126.epoch=1;captured.mountStamp126.tick=GetTickCount64();captured.mount.p={-.25,0,.85};
  Check(PhysGestureMount126(in,belt,mounts,in.hands[0].p,mount)&&mount.x<0,"sword follows observed left belt attachment on current equipment");
  captured.mount.p.x=.28;Check(PhysGestureMount126(in,belt,mounts,in.hands[0].p,mount)&&mount.x>0,"changed native belt attachment moves the acquisition zone");
  captured.mountStamp126.tick-=101;Check(!PhysGestureMount126(in,belt,mounts,in.hands[0].p,mount),"stale native mount cannot authorize adaptive belt draw");
  p.classId=12;Check(!ph::holster_mount126(in,p,mount),"fists are not a sheathed item");
 }
 {
  std::array<unsigned char,0xB00> character{};std::array<unsigned char,0xB0> attachment{},manager{};
  const auto oldModule=hostModuleHandleW;hostModuleHandleW=character.data();const auto base=WhBase();
  auto put=[&](unsigned char* p,const auto& value){std::memcpy(p,&value,sizeof(value));};
  const auto cv=base+0x40602F8ull,av=base+0x3A657C0ull;put(character.data(),cv);put(attachment.data(),av);
  auto* mp=manager.data();auto* cp=character.data();put(attachment.data()+0x28,mp);put(manager.data()+0x70,cp);
  const float cworld[8]{0,0,.70710678f,.70710678f,10,20,30,1};std::memcpy(character.data()+0xA08,cworld,sizeof(cworld));
  ModelWorld6692 world{};const auto identity=reinterpret_cast<std::uintptr_t>(cp);
  kcd2vr::retail_build112::hostRetailVerified112=false;
  Check(!PhysAttachmentWorld164(attachment.data(),identity,world),"current-world lookup rejects unverified retail");
  kcd2vr::retail_build112::hostRetailVerified112=true;
  Check(PhysAttachmentWorld164(attachment.data(),identity,world),"exact attachment character supplies its current world");
  const auto placed=PhysCurrentWorld164(ph::Pose{{},{1,0,0}},world);
  Check(pm::length(placed.p-pm::Vec3{10,21,30})<1e-5,"turn and translation rebase the accepted local weapon with Henry");
  Check(!PhysAttachmentWorld164(attachment.data(),identity+8,world),"another character cannot borrow a current-world lease");
  put(attachment.data(),cv);Check(!PhysAttachmentWorld164(attachment.data(),identity,world),"wrong attachment type fails closed");
  kcd2vr::retail_build112::hostRetailVerified112=false;hostModuleHandleW=oldModule;
 }
 {
  const auto oldBase=hostModuleHandleW;const auto oldPin=g_wh_pinned104.exchange(0);const auto oldManager=g_henry_item_attach_manager.load();const int oldMode=g_weapon_control_mode.load();
  std::uint64_t sentinel=0xfefefefefefefefeull;
  hostModuleHandleW=reinterpret_cast<void*>(reinterpret_cast<std::uintptr_t>(&sentinel)-0x5325B60ull);
  const auto base=WhBase();std::array<unsigned char,0x80> manager{};std::array<unsigned char,0xA00> player{};std::array<unsigned char,0x280> slots{};
  auto put=[](unsigned char* to,const auto& value){std::memcpy(to,&value,sizeof(value));};
  const auto mv=base+0x3EC9958ull,pv=base+0x3AA4B98ull;put(manager.data(),mv);put(player.data(),pv);
  void* m=manager.data();void* p=player.data();void* slot=slots.data();put(manager.data()+0x38,p);put(player.data()+0x9D0,m);put(player.data()+0x990,slot);
  g_henry_item_attach_manager=m;g_weapon_control_mode=0;kcd2vr::retail_build112::hostRetailVerified112=true;
  g_native_side_item_state[0]=1; // stale historical attachment cannot claim the empty hand
  Check(ExactHandItemState(0)==0,"F10-only occupancy uses live empty hand despite stale attach record");
  put(slots.data()+0x260,sentinel);Check(ExactHandItemState(0)==0,"native empty WUID sentinel also authorizes free-hand contact");
  const std::uint64_t key=445;put(slots.data()+0x260,key);Check(ExactHandItemState(0)==1,"live held item yields free-hand contact");
  put(player.data()+0x9D0,p);Check(ExactHandItemState(0)==-1,"broken Henry backlink stays unknown");
  hostModuleHandleW=oldBase;g_wh_pinned104=oldPin;g_henry_item_attach_manager=oldManager;g_weapon_control_mode=oldMode;kcd2vr::retail_build112::hostRetailVerified112=false;
 }
 {
  const auto character=g_bow_char128.load();g_bow_char128=reinterpret_cast<void*>(0x1122);
  BowGeometry166 geometry{};geometry.key=55;geometry.generation=3;geometry.request=7;geometry.epoch=8;geometry.tick=1000;
  geometry.character=g_bow_char128.load();geometry.nock={0,-.15,.1};geometry.axis={0,-1,0};geometry.drawLength=.548;
  PhysEquipment6698 gear{};gear.request=7;gear.epoch=8;ph::Profile bow{};bow.key=55;bow.generation=3;bow.classId=9;bow.primary=true;
  geometry.entity=reinterpret_cast<void*>(0x2233);geometry.pose=reinterpret_cast<void*>(0x3344);geometry.owner=reinterpret_cast<void*>(0x4455);
  geometry.poseType=0x5566;geometry.scale=1;geometry.lifetime=1;
  bowgeom59::Placement placed{static_cast<const bowgeom59::Identity&>(geometry),1,1050,1050,1};
  g_bow_placement59c10r2.publish(placed);
  gear.valid=true;gear.tick=1050;gear.count=1;gear.heldIndex[1]=0;gear.handKnown[1]=true;gear.handKey[1]=55;
  gear.items[0].held=true;gear.items[0].entity=geometry.entity;gear.items[0].profile=bow;
  Check(PhysApplyBowGeometry166(geometry,gear,bow,1050)&&bow.support&&bow.bowDrawLength166==.548,"same-item correlated named bow geometry replaces wrist-derived nock/axis/draw span");
  gear.tick=1200;
  Check(!PhysApplyBowGeometry166(geometry,gear,bow,1200),"stale current placement cannot authorize cached local geometry");
  placed.tick=placed.sourceTick=1200;g_bow_placement59c10r2.publish(placed);
  Check(PhysApplyBowGeometry166(geometry,gear,bow,1200),"fresh current placement reuses measured local geometry past old 150 ms expiry");
  gear.tick=1050;placed.tick=placed.sourceTick=1050;g_bow_placement59c10r2.publish(placed);
  ++gear.epoch;Check(!PhysApplyBowGeometry166(geometry,gear,bow,1050),"recenter invalidates copied bow geometry");--gear.epoch;
  ++bow.key;Check(!PhysApplyBowGeometry166(geometry,gear,bow,1050),"another bow cannot borrow learned nock");
  g_bow_char128=character;
  g_bow_placement59c10r2.publish({});
  g_phys_native6698={};g_phys_input6698=true;
  PhysCancelNative6698(false);Check(g_phys_input6698.load(),"local bow cancel retains input ownership until real frame/lifecycle loss");
  PhysCancelNative6698();Check(g_phys_input6698.load()==0,"lifecycle cancellation releases physical input ownership");
 }
 {
  const auto oldTick=hostTickCount64;const auto oldMode=g_weapon_control_mode.load();
  const auto oldLab=kcd2vr::diagnostics::lab::Get(kcd2vr::diagnostics::lab::Key::F3);
  hostTickCount64=20000;g_weapon_control_mode=6;g_phys_fault6698=false;g_gameplay_allowed=true;
  g_phys_input6698=true;g_phys_input_tick6698=hostTickCount64;
  kcd2vr::diagnostics::lab::Set(kcd2vr::diagnostics::lab::Key::F3,0);
  PhysFrame6698 f{};f.input.sample=123;f.input.tick=hostTickCount64;f.output.active[1]=true;f.profiles[1].classId=9;
  f.input.quiverGesture114=true;f.input.bowStringTrigger160=false;g_phys_frame6698.publish(f);
  Check(!PhysicalWeaponConsumesRightTrigger139(),"manual Grip-string leaves RT to native transport");
  f.input.bowStringTrigger160=true;g_phys_frame6698.publish(f);
  Check(PhysicalWeaponConsumesRightTrigger139(),"manual Trigger-string consumes RT exactly once");
  f.input.bowTrigger=true;f.input.quiverGesture114=false;g_phys_frame6698.publish(f);
  Check(PhysicalWeaponConsumesRightTrigger139(),"simplified Trigger bow still owns RT");
  f.profiles[1].classId=4;g_phys_frame6698.publish(f);
  Check(!PhysicalWeaponConsumesRightTrigger139(),"non-bow never borrows bow RT ownership");
  f.profiles[1].classId=9;f.input.tick=hostTickCount64-101;g_phys_frame6698.publish(f);
  Check(!PhysicalWeaponConsumesRightTrigger139(),"stale bow frame cannot consume RT");
  f.input.tick=hostTickCount64;g_phys_frame6698.publish(f);kcd2vr::diagnostics::lab::Set(kcd2vr::diagnostics::lab::Key::F3,1);
  Check(!PhysicalWeaponConsumesRightTrigger139(),"native reference releases RT");
  kcd2vr::diagnostics::lab::Set(kcd2vr::diagnostics::lab::Key::F3,0);g_gameplay_allowed=false;
  Check(!PhysicalWeaponConsumesRightTrigger139(),"menu/gameplay loss releases RT");
  kcd2vr::diagnostics::lab::Set(kcd2vr::diagnostics::lab::Key::F3,oldLab);hostTickCount64=oldTick;g_weapon_control_mode=oldMode;
 }
 {
  std::array<unsigned char,0x80> manager{};PhysEquipment6698 gear{};
  gear.manager=manager.data();gear.count=1;auto& item=gear.items[0];
  item.set=3;item.slot126=2;item.profile.key=321;item.profile.generation=7;item.profile.side=0;item.profile.classId=7;
  std::memcpy(manager.data()+0x78,&item.profile.key,sizeof(item.profile.key));
  ph::Command command{};command.set=3;command.side=0;command.key=321;command.generation=7;
  Check(PhysEquipmentSlot126(gear,command),"C45 exact native extra-key polearm authorizes its logical slot");
  command.key=322;Check(!PhysEquipmentSlot126(gear,command),"C45 extra-key rejects another item");command.key=321;
  command.generation=8;Check(!PhysEquipmentSlot126(gear,command),"C45 extra-key rejects another generation");command.generation=7;
  item.profile.classId=11;Check(!PhysEquipmentSlot126(gear,command),"C45 extra-key rejects non-polearm class");item.profile.classId=7;
  item.profile.classId=0;Check(!PhysDrawCandidate126(gear,0),"C45 chest gesture cannot invent right-hand dagger ownership");item.profile.classId=7;
  gear.valid=true;gear.selected=0;gear.handKnown[0]=gear.handKnown[1]=true;
  Check(PhysDrawCandidate126(gear,0),"C54 exact extra-slot polearm is a draw candidate with both native hands empty");
  ph::Input in{};in.epoch=in.bodyEpoch=1;in.tick=1000;in.sample=1;in.requirePrimaryGrip114=in.allowed=true;
  in.tracked[0]=in.tracked[1]=in.intentTracked[0]=in.intentTracked[1]=true;
  in.shoulders[0]={.2,0,1.3};in.shoulders[1]={-.2,0,1.3};ph::Vec3 mount{};
  ph::holster_mount126(in,item.profile,mount);in.hands[0].p=mount;
  std::array<ph::HolsterState,8> states{};ph::Output output{};
  auto sample=[&](){output={};++in.sample;in.tick+=20;PhysHolsterGestures6698(in,gear,output,states);};
  sample();Check(output.cueKey[0]==item.profile.key&&output.cueGap[0]<.001,
    "C54 metadata-only heavy weapon exposes right-shoulder proximity cue without requiring model grips");
  in.grip[1]=1;sample();Check(!states[0].comparison114.latched,
    "C54 left-hand grip cannot latch the right-handed polearm");in.grip[1]=0;
  in.grip[0]=1;sample();in.hands[0].p.y+=.20;
  for(int n=0;n<4;++n)sample();
  Check(output.commandCount==1&&output.commands[0].kind==ph::CommandKind::Draw&&output.commands[0].side==0&&
    output.commands[0].set==3&&output.commands[0].key==item.profile.key&&PhysEquipmentSlot126(gear,output.commands[0]),
    "C54 open right hand above right shoulder then Grip and pull emits one exact extra-slot polearm Draw");
  sample();Check(!output.commandCount,"C54 pending heavy-weapon draw never repeats before native hand acknowledgement");
  gear.handKey[0]=item.profile.key;item.held=true;sample();Check(!states[0].comparison114.pending,
    "C54 native right-hand membership acknowledges heavy draw");
  in.grip[0]=0;sample();Check(output.commandCount==1&&output.commands[0].kind==ph::CommandKind::Holster,
    "C54 primary Grip release requests immediate heavy-weapon sheath");
  in.allowed=false;sample();Check(!output.commandCount&&!states[0].comparison114.pending,
    "C54 menu/disallowed scope clears heavy holster state");
  const std::uint64_t changed=999;std::memcpy(manager.data()+0x78,&changed,sizeof(changed));
  Check(!PhysEquipmentSlot126(gear,command),"C45 changed native extra-key revokes admission");
 }
 std::printf("physical_bridge=%u/%u native function-address calls=NOT_EXECUTED\n",checks-failures,checks);return failures?1:0;
}
