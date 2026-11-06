void SetInteractionHandContext158(int side,bool passive,float x,float y,float z,float maxReach) noexcept {
    InteractionHandContext158 c{};const auto now=std::uint64_t(GetTickCount64());
    if(side>=0&&side<2&&std::isfinite(x)&&std::isfinite(y)&&std::isfinite(z)&&std::isfinite(maxReach)&&maxReach>0)
        c={side,passive,true,x,y,z,maxReach,now};
    g_interaction_hand_context158.publish(c);
}
