// R94: observe a native applied Henry kill across the existing SubmitHit call.
// Retail 6A350E20: D568F6 RPG+8 attacker; D56932 request+10 target;
// 725F6E target id +28; 725F89 death +40 (726440), separate KO +41.
// No damage, animation, or physics writes; the original is called exactly once.
struct NativeKillWitnessR94 {
    const void *attacker=nullptr,*target=nullptr,*ae=nullptr,*as=nullptr,*te=nullptr,*ts=nullptr;
    std::uint32_t attackerId=0,targetId=0;
};
static NativeKillWitnessR94 ReadNativeKillWitnessR94(const void* rpg,const void* request,bool applyNative) noexcept {
    NativeKillWitnessR94 w{};
    if(!applyNative||!rpg||!request||!g_base||!retail_build112::Ready(g_base))return w;
    w.attacker=Rd<const void*>(rpg,0x8,nullptr);w.target=Rd<const void*>(request,0x10,nullptr);
    if(!w.attacker||!w.target||w.attacker==w.target||
       !CombatActor(w.attacker,w.ae,w.as,false)||!w.ae||!IsPlayer(w.ae)||
       !CombatActor(w.target,w.te,w.ts,false)||!w.te||IsPlayer(w.te))return {};
    w.attackerId=EntityId(w.ae);w.targetId=EntityId(w.te);
    if(!w.attackerId||!w.targetId||w.attackerId==w.targetId)return {};
    return w;
}
static bool SubmitObservedNativeKillR94(void* rpg,void* hitInfo,const float* strength,bool* post,
    const void* request,bool applyNative,int attackType,void* details) noexcept {
    const auto before=ReadNativeKillWitnessR94(rpg,request,applyNative);
    const bool ok=g_submit_original?g_submit_original(rpg,hitInfo,strength,post,request,applyNative,attackType,details):false;
    if(!ok||!before.targetId||!hitInfo||Rd<std::uint8_t>(hitInfo,0x40,0)!=1)return ok;
    const auto after=ReadNativeKillWitnessR94(rpg,request,applyNative);
    if(before.attacker!=after.attacker||before.target!=after.target||before.ae!=after.ae||before.as!=after.as||
       before.te!=after.te||before.ts!=after.ts||before.attackerId!=after.attackerId||before.targetId!=after.targetId||
       Rd<std::uint32_t>(hitInfo,0x00,0)!=after.attackerId||Rd<std::uint32_t>(hitInfo,0x28,0)!=after.targetId)return ok;
    kcd2vr::gore32::NoteDead(after.targetId,reinterpret_cast<std::uintptr_t>(after.te),std::uint64_t(GetTickCount64()));
    return ok;
}
