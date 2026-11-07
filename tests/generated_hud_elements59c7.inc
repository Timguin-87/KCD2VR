bool ApplyUniformHudElements171(float compass,float compassY,float stats,float lower,float lowerY,float spread,float statsX,float statsY) noexcept {
    std::lock_guard<std::recursive_mutex> lock(g_hud_draw_mutex162r1);
    const std::array<ui_hud_elements171::Adjustment,4> targets{{
        {std::clamp(compass,.15f,1.50f),0,-compassY},{std::clamp(stats,.15f,1.50f),statsX,-statsY},
        {std::clamp(lower,.15f,2.f),-spread,-lowerY},{std::clamp(lower,.15f,2.f),spread,-lowerY}}};
    std::uint64_t key=1469598103934665603ull;
    ui_hud_work59c7::Mix(key,HudGeneration());ui_hud_work59c7::Mix(key,g_canvas_lifetime.load());
    ui_hud_work59c7::Mix(key,g_children59c7.Epoch());
    for(auto a:targets)for(float f:{a.scale,a.x,a.y}){std::uint32_t b=0;std::memcpy(&b,&f,4);ui_hud_work59c7::Mix(key,b);}
    static std::uint64_t resultKey=0;static bool resultOk=false;
    if(resultKey!=key){resultKey=key;resultOk=false;}
    if(!g_elements_retry59c7.Due(key,GetTickCount64()))return resultOk;
    HudHolder holder{};void* hud=nullptr;
    if(!g_display_verified170||!AcquireHudDirect(holder,hud)){ReleaseHudHolder(holder);return false;}
    NativeCanvasBackend backend{};backend.hud=hud;backend.generation=g_canvas_lifetime.load();
    const bool ok=g_elements171.Run(backend,backend.generation,true,targets);
    const auto report=g_elements171.LastReport();resultOk=ok;g_elements_retry59c7.Complete(ok&&!report.pending);
    char msg[380]{};std::snprintf(msg,sizeof(msg),"R59.16C7 HUD_ELEMENTS compass=%.2f health=%.2f quickSlots=%.2f result=%d resolved=%X applied=%X pending=%X queried=%X statsResolved=%d animation=NATIVE retries=BOUNDED_EVENT",
        double(compass),double(stats),double(lower),ok,report.resolved,report.applied,report.pending,report.queried,(report.resolved&2)!=0);
    diagnostic_trace::Write(msg);ReleaseHudHolder(holder);return ok;
}
