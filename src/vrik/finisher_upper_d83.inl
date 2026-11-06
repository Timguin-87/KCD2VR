// D83R1: bounded head/neck/chest rotation over a qualified native paired action.
// The existing Henry-only skinning callback owns the pose mutex and excludes
// recursion. No locomotion, camera, weapon or input owner is changed here.
namespace fu83=kcd2vr::finisher_upper_d83;

struct FinisherLatchD83 {
    void* character=nullptr;
    std::uint64_t epoch=0,tracking=0,policy=0,tick=0;
    Q4 head{},hmd{},frame{};
    bool valid=false;
};
struct FinisherRigD83 {
    void* owner=nullptr;std::uint32_t count=0;std::uint64_t epoch=0;RigMap rig{};bool ok=false;
    std::array<int,kPoseCapacity6689> parent{},order{};
    std::array<bool,kPoseCapacity6689> affected{};
    int chain[4]{-1,-1,-1,-1};
    float share[4]{fu83::kShareSpine2,fu83::kShareSpine3,fu83::kShareNeck,fu83::kShareHead};
};
struct FinisherStateD83 {
    unsigned side=0;float weight=0;std::uint64_t lastTick=0;Q4 lastR{};
    void* character=nullptr;std::uint64_t epoch=0,tracking=0,policy=0;
    char alias[192]{};bool announced=false;std::uint64_t writes=0;
};
// Serialized by the existing coherent-pose mutex and callback recursion guard.
static FinisherLatchD83 g_finisher_latch_d83;
static FinisherRigD83 g_finisher_rig_d83;
static FinisherStateD83 g_finisher_state_d83;
static std::array<QuatT28,kPoseCapacity6689> g_finisher_cur_d83{},g_finisher_local_d83{},g_finisher_model_d83{};
static std::atomic<bool> g_finisher_fault_d83{false};

struct FinisherWriteD83;
static void FinisherRestoreD83(FinisherWriteD83&) noexcept;
struct FinisherWriteD83 {
    void* pose=nullptr;PoseSetFn setter=nullptr;std::uint32_t count=0;
    restoreR96::Owner restoreOwnerR96{};
    std::array<bool,kPoseCapacity6689> attempted{};bool wrote=false;
    FinisherWriteD83()=default;
    FinisherWriteD83(const FinisherWriteD83&)=delete;
    FinisherWriteD83& operator=(const FinisherWriteD83&)=delete;
    ~FinisherWriteD83(){FinisherRestoreD83(*this);}
};
static fu83::Q FuQ83(Q4 q) noexcept {return {q.x,q.y,q.z,q.w};}
static Q4 QFu83(fu83::Q q) noexcept {return {q.x,q.y,q.z,q.w};}

static bool FinisherContextD83(unsigned side,std::uint64_t epoch,
                               std::uint64_t tracking,std::uint64_t policy) noexcept {
    return fu83::Enabled(fu83::Side(side))&&epoch&&tracking&&!(policy&1)&&
        epoch==g_pose_epoch6689.load(std::memory_order_acquire)&&
        tracking==kcd2vr::tracking59c6::epoch.load(std::memory_order_acquire)&&
        policy==g_pose_policy_generation57.load(std::memory_order_acquire)&&
        g_cal_state.load(std::memory_order_acquire)==2&&
        !g_pose_restore_fault6689.load(std::memory_order_acquire)&&
        !g_finisher_fault_d83.load(std::memory_order_acquire)&&
        g_gameplay_allowed.load(std::memory_order_acquire)&&
        !g_ctx_menu.load(std::memory_order_acquire)&&!g_ctx_dialogue.load(std::memory_order_acquire)&&
        !g_ctx_cutscene.load(std::memory_order_acquire)&&!g_ctx_reading.load(std::memory_order_acquire)&&
        !g_ctx_scripted.load(std::memory_order_acquire)&&
        g_census_mode.load(std::memory_order_acquire)>0&&g_weapon_control_mode.load(std::memory_order_acquire)!=2&&
        !diagnostics::lab::NativeReference();
}

