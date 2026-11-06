// Included after existing exact Henry root proof. No new native ABI or hook.
// Called only after exact Henry native Update, under OwnerTransaction19c4.
// Snapshot observation does not require the separate native-mutation thread.
// This native-pose yield MUST NOT depend on acquisition/gameplay tracking input:
// that input intentionally becomes invalid while nativeBodyOwned112 is true.
static void ObserveNativePassengerA34(void* ctrl,std::uint64_t now) noexcept {
    namespace passenger=kcd2vr::native_passenger_a34;
    static passenger::Root previous{};static std::uint64_t serial=0;
    passenger::Root out{};CurrentRoot57 root{};
    const auto character=reinterpret_cast<std::uintptr_t>(HenryCharacter18());
    const auto body=kcd2vr::frik_pose_probe::PoseEpochH24();
    const void* contexts=ctrl?Rd<const void*>(ctrl,kCtrlScopeContexts,nullptr):nullptr;
    if(retail_build112::Ready(WhBase())&&IsHenryController(ctrl)&&
       character&&body&&contexts&&Rd<std::uintptr_t>(contexts,kScopeContextChar,0)==character&&
       ReadCurrentRootIdentity57(ctrl,false,root)&&root.action&&root.scope&&
       passenger::classify(root.crc)!=passenger::Kind::None&&
       Rd<int>(root.action,kActStatus,-1)==2&&Rd<int>(root.scope,kScopeLayer,-1)==0){
        out.controller=reinterpret_cast<std::uintptr_t>(ctrl);out.character=character;
        out.action=reinterpret_cast<std::uintptr_t>(root.action);out.scope=reinterpret_cast<std::uintptr_t>(root.scope);
        out.context=reinterpret_cast<std::uintptr_t>(root.context);out.epoch=body;
        out.sourceTick=now;out.tick=now;out.token=root.token;out.crc=root.crc;
        out.fragment=root.fragment;out.status=2;out.layer=0;
        // Rejoin after classification. A native synchronous restart/replacement
        // or status transition cannot inherit the previous action's authority.
        CurrentRoot57 after{};
        if(!ReadCurrentRootIdentity57(ctrl,false,after)||after.action!=root.action||after.scope!=root.scope||
           after.context!=root.context||after.token!=root.token||after.fragment!=root.fragment||after.crc!=root.crc||
           Rd<int>(root.action,kActStatus,-1)!=2||Rd<int>(root.scope,kScopeLayer,-1)!=0||
           Rd<const void*>(ctrl,kCtrlScopeContexts,nullptr)!=contexts||Rd<std::uintptr_t>(contexts,kScopeContextChar,0)!=character||
           reinterpret_cast<std::uintptr_t>(HenryCharacter18())!=character||kcd2vr::frik_pose_probe::PoseEpochH24()!=body)out={};
    }
    if(out.action)out.serial=passenger::sameAction(previous,out)&&previous.serial?previous.serial:++serial;
    const bool changed=bool(out.action)!=bool(previous.action)||(out.action&&!passenger::sameAction(previous,out));
    if(changed&&diagnostics::capture168::active(diagnostics::capture168::Domain::Interaction,now)){char line[256]{};std::snprintf(line,sizeof(line),
        "A34 NATIVE_PASSENGER active=%d kind=%u serial=%llu token=%u fragment=%d body=%s camera=UNCHANGED gameplayOutcome=UNCLAIMED",
        out.action?1:0,unsigned(passenger::classify(out.crc)),(unsigned long long)out.serial,out.token,out.fragment,passenger::trackedWash(out,character,body,now)?"TRACKED_WASH_ARMS":"NATIVE_FULL");
        diagnostic_trace::Write(line);}
    previous=out;(void)passenger::witness.publish(out);
}
