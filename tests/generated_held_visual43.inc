// Actual production visual-only permission; regenerate via tools/extract_held_visual43.py.
static int HeldInteractionVisualSideD78() noexcept {
    if(g_cal_state.load(std::memory_order_acquire)!=2||diagnostics::lab::NativeReference()||
       !action_time_owner59c11::NativeGraspEnabled())return -1;
    kcd2vr::grasp13::Binding b{};
    const bool current=action_time_owner59c11::ReadGraspBinding(b)&&b.attached&&b.place&&b.key&&b.side>=0&&b.side<2&&
        b.character&&b.character==reinterpret_cast<std::uintptr_t>(g_committed_character6689.load(std::memory_order_acquire))&&
        b.epoch==g_pose_epoch6689.load(std::memory_order_acquire)&&
        action_time_owner59c11::WristOwnedByController(b.side);
    return current?b.side:-1;
}
bool HeldInteractionVisual43() noexcept {return HeldInteractionVisualSideD78()>=0;}
bool HeldInteractionVisual43(int side) noexcept {return side>=0&&side<2&&HeldInteractionVisualSideD78()==side;}
