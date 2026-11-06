void TraceHudVisibility57c1(bool changed=true) noexcept {
    using namespace diagnostics::capture168;
    const auto now=GetTickCount64();const auto session=gate.session[unsigned(Domain::Hud)].load();
    static ui_hud_work59c7::CaptureBudget budget;
    if(!budget.Take(active(Domain::Hud,now),session,changed))return;
    HudHolder holder{};void* hud=nullptr;
    if(!g_display_verified170||!AcquireHudDirect(holder,hud)){ReleaseHudHolder(holder);return;}
    constexpr const char* names[]{"bc","bc.mc_StatsContainer","Stats","HealthBar","StaminaBar",
        "QAMWeapon","QAMFood","Cursor","CursorCross","CursorOutline","CombatCursorCross"};
    for(const auto* name:names){
        if(!active(Domain::Hud,GetTickCount64())||gate.session[unsigned(Domain::Hud)].load()!=session)break;
        void* clip=nullptr;void* get=nullptr;void* set=nullptr;alignas(16) unsigned char data[0x38]{};
        const bool ok=FindMovieClip(hud,name,clip,get,set)&&HudVisibilityBackend170::Call(get,clip,data);
        float x=0,y=0,xs=0,ys=0,a=0;std::uint16_t flags=0;
        if(ok){std::memcpy(&x,data,4);std::memcpy(&y,data+4,4);std::memcpy(&xs,data+12,4);std::memcpy(&ys,data+16,4);std::memcpy(&a,data+20,4);std::memcpy(&flags,data+52,2);}
        char m[420]{};std::snprintf(m,sizeof(m),"R59.16C7 HUD_VIS session=%llu snapshot=%u gen=%llu clip=%s read=%d flags=%X visible=%d alpha=%.2f xy=%.2f/%.2f scale=%.2f/%.2f hidden=%u vitalsPinned=%d",
            (unsigned long long)session,budget.Count(),(unsigned long long)HudGeneration(),name,ok,flags,ok&&(flags&0x400)?int(data[40]!=0):-1,double(a),double(x),double(y),double(xs),double(ys),g_transition_hidden170.load(),g_vitals_pin59c7.load());diagnostic_trace::Write(m);
    }
    ReleaseHudHolder(holder);
}
