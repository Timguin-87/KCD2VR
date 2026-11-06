// V65.56: default controller->hand orientation, independent of F8 pose calibration.
// OpenXR grip convention supplies a fixed controller frame. We derive Henry's
// hand-bone -> anatomical frame once from the four proximal finger-root positions;
// that relationship belongs to the skeleton, not to the player's calibration pose.
struct HandAnatomyCacheV6556 { void* owner=nullptr; bool valid[2]{false,false}; Q4 boneToAnatomy[2]{}; };
static HandAnatomyCacheV6556 g_hand_anatomy_v6556{};
// Retired unused implementation archived: research_archive/retired_pose_implementations

// Retired unused implementation archived: research_archive/retired_pose_implementations


static bool ReadF12HandDeltaV6557(int side,QuatT28& nativeHand,QuatT28& vrHand) noexcept;


// V66.88: symmetric tracked-arm solve with recovered elbow plane + hard reach. The controller target is authoritative while
// reachable; beyond anatomical reach the final hand endpoint hard-stops at the
// two-bone limit with a small anti-lockout reserve (no elastic endpoint extension).
// Retired unused implementation archived: research_archive/retired_pose_implementations


// V65.48: cache Henry arm ownership topology once. V65.47 rebuilt ancestry for all
// ~718 joints on every pose callback (and for both arms), which was catastrophic.
// The cache records only descendant joint IDs + which solved anchor owns them.
struct ArmOwnershipTopologyV6548 {
    void* owner=nullptr; std::uint32_t jointCount=0; int upper=-1,fore=-1,hand=-1,item=-1;
    struct Node { int id=-1; unsigned char anchor=0; bool itemBranch=false; }; // 0=upper,1=fore,2=hand
    std::vector<Node> nodes;
};
static ArmOwnershipTopologyV6548 g_arm_topology_v6548[2];
static std::mutex g_arm_topology_mutex_v6548;

// Retired unused implementation archived: research_archive/retired_pose_implementations


// Retired unused implementation archived: research_archive/retired_pose_implementations


// Retired unused implementation archived: research_archive/retired_pose_implementations


SubtreeEntry* FindSubtreeEntry(SubtreeBatch& b,int id) noexcept;

// Retired unused implementation archived: research_archive/retired_pose_implementations


// Retired unused implementation archived: research_archive/retired_pose_implementations


// Retired unused implementation archived: research_archive/retired_pose_implementations



struct FingerChainV6528 { std::array<int,6> id{}; int count{}; };

// Retired unused implementation archived: research_archive/retired_pose_implementations


// Retired unused implementation archived: research_archive/retired_pose_implementations


// Retired unused implementation archived: research_archive/retired_pose_implementations



