// Actual xr_manager.cpp math; regenerate, do not hand edit.
struct V3f { float x,y,z; };
static XrQuaternionf QConj(const XrQuaternionf& q){ return {-q.x,-q.y,-q.z,q.w}; }
static XrQuaternionf QMul(const XrQuaternionf& a,const XrQuaternionf& b){ return {
    a.w*b.x+a.x*b.w+a.y*b.z-a.z*b.y,
    a.w*b.y-a.x*b.z+a.y*b.w+a.z*b.x,
    a.w*b.z+a.x*b.y-a.y*b.x+a.z*b.w,
    a.w*b.w-a.x*b.x-a.y*b.y-a.z*b.z}; }
static V3f QRotate(const XrQuaternionf& q,const V3f& v){ XrQuaternionf p{v.x,v.y,v.z,0}; auto r=QMul(QMul(q,p),QConj(q)); return {r.x,r.y,r.z}; }
XrQuaternionf QNormalize20(const XrQuaternionf& q){
    const float n=std::sqrt(q.x*q.x+q.y*q.y+q.z*q.z+q.w*q.w);
    return (std::isfinite(n)&&n>1.0e-6f)?XrQuaternionf{q.x/n,q.y/n,q.z/n,q.w/n}:XrQuaternionf{0,0,0,1};}
XrQuaternionf QNlerp20(const XrQuaternionf& a,XrQuaternionf b,float t){
    if(a.x*b.x+a.y*b.y+a.z*b.z+a.w*b.w<0.0f)b={-b.x,-b.y,-b.z,-b.w};
    return QNormalize20({a.x+(b.x-a.x)*t,a.y+(b.y-a.y)*t,a.z+(b.z-a.z)*t,a.w+(b.w-a.w)*t});}
float QAngle20(const XrQuaternionf& a,const XrQuaternionf& b){
    const float d=std::fabs(a.x*b.x+a.y*b.y+a.z*b.z+a.w*b.w);return 2.0f*std::acos(std::min(1.0f,d));}
