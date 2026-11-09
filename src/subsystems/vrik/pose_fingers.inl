static Q4 DesiredHand6689(const CoherentState6689& s,int side,Q4 frame,Q4 xr) noexcept {
    const Q4 controller=QMul(frame,XrToCryQ(xr));
    // Orientation has already undergone B*q*B^-1. Its local vectors must be B*v,
    // not raw OpenXR -Z; otherwise an accidental extra quarter-turn is introduced.
    // OpenXR GRIP, not AIM: -Y is extended-finger distal, -Z is the
    // little-to-index/thumb side of the grip tube. Cross products establish
    // handedness; do not copy the aim-pose -Z direction into this triad.
    const V3 distal=QRotateV(controller,XrToCryV(V3{0,-1,0}));
    const V3 sideAxis=QRotateV(controller,XrToCryV(V3{0,0,-1}));
    const Q4 anatomical=QFromBasis(distal,sideAxis,Normalize(Cross(distal,sideAxis)));
    return s.anatomyHand[side]?QNormalize(QMul(anatomical,QConj(s.boneToAnatomy[side]))):controller;
}
static void SetWorldRotation6689(CoherentState6689& s,int id,Q4 desired) noexcept {
    s.dirty[id]=true;const int p=s.parent[id];QToQuatT(s.local[id],p<0?desired:QMul(QConj(QFromQuatT(s.work[p])),desired));
}
static std::atomic<float> g_relaxed_middle101{1.f},g_relaxed_ring101{2.f},g_relaxed_pinky101{4.f};
// V66.105 OPEN HAND. ROOT CAUSE of the "half-open" free hand: since V66.97 the open endpoint
// is the game's evaluated relaxed-idle curve (NATIVE_CURVE_CACHE), which is a half-closed hand;
// RelaxedFinger101 only opened it by 1/2/4 degrees. The V66.100 user-captured pair lived in the
// INI section VRIK.Fingers.<rigHash>; V66.101R2 changed the rig-hash recipe (V1 -> V2) and the
// loader is only reached after a rig descriptor publication that fails silently, so the saved
// pair stopped being applied. Fix: per joint, when the skeleton's own bind pose (CDefaultSkeleton
// companion = flat open hand) is MORE open than the native open endpoint, blend the open endpoint
// toward the bind pose (fingers 80%, thumb 40%). A saved/hardcoded pair still has priority.
static std::atomic<float> g_open_bind105{.80f},g_open_bind_thumb105{.40f};
static std::atomic<bool> g_open_bind_enabled105{true};
static pc::Quat OpenTowardBind105(pc::Quat open,pc::Quat closed,pc::Quat bind,int finger) noexcept {
    if(!g_open_bind_enabled105.load(std::memory_order_relaxed))return open;
    const double toOpen=pc::angle(open,closed),toBind=pc::angle(bind,closed);
    if(!std::isfinite(toOpen)||!std::isfinite(toBind)||toBind<=toOpen+.02||toBind>2.6)return open; // bind not more open: keep native
    const double w=std::clamp(double(finger==0?g_open_bind_thumb105.load():g_open_bind105.load()),0.0,1.0);
    auto q=pc::slerp(open,bind,w);return q?*q:open;
}
static pc::Quat RelaxedFinger101(pc::Quat open,pc::Quat closed,int finger,int joint,int count,const pc::Quat* bind=nullptr) noexcept {
    if(bind)open=OpenTowardBind105(open,closed,*bind,finger);
    if((finger<2||finger>4)||joint==0||count<2)return open;
    const double delta=pc::angle(open,closed);if(!std::isfinite(delta)||delta<.025)return open;
    double degrees=finger==2?g_relaxed_middle101.load(std::memory_order_relaxed):finger==3?g_relaxed_ring101.load(std::memory_order_relaxed):g_relaxed_pinky101.load(std::memory_order_relaxed);
    degrees=std::clamp(degrees,0.0,6.0)/(count-1);
    // Walk a few degrees opposite the captured curl, not around a donor axis.
    // Slerp here clamps to [0,1], so use a small inverse relative rotation.
    const auto rel=pc::multiply(closed,pc::conjugate(open));
    auto shortRel=rel;if(shortRel.w<0)shortRel={-shortRel.x,-shortRel.y,-shortRel.z,-shortRel.w};
    const auto axis=pc::unit(pc::Vec3{shortRel.x,shortRel.y,shortRel.z});if(!axis)return open;
    const double amount=std::min(degrees*pc::armPi/180,delta*.2);
    return pc::multiply(pc::from_rotation_vector(*axis*(-amount)),open);
}
static void TuneFingerBase59(const CoherentState6689& s,int side,int finger,int joint,int id,pc::Quat& open,pc::Quat& closed) noexcept {
    namespace ft=kcd2vr::finger40;const float a=ft::get(side,finger,0),b=ft::get(side,finger,1),spread=ft::get(side,finger,2),proximal=ft::get(side,finger,3);
    if(a==0&&b==1&&spread==0&&proximal==0)return; // preserve default finger pose and contact cost
    pc::Vec3 axis{};
    const int hand=side?s.rig.hand_l:s.rig.hand_r,parent=s.parent[id];
    if(spread!=0&&joint==0&&parent>=0&&s.anatomyHand[side]){
      const auto model=pc::multiply(PQ(QFromQuatT(s.rest[hand])),PQ(s.boneToAnatomy[side]));
      axis=pc::rotate(pc::conjugate(PQ(QFromQuatT(s.rest[parent]))),pc::rotate(model,{0,0,1}));
    }
    const auto e=ft::tune(open,closed,a,b,spread,axis,joint==0,proximal);
    open=e.open;closed=e.closed;
}
// Current authored closed chain in hand space. No live attack pose or cross-hand
// quaternion mirroring: offsets stay anatomical and only local rotations tune.
static bool ClosedFingerJoint59(const CoherentState6689& s,int side,int finger,int joint,
                                QuatT28& before,QuatT28& result) noexcept {
    if(side<0||side>1||finger<0||finger>4||!s.fingerMap[side]||!s.open[side].ready||!s.closed[side].ready)return false;
    const auto& chain=s.fingers[side][finger];if(joint<0||joint>=chain.count)return false;
    int offset=0;for(int f=0;f<finger;++f)offset+=s.fingers[side][f].count;
    const int hand=side?s.rig.hand_l:s.rig.hand_r,base=chain.id[0];
    if(base<0||base>=int(s.count)||s.parent[base]<0||s.parent[base]>=int(s.count)||hand<0||hand>=int(s.count))return false;
    auto parent=RelativeFromAbsolute(s.rest[hand],s.rest[s.parent[base]]);
    for(int k=0;k<=joint;++k){const int id=chain.id[k];if(id<0||id>=int(s.count)||offset+k>=s.fingerCount[side])return false;
        auto open=PQ(QFromQuatT(s.open[side].local[offset+k])),closed=PQ(QFromQuatT(s.closed[side].local[offset+k]));
        TuneFingerBase59(s,side,finger,k,id,open,closed);auto local=s.restLocal[id];QToQuatT(local,QP(closed));
        before=parent;result=AbsoluteFromRelative(parent,local);if(!ValidPose6689(result))return false;parent=result;
    }
    return true;
}
struct MiddleFrameH31 {pc::Vec3 rootShiftRatio{},towardIndex{};bool valid=false;};
static MiddleFrameH31 MiddleFingerFrameH31(const CoherentState6689& s,int side) noexcept {
    MiddleFrameH31 out{};
    if(side<0||side>1||!kcd2vr::finger40::middleCorrectionH31(side)||!s.fingerMap[side]||
       !s.open[side].ready||!s.closed[side].ready)return out;
    const int hand=side?s.rig.hand_l:s.rig.hand_r;
    if(hand<0||hand>=int(s.count))return out;
    pc::Vec3 base[3]{};
    for(int f=1;f<=3;++f){const auto& chain=s.fingers[side][f];if(chain.count<3)return out;
        const int id=chain.id[0];if(id<0||id>=int(s.count)||s.parent[id]!=hand)return out;
        base[f-1]=PV(FromT(s.restLocal[id]));if(!pc::finite(base[f-1]))return out;
    }
    // Henry's semantic roots, not a guessed +/- local axis or mirrored quaternion.
    // Index->ring points away from index on BOTH independently authored hands.
    const auto towardRing=pc::unit(base[2]-base[1]),awayIndex=pc::unit(base[1]-base[0]);
    if(!towardRing||!awayIndex||pc::dot(*towardRing,*awayIndex)<=0)return out;
    const auto lateral=pc::unit(base[2]-base[0]);const double rootLength=pc::length(base[1]);
    if(!lateral||!std::isfinite(rootLength)||rootLength<1e-6)return out;
    out.rootShiftRatio=*lateral*(double(kcd2vr::finger40::getShiftMmH31(side))*.001/rootLength);
    out.towardIndex=*lateral*(-1);out.valid=true;return out;
}
static pc::Vec3 MiddleFingerOffsetH31(const CoherentState6689& s,int side,int finger,int joint,int id,double curl) noexcept {
    const auto original=PV(FromT(s.restLocal[id]));if(finger!=2||joint)return original;
    const auto frame=MiddleFingerFrameH31(s,side);if(!frame.valid)return original;
    pc::FingerCurve103 curve{};curve.offset[0]=original;curve.rootClosedShiftRatioH31=frame.rootShiftRatio;
    return pc::finger_offset103(curve,0,curl);
}
static bool MiddleFingerClosedH31(const CoherentState6689& s,int side,int joint,const MiddleFrameH31& frame,pc::Quat& result) noexcept {
    if(!frame.valid||joint<1||joint>2)return false;
    const auto& chain=s.fingers[side][2];if(joint>=chain.count)return false;
    const int hand=side?s.rig.hand_l:s.rig.hand_r,base=chain.id[0];
    int offset=0;for(int f=0;f<2;++f)offset+=s.fingers[side][f].count;
    auto parent=RelativeFromAbsolute(s.rest[hand],s.rest[s.parent[base]]);
    for(int k=0;k<=joint;++k){const int id=chain.id[k];
        if(id<0||id>=int(s.count)||offset+k>=s.fingerCount[side])return false;
        auto open=PQ(QFromQuatT(s.open[side].local[offset+k])),closed=PQ(QFromQuatT(s.closed[side].local[offset+k]));
        TuneFingerBase59(s,side,2,k,id,open,closed);auto local=s.restLocal[id];
        if(k==0)ToT(local,VP(MiddleFingerOffsetH31(s,side,2,0,id,1.)));
        const float degrees=kcd2vr::finger40::getDistalMedialDegH31(side);
        if(k>=1&&degrees!=0){
            const auto parentQ=PQ(QFromQuatT(parent));const auto jointQ=pc::multiply(parentQ,closed);
            pc::Vec3 outgoing{};
            if(k+1<chain.count){const int child=chain.id[k+1];if(child<0||child>=int(s.count))return false;
                outgoing=PV(FromT(s.restLocal[child]));}
            else outgoing=pc::rotate(pc::conjugate(closed),PV(FromT(s.restLocal[id])));
            const auto axis=pc::unit(pc::cross(pc::rotate(jointQ,outgoing),frame.towardIndex));
            if(axis){const auto localAxis=pc::rotate(pc::conjugate(parentQ),*axis);
                const auto q=pc::unit(pc::multiply(pc::from_rotation_vector(localAxis*(double(degrees)*pc::armPi/180.)),closed));
                if(!q)return false;closed=*q;}
        }
        QToQuatT(local,QP(closed));parent=AbsoluteFromRelative(parent,local);if(!ValidPose6689(parent))return false;
        if(k==joint)result=closed;
    }
    return true;
}
static void TuneFinger40(const CoherentState6689& s,int side,int finger,int joint,int id,pc::Quat& open,pc::Quat& closed) noexcept {
    TuneFingerBase59(s,side,finger,joint,id,open,closed);
    if(finger==2){const auto frame=MiddleFingerFrameH31(s,side);if(frame.valid){
        // New physical root/PIP/DIP controls replace the unsuccessful PIP-only
        // C59/C54 correction for this digit. Both new controls0 restore legacy.
        pc::Quat corrected{};if(MiddleFingerClosedH31(s,side,joint,frame,corrected))closed=corrected;
        return;
    }}
    // C59 headset: middle-finger PIP toward ring by a few degrees, CLOSED only.
    // Derive its axis from this hand's authored closed chain and neighbor. That
    // keeps the two actual hands independent and leaves the accepted open pose.
    if(finger==2&&joint==1&&s.fingers[side][finger].count>=3){
    QuatT28 parent{},middle{},ringParent{},ring{};
    if(!ClosedFingerJoint59(s,side,2,1,parent,middle)||!ClosedFingerJoint59(s,side,3,1,ringParent,ring))return;
    const int child=s.fingers[side][2].id[2];if(child<0||child>=int(s.count))return;
    const auto tuned=kcd2vr::finger40::towardRing59(open,closed,PQ(QFromQuatT(parent)),PQ(QFromQuatT(middle)),
        PV(FromT(s.restLocal[child])),PV(FromT(middle)),PV(FromT(ring)),kcd2vr::finger40::middleTowardRing59.load());
    closed=tuned.closed;
    }
    // The same authored anatomical spread normal already used by MCP tuning,
    // transformed into THIS mapped middle joint's parent. No guessed bone axis.
    const float lateral=kcd2vr::finger40::getMiddleLateralC54(side,finger);
    if(lateral==0||joint!=1||s.fingers[side][finger].count<3||!s.anatomyHand[side])return;
    const int hand=side?s.rig.hand_l:s.rig.hand_r,parentId=s.parent[id];
    if(hand<0||hand>=int(s.count)||parentId<0||parentId>=int(s.count))return;
    const auto model=pc::multiply(PQ(QFromQuatT(s.rest[hand])),PQ(s.boneToAnatomy[side]));
    const auto axis=pc::rotate(pc::conjugate(PQ(QFromQuatT(s.rest[parentId]))),pc::rotate(model,{0,0,1}));
    closed=kcd2vr::finger40::middleLateralRotationC54(closed,axis,lateral);
}
// One free-hand curve recipe for the sole writer and all contact/FK consumers.
// Native-curve trial changes no open tuning, hand ownership or combat input.
static void FreeFingerCurveH7(const CoherentState6689& s,int side,int finger,float openBias,
                              bool trial,pc::FingerCurve103& curve) noexcept {
    curve={};const auto& chain=s.fingers[side][finger];curve.count=unsigned(chain.count);
    trial=trial&&s.nativeCurveClosedH7[side].ready;
    if(finger==2&&!trial){const auto frame=MiddleFingerFrameH31(s,side);if(frame.valid)curve.rootClosedShiftRatioH31=frame.rootShiftRatio;}
    int n=0;for(int f=0;f<finger;++f)n+=s.fingers[side][f].count;
    for(int k=0;k<chain.count;++k,++n){const int id=chain.id[k];
        const auto bindQ=PQ(QFromQuatT(s.restLocal[id]));
        auto open=RelaxedFinger101(PQ(QFromQuatT(s.open[side].local[n])),PQ(QFromQuatT(s.closed[side].local[n])),finger,k,chain.count,g_capture_runtime66100.applied?nullptr:&bindQ);
        if(openBias>0.f){const auto o=pc::slerp(open,PQ(QFromQuatT(s.open[side].local[n])),.5*std::clamp(openBias,0.f,1.f));if(o)open=*o;}
        auto closed=PQ(QFromQuatT(s.closed[side].local[n]));TuneFinger40(s,side,finger,k,id,open,closed);
        if(trial)closed=PQ(QFromQuatT(s.nativeCurveClosedH7[side].local[n]));
        curve.open[k]=open;curve.closed[k]=closed;curve.offset[k]=PV(FromT(s.restLocal[id]));
    }
}
// The sole visual free-finger consumer may use the current dialogue packet;
// combat/Use/chooser readers retain the original gameplay trigger fields.
static float FreeFingerTriggerD78(const TrackingSnapshot& ts,int side) noexcept {
    return kcd2vr::dialogue_fingers_d78::presentationTrigger(ts,side,
        g_ctx_dialogue.load(std::memory_order_acquire),g_dialogue_body178r9.load(std::memory_order_acquire),
        g_gameplay_allowed.load(std::memory_order_acquire),GetTickCount64(),
        kcd2vr::tracking59c6::epoch.load(std::memory_order_acquire));
}
static double FreeFingerInputH7(int finger,float grip,float trigger,bool trial) noexcept {
    const double value=pc::finger_input(static_cast<pc::Finger>(finger),grip,trigger);
    return trial?std::clamp(value/double(.90f),0.0,1.0):value;
}
static bool PalmGeometry103(const CoherentState6689& s,int side,Q4 orientation,float grip,float trigger,
                            std::uint64_t heldKey,V3 (&out)[6],FingerContactGeometry103& geometry,float openBias=0,bool nativeCurveTrialH7=false) noexcept {
    if(!s.fingerMap[side])return false;
    geometry={};geometry.orientation=orientation;
    V3 tips[5]{},bases[5]{};int n=0;
    const bool endpoints=s.open[side].ready&&s.closed[side].ready;
    const bool trial=nativeCurveTrialH7&&s.nativeCurveClosedH7[side].ready&&endpoints&&!heldKey;
    const bool cachedObject=heldKey&&heldKey==s.objectFingerKey[side]&&s.objectCandidate[side].ready;
    for(int finger=0;finger<5;++finger){
        const auto& chain=s.fingers[side][finger];if(chain.count<2||chain.count>6)return false;
        auto& curve=geometry.curve[finger];curve.count=unsigned(chain.count);
        const double amount=FreeFingerInputH7(finger,grip,trigger,trial);
        geometry.input[finger]=float(amount);geometry.enabled[finger]=endpoints&&!heldKey;
        if(!heldKey&&endpoints)FreeFingerCurveH7(s,side,finger,openBias,trial,curve);
        QuatT28 parent{};parent.q[3]=1;
        for(int k=0;k<chain.count;++k,++n){
            const int id=chain.id[k];QuatT28 local=s.restLocal[id];
            if(heldKey||!endpoints){
                const auto native=cachedObject?s.objectFinger[side][n]:RelativeFromAbsolute(s.original[s.parent[id]],s.original[id]);
                curve.open[k]=curve.closed[k]=PQ(QFromQuatT(native));curve.offset[k]=PV(FromT(s.restLocal[id]));
            }
            const auto q=pc::slerp(curve.open[k],curve.closed[k],amount);if(!q)return false;QToQuatT(local,QP(*q));
            ToT(local,VP(pc::finger_offset103(curve,unsigned(k),amount)));
            parent=AbsoluteFromRelative(parent,local);if(k==0)bases[finger]=FromT(parent);
        }
        tips[finger]=FromT(parent);
    }
    V3 center{};for(int i=1;i<5;++i)center=Add(center,bases[i]);center=Mul(center,.25f);
    out[0]=QRotateV(orientation,center);
    for(int i=0;i<5;++i)out[i+1]=QRotateV(orientation,tips[i]);
    for(const auto& p:out)if(!pc::finite(PV(p))||Len(p)>.35f)return false;
    return true;
}
// Coarse free-hand contact uses only cached wrist/metacarpal rest positions.
// Five surface samples cover a palm quadrilateral; this is NOT a solid box,
// finger collider, new rigid body or native sweep primitive. Grip/curl cannot
// change this palm footprint; the same tracked hand orientation rotates it.
static bool SimplePalmGeometryR24(const CoherentState6689& s,int side,Q4 orientation,
                                  V3 (&out)[6],kcd2vr::hand_shape59c7f::Shape* shape=nullptr) noexcept {
    if(shape)*shape={};
    if(side<0||side>1||!s.fingerMap[side])return false;
    const auto q=pc::unit(PQ(orientation));if(!q)return false;
    V3 base[4]{},middle{};
    for(int f=1;f<5;++f){
        const auto& chain=s.fingers[side][f];if(chain.count<2||chain.count>6)return false;
        const int id=chain.id[0];if(id<0||unsigned(id)>=s.count)return false;
        base[f-1]=FromT(s.restLocal[id]);
        if(!pc::finite(PV(base[f-1]))||Len(base[f-1])>.35f)return false;
        middle=Add(middle,base[f-1]);
    }
    middle=Mul(middle,.25f);
    const V3 halfWidth=Mul(Sub(base[0],base[3]),.5f);
    if(Len(halfWidth)<.001f||Len(middle)<.001f)return false;
    const V3 local[5]{Mul(middle,.5f),base[0],base[3],Mul(halfWidth,-1.f),halfWidth};
    for(int i=0;i<5;++i)out[i]=VP(pc::rotate(*q,PV(local[i])));
    out[5]={};
    if(shape){
        // Existing palm thickness, four bounded perimeter capsules for local
        // body/other-hand/own-blade approximation only; these issue no native rays.
        constexpr int next[4]{2,3,4,1};
        for(int i=0;i<4;++i)shape->part[i]={PV(out[i+1]),PV(out[next[i]]),.014};
        shape->count=4;shape->valid=true;
    }
    return true;
}
// Balanced contact uses the same accepted input-driven finger recipe without
// granting collision any finger-curl authority. The five palm identities stay
// fixed and two stable tips cover reaching/pinching beyond that footprint.
static bool BalancedPalmGeometryD79(const CoherentState6689& s,int side,Q4 orientation,
                                    float grip,float trigger,V3 (&out)[7],float openBias=0.f,bool trial=false) noexcept {
    V3 palm[6]{},tips[6]{};FingerContactGeometry103 geometry{};
    if(!SimplePalmGeometryR24(s,side,orientation,palm)||
       !PalmGeometry103(s,side,orientation,grip,trigger,0,tips,geometry,openBias,trial))return false;
    kcd2vr::contact_balanced_d79::compose(palm,tips,out);return true;
}
static bool ContactPalmGeometryD79(const CoherentState6689& s,int side,Q4 orientation,
                                   float grip,float trigger,std::uint64_t heldKey,V3 (&out)[7],
                                   FingerContactGeometry103& geometry,bool precise,bool balanced,
                                   float openBias=0.f,bool trial=false) noexcept {
    if(balanced)return BalancedPalmGeometryD79(s,side,orientation,grip,trigger,out,openBias,trial);
    V3 legacy[6]{};const bool ok=precise?
        PalmGeometry103(s,side,orientation,grip,trigger,heldKey,legacy,geometry,openBias,trial):
        SimplePalmGeometryR24(s,side,orientation,legacy);
    if(ok)std::copy_n(legacy,precise?6:5,out);return ok;
}
// Optional free-finger presentation only. Runs AFTER fixed-length IK and before
// the sole local writer; mandatory hard-fit curls remain the fallback. All state
// belongs to stagedContact and is committed only by the successful transaction.
static void ApplyFingerContactSpringH31(const CoherentState6689& s,int side,HandContact6695& c,
    const kcd2vr::finger_contact_h31::Source& source,float grip,float trigger,float openBias,std::uint64_t now,bool nativeCurveTrialH7=false) noexcept {
    namespace fc=kcd2vr::finger_contact_h31;
    for(int f=0;f<5;++f)if(c.fingerContact[f])c.curl[f]=c.hardCurlH31[f];
    const auto reset=[&](){
        if(c.springH31.have&&c.springH31.source.sample==c.springMeasuredH31.sample&&c.springH31.source.tick==c.springMeasuredH31.tick)
            for(int i=0;i<7;++i)c.offset[i]=VP(c.springH31.hardOffsets[i]);
        c.springH31={};
    };
    if(!fc::current(source,now)||!c.have||c.character!=s.character||c.epoch!=source.epoch||
       !fc::sameOwner(source,c.springMeasuredH31)||source.sample!=c.springMeasuredH31.sample||
       source.tick!=c.springMeasuredH31.tick||c.lastCompleteTickH18!=source.tick||
       (!c.fresh169r1&&(!c.springH31.have||c.springH31.source.sample!=source.sample||c.springH31.source.tick!=source.tick))){reset();return;}
    // Never let a repeated source use last callback's optional curl as hard fit.
    ModelWorld6692 world{};FingerContactGeometry103 geometry{};V3 points[6]{};
    const int hand=side?s.rig.hand_l:s.rig.hand_r;
    if(!ReadModelWorld6692(s.character,world)||!PalmGeometry103(s,side,QFromQuatT(s.work[hand]),grip,trigger,0,points,geometry,openBias,nativeCurveTrialH7)||
       c.planeCount<=0||c.planeCount>7){reset();return;}
    const auto curveKey=fc::curveKey(geometry.curve);std::uint64_t surfaces=1469598103934665603ull;
    std::array<std::uint64_t,5> digitSurfaces{};digitSurfaces.fill(1469598103934665603ull);
    for(int p=0;p<c.planeCount;++p){const auto& plane=c.planes[p];
        const auto n=PV(plane.normal);if(!pc::finite(PV(plane.point))||!pc::finite(n)||std::abs(pc::length(n)-1.)>.01){reset();return;}
        const auto& w=plane.native172;fc::hash(surfaces,w.mask);fc::hash(surfaces,unsigned(w.body));fc::hash(surfaces,w.collider);
        fc::hash(surfaces,w.owner);fc::hash(surfaces,w.entityId);fc::hash(surfaces,std::uint64_t(w.part));fc::hash(surfaces,std::uint64_t(plane.feature));
        if(plane.feature>=1&&plane.feature<=5){auto& h=digitSurfaces[plane.feature-1];
            fc::hash(h,w.mask);fc::hash(h,unsigned(w.body));fc::hash(h,w.collider);fc::hash(h,w.vft);
            fc::hash(h,w.owner);fc::hash(h,w.entityId);fc::hash(h,std::uint64_t(w.part));}
    }
    auto next=c.springH31;
    const bool continuous=next.have&&fc::sameOwner(source,next.source)&&next.curve==curveKey&&
        next.scale==world.scale&&source.tick>=next.source.tick&&source.tick-next.source.tick<=80;
    const bool duplicate=continuous&&source.sample==next.source.sample&&source.tick==next.source.tick;
    if(!continuous)next={};
    if(!duplicate)for(int i=0;i<7;++i)next.hardOffsets[i]=PV(c.offset[i]);
    const double dt=continuous&&!duplicate?double(source.tick-next.source.tick)*.001:0.;
    const auto position=PV(ModelToWorld6692(world,FromT(s.work[hand])));
    const auto orientation=PQ(QMul(world.q,QFromQuatT(s.work[hand])));
    const double skin=.008*world.scale;
    double actualCurl[5]{};
    for(int f=0;f<5;++f){actualCurl[f]=c.fingerContact[f]?c.hardCurlH31[f]:geometry.input[f];
        if(!c.fingerContact[f]||!geometry.enabled[f]||!c.planeCount){next.digit[f]={};continue;}
        auto curve=geometry.curve[f];for(unsigned k=0;k<curve.count;++k)curve.offset[k]=curve.offset[k]*world.scale;
        const bool digitContinuous=continuous&&next.digitSurfaces[f]==digitSurfaces[f];
        if(!digitContinuous)next.digit[f]={};
        const auto clearance=[&](double curl,double extra){
            return fc::digitClearance(curve,curl,orientation,position,c.planes,c.planeCount,f,skin+extra);
        };
        const double hard=actualCurl[f];
        if(clearance(hard,0)<-1e-7){next.digit[f]={};continue;} // preserve existing hard owner on an unsolved final pose
        const double pressure=std::max(0.,-clearance(geometry.input[f],0));
        const double gap=duplicate?next.digit[f].gapWorld:fc::advance(next.digit[f].gapWorld,pressure,digitContinuous?dt:0.);
        double chosen=hard;bool found=clearance(hard,gap)>=-1e-7;
        if(!found){double bestDistance=2.;
            for(double endpoint:{std::max(0.,hard-.12),std::min(1.,hard+.12)}){double before=hard;
                for(int step=1;step<=8;++step){const double candidate=hard+(endpoint-hard)*step/8.;
                    if(clearance(candidate,gap)>=-1e-7){double bad=before,good=candidate;
                        for(int refine=0;refine<5;++refine){const double mid=(bad+good)*.5;if(clearance(mid,gap)>=-1e-7)good=mid;else bad=mid;}
                        if(std::abs(good-hard)<bestDistance){chosen=good;bestDistance=std::abs(good-hard);found=true;}break;}
                    before=candidate;}
            }
        }
        bool small=found&&pc::length(pc::finger_offset103(curve,0,chosen)-pc::finger_offset103(curve,0,hard))<=.002;
        for(unsigned k=0;k<curve.count&&small;++k){const auto a=pc::slerp(curve.open[k],curve.closed[k],hard),b=pc::slerp(curve.open[k],curve.closed[k],chosen);
            small=a&&b&&pc::angle(*a,*b)<=5.*pc::armPi/180.;}
        if(small&&clearance(chosen,0)>=-1e-7){c.curl[f]=actualCurl[f]=chosen;next.digit[f]={gap,chosen};}
        else next.digit[f]={};
    }
    // Exact final writer FK: previous-sample ray starts and knuckle/palm probes
    // must follow the applied curve, including H31's moving middle-finger root.
    pc::Vec3 center{};for(int f=0;f<5;++f){const auto& curve=geometry.curve[f];pc::Quat rotation{};pc::Vec3 point{},base{};
        for(unsigned k=0;k<curve.count;++k){point=point+pc::rotate(rotation,pc::finger_offset103(curve,k,actualCurl[f]));if(!k)base=point;
            const auto q=pc::slerp(curve.open[k],curve.closed[k],actualCurl[f]);if(!q){reset();return;}rotation=pc::multiply(rotation,*q);}
        c.offset[f+1]=VP(pc::rotate(orientation,point*world.scale));if(f)center=center+base*.25;}
    c.offset[0]=VP(pc::rotate(orientation,center*world.scale));
    float curls[5]{};for(int f=0;f<5;++f)curls[f]=float(actualCurl[f]);
    const auto knuckle=kcd2vr::knuckle174::offset(geometry.curve,curls,orientation);
    if(knuckle&&c.featureCount==7)c.offset[6]=VP(*knuckle*world.scale);
    next.have=true;next.source=source;next.curve=curveKey;next.surfaces=surfaces;next.digitSurfaces=digitSurfaces;next.scale=world.scale;c.springH31=next;
}
// Old helper retained only for compatibility with geometry tests, not the contact writer.
static bool SampleFingerEndpoint6689(CoherentState6689& s,int side,bool fist,
                                    std::uint64_t serial,std::uint64_t now) noexcept {
    auto& e=fist?s.closed[side]:s.open[side];if(e.ready)return true;
    if(!s.fingerMap[side]||s.fingerCount[side]<10){e.issue="INCOMPLETE_MAP";return false;}
    std::array<QuatT28,32> values{};double maxAngle=0;
    for(int i=0;i<s.fingerCount[side];++i) {
        const int id=s.fingerIds[side][i],p=s.parent[id];
        if(p<0||!ValidPose6689(s.original[id])||!ValidPose6689(s.original[p])){e.issue="INVALID_PARENT_LOCAL";e.rejectedJoint=id;return false;}
        values[i]=RelativeFromAbsolute(s.original[p],s.original[id]);
        // Fixed anatomical offsets: donor rotations are allowed to change; lengths are not.
        if(std::fabs(Len(FromT(values[i]))-Len(FromT(s.restLocal[id])))>0.004f){e.issue="NATIVE_LENGTH_DIFF";e.rejectedJoint=id;return false;}
        ToT(values[i],FromT(s.restLocal[id]));
        if(e.have)maxAngle=std::max(maxAngle,pc::angle(PQ(QFromQuatT(values[i])),PQ(QFromQuatT(e.local[i]))));
    }
    if(!e.have||e.sequence!=serial||maxAngle>0.055||now<e.lastSample||now-e.lastSample>100) {e.issue=!e.have?"STARTING":e.sequence!=serial?"REFERENCE_CHANGED":maxAngle>0.055?"ROTATION_UNSTABLE":"SAMPLE_GAP";
        e.rejectedJoint=-1;e.local=values;e.have=true;e.since=now;e.lastSample=now;e.sequence=serial;return false;}
    e.lastSample=now;
    if(now-e.since<350){e.issue="STABILIZING";return false;}
    e.local=values;e.ready=true;e.issue="READY_CANDIDATE";e.rejectedJoint=-1;
    if(!kcd2vr::diagnostics::vrik_h29::selected()||!kcd2vr::diagnostics::vrik_h29::active(now))return true;
    char m[384]{};std::snprintf(m,sizeof(m),"V66.89-FINGER-ENDPOINT side=%c kind=%s complete=%d provenance=SOLE-ACTIVATED-FIFO+STABLE-350ms scopeWeight=UNCLAIMED parentLocal=1 segmentOffsets=FIXED serial=%llu",side?'L':'R',fist?"DEFENSIVE_FIST":"RELAXED",s.fingerCount[side],(unsigned long long)serial);diagnostic_trace::Write(m);
    // One finite export per complete endpoint. A missing/truncated export is not
    // usable as an endpoint; index/count, joint/parent and serial support checking.
    for(int i=0;i<s.fingerCount[side];++i){
        const int id=s.fingerIds[side][i];const auto& value=e.local[i];char d[512]{};
        std::snprintf(d,sizeof(d),"V66.92R1-FINGER-DATA side=%c kind=%s serial=%llu character=%p skeleton=%p tick=%llu index=%d count=%d joint=%d parent=%d q=(%.9g,%.9g,%.9g,%.9g) offset=(%.9g,%.9g,%.9g) provenance=EVALUATED_CANDIDATE_NOT_WEIGHT_PROOF",
            side?'L':'R',fist?"FIST":"RELAXED",(unsigned long long)serial,s.character,s.owner,(unsigned long long)now,i,s.fingerCount[side],id,s.parent[id],
            value.q[0],value.q[1],value.q[2],value.q[3],value.t[0],value.t[1],value.t[2]);diagnostic_trace::Write(d);
    }
    return true;
}
// Retired live-FIFO endpoint pump: continuity/v6694/RETIRED_LIVE_FIFO_ENDPOINT_GATE.cpp.txt
// Combat-fist presentation reservation for the optional paper map only. This
// does not choose visible finger curl; Grip and Trigger retain their own curves.
static bool QualifiedCombatFistH34(const TrackingSnapshot& ts,std::uint64_t epoch,int side,std::uint64_t now) noexcept {
    if(side<0||side>1||!FistSelected101()||!FistModePermits178r22c2()||!g_gameplay_allowed.load()||
       animation_filter::nativeBodyOwned112.load()||diagnostics::lab::NativeReference()||
       !ts.allowed||!ts.head_valid||ts.head_held||!kcd2vr::contact176::fresh(ts.tick,now,80)||
       !(side?ts.left_valid:ts.right_valid)||(side?ts.left_held:ts.right_held)||
       !((side?ts.left_grip:ts.right_grip)>.60f)||!((side?ts.left_trigger:ts.right_trigger)>.60f))return false;
    FistOwner101 owner{};
    return g_fist_owner101.read(owner)&&owner.empty&&owner.player&&owner.manager&&owner.epoch==epoch&&
        owner.request==g_fist_epoch101.load()&&kcd2vr::contact176::fresh(owner.tick,now,80);
}
static bool ApplyFingerLocals6689(CoherentState6689& s,int side,float grip,float trigger,const HandContact6695* contact=nullptr,float openBias=0.f,bool nativeCurveTrialH7=false) noexcept {
    if(HandClaimBlockedT24(side,reinterpret_cast<std::uintptr_t>(s.character),g_pose_epoch6689.load(),g_weapon_request_epoch6694.load())&&!PropHoldOwnsHandR155(side))return true; // Native evaluated finger locals retain ownership (R155: not for the mod's own held item).
    if(!s.fingerMap[side]||!s.open[side].ready||!s.closed[side].ready)return true; // Native, never invented fist.
    openBias=std::clamp(openBias,0.f,1.f);
    const bool trial=nativeCurveTrialH7&&s.nativeCurveClosedH7[side].ready;
    const bool observed28=kcd2vr::diagnostics::hands28::selected()&&kcd2vr::diagnostics::hands28::active(GetTickCount64());
    if(observed28)s.freeObserved28[side]=true;
    for(int chain=0;chain<5;++chain){pc::FingerCurve103 curve{};
        FreeFingerCurveH7(s,side,chain,openBias,trial,curve);
        const double curl=contact&&contact->fingerContact[chain]?contact->curl[chain]:FreeFingerInputH7(chain,grip,trigger,trial);
        if(observed28){s.freeCurlObserved28[side][chain]=curl;s.freeClampObserved28[side][chain]=contact&&contact->fingerContact[chain];}
        for(int k=0;k<s.fingers[side][chain].count;++k){const int id=s.fingers[side][chain].id[k];
            auto blended=pc::blend_finger(curve.open[k],curve.closed[k],pc::finger_offset103(curve,unsigned(k),curl),curl);
            if(!blended)return false;
            s.dirty[id]=true;QToQuatT(s.local[id],QP(blended->rotation));ToT(s.local[id],VP(blended->fixed_offset));
        }
    }
    return true;
}
// Positively held native fingers are independent of collision/two-hand readiness.
// While settling, pass through the actual native locals. Free hands use the
// unchanged saved fist; no arbitrary grasp, wrist offset or stretched fingers.
static bool ApplyObjectFingerLocals6698(CoherentState6689& s,int side,std::uint64_t key,std::uint64_t generation,
        std::uint64_t sample=0,std::uint64_t now=0,bool followNativeAction=false) noexcept {
    if(side<0||side>1||!key||!s.fingerMap[side]||s.fingerCount[side]<=0)return false;
    auto& e=s.objectCandidate[side];
    if(s.objectFingerKey[side]!=key||s.objectFingerGeneration[side]!=generation){
        e={};s.objectFingerKey[side]=key;s.objectFingerGeneration[side]=generation;
    }
    std::array<QuatT28,32> candidate{};double change=0;
    for(int i=0;i<s.fingerCount[side];++i){const int id=s.fingerIds[side][i],parent=s.parent[id];
        if(parent<0||!ValidPose6689(s.original[id])||!ValidPose6689(s.original[parent]))return false;
        candidate[i]=RelativeFromAbsolute(s.original[parent],s.original[id]);
        if(!ValidPose6689(candidate[i])||std::fabs(Len(FromT(candidate[i]))-Len(FromT(s.restLocal[id])))>.004f)return false;
        if(e.have)change=std::max(change,pc::angle(PQ(QFromQuatT(candidate[i])),PQ(QFromQuatT(e.local[i]))));
    }
    // A tool/door action can wait with an open hand before closing on its handle.
    // The weapon's settled-grip cache must not freeze that early open pose for
    // the rest of the action. Follow the evaluated native finger locals only;
    // wrist placement and anatomical finger lengths remain controller-owned.
    if(followNativeAction)e={};
    if(!followNativeAction&&!e.ready&&sample&&sample!=e.sequence){
        if(!e.have||now<e.lastSample||now-e.lastSample>100||change>.055){e.since=now;e.have=true;e.lastSample=now;e.local=candidate;}
        else if(now>=e.since&&now-e.since>=180)e.ready=true;
        e.sequence=sample;e.lastSample=now;
        if(e.ready)s.objectFinger[side]=candidate;
    }
    if(kcd2vr::diagnostics::hands28::selected()&&kcd2vr::diagnostics::hands28::active(GetTickCount64())){s.objectObserved28[side]=true;s.objectFollow28[side]=followNativeAction;}
    const auto& pose=e.ready?s.objectFinger[side]:candidate;
    for(int i=0;i<s.fingerCount[side];++i){const int id=s.fingerIds[side][i];s.dirty[id]=true;
        QToQuatT(s.local[id],QFromQuatT(pose[i]));ToT(s.local[id],FromT(s.restLocal[id]));}
    return true;
}
// Preserve the actual native grasp observed at its real attachment, before
// FinishPicking returns the native skeleton to idle. Only fingers are cached;
// wrists remain controller-owned and anatomical local translations stay fixed.