// W1: chains found by finger NAME crc + parent links on whatever joint numbering this Henry skeleton has
// (25 Oct: a 163/198 rig got no fingers and a raw controller wrist). the old fixed-id path stays as the second try.
static bool DiscoverFingerChainsByNameW1(void* owner,std::uint32_t count,int hand,int item,bool left,std::array<FingerChainV6528,5>& chains) noexcept {
    namespace fr=kcd2vr::finger_rig_w1;
    if(!owner||hand<0||count==0||count>4096)return false;
    void* table=nullptr;if(!Read(static_cast<unsigned char*>(owner)+8,table)||!table)return false;
    std::vector<std::uint32_t> crc(count,0u);std::vector<int> parent(count,-2);
    for(std::uint32_t j=0;j<count;++j){
        std::uint32_t c=0;if(Read(static_cast<unsigned char*>(table)+std::size_t(j)*0x30+8,c))crc[j]=c;
        int p=-2;if(ReadParentDirect(owner,int(j),p))parent[j]=p;
    }
    std::array<fr::Chain,5> found{};
    auto crcOf=[&](int j)->std::uint32_t{return j>=0&&std::uint32_t(j)<count?crc[std::size_t(j)]:0u;};
    auto parentOf=[&](int j)->int{return j>=0&&std::uint32_t(j)<count?parent[std::size_t(j)]:-2;};
    fr::Method method=fr::Crc;
    // R8b (Codex review): no topology-only guess - numeric order does not prove thumb/index identity. name crc or nothing
    // (then the proven fixed ids, else the visible 'no mappable fingers' line).
    (void)item;if(!fr::DiscoverByCrc(int(count),hand,left,crcOf,parentOf,found))return false;
    for(int f=0;f<5;++f){FingerChainV6528 c{};c.count=found[f].count;for(int k=0;k<found[f].count;++k)c.id[k]=found[f].id[k];chains[f]=c;}
    fr::lastMethod[left?1:0].store(method,std::memory_order_relaxed);
    return true;
}
bool DiscoverFingerChainsV6546(void* pose,PoseGetFn getter,void* owner,std::uint32_t count,const RigMap& rig,bool left,std::array<FingerChainV6528,5>& chains) noexcept {
    (void)pose; (void)getter;
    const int hand=left?rig.hand_l:rig.hand_r,item=left?rig.item_l:rig.item_r;if(!owner||hand<0)return false;
    if(DiscoverFingerChainsByNameW1(owner,count,hand,item,left,chains))return true;
    if(item<0)return false;
    // V65.72: use the proven Henry semantic roots directly. These were established
    // before the geometric sorter that visibly swapped ring/pinky chains.
    const int semanticRootsR[5]={416,415,414,413,412};
    const int semanticRootsL[5]={451,450,449,448,447};
    if(rig.hand_r!=408||rig.hand_l!=443)return false; // W1: head id no longer part of the hand check
    kcd2vr::finger_rig_w1::lastMethod[left?1:0].store(kcd2vr::finger_rig_w1::Fixed,std::memory_order_relaxed);
    const int* roots=left?semanticRootsL:semanticRootsR;
    for(int ci=0;ci<5;++ci){
        const int root=roots[ci]; if(root<0 || (std::uint32_t)root>=count) return false;
        int parent=-2; if(!ReadParentDirect(owner,root,parent)||parent!=hand) return false;
        FingerChainV6528 c{}; c.id[c.count++]=root; int cur=root;
        for(int d=0;d<5&&c.count<(int)c.id.size();++d){
            int child=-1,n=0;
            for(std::uint32_t j=0;j<count;++j){int p=-2;if(ReadParentDirect(owner,(int)j,p)&&p==cur&&(int)j!=item){child=(int)j;if(++n>1)break;}}
            if(n==0)break; if(n!=1)return false; c.id[c.count++]=child; cur=child;
        }
        if(c.count<2||c.count>5)return false; chains[ci]=c;
    }
    return true;
}
bool GetCachedFingerChainsV6546(void* pose,PoseGetFn getter,void* owner,std::uint32_t count,const RigMap& rig,bool left,std::array<FingerChainV6528,5>& chains) noexcept {
    struct Cache{void* owner=nullptr;std::uint32_t count=0;int hand=-1;bool valid=false;std::uint64_t lastTry=0;std::array<FingerChainV6528,5> c{};};static Cache cache[2];static std::atomic_flag lock=ATOMIC_FLAG_INIT;const int k=left?1:0;
    const int handW1=left?rig.hand_l:rig.hand_r;
    if(cache[k].valid&&cache[k].owner==owner&&cache[k].count==count&&cache[k].hand==handW1){chains=cache[k].c;return true;}if(lock.test_and_set(std::memory_order_acquire))return false;auto& x=cache[k];
    const auto nowW1=static_cast<std::uint64_t>(GetTickCount64());
    const bool sameW1=x.owner==owner&&x.count==count&&x.hand==handW1;
    // W1: a failed rig is retried at most every 2 s (the name scan reads every joint record once).
    if(!(x.valid&&sameW1)&&!(sameW1&&!x.valid&&nowW1-x.lastTry<2000)){std::array<FingerChainV6528,5> fresh{};x.valid=DiscoverFingerChainsV6546(pose,getter,owner,count,rig,left,fresh);const bool changedW1=!sameW1;x.owner=owner;x.count=count;x.hand=handW1;x.lastTry=nowW1;if(x.valid)x.c=fresh;
        const auto methodW1=kcd2vr::finger_rig_w1::lastMethod[k].load(std::memory_order_relaxed);
        if(x.valid){char m[384]{};std::snprintf(m,sizeof(m),"V65.46-FINGER-CACHE side=%c roots=%d/%d/%d/%d/%d order=CORRECTED thumb/index/middle/ring/pinky roots-reversed-pairs trigger=index W1method=%s hand=%d joints=%u",left?'L':'R',x.c[0].id[0],x.c[1].id[0],x.c[2].id[0],x.c[3].id[0],x.c[4].id[0],methodW1==1?"NAME_CRC":methodW1==2?"FIXED_IDS":"TOPOLOGY",handW1,count);diagnostic_trace::Write(m);}
        else if(changedW1){char m[256]{};std::snprintf(m,sizeof(m),"W1 FINGER_MAP_FAILED side=%c hand=%d joints=%u owner=%p why=NO_NAMED_OR_SINGLE_CHAIN_FINGERS fingers=NATIVE",left?'L':'R',handW1,count,owner);diagnostic_trace::Write(m);
            (void)kcd2vr::visible_failure::publish("VRIK: this skeleton has no mappable fingers - native fingers",nowW1);}}
    bool ok=x.valid;if(ok)chains=x.c;lock.clear(std::memory_order_release);return ok;
}
struct RelaxedFingerJointV6553 { int id=-1; bool valid=false; QuatT28 rel{}; };
struct RelaxedFingerCacheV6553 {
    void* owner=nullptr;
    std::array<RelaxedFingerJointV6553,32> joints{};
    std::uint64_t neutralSinceMs=0;
    bool openLocked=false;
};
static RelaxedFingerCacheV6553 g_relaxed_fingers_v6553[2]{};
static RelaxedFingerCacheV6553 g_closed_fingers_v6688[2]{};
static bool g_closed_fingers_ready_v6688[2]{false,false};
// Retired unused implementation archived: research_archive/retired_pose_implementations

