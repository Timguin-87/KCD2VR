// Door's own Lua update reads resident curves. Henry's native controller only
// projects copied XR intent and publishes an animation-time request.
namespace dp66=kcd2vr::door_path66;
struct DoorCurveFrame66 {door4::Request request{};dp66::Curve path{};dp66::Pose world{};float time=0;std::uint64_t tick=0;};
struct DoorDisplay66 {door4::Request request{};kcd2vr::door_hold65::Hand source{};pc::LocalPose wrist{};pc::Vec3 handle{};std::uint64_t tick=0;ph::Pose leaf{};};
// Inert native grip geometry may survive a same-door regrip, like a weapon
// socket. It is never a hand/tracking/animation lease by itself.
struct NativeGrip66 {door4::Request request{};std::uint64_t curve=0;ph::Pose local{};bool valid=false;};
static kcd2vr::core::SnapshotMailbox<DoorCurveFrame66> g_door_curve66;
static kcd2vr::core::SnapshotMailbox<DoorDisplay66> g_door_display66;
static kcd2vr::core::SnapshotMailbox<NativeGrip66> g_door_native_grip66;
static dp66::State g_door_projection66;
static ph::Pose g_door_wrist_offset66{};
static float g_door_wanted66=0;
// Synchronous native OnEnter can apply approach motion before 702288 returns.
// This proposal alone grants nothing: the native hook must ALSO prove the
// genuinely installed Henry AnimationControlled root and exact target backlinks.
static thread_local door4::Request g_door_entry66;
static door4::Request DoorEntry66(const DoorRequestWitness38& w,std::uint32_t target,int side,std::uint64_t now) noexcept {
    door4::Request r{};if(!w.handSource65||side<0||side>1||(!w.input46.token&&!w.acceptedUse62.valid)||
       !kcd2vr::door_hold65::current(w.hand65,now)||w.hand65.side!=side||w.hand65.grip<.30f||!w.hand65.press||
       !DoorPins19c4())return r;
    kcd2vr::door_surface65::Witness surface{};
    if(!kcd2vr::door_surface65::read(target,w.hand65,now,surface))return r;
    const auto& h=w.hand65;r.serial=0x100000000ull|h.press;r.tick=now;r.epoch=w.epoch;r.player=w.player;
    r.id=target;r.side=side;r.entity=reinterpret_cast<std::uintptr_t>(EntityById6694(target));
    r.sourceGeneration=h.sourceGeneration;r.trackingEpoch46=h.trackingEpoch;r.inputSettingsEpoch46=h.settingsEpoch;
    r.inputExperimentEpoch65=h.experimentEpoch;r.inputPress65=h.press;r.surface66=surface.point;
    // Preserve the actual public mode for this synchronous entry proposal.
    // Default2 is not the ordinary0 or legacy3 source family; control65 still
    // rechecks this copied mode and every existing live owner predicate.
    r.mode69=int(kcd2vr::combat_rules178r10::f5Rung.load(std::memory_order_acquire));
    DoorScriptIO19c4r4 io{};kcd2vr::door_hold65::Hand current{};
    if(!io.control65(r,current)||!kcd2vr::door_hold65::sameSource(h,current)||current.press!=h.press)return {};
    return r;
}
static bool DoorActiveHand66(int side,std::uint64_t epoch,std::uint64_t now,std::uint64_t& serial) noexcept {
    serial=0;if(side<0||side>1||!g_door_live_serial66.load(std::memory_order_acquire))return false;
    door4::Request r{};kcd2vr::door_hold65::Hand h{};DoorScriptIO19c4r4 io{};
    if(!g_door_request19c4.read(r)||r.serial!=g_door_live_serial66.load(std::memory_order_acquire)||
       r.side!=side||r.epoch!=epoch||!io.control65(r,h)||!kcd2vr::door_hold65::fresh(h.tick,now)||
       h.grip<.30f||!h.press||(side?g_left_grip.load():g_right_grip.load())<.30f)return false;
    serial=r.serial;return true;
}
static std::uint32_t DoorCurveCallback66(const kcd2vr::door_curve_signal66::Query& q,std::uint64_t now) noexcept {
    door4::Request r{};kcd2vr::door_hold65::Hand h{};DoorScriptIO19c4r4 io{};
    if(!g_door_request19c4.read(r)||r.serial!=g_door_live_serial66.load(std::memory_order_acquire)||
       r.serial!=q.lease.request||r.id!=q.lease.target||r.entity!=q.lease.entity||!io.control65(r,h))return false;
    DoorCurveFrame66 frame{};frame.request=r;auto& s=frame.path.source;
    s.operation=r.serial;s.press=r.inputPress65;s.poseEpoch=r.epoch;s.trackingEpoch=r.trackingEpoch46;
    s.generation=r.sourceGeneration;s.settingsEpoch=r.inputSettingsEpoch46;s.experimentEpoch=r.inputExperimentEpoch65;
    s.player=r.player;s.entity=r.entity;s.target=r.id;s.side=r.side;
    frame.time=float(q.micro)/1000000.f;frame.tick=now;
    const bool bound=DoorCurveBind66(r,q.clip,frame.time,frame.path,frame.world);
    if(!io.control65(r,h)||r.serial!=g_door_live_serial66.load(std::memory_order_acquire))return 0;
    if(!bound)return DoorCurvePending66()?2:0;
    return g_door_curve66.publish(frame)?1:0;
}
static bool DoorIntent66(const door4::Request& r,const kcd2vr::door_hold65::Hand& hand,
                         ph::Pose& wrist,dp66::Point& palm,std::uint64_t& poseTick) noexcept {
    CommittedFrame6695 p{};const auto now=std::uint64_t(GetTickCount64());
    if(!ReadCommittedPacket6695(p)||p.doorPlayer38!=r.player||p.stamp.epoch!=r.epoch||p.doorTrackingEpoch66!=hand.trackingEpoch||
       !p.valid[r.side]||!p.propRequestedValid5916c1[r.side]||!p.palmValid59c9[r.side]||
       !kcd2vr::door_hold65::fresh(p.stamp.tick,now)||hand.poseEpoch!=p.stamp.epoch)return false;
    const auto& raw=p.propRequested5916c1[r.side];const auto& allowed=p.allowed[r.side];
    const auto rawWorld=HandWorld6694(raw,p.world);wrist={PQ(QFromQuatT(rawWorld)),PV(FromT(rawWorld))};
    // Preserve the anatomical palm offset, evaluated before this Door display
    // override. The constrained/displayed palm must never drive its own clock.
    const auto localPalm=QRotateV(QConj(QFromQuatT(allowed)),Sub(p.palm59c9[r.side],FromT(allowed)));
    const auto worldPalm=Add(FromT(rawWorld),QRotateV(QFromQuatT(rawWorld),Mul(localPalm,p.world[7])));
    if(!ph::finite(wrist)||!pc::finite(PV(worldPalm)))return false;
    palm={worldPalm.x,worldPalm.y,worldPalm.z};poseTick=p.stamp.tick;return true;
}
static int DriveDoor66(const door4::Request& r,const kcd2vr::door_hold65::Hand& hand,float actual,bool bind,float& out) noexcept {
    const auto now=std::uint64_t(GetTickCount64());DoorCurveFrame66 f{};
    const auto refusedH4=[&](const char* why,int result,int line){namespace h4=diagnostics::interaction_cycle_h4;if(diagnostics::capture168::active(diagnostics::capture168::Domain::Interaction,now)){auto e=h4::doorEvent(r,h4::Stage::DoorProgress,now,why,actual,false);e.sourceTick=hand.poseTick;e.code=result;e.detail=line;h4::copy(e.label,sizeof(e.label),"DriveDoor66 EXISTING_RETURN");h4::observe(e,now);}return result;};
    if(!g_door_curve66.read(f)||f.request.serial!=r.serial||!kcd2vr::door_hold65::fresh(f.tick,now))return refusedH4("RESIDENT_CURVE_MAILBOX_MISSING_REPLACED_OR_OLDER_80MS",0,__LINE__);
    // Lua reads the resident curve BEFORE seeking and ACKs AFTER seeking. A
    // current all-phases curve is not stale merely because those times differ.
    if(f.request.entity!=r.entity||f.request.epoch!=r.epoch)return refusedH4("RESIDENT_CURVE_ENTITY_OR_POSE_EPOCH_CHANGED",-1,__LINE__);
    ph::Pose wrist{};dp66::Point palm{};std::uint64_t poseTick=0;
    if(!DoorIntent66(r,hand,wrist,palm,poseTick))return refusedH4("RESIDENT_CURVE_DoorIntent66_CURRENT_SOURCE_REFUSED",-1,__LINE__);
    auto path=f.path;path.source.press=hand.press;
    const dp66::Input input{path.source,f.world,palm,now,poseTick};
    const auto local=dp66::at(path,actual);
    const ph::Pose world{PQ(Q4{float(f.world.q.x),float(f.world.q.y),float(f.world.q.z),float(f.world.q.w)}),
                         {f.world.p.x,f.world.p.y,f.world.p.z}};
    const ph::Pose handle=ph::compose(world,{{local.q.x,local.q.y,local.q.z,local.q.w},{local.p.x,local.p.y,local.p.z}});
    bool progressed=false;
    if(bind){if(!dp66::bind(g_door_projection66,path,input,now,actual))return refusedH4("RESIDENT_CURVE_PROJECTION_INITIAL_BIND_REFUSED",-1,__LINE__);
        g_door_wrist_offset66=ph::compose(ph::inverse(handle),wrist);out=actual;g_door_wanted66=actual;
    }else if(now==g_door_projection66.tick){out=g_door_wanted66;}
    else{dp66::Result projected{};
        // Native clip time is an authored parameter. All actual travel comes
        // from controller displacement along its sampled rigid-leaf trajectory.
        if(!dp66::project(g_door_projection66,path,input,now,{1.,.20,.001},projected))return refusedH4("RESIDENT_CURVE_PROJECT_CURRENT_SOURCE_OR_REACH_REFUSED",-1,__LINE__);
        progressed=!projected.stopped;
        out=progressed?float(std::clamp(projected.phase,0.,.9995)):g_door_wanted66;
    }
    if(progressed){
        // Door clips also contain stationary approach/release intervals. The
        // physical leaf can reach its final pose before normalized time1. Do
        // not force extra hand travel through a stationary animation tail.
        const auto nearPose=[](const dp66::Pose& a,const dp66::Pose& b){const auto d=dp66::sub(a.p,b.p);
            const double dot=std::fabs(a.q.x*b.q.x+a.q.y*b.q.y+a.q.z*b.q.z+a.q.w*b.q.w);
            return dp66::dot(d,d)<=.001*.001&&dot>=std::cos(.002*.5);};
        const auto leaf=dp66::at(path,out);
        if(nearPose(leaf,path.samples.back().handle)&&!nearPose(path.samples.front().handle,path.samples.back().handle))out=.9995f;
        else if(nearPose(leaf,path.samples.front().handle))out=0.f;
    }
    g_door_wanted66=out;
    NativeGrip66 grip{};
    if(g_door_native_grip66.read(grip)&&grip.valid&&grip.curve==path.source.curve&&grip.request.serial==r.serial&&
       grip.request.id==r.id&&grip.request.entity==r.entity&&grip.request.player==r.player&&grip.request.side==r.side&&
       grip.request.epoch==r.epoch&&grip.request.trackingEpoch46==r.trackingEpoch46&&
       grip.request.sourceGeneration==r.sourceGeneration&&grip.request.inputSettingsEpoch46==r.inputSettingsEpoch46&&
       grip.request.inputExperimentEpoch65==r.inputExperimentEpoch65&&ph::finite(grip.local))g_door_wrist_offset66=grip.local;
    const auto attached=ph::compose(handle,g_door_wrist_offset66);
    DoorDisplay66 display{r,hand,{attached.q,attached.p},handle.p,now,handle};
    const bool publishedH4=g_door_display66.publish(display);if(!publishedH4)return refusedH4("RESIDENT_CURVE_DISPLAY_MAILBOX_CONTENDED",-1,__LINE__);return 1;
}
static void DetachDoor66(const door4::Request& r) noexcept {
    DoorDisplay66 d{};if(g_door_display66.read(d)&&d.request.serial==r.serial)(void)g_door_display66.publish({});
    g_door_projection66={};
    g_door_wanted66=0;
}
static void RetireDoor66(const door4::Request& r) noexcept {
    auto expected=r.serial;if(expected)(void)g_door_live_serial66.compare_exchange_strong(expected,0);
    DetachDoor66(r);
    NativeGrip66 grip{};if(g_door_native_grip66.read(grip)&&grip.request.serial==r.serial)(void)g_door_native_grip66.publish({});
}
static bool DoorGoalRead66(int side,std::uint64_t epoch,std::uint64_t now,pc::LocalPose& wrist,
                           pc::Vec3& handle,std::uint64_t& serial,int& nativeSide) noexcept {
    DoorDisplay66 d{};std::uint64_t active=0;
    if(!DoorActiveHand66(side,epoch,now,active)||!g_door_display66.read(d)||d.request.serial!=active||
       !kcd2vr::door_hold65::fresh(d.tick,now)||d.request.side!=side||d.request.epoch!=epoch)return false;
    kcd2vr::door_hold65::Hand h{};DoorScriptIO19c4r4 io{};
    if(!io.control65(d.request,h)||h.press!=d.source.press||!kcd2vr::door_hold65::sameSource(d.source,h))return false;
    wrist=d.wrist;handle=d.handle;serial=active;nativeSide=d.request.nativeSide66;return true;
}
static bool DoorNativeFingers66(int side,std::uint64_t epoch,std::uint64_t now) noexcept {
    auto& s=g_pose6689;pc::LocalPose wrist{};pc::Vec3 handle{};std::uint64_t serial=0;int native=-1;
    if(!DoorGoalRead66(side,epoch,now,wrist,handle,serial,native)||native<0||native>1)return false;
    if(s.doorFingerSerial66[side]!=serial){s.doorFingerSerial66[side]=0;s.doorFingerCount66[side]=0;
        ModelWorld6692 mw{};const int h=native?s.rig.hand_l:s.rig.hand_r;
        if(!ReadModelWorld6692(s.character,mw)||!ValidPose6689(s.original[h])||
           pc::length(PV(ModelToWorld6692(mw,FromT(s.original[h])))-handle)>.16)return false;
        namespace df=kcd2vr::door_fingers66;df::References ref[2]{};df::NativeShape shape{};shape.side=native;
        for(int which=0;which<2;++which){auto& r=ref[which];
            if(!s.fingerMap[which]||!s.open[which].ready||!s.closed[which].ready)return false;
            unsigned n=0;for(int finger=0;finger<5;++finger){const auto& chain=s.fingers[which][finger];r.layout.joints[finger]=unsigned(chain.count);
                for(int k=0;k<chain.count;++k,++n){if(n>=32||chain.id[k]!=s.fingerIds[which][n])return false;
                    const int id=chain.id[k];r.open[n]=PQ(QFromQuatT(s.open[which].local[n]));r.closed[n]=PQ(QFromQuatT(s.closed[which].local[n]));r.rest[n]=PV(FromT(s.restLocal[id]));
                    if(which==native){if(s.parent[id]<0)return false;const auto local=RelativeFromAbsolute(s.original[s.parent[id]],s.original[id]);
                        if(!ValidPose6689(local))return false;shape.local[n]=PQ(QFromQuatT(local));}}}r.ready=true;
        }
        shape.layout=ref[native].layout;double curl=0;unsigned measured=0;
        for(unsigned i=0;i<df::count(shape.layout);++i){const auto c=df::curl(ref[native].open[i],ref[native].closed[i],shape.local[i]);
            if(c){curl+=*c;++measured;}}
        if(!measured||curl/measured<.12)return false; // not the early open approach
        df::Retargeted mapped{};if(!df::retarget(shape,side,ref[native],ref[side],mapped))return false;
        DoorDisplay66 display{};DoorCurveFrame66 curve{};
        if(!g_door_display66.read(display)||display.request.serial!=serial||
           !g_door_curve66.read(curve)||curve.request.serial!=serial||!ph::finite(display.leaf))return false;
        const auto nativeWrist=ph::compose({PQ(mw.q),PV(mw.t)},
            {PQ(QFromQuatT(s.original[h])),PV(FromT(s.original[h]))*mw.scale});
        ph::Pose target=nativeWrist;
        if(native!=side){
            const auto palmOffset=[&](int which){pc::Vec3 offset{};
                for(int f=1;f<5;++f)offset=offset+PV(FromT(s.restLocal[s.fingers[which][f].id[0]]));
                return offset*(.125*mw.scale);};
            const auto nativePalm=nativeWrist.p+pc::rotate(nativeWrist.q,palmOffset(native));
            target={wrist.rotation,nativePalm-pc::rotate(wrist.rotation,palmOffset(side))};
        }
        NativeGrip66 grip{display.request,curve.path.source.curve,ph::compose(ph::inverse(display.leaf),target),true};
        if(!ph::finite(grip.local)||pc::length(grip.local.p)>.35||!g_door_native_grip66.publish(grip))return false;
        for(unsigned i=0;i<mapped.count;++i){QuatT28 q{};QToQuatT(q,QP(mapped.local[i].rotation));ToT(q,VP(mapped.local[i].fixed_offset));s.doorFingerLocal66[side][i]=q;}
        s.doorFingerCount66[side]=mapped.count;s.doorFingerSerial66[side]=serial;
    }
    if(s.doorFingerCount66[side]!=unsigned(s.fingerCount[side]))return false;
    for(unsigned i=0;i<s.doorFingerCount66[side];++i){const int id=s.fingerIds[side][i];s.dirty[id]=true;s.local[id]=s.doorFingerLocal66[side][i];}
    return true;
}
static void DoorBindingsInstall66() noexcept {
    g_doorActive66=&DoorActiveHand66;g_doorGoal66=&DoorGoalRead66;g_doorFingers66=&DoorNativeFingers66;
    kcd2vr::door_signal19c4r4::SetCurveHandler66(&DoorCurveCallback66);
}
static bool OwnedDoorMotion66(std::uintptr_t actor,std::uint32_t target,std::uintptr_t entity,std::uint64_t now,std::uint64_t* serial) noexcept {
    door4::Request r{};kcd2vr::door_hold65::Hand h{};DoorScriptIO19c4r4 io{};
    const auto entry=g_door_entry66;
    if(entry.serial&&entry.player==actor&&entry.id==target&&entry.entity==entity&&
       kcd2vr::door_hold65::fresh(entry.tick,now)&&io.control65(entry,h)&&h.press==entry.inputPress65&&h.grip>=.30f){
        if(serial)*serial=entry.serial;return true;}
    if(!g_door_live_serial66.load(std::memory_order_acquire)||!g_door_request19c4.read(r)||
       r.serial!=g_door_live_serial66.load(std::memory_order_acquire)||r.player!=actor||r.id!=target||r.entity!=entity||
       !io.control65(r,h)||!kcd2vr::door_hold65::fresh(h.tick,now))return false;
    if(serial)*serial=r.serial;return true;
}
