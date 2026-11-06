// Shared by the actual private native getter and its focused callback fixture.
// The channel joins copied native cleanup identity before any binder callback.
static bool DoorCurveMessage66(const char* name,std::uint64_t now,std::uint32_t& value) noexcept {
    if(std::strncmp(name,"kcd2vr_door_curve_",17))return false;
    value=0;door_curve_signal66::Query q{};const auto handler=g_curve_handler66.load(std::memory_order_acquire);
    if(door_curve_signal66::parse(name,q)&&g_channel19c4r4.curve66(q,now)&&handler){const auto status=handler(q,now);if(status<=2)value=status;}
    return true;
}
