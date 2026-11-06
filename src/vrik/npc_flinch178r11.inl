// V66.178R11 EXPERIMENTAL NPC FLINCH (F4 rung 3, INI VrNpcFlinch).
// Included in frik_pose_probe's private namespace after bow_string128.inl.
//
// Visual only. When a qualified strike (V66.178 STRIKE accepted) is stopped by a living /
// articulated body with a native entity id, the hit character's torso is bent away from the
// blade for 350 ms through the SAME skinning boundary and pose API the bow string uses
// (validated CCharInstance, joint count/parents, b1 model buffer, buffer-1 setter, restore on
// fault, disabled for the session after a fault). No damage, stamina, physics, ragdoll or
// native reaction is written; the game's own hit reaction (if the hit was admitted) plays
// underneath and wins because our offset decays. Never applied to Henry.
constexpr std::uint32_t kFlinchJoints178r11=320;
constexpr std::uint64_t kFlinchMs178r11=350,kFlinchAttackMs178r11=60;
struct FlinchActive178r11 {
    void* character=nullptr;void* anim=nullptr;std::uint64_t start=0,serial=0;
    V3 axisModel{};float amplitude=0;std::uint32_t entityId=0;
};
static kcd2vr::core::SnapshotMailbox<FlinchActive178r11> g_flinch_active178r11;
static std::atomic<bool> g_flinch_enabled178r11{false};
static std::atomic<bool> g_flinch_fault178r11{false};
static std::atomic<std::uint64_t> g_flinch_serial178r11{0},g_flinch_writes178r11{0};
static std::atomic_flag g_flinch_lock178r11=ATOMIC_FLAG_INIT;
struct FlinchRig178r11 {
    void* character=nullptr;void* owner=nullptr;std::uint32_t count=0;RigMap rig{};bool rigOk=false;
    std::array<int,kFlinchJoints178r11> parent{},order{};std::array<bool,kFlinchJoints178r11> affected{};
    std::array<QuatT28,kFlinchJoints178r11> lastWritten{};bool wroteLast=false;
};
static FlinchRig178r11 g_flinch_rig178r11;

