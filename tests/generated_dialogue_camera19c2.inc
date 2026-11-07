// Generated from src/camera_hook.cpp; do not edit.
core::SnapshotMailbox<ds::Policy> g_dialogue_policy59c10r2;
core::SnapshotMailbox<ds::EyeOffset> g_dialogue_eye59c10r2;
struct DialogueReceipt59c10r2 {std::uint64_t tick=0;unsigned reason=0;ds::State state{};bool head=false,learned=false,allowed=false;
    bool defaultEye=false,fromCull=false,headFromView28=false;std::uintptr_t headCharacter28=0;
    ds::V headWorld{},nativeView{},eye{};
    bool smooth38=false,held38=false;unsigned heldCause38=0;std::uint32_t holds38=0,snaps38=0;float lagM38=0;}; // A camera attempt, not a claim that this cull was published.
core::SnapshotMailbox<DialogueReceipt59c10r2> g_dialogue_receipt59c10r2;
std::atomic<std::uint64_t> g_dialogue_applied_tick59c13{0};
std::atomic<bool> g_dialogue_default_eye59c13{true};
core::SnapshotMailbox<dsm38::State> g_dialogue_smooth38; // C38 anchor low-pass + late-sample hold (visual base only)
std::atomic<bool> g_dialogue_smooth_on38{true}; // INI/menu VrDialogueSmoothCamera; false = exact C37C2 per-sample base
bool DialogueEyeBase59c10r2(Matrix34f& matrix,bool render,std::uint64_t roomRevisionD76=0,const native_roomscale_d76::Origin* canonicalD76=nullptr) noexcept {
    const auto now=std::uint64_t(GetTickCount64());
    const auto state=activity_lens59c10r1::Dialogue(now);
    ds::NativeHead head{};ds::EyeOffset learned{};ds::Policy policy{};
    const bool headFromView28=frik_pose_probe::NativeDialogueViewHead28(head,state,now);
    const bool haveHead=headFromView28||frik_pose_probe::NativeDialogueHead59c10r2(head,now);
    if(haveHead&&canonicalD76&&roomRevisionD76&&!roomscale_d76::compatible(*canonicalD76,head.roomOriginD76,roomRevisionD76))return false;
    (void)g_dialogue_eye59c10r2.read(learned);(void)g_dialogue_policy59c10r2.read(policy);
    ds::Eye native{};native.position={matrix.m[0][3],matrix.m[1][3],matrix.m[2][3]};
    for(unsigned i=0;i<3;++i)native.basis[i]={matrix.m[0][i],matrix.m[1][i],matrix.m[2][i]};
    // A loaded/replaced Henry character can enter dialogue before a neutral
    // gameplay eye is available. A ready calibration for another body must not
    // prevent the existing opted-in default eye from serving this fresh body.
    // Unknown/mismatched owners and missing head data never retire calibration;
    // recenter alone preserves it because the same body's identity is stable.
    if(haveHead&&ds::fresh(head,now)&&ds::fresh(state,now)&&state.entity==head.entity&&learned.ready&&
       (learned.entity!=head.entity||learned.character!=head.character)){
        learned=ds::EyeOffset{};g_dialogue_eye59c10r2.publish(learned);
    }
    const auto activityState19=activity_lens59c10r1::EyeState(now);
    // An activity may use the FirstPerson camera while moving its eye. Such a
    // sample must never become the neutral dialogue eye calibration.
    // Both callers supply the untouched native base, before the ONE HMD/IPD
    // composition below. The common render route reuses this exact culling
    // matrix, so render-only learning silently missed ordinary gameplay.
    // POD publication does not advance rendered-frame identity or query native/XR.
    if(haveHead&&activityState19.known&&!activityState19.active&&frik_pose_probe::BowCameraNeutral35(now)&&
       ds::learn(state,head,native,now,learned))g_dialogue_eye59c10r2.publish(learned);
    ds::Eye eye{};bool applied=haveHead&&ds::reconstruct(state,head,learned,policy,now,eye);bool viaDefault=false;
    // R59.16C10R13 (Miki: "in dialogues we replace the Henry actor with us"): when no first-person eye offset was learned yet, the
    // view still comes from Henry's own head - the measured gameplay relationship (V66.127: the camera sits ~5.6 cm above the head
    // bone, looking along the body) stands in. Never the dialogue camera entity.
    if(!applied&&haveHead&&!learned.ready&&g_dialogue_default_eye59c13.load(std::memory_order_relaxed)){
        ds::EyeOffset def{};def.entity=head.entity;def.character=head.character;def.local={0.f,0.f,0.056f};
        def.basis[0]={1,0,0};def.basis[1]={0,1,0};def.basis[2]={0,0,1};def.ready=true;
        applied=ds::reconstruct(state,head,def,policy,now,eye);viaDefault=applied;}
    const unsigned rawReason=applied?0u:!state.known?1u:!state.active?2u:!ds::embodiedOwner(state,now)?3u:
        !policy.enabled||!ds::age(policy.tick,now,80)?4u:!haveHead?5u:!learned.ready?6u:7u;
    // C38: the native anchor arrives at game/animation rate and sometimes late; follow it with a short
    // time-based low-pass and bridge a LATE sample of the same active dialogue with the last smoothed eye.
    // The HMD is still composed once afterwards on every publication. OFF = exact C37C2 behaviour.
    const bool smooth38=g_dialogue_smooth_on38.load(std::memory_order_relaxed);bool held38=false;dsm38::State sm38{};
    if(smooth38){(void)g_dialogue_smooth38.read(sm38);const dsm38::Config c38{};
        if(applied){(void)dsm38::follow(sm38,eye,head.entity,head.character,state.instance,DialogueSmoothNowNs38(),now,c38,eye);(void)g_dialogue_smooth38.publish(sm38);}
        else if((rawReason==4u||rawReason==5u||rawReason==3u)&&dsm38::hold(sm38,state,policy,now,c38,eye)){held38=applied=true;(void)g_dialogue_smooth38.publish(sm38);}}
    if(applied)g_dialogue_applied_tick59c13.store(now,std::memory_order_release);
    if(applied){
        matrix.m[0][3]=eye.position.x;matrix.m[1][3]=eye.position.y;matrix.m[2][3]=eye.position.z;
        for(unsigned i=0;i<3;++i){matrix.m[0][i]=eye.basis[i].x;matrix.m[1][i]=eye.basis[i].y;matrix.m[2][i]=eye.basis[i].z;}
    }
    // POD publication only. Formatting/file IO stay on the control thread.
    // Include the culling attempt: a later publish may reuse that composed
    // matrix without invoking this function again. This does not assert that
    // the culling matrix was consumed; the receipt explicitly names its stage.
    if(state.active||applied){
        DialogueReceipt59c10r2 r{};r.tick=now;r.reason=held38?8u:rawReason;
        r.smooth38=smooth38;r.held38=held38;r.heldCause38=held38?rawReason:0u;r.holds38=sm38.holds;r.snaps38=sm38.snaps;r.lagM38=sm38.lagM;
        r.state=state;r.head=haveHead;r.learned=learned.ready;r.allowed=policy.enabled;r.defaultEye=viaDefault;r.fromCull=!render;
        r.headFromView28=headFromView28;r.headCharacter28=haveHead?head.character:0;
        r.headWorld=head.world;r.nativeView=native.position;r.eye=applied?eye.position:native.position;
        g_dialogue_receipt59c10r2.publish(r);}
    return applied;
}
