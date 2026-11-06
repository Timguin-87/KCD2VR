static bool SelfBladeFromProfile54(const ph::Profile& p,ph::Pose pose,kcd2vr::self_contact54::Blade& out) noexcept {
    out={};if(!p.key||!p.generation||!p.entity||!p.primary||!p.geometry||!p.diagnosticBounds59c6||
       !(p.classId==1||p.classId==2||p.classId==4||p.classId==16)||p.side<0||p.side>1||!ph::finite(pose))return false;
    const auto axis=pc::unit(p.tip-p.grip[p.side].p);if(!axis)return false;
    // Exclude the native gripping hand/hilt. The blade radius is a bounded visual
    // skin approximation; exact triangle/collision thickness is not claimed.
    out.a=ph::point(pose,p.grip[p.side].p+*axis*.08);out.b=ph::point(pose,p.tip);out.radius=.006;out.valid=true;
    return kcd2vr::self_contact54::valid(out);
}
static bool StageOwnBladeContact54(CoherentTransaction6689& tx,int side,bool free,
    const kcd2vr::hand_shape59c7f::Shape& shape,Q4 orientation,V3& goal) noexcept {
    namespace sc=kcd2vr::self_contact54;
    if(!free||side<0||side>1||!tx.physicalActive||!tx.tracking.allowed||!tx.tracking.head_valid||
       tx.tracking.head_held||(side?tx.tracking.left_held:tx.tracking.right_held)||
       !(side?tx.tracking.left_valid:tx.tracking.right_valid))return false;
    const int weapon=1-side;const auto& p=tx.physicalProfiles[weapon];const auto& e=tx.physicalEquipment;
    const auto& correlation=tx.physicalCorrelation126[weapon];
    if(!e.valid||e.epoch!=tx.epoch||e.request!=tx.physicalInput.epoch||!e.player||
       !e.handKnown[side]||e.handKey[side]||!e.handKnown[weapon]||e.handKey[weapon]!=p.key||p.side!=weapon||
       !tx.physicalOutput.active[weapon]||!tx.physicalOutput.owned[weapon]||tx.physicalOutput.owned[side]||
       tx.physicalOutput.support[weapon]||tx.physicalOutput.bowHand169r2||
       tx.physicalOutput.displayLatched[weapon]||tx.physicalOutput.displayRejoinR13[weapon]||tx.physicalOutput.handlingOnly55c1[weapon]||
       !correlation.socket.valid||correlation.socket.key!=p.key||correlation.socket.generation!=p.generation||
       correlation.socket.character!=g_pose6689.character||correlation.stamp.actor!=reinterpret_cast<std::uintptr_t>(g_pose6689.character)||
       correlation.stamp.epoch!=tx.epoch||!correlation.stamp.sample||correlation.stamp.sample>tx.tracking.sequence||
       !correlation.stamp.tick||tx.tracking.tick<correlation.stamp.tick||tx.tracking.tick-correlation.stamp.tick>80)return false;
    const int held=e.heldIndex[weapon];
    if(held<0||held>=e.count||e.count>int(e.items.size())||!e.items[held].held||
       e.items[held].profile.key!=p.key||e.items[held].profile.generation!=p.generation||
       e.items[held].profile.entity!=p.entity)return false;
    sc::Blade blade{};if(!SelfBladeFromProfile54(p,tx.physicalOutput.weapons[weapon],blade))return false;
    const auto correction=sc::separate(shape,PV(goal),blade,PV(goal)-tx.physicalOutput.hands[weapon].p);
    if(!correction.valid)return false;
    auto& sample=tx.selfContact54;sample={};
    sample.owner={reinterpret_cast<std::uintptr_t>(g_pose6689.character),reinterpret_cast<std::uintptr_t>(e.player),
        tx.epoch,e.request,p.key,p.generation,p.entity,side,weapon};
    sample.shape=shape;sample.blade=blade;sample.wrist=PV(goal)+correction.correction;sample.orientation=PQ(orientation);
    sample.tick=tx.tracking.tick;sample.sample=tx.tracking.sequence;sample.timeNs=tx.tracking.timeNs;sample.scale=tx.physicalInput.worldScale;sample.eligible=true;
    goal=VP(sample.wrist);return true;
}
// Sword against the opposite held shield: native LOCAL model bounds only,
// voice gesture proxy. Neither grip/weapon pose is corrected and this never
// becomes a native shield-block witness, collision ray or damage input.
static bool StageShieldPat54(CoherentTransaction6689& tx,const char** refusalB4=nullptr) noexcept {
    const auto refuseB4=[&](const char* reason){if(refusalB4)*refusalB4=reason;return false;};
    namespace sc=kcd2vr::self_contact54;auto& sample=tx.shieldContact54;sample={};
    if(!tx.physicalActive||!tx.tracking.allowed||!tx.tracking.head_valid||tx.tracking.head_held||
       !tx.tracking.left_valid||!tx.tracking.right_valid||tx.tracking.left_held||tx.tracking.right_held)return refuseB4("TRACKING");
    const auto& e=tx.physicalEquipment;
    if(!e.valid||e.epoch!=tx.epoch||e.request!=tx.physicalInput.epoch||!e.player||e.count<0||e.count>int(e.items.size()))return refuseB4("EQUIPMENT");
    int shield=-1;for(int side=0;side<2;++side)if(tx.physicalProfiles[side].shield176())shield=side;
    if(shield<0)return refuseB4("NO_SHIELD");const int weapon=1-shield;
    for(int side=0;side<2;++side){const auto& p=tx.physicalProfiles[side];const auto& c=tx.physicalCorrelation126[side];const int held=e.heldIndex[side];
        if(!e.handKnown[side]||e.handKey[side]!=p.key||p.side!=side||!p.key||!p.generation||!p.entity||
           !p.primary||!p.geometry||!p.diagnosticBounds59c6||!tx.physicalOutput.active[side]||!tx.physicalOutput.owned[side]||
           tx.physicalOutput.support[side]||tx.physicalOutput.displayLatched[side]||tx.physicalOutput.displayRejoinR13[side]||tx.physicalOutput.handlingOnly55c1[side]||
           held<0||held>=e.count||!e.items[held].held||e.items[held].profile.key!=p.key||
           e.items[held].profile.generation!=p.generation||e.items[held].profile.entity!=p.entity||
           !c.socket.valid||c.socket.key!=p.key||c.socket.generation!=p.generation||c.socket.character!=g_pose6689.character||
           c.stamp.actor!=reinterpret_cast<std::uintptr_t>(g_pose6689.character)||c.stamp.epoch!=tx.epoch||!c.stamp.sample||
           c.stamp.sample>tx.tracking.sequence||!c.stamp.tick||tx.tracking.tick<c.stamp.tick||tx.tracking.tick-c.stamp.tick>80)return refuseB4(side?"LEFT_ITEM_SOCKET_OR_POSE":"RIGHT_ITEM_SOCKET_OR_POSE");
    }
    const auto& p=tx.physicalProfiles[weapon];const auto& target=tx.physicalProfiles[shield];
    sc::Blade blade{};if(!SelfBladeFromProfile54(p,tx.physicalOutput.weapons[weapon],blade))return refuseB4("SWORD_GEOMETRY");
    const auto transform=tx.physicalOutput.weapons[shield];if(!ph::finite(transform))return refuseB4("SHIELD_POSE");
    sc::Box box{target.diagnosticBoundsMin59c6,target.diagnosticBoundsMax59c6,transform.p,transform.q,true};
    if(!sc::valid(box))return refuseB4("SHIELD_BOUNDS");
    sample.owner={reinterpret_cast<std::uintptr_t>(g_pose6689.character),reinterpret_cast<std::uintptr_t>(e.player),
        tx.epoch,e.request,p.key,p.generation,p.entity,shield,weapon};
    sample.blade=blade;sample.box=box;sample.shieldKey=target.key;sample.shieldGeneration=target.generation;sample.shieldEntity=target.entity;
    sample.tick=tx.tracking.tick;sample.sample=tx.tracking.sequence;sample.timeNs=tx.tracking.timeNs;
    sample.scale=tx.physicalInput.worldScale;sample.eligible=true;return true;
}
// C60: the two real current native hand/socket owners qualify the local solid
// proxy. This runs with ordinary held-weapon handling, independent of F4 and the
// optional free-hand/finger contact checkbox. No new native call or body exists.
static kcd2vr::own_shield_constraint60::Result ConstrainOwnShield60(CoherentTransaction6689& tx) noexcept {
    kcd2vr::own_shield_constraint60::Result refused{};
    const auto receiptB4=[&](const char* reason,kcd2vr::own_shield_constraint60::Result result){
        // Capture-only changed-state receipts; no new ray, script or native call.
        // Thirty-two lines per process at most, formatting off when F3 is off.
        if((tx.physicalProfiles[0].shield176()||tx.physicalProfiles[1].shield176())&&cc::active(tx.tracking.tick)){
            static std::atomic_flag busyB4=ATOMIC_FLAG_INIT;
            if(busyB4.test_and_set(std::memory_order_acquire))return result;
            static const char* previous=nullptr;static std::uint64_t last=0;static unsigned count=0;
            if(count<32&&previous!=reason&&(!last||(tx.tracking.tick>=last&&tx.tracking.tick-last>=1000))){
                previous=reason;last=tx.tracking.tick;++count;char line[256]{};
                std::snprintf(line,sizeof(line),"B4 OWN_SHIELD stage=%s sample=%llu usable=%d stopped=%d fraction=%.4f backend=CHECKED_PAIR_PROXY nativeBlock=NONE",
                    reason,(unsigned long long)tx.tracking.sequence,int(result.usable),int(result.limited),result.fraction);
                diagnostic_trace::Write(line);
            }
            busyB4.clear(std::memory_order_release);
        }
        return result;
    };
    if(!tx.physicalInput.contact||!tx.physicalInput.allowed||tx.physicalOutput.impaleActive||
       !tx.tracking.source_epoch||tx.tracking.source_epoch!=kcd2vr::tracking59c6::epoch.load(std::memory_order_acquire)||
       tx.epoch!=g_pose_epoch6689.load(std::memory_order_acquire)||
       tx.physicalInput.epoch!=g_weapon_request_epoch6694.load(std::memory_order_acquire)||
       !tx.physicalEquipment.manager||!g_gameplay_allowed.load(std::memory_order_acquire)||g_ctx_menu.load(std::memory_order_acquire)||
       g_ctx_dialogue.load(std::memory_order_acquire)||g_ctx_cutscene.load(std::memory_order_acquire)||
       g_ctx_reading.load(std::memory_order_acquire)||animation_filter::nativeBodyOwned112.load(std::memory_order_acquire)||
       animation_filter::NativeKickActive175()||WorkstationContextRefusal19c4r2(tx.tracking.tick,true)!=nullptr)return receiptB4("CONTEXT",refused);
    const char* refusalB4="ITEM_SOCKET_OR_POSE";
    if(!StageShieldPat54(tx,&refusalB4))return receiptB4(refusalB4,refused);
    for(int side=0;side<2;++side){
        if(!tx.physicalInput.intentTracked[side])return receiptB4("INTENT_TRACKING",refused);
        const auto& path=tx.physicalOutput.checkedPath60[side];const auto& p=tx.physicalProfiles[side];
        if(!path.valid||path.count<2||path.count>path.point.size())return receiptB4(side?"LEFT_WORLD_PATH":"RIGHT_WORLD_PATH",refused);
        const auto end=path.point[path.count-1];
        if(pc::length(end.p-tx.physicalOutput.weapons[side].p)>1e-7||
           pc::angle(end.q,tx.physicalOutput.weapons[side].q)>1e-7)return receiptB4("PATH_ENDPOINT",refused);
        // Every prefix has a reachable primary pivot (linear within each
        // checked segment). A native acquisition/recenter outside the current
        // anatomical envelope never gains a new local constraint owner.
        for(unsigned i=0;i<path.count;++i)if(pc::length(ph::point(path.point[i],p.grip[side].p)-
            tx.physicalInput.shoulders[side])>tx.physicalInput.reach[side]+.002)return receiptB4("REACH",refused);
    }
    const auto result=kcd2vr::own_shield_constraint60::apply(tx.physicalInput,tx.physicalProfiles,
        tx.physicalState,tx.physicalOutput,tx.shieldContact54.owner.weapon,tx.shieldContact54.owner.hand);
    if(result.usable&&result.limited)(void)StageShieldPat54(tx); // actual allowed pair, never the discarded endpoint
    return receiptB4(result.usable?(result.limited?"STOPPED":"CLEAR"):"GEOMETRY_REFUSED",result);
}
