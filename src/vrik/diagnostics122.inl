// Bounded observation only. Camera producer copies a committed POD snapshot;
// formatting/file output happens on the existing diagnostic service worker.
struct CameraBodyEvidence122 {
    CommittedFrame6695 body{};V3 eye{};std::uint64_t tick=0,xrFrame=0;int eyeIndex=0;
};
static kcd2vr::core::SnapshotMailbox<CameraBodyEvidence122> g_camera_body122;
static bool CaptureCameraBody122(V3 eye,int eyeIndex,std::uint64_t xrFrame) noexcept {
    const auto now=std::uint64_t(GetTickCount64());
    if(!diagnostics::capture168::active(diagnostics::capture168::Domain::Combat,now)&&!diagnostics::capture168::active(diagnostics::capture168::Domain::Hud,now))return false;
    if(g_census_mode.load()<3||!g_gameplay_allowed.load()||!g_head_valid.load()||
       !g_committed_pose_valid6689.load()||!std::isfinite(eye.x)||!std::isfinite(eye.y)||!std::isfinite(eye.z))return false;
    CameraBodyEvidence122 e{};
    if(!g_committed_frame6695.read(e.body)||!e.body.headEvidence122||
       !pm::fresh(e.body.stamp,g_pose_epoch6689.load(),reinterpret_cast<std::uintptr_t>(g_committed_character6689.load()),now,80))return false;
    e.eye=eye;e.tick=now;e.xrFrame=xrFrame;e.eyeIndex=eyeIndex;return g_camera_body122.publish(e);
}
static void ServiceBodyEvidence122() noexcept {
    const auto now=std::uint64_t(GetTickCount64());
    if(!diagnostics::capture168::active(diagnostics::capture168::Domain::Combat,now)&&!diagnostics::capture168::active(diagnostics::capture168::Domain::Hud,now))return;
    static std::uint64_t last=0;CameraBodyEvidence122 e{};
    if(!g_camera_body122.read(e)||!e.tick||e.tick==last)return;last=e.tick;
    const auto world=[&](V3 p){return ph::point(PhysWorld6698(e.body.world),pm::Vec3{p.x,p.y,p.z}*e.body.world[7]);};
    const auto native=world(e.body.nativeHead122),solved=world(e.body.solvedHead122),target=world(e.body.targetHead122),pelvis=world(e.body.pelvis122);
    const pm::Vec3 eye{e.eye.x,e.eye.y,e.eye.z};
    char msg[960]{};std::snprintf(msg,sizeof(msg),
        "V66.122 CAMERA_BODY sample=%llu xrFrame=%llu epoch=%llu ageMs=%llu eye=%d camera=(%.4f,%.4f,%.4f) nativeHead=(%.4f,%.4f,%.4f) solvedHead=(%.4f,%.4f,%.4f) targetHead=(%.4f,%.4f,%.4f) pelvis=(%.4f,%.4f,%.4f) cameraMinusSolvedZ=%.4f targetErrorM=%.4f space=WORLD camera=IMMERSIVE_EYE eyeToBoneOffset=UNMEASURED sameRenderPose=UNPROVEN",
        (unsigned long long)e.body.stamp.sample,(unsigned long long)e.xrFrame,(unsigned long long)e.body.stamp.epoch,
        (unsigned long long)(e.tick-e.body.stamp.tick),e.eyeIndex,eye.x,eye.y,eye.z,native.x,native.y,native.z,
        solved.x,solved.y,solved.z,target.x,target.y,target.z,pelvis.x,pelvis.y,pelvis.z,eye.z-solved.z,pm::length(target-solved));
    diagnostic_trace::Write(msg);
}
