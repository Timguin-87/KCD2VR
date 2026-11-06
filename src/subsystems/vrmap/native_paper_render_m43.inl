// Uses the already-pinned D63 native BONE render leaf and PRIVATE context.
// Included after D63 input/authority helpers, before its existing detour.
static bool NativePaperRenderM43(void* context,void* bone,const StowedRenderHeaderD63& h,
 std::uintptr_t base,void* manager,void* character,StowedRenderFnD63 original) noexcept {
 namespace p=kcd2vr::native_paper_m43;
 const int slot=p::slot(h.name);if(slot<0)return false;
 p::Frame frame{};
 if(!p::frames.read(frame)||!p::owns(frame,slot,reinterpret_cast<std::uintptr_t>(manager),
    reinterpret_cast<std::uintptr_t>(character),reinterpret_cast<std::uintptr_t>(bone),reinterpret_cast<std::uintptr_t>(h.binding)))return false;
 // Only an exact binding recorded by the completed native-frame owner reaches
 // this point. Missing/stale authority suppresses OUR paper, never a game item.
 const auto valid=[&](std::uint64_t now) noexcept {
  const auto epoch=g_pose_epoch6689.load(std::memory_order_acquire);
  return held_map_m1::NativePaperSelectedM43()&&p::visible.load(std::memory_order_acquire)&&p::fresh(frame,now,epoch,
   held_map_m1::activation.load(std::memory_order_acquire),held_map_m1::Requested(),held_map_m1::Allowed(now))&&
   StowedRenderAuthorityD63(base,manager,character,epoch,g_pose_policy_generation57.load(std::memory_order_acquire),
    ReadCompletedPublicationT23(),now)&&TrackingHandUsable111(0,now)&&ExactHandItemState(0)==0;
 };
 if(!valid(std::uint64_t(GetTickCount64())))return true;
 // CCGFAttachment owns the same loaded IStatObj; a rebinding/model replacement
 // between native publication and rendering cannot consume an old receipt.
 std::uintptr_t stat=0;
 if(StowedRenderBindingVftD63(h.binding)!=base+0x3E5F4C0||
    !Read(static_cast<unsigned char*>(h.binding)+8,stat)||stat!=frame.slots[slot].stat)return true;
 alignas(16) StowedRenderContextD63 privateContext{};
 alignas(16) kcd2vr::stowed_render_pose_d63::Mat34 ignored{},corrected{};
 QuatT28 nativeCache{},relative{};
 if(!StowedRenderInputsD63(context,bone,privateContext,ignored,nativeCache,relative)||
    !kcd2vr::stowed_render_pose_d63::privateParent(frame.paper,StowedRenderPoseD63(nativeCache),
      frame.local[slot],{true,false},corrected)||!valid(std::uint64_t(GetTickCount64())))return true;
 // Native original computes privateParent*nativeCache*additional. This becomes
 // paperWorld*publishedLocal*additional, preserving size, D64 edge grip,
 // optional support and the player glyph's current local offset exactly once.
 privateContext.parent=corrected.m.data();
 original(&privateContext,bone);
 return true;
}
