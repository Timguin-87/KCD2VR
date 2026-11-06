// Generated from current frik_pose_probe.cpp; do not hand-edit.
struct RigMap {
    int pelvis{-1}, spine{-1}, spine1{-1}, spine2{-1}, spine3{-1}, neck{-1}, head{-1};
    int clav_l{-1}, upper_l{-1}, fore_l{-1}, elbow_l{-1}, hand_l{-1}, item_l{-1};
    int clav_r{-1}, upper_r{-1}, fore_r{-1}, elbow_r{-1}, hand_r{-1}, item_r{-1};
};
struct V3 { float x{},y{},z{}; };
struct Q4 { float x{},y{},z{},w{1.0f}; };
struct TrackingSnapshot {
    std::uint64_t sequence=0,tick=0,timeNs=0;
    std::int64_t poseTime=0; // XR locate target; never used as input/freshness time
    std::uint64_t input_tick=0,source_epoch=0;
    std::uint64_t head_good_tick=0,left_good_tick=0,right_good_tick=0;
    V3 head{}, left{}, right{};
    Q4 hq{}, lq{}, rq{};
    float left_grip=0.0f,right_grip=0.0f;
    float left_trigger=0.0f,right_trigger=0.0f;
    kcd2vr::dialogue_fingers_d78::VisualInput dialogueFingersD78{}; // presentation only
    bool head_valid=false,left_valid=false,right_valid=false,allowed=false;
    bool head_held=false,left_held=false,right_held=false; // V66.86 VRIK-only short dropout hold
    std::int64_t handTargetD69[2]{};unsigned handLeadSourceD69[2]{};std::uint64_t handPolicyD69=0;

};

static V3 Add(V3 a,V3 b) noexcept { return {a.x+b.x,a.y+b.y,a.z+b.z}; }
static V3 Sub(V3 a,V3 b) noexcept { return {a.x-b.x,a.y-b.y,a.z-b.z}; }
static V3 Mul(V3 a,float s) noexcept { return {a.x*s,a.y*s,a.z*s}; }
static float Dot(V3 a,V3 b) noexcept { return a.x*b.x+a.y*b.y+a.z*b.z; }
static V3 Cross(V3 a,V3 b) noexcept { return {a.y*b.z-a.z*b.y,a.z*b.x-a.x*b.z,a.x*b.y-a.y*b.x}; }
static float Len(V3 a) noexcept { return std::sqrt(std::max(0.0f,Dot(a,a))); }
static V3 Normalize(V3 a) noexcept { const float l=Len(a); return l>1.0e-6f?Mul(a,1.0f/l):V3{}; }
// Retired unused implementation archived: research_archive/retired_pose_implementations