// The caller supplies the transaction's ORIGINAL generations after successful
// normal pose publication. Reading new globals and stamping old work with them
// would incorrectly bridge a recenter or policy change during native skinning.
static void FinisherLatchCommittedD83(void* character,const TrackingSnapshot& ts,
                                      std::uint64_t epoch,std::uint64_t policy) noexcept {
    const auto& s=g_pose6689;const auto now=std::uint64_t(GetTickCount64());
    if(!character||s.character!=character||s.rig.head<0||std::uint32_t(s.rig.head)>=s.count||
       !ValidPose6689(s.work[s.rig.head])||!g_committed_pose_valid6689.load(std::memory_order_acquire)||
       !epoch||epoch!=g_pose_epoch6689.load(std::memory_order_acquire)||(policy&1)||
       policy!=g_pose_policy_generation57.load(std::memory_order_acquire)||g_pose_restore_fault6689.load()||
       !ts.allowed||!ts.head_valid||ts.head_held||!fu83::Valid(FuQ83(ts.hq))||
       !ts.source_epoch||ts.source_epoch!=kcd2vr::tracking59c6::epoch.load(std::memory_order_acquire)||
       !ts.tick||now<ts.tick||now-ts.tick>100)return;
    const Q4 frame{g_cal_frame_qx.load(),g_cal_frame_qy.load(),g_cal_frame_qz.load(),g_cal_frame_qw.load()};
    if(!fu83::Valid(FuQ83(frame))||epoch!=g_pose_epoch6689.load(std::memory_order_acquire)||
       policy!=g_pose_policy_generation57.load(std::memory_order_acquire))return;
    auto& l=g_finisher_latch_d83;
    l.character=character;l.epoch=epoch;l.tracking=ts.source_epoch;l.policy=policy;l.tick=ts.tick;
    l.head=QFromQuatT(s.work[s.rig.head]);l.hmd=QNormalize(ts.hq);l.frame=QNormalize(frame);l.valid=true;
}
static void FinisherLogD83(const char* what,unsigned side,const char* alias) noexcept {
    char m[400]{};std::snprintf(m,sizeof(m),"D83R1 FINISHER %s side=%s clip=%s",what,
        side==fu83::Player?"player":side==fu83::Npc?"npc":"-",alias&&*alias?alias:"-");
    diagnostic_trace::Write(m);
}
static bool FinisherRigD83Ready(void* owner,std::uint32_t count,const RigMap& rig) noexcept {
    auto& r=g_finisher_rig_d83;const auto epoch=g_pose_epoch6689.load(std::memory_order_acquire);
    const bool sameRig=r.rig.spine2==rig.spine2&&r.rig.spine3==rig.spine3&&r.rig.neck==rig.neck&&r.rig.head==rig.head&&
        r.rig.clav_l==rig.clav_l&&r.rig.clav_r==rig.clav_r&&r.rig.upper_l==rig.upper_l&&r.rig.upper_r==rig.upper_r&&
        r.rig.hand_l==rig.hand_l&&r.rig.hand_r==rig.hand_r&&r.rig.item_l==rig.item_l&&r.rig.item_r==rig.item_r;
    if(r.owner==owner&&r.count==count&&r.epoch==epoch&&sameRig)return r.ok;
    r=FinisherRigD83{};r.owner=owner;r.count=count;r.epoch=epoch;r.rig=rig;
    if(count<8||count>kPoseCapacity6689)return false;
    for(std::uint32_t j=0;j<count;++j)if(!ReadParentDirect(owner,int(j),r.parent[j])||
        r.parent[j]<-1||r.parent[j]>=int(count)||r.parent[j]==int(j))return false;
    std::array<unsigned short,kPoseCapacity6689> depth{};
    for(std::uint32_t j=0;j<count;++j){int p=r.parent[j];unsigned short n=0;
        while(p>=0&&n<count){p=r.parent[p];++n;}if(n>=count)return false;depth[j]=n;r.order[j]=int(j);}
    std::sort(r.order.begin(),r.order.begin()+count,[&](int x,int y){return depth[x]<depth[y]||(depth[x]==depth[y]&&x<y);});
    // Torso sockets (back weapons, quiver, sheath, etc.) are siblings of the
    // anatomical chain, not necessarily children of an arm. Keep them native.
    // Only the connecting spine-to-head path and neck/head descendants follow.
    std::array<bool,kPoseCapacity6689> protectedBranch{},neckBranch{};
    for(std::uint32_t k=0;k<count;++k){const int j=r.order[k],p=r.parent[j];
        protectedBranch[j]=j==rig.clav_l||j==rig.clav_r||j==rig.upper_l||j==rig.upper_r||
            j==rig.hand_l||j==rig.hand_r||j==rig.item_l||j==rig.item_r||(p>=0&&protectedBranch[p]);
        neckBranch[j]=j==rig.neck||(p>=0&&neckBranch[p]);
        r.affected[j]=!protectedBranch[j]&&neckBranch[j];}
    r.chain[0]=rig.spine2;r.chain[1]=rig.spine3;r.chain[2]=rig.neck;r.chain[3]=rig.head;
    for(int c:r.chain)if(c<0||std::uint32_t(c)>=count||protectedBranch[c])return false;
    for(unsigned c=1;c<4;++c){int p=r.parent[r.chain[c]];
        while(p>=0&&p!=r.chain[c-1])p=r.parent[p];if(p!=r.chain[c-1])return false;}
    for(int c:r.chain){int p=c;for(;;){
        if(p<0||protectedBranch[p])return false;
        r.affected[p]=true;if(p==rig.spine2)break;p=r.parent[p];}}
    r.ok=true;return true;
}
static void FinisherIdleD83() noexcept {if(g_finisher_state_d83.side)g_finisher_state_d83=FinisherStateD83{};}

