void SetStatSim26(int mode) noexcept { // C26c
    const int m=std::clamp(mode,0,3);if(kcd2vr::dynamics177::statSim26.exchange(m)==m)return;
    static const char* const names[]{"OFF_REAL_STATS","LOW(str 6 agi 6 skill 2)","MEDIUM(str 14 agi 14 skill 12)","HIGH(str 25 agi 25 skill 25)"};
    char msg[200]{};std::snprintf(msg,sizeof(msg),"C26 SIMULATE_HENRY %s effect=WEAPON_FEEL_ONLY gameStats=UNCHANGED",names[m]);diagnostic_trace::Write(msg);
}
