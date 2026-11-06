static bool PhysHolsterCaptureD48(std::uint64_t now) noexcept {
    namespace cap=kcd2vr::diagnostics::capture168;
    return cap::active(cap::Domain::Interaction,now)||cap::active(cap::Domain::Bug,now);
}
static bool PhysGestureMount126(const ph::Input& in,const PhysEquipmentItem6698& item,const PhysGripBank6698& grips,pm::Vec3 hand,pm::Vec3& mount) noexcept {
    if(ph::shoulder_holster_closest131(in,item.profile,hand,mount))return pm::finite(mount);
    if(ph::holster_mount126(in,item.profile,mount))return pm::finite(mount);
    if(ph::holster_zone126(item.profile)!=ph::HolsterZone126::NativeBelt)return false;
    const auto now=std::uint64_t(GetTickCount64());const int gestureSide=ph::holster_hand126(item.profile);
    for(const auto& g:grips.grip)if(g.key==item.profile.key&&g.generation==item.profile.generation&&g.mountReady&&
        g.mountStamp126.epoch==in.bodyEpoch&&g.mountStamp126.tick&&now>=g.mountStamp126.tick&&now-g.mountStamp126.tick<=100){
        // Current native holstered item transform, with the captured hilt grip
        // when available. Before first draw, the observed attachment origin is
        // an approximate acquisition point, never a claimed measured hand socket.
        mount=g.ready[gestureSide]?ph::point(g.mount,g.hand[gestureSide].p):g.mount.p;return pm::finite(mount);
    }
    return false;
}
// Native offhand shields/torches and the separate throwing-knife carrier are
// not a second main weapon. This only controls a NEW equipment acquisition;
// an already held weapon still receives its normal release/stow operation.
static bool PhysMainWeaponR27(const PhysEquipment6698& e,const PhysEquipmentItem6698& item) noexcept {
    const auto& p=item.profile;
    if(p.classId==0&&p.side==1&&PhysSupplyHeldT2(e,item,1))return false;
    return p.classId>=0&&p.classId!=12&&!p.shield176()&&!p.torch40();
}
static bool PhysAnotherMainHeldR27(const PhysEquipment6698& e,int index) noexcept {
    if(index<0||index>=e.count||e.count>int(e.items.size())||!PhysMainWeaponR27(e,e.items[index]))return false;
    for(int j=0;j<e.count;++j)if(j!=index&&e.items[j].held&&PhysMainWeaponR27(e,e.items[j]))return true;
    return false;
}
static bool PhysDrawCandidatePolicyR38(const PhysEquipment6698& e,int index,bool currentMainBlocks) noexcept {
    if(index<0||index>=e.count||e.count>int(e.items.size()))return false;
    const auto& wanted=e.items[index];
    if(currentMainBlocks&&PhysAnotherMainHeldR27(e,index))return false;
    if(wanted.profile.classId==0)return false; // C45: dedicated dagger slot/right-hand ownership not proven
    const auto zone=ph::holster_zone126(wanted.profile);
    const int gesture=ph::holster_hand126(wanted.profile);
    if(zone==ph::HolsterZone126::None||gesture<0)return false;
    int candidate=-1,selected=-1,count=0,selectedCount=0,third=-1,thirdCount=0;
    for(int j=0;j<e.count;++j){const auto& item=e.items[j];const int side=item.profile.side;
        if(item.held||item.set<0||item.set>3||item.slot126<0||item.slot126>2||
           (item.set==3&&(item.slot126!=2||item.profile.classId!=7))||side<0||side>1||
           ph::holster_zone126(item.profile)!=zone||ph::holster_hand126(item.profile)!=gesture||
           !e.handKnown[side]||!e.handKnown[gesture]||e.handKey[side]||e.handKey[gesture]||
           (item.set!=e.selected&&(e.handKey[0]||e.handKey[1])))continue;
        candidate=j;++count;if(zone==ph::HolsterZone126::RightBackR12&&item.set==2&&item.slot126==0){third=j;++thirdCount;}if(item.set==e.selected){selected=j;++selectedCount;}
    }
    // The third main loadout wins its back slot. Otherwise prefer the selected
    // matching loadout, then a sole stored item; never choose between ambiguous keys.
    if(thirdCount)return thirdCount==1&&index==third; // one explicit third-loadout back slot wins
    return selectedCount==1?index==selected:selectedCount==0&&count==1&&index==candidate;
}
// Normal draws always require the other main weapon to be stowed. HS2
// preflight alone may inspect the wanted item while a represented current
// weapon awaits its native stow; that preflight cannot emit a Draw.
static bool PhysDrawCandidate126(const PhysEquipment6698& e,int index) noexcept {
    return PhysDrawCandidatePolicyR38(e,index,true);
}
// V66.136 holster policy (INI HolsterZoneRadius / HolsterOnGripRelease).
std::atomic<float> g_holster_radius136{.24f};
std::atomic<bool> g_holster_release_anywhere136{true}; // V66.146R1 user choice: release grip anywhere = holster
// V66.146R1 true bow reload (INI BowManualReload / BowNativeGrabAtQuiver).
std::atomic<bool> g_bow_manual_reload146{false},g_bow_native_grab146{false};
// V66.160: manual bow string held with the Trigger (INI BowStringTrigger, default ON).
std::atomic<bool> g_bow_string_trigger160{true};
// V66.169R1 F2 rung 2: retained bow-distance comparison; no UNKNOWN contact shortcuts.
std::atomic<bool> g_experiments169{false};
std::atomic<std::uint64_t> g_holster_requests136{0};
// Clear confirmation buzz on the gesture hand when a draw/holster is requested.
static void HolsterConfirmPulse136(int side,std::uint64_t key,const ph::Input& in) noexcept {
    namespace hp=kcd2vr::haptics101;if(side<0||side>1||!hp::enabled.load()||!hp::allowed.load())return;
    (void)hp::queue[side].publish(hp::Pulse{hp::epoch.load(),in.sample,in.tick,key?key:1,.85f,90});
}
// HS2 (Claude 25 Oct): weapon switch instead of a silent refusal. H50 log: the polearm had 222 zone samples and
// 0 draws because PhysDrawCandidate126 refuses an item of another loadout while EITHER hand holds a native item
// (bow held 10:35:14, sword held 10:38:37). Now a completed draw gesture on such an item first stows what is held
// (native stow, same command path as a release), then draws the wanted item as soon as the hands are free while
// Grip is still closed (3 s). if the game keeps the held item, one visible line says so.
// HS2b: no global pending switch any more - it is ph::HolsterState::switchHS2 in states[0] of the pose transaction.
static const char* PhysClassNameHS2(int c) noexcept {
    switch(c){case 7:return "polearm";case 9:return "bow";case 8:case 17:return "shield";case 10:case 14:case 15:return "crossbow";
    case 13:return "long gun";case 3:case 5:case 6:return "hip weapon";default:return "weapon";}
}
static void PhysHolsterSayHS2(const char* text,std::uint64_t now) noexcept {
    static std::uint64_t last=0;if(last&&now>=last&&now-last<10000)return;last=now;
    (void)kcd2vr::visible_failure::publish(text,now);
    if(PhysHolsterCaptureD48(now)){char m[200]{};std::snprintf(m,sizeof(m),"HS2 HOLSTER_SAY \"%s\"",text);diagnostic_trace::Write(m);}
}
// A confirmed shoulder acquisition owns the held Grip throughout its pull.
// Before acquisition, only the same available candidate used by the haptic cue
// can take priority over an incidental Trigger+Grip squeeze behind the shoulder.
static unsigned PhysOverShoulderGrabJ28(const ph::Input& in,const PhysEquipment6698& e,
    const PhysGripBank6698& grips,const std::array<ph::HolsterState,8>& states,const bool* switchCandidates=nullptr) noexcept {
    unsigned mask=0;if(!in.allowed||e.count<0||e.count>int(e.items.size()))return 0;
    // HS: every holster zone, not only bow/back (H3 had no fist owner at all; R14 H39 let a Grip+Trigger fist
    // take the hand at the hip, chest and shield too - J27 FIST_TRIGGER_GRIP / GRIP_CLOSED_AT_ENTRY).
    const double baseR=std::clamp(double(g_holster_radius136.load()),.10,.40);
    for(int i=0;i<e.count;++i){const auto& item=e.items[i];
        const auto zoneHS=ph::holster_zone126(item.profile);
        if(item.held||item.set<0||zoneHS==ph::HolsterZone126::None||item.profile.torch40())continue;
        const double radius=std::min(.45,baseR+((zoneHS==ph::HolsterZone126::LeftShoulder||ph::over_shoulder_zone_hs2(zoneHS))?.04:0.0));
        const int h=ph::holster_hand126(item.profile),side=item.profile.side;
        const bool switching=switchCandidates&&i<8&&switchCandidates[i];
        if(h<0||h>1||side<0||side>1||!in.tracked[h]||!in.intentTracked[h]||!e.handKnown[side]||
           (e.handKey[side]&&e.handKey[side]!=item.profile.key)||(PhysAnotherMainHeldR27(e,i)&&!switching))continue;
        const bool acquired=in.requirePrimaryGrip114&&equipment114::HeldAcquisitionR15(states[i].comparison114,
            item.profile.key,item.profile.generation,in.epoch,in.tick,in.grip[h]);
        if(acquired){mask|=1u<<h;continue;}
        if(!PhysDrawCandidate126(e,i)&&!switching)continue;
        pm::Vec3 mount{};if(PhysGestureMount126(in,item,grips,in.hands[h].p,mount)&&
            ph::holster_grab_hs(in,item.profile,in.hands[h].p,mount,radius))mask|=1u<<h;
    }
    return mask;
}
// R8d (F2 rung 1, vrik/holster_diag_r8d.h via physical_weapon.h): read-only holster attempt diagnostics. Mirrors the real gates as reason codes; never
// changes eligibility, FSM state or commands. Zero work unless the Bug domain (F2 rung 1) is armed.
// R9 lane A: a bow in hand makes the RIGHT hand the arrow hand. the arrow reach over the right shoulder sits inside
// the HS2 polearm zone -> reload drew the polearm (16:51:38). no right-hand draw or weapon switch while a bow is held.
static bool PhysBowHeldR9(const PhysEquipment6698& e) noexcept {
    if(e.count<0||e.count>int(e.items.size()))return false;
    for(int j=0;j<e.count;++j)if(e.items[j].held&&e.items[j].profile.bow())return true;
    // D83R1: the same right-shoulder quiver serves the XR34 crossbow owner.
    // Match all existing reload entry modes; this is draw arbitration only.
    // Keep belt draws, the left hand and stowing on their established paths.
    for(int j=0;j<e.count;++j)if(e.items[j].held&&kcd2vr::ranged_kind_xr32::crossbow(e.items[j].profile.classId))
        return kcd2vr::instant_d72::Reloads()||ReloadMode47C2()==2||
            kcd2vr::ranged_options_d65c::crossbowReload.load(std::memory_order_acquire);
    return false;
}
static kcd2vr::holster_diag_r8d::Refuse PhysDrawRefuseR8d(const ph::Input& in,const PhysEquipment6698& e,int index,
    const std::array<ph::HolsterState,8>& states) noexcept {
    using R=kcd2vr::holster_diag_r8d::Refuse;
    if(index<0||index>=e.count||e.count>int(e.items.size()))return R::NotEquipped;
    const auto& w=e.items[index];const int side=w.profile.side;
    if(w.profile.classId==0)return R::DaggerSlot;
    const auto zone=ph::holster_zone126(w.profile);const int gesture=ph::holster_hand126(w.profile);
    if(zone==ph::HolsterZone126::None||gesture<0||gesture>1)return R::NoMount;
    if(w.held)return R::ItemHeld;
    if(w.set<0||w.set>3||w.slot126<0||w.slot126>2||(w.set==3&&(w.slot126!=2||w.profile.classId!=7))||side<0||side>1)return R::NotEquipped;
    if(!e.valid||!e.handKnown[side]||!e.handKnown[gesture])return R::NativeNotReady;
    if(kcd2vr::bow_arm_aim_r9::arrow_hand_blocks_draw(gesture,PhysBowHeldR9(e),zone==ph::HolsterZone126::RightBackR12))return R::BowQuiver; // R9/R11
    if(!in.allowed||!in.tracked[gesture]||!in.intentTracked[gesture])return R::NotTracked;
    if(e.handKey[side]||e.handKey[gesture]||PhysAnotherMainHeldR27(e,index))return R::HandHolds;
    if(w.set!=e.selected&&(e.handKey[0]||e.handKey[1]))return R::OtherLoadoutHeld;
    if(!PhysDrawCandidate126(e,index))return w.set!=e.selected?R::OtherSelected:R::Ambiguous;
    for(int j=0;j<e.count;++j)if(j!=index){const auto& o=e.items[j];
        if(!o.profile.torch40()&&o.set>=0&&equipment114::PendingDrawBlocks(states[j].comparison114,o.profile.key,o.profile.generation,in.epoch,in.tick,o.held))return R::PendingOther;}
    return R::None;
}
// D63 legacy draw-source transport. This metadata belongs to the existing
// HolsterState transaction: failed publication or command capacity rolls it
// back together with the latch, and ownership/reset clears it together.
static kcd2vr::draw_motion_d63::Source PhysLegacyDrawSourceD63(const ph::Input& in,int side,
    ph::CommandKind kind,const ph::HolsterState& before,ph::HolsterState& after) noexcept {
    if(after.latched&&(!before.latched||before.key!=after.key||before.since!=after.since))
        after.drawOriginD63=ph::draw_source_d63(in,side);
    kcd2vr::draw_motion_d63::Source source{};
    if(kind==ph::CommandKind::Draw)
        source=kcd2vr::draw_motion_d63::extractedSource(ph::draw_source_d63(in,side),before.drawOriginD63);
    if(!after.latched)after.drawOriginD63={};
    return source;
}

