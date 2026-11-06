void SetStowedBowAttachments59c15(const char* csv) noexcept {
    int n=0; if(csv){const char* p=csv; while(*p&&n<4){while(*p==' '||*p==','||*p=='\t')++p; if(!*p)break; std::size_t k=0;
        while(*p&&*p!=','&&k+1<sizeof(g_bow_att_names59c15[0])){if(*p!=' '&&*p!='\t')g_bow_att_names59c15[n][k++]=*p;++p;} g_bow_att_names59c15[n][k]=0; if(k)++n; while(*p&&*p!=',')++p;}}
    // R120: the stowed crossbow is a back model like the bow (Miki 6 Nov: "every back model hide").
    // Old INIs carry "weapon_bow" only, so the crossbow joins any list that has the bow.
    bool bowR120=false,crossbowR120=false;
    for(int i=0;i<n;++i){bowR120|=!std::strcmp(g_bow_att_names59c15[i],"weapon_bow");crossbowR120|=!std::strcmp(g_bow_att_names59c15[i],"weapon_crossbow");}
    if(bowR120&&!crossbowR120&&n<4){std::snprintf(g_bow_att_names59c15[n],sizeof(g_bow_att_names59c15[n]),"%s","weapon_crossbow");++n;}
    g_bow_att_count59c15.store(n,std::memory_order_release);
}
void SetHideStowedBow59c14(int mode,float pitchDeg) noexcept {
    g_hide_stowed_bow59c14.store(std::clamp(mode,0,2),std::memory_order_relaxed);
    g_hide_bow_pitch_deg59c14.store(std::clamp(pitchDeg,0.f,80.f),std::memory_order_relaxed);
    char m[160]{};std::snprintf(m,sizeof(m),"R59.16C10R14 STOWED_BOW mode=%d (0 native, 1 hidden when the HMD looks up > %.0f deg, 2 always hidden while stowed)",std::clamp(mode,0,2),double(std::clamp(pitchDeg,0.f,80.f)));diagnostic_trace::Write(m);
}
void SetHideStowedWeaponsHW(bool atStations,bool back) noexcept { // D33
    kcd2vr::stowed_hide_hw::hideAtStationsSetting.store(atStations,std::memory_order_relaxed);kcd2vr::stowed_hide_hw::hideBackSetting.store(back,std::memory_order_relaxed);
    char m[160]{};std::snprintf(m,sizeof(m),"D33 STOWED_HIDE stations=%d backWeapons=%d (front = hip swords + dagger stay visible)",atStations?1:0,back?1:0);diagnostic_trace::Write(m);
}
void SetHeadPitch59c14(float pitchDeg) noexcept {if(std::isfinite(pitchDeg))g_hmd_pitch_deg59c14.store(pitchDeg,std::memory_order_relaxed);}