static V3 FromT(const QuatT28& q) noexcept { return {q.t[0],q.t[1],q.t[2]}; }
static void ToT(QuatT28& q,V3 v) noexcept { q.t[0]=v.x;q.t[1]=v.y;q.t[2]=v.z; }
static Q4 QConj(Q4 q) noexcept { return {-q.x,-q.y,-q.z,q.w}; }
static Q4 QMul(Q4 a,Q4 b) noexcept {
    return {a.w*b.x+a.x*b.w+a.y*b.z-a.z*b.y,
            a.w*b.y-a.x*b.z+a.y*b.w+a.z*b.x,
            a.w*b.z+a.x*b.y-a.y*b.x+a.z*b.w,
            a.w*b.w-a.x*b.x-a.y*b.y-a.z*b.z};
}
static Q4 QNormalize(Q4 q) noexcept { const float n=std::sqrt(std::max(0.0f,q.x*q.x+q.y*q.y+q.z*q.z+q.w*q.w)); return n>1.0e-6f?Q4{q.x/n,q.y/n,q.z/n,q.w/n}:Q4{}; }
static Q4 QNlerpIdentity(Q4 q,float t) noexcept { q=QNormalize(q); if(q.w<0.0f) q={-q.x,-q.y,-q.z,-q.w}; return QNormalize(Q4{q.x*t,q.y*t,q.z*t,1.0f+(q.w-1.0f)*t}); }
static Q4 QFromQuatT(const QuatT28& q) noexcept { return QNormalize(Q4{q.q[0],q.q[1],q.q[2],q.q[3]}); }
static void QToQuatT(QuatT28& q,Q4 v) noexcept { v=QNormalize(v); q.q[0]=v.x;q.q[1]=v.y;q.q[2]=v.z;q.q[3]=v.w; }
static Q4 XrToCryQ(Q4 q) noexcept { const Q4 basis{0.70710678118f,0,0,0.70710678118f}; return QNormalize(QMul(QMul(basis,QNormalize(q)),QConj(basis))); }
static V3 XrToCryV(V3 v) noexcept { return {v.x,-v.z,v.y}; }
static Q4 QFromBasis(V3 x,V3 y,V3 z) noexcept {
    // Columns are the local +X/+Y/+Z axes expressed in model space.
    x=Normalize(x); y=Normalize(y); z=Normalize(z);
    const float m00=x.x,m01=y.x,m02=z.x;
    const float m10=x.y,m11=y.y,m12=z.y;
    const float m20=x.z,m21=y.z,m22=z.z;
    Q4 q{}; const float tr=m00+m11+m22;
    if(tr>0.0f){const float s=std::sqrt(tr+1.0f)*2.0f;q.w=0.25f*s;q.x=(m21-m12)/s;q.y=(m02-m20)/s;q.z=(m10-m01)/s;}
    else if(m00>m11&&m00>m22){const float s=std::sqrt(std::max(0.0f,1.0f+m00-m11-m22))*2.0f;q.w=(m21-m12)/s;q.x=0.25f*s;q.y=(m01+m10)/s;q.z=(m02+m20)/s;}
    else if(m11>m22){const float s=std::sqrt(std::max(0.0f,1.0f+m11-m00-m22))*2.0f;q.w=(m02-m20)/s;q.x=(m01+m10)/s;q.y=0.25f*s;q.z=(m12+m21)/s;}
    else {const float s=std::sqrt(std::max(0.0f,1.0f+m22-m00-m11))*2.0f;q.w=(m10-m01)/s;q.x=(m02+m20)/s;q.y=(m12+m21)/s;q.z=0.25f*s;}
    return QNormalize(q);
}
static V3 QRotateV(Q4 q,V3 v) noexcept {
    q=QNormalize(q);
    const V3 u{q.x,q.y,q.z};
    const float ss=q.w;
    return Add(Add(Mul(u,2.0f*Dot(u,v)),Mul(v,ss*ss-Dot(u,u))),Mul(Cross(u,v),2.0f*ss));
}
// Retired unused implementation archived: research_archive/retired_pose_implementations

static QuatT28 ApplyRigidHandDelta(const QuatT28& handNative,const QuatT28& handGoal,const QuatT28& targetNative) noexcept {
    const Q4 delta=QNormalize(QMul(QFromQuatT(handGoal),QConj(QFromQuatT(handNative))));
    QuatT28 out=targetNative; QToQuatT(out,QMul(delta,QFromQuatT(targetNative)));
    const V3 rel=Sub(FromT(targetNative),FromT(handNative)); ToT(out,Add(FromT(handGoal),QRotateV(delta,rel))); return out;
}
static QuatT28 RelativeFromAbsolute(const QuatT28& parent,const QuatT28& child) noexcept {
    const Q4 ip=QConj(QFromQuatT(parent));
    QuatT28 out=child;
    QToQuatT(out,QMul(ip,QFromQuatT(child)));
    ToT(out,QRotateV(ip,Sub(FromT(child),FromT(parent))));
    return out;
}
static QuatT28 AbsoluteFromRelative(const QuatT28& parent,const QuatT28& rel) noexcept {
    QuatT28 out=rel; const Q4 pq=QFromQuatT(parent);
    QToQuatT(out,QMul(pq,QFromQuatT(rel)));
    ToT(out,Add(FromT(parent),QRotateV(pq,FromT(rel))));
    return out;
}
