void SetBodyFollow128(bool on) noexcept {g_body_follow128.store(on,std::memory_order_release);diagnostic_trace::Write(on?"V66.128-BODY_FOLLOW ON (pelvis follow + leg IK)":"V66.128-BODY_FOLLOW OFF (pelvis/legs native)");}
bool BodyFollow128() noexcept {return g_body_follow128.load(std::memory_order_acquire);}
void SetChestTwist128(bool natural,bool handBias) noexcept {g_chest_natural128.store(natural);g_chest_hand_bias128.store(handBias);}
