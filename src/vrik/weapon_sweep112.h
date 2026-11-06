#pragma once
#include "physical_weapon.h"
namespace kcd2vr::physical {
// A shield is a broad face, not the weapon grip-to-tip half-line. Keep this an
// explicit bounds proxy: centre plus all four rim directions, no convex-body
// or native-block claim. Exact item bounds are already copied by the adapter.
inline bool shield_features59c10r2(const Profile& p,std::array<Vec3,5>& features,
                                  double& radius,double& halfThickness) noexcept {
    if(!p.shield176()||!p.geometry||!p.diagnosticBounds59c6||p.side<0||p.side>1)return false;
    const auto lo=p.diagnosticBoundsMin59c6,hi=p.diagnosticBoundsMax59c6;
    if(!m::finite(lo)||!m::finite(hi))return false;
    const double low[3]{lo.x,lo.y,lo.z},high[3]{hi.x,hi.y,hi.z};
    double span[3]{};int thin=0;
    for(int k=0;k<3;++k){span[k]=high[k]-low[k];if(!std::isfinite(span[k])||span[k]<0||span[k]>3)return false;if(span[k]<span[thin])thin=k;}
    const int u=(thin+1)%3,v=(thin+2)%3;
    if(span[u]<.08||span[v]<.08||span[thin]>.24)return false;
    const Vec3 center=(lo+hi)*.5;
    features.fill(center);
    auto at=[](Vec3& x,int k)->double&{return k==0?x.x:k==1?x.y:x.z;};
    at(features[1],u)=low[u];at(features[2],u)=high[u];
    at(features[3],v)=low[v];at(features[4],v)=high[v];
    radius=0;for(const auto feature:features)radius=std::max(radius,m::length(feature-p.grip[p.side].p));
    halfThickness=span[thin]*.5;
    return std::isfinite(radius)&&radius<=3;
}
// Every fixed native feature is subdivided for its own distance from the grip.
// D72 shares the existing bounded radius plan with the broad crossbow proxy.
// The melee wrapper below retains all five original shaft features exactly.
template<std::size_t N,class Ray> Contact103 sweep_feature_radius_d72(const Profile& p,Pose from,Pose to,
        bool initial,double chord,const std::array<Vec3,N>& features,Ray&& ray) {
    static_assert(N>=1&&N<=18);
    const double angle=m::angle(from.q,to.q);const unsigned cap=initial?72:18;
    const Vec3 grip=p.grip[p.side].p;
    std::array<double,N> radii{};std::array<unsigned,N> plan{};
    for(unsigned i=0;i<N;++i){
        radii[i]=m::length(features[i]-grip);
        if(!std::isfinite(radii[i])||radii[i]>3){Contact103 unknown{};unknown.known=false;unknown.fraction=0;
            unknown.coverage169r2.reason=sweep169r2::Reason::InvalidQuery;return unknown;}
    }
    const auto makePlan=[&](double arc,double tolerance,std::array<unsigned,N>& counts){
        unsigned total=0;for(unsigned i=0;i<N;++i){
            const double step=radii[i]>tolerance?2*std::acos(std::clamp(1-tolerance/radii[i],-1.,1.)):3.141592653589793;
            counts[i]=std::max(1u,unsigned(std::ceil(std::max(0.,arc/step-1e-12))));total+=counts[i];
        }return total;
    };
    double prefix=1.;unsigned total=makePlan(angle,chord,plan);
    // Keep fine4mm initial binding. Ordinary motion may choose a coarser
    // current plan only as needed, within the existing12mm chord-error cap.
    if(total>cap&&!initial){
        std::array<unsigned,N> trial{};
        if(makePlan(angle,.012,trial)<=cap){
            double low=chord,high=.012;
            for(unsigned i=0;i<20;++i){const double mid=(low+high)*.5;
                if(makePlan(angle,mid,trial)<=cap)high=mid;else low=mid;}
            chord=high;
        }else chord=.012;
        total=makePlan(angle,chord,plan);
    }
    if(total>cap){
        double low=0,high=1;std::array<unsigned,N> trial{};
        for(unsigned i=0;i<28;++i){const double mid=(low+high)*.5;
            if(makePlan(angle*mid,chord,trial)<=cap)low=mid;else high=mid;}
        prefix=low;total=makePlan(angle*prefix,chord,plan);
    }
    const Pose goal=blend_weapon116(p,from,to,prefix);
    // Diagnostics count common arc intervals (the union of feature endpoints),
    // not a fictitious uniform grid. Motion admission below uses the exact
    // minimum checked fraction, independently of these integer counters.
    std::array<double,72> endpoints{};unsigned endpointCount=0;
    for(unsigned i=0;i<N;++i)for(unsigned j=1;j<=plan[i];++j){
        const double t=double(j)/plan[i];bool exists=false;
        for(unsigned k=0;k<endpointCount;++k)exists|=std::abs(endpoints[k]-t)<1e-12;
        if(!exists)endpoints[endpointCount++]=t;
    }
    std::sort(endpoints.begin(),endpoints.begin()+endpointCount);
    sweep169r2::Evidence evidence{};evidence.planned=endpointCount;evidence.requestedAngle=angle;
    std::array<unsigned,N> done{};std::array<double,N> checked{};Contact103 earliest{};bool haveHit=false;
    const auto common=[&](){return *std::min_element(checked.begin(),checked.end());};
    const auto progress=[&](double fraction){evidence.completed=0;
        for(unsigned i=0;i<endpointCount;++i)evidence.completed+=endpoints[i]<=fraction+1e-12;
        evidence.checkedAngle=angle*prefix*fraction;};
    for(unsigned query=0;query<total;++query){
        const double stop=haveHit?earliest.fraction:1.;unsigned feature=N;double least=2.;
        for(unsigned i=0;i<N;++i)if(checked[i]+1e-12<stop&&done[i]<plan[i]&&checked[i]<least){feature=i;least=checked[i];}
        if(feature==N)break;
        const auto a=blend_weapon116(p,from,goal,double(done[feature])/plan[feature]);
        const auto b=blend_weapon116(p,from,goal,double(done[feature]+1)/plan[feature]);
        const double margin=.009+radii[feature]*(1-std::cos(angle*prefix/(2*plan[feature])));
        ++evidence.attemptedRays;auto hit=contact103(ray(point(a,features[feature]),point(b,features[feature]),margin));
        if(!hit.known){
            const double checkedPrefix=common();progress(checkedPrefix);evidence.reason=hit.coverage169r2.reason;
            if(evidence.reason==sweep169r2::Reason::FullClear)evidence.reason=sweep169r2::Reason::InvalidQuery;
            Contact103 limited{};limited.known=checkedPrefix>0;limited.fraction=prefix*checkedPrefix;
            limited.limited115=limited.known;limited.coverage169r2=evidence;return limited;
        }
        ++evidence.completedRays;
        const double reached=(done[feature]+hit.fraction)/plan[feature];checked[feature]=reached;
        if(hit.fraction<1){
            if(!haveHit||reached<earliest.fraction){earliest=hit;earliest.fraction=reached;earliest.feature=features[feature];haveHit=true;}
            // No query may advance this feature through its measured surface.
            done[feature]=plan[feature];
        }else ++done[feature];
        // Other features must also cover this prefix before a hit can authorize
        // it. A tip ray checked farther ahead cannot clear untested shaft space.
        if(haveHit&&common()+1e-12>=earliest.fraction){
            progress(earliest.fraction);earliest.fraction*=prefix;evidence.reason=sweep169r2::Reason::SurfaceHit;
            earliest.coverage169r2=evidence;return earliest;
        }
    }
    const double checkedPrefix=common();progress(checkedPrefix);Contact103 result{};
    result.fraction=prefix*checkedPrefix;result.limited115=result.fraction<1;
    evidence.reason=result.limited115?sweep169r2::Reason::AngularPrefix:sweep169r2::Reason::FullClear;
    result.coverage169r2=evidence;return result;
}
template<class Ray> Contact103 sweep_shaft_radius_d67r1(const Profile& p,Pose from,Pose to,
        bool initial,double chord,Ray&& ray) {
    std::array<Vec3,5> features{};const auto grip=p.grip[p.side].p;
    for(unsigned i=0;i<5;++i)features[i]=grip+(p.tip-grip)*(i/4.);
    return sweep_feature_radius_d72(p,from,to,initial,chord,features,ray);
}
// Native whole-model bounds supply both broad axes; no assumed prod orientation
// or length. A 3x3 grid on the measured thin-axis midplane covers the transverse
// and stock bounds. Like the retained shield/shaft paths this is a bounded
// ray proxy, not a full mesh or swept-volume proof; the shared ray/time cap holds.
inline bool crossbow_features_d72(const Profile& p,std::array<Vec3,9>& features) noexcept {
    if(!crossbow_bounds_d72(p))return false;
    const auto lo=p.diagnosticBoundsMin59c6,hi=p.diagnosticBoundsMax59c6;
    const double low[3]{lo.x,lo.y,lo.z},high[3]{hi.x,hi.y,hi.z};
    int thin=0;for(int k=1;k<3;++k)if(high[k]-low[k]<high[thin]-low[thin])thin=k;
    const int u=(thin+1)%3,v=(thin+2)%3;
    for(unsigned i=0;i<3;++i)for(unsigned j=0;j<3;++j){
        double q[3]{(low[0]+high[0])*.5,(low[1]+high[1])*.5,(low[2]+high[2])*.5};
        q[u]=low[u]+(high[u]-low[u])*(i*.5);q[v]=low[v]+(high[v]-low[v])*(j*.5);
        features[i*3+j]={q[0],q[1],q[2]};
    }
    return true;
}
inline unsigned initial_target_ray_count_d72(const Profile& p) noexcept {return p.crossbowD72()?54u:22u;}
// D70 initial placement is an ownership handover, not a controller sweep from
// Henry's authored draw pose. Check the CURRENT thin-shaft proxy first: the
// complete shaft in both directions plus two 18mm diameters at all five existing
// shaft features. This is bounded ray geometry, not a convex/mesh overlap claim.
// Any hit, UNKNOWN or incomplete budget leaves the old transport path unchanged.
// Ordinary held motion and post-native return never enter this query.
template<class Ray> bool initial_target_clear_d70(const Profile& p,Pose target,Ray&& ray) {
    if(!p.initialAcquireSweepD44||p.nativeRejoinSweepR28||p.shield176()||
       !p.primary||!p.geometry||!p.nativeSeed||!p.key||!p.generation||!p.entity||
       p.side<0||p.side>1||!finite(target)||!m::finite(p.tip)||!finite(p.grip[p.side]))return false;
    const auto clear=[&](Vec3 a,Vec3 b){const auto h=contact103(ray(point(target,a),point(target,b),0.));
        return h.known&&!h.limited115&&!h.plane&&h.fraction>=1.;};
    if(p.crossbowD72()){
        if(!crossbow_bounds_d72(p))return false;
        const auto lo=p.diagnosticBoundsMin59c6,hi=p.diagnosticBoundsMax59c6;
        const double low[3]{lo.x,lo.y,lo.z},high[3]{hi.x,hi.y,hi.z};
        // All three native axes, with low/mid/high positions on both other axes.
        // Every chord is checked in both directions, including the full prod.
        // 54 rays fit within the existing one-shot 72-ray acquisition allowance.
        for(int axis=0;axis<3;++axis){const int u=(axis+1)%3,v=(axis+2)%3;
            for(unsigned i=0;i<3;++i)for(unsigned j=0;j<3;++j){
                double a[3]{};a[axis]=low[axis];a[u]=low[u]+(high[u]-low[u])*(i*.5);a[v]=low[v]+(high[v]-low[v])*(j*.5);
                double b[3]{a[0],a[1],a[2]};b[axis]=high[axis];
                const Vec3 from{a[0],a[1],a[2]},to{b[0],b[1],b[2]};
                if(!clear(from,to)||!clear(to,from))return false;
            }
        }
        return true;
    }
    const auto grip=p.grip[p.side].p;const auto shaft=p.tip-grip;
    const double length=m::length(shaft);if(!std::isfinite(length)||length<.04||length>2.4)return false;
    const auto axis=shaft*(1./length);
    const auto side=m::unit(m::cross(axis,std::abs(axis.z)<.8?Vec3{0,0,1}:Vec3{1,0,0}));
    if(!side)return false;const auto across=m::cross(axis,*side);
    if(!clear(grip,p.tip)||!clear(p.tip,grip))return false;
    for(unsigned i=0;i<5;++i){const auto at=grip+shaft*(i/4.);
        for(auto direction:{*side,across}){
            const auto offset=direction*.009;
            if(!clear(at-offset,at+offset)||!clear(at+offset,at-offset))return false;
        }
    }
    return true;
}

// Ray-query backend only: bounded approximation to rotating blade trajectories,
// not a convex cast. Ordinary passes use at most18 rays; one new binding may
// request at most72 under the same shared1000us ceiling. Every admitted segment
// needs all of its feature queries. The native rays have zero width: the margin
// only stops earlier ALONG a ray that actually hits. It neither fills the space
// between shaft features nor covers lateral arc/chord error. Sparse-ray misses
// remain possible; FULL_CLEAR means this bounded proxy was clear, not full mesh.
template<class Ray> Contact103 sweep_weapon112(const Profile& p,Pose from,Pose to,bool acquire,Ray&& ray) {
    const auto unknown=[](){Contact103 c{};c.known=false;c.fraction=0;c.coverage169r2.reason=sweep169r2::Reason::InvalidQuery;return c;};
    if(p.side<0||p.side>1||!finite(from)||!finite(to)||!m::finite(p.tip))return unknown();
    const Vec3 start=p.grip[p.side].p,end=p.tip;
    double radius=acquire?m::length(p.grip[1-p.side].p-start):m::length(end-start),halfThickness=0;
    std::array<Vec3,5> shieldFeatures{};
    const bool shield=p.shield176()&&!acquire;
    if(shield&&!shield_features59c10r2(p,shieldFeatures,radius,halfThickness))return unknown();
    if(!std::isfinite(radius)||radius>3)return unknown();
    const double angle=m::angle(from.q,to.q);
    const double fineStep=radius>.004?2*std::acos(std::clamp(1-.004/radius,-1.0,1.0)):3.141592653589793;
    // D67R1: D67 expanded the current arc by using6 segments but then silently
    // reduced the shaft to3 features. The quarter-shaft counterexample in
    // weapon_sweep_d67r1_test misses a10mm obstacle under that policy. The
    // current radius plan below retains all five shaft locations and can cover
    // the recorded42.8077deg sword arc and80deg native return under18 rays.
    // Initial binding keeps its fine chord and existing72-ray burst.
    // R28 uses the already-existing slow-frame coverage policy for one
    // qualified post-native rejoin. Shared ray/time limits are never renewed.
    // First acquisition has its own fine-chord burst below, separately labelled
    // so it cannot masquerade as an ordinary/post-native return query.
    const double rejoinMinimumR28=(p.nativeRejoinSweepR28||p.initialAcquireSweepD44)&&!acquire&&p.melee()?2.0:1.0;
    // Current same-owner world turns use the same bounded subdivision as
    // initial/native-return acquisition. The actual world arc is queried;
    // neither its destination nor stale root transport becomes clearance.
    const bool rootArcD44=!acquire&&std::isfinite(p.rootTurnAngleD44)&&
        p.rootTurnAngleD44>3*fineStep&&p.rootTurnAngleD44<=3.141592653589793;
    const double minimumCoverageD45=std::max(rejoinMinimumR28,rootArcD44?2.0:1.0);
    const double coverage=std::clamp(g_sweep_coverage_scale178r6.load(std::memory_order_relaxed),minimumCoverageD45,4.0);
    const bool initialD45=p.initialAcquireSweepD44&&!acquire&&!shield;
    const double coarseChord=initialD45?.004:std::clamp(.004*coverage,.004,.012);
    if(!acquire&&p.crossbowD72()){
        std::array<Vec3,9> features{};if(!crossbow_features_d72(p,features))return unknown();
        return sweep_feature_radius_d72(p,from,to,initialD45,coarseChord,features,ray);
    }
    if(!acquire&&!shield&&kcd2vr::held_motion_d67::currentArcCoverage())
        return sweep_shaft_radius_d67r1(p,from,to,initialD45,coarseChord,ray);
    // Exact legacy comparison when current-arc coverage is disabled. Shields
    // and the offhand's one exact grip feature keep their existing policies.
    const double step=radius>coarseChord?2*std::acos(std::clamp(1-coarseChord/radius,-1.0,1.0)):3.141592653589793;
    const int maxSegments=initialD45?24:(!shield&&(coverage>1.5||kcd2vr::held_motion_d67::currentArcCoverage())&&angle>3*fineStep)?6:3;
    const double prefix=angle>maxSegments*step?(maxSegments*step)/angle:1.0;
    const Pose goal=blend_weapon116(p,from,to,prefix);
    const int segments=std::min(maxSegments,std::max(1,int(std::ceil(angle*prefix/step))));
    const double margin=.009+halfThickness+radius*(1-std::cos(angle*prefix/(2*segments)));
    const int probes=acquire?1:(segments>3?3:5);
    sweep169r2::Evidence evidence{};evidence.planned=unsigned(segments);evidence.requestedAngle=angle;
    for(int segment=0;segment<segments;++segment){
        const Pose a=blend_weapon116(p,from,goal,double(segment)/segments),b=blend_weapon116(p,from,goal,double(segment+1)/segments);
        Contact103 contact{};
        for(int i=0;i<probes;++i){
            const Vec3 feature=shield?shieldFeatures[i]:acquire?p.grip[1-p.side].p:start+(end-start)*(double(i)/(probes>1?probes-1:1));
            ++evidence.attemptedRays;
            auto hit=contact103(ray(point(a,feature),point(b,feature),margin));
            if(!hit.known){
                // Keep only COMPLETE earlier segments. An unchecked feature in
                // this segment cannot authorize any of this segment's motion.
                evidence.reason=hit.coverage169r2.reason;
                if(evidence.reason==sweep169r2::Reason::FullClear)evidence.reason=sweep169r2::Reason::InvalidQuery;
                evidence.completed=unsigned(segment);evidence.checkedAngle=angle*prefix*segment/segments;
                if(segment==0){auto result=unknown();result.coverage169r2=evidence;return result;}
                Contact103 prefixOnly{};prefixOnly.fraction=prefix*segment/segments;
                prefixOnly.limited115=true;prefixOnly.coverage169r2=evidence;return prefixOnly;
            }
            ++evidence.completedRays;
            if(hit.fraction<contact.fraction){contact=hit;contact.feature=feature;}
            // Zero is the earliest possible hit in this segment. More feature
            // rays cannot admit any additional motion; preserve this proven stop.
            if(contact.fraction==0)break;
        }
        if(contact.fraction<1){contact.fraction=prefix*(segment+contact.fraction)/segments;
            evidence.reason=sweep169r2::Reason::SurfaceHit;evidence.completed=unsigned(segment);evidence.checkedAngle=angle*contact.fraction;
            contact.coverage169r2=evidence;return contact;}
        evidence.completed=unsigned(segment+1);
    }
    Contact103 clear{};clear.fraction=prefix;clear.limited115=prefix<1;
    evidence.reason=prefix<1?sweep169r2::Reason::AngularPrefix:sweep169r2::Reason::FullClear;
    evidence.checkedAngle=angle*prefix;clear.coverage169r2=evidence;return clear;
}
}