static void FinisherBeginD83(FinisherWriteD83& out,unsigned side,const char* alias,void* pose,void* owner,void* character,
                             const RigMap& rig,bool rigOk,PoseGetFn get,std::uint32_t count,
                             const TrackingSnapshot& ts,bool haveSnapshot,std::uint64_t now,bool vrikOn) noexcept {
    auto& st=g_finisher_state_d83;
    const auto epoch=g_pose_epoch6689.load(std::memory_order_acquire);
    const auto tracking=kcd2vr::tracking59c6::epoch.load(std::memory_order_acquire);
    const auto policy=g_pose_policy_generation57.load(std::memory_order_acquire);
    if(st.side!=side||st.character!=character||st.epoch!=epoch||st.tracking!=tracking||st.policy!=policy||
       std::strcmp(st.alias,alias?alias:"")!=0){
        st=FinisherStateD83{};st.side=side;st.character=character;st.epoch=epoch;st.tracking=tracking;st.policy=policy;
        std::snprintf(st.alias,sizeof(st.alias),"%s",alias?alias:"");
    }
    const auto refuse=[&](const char* why){
        st.weight=0;st.lastTick=0;st.lastR={};
        if(!st.announced){st.announced=true;char m[192]{};
            std::snprintf(m,sizeof(m),"native only (%s)",why);FinisherLogD83(m,side,alias);}
    };
    if(out.wrote)return; // one owner per native skinning call
    if(!vrikOn||!FinisherContextD83(side,epoch,tracking,policy)||(haveSnapshot&&!ts.allowed))
        return refuse("current context, source or setting unavailable");
    if(!rigOk||!get||!pose||!owner||!character)return refuse("skeleton not readable");
    if(!FinisherRigD83Ready(owner,count,rig))return refuse("skeleton chain unsupported");
    const auto& l=g_finisher_latch_d83;
    if(!l.valid||l.character!=character||l.epoch!=epoch||l.tracking!=tracking||l.policy!=policy||
       now<l.tick||now-l.tick>30000)return refuse("no current VR head reference");
    if(haveSnapshot&&ts.source_epoch!=tracking)return refuse("tracking source changed");
    if(haveSnapshot&&ts.head_valid&&!ts.head_held&&!fu83::Valid(FuQ83(ts.hq)))return refuse("invalid head quaternion");
    const bool fresh=haveSnapshot&&ts.head_valid&&!ts.head_held&&ts.tick&&now>=ts.tick&&now-ts.tick<=100;
    auto& cur=g_finisher_cur_d83;
    if(!ReadNativeBuffer6689(pose,get,count,cur.data()))return refuse("native pose not readable");
    for(std::uint32_t j=0;j<count;++j)if(!ValidPose6689(cur[j]))return refuse("native pose invalid");
    const float dt=st.lastTick&&now>=st.lastTick?float(std::min<std::uint64_t>(now-st.lastTick,100)):0.f;st.lastTick=now;
    fu83::Q rotation=FuQ83(st.lastR);
    if(fresh){
        // Same XR->Cry and calibration-frame conjugation as ordinary VRIK.
        // The reference frame belongs to the qualified published pose above.
        const Q4 delta=QNormalize(QMul(QMul(l.frame,QMul(XrToCryQ(ts.hq),QConj(XrToCryQ(l.hmd)))),QConj(l.frame)));
        const Q4 target=QNormalize(QMul(delta,l.head));
        rotation=fu83::Clamp(FuQ83(QNormalize(QMul(target,QConj(QFromQuatT(cur[rig.head]))))));
        st.lastR=QFu83(rotation);st.weight=std::min(1.f,st.weight+dt/fu83::kBlendMs);
    }else st.weight=std::max(0.f,st.weight-dt/fu83::kBlendMs);
    if(!st.announced){st.announced=true;FinisherLogD83(fresh?"native action + tracked head turns":"native action (waiting for head tracking)",side,alias);}
    if(st.weight<=.001f)return;
    const fu83::Q weighted=fu83::Scale(rotation,st.weight);
    auto& r=g_finisher_rig_d83;auto& local=g_finisher_local_d83;auto& model=g_finisher_model_d83;
    for(std::uint32_t k=0;k<count;++k){const int j=r.order[k],p=r.parent[j];
        model[j]=cur[j];if(!r.affected[j])continue;
        local[j]=p>=0?RelativeFromAbsolute(cur[p],cur[j]):cur[j];
        model[j]=p>=0?AbsoluteFromRelative(model[p],local[j]):local[j];
        for(int c=0;c<4;++c)if(j==r.chain[c]){
            QToQuatT(model[j],QMul(QFu83(fu83::Scale(weighted,r.share[c])),QFromQuatT(cur[j])));break;}
        if(!ValidPose6689(model[j]))return refuse("overlay pose invalid");
    }
    PoseSetFn setter=nullptr;if(!ValidateBuffer1Setter(pose,setter))return refuse("pose setter unavailable");
    // Native reads/setter qualification can race a control-source reset. Check
    // the ORIGINAL context again, then rollback if it changes while writing.
    if(!FinisherContextD83(side,epoch,tracking,policy))return refuse("context changed before pose write");
    out.pose=pose;out.setter=setter;out.count=count;out.wrote=true;
    out.restoreOwnerR96=PoseRestoreOwnerR96(owner,character,pose,count);
    bool ok=true;
    for(std::uint32_t j=0;j<count&&ok;++j)if(r.affected[j]){
        out.attempted[j]=true; // a failed native call may already have touched memory
        ok=WriteQuat(setter,pose,int(j),model[j]);
    }
    const bool current=FinisherContextD83(side,epoch,tracking,policy);
    if(!ok||!current){
        FinisherRestoreD83(out);
        if(!ok)g_finisher_fault_d83.store(true,std::memory_order_release);
        refuse(ok?"context changed during pose write":"pose write failed; overlay disabled for session");
        return;
    }
    ++st.writes;
}
static void FinisherRestoreD83(FinisherWriteD83& out) noexcept {
    if(!out.wrote||!out.setter||!out.pose)return;
    bool ok=true;std::array<bool,kPoseCapacity6689> failed{};
    for(std::size_t j=out.count;j-->0;)if(out.attempted[j]){
        const bool restored=WriteQuat(out.setter,out.pose,int(j),g_finisher_cur_d83[j]);
        failed[j]=!restored;ok=restored&&ok;
    }
    out.wrote=false;
    if(!ok){
        g_finisher_fault_d83.store(true,std::memory_order_release);
        RecordPoseRestoreDebtR96(out.restoreOwnerR96,g_finisher_cur_d83.data(),failed);
        diagnostic_trace::Write("D83R1 FINISHER restore failed; shared pose production disabled");
    }
}
