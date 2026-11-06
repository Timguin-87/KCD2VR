#pragma once
// Gesture Alchemy work intent. No native calls, recipe effects, event replay,
// attachment writes or clock reads. Native admission/bindings remain the host's.
#include <algorithm>
#include <cmath>
#include <cstdint>

namespace kcd2vr::subsystems::alchemy::gesture67 {
enum Kind : unsigned { None=0, Bellows=1, Mortar=2, Resource=3, Book=4 };
enum class Status { Off, InvalidFrame, NativePassenger, NativeOwnerRequired,
    GripRequired, GeometryRequired, MoveToPot, TiltToPour, Pouring,
    Grinding, Pumping, WorkStopped, CompletedHeld, ReleaseAwaitingNativeReturn,
    NativeReturn, ReadyGesture, MovingBelowQuantumD49 };
inline const char* name(Status s) noexcept {
    switch(s){
    case Status::Off:return "OFF";case Status::InvalidFrame:return "INVALID_TRACKING_FRAME";
    case Status::NativePassenger:return "NATIVE_BUTTON_PASSENGER";
    case Status::NativeOwnerRequired:return "CURRENT_NATIVE_WORK_REQUIRED";
    case Status::GripRequired:return "JOINED_NATIVE_GRIP_REQUIRED";
    case Status::GeometryRequired:return "CURRENT_REGISTERED_RECIPIENT_REQUIRED";
    case Status::MoveToPot:return "MOVE_HELD_CARAFE_TO_POT";
    case Status::TiltToPour:return "TILT_HELD_CARAFE";case Status::Pouring:return "GESTURE_POUR_CREDIT";
    case Status::Grinding:return "RELATIVE_PESTLE_CREDIT";case Status::Pumping:return "BELLOWS_PUMP_CREDIT";
    case Status::WorkStopped:return "HAND_STOPPED_NO_NEW_WORK";
    case Status::CompletedHeld:return "NATIVE_USE_OBSERVED_STILL_HELD";
    case Status::ReleaseAwaitingNativeReturn:return "RELEASE_AWAITING_NATIVE_RETURN";
    case Status::NativeReturn:return "NATIVE_RETURN_AUTHORIZED";
    case Status::ReadyGesture:return "QUALIFIED_NATIVE_READY_REQUEST";
    case Status::MovingBelowQuantumD49:return "HAND_MOVING_PENDING_QUANTUM";}
    return "UNKNOWN";
}
struct WorkOwner {
    std::uintptr_t instance=0,parent=0,child=0;
    std::uint64_t lifetime=0,tick=0;
    std::uint32_t token=0;
    unsigned kind=None;int phase=-1;
    bool working=false,ready=false;
};
// Copies of CURRENT real native attachments. A close candidate, command or
// visual proxy cannot set joined. Each side must belong to this same work owner.
struct Grip {
    bool joined=false;
    std::uintptr_t instance=0,parent=0;
    std::uint64_t lifetime=0,tick=0,poseEpoch=0,sourceEpoch=0;
    std::uint32_t entity=0;int side=-1,type=-1;
};
// Registered table recipient point in NATIVE WORLD (+Z up), never compared to
// raw XR (+Y up) coordinates. This conservative volume is not a rendered mouth.
struct Geometry {
    bool valid=false;
    std::uintptr_t instance=0;
    std::uint64_t lifetime=0,tick=0,poseEpoch=0,sourceEpoch=0;
    std::uint32_t entity=0;int type=-1;
    float point[3]{};float radius=.20f;
};
struct WorldPalm {
    bool valid=false;float point[3]{};
    std::uint64_t tick=0,poseEpoch=0,sourceEpoch=0;
};
// Mounted Bellows has native LEFT tool membership18, not an entity_left held
// attachment. This separately typed witness cannot grant a tool/wrist pose.
struct TableContact {
    bool valid=false,geometryValid=false;
    std::uintptr_t instance=0,parent=0;
    std::uint64_t lifetime=0,tick=0,poseEpoch=0,sourceEpoch=0;
    std::uint32_t entity=0;int side=-1,type=-1,nativeHandType=-1;
    float point[3]{}; // closest point on actual current native WORLD bounds
    float rangeCraft1=.10f; // the same role-specific contact region, never greater than 20 cm
};
struct MortarToolContact {
    bool required=false,valid=false;
    std::uintptr_t instance=0,parent=0;
    std::uint64_t lifetime=0,tick=0,poseEpoch=0,sourceEpoch=0;
    std::uint32_t pestle=0,mortar=0;
    double gap=0;
    // Radius of the actual native pestle's two shorter bound axes. Used only
    // to convert tracked relative rotation into bounded surface travel; never
    // a guessed mouth, contact, native work result or attachment permission.
    double surfaceRadiusD41=0;
};
struct Input {
    bool enabled=false,nativePassenger=false,nativeReturnAuthorized=false;
    WorkOwner work{};Grip grip[2]{};Geometry pot{},mortar{};WorldPalm palm[2]{};
    // Host may supply the original physical admission quaternion, qualified to
    // THIS instance/action/tool and pose/source epochs. Native Attach can arrive
    // later, after the user has already moved/tilted the selected vessel.
    bool referenceRotationValid=false;float referenceRotation[4]{0,0,0,1};
    // Same physical Resource admission already crossed the initial Grip threshold.
    // The host qualifies its unchanged native attachment/child before setting this.
    bool retainedResourceGripR23=false;
    // Actual Use already consumed one recipe effect. The same still-held
    // carafe may continue its ORIGINAL child for native pour FX / return,
    // using only fresh gesture time. Home is qualified by the host's copied
    // original source bounds plus the current upright palm, never gaze.
    bool resourceTailD86=false,resourceHomeD86=false;
    // Both physical mortar/pestle grips crossed .55 in this admission and
    // remain at .35+, even if native pair Attach arrived later.
    bool retainedMortarGripD39=false;
    bool trackedPestleD56=false; // exact mode3 retained native pair; left Grip still holds the mortar
    bool pestleParkedD47=false;
    TableContact tableContact{};
    MortarToolContact mortarContact{};
};
struct Output {
    bool enabled=false,clockStop=true,releaseRequested=false,completedHeld=false,use=false;
    std::uintptr_t parent=0,child=0;
    std::uint32_t token=0;int hand=-1;
    double totalSeconds=0;
    std::uint64_t useUntil=0;
    Status status=Status::Off;
};
inline bool finite3(const float* v) noexcept {
    return std::isfinite(v[0])&&std::isfinite(v[1])&&std::isfinite(v[2]);
}
inline bool fresh(std::uint64_t tick,std::uint64_t now) noexcept {
    return tick&&now>=tick&&now-tick<=80;
}
inline float distance(const float* a,const float* b) noexcept {
    const float x=a[0]-b[0],y=a[1]-b[1],z=a[2]-b[2];return std::sqrt(x*x+y*y+z*z);
}
inline bool upVector(const float* q,float* out) noexcept {
    float n=0;for(int j=0;j<4;++j){if(!std::isfinite(q[j]))return false;n+=q[j]*q[j];}
    if(n<.8f||n>1.2f)return false;
    const float s=1.f/std::sqrt(n),x=q[0]*s,y=q[1]*s,z=q[2]*s,w=q[3]*s;
    out[0]=2.f*(x*y-w*z);out[1]=1.f-2.f*(x*x+z*z);out[2]=2.f*(y*z+w*x);return true;
}
inline bool localVector(const float* q,const float* v,float* out) noexcept {
    float n=0;for(int j=0;j<4;++j){if(!std::isfinite(q[j]))return false;n+=q[j]*q[j];}
    if(n<.8f||n>1.2f)return false;
    const float s=1.f/std::sqrt(n),x=-q[0]*s,y=-q[1]*s,z=-q[2]*s,w=q[3]*s;
    const float tx=2.f*(y*v[2]-z*v[1]),ty=2.f*(z*v[0]-x*v[2]),tz=2.f*(x*v[1]-y*v[0]);
    out[0]=v[0]+w*tx+y*tz-z*ty;out[1]=v[1]+w*ty+z*tx-x*tz;out[2]=v[2]+w*tz+x*ty-y*tx;
    return finite3(out);
}
inline bool relativeRotationD41(const float* right,const float* left,float* out) noexcept {
    double n[2]{};for(int j=0;j<4;++j){if(!std::isfinite(right[j])||!std::isfinite(left[j]))return false;
        n[0]+=double(right[j])*right[j];n[1]+=double(left[j])*left[j];}
    if(n[0]<.8||n[0]>1.2||n[1]<.8||n[1]>1.2)return false;
    const double scale=1./std::sqrt(n[0]*n[1]);
    // inverse(left) * right. A common tracking-space/body rotation cancels.
    out[0]=float((left[3]*right[0]-left[0]*right[3]-left[1]*right[2]+left[2]*right[1])*scale);
    out[1]=float((left[3]*right[1]+left[0]*right[2]-left[1]*right[3]-left[2]*right[0])*scale);
    out[2]=float((left[3]*right[2]-left[0]*right[1]+left[1]*right[0]-left[2]*right[3])*scale);
    out[3]=float((left[3]*right[3]+left[0]*right[0]+left[1]*right[1]+left[2]*right[2])*scale);
    return true;
}
struct Controller {
    WorkOwner owner{};std::uint64_t sequence=0,timeNs=0,poseEpoch=0,sourceEpoch=0,useUntil=0;
    std::uint32_t entities[2]{};int side=-1;
    bool acquired=false,released=false,completed=false,haveMotion=false,readyUsed=false;
    float neutralUp[3]{},lastRelative[3]{},lastVertical=0,lastWorldVertical=0,strokeTravel=0;
    float lastRotationD41[4]{0,0,0,1};bool haveRotationD41=false;
    double totalSeconds=0;bool lastClockStop=true;Status lastStatus=Status::Off;
    void reset() noexcept {*this=Controller{};}
    template<class Frame> Output step(const Input& in,const Frame& f,std::uint64_t now) noexcept {
        Output out{};const auto& w=in.work;
        struct Remember {Controller& self;Output& output;~Remember(){self.lastClockStop=output.clockStop;self.lastStatus=output.status;}} remember{*this,out};
        if(!in.enabled){reset();return out;}
        if(in.nativePassenger){reset();out.status=Status::NativePassenger;return out;}
        bool frame=f.allowed&&f.headValid&&f.sequence&&f.timeNs&&f.poseEpoch&&f.sourceEpoch&&fresh(f.tick,now);
        for(int s=0;s<2;++s)if(f.hand[s].valid)frame=frame&&finite3(f.hand[s].position)&&std::isfinite(f.hand[s].grip);
        if(!frame){reset();out.status=Status::InvalidFrame;return out;}
        if(!w.instance||!w.lifetime||!w.parent||!fresh(w.tick,now)||
            (w.kind!=Resource&&w.kind!=Mortar&&w.kind!=Bellows)||
            ((!w.working&&!w.ready)&&!(w.kind==Resource&&w.phase==2))||
            (!w.child&&w.working)||(!w.token&&w.child)){
            reset();out.status=Status::NativeOwnerRequired;return out;
        }
        const bool same=owner.instance==w.instance&&owner.lifetime==w.lifetime&&owner.parent==w.parent&&owner.kind==w.kind&&
            poseEpoch==f.poseEpoch&&sourceEpoch==f.sourceEpoch;
        if(!same){reset();owner=w;poseEpoch=f.poseEpoch;sourceEpoch=f.sourceEpoch;}
        if(sequence&&((f.sequence<sequence)||(f.sequence!=sequence&&f.timeNs<=timeNs)||
           (f.timeNs>timeNs&&f.timeNs-timeNs>80000000ull))){reset();out.status=Status::InvalidFrame;return out;}
        const auto joined=[&](int s,int type) noexcept {
            const auto& g=in.grip[s];return g.joined&&g.side==s&&g.type==type&&g.entity&&g.instance==w.instance&&
                g.parent==w.parent&&g.lifetime==w.lifetime&&g.poseEpoch==f.poseEpoch&&g.sourceEpoch==f.sourceEpoch&&fresh(g.tick,now)&&
                f.hand[s].valid&&finite3(f.hand[s].position)&&std::isfinite(f.hand[s].grip);
        };
        int next=-1;
        if(w.kind==Resource){const int t=in.grip[1].type;if(t>=13&&t<=16&&joined(1,t))next=1;}
        else if(w.kind==Mortar){if(joined(0,3)&&joined(1,2))next=0;}
        else {const auto& c=in.tableContact;
            if(c.valid&&c.instance==w.instance&&c.parent==w.parent&&c.lifetime==w.lifetime&&
               c.poseEpoch==f.poseEpoch&&c.sourceEpoch==f.sourceEpoch&&fresh(c.tick,now)&&
               c.entity&&c.type==18&&c.nativeHandType==18&&c.side==1&&f.hand[1].valid)next=1;
        }
        if(next<0){reset();out.status=Status::GripRequired;return out;}
        const auto nextEntity=w.kind==Bellows?in.tableContact.entity:in.grip[next].entity;
        const bool identities=(!acquired)||(side==next&&entities[next]==nextEntity&&
            (w.kind!=Mortar||entities[1]==in.grip[1].entity));
        if(!identities){reset();out.status=Status::GripRequired;return out;}
        const bool retainedResourceR23=w.kind==Resource&&next==1&&in.retainedResourceGripR23;
        const bool retainedMortarD39=w.kind==Mortar&&in.retainedMortarGripD39;
        const bool trackedPestleD56=w.kind==Mortar&&in.trackedPestleD56&&!in.pestleParkedD47;
        const bool held=(trackedPestleD56||f.hand[next].grip>=((acquired||retainedResourceR23||retainedMortarD39)?.35f:.55f))&&
            (w.kind!=Mortar||(!in.pestleParkedD47&&f.hand[1].grip>=((acquired||retainedMortarD39||trackedPestleD56)?.35f:.55f)));
        out.parent=w.parent;out.child=w.child;out.token=w.token;out.hand=next;
        if(!acquired){
            if(!held){out.status=Status::GripRequired;return out;}
            if(w.kind==Resource&&!upVector(in.referenceRotationValid?in.referenceRotation:f.hand[next].rotation,neutralUp)){
                reset();out.status=Status::InvalidFrame;return out;}
            acquired=true;side=next;entities[0]=in.grip[0].entity;entities[1]=in.grip[1].entity;entities[next]=nextEntity;
        }
        completed=completed||(w.kind==Resource&&w.phase==2);
        // A released pestle pauses this same native grind. It is not the
        // irreversible resource-pour return transaction. Rejoining the exact
        // pair after a fresh .55 squeeze must be able to start a NEW stroke;
        // keep the consumed cumulative work ledger and discard gap movement.
        if(w.kind==Mortar&&released&&held&&!in.nativeReturnAuthorized&&
           (trackedPestleD56||retainedMortarD39||(f.hand[0].grip>=.55f&&f.hand[1].grip>=.55f))){
            released=false;haveMotion=haveRotationD41=false;strokeTravel=0;
        }
        released=released||!held;
        out.releaseRequested=released;out.completedHeld=completed&&!released;
        if(released&&(completed||in.nativeReturnAuthorized||w.kind==Bellows)){
            out.status=Status::NativeReturn;out.totalSeconds=totalSeconds;return out;
        }
        out.enabled=true;
        if(released){
            // Samples are still observed while the grip is open. Failing to
            // advance this continuity cursor reset the ledger after 80 ms and
            // could make an open hand look like a lost tracking interval.
            sequence=f.sequence;timeNs=f.timeNs;haveMotion=haveRotationD41=false;strokeTravel=0;
            out.status=Status::ReleaseAwaitingNativeReturn;out.totalSeconds=totalSeconds;return out;
        }
        if(completed&&!(w.kind==Resource&&w.phase==2&&w.working&&in.resourceTailD86)){sequence=f.sequence;timeNs=f.timeNs;
            out.status=Status::CompletedHeld;out.totalSeconds=totalSeconds;return out;}
        // One native child/token owns one cumulative ledger. A genuine new child
        // starts at zero, while phase2(queue)->phase3(live) retains its ledger.
        if(owner.child!=w.child||owner.token!=w.token){
            totalSeconds=0;haveMotion=haveRotationD41=false;strokeTravel=0;readyUsed=false;useUntil=0;
            // D62: a newly observed native child may share the previous XR
            // sample. It owns neither that child's motion permission nor its
            // fractional ready-stroke travel. Wait for fresh physical work.
            lastClockStop=true;lastStatus=Status::WorkStopped;
        }
        owner=w;out.totalSeconds=totalSeconds;
        out.use=w.ready&&useUntil&&now<useUntil;out.useUntil=out.use?useUntil:0;
        const auto geometry=[&](const Geometry& g,int type) noexcept {
            return g.valid&&g.entity&&g.type==type&&g.instance==w.instance&&g.lifetime==w.lifetime&&
                g.poseEpoch==f.poseEpoch&&g.sourceEpoch==f.sourceEpoch&&fresh(g.tick,now)&&finite3(g.point)&&
                std::isfinite(g.radius)&&g.radius>=.02f&&g.radius<=.40f;
        };
        const auto worldPalm=[&](int s) noexcept {const auto& p=in.palm[s];
            return p.valid&&finite3(p.point)&&fresh(p.tick,now)&&p.poseEpoch==f.poseEpoch&&p.sourceEpoch==f.sourceEpoch;};
        // D62: native geometry/contact can change between two consumers of one
        // XR sample. Reuse of a motion sample never renews its native permission.
        // Qualify the current recipient/contact before the duplicate fast path;
        // a lost contact consumes the cursor and fractional movement, so coming
        // back cannot earn travel across a gap or revive the same sample.
        const auto pause=[&](Status why) noexcept {
            sequence=f.sequence;timeNs=f.timeNs;haveMotion=haveRotationD41=false;strokeTravel=0;
            useUntil=0;out.use=false;out.useUntil=0;out.status=why;return out;
        };
        float resourceTilt=0,relative[3]{},rotationD41[4]{},bellowsY=0,bellowsWorldZ=0;
        bool angularD41=false;
        const bool resourceHomeD86=w.kind==Resource&&completed&&w.phase==2&&in.resourceTailD86&&in.resourceHomeD86;
        if(w.kind==Resource&&!resourceHomeD86){
            if(!geometry(in.pot,6)||!worldPalm(next))return pause(Status::GeometryRequired);
            if(distance(in.palm[next].point,in.pot.point)>in.pot.radius)return pause(Status::MoveToPot);
            float up[3]{};if(!upVector(f.hand[next].rotation,up))return pause(Status::InvalidFrame);
            float dot=0;for(int j=0;j<3;++j)dot+=up[j]*neutralUp[j];
            resourceTilt=std::acos(std::clamp(dot,-1.f,1.f));
            if(resourceTilt<.40f)return pause(Status::TiltToPour);
        }else if(resourceHomeD86){
            if(!worldPalm(next))return pause(Status::GeometryRequired);
        }else if(w.kind==Mortar){
            if(!geometry(in.mortar,2)||in.mortar.entity!=entities[1]||!worldPalm(0)||!worldPalm(1))
                return pause(Status::GeometryRequired);
            if(in.mortarContact.required){
                const auto& c=in.mortarContact;
                if(!c.valid||c.instance!=w.instance||c.parent!=w.parent||c.lifetime!=w.lifetime||
                   c.poseEpoch!=f.poseEpoch||c.sourceEpoch!=f.sourceEpoch||!fresh(c.tick,now)||
                   c.pestle!=entities[0]||c.mortar!=entities[1]||!std::isfinite(c.gap)||c.gap<0)
                    return pause(Status::GeometryRequired);
                if(c.gap>.035)return pause(Status::WorkStopped);
            }else if(distance(in.palm[0].point,in.mortar.point)>in.mortar.radius)return pause(Status::WorkStopped);
            float worldRelative[3]{};for(int j=0;j<3;++j)worldRelative[j]=f.hand[0].position[j]-f.hand[1].position[j];
            if(!localVector(f.hand[1].rotation,worldRelative,relative))return pause(Status::InvalidFrame);
            const auto& contactD41=in.mortarContact;
            angularD41=contactD41.required&&contactD41.valid&&std::isfinite(contactD41.surfaceRadiusD41)&&
                contactD41.surfaceRadiusD41>=.001&&contactD41.surfaceRadiusD41<=.08;
            if(angularD41&&!relativeRotationD41(f.hand[0].rotation,f.hand[1].rotation,rotationD41))return pause(Status::InvalidFrame);
        }else{
            const auto& c=in.tableContact;
            if(!c.geometryValid||!finite3(c.point)||!worldPalm(next)||
               !std::isfinite(c.rangeCraft1)||c.rangeCraft1<.10f||c.rangeCraft1>.200001f||
               distance(in.palm[next].point,c.point)>c.rangeCraft1+.000001f)return pause(Status::GeometryRequired);
            bellowsY=f.hand[next].position[1]-f.head[1];bellowsWorldZ=in.palm[next].point[2];
            if(!std::isfinite(bellowsY))return pause(Status::InvalidFrame);
        }
        if(f.sequence==sequence){out.clockStop=lastClockStop;out.status=lastStatus;return out;}
        double dt=timeNs&&f.timeNs>timeNs?double(f.timeNs-timeNs)*1e-9:0;
        sequence=f.sequence;timeNs=f.timeNs;
        double credit=0;bool physicalWorkD49=false;
        if(w.kind==Resource){
            // Absolute sustained tilt permits steady flow. Pure yaw about local
            // up does not tilt the acquired vessel; translating adds no work.
            if(w.working&&dt>0&&dt<=.08)credit=dt*(resourceHomeD86?1.:std::clamp(double((resourceTilt-.40f)/.50f),.15,1.5));
            physicalWorkD49=credit>0;out.status=credit>0?Status::Pouring:Status::WorkStopped;
        }else if(w.kind==Mortar){
            const auto& contactD41=in.mortarContact;
            if(haveMotion){const float d=distance(relative,lastRelative);
                double arcD41=0;bool continuousD41=true;
                if(angularD41&&haveRotationD41){double dot=0,aNorm=0,bNorm=0;
                    for(int j=0;j<4;++j){dot+=double(rotationD41[j])*lastRotationD41[j];
                        aNorm+=double(rotationD41[j])*rotationD41[j];bNorm+=double(lastRotationD41[j])*lastRotationD41[j];}
                    // Re-normalize the float storage before comparing it;
                    // roundoff of an unchanged quaternion must not accumulate
                    // fictitious rotation while the pestle is held still.
                    const double angle=2.*std::acos(std::clamp(std::fabs(dot)/std::sqrt(aNorm*bNorm),0.,1.));
                    // Reject tracking discontinuities rather than turn them
                    // into grind work. Quaternion sign alone has no movement.
                    continuousD41=dt>0&&dt<=.08&&angle<=std::min(1.,dt*16.);
                    if(continuousD41&&angle>1.e-5)arcD41=angle*contactD41.surfaceRadiusD41;
                }
                if(d<=.10f&&continuousD41){
                    const double travel=std::max(double(d),arcD41);
                    // Permission is current motion, not the quantized credit below.
                    // Otherwise XR90/native45 can read only intervening zero-credit
                    // samples and discard all work earned by continuous movement.
                    physicalWorkD49=w.working&&travel>1.e-6;
                    strokeTravel+=float(travel);
                    if(w.working&&strokeTravel>=.0015f){credit=double(strokeTravel)/.16;strokeTravel=0;}}
                else strokeTravel=0;
            }
            for(int j=0;j<3;++j)lastRelative[j]=relative[j];haveMotion=true;
            haveRotationD41=angularD41;for(int j=0;j<4;++j)lastRotationD41[j]=rotationD41[j];
            if(w.ready&&!readyUsed&&credit==0&&strokeTravel>=.025f){out.use=true;readyUsed=true;strokeTravel=0;}
            out.status=credit>0?Status::Grinding:physicalWorkD49?Status::MovingBelowQuantumD49:Status::WorkStopped;
        }else{
            // Both actual WORLD vertical travel and hand-relative-to-HMD XR
            // travel must agree. Walking/crouching the whole body is no pump.
            const float y=bellowsY,worldZ=bellowsWorldZ;
            if(haveMotion){const float relative=y-lastVertical,world=worldZ-lastWorldVertical;
                const float d=(relative*world>0)?(std::min)(std::fabs(relative),std::fabs(world)):0.f;
                if(std::fabs(relative)<=.10f&&std::fabs(world)<=.10f){
                    physicalWorkD49=w.working&&d>1.e-6f;
                    // D51: READY is a deliberate rope PULL, not arbitrary
                    // vertical motion. Keep both legs of an already running
                    // native pump eligible for clock credit, but only a
                    // continuous downward stroke may request the next LT.
                    // Raising the hand rearms its baseline; repeated small
                    // up/down motion cannot accumulate a fictitious pull.
                    if(w.ready&&!w.working){
                        if(relative<0&&world<0)strokeTravel+=d;
                        else if(relative>0&&world>0)strokeTravel=0;
                    }else strokeTravel+=d;
                    if(strokeTravel>=.0015f){if(w.working)credit=double(strokeTravel)/.16;
                        if(w.ready&&!readyUsed&&strokeTravel>=.025f){out.use=true;readyUsed=true;}
                        if(w.working||out.use)strokeTravel=0;}}
                else strokeTravel=0;
            }
            lastVertical=y;lastWorldVertical=worldZ;haveMotion=true;out.status=credit>0?Status::Pumping:physicalWorkD49?Status::MovingBelowQuantumD49:Status::WorkStopped;
        }
        if(out.use){if(!useUntil)useUntil=now+80;out.useUntil=useUntil;out.status=Status::ReadyGesture;}
        // Drop excess movement; never bank work that continues after the hand stops.
        credit=std::clamp(credit,0.,.12);totalSeconds+=credit;
        out.totalSeconds=totalSeconds;out.clockStop=!physicalWorkD49;return out;
    }
};
}
