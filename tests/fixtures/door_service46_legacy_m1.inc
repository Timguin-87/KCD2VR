// C46R1 worker lifecycle. Ordinary calibrated handling admits the exact door
// bridge. Debug capture / the old workstation experiment do not own its lifetime.
{
    namespace channel46=kcd2vr::door_signal19c4r4;
    const auto now46=std::uint64_t(GetTickCount64());
    const bool wanted46=kcd2vr::door_use46::enabled.load(std::memory_order_acquire)&&
        MiscInteractionDiagMode()==2&&CalibrationReady()&&InteractionActionReady175()&&
        retail_build112::Ready(WhBase())&&action_time_owner59c11::Installed();
    static bool previousWanted46=false;
    static std::uint64_t retryAfter46=0;
    static int reportedBridge46=-1;
    if(!wanted46){
        (void)StopNativeDoor19c4(); // cleanup stays on the existing native/Lua owners
        retryAfter46=0;reportedBridge46=-1;
    }else if(channel46::Ready())StartNativeDoor19c4();
    else if(!retryAfter46||now46>=retryAfter46){
        const bool ready46=channel46::SetEnabled(true);
        if(ready46)StartNativeDoor19c4();
        retryAfter46=now46+1000;
        if(reportedBridge46!=int(ready46)){
            reportedBridge46=int(ready46);
            diagnostic_trace::Write(ready46?
                "C46 DOOR_SERVICE bridge=READY source=NORMAL_CALIBRATED_HANDLING companionAck=REQUIRED":
                "C46 DOOR_SERVICE bridge=UNAVAILABLE exactPinsOrHook=REFUSED nativeControls=AVAILABLE");
        }
    }
    if(previousWanted46&&!wanted46)
        diagnostic_trace::Write("C46 DOOR_SERVICE mode=REVOKED cleanup=NATIVE_AND_LUA_OWNERS");
    previousWanted46=wanted46;
}