struct HolsterDiagR8d {
    bool on=false;const ph::Input* in=nullptr;const PhysEquipment6698* e=nullptr;const ph::Output* out=nullptr;
    const std::array<ph::HolsterState,8>* states=nullptr;unsigned door=3u,talk=0u;bool f9[2]{},claim[2]{};
    int best[2]{-1,-1};bool sw[8]{};int switchTimeoutHand=-1; // R8d2: owned here, outlive the flush in the destructor
    ~HolsterDiagR8d();
};
static void PhysHolsterDiagFlushR8d(const HolsterDiagR8d& d) noexcept {
    namespace hd=kcd2vr::holster_diag_r8d;static hd::Diag diag;
    const auto& in=*d.in;const auto& e=*d.e;const auto& out=*d.out;const auto& states=*d.states;const auto now=in.tick;
    if(e.count<0||e.count>int(e.items.size()))return;
    hd::HandFrame f[2]{};PhysGripBank6698 bank{};(void)g_phys_grips6698.read(bank);
    for(int s=0;s<2;++s){f[s].grip=in.grip[s];
        f[s].claim=d.f9[s]?hd::Refuse::F9:d.claim[s]?hd::Refuse::HandClaim:(d.talk&(1u<<s))?hd::Refuse::QuickTalk:
            !(d.door&(1u<<s))?hd::Refuse::Door:
            equipment114::CombatFistOwnsHandH39(!d.claim[s]&&e.valid&&e.handKnown[s]&&!e.handKey[s],in.grip[s],in.trigger[s])?hd::Refuse::Fist:hd::Refuse::None;}
    const auto& sw=states[0].switchHS2;
    if(sw.active&&sw.hand>=0&&sw.hand<2)f[sw.hand].switchState=1;
    if(d.switchTimeoutHand>=0&&d.switchTimeoutHand<2)f[d.switchTimeoutHand].switchState=2;
    const double baseR=std::clamp(double(g_holster_radius136.load(std::memory_order_relaxed)),.10,.40);
    for(int i=0;i<e.count&&i<8;++i){const auto& item=e.items[i];
        if(item.profile.torch40()||item.set<0)continue;
        const int side=item.profile.side;const int hand=item.held?side:ph::holster_hand126(item.profile);
        if(hand<0||hand>1||side<0||side>1||f[hand].count>=8)continue;
        hd::Item x{};x.key=item.profile.key;x.classId=item.profile.classId;const auto zone=ph::holster_zone126(item.profile);x.zone=unsigned(zone);
        x.hand=hand;x.held=item.held;x.handKeyMatch=e.handKey[side]==x.key;x.handBusy=e.handKey[hand]!=0||e.handKey[side]!=0;
        pm::Vec3 mount{};x.mountKnown=PhysGestureMount126(in,item,bank,in.hands[hand].p,mount);
        if(x.mountKnown)x.gap=pm::length(in.hands[hand].p-mount);
        x.radius=std::min(.45,baseR+((zone==ph::HolsterZone126::LeftShoulder||zone==ph::HolsterZone126::RightBackR12||zone==ph::HolsterZone126::RightShoulderBowR12)?.04:0.0));
        x.volumeOk=ph::draw_volume_ok_hs2(in,item.profile,in.hands[hand].p);
        if(!item.held&&d.best[hand]>=0&&d.best[hand]!=i&&d.best[hand]<e.count){x.arbitrationWon=false;x.lostToClass=e.items[d.best[hand]].profile.classId;}
        for(int j=0;j<e.count;++j)if(e.items[j].held&&e.items[j].profile.side==hand){x.heldClass=e.items[j].profile.classId;break;}
        x.gate=item.held||d.sw[i]?hd::Refuse::None:PhysDrawRefuseR8d(in,e,i,states);
        if(x.gate==hd::Refuse::None&&!item.held&&zone==ph::HolsterZone126::RightHip45&&
           (out.hipGrantT10.owner==kcd2vr::hip_t10::Owner::Dagger||out.hipGrantT10.owner==kcd2vr::hip_t10::Owner::Axe))x.gate=hd::Refuse::HipKnife;
        const auto& st=states[i].comparison114;
        if(st.key==x.key){x.neutral=st.neutral;x.latched=st.latched;x.pending=st.pending;x.blocked=st.blocked;x.wantDrawn=st.wantDrawn;
            x.latchedMs=st.latched&&now>=st.started?now-st.started:0;}
        for(unsigned c=0;c<out.commandCount&&c<out.commands.size();++c)if(out.commands[c].key==x.key){const auto k=out.commands[c].kind;
            x.request=k==ph::CommandKind::Draw?1:k==ph::CommandKind::Holster?2:k==ph::CommandKind::DropT1?3:0;if(x.request)break;}
        f[hand].items[f[hand].count++]=x;
    }
    for(int s=0;s<2;++s){hd::Attempt a{};
        if(!hd::step(diag.hands[s],f[s],s,now,a))continue;
        diag.summary.add(a);char m[512]{};hd::format_attempt(a,m,sizeof(m));diagnostic_trace::Write(m);
        // R9: a squeeze that never reached the zone (door, apple, anything at belt/shoulder height) is not a holster
        // attempt for the player: trace + summary only, no visible line (14 of 20 R8d2 refusals were these).
        const bool reachedZoneR9=a.why!=hd::Refuse::OutsideZone&&a.why!=hd::Refuse::OutsideVolume&&a.why!=hd::Refuse::NoMount;
        if(reachedZoneR9&&kcd2vr::diagnostics::capture168::active(kcd2vr::diagnostics::capture168::Domain::Bug,now)&&hd::visible_due(diag,now)){
            char t[kcd2vr::visible_failure::kMaxText+1]{};hd::visible_text(a,t,sizeof(t));
            namespace cap=kcd2vr::diagnostics::capture168;
            (void)kcd2vr::visible_failure::publishDiagnosticD74(t,now,cap::Domain::Bug);
        }
    }
    if(now>=diag.summary.due){diag.summary.due=now+hd::kSummaryMs;char m[1024]{};hd::format_summary(diag.summary,m,sizeof(m));diagnostic_trace::Write(m);}
}
HolsterDiagR8d::~HolsterDiagR8d(){if(on&&in&&e&&out&&states)PhysHolsterDiagFlushR8d(*this);}
// Bounded F2 cancellation edges, copied from the actual caller rather than
// guessing "closed at entry" from the final state. No native read or clock.
static void PhysCancelHolsterD44(equipment114::State& state,double grip,const ph::Input& in,
    int side,std::uint64_t key,const char* reason,bool pickupOccurrenceD64=false) noexcept {
    const bool changed=state.initialized||state.latched||state.pending||
        state.requireReleaseR15!=(!pickupOccurrenceD64&&std::isfinite(grip)&&grip>=.25);
    const bool latched=state.latched,pending=state.pending,rearm=state.requireReleaseR15;
    equipment114::CancelAcquisitionR15(state,grip);
    // The exact raw-input retirement owns this release requirement. Duplicating
    // it in the pose-local latch would miss an opening during skipped poses.
    if(pickupOccurrenceD64)state.requireReleaseR15=false;
    namespace cap=kcd2vr::diagnostics::capture168;
    if(!changed||!PhysHolsterCaptureD48(in.tick))return;
    static std::uint64_t session=0;static unsigned lines=0;
    const auto domain=cap::active(cap::Domain::Interaction,in.tick)?cap::Domain::Interaction:cap::Domain::Bug;
    const auto current=(cap::gate.session[unsigned(domain)].load(std::memory_order_relaxed)<<1)|(domain==cap::Domain::Interaction?1u:0u);
    if(current!=session){session=current;lines=0;}if(lines>=48)return;
    ++lines;char line[384]{};std::snprintf(line,sizeof(line),
        "R8D HOLSTER_CANCEL_D44 session=%llu sample=%llu tick=%llu side=%d key=%016llX reason=%s grip=%.3f beforeLatched=%d beforePending=%d beforeRearm=%d afterRearm=%d budget=%u/48%s",
        (unsigned long long)session,(unsigned long long)in.sample,(unsigned long long)in.tick,side,
        (unsigned long long)key,reason,grip,latched,pending,rearm,state.requireReleaseR15,lines,
        lines==48?" detailBudgetExhausted=1":"");diagnostic_trace::Write(line);
}
// A coherent pose was published and this exact command entered the ring.
// Confirmation is queue acceptance, never a claim of native completion.
static void PhysHolsterEnqueuedD48(const ph::Input& in,const PhysEquipment6698& e,
    std::array<ph::HolsterState,8>& states,const ph::Command& command) noexcept {
    if(command.kind!=ph::CommandKind::Draw&&command.kind!=ph::CommandKind::Holster&&
       command.kind!=ph::CommandKind::DropT1)return;
    if(e.count<0||e.count>int(e.items.size()))return;
    int index=-1;
    for(int i=0;i<e.count;++i)if(e.items[i].profile.key==command.key&&e.items[i].profile.generation==command.generation){index=i;break;}
    if(index<0)return;
    const auto& item=e.items[index];
    const int hand=command.kind==ph::CommandKind::Draw?ph::holster_hand126(item.profile):command.side;
    if(hand<0||hand>1)return;
    g_holster_requests136.fetch_add(1,std::memory_order_relaxed);
    HolsterConfirmPulse136(hand,command.key,in);
    if(PhysHolsterCaptureD48(in.tick)){char line[280]{};std::snprintf(line,sizeof(line),
        "D48 HOLSTER_REQUEST stage=QUEUED kind=%s side=%d hand=%d class=%d key=%016llX generation=%llu sample=%llu nativeAck=NOT_YET",
        command.kind==ph::CommandKind::Draw?"DRAW":command.kind==ph::CommandKind::DropT1?"DROP_THROW":"HOLSTER",
        command.side,hand,item.profile.classId,(unsigned long long)command.key,
        (unsigned long long)command.generation,(unsigned long long)command.sample);diagnostic_trace::Write(line);}
    if(command.kind==ph::CommandKind::Draw)
        for(int i=0;i<e.count;++i)if(i!=index&&ph::holster_hand126(e.items[i].profile)==hand&&
            !states[i].comparison114.pending&&!states[i].pending)
            PhysCancelHolsterD44(states[i].comparison114,in.grip[hand],in,hand,e.items[i].profile.key,"OTHER_ITEM_DRAW_QUEUED");
}
static void PhysHolsterGestures6698(const ph::Input& in,const PhysEquipment6698& e,ph::Output& out,std::array<ph::HolsterState,8>& states,unsigned doorAdmissionH40,unsigned coordinationCg1) noexcept {
    // D49: speculative/query/unknown debt delays dispatch; only an ACK with
    // a concrete same-token door may consume this hand's holster Grip.
    // Busy admission leases and no-door queries preserve the original squeeze.
    const auto doorReservedD48=kcd2vr::hand_only_door_h40::Reserved();
    const auto doorAcquiredD49=kcd2vr::hand_only_door_h40::Acquired();
    doorAdmissionH40=3u&~doorAcquiredD49;
    if(PhysHolsterCaptureD48(in.tick)){
        namespace cap=kcd2vr::diagnostics::capture168;
        const auto domain=cap::active(cap::Domain::Interaction,in.tick)?cap::Domain::Interaction:cap::Domain::Bug;
        const auto session=cap::gate.session[unsigned(domain)].load(std::memory_order_relaxed);
        const auto captureKey=(session<<1)|(domain==cap::Domain::Interaction?1u:0u);
        const auto entered=kcd2vr::hand_only_door_h40::Entered();
        const unsigned signature=doorReservedD48|(entered<<2)|(doorAcquiredD49<<4)|((coordinationCg1&3u)<<6);
        static std::uint64_t lastSession=0;static unsigned previous=~0u,lines=0;
        if(captureKey!=lastSession){lastSession=captureKey;previous=~0u;lines=0;}
        if(signature!=previous&&lines<64){previous=signature;++lines;char line[384]{};
            std::snprintf(line,sizeof(line),"D49 HOLSTER_DOOR_ROUTE capture=%s session=%llu sample=%llu tick=%llu epoch=%llu reserved=%u mayOwn=%u acquired=%u cancelMask=%u coordination=%u queryPolicy=DEFER_KEEP_GRIP budget=%u/64%s",
                domain==cap::Domain::Interaction?"F5":"F2",(unsigned long long)session,(unsigned long long)in.sample,(unsigned long long)in.tick,(unsigned long long)in.epoch,
                doorReservedD48,entered,doorAcquiredD49,doorAcquiredD49,coordinationCg1,lines,lines==64?" detailBudgetExhausted=1":"");
            diagnostic_trace::Write(line);
        }
    }
    // F9 prototype has no native release-velocity transfer yet. Require a fresh
    // Grip after exact retirement; never drop/sheath alongside a live solver.
    out.hipGrantT10={};
    kcd2vr::corpse_grip_cg1::DrawOccurrence drawClaimCg1{};
    const bool drawReadyCg1=kcd2vr::corpse_grip_cg1::claims.capture(coordinationCg1,drawClaimCg1);
    namespace talkH4=kcd2vr::quick_talk_owner_h4;talkH4::Stamp talkStampH4{};
    const bool talkKnownH4=talkH4::mailbox.read(talkStampH4);
    // A contended copy is unknown, not a claim or permission for either hand.
    unsigned talkMaskH4=talkKnownH4?talkH4::current(talkStampH4,GetTickCount64(),in.bodyEpoch,in.epoch,reinterpret_cast<std::uintptr_t>(e.player)):0u;
    for(unsigned side=0;side<2;++side)if(!e.handKnown[side]||e.handKey[side])talkMaskH4&=~(1u<<side);
    const bool sharedFenceH36=NativeHeldBusyT4()||out.solverOwnedT4[0]||out.solverOwnedT4[1];
    const bool rightInhibitH36=g_native_held_release_inhibit_t4.load(std::memory_order_acquire);
    const bool rightClaimT24=HandClaimBlockedT24(0,reinterpret_cast<std::uintptr_t>(g_committed_character6689.load()),in.bodyEpoch,in.epoch);
    const bool leftClaimT24=HandClaimBlockedT24(1,reinterpret_cast<std::uintptr_t>(g_committed_character6689.load()),in.bodyEpoch,in.epoch);
    const auto pickupCharacterD64=reinterpret_cast<std::uintptr_t>(g_committed_character6689.load());
    const bool pickupRightD64=kcd2vr::pickup_grip_d64::retirement.blocked(0,pickupCharacterD64,in.bodyEpoch);
    const bool pickupLeftD64=kcd2vr::pickup_grip_d64::retirement.blocked(1,pickupCharacterD64,in.bodyEpoch);
    const bool rightFenceH36=sharedFenceH36||rightInhibitH36||rightClaimT24||pickupRightD64;
    const bool leftFenceH36=sharedFenceH36||(rightInhibitH36&&!PhysLeftHolsterRetiredH36(in,e))||leftClaimT24||pickupLeftD64;
    // R8d: observer flushed on every exit (declared first = destroyed last, after the HS2 switch write-back).
    HolsterDiagR8d diagR8d{};
    if(PhysHolsterCaptureD48(in.tick)){
        diagR8d.on=true;diagR8d.in=&in;diagR8d.e=&e;diagR8d.out=&out;diagR8d.states=&states;diagR8d.door=doorAdmissionH40;diagR8d.talk=talkMaskH4;
        diagR8d.f9[0]=sharedFenceH36||rightInhibitH36;diagR8d.f9[1]=sharedFenceH36||(rightInhibitH36&&!PhysLeftHolsterRetiredH36(in,e));
        diagR8d.claim[0]=rightClaimT24;diagR8d.claim[1]=leftClaimT24;}
    if(rightFenceH36||leftFenceH36){
        // A live shared collider still parks both hands. After its exact
        // retirement, only the original RIGHT rearm remains fenced.
        static unsigned fenceReceiptsH24=0;static std::uint64_t fenceDueH24=0;
        if(PhysHolsterCaptureD48(in.tick)&&fenceReceiptsH24<24&&in.tick>=fenceDueH24){++fenceReceiptsH24;fenceDueH24=in.tick+2000;
            char msg[224]{};std::snprintf(msg,sizeof(msg),"H24 HOLSTER_FENCE nativeBusy=%d releaseInhibit=%d solverRight=%d solverLeft=%d tick=%llu",
                NativeHeldBusyT4(),g_native_held_release_inhibit_t4.load(std::memory_order_acquire),
                out.solverOwnedT4[0],out.solverOwnedT4[1],(unsigned long long)in.tick);diagnostic_trace::Write(msg);}
        states[0].hipStateT10={};
        // A real object/reload/physics owner cancels unissued holster intent.
        // Never erase an already queued native draw or turn its release into retry.
        auto& blockedSwitchR15=states[0].switchHS2;
        if(blockedSwitchR15.active&&blockedSwitchR15.hand>=0&&blockedSwitchR15.hand<2&&
           (blockedSwitchR15.hand?leftFenceH36:rightFenceH36))blockedSwitchR15={};
        if(in.requirePrimaryGrip114&&e.count>=0&&e.count<=int(e.items.size()))
            for(int j=0;j<e.count;++j){const auto& item=e.items[j];const int hand=ph::holster_hand126(item.profile);
                if(!item.held&&hand>=0&&hand<2&&(hand?leftFenceH36:rightFenceH36)&&
                   !states[j].comparison114.pending){
                    const bool pickupOnlyD64=hand?(pickupLeftD64&&!sharedFenceH36&&
                        !(rightInhibitH36&&!PhysLeftHolsterRetiredH36(in,e))&&!leftClaimT24):
                        (pickupRightD64&&!sharedFenceH36&&!rightInhibitH36&&!rightClaimT24);
                    PhysCancelHolsterD44(states[j].comparison114,in.grip[hand],in,hand,item.profile.key,
                        pickupOnlyD64?"PICKUP_GRIP_OCCURRENCE":"OBJECT_RELOAD_PHYSICS_OWNER",pickupOnlyD64);
                }
            }
        // F4 throw test may retain ONE real primary release while the exact
        // F9 collider retires. This is a staged pose command, never a Drop here.
        heldT4::Owner owner{};PhysGripBank6698 bank{};
        if(kcd2vr::weapon_release_t1::TestEnabledH21()&&kcd2vr::weapon_release_t1::Mode()==1&&
           heldT4::ReadRetainedOwner(owner)&&owner.key&&owner.side==0&&owner.epoch==in.bodyEpoch&&owner.request==in.epoch&&
           in.allowed&&in.tracked[0]&&in.intentTracked[0]&&e.valid&&e.count>=0&&e.count<=int(e.items.size())){
            (void)g_phys_grips6698.read(bank);
            for(int i=0;i<e.count;++i){const auto& item=e.items[i];
                if(!item.held||!item.dropUnitT1||item.profile.side!=0||item.profile.key!=owner.key||
                   item.profile.generation!=owner.generation||item.profile.entity!=owner.entityId)continue;
                equipment114::Input release{};release.key=owner.key;release.generation=owner.generation;release.epoch=in.epoch;
                release.tick=in.tick;release.hand=in.hands[0].p;release.grip=in.grip[0];release.drawn=release.eligible=true;
                release.releaseAnywhere=true;release.releaseDebounceMs=0;release.radius=g_holster_radius136.load();
                release.mountKnown=PhysGestureMount126(in,item,bank,release.hand,release.mount);
                const auto before=states[i].comparison114;
                const auto command=equipment114::update(states[i].comparison114,release);
                const bool drop=kcd2vr::weapon_release_t1::DropOutsideHolster(command==equipment114::Request::Holster,
                    item.held,release.eligible&&release.grip<.25,release.mountKnown,
                    release.mountKnown?pm::length(release.hand-release.mount):1.e9,release.radius,
                    kcd2vr::weapon_release_t1::Mode());
                if(drop&&out.commandCount<out.commands.size())
                    {out.commands[out.commandCount++]={ph::CommandKind::DropT1,0,item.set,owner.key,in.epoch,in.sample,in.tick,0,{},in.bodyEpoch,owner.generation};
                     out.commands[out.commandCount-1].throwEpochH21=kcd2vr::collab_h21::throwEpoch.load(std::memory_order_acquire);out.commands[out.commandCount-1].physicsEpochH21=kcd2vr::collab_h21::epoch.load(std::memory_order_acquire);}
                else if(command!=equipment114::Request::None)states[i].comparison114=before;
                break;
            }
        }
        if(rightFenceH36&&leftFenceH36)return;
    }
    PhysGripBank6698 grips{};const bool haveGrips=g_phys_grips6698.read(grips);
    if(e.count<0||e.count>int(e.items.size()))return;
    // HS2 arbitration: one unheld item per gesture hand may acquire, the nearest zone whose draw volume holds the
    // hand (an item already latched keeps it). overlapping spheres (bow/shield/heavy over the left shoulder) can no
    // longer hand the grip to whichever item comes first in the list.
    // R8d2: the arbitration arrays live in the observer (declared first, destroyed last) so its flush never reads dead locals.
    int (&bestHS2)[2]=diagR8d.best;bool (&switchHS2)[8]=diagR8d.sw;double bestGapHS2[2]{1.0e9,1.0e9};bool latchedHS2[2]{};
    {PhysEquipment6698 freeHS2=e;freeHS2.handKey[0]=freeHS2.handKey[1]=0;
     const bool anyHeld=e.handKey[0]||e.handKey[1];const bool bowHeldR9=PhysBowHeldR9(e);
     // A switch can stow only current native items represented in this same
     // equipment snapshot. One two-handed item may represent both hand keys.
     bool heldKeysRepresentedR15=true;
     for(int hand=0;hand<2;++hand)if(e.handKey[hand]){
        bool represented=false;
        for(int j=0;j<e.count;++j)if(e.items[j].held&&e.items[j].profile.key==e.handKey[hand]){represented=true;break;}
        if(!represented){heldKeysRepresentedR15=false;break;}
     }
     for(int i=0;i<e.count&&i<8;++i){const auto& item=e.items[i];
        if(item.held||item.set<0||item.profile.torch40())continue;
        const int hand=ph::holster_hand126(item.profile);if(hand<0||hand>1)continue;
        if(kcd2vr::bow_arm_aim_r9::arrow_hand_blocks_draw(hand,bowHeldR9,ph::holster_zone126(item.profile)==ph::HolsterZone126::RightBackR12))continue; // R9/R11: right shoulder = quiver
        const bool direct=PhysDrawCandidate126(e,i);
        // Only a free grabbing hand can acquire another holster. Holding a
        // weapon (including its support hand) is not a request to switch it.
        const bool grabbingHandFreeR26=e.handKnown[hand]&&!e.handKey[hand]&&!out.owned[hand];
        // D44: a holster Grip does not stow/swap another main weapon.
        // Keep the legacy comparison policy only; modern acquisition waits
        // for the user's explicit release/stow of that weapon.
        switchHS2[i]=!in.requirePrimaryGrip114&&!direct&&grabbingHandFreeR26&&anyHeld&&heldKeysRepresentedR15&&PhysDrawCandidatePolicyR38(freeHS2,i,false);
        if(!direct&&!switchHS2[i])continue;
        if(equipment114::HeldAcquisitionR15(states[i].comparison114,item.profile.key,item.profile.generation,in.epoch,in.tick,in.grip[hand])){bestHS2[hand]=i;bestGapHS2[hand]=-1;latchedHS2[hand]=true;continue;}
        if(latchedHS2[hand])continue;
        pm::Vec3 mount{};if(!PhysGestureMount126(in,item,grips,in.hands[hand].p,mount))continue;
        if(!ph::draw_volume_ok_hs2(in,item.profile,in.hands[hand].p))continue;
        const double gap=pm::length(in.hands[hand].p-mount);
        const auto zone=ph::holster_zone126(item.profile);
        const double radius=std::min(.45,std::clamp(double(g_holster_radius136.load(std::memory_order_relaxed)),.10,.40)+
            ((zone==ph::HolsterZone126::LeftShoulder||zone==ph::HolsterZone126::RightBackR12||zone==ph::HolsterZone126::RightShoulderBowR12)?.04:0.0));
        if(gap>=radius)continue; // a distant mount cannot steal another zone's acquisition
        if(gap<bestGapHS2[hand]){bestGapHS2[hand]=gap;bestHS2[hand]=i;}
     }}
    // deferred half of a weapon switch: the held item is gone, the wanted one draws now (Grip still closed).
    // HS2b: work on a local copy, written back into the transaction copy on every exit (states[0] may be reset by
    // holster() for item 0 in between, same as hipStateT10). never a global: a failed publication loses nothing.
    auto sw=states[0].switchHS2;
    struct SaveSwitchHS2 {ph::WeaponSwitchHS2& dst;const ph::WeaponSwitchHS2& value;~SaveSwitchHS2(){dst=value;}} saveSwitchHS2{states[0].switchHS2,sw};
    if(sw.active){
        int wanted=-1;for(int i=0;i<e.count;++i)if(e.items[i].profile.key==sw.key&&e.items[i].profile.generation==sw.generation)wanted=i;
        const int h=sw.hand>=0&&sw.hand<2?sw.hand:0;
        const bool switchLostR15=!kcd2vr::corpse_grip_cg1::claims.current(sw.corpseClaimCg1)||!in.allowed||!in.tracked[h]||!in.intentTracked[h]||!std::isfinite(in.grip[h])||in.grip[h]<.25||
            (h?leftFenceH36:rightFenceH36)||!(doorAdmissionH40&(1u<<h));
        // R38: the accepted switch owns this held Grip while native stow is
        // pending. Pressing Trigger cannot turn that same acquisition into a
        // fist; release, source loss or a real competing owner still cancels.
        if(switchLostR15){if(wanted>=0)PhysCancelHolsterD44(states[wanted].comparison114,in.grip[h],in,h,e.items[wanted].profile.key,"SWITCH_SOURCE_OR_OWNER_LOST");sw={};}
        const auto step=ph::switch_step_hs2(sw,in.bodyEpoch,in.tick,wanted>=0,wanted>=0&&e.items[wanted].held,
            !e.handKey[0]&&!e.handKey[1],wanted>=0&&PhysDrawCandidate126(e,wanted),in.grip[h],in.allowed,in.tracked[h]);
        if(step==ph::SwitchStepHS2::Clear){sw={};}
        else if(step==ph::SwitchStepHS2::Timeout){
            diagR8d.switchTimeoutHand=sw.hand;
            char t[128]{};std::snprintf(t,sizeof(t),"%s: the game kept the weapon in hand - stow it first",PhysClassNameHS2(e.items[wanted].profile.classId));
            PhysHolsterSayHS2(t,in.tick);if(wanted>=0)PhysCancelHolsterD44(states[wanted].comparison114,in.grip[h],in,h,e.items[wanted].profile.key,"SWITCH_NATIVE_STOW_TIMEOUT");sw={};
        }else if(step==ph::SwitchStepHS2::Draw&&drawReadyCg1&&!(doorReservedD48&(1u<<h))&&out.commandCount<out.commands.size()){
            const auto& item=e.items[wanted];
            out.commands[out.commandCount++]={ph::CommandKind::Draw,item.profile.side,item.set,item.profile.key,in.epoch,in.sample,in.tick,0,{},in.bodyEpoch,item.profile.generation};
            out.commands[out.commandCount-1].corpseClaimCg1=sw.corpseClaimCg1;
            out.commands[out.commandCount-1].drawSourceD63=sw.drawSourceD63;
            out.commands[out.commandCount-1].physicsEpochH21=kcd2vr::collab_h21::epoch.load(std::memory_order_acquire);
            if(PhysHolsterCaptureD48(in.tick)){char hm[200]{};std::snprintf(hm,sizeof(hm),"HS2 WEAPON_SWITCH stagedDraw class=%d hand=%d afterMs=%llu",item.profile.classId,sw.hand,(unsigned long long)(in.tick-sw.tick));diagnostic_trace::Write(hm);}
            auto& pending=states[wanted].comparison114;pending={};
            pending.initialized=pending.pending=pending.wantDrawn=pending.requireReleaseR15=true;
            pending.key=item.profile.key;pending.generation=item.profile.generation;pending.epoch=in.epoch;
            pending.tick=pending.sent=in.tick;sw={};return;
        }
    }
    // D44 human policy: Grip alone grabs; the actual Trigger+Grip chord is
    // the fist exception, including inside a holster. No remembered fist mode.
    const bool fistOwnsH39[2]{
        equipment114::CombatFistOwnsHandH39(!rightClaimT24&&e.valid&&e.handKnown[0]&&!e.handKey[0],in.grip[0],in.trigger[0]),
        equipment114::CombatFistOwnsHandH39(!leftClaimT24&&e.valid&&e.handKnown[1]&&!e.handKey[1],in.grip[1],in.trigger[1])};
    // Keep arbitration in the pose transaction's copied state. Failed pose
    // publication cannot advance a global grip latch or choose a second owner.
    auto hipState=states[0].hipStateT10;
    struct SaveHipT10 {kcd2vr::hip_t10::State& dst;const kcd2vr::hip_t10::State& value;~SaveHipT10(){dst=value;}} saveHip{states[0].hipStateT10,hipState};
    kcd2vr::hip_t10::Input hip{};hip.body=in.bodyEpoch;hip.request=in.epoch;hip.sample=in.sample;hip.tick=in.tick;hip.grip=in.grip[0];
    hip.allowed=(doorAdmissionH40&1u)&&!rightFenceH36&&in.allowed&&in.tracked[0]&&in.intentTracked[0]&&e.valid&&e.handKnown[0]&&!e.handKey[0]&&!fistOwnsH39[0]&&!(talkMaskH4&1u);
    hip.stock=!(kcd2vr::ammo_service_t22::integrated&&kcd2vr::ammo_service_t22::leftChestKnivesT26)&&kcd2vr::weapon_release_t1::TestEnabledH21()&&kcd2vr::action_time_owner59c11::ThrowingSupplyT2();hip.equipmentRadius=g_holster_radius136.load();
    hip.available[0]=kcd2vr::inventory_lifecycle_t8::daggers.load()>0;hip.available[1]=kcd2vr::inventory_lifecycle_t8::axes.load()>0;
    const auto axes=ph::body_axes(in);const auto centre=ph::body_shoulder_r38(in,0)-axes.up*.50+axes.right*.03;
    hip.distance[0]=pm::length(in.hands[0].p-centre);hip.distance[1]=pm::length(in.hands[0].p-(centre+axes.forward*.13));
    hip.distance[2]=pm::length(in.hands[0].p-(ph::body_shoulder_r38(in,0)-axes.up*.25-axes.forward*.25+axes.right*.06));
    for(int j=0;j<e.count;++j)if(ph::holster_zone126(e.items[j].profile)==ph::HolsterZone126::RightHip45&&PhysDrawCandidate126(e,j)){
        hip.equipment=true;hip.key=e.items[j].profile.key;hip.generation=e.items[j].profile.generation;break;}
    // Observe the actual Grip cycle and source even if Talk cannot be copied.
    // A proposed local selection is not a grant until arbitration is known.
    const auto hipGrantH4=kcd2vr::hip_t10::update(hipState,hip);
    if(talkKnownH4)out.hipGrantT10=hipGrantH4;
    for(int i=0;i<e.count;++i){const auto& item=e.items[i];if(item.profile.torch40()){states[i]={};continue;} // native equip/stow owns torch; no held Grip required
        const bool supply=PhysSupplyHeldT2(e,item,0);
        if(item.set<0&&!supply)continue;
        const int talkSideH4=item.held?item.profile.side:(supply?0:ph::holster_hand126(item.profile));
        if(talkSideH4>=0&&talkSideH4<2&&(talkSideH4==0?rightFenceH36:leftFenceH36))continue;
        // A deferred native stow already owns this hand's draw gesture. Wait
        // for that exact switch; do not issue another stow from the same Grip.
        if(!item.held&&sw.active&&sw.hand==talkSideH4)continue;
        const bool retainedHolsterR15=in.requirePrimaryGrip114&&talkSideH4>=0&&talkSideH4<2&&
            equipment114::HeldAcquisitionR15(states[i].comparison114,item.profile.key,item.profile.generation,in.epoch,in.tick,in.grip[talkSideH4]);
        const bool talkOwnsH4=!retainedHolsterR15&&talkSideH4>=0&&talkSideH4<2&&(talkMaskH4&(1u<<talkSideH4));
        const int fistSideH39=in.requirePrimaryGrip114?talkSideH4:item.profile.side;
        const bool corpseConflictCg1=kcd2vr::corpse_grip_cg1::claims.reserved()||
            (states[i].acquisitionCg1.mask&&!kcd2vr::corpse_grip_cg1::claims.current(states[i].acquisitionCg1));
        const bool knownAcquisitionOwnerH4=corpseConflictCg1||talkOwnsH4||(fistSideH39>=0&&fistSideH39<2&&(fistOwnsH39[fistSideH39]||!(doorAdmissionH40&(1u<<fistSideH39))));
        const bool talkUnknownH4=!talkKnownH4&&talkSideH4>=0&&talkSideH4<2;
        const bool issuedDrawH4=in.requirePrimaryGrip114?
            (states[i].comparison114.pending&&states[i].comparison114.wantDrawn):
            (states[i].pending&&states[i].expectedDrawn);
        const bool observeUnknownH4=talkUnknownH4&&(in.requirePrimaryGrip114?issuedDrawH4:
            (e.handKnown[talkSideH4]&&!e.handKey[talkSideH4]));
        if(!item.held&&(knownAcquisitionOwnerH4||observeUnknownH4)){
            auto& pending=states[i].comparison114;
            if(in.requirePrimaryGrip114&&pending.pending&&pending.wantDrawn){
                // Observe current identity/input and issued-request deadlines even
                // while Talk cannot be copied. Without a mount this cannot admit
                // a new draw. Only a known competing owner cancels acquisition.
                equipment114::Input observation{};observation.key=item.profile.key;observation.generation=item.profile.generation;
                observation.zoneGripR15=true;observation.epoch=in.epoch;observation.tick=in.tick;observation.hand=in.hands[talkSideH4].p;
                observation.grip=in.grip[talkSideH4];const int nativeSide=item.profile.side;
                observation.eligible=nativeSide>=0&&nativeSide<2&&in.allowed&&in.tracked[talkSideH4]&&in.intentTracked[talkSideH4]&&
                    e.handKnown[nativeSide]&&(!e.handKey[nativeSide]||e.handKey[nativeSide]==item.profile.key);
                // No mount means this observation cannot create a new draw.
                (void)equipment114::update(pending,observation);
                if(knownAcquisitionOwnerH4)pending.neutral=pending.latched=false;
            }else if(!in.requirePrimaryGrip114&&states[i].pending&&states[i].expectedDrawn){
                // The retained legacy producer has its own pending FSM. A
                // closed fist cancels acquisition, not an already issued draw.
                auto& legacy=states[i];
                legacy.latched=false;legacy.rearm=true;
                if(haveGrips&&item.profile.classId!=7)
                    for(const auto& g:grips.grip)if(g.key==item.profile.key&&g.generation==item.profile.generation&&g.mountReady){
                        const int side=item.profile.side;
                        if(side<0||side>1||!in.tracked[side]){legacy={};break;}
                        if(legacy.epoch!=in.epoch||in.tick<legacy.lastTick||(legacy.lastTick&&in.tick-legacy.lastTick>100)){legacy={};legacy.epoch=in.epoch;break;}
                        if(legacy.lastTick==in.tick)break;
                        const auto mount=item.profile.bow()?ph::left_shoulder_zone_h42(in):(g.ready[side]?ph::point(g.mount,g.hand[side].p):g.mount.p);
                        // Existing original last/2s timeout and actual mount;
                        // rearm=true prevents expiry from emitting another draw.
                        (void)ph::holster(legacy,item.profile.key,in.hands[side].p,mount,in.grip[side],false,true,in.tick);
                        legacy.latched=false;legacy.rearm=true;
                        legacy.epoch=in.epoch;legacy.lastTick=in.tick;
                        break;
                    }
            }else if(knownAcquisitionOwnerH4||!in.requirePrimaryGrip114){
                const auto previous=states[i].comparison114;
                const bool consumed=previous.latched||previous.pending||previous.blocked||previous.requireReleaseR15;
                const bool onlyFist=fistSideH39>=0&&fistSideH39<2&&fistOwnsH39[fistSideH39]&&
                    !corpseConflictCg1&&!talkOwnsH4&&(doorAdmissionH40&(1u<<fistSideH39));
                // HS: any zone - a fist-only cancel of an unconsumed approach never demands a Grip release.
                const bool approachingShoulder=!consumed&&onlyFist;
                states[i]={};
                if(in.requirePrimaryGrip114&&!approachingShoulder){
                    states[i].comparison114=previous; // report the actual pre-cancellation owner state
                    PhysCancelHolsterD44(states[i].comparison114,in.grip[talkSideH4],in,talkSideH4,item.profile.key,corpseConflictCg1?"CORPSE_CLAIM_CHANGED":talkOwnsH4?"QUICKTALK_OWNER":!(doorAdmissionH40&(1u<<talkSideH4))?"DOOR_OWNER":"FIST_OWNER");
                }
            }
            continue; // no same-hand acquisition or proximity buzz
        }
        if(in.requirePrimaryGrip114){
            const int side=item.profile.side;if(side<0||side>1)continue;
            const int drawHand=supply?0:ph::holster_hand126(item.profile);
            const int gestureSide=item.held?side:drawHand;if(gestureSide<0||gestureSide>1)continue;
            equipment114::Input input{};input.zoneGripR15=true;input.instantZoneDrawD44=true;
            input.dispatchReadyD44=drawReadyCg1&&!talkUnknownH4&&!(doorReservedD48&(1u<<gestureSide));
            input.key=item.profile.key;input.generation=item.profile.generation;
            input.epoch=in.epoch;input.tick=in.tick;input.hand=in.hands[gestureSide].p;input.grip=in.grip[gestureSide];input.drawn=item.held;
            input.eligible=in.allowed&&in.tracked[gestureSide]&&in.intentTracked[gestureSide]&&e.handKnown[side]&&
                (!e.handKey[side]||e.handKey[side]==item.profile.key);
            if(!item.held){
                // HS2: an item of another loadout may start the switch even though a hand holds something.
                const bool switchOnlyHS2=i<8&&switchHS2[i];
                // Pending Draw observes the strict native hand-key eligibility
                // above. Neither a new switch nor reacquisition may replace it;
                // matching native key can precede the real held-item ACK.
                if(!(states[i].comparison114.pending&&states[i].comparison114.wantDrawn)){
                    if(switchOnlyHS2)input.eligible=in.allowed&&in.tracked[gestureSide]&&in.intentTracked[gestureSide]&&e.handKnown[side];
                    else input.eligible=input.eligible&&PhysDrawCandidate126(e,i);
                }
                // Spatial selection gates the mount below, not observation of a
                // released Grip. Native/context eligibility still cancels safely.
                if(hip.stock&&ph::holster_zone126(item.profile)==ph::HolsterZone126::RightHip45)
                    input.eligible=input.eligible&&hipGrantH4.owner==kcd2vr::hip_t10::Owner::Equipment&&
                        hipGrantH4.key==input.key&&hipGrantH4.generation==input.generation;
                // V66.146R1: while a bow is out with manual reload, the right
                // shoulder is the QUIVER. Never start a right-shoulder weapon draw there.
                // R9 lane A: with ANY bow mode (not only the retired manual reload) the right hand is the arrow hand.
                if(kcd2vr::bow_arm_aim_r9::arrow_hand_blocks_draw(drawHand,PhysBowHeldR9(e),ph::holster_zone126(item.profile)==ph::HolsterZone126::RightBackR12))input.eligible=false;
                for(int j=0;j<e.count;++j)if(j!=i){const auto& other=e.items[j];
                    const bool currentCandidate=!other.profile.torch40()&&(other.set>=0||PhysSupplyHeldT2(e,other,0));
                    if(currentCandidate&&equipment114::PendingDrawBlocks(states[j].comparison114,
                        other.profile.key,other.profile.generation,in.epoch,in.tick,other.held))input.eligible=false;
                }
            }
            const auto zone=ph::holster_zone126(item.profile);
            const double baseR=std::clamp(double(g_holster_radius136.load(std::memory_order_relaxed)),.10,.40);
            input.radius=std::min(.45,baseR+((zone==ph::HolsterZone126::LeftShoulder||zone==ph::HolsterZone126::RightBackR12||zone==ph::HolsterZone126::RightShoulderBowR12)?.04:0.0));
            input.releaseAnywhere=g_holster_release_anywhere136.load(std::memory_order_relaxed);input.releaseDebounceMs=0; // selected primary-release disposition
            bool liveMountKnown=false;
            if(supply){const auto a=ph::body_axes(in);input.mount=item.profile.classId==0?ph::body_shoulder_r38(in,0)-a.up*.50+a.right*.03+a.forward*.13:ph::body_shoulder_r38(in,0)-a.up*.25-a.forward*.25+a.right*.06;liveMountKnown=pm::finite(input.mount);}
            else liveMountKnown=PhysGestureMount126(in,item,grips,input.hand,input.mount);
            input.mountKnown=liveMountKnown;
            // V66.140: a one-handed NATIVE_HIP holster must not chase the held
            // weapon attachment. Capture the native belt point only while the
            // item is actually holstered, then keep that model/body-relative
            // point stable for the whole drawn interval. Shoulder zones are
            // already synthetic body-space zones and do not need this cache.
            if(zone==ph::HolsterZone126::NativeBelt){
                auto& hs=states[i];
                const bool cacheMatch=hs.beltMountReady140&&hs.beltKey140==item.profile.key&&
                    hs.beltGeneration140==item.profile.generation&&pm::finite(hs.beltMount140);
                if(!item.held&&liveMountKnown){
                    hs.beltMount140=input.mount;hs.beltKey140=item.profile.key;
                    hs.beltGeneration140=item.profile.generation;hs.beltMountReady140=true;
                }else if(item.held&&cacheMatch){
                    input.mount=hs.beltMount140;input.mountKnown=true;
                }
            }
            // Unknown Talk defers only NEW acquisition. Keep all ordinary
            // selected-loadout, source, raw-input and pending-item observation;
            // no mount means this update cannot issue a draw or proximity cue.
            if(!item.held){
                const bool holding=equipment114::HeldAcquisitionR15(states[i].comparison114,input.key,input.generation,input.epoch,input.tick,input.grip);
                if((!holding&&(!drawReadyCg1||talkUnknownH4))||(!holding&&!ph::draw_volume_ok_hs2(in,item.profile,input.hand))||
                   (bestHS2[gestureSide]>=0&&bestHS2[gestureSide]!=i))input.mountKnown=false;
            }
            // Draw AND stow use the same body-space proximity cue. V135 only
            // published this for unheld items, so returning a drawn sword to the
            // holster could never buzz even when the zone detector latched.
            // V66.146R1: with release-anywhere stowing, a drawn weapon needs no zone
            // cue; that cue was the "buzz while swinging" past the shoulder/hip.
            const bool cueMotionQuiet149 = !out.strokeActive119 &&
                out.melee118.reason!=melee118::Reason::Candidate &&
                out.melee118.reason!=melee118::Reason::Prepared &&
                out.melee118.reason!=melee118::Reason::Committed &&
                std::max(out.melee118.handSpeed,out.melee118.tipSpeed)<0.65;
            if(cueMotionQuiet149&&input.eligible&&input.dispatchReadyD44&&input.mountKnown&&!states[i].comparison114.pending&&
               (!states[i].comparison114.requireReleaseR15||input.grip<.25)&&!(item.held&&input.releaseAnywhere)){const auto gap=pm::length(input.hand-input.mount);
                if(gap<out.cueGap[gestureSide]){out.cueGap[gestureSide]=gap;out.cueKey[gestureSide]=input.key;out.cueRadiusD48[gestureSide]=input.radius;}}
            const auto before=states[i].comparison114;
            const auto claimBeforeD48=states[i].acquisitionCg1;
            const auto request=equipment114::update(states[i].comparison114,input);
            const auto& after=states[i].comparison114;
            // Preserve the first admitted latch across transient coordination loss.
            // An intervening CG1 acquire/release invalidates this original intent.
            if(!item.held&&(after.latched||request==equipment114::Request::Draw)&&!states[i].acquisitionCg1.mask&&drawReadyCg1)
                states[i].acquisitionCg1=drawClaimCg1;
            if(item.held||(!after.latched&&!after.pending))states[i].acquisitionCg1={};
            static std::uint64_t logDue121[8]{};
            const bool edge=request!=equipment114::Request::None||before.pending!=after.pending||before.latched!=after.latched||before.requireReleaseR15!=after.requireReleaseR15;
            if((edge||in.tick>=logDue121[i])&&PhysHolsterCaptureD48(in.tick)){logDue121[i]=in.tick+2000;char msg[512]{}; // D30: no formatting while no F capture is on
                const char* ack=before.pending&&!after.pending?(input.eligible&&before.key==input.key&&before.generation==input.generation&&before.epoch==input.epoch&&input.drawn==before.wantDrawn?"ATTACHMENT_CHANGED":"TIMEOUT_OR_RESET"):"NONE";
                std::snprintf(msg,sizeof(msg),"V66.121 HOLSTER_ZONE side=%d set=%d key=%016llX eligible=%d drawn=%d mountKnown=%d gap=%.3f grip=%.3f latched=%d pending=%d request=%u ack=%s slot=%d gestureHand=%d class=%d zone=%u selected=%d mountModel=(%.3f,%.3f,%.3f) neutral=%d rearm=%d dispatch=%d coordination=%u",
                    side,item.set,(unsigned long long)input.key,input.eligible,input.drawn,input.mountKnown,
                    input.mountKnown?pm::length(input.hand-input.mount):-1.0,input.grip,after.latched,after.pending,unsigned(request),ack,
                    item.slot126,gestureSide,item.profile.classId,unsigned(ph::holster_zone126(item.profile)),e.selected,input.mount.x,input.mount.y,input.mount.z,after.neutral,after.requireReleaseR15,input.dispatchReadyD44,coordinationCg1);diagnostic_trace::Write(msg);
            }
            auto kind=request==equipment114::Request::Draw?ph::CommandKind::Draw:
                request==equipment114::Request::Holster?ph::CommandKind::Holster:ph::CommandKind::None;
            if(kcd2vr::weapon_release_t1::DropOutsideHolster(request==equipment114::Request::Holster,
                item.held,input.eligible&&input.grip<.25,input.mountKnown,
                input.mountKnown?pm::length(input.hand-input.mount):1.e9,input.radius,
                kcd2vr::weapon_release_t1::Mode())){
                if(item.dropUnitT1)kind=ph::CommandKind::DropT1;
                else if(PhysHolsterCaptureD48(in.tick))diagnostic_trace::Write("T1 WEAPON_RELEASE preflight=UNSUPPORTED_OR_PROTECTED_UNIT action=SAFE_NATIVE_STOW nativeDrop=NOT_INVOKED");
            }
            if(kind==ph::CommandKind::Draw&&(!drawReadyCg1||!kcd2vr::corpse_grip_cg1::claims.current(states[i].acquisitionCg1))){PhysCancelHolsterD44(states[i].comparison114,in.grip[gestureSide],in,gestureSide,item.profile.key,"DRAW_OCCURRENCE_CHANGED");continue;}
            if(kind==ph::CommandKind::Draw&&!item.held&&i<8&&switchHS2[i]){
                // HS2 weapon switch, first half: stow what the hands hold (native stow), draw follows when free.
                unsigned stowed=0;
                for(int j=0;j<e.count;++j){const auto& h=e.items[j];if(!h.held||j==i||out.commandCount>=out.commands.size())continue;
                    out.commands[out.commandCount++]={ph::CommandKind::Holster,h.profile.side,h.set,h.profile.key,in.epoch,in.sample,in.tick,0,{},in.bodyEpoch,h.profile.generation};
                    out.commands[out.commandCount-1].corpseClaimCg1=states[i].acquisitionCg1;
                    out.commands[out.commandCount-1].physicsEpochH21=kcd2vr::collab_h21::epoch.load(std::memory_order_acquire);++stowed;}
                sw={item.profile.key,item.profile.generation,in.bodyEpoch,in.tick,gestureSide,stowed>0};sw.corpseClaimCg1=states[i].acquisitionCg1;sw.drawSourceD63=ph::draw_source_d63(in,gestureSide);
                if(!stowed){char t[128]{};std::snprintf(t,sizeof(t),"%s: hands not free and nothing to stow",PhysClassNameHS2(item.profile.classId));PhysHolsterSayHS2(t,in.tick);}
                if(PhysHolsterCaptureD48(in.tick)){char hm[200]{};std::snprintf(hm,sizeof(hm),"HS2 WEAPON_SWITCH stagedStow=%u for class=%d hand=%d",stowed,item.profile.classId,gestureSide);diagnostic_trace::Write(hm);}
                states[i].comparison114={};return;
            }
            if(kind!=ph::CommandKind::None&&out.commandCount>=out.commands.size()){
                // A staged request that cannot fit is not a consumed gesture.
                states[i].comparison114=before;states[i].acquisitionCg1=claimBeforeD48;
                continue;
            }
            if(kind!=ph::CommandKind::None)
                {out.commands[out.commandCount++]={kind,side,item.set,item.profile.key,in.epoch,in.sample,in.tick,0,{},in.bodyEpoch,item.profile.generation};
                 if(kind==ph::CommandKind::Draw){out.commands[out.commandCount-1].corpseClaimCg1=states[i].acquisitionCg1;out.commands[out.commandCount-1].drawSourceD63=ph::draw_source_d63(in,gestureSide);}
                 if(kind==ph::CommandKind::DropT1)out.commands[out.commandCount-1].throwEpochH21=kcd2vr::collab_h21::throwEpoch.load(std::memory_order_acquire);out.commands[out.commandCount-1].physicsEpochH21=kcd2vr::collab_h21::epoch.load(std::memory_order_acquire);}
            // Success feedback and competing-intent retirement happen only at
            // the actual ring commit in PhysHolsterEnqueuedD48.
            if(kind==ph::CommandKind::Draw)return;
            continue;
        }
        if(!haveGrips||item.profile.classId==7)continue; // retained legacy handling policy
        states[i].comparison114={};
        for(const auto& g:grips.grip)if(g.key==item.profile.key&&g.generation==item.profile.generation&&g.mountReady){
            const int side=item.profile.side;if(!in.tracked[side]){states[i]={};break;}
            if(states[i].epoch!=in.epoch||in.tick<states[i].lastTick||(states[i].lastTick&&in.tick-states[i].lastTick>100)){states[i]={};states[i].epoch=in.epoch;}
            if(states[i].lastTick==in.tick)break;states[i].lastTick=in.tick;
            // H42: the bow lives above the LEFT shoulder for the left hand;
            // every other item keeps its real native mount.
            const auto mount=item.profile.bow()?ph::left_shoulder_zone_h42(in):(g.ready[side]?ph::point(g.mount,g.hand[side].p):g.mount.p);
            const double gap=pm::length(in.hands[side].p-mount);
            if(!states[i].pending&&gap<out.cueGap[side]){out.cueGap[side]=gap;out.cueKey[side]=item.profile.key;}
            const auto beforeCg1=states[i];
            const auto kind=ph::holster(states[i],item.profile.key,in.hands[side].p,mount,in.grip[side],item.held,true,in.tick);states[i].epoch=in.epoch;states[i].lastTick=in.tick;
            const auto legacyDrawSourceD63=PhysLegacyDrawSourceD63(in,side,kind,beforeCg1,states[i]);
            if(!item.held&&!drawReadyCg1&&!beforeCg1.latched&&!beforeCg1.pending)
                states[i].latched=false; // no new legacy latch without both leases
            if(!item.held&&(!drawReadyCg1||(doorReservedD48&(1u<<side)))&&kind==ph::CommandKind::Draw){
                // Busy coordination is not a consumed gesture. Keep its original
                // bounded legacy intent, while release/identity observation runs.
                states[i]=beforeCg1;states[i].lastTick=in.tick;
                if(in.tick<states[i].since||in.tick-states[i].since>2000){states[i].latched=false;states[i].rearm=true;}
                break;
            }
            if(!item.held&&states[i].latched&&!states[i].acquisitionCg1.mask&&drawReadyCg1)
                states[i].acquisitionCg1=drawClaimCg1;
            if(item.held||(!states[i].latched&&!states[i].pending))states[i].acquisitionCg1={};
            if(kind!=ph::CommandKind::None&&out.commandCount<out.commands.size()&&(kind!=ph::CommandKind::Draw||drawReadyCg1)){
                out.commands[out.commandCount++]={kind,side,item.set,item.profile.key,in.epoch,in.sample,in.tick,0,{},in.bodyEpoch,item.profile.generation};
                if(kind==ph::CommandKind::Draw){out.commands[out.commandCount-1].corpseClaimCg1=states[i].acquisitionCg1;out.commands[out.commandCount-1].drawSourceD63=legacyDrawSourceD63;}
            }else if(kind!=ph::CommandKind::None)states[i]=beforeCg1;
            break;
        }
    }
}
