static bool BowCameraInput35(kcd2vr::bow_camera35::Input& out,bool& bow) noexcept {
    out={};bow=false;
    if(!BowHandlingMode59c9(g_weapon_control_mode.load())||!g_gameplay_allowed.load()||
       diagnostics::lab::NativeReference()||g_phys_fault6698.load()||!retail_build112::Ready(WhBase()))return false;
    const auto now=std::uint64_t(GetTickCount64()),ready=g_phys_ready_sample6698.load(std::memory_order_acquire);
    PhysFrame6698 f{};
    if(!ready||!g_phys_frame6698.read(f))return false;
    bow=f.profiles[1].bow(); // observation only; an unqualified advertised bow is never neutral
    if(f.input.sample!=ready||ready!=g_phys_ready_sample6698.load(std::memory_order_acquire)||
       !f.player||!f.manager||f.manager!=g_henry_item_attach_manager.load(std::memory_order_acquire)||
       f.input.epoch!=f.request||f.request!=g_weapon_request_epoch6694.load(std::memory_order_acquire)||f.input.bodyEpoch!=g_pose_epoch6689.load(std::memory_order_acquire)||
       !f.input.allowed||!f.input.tick||now<f.input.tick||now-f.input.tick>80||!f.input.haveWorld||!ph::finite(f.input.world))return false;
    if(!bow)return f.profiles[1].classId>=0;
    out.owner={reinterpret_cast<std::uintptr_t>(f.player),reinterpret_cast<std::uintptr_t>(f.manager),
        f.profiles[1].key,f.profiles[1].generation,f.request,f.input.bodyEpoch};
    out.tick=f.input.tick;out.sample=f.input.sample;out.origin=f.input.world.p;out.rotation=f.input.world.q;out.scale=f.input.worldScale;
    out.drawing=f.output.bowHeld||(f.input.bowGripDraw47c2?f.input.grip[0]>.5:f.input.trigger[0]>.5);
    // C63: ammunition/shot readiness may be false while the same tracked bow
    // remains owned (empty/quiver/nock). It must not erase the neutral eye and
    // reintroduce the native reload/draw sway. No firing authority is granted.
    out.qualified=f.input.tracked[0]&&f.input.tracked[1]&&f.output.active[1]&&f.output.owned[1];
    return kcd2vr::bow_camera35::fresh(out,now);
}
bool BowCameraNeutral35(std::uint64_t now) noexcept {
    if(!BowHandlingMode59c9(g_weapon_control_mode.load()))return true;
    kcd2vr::bow_camera35::Input input{};bool bow=false;
    if(BowCameraInput35(input,bow))return !bow||!input.drawing;
    if(bow||!g_gameplay_allowed.load()||g_phys_fault6698.load()||diagnostics::lab::NativeReference()||
       !retail_build112::Ready(WhBase()))return false;
    // Bare hands can enter dialogue before any physical weapon frame exists.
    // Use the already-copied CURRENT native equipment witness to prove empty /
    // nonbow; no stale or unknown held-bow frame may acquire this exception.
    PhysEquipment6698 equipment{};
    if(!g_phys_equipment6698.read(equipment)||!equipment.valid||!equipment.player||!equipment.manager||
       equipment.manager!=g_henry_item_attach_manager.load(std::memory_order_acquire)||!equipment.tick||
       now<equipment.tick||now-equipment.tick>80||equipment.request!=g_weapon_request_epoch6694.load()||
       equipment.epoch!=g_pose_epoch6689.load()||!equipment.handKnown[1])return false;
    if(!equipment.handKey[1])return true;
    const int index=equipment.heldIndex[1];
    if(index<0||index>=equipment.count||index>=int(equipment.items.size()))return false;
    const auto& item=equipment.items[index];
    return item.held&&item.profile.key==equipment.handKey[1]&&item.profile.classId>=0&&!item.profile.bow();
}
bool BowCameraCorrection127(float baseX,float baseY,float baseZ,float& cx,float& cy,float& cz) noexcept {
    cx=cy=cz=0.f;
    static kcd2vr::bow_camera35::State state{};static std::mutex mutex;
    std::unique_lock<std::mutex> lock(mutex,std::try_to_lock);if(!lock.owns_lock())return false;
    kcd2vr::bow_camera35::Input input{};bool bow=false;
    if(!BowCameraInput35(input,bow)||!bow){state={};return false;}
    pm::Vec3 correction{};
    if(!kcd2vr::bow_camera35::correct(state,input,{baseX,baseY,baseZ},GetTickCount64(),correction))return false;
    cx=float(correction.x);cy=float(correction.y);cz=float(correction.z);return true;
}
void ObserveBowCamera126(float x,float y,float z,float hmdX,float hmdY,float hmdZ,int eye,std::uint64_t xrFrame) noexcept {
    if(!FullWeaponMode120(g_weapon_control_mode.load())||!g_gameplay_allowed.load())return;
    static std::atomic<std::uint64_t> next{0};const auto now=std::uint64_t(GetTickCount64());auto due=next.load();
    if(now<due||!next.compare_exchange_strong(due,now+500))return;
    PhysFrame6698 f{};
    if(!g_phys_frame6698.read(f)||!f.profiles[1].bow()||!f.input.allowed||!f.input.sample||
       now<f.input.tick||now-f.input.tick>80||f.input.bodyEpoch!=g_pose_epoch6689.load()||
       f.request!=g_weapon_request_epoch6694.load()||!f.input.haveWorld||!ph::finite(f.input.world))return;
    const auto nativeLocal=pm::rotate(pm::conjugate(f.input.world.q),pm::Vec3{x,y,z}-f.input.world.p);
    if(!pm::finite(nativeLocal))return;
    char msg[512]{};std::snprintf(msg,sizeof(msg),
        "V66.126 BOW_CAMERA xrFrame=%llu sample=%llu epoch=%llu poseAgeMs=%llu eye=%d triggerR=%.3f drawing=%d nativeCameraModel=(%.4f,%.4f,%.4f) hmdDeltaXR=(%.4f,%.4f,%.4f) nativePosition=OBSERVED_UNMODIFIED",
        (unsigned long long)xrFrame,(unsigned long long)f.input.sample,(unsigned long long)f.input.bodyEpoch,
        (unsigned long long)(now-f.input.tick),eye,f.input.trigger[0],f.output.bowHeld,
        nativeLocal.x,nativeLocal.y,nativeLocal.z,double(hmdX),double(hmdY),double(hmdZ));
    diagnostic_trace::Write(msg);
}