// Retired unused implementation archived: research_archive/retired_pose_implementations


// Retired unused implementation archived: research_archive/retired_pose_implementations

// Retired unused implementation archived: research_archive/retired_pose_implementations


// Retired unused implementation archived: research_archive/retired_pose_implementations


// Retired unused implementation archived: research_archive/retired_pose_implementations



// V65.54 F10 rung 4: full tracked upper-body overlay. Lower body remains native.
// Both arm solvers still own the hands/forearms; this only distributes HMD rotation
// through chest/neck/head and lets both clavicles follow their tracked upper arms.
// Retired unused implementation archived: research_archive/retired_pose_implementations



// V65.77 F10 rung 5: HARD upper-body authority test.
// Unlike V65.54's weighted overlay, this captures an idle/model-space upper-body baseline
// when rung 5 is entered and always rebuilds the rendered torso from that fixed baseline.
// Native draw/sheath/combat shoulder animation may continue internally for gameplay events,
// but it no longer becomes the visual base pose for spine/clavicle/head skinning.
// Retired unused implementation archived: research_archive/retired_pose_implementations



// V65.83 F10 rung 5 production-candidate foundation. Reconstruct the torso and
// shoulder roots from a fixed validated Henry baseline BEFORE solving either arm.
// Native combat animation may continue internally, but it cannot contribute its
// current upper-arm/clavicle pose to the visible VR shoulder foundation.
// Retired unused implementation archived: research_archive/retired_pose_implementations


// V65.20 F10 focused grip hypothesis: isolate only the mirrored solver/attachment-side
// chain 417 -> 419/420. Historical whole-rig recordings showed these three moving
// rigidly together, and 417 mirrors left-side 452 at the same +35 skeleton offset.
// This is deliberately narrower than the complete 408 descendant propagation.
// Retired unused implementation archived: research_archive/retired_pose_implementations


// Retired unused implementation archived: research_archive/retired_pose_implementations


// Retired unused implementation archived: research_archive/retired_pose_implementations


// Retired unused implementation archived: research_archive/retired_pose_implementations


// Retired unused implementation archived: research_archive/retired_pose_implementations



// Retired unused implementation archived: research_archive/retired_pose_implementations