static void* FlinchEntityById178r11(std::uint32_t id) noexcept {
    void* system=nullptr;void* vf=nullptr;void* get=nullptr;void* target=nullptr;
    const auto b=WhBase();
    if(!id||!retail_build112::Ready(b)||!Read(reinterpret_cast<void*>(b+0x492D8A0ull),system)||!system||!Read(system,vf)||
       vf!=reinterpret_cast<void*>(b+0x4077048ull)||!Read(static_cast<unsigned char*>(vf)+0x70,get)||
       get!=reinterpret_cast<void*>(b+0x4B4F98ull))return nullptr;
    __try{target=reinterpret_cast<void*(__fastcall*)(void*,std::uint32_t)>(get)(system,id);}__except(EXCEPTION_EXECUTE_HANDLER){target=nullptr;}
    std::uint32_t actual=0;
    if(!target||!Read(target,vf)||vf!=reinterpret_cast<void*>(b+0x4072C78ull)||
       !Read(static_cast<unsigned char*>(target)+0xC,actual)||actual!=id)return nullptr;
    return target;
}
// Game thread (NativeRhythmTick178): one accepted strike -> one flinch request.
static void FlinchRequest178r11(const PhysFrame6698& f,int side,double scale) noexcept {
    if(!g_flinch_enabled178r11.load(std::memory_order_relaxed)||g_flinch_fault178r11.load()||!CombatFull178r10Enabled())return;
    namespace n=kcd2vr::npc_contact172;const auto& c=f.output.contact119[side];
    if(!c.native172.entityId||!(c.native172.body==n::Body::Living||c.native172.body==n::Body::Articulated))return;
    void* entity=FlinchEntityById178r11(c.native172.entityId);if(!entity)return;
    void* ci=CharacterForEntity6689(entity);void* anim=nullptr;if(!ci||!ValidateCharInstance(ci,anim))return;
    void* player=g_last_player_pose.load(std::memory_order_acquire);void* playerChar=nullptr;void* playerAnim=nullptr;
    if(player&&ReadValidatedCharInstanceFromPlayerPose(player,playerChar,playerAnim)&&playerChar==ci)return; // never Henry
    ModelWorld6692 npcWorld{};if(!ReadModelWorld6692(ci,npcWorld))return;
    // Contact normal: Henry-model space -> world -> NPC-model space. The body bends away from the
    // blade, i.e. about the axis up x (-normal); |normal| ~ 1.
    const V3 nHenry{float(c.normal.x),float(c.normal.y),float(c.normal.z)};
    const auto hw=PhysWorld6698(f.world);const Q4 hq{float(hw.q.x),float(hw.q.y),float(hw.q.z),float(hw.q.w)};
    const V3 nWorld=QRotateV(hq,nHenry);const V3 into=Mul(QRotateV(QConj(npcWorld.q),nWorld),-1.f);
    V3 flat{into.x,into.y,0};if(Len(flat)<.2f)return;flat=Normalize(flat);
    const V3 axis=Normalize(V3{-flat.y,flat.x,0}); // up(0,0,1) x flat
    FlinchActive178r11 a{};a.character=ci;a.anim=anim;a.start=std::uint64_t(GetTickCount64());a.serial=++g_flinch_serial178r11;
    a.axisModel=axis;a.amplitude=float(std::clamp(.08+.22*std::clamp(scale,0.0,1.25),.08,.32));a.entityId=c.native172.entityId;
    g_flinch_active178r11.publish(a);
    char m[256]{};std::snprintf(m,sizeof(m),"V66.178R11 NPC_FLINCH REQUEST serial=%llu entity=%u char=%p amplitudeRad=%.3f axis=(%.2f,%.2f,%.2f) durationMs=%llu damage=NONE physics=NONE",
        (unsigned long long)a.serial,a.entityId,ci,double(a.amplitude),axis.x,axis.y,axis.z,(unsigned long long)kFlinchMs178r11);
    diagnostic_trace::Write(m);
}
static bool FlinchCandidate178r11(void* charPlus8,FlinchActive178r11& a) noexcept {
    if(!g_flinch_enabled178r11.load(std::memory_order_relaxed)||g_flinch_fault178r11.load()||!charPlus8)return false;
    if(!g_flinch_active178r11.read(a)||!a.character)return false;
    if(reinterpret_cast<std::uintptr_t>(charPlus8)!=reinterpret_cast<std::uintptr_t>(a.character)+8u)return false;
    const auto now=std::uint64_t(GetTickCount64());return now>=a.start&&now-a.start<=kFlinchMs178r11;
}
static Q4 FlinchAxisAngle178r11(V3 axis,float angle) noexcept {
    const float s=std::sin(angle*.5f);return QNormalize(Q4{axis.x*s,axis.y*s,axis.z*s,std::cos(angle*.5f)});
}
// Skinning thread (SkinningTransformCoreDetour), same boundary as the bow string.
static void FlinchSkinning178r11(void* pose,void* owner,void* charPlus8,const FlinchActive178r11& a) noexcept {
    if(g_flinch_lock178r11.test_and_set(std::memory_order_acquire))return;
    struct Unlock{~Unlock(){g_flinch_lock178r11.clear(std::memory_order_release);}} unlock;
    void* ci=nullptr;void* anim=nullptr;void* skel=nullptr;
    if(!ReadValidatedCharInstanceFromSkinning(charPlus8,ci,anim)||ci!=a.character||anim!=a.anim||
       !ReadValidatedDefaultSkeletonFromPose(pose,skel)||skel!=owner)return;
    auto& r=g_flinch_rig178r11;
    if(r.character!=ci||r.owner!=owner){
        r=FlinchRig178r11{};r.character=ci;r.owner=owner;std::uint32_t count=0;
        if(!ReadJointCount(owner,count)||count<8||count>kFlinchJoints178r11){char m[160]{};
            std::snprintf(m,sizeof(m),"V66.178R11 NPC_FLINCH RIG UNSUPPORTED joints=%u (max %u)",count,kFlinchJoints178r11);diagnostic_trace::Write(m);return;}
        for(std::uint32_t j=0;j<count;++j)if(!ReadParentDirect(owner,int(j),r.parent[j])||r.parent[j]>=int(count)||r.parent[j]==int(j))return;
        r.count=count;r.rigOk=ResolveRig(owner,count,r.rig);
        std::array<unsigned short,kFlinchJoints178r11> depth{};
        for(std::uint32_t j=0;j<count;++j){int p=r.parent[j];unsigned short n=0;while(p>=0&&n<count){p=r.parent[p];++n;}depth[j]=n;r.order[j]=int(j);}
        std::sort(r.order.begin(),r.order.begin()+count,[&](int x,int y){return depth[x]<depth[y]||(depth[x]==depth[y]&&x<y);});
        if(r.rigOk){for(std::uint32_t k=0;k<count;++k){const int j=r.order[k],p=r.parent[j];r.affected[j]=j==r.rig.spine1||(p>=0&&r.affected[p]);}}
        char m[224]{};std::snprintf(m,sizeof(m),"V66.178R11 NPC_FLINCH RIG char=%p skeleton=%p joints=%u rig=%s spine1=%d spine3=%d neck=%d",ci,owner,count,r.rigOk?"OK":"UNRESOLVED",r.rig.spine1,r.rig.spine3,r.rig.neck);diagnostic_trace::Write(m);
    }
    if(!r.count||!r.rigOk)return;
    const auto now=std::uint64_t(GetTickCount64());if(now<a.start)return;const auto t=now-a.start;if(t>kFlinchMs178r11)return;
    const float env=t<kFlinchAttackMs178r11?float(t)/float(kFlinchAttackMs178r11):1.f-float(t-kFlinchAttackMs178r11)/float(kFlinchMs178r11-kFlinchAttackMs178r11);
    const float amp=a.amplitude*std::clamp(env,0.f,1.f);if(amp<.002f)return;
    PoseGetFn b0=nullptr,b1=nullptr;static std::array<QuatT28,kFlinchJoints178r11> cur{},local{},model{};
    if(!ValidateRawPoseApi(pose,b0,b1)||!ReadNativeBuffer6689(pose,b1,r.count,cur.data()))return;
    for(std::uint32_t j=0;j<r.count;++j)if(!ValidPose6689(cur[j]))return;
    // Our previous output must not be re-bent: only a fresh native buffer is modified.
    if(r.wroteLast&&std::memcmp(cur.data(),r.lastWritten.data(),r.count*sizeof(QuatT28))==0)return;
    for(std::uint32_t j=0;j<r.count;++j){const int p=r.parent[j];local[j]=p>=0?RelativeFromAbsolute(cur[p],cur[j]):cur[j];}
    // Distribute the bend over spine1..spine3 and the neck (the head lags a little).
    const int joints[4]{r.rig.spine1,r.rig.spine2,r.rig.spine3,r.rig.neck};const float share[4]{.3f,.3f,.3f,.25f};
    for(int i=0;i<4;++i){const int j=joints[i];if(j<0||j>=int(r.count))continue;const int p=r.parent[j];
        const Q4 pq=p>=0?QFromQuatT(cur[p]):Q4{};
        const Q4 offModel=FlinchAxisAngle178r11(a.axisModel,amp*share[i]);
        const Q4 offParent=QNormalize(QMul(QMul(QConj(pq),offModel),pq));
        QToQuatT(local[j],QNormalize(QMul(offParent,QFromQuatT(local[j]))));
    }
    for(std::uint32_t k=0;k<r.count;++k){const int j=r.order[k],p=r.parent[j];model[j]=r.affected[j]?(p>=0?AbsoluteFromRelative(model[p],local[j]):local[j]):cur[j];if(!ValidPose6689(model[j]))return;}
    PoseSetFn setter=nullptr;if(!ValidateBuffer1Setter(pose,setter))return;
    bool ok=true;for(std::uint32_t j=0;j<r.count&&ok;++j)if(r.affected[j])ok=WriteQuat(setter,pose,int(j),model[j]);
    if(!ok){for(std::uint32_t j=0;j<r.count;++j)if(r.affected[j])(void)WriteQuat(setter,pose,int(j),cur[j]);
        g_flinch_fault178r11.store(true);diagnostic_trace::Write("V66.178R11 NPC_FLINCH FAULT write failed; native pose restored; flinch disabled for this session");return;}
    for(std::uint32_t j=0;j<r.count;++j)r.lastWritten[j]=r.affected[j]?model[j]:cur[j];
    r.wroteLast=true;
    if(g_flinch_writes178r11.fetch_add(1)%32==0){char m[192]{};
        std::snprintf(m,sizeof(m),"V66.178R11 NPC_FLINCH WRITE serial=%llu tMs=%llu ampRad=%.3f writes=%llu",(unsigned long long)a.serial,(unsigned long long)t,double(amp),(unsigned long long)g_flinch_writes178r11.load());
        diagnostic_trace::Write(m);}
}
