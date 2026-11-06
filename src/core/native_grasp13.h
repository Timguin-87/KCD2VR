#pragma once
// Shared POD contract for the native pickup owner and the existing VRIK attachment writer.
// This code does not create items, apply inventory changes or emulate native physics.
#include <cmath>
#include <cstring>
#include <cstdint>
#include <algorithm>
#include <atomic>
#include <array>
#include "pickup_grip_d64.h"
namespace kcd2vr::grasp13 {
// Legacy clearance preference stays readable for compatibility. D40 ordinary
// native grasps use the same native item/finger relation from first placement;
// bootstrap-only clearance no longer moves the item and then snaps it back.
// Chest stow radius remains live; native Drop/Stow owns the transaction.
inline std::atomic<float> heldClearance35{.03f},stowRadius35{.32f}; // seeds match LoadConfig defaults (VrHeldPropClearanceMeters35/VrPropStowRadiusMeters35)
inline float safeClearance35(float x) noexcept {return std::isfinite(x)?std::clamp(x,0.f,.08f):.03f;}
inline float safeStowRadius35(float x) noexcept {return std::isfinite(x)?std::clamp(x,.20f,.45f):.32f;}
inline void setPresentation35(float clearance,float radius) noexcept {
    heldClearance35.store(safeClearance35(clearance),std::memory_order_release);
    stowRadius35.store(safeStowRadius35(radius),std::memory_order_release);
}
inline bool outward35(const float (&nativeDelta)[3],float metres,float (&shift)[3]) noexcept {
    for(float& x:shift)x=0;double n=0;
    for(float x:nativeDelta){if(!std::isfinite(x))return false;n+=double(x)*x;}
    if(n<1e-8||n>.8*.8)return false; // no guessed direction for a zero/native-invalid grip
    const float scale=safeClearance35(metres)/float(std::sqrt(n));
    for(int i=0;i<3;++i)shift[i]=nativeDelta[i]*scale;return true;
}
struct Pose { float q[4]{0,0,0,1}; float p[3]{}; float scale=1; };
static_assert(sizeof(Pose)==32,"retail QuatTS used by the native hand-drop consumer");
inline bool valid(const Pose& p) noexcept {
    double n=0;for(float f:p.q){if(!std::isfinite(f))return false;n+=double(f)*f;}
    for(float f:p.p)if(!std::isfinite(f))return false;
    return n>.98&&n<1.02&&std::isfinite(p.scale)&&p.scale>.01f&&p.scale<10.f;
}
// Native CEntity cached Matrix34 -> uniform-scale QuatTS. Reject shear, mirrors,
// singular/nonfinite matrices; do not invent a release pose on failed conversion.
inline bool fromMatrix(const float (&m)[12],Pose& p) noexcept {
    for(float x:m)if(!std::isfinite(x))return false;
    double r[3][3]{};double lengths[3]{};
    for(int c=0;c<3;++c){for(int row=0;row<3;++row)lengths[c]+=double(m[row*4+c])*m[row*4+c];lengths[c]=std::sqrt(lengths[c]);}
    const double scale=(lengths[0]+lengths[1]+lengths[2])/3.;
    if(scale<=.01||scale>=10)return false;
    for(int c=0;c<3;++c){if(std::abs(lengths[c]-scale)>scale*.002)return false;
        for(int row=0;row<3;++row)r[row][c]=m[row*4+c]/scale;}
    for(int a=0;a<3;++a)for(int b=a+1;b<3;++b){double d=0;for(int row=0;row<3;++row)d+=r[row][a]*r[row][b];if(std::abs(d)>.003)return false;}
    const double det=r[0][0]*(r[1][1]*r[2][2]-r[1][2]*r[2][1])-r[0][1]*(r[1][0]*r[2][2]-r[1][2]*r[2][0])+r[0][2]*(r[1][0]*r[2][1]-r[1][1]*r[2][0]);
    if(det<.995||det>1.005)return false;
    double q[4]{};const double trace=r[0][0]+r[1][1]+r[2][2];
    if(trace>0){const double v=2*std::sqrt(trace+1);q[3]=.25*v;q[0]=(r[2][1]-r[1][2])/v;q[1]=(r[0][2]-r[2][0])/v;q[2]=(r[1][0]-r[0][1])/v;}
    else {int i=r[1][1]>r[0][0]?1:0;if(r[2][2]>r[i][i])i=2;const int j=(i+1)%3,k=(i+2)%3;
        const double v=2*std::sqrt(std::max(0.,1+r[i][i]-r[j][j]-r[k][k]));if(v<1e-8)return false;
        q[i]=.25*v;q[j]=(r[i][j]+r[j][i])/v;q[k]=(r[i][k]+r[k][i])/v;q[3]=(r[k][j]-r[j][k])/v;}
    for(int i=0;i<4;++i)p.q[i]=float(q[i]);for(int i=0;i<3;++i)p.p[i]=m[i*4+3];p.scale=float(scale);
    return valid(p);
}
struct HandFrame {
    std::uintptr_t character=0;std::uint64_t epoch=0,sample=0,tick=0;
    bool tracked[2]{};float grip[2]{},chestDistance[2]{};
};
// Original XR input, before pose permission or grace. This is an identity/input
// witness for acquisition only; it has no chest, item placement or terminal pose.
struct AcquisitionInput55 {
    std::uintptr_t character=0;
    std::uint64_t epoch=0,trackingEpoch=0,sample=0,tick=0;
    bool headTracked=false,tracked[2]{};float grip[2]{};
};
// A transported hand Use may preserve a pre-action visual reference. It does
// not identify an accepted action or authorize pose/item/native writes.
struct PendingUse55 {
    std::uintptr_t character=0;std::uint64_t epoch=0,serial=0,requestedAt=0;
    int side=-1;
};
// Read-only presentation witness. A queued native Picking action only permits
// saving a pre-action reference; active requires the SAME installed root action.
// It never authorizes item placement, native event skipping or inventory writes.
struct Acquisition54 {
    std::uintptr_t character=0,action=0,scope=0;
    std::uint64_t serial=0,epoch=0,startedAt=0,tick=0;
    std::uint32_t nativeToken=0;int side=-1;bool active=false;
    // Copied only from this exact active Picking's native grasp. Presentation
    // must also join the independently current real attachment Binding.
    std::uint64_t graspToken=0,graspKey=0;std::uintptr_t graspItem=0;
    std::uint32_t graspEntity=0;
    std::uint64_t generation55=0,trackingEpoch55=0;
};
inline bool freshAcquisition54(const Acquisition54& p,std::uint64_t now) noexcept {
    return p.character&&p.action&&p.serial&&p.epoch&&p.startedAt&&p.tick&&
        p.side>=0&&p.side<2&&now>=p.tick&&now-p.tick<=80&&p.startedAt<=p.tick&&
        (!p.active||p.scope);
}
// D56: body/camera foundation reads an exact installed Picking lifecycle.
// Physical grabbing still uses ReadAcquisition54, whose raw-input/attachment
// requirements are intentionally stricter. This seam never grants item writes.
using FoundationReaderD56=bool (*)(Acquisition54&) noexcept;
inline FoundationReaderD56 foundationReaderD56=nullptr;
inline bool readFoundationD56(FoundationReaderD56 legacy,Acquisition54& out) noexcept {
    return foundationReaderD56?foundationReaderD56(out):(legacy&&legacy(out));
}
using FingerRotations54=std::array<std::array<float,4>,32>;
struct FingerMapping55 {
    std::uintptr_t skeleton=0;std::uint64_t generation=0;std::uint32_t joints=0;
    int wrist=-1;unsigned count=0;std::array<int,32> ids{},parents{};
    bool operator==(const FingerMapping55& b)const noexcept {
        if(skeleton!=b.skeleton||generation!=b.generation||joints!=b.joints||wrist!=b.wrist||count!=b.count)return false;
        for(unsigned i=0;i<count&&i<ids.size();++i)if(ids[i]!=b.ids[i]||parents[i]!=b.parents[i])return false;
        return true;
    }
};
inline bool validFingerMapping55(const FingerMapping55& m) noexcept {
    if(!m.skeleton||!m.generation||!m.joints||m.wrist<0||unsigned(m.wrist)>=m.joints||!m.count||m.count>m.ids.size())return false;
    for(unsigned i=0;i<m.count;++i)if(m.ids[i]<0||unsigned(m.ids[i])>=m.joints||m.parents[i]<0||
        unsigned(m.parents[i])>=m.joints||m.ids[i]==m.parents[i]||m.ids[i]==m.wrist)return false;
    for(unsigned i=0;i<m.count;++i)for(unsigned j=0;j<i;++j)if(m.ids[i]==m.ids[j])return false;
    return true;
}
// Native finger locals from the SAME original-joint transaction as a held
// attachment grip. An XR sample alone does not identify an animation phase;
// this immutable payload travels with that transaction's tool pose/proof.
struct FingerReference55 {
    Acquisition54 acquisition{};FingerRotations54 rotations{};FingerMapping55 mapping{};
    std::uint64_t sample=0,tick=0;unsigned count=0;
};
struct Binding {
    std::uintptr_t character=0,manager=0,item=0,logic=0;
    std::uint64_t token=0,key=0,epoch=0;std::uint32_t entity=0,actor=0;int side=-1;
    bool attached=false,place=false;
    bool socketOnly28=false; // observation/correlation only; D71 may separately prove callback-local visual placement, never HELD/input/physics
    Pose grip55{};bool gripReady55=false; // native original-joint grip, fixed for this exact held resource
    FingerReference55 fingers55{};
    // D72 presentation receipt only: real AttachOnly returned and native hand
    // WUID acknowledged this exact request. No semantic HELD/place/socket claim.
    std::uint64_t acceptedAttachTickD72=0;
    Acquisition54 acceptedAcquisitionD72{}; // copied only on the admitted native Attach callback
};
// Presentation only after native Picking cleanup returned, before controller
// adoption. No action/logic pointer or HELD/inventory/physics authority survives.
struct FinishingPresentationR94 {
    Binding binding{};
    std::uint64_t requestedAt=0,serial=0,generation=0,trackingEpoch=0,sourceTick=0;
};
inline bool sameFinishingPresentationR94(const FinishingPresentationR94& a,const FinishingPresentationR94& b) noexcept {
    return a.binding.token&&a.binding.token==b.binding.token&&a.binding.key==b.binding.key&&
        a.binding.manager==b.binding.manager&&a.binding.item==b.binding.item&&
        a.binding.character==b.binding.character&&a.binding.epoch==b.binding.epoch&&
        a.binding.side==b.binding.side&&a.binding.entity==b.binding.entity&&a.binding.actor==b.binding.actor&&
        a.requestedAt==b.requestedAt&&a.serial==b.serial&&a.generation==b.generation&&a.trackingEpoch==b.trackingEpoch&&
        a.binding.gripReady55==b.binding.gripReady55&&std::memcmp(&a.binding.grip55,&b.binding.grip55,sizeof(a.binding.grip55))==0;
}
// Produced only after the real current attachment socket was matched against
// an ORIGINAL native-joint pose transaction. This is NOT an item placement.
struct SocketProof28 {
    Binding binding{};std::uintptr_t self=0,attachment=0;
    std::uint64_t sample=0,tick=0;int joint=-1;float relative[7]{};
    Pose grip55{};bool gripReady55=false;
    FingerReference55 fingers55{};
};
struct Placement {
    Binding binding{};std::uintptr_t self=0,attachment=0;
    std::uint64_t sample=0,tick=0;Pose world{};
};
inline bool same(const Binding& a,const Binding& b) noexcept {
    return a.token&&a.token==b.token&&a.key==b.key&&a.manager==b.manager&&
        a.item==b.item&&a.logic==b.logic&&a.character==b.character&&a.epoch==b.epoch&&
        a.side==b.side&&a.entity==b.entity&&a.actor==b.actor;
}
// Presentation identity outlives the unique native Picking logic: native cleanup
// deliberately clears logic while the SAME item remains in the SAME hand. This
// comparator is for finger-pose caching and independently qualified visual
// continuity only; never use it for transfer or semantic hold authority.
inline bool sameHeldResource54(const Binding& a,const Binding& b) noexcept {
    return a.token&&a.key&&a.item&&a.manager&&a.character&&a.epoch&&a.entity&&a.actor&&
        a.token==b.token&&a.key==b.key&&a.item==b.item&&a.manager==b.manager&&a.character==b.character&&
        a.epoch==b.epoch&&a.entity==b.entity&&a.actor==b.actor&&a.side==b.side;
}
inline bool acquisitionOwns55(const Binding& b,const Acquisition54& a) noexcept {
    return sameHeldResource54(b,b)&&b.side>=0&&b.side<2&&
        a.active&&a.action&&a.scope&&a.serial&&a.startedAt&&a.tick&&a.startedAt<=a.tick&&
        a.character==b.character&&a.epoch==b.epoch&&a.side==b.side&&
        a.graspToken==b.token&&a.graspKey==b.key&&a.graspItem==b.item&&a.graspEntity==b.entity;
}
inline bool pendingAttachmentPresentationD72(const Binding& b,const Acquisition54& a,std::uint64_t now) noexcept {
    return b.acceptedAttachTickD72&&now>=b.acceptedAttachTickD72&&now-b.acceptedAttachTickD72<=80&&
        b.logic&&!b.attached&&!b.place&&!b.socketOnly28&&acquisitionOwns55(b,a)&&freshAcquisition54(a,now);
}
inline bool sameAcquisition55(const Acquisition54& a,const Acquisition54& b) noexcept {
    return a.serial&&a.serial==b.serial&&a.action==b.action&&a.scope==b.scope&&a.nativeToken==b.nativeToken&&
        a.character==b.character&&a.epoch==b.epoch&&a.side==b.side&&a.startedAt==b.startedAt&&
        a.generation55==b.generation55&&a.trackingEpoch55==b.trackingEpoch55;
}
inline bool validFingerReference55(const Binding& b,const FingerReference55& r) noexcept {
    if(!r.sample||!r.tick||!r.count||r.count>r.rotations.size()||!acquisitionOwns55(b,r.acquisition)||
       !validFingerMapping55(r.mapping)||r.mapping.count!=r.count)return false;
    for(unsigned i=0;i<r.count;++i){double q=0;for(float x:r.rotations[i]){if(!std::isfinite(x))return false;q+=double(x)*x;}
        if(q<.98||q>1.02)return false;}
    return true;
}
struct NativeFinger54 {
    Binding owner{};FingerRotations54 rotations{};unsigned count=0;bool ready=false;
    void observe(const Binding& b) noexcept {
        if(!sameHeldResource54(owner,b)||(!(b.attached&&b.place)&&!b.socketOnly28))*this={};
    }
    bool capture(const Binding& b,const Acquisition54& acquisition,std::uint64_t now,
                 const FingerRotations54& native,unsigned n) noexcept {
        observe(b);if(ready)return count==n;
        // Match the actual pickup resource, not merely the same actor/hand.
        // An active reach for another item must never seed this held pose.
        if(!freshAcquisition54(acquisition,now)||!acquisition.active||!sameHeldResource54(b,b)||
           acquisition.character!=b.character||acquisition.epoch!=b.epoch||acquisition.side!=b.side||
           acquisition.graspToken!=b.token||acquisition.graspKey!=b.key||
           acquisition.graspItem!=b.item||acquisition.graspEntity!=b.entity||
           (!(b.attached&&b.place)&&!b.socketOnly28)||!n||n>rotations.size())return false;
        for(unsigned i=0;i<n;++i){double q=0;for(float x:native[i]){if(!std::isfinite(x))return false;q+=double(x)*x;}
            if(q<.98||q>1.02)return false;}
        owner=b;rotations=native;count=n;ready=true;return true;
    }
    bool current(const Binding& b,unsigned n) noexcept {
        observe(b);return ready&&n==count;
    }
    bool currentCanonical55(const Binding& b,const FingerMapping55& mapping) noexcept {
        observe(b);
        if(ready&&(!validFingerMapping55(mapping)||!(owner.fingers55.mapping==mapping)))*this={};
        return ready&&count==mapping.count;
    }
    bool captureCanonical55(const Binding& b,unsigned n,const FingerMapping55* mapping=nullptr) noexcept {
        observe(b);if(mapping&&ready&&(!validFingerMapping55(*mapping)||!(owner.fingers55.mapping==*mapping)))*this={};
        if(ready)return count==n;
        if(!b.gripReady55||b.fingers55.count!=n||!validFingerReference55(b,b.fingers55)||
           (mapping&&!(b.fingers55.mapping==*mapping)))return false;
        owner=b;rotations=b.fingers55.rotations;count=n;ready=true;return true;
    }
};
inline bool fresh(std::uint64_t now,std::uint64_t then,std::uint64_t cap=150) noexcept {
    return then&&now>=then&&now-then<=cap;
}
enum class Intent { Hold, Drop, Stow, SafetyDrop };
// C34 review correction + Codex/Claude compatibility helper: the leave-arm
// threshold follows the OLD radius+0.06 rule for saved smaller radii and only
// floors at 0.34 m for the new larger defaults. So a saved 0.20/0.24 radius
// keeps its original behavior, 0.28 is bit-identical (0.34 = 0.28+0.06), the
// 0.32 default gets the 0.34 floor (the 0.35->0.20 stow fix), and radii above
// the floor must leave their own zone.
inline constexpr float kStowLeaveFloor35=.34f;
inline float stowLeave35(float radius) noexcept {
    const float r=safeStowRadius35(radius);
    return std::max(r,std::min(kStowLeaveFloor35,r+.06f));
}
struct Gesture {
    std::uint64_t lastSample=0,lastTick=0,nearSince=0;
    float acquiredDistance=0;bool leftStowArea=false;
    void acquired(float d,float radius=.24f) noexcept { *this=Gesture{};acquiredDistance=d;leftStowArea=std::isfinite(d)&&d>stowLeave35(radius); }
    Intent update(const HandFrame& f,int side,std::uint64_t now,bool context,float release=.35f,float pull=.10f,float radius=.24f) noexcept {
        if(!context)return Intent::SafetyDrop;
        if(side<0||side>1||!f.tracked[side]||!fresh(now,f.tick)||
            !std::isfinite(f.grip[side])||!std::isfinite(f.chestDistance[side]))return Intent::SafetyDrop;
        if(f.tick<lastTick||f.sample<lastSample){nearSince=0;return Intent::SafetyDrop;}
        const float d=f.chestDistance[side];
        radius=safeStowRadius35(radius);
        const bool near=d<=radius&&leftStowArea&&acquiredDistance-d>=pull;
        // C59: releasing a prop after a witnessed inward move INTO the chest is
        // an explicit stow, even before the held 180-ms dwell completes. The
        // previous held-near sample must still be fresh and distinct: a first
        // release sample, duplicate pose, near-body acquisition or tracking/UI
        // loss never invents storage. Native transfer remains the only result.
        if(f.grip[side]<release)return near&&nearSince&&f.sample>lastSample&&
            f.tick>lastTick&&fresh(f.tick,lastTick)?Intent::Stow:Intent::Drop;
        if(f.sample==lastSample)return Intent::Hold;
        lastSample=f.sample;lastTick=f.tick;
        if(d>stowLeave35(radius)){leftStowArea=true;acquiredDistance=std::max(acquiredDistance,d);nearSince=0;}
        if(!near){nearSince=0;return Intent::Hold;}
        if(!nearSince)nearSince=f.tick;
        return f.tick>=nearSince&&f.tick-nearSince>=180?Intent::Stow:Intent::Hold;
    }
};
enum class Terminal { None, Drop, Stow, Fault, CancelAttach };
// An attempted irreversible native transaction cannot become eligible again after
// an exception or a missing receipt. Success is acknowledged separately.
struct Once {
    Terminal chosen=Terminal::None;bool invoked=false,acknowledged=false;
    bool claim(Terminal t) noexcept {
        if(chosen!=Terminal::None||t==Terminal::None)return false;
        chosen=t;invoked=true;return true;
    }
    void acknowledge() noexcept {if(invoked&&chosen!=Terminal::Fault)acknowledged=true;}
    void fault() noexcept {chosen=Terminal::Fault;}
};
struct Motion {
    Placement older{},newer{};bool have=false,pair=false;
    bool add(const Placement& p) noexcept {
        if(!p.binding.token||!p.sample||!p.tick||!valid(p.world))return false;
        if(have&&same(newer.binding,p.binding)&&p.sample==newer.sample)return false;
        pair=have&&same(newer.binding,p.binding)&&p.self==newer.self&&p.attachment==newer.attachment&&
             p.sample>newer.sample&&p.tick>newer.tick&&p.tick-newer.tick<=100;
        if(pair){double d=0;for(int i=0;i<3;++i){const double v=p.world.p[i]-newer.world.p[i];d+=v*v;}
            // A discontinuity is not a throw. Normal high-speed movement is bounded below.
            pair=d<=.50*.50&&std::abs(p.world.scale-newer.world.scale)<.001f;}
        older=newer;newer=p;have=true;return true;
    }
    void velocity(std::uint64_t now,bool deliberate,float (&v)[3],float (&w)[3]) const noexcept {
        for(int i=0;i<3;++i)v[i]=w[i]=0;
        if(!deliberate||!pair||!fresh(now,newer.tick,100))return;
        const double dt=double(newer.tick-older.tick)*.001;
        if(dt<.002||dt>.10)return;
        double n=0;for(int i=0;i<3;++i){v[i]=float((newer.world.p[i]-older.world.p[i])/dt);n+=double(v[i])*v[i];}
        if(n>64){const double k=8/std::sqrt(n);for(float& x:v)x=float(x*k);}
        const auto& a=newer.world.q;const auto& b=older.world.q;
        double q[4]{-a[3]*b[0]+a[0]*b[3]-a[1]*b[2]+a[2]*b[1],
                    -a[3]*b[1]+a[0]*b[2]+a[1]*b[3]-a[2]*b[0],
                    -a[3]*b[2]-a[0]*b[1]+a[1]*b[0]+a[2]*b[3],
                    a[3]*b[3]+a[0]*b[0]+a[1]*b[1]+a[2]*b[2]};
        if(q[3]<0)for(double& x:q)x=-x;
        const double s=std::sqrt(q[0]*q[0]+q[1]*q[1]+q[2]*q[2]);
        if(s>1e-8){const double speed=std::min(20.,2*std::atan2(s,std::max(0.,q[3]))/dt);
            for(int i=0;i<3;++i)w[i]=float(q[i]*speed/s);}
    }
};
// D85 interact: release velocity frozen at the grip-release edge.
// Before D85 the drop read Motion::velocity at terminalAt, after the owner had
// waited for native cleanup (WAIT_NATIVE_CLEANUP), with a 100 ms freshness cap,
// and additionally required the current entity within 2 cm of the last
// placement. A held item swung at 2 m/s travels 2.2 cm in one 11 ms VR frame,
// so the entity's own pose (one native update behind the placement) failed the
// 2 cm test for every real throw and the item was dropped with v=0; a release
// that waited for cleanup lost the sample to the 100 ms cap. The velocity is
// the same two-sample measurement of the item's own placements, taken when
// the hand opens; continuity still rejects a teleport/socket return.
struct ReleaseD85 {
    bool valid=false;std::uint64_t at=0,sampleTick=0,pairMs=0;
    float v[3]{},w[3]{},p[3]{};
    void freeze(const Motion& m,std::uint64_t now) noexcept {
        *this={};float lv[3]{},aw[3]{};
        if(!m.pair||!fresh(now,m.newer.tick,100))return;
        m.velocity(now,true,lv,aw);
        valid=true;at=now;sampleTick=m.newer.tick;pairMs=m.newer.tick-m.older.tick;
        for(int i=0;i<3;++i){v[i]=lv[i];w[i]=aw[i];p[i]=m.newer.world.p[i];}
    }
    double speed() const noexcept {return std::sqrt(double(v[0])*v[0]+double(v[1])*v[1]+double(v[2])*v[2]);}
    // Old 2 cm plus the distance this measured speed covers since the frozen
    // sample (at least one sample interval), never more than 1 m.
    double tolerance(std::uint64_t now) const noexcept {
        if(!valid||now<sampleTick)return 0;
        const auto ms=std::max<std::uint64_t>(now-sampleTick,pairMs);
        return std::min(1.,.02+speed()*double(ms)*.001);
    }
    bool continuous(const Pose& release,std::uint64_t now) const noexcept {
        if(!valid||now<sampleTick||!kcd2vr::grasp13::valid(release))return false;
        double d=0;for(int i=0;i<3;++i){const double x=double(release.p[i])-p[i];d+=x*x;}
        const double t=tolerance(now);return d<=t*t;
    }
};
}
