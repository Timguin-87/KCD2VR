// Real producer/reader, explicit synthetic committed body. No game memory access.
{
    ResetPageMemo169();const auto reuseStart169=g_perf101R2[6].reuse122.load();
    const auto queryStart122=g_perf101R2[6].queries122.load();int readable122=1,out122=0;
    {PerfScope101R2 timing122(6);
     {ReadScope101R2 first122;Read(&readable122,out122);}
     {ReadScope101R2 second122;Read(&readable122,out122);}}
    // V66.169: the second callback reuses the 150 ms page memo instead of a new query.
    ck(g_perf101R2[6].queries122.load()==queryStart122+1&&g_perf101R2[6].reuse122.load()>=reuseStart169+1,"V169 per-stage totals: second independent callback reuses the page memo");
    CommittedFrame6695 saved122{};g_committed_frame6695.read(saved122);
    const auto savedChar122=g_committed_character6689.load();const auto savedValid122=g_committed_pose_valid6689.load();
    const auto savedMode122=g_census_mode.load();const auto savedAllowed122=g_gameplay_allowed.load();const auto savedHead122=g_head_valid.load();
    CommittedFrame6695 b{};b.stamp={g_pose_epoch6689.load(),12,hostTickCount64,0x1234};
    b.world[3]=b.world[7]=1;b.headEvidence122=true;b.nativeHead122={0,0,1.7f};b.solvedHead122={0,0,1.5f};b.targetHead122={0,0,1.7f};
    g_committed_frame6695.publish(b);g_committed_character6689=reinterpret_cast<void*>(0x1234);g_committed_pose_valid6689=true;
    g_census_mode=3;g_gameplay_allowed=g_head_valid=true;
    ck(CaptureCameraBody122({0,0,1.8f},1,55),"camera diagnostic pairs with fresh committed body without native reads");
    CameraBodyEvidence122 e{};ck(g_camera_body122.read(e)&&e.body.stamp.sample==12&&e.xrFrame==55&&e.eyeIndex==1,
        "camera report retains both identities rather than claiming identical render samples");
    ck(e.body.solvedHead122.z==1.5f&&e.eye.z==1.8f,"camera observation does not snap the body or eye to hide error");
    hostTickCount64+=81;ck(!CaptureCameraBody122({0,0,1.8f},0,56),"stale body is not labelled current camera evidence");hostTickCount64-=81;
    g_gameplay_allowed=false;ck(!CaptureCameraBody122({0,0,1.8f},0,56),"Theater/menu ownership prevents immersive body observation");g_gameplay_allowed=true;
    g_committed_character6689=reinterpret_cast<void*>(0x2345);ck(!CaptureCameraBody122({0,0,1.8f},0,56),"replaced actor cannot inherit body evidence");g_committed_character6689=reinterpret_cast<void*>(0x1234);
    g_census_mode=2;ck(!CaptureCameraBody122({0,0,1.8f},0,56),"arms-only rung does not pretend the mod solved the head");
    // Counter assertions explicitly select an active predictor observation lease.
    const auto savedWeapon176=g_weapon_control_mode.load();const auto savedUntil176=g_contact_until114.load();
    const auto savedCaptureMask176=diagnostics::capture168::gate.mask.load();
    const auto savedCaptureUntil176=diagnostics::capture168::gate.until[0].load();
    diagnostics::capture168::gate.select(diagnostics::capture168::Domain::Combat,true,GetTickCount64());
    g_weapon_control_mode=7;g_contact_until114=GetTickCount64()+diagnostics::capture168::Duration;
    const auto count122=g_prediction_gates122[11].load(),other122=g_prediction_gates122[12].load();
    for(int n=0;n<23;++n)PredictionRejectContext122("CONTACT_DEFERRED");
    ck(g_prediction_gates122[11].load()==count122+23&&g_prediction_gates122[12].load()==other122,
        "all rotation deferrals count separately from unknown geometry, independent of print throttle");
    g_prediction_gates122[11]=count122;
    g_weapon_control_mode=savedWeapon176;g_contact_until114=savedUntil176;
    diagnostics::capture168::gate.until[0]=savedCaptureUntil176;diagnostics::capture168::gate.mask=savedCaptureMask176;
    g_committed_frame6695.publish(saved122);g_committed_character6689=savedChar122;g_committed_pose_valid6689=savedValid122;
    g_census_mode=savedMode122;g_gameplay_allowed=savedAllowed122;g_head_valid=savedHead122;
}
