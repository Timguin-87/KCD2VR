// Game-thread-only native read. Skinning/camera consume the copied receipt.
static bool CallNativeCrouchR93(std::uintptr_t function,std::uintptr_t player,bool& value) noexcept {
 __try {value=reinterpret_cast<bool(__fastcall*)(void*)>(function)(reinterpret_cast<void*>(player));return true;}
 __except(EXCEPTION_EXECUTE_HANDLER){value=false;return false;}
}
static void PublishNativeCrouchR93() noexcept {
 namespace c=kcd2vr::native_crouch_r93;
 // R98: COMBINED body height reads the same exact stance in either crouch mode.
 const auto combinedR98=[]() noexcept {const auto c51=g_frame_control51.load(std::memory_order_acquire);
  return (c51&kNativeBodyHeightBit54)==0&&(c51&kCombinedBodyHeightBitR98)!=0;};
 if(!c::enabled.load(std::memory_order_acquire)&&!combinedR98())return;
 const auto now=std::uint64_t(GetTickCount64());
 const auto character=g_player_char37.load(std::memory_order_acquire);
 const auto player=reinterpret_cast<std::uintptr_t>(g_player_module_d76.load(std::memory_order_acquire));
 const auto entity=reinterpret_cast<std::uintptr_t>(g_last_player_entity.load(std::memory_order_acquire));
 c::Receipt r{};r.generation=c::generation.load(std::memory_order_acquire);
 r.owner={WhBase(),player,entity,character,g_pose_epoch6689.load(std::memory_order_acquire),
  kcd2vr::tracking59c6::epoch.load(std::memory_order_acquire),g_frame_control51.load(std::memory_order_acquire),
  g_player_lifetime_d44.load(std::memory_order_acquire),now};
 // A fully adopted ordinary item may outlive the generic Picking policy
 // cache. Query the same live native stance, never retain an old height receipt.
 kcd2vr::grasp13::FinishingPresentationR94 adoptedR95{};
 const bool policyGapR95=!g_gameplay_allowed.load()||g_ctx_scripted.load();
 const bool adoptedR95Valid=policyGapR95&&action_time_owner59c11::ReadAdoptedAttachmentPresentationR95(adoptedR95)&&
  adoptedR95.binding.character==character&&adoptedR95.binding.epoch==r.owner.body&&
  adoptedR95.trackingEpoch==r.owner.tracking;
 const auto presentationCurrentR95=[&]() noexcept {
  return (g_gameplay_allowed.load()&&!g_ctx_scripted.load())||
   (policyGapR95&&adoptedR95Valid&&action_time_owner59c11::AdoptedAttachmentPresentationCurrentR95(adoptedR95));
 };
 const auto context=[&]() noexcept {return (c::enabled.load(std::memory_order_acquire)||combinedR98())&&
  GetCurrentThreadId()==g_stage_thread596.load(std::memory_order_acquire)&&
  PlayerIdentityAccepts37(reinterpret_cast<void*>(character),now)&&g_cal_state.load()==2&&
  presentationCurrentR95()&&!g_ctx_menu.load()&&!g_ctx_dialogue.load()&&!g_ctx_cutscene.load()&&
  !g_ctx_reading.load()&&!diagnostics::lab::NativeReference()&&
  !animation_filter::nativeBodyOwned112.load()&&!animation_filter::NativeKickActive175()&&
  !NativePassengerCurrentA34(now)&&!kcd2vr::activity_input163::witness175.Active()&&
  (r.owner.control&kNativeBodyHeightBit54)==0&&
  player==reinterpret_cast<std::uintptr_t>(g_player_module_d76.load())&&entity==reinterpret_cast<std::uintptr_t>(g_last_player_entity.load())&&
  character==g_player_char37.load()&&r.owner.body==g_pose_epoch6689.load()&&
  r.owner.tracking==kcd2vr::tracking59c6::epoch.load()&&r.owner.control==g_frame_control51.load()&&
  r.owner.lifetime==g_player_lifetime_d44.load()&&r.generation==c::generation.load();};
 const auto read=[](std::uintptr_t address,auto& value) noexcept {return Read(reinterpret_cast<void*>(address),value);};
 if(!context()||!c::query(r.owner,retail_build112::Ready(r.owner.base),read,CallNativeCrouchR93,r.crouched)||!context()){
  c::invalidate();return;
 }
 (void)c::receipt.publish(r); // Busy positive refresh never renews the preceding receipt's time.
}
