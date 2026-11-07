bool ApplyWholeReadableHud44c2(bool enabled,const ui_surface_geometry::Hud& geometry,float size,float width,float upper,float lower,float height ,float iconScale) noexcept {
    std::lock_guard<std::recursive_mutex> lock(g_hud_draw_mutex162r1);
    const auto fit59c7=ui_native_canvas::ComputeAdjustableReadable49(geometry,size,width,height,upper,lower,iconScale);
    std::uint64_t key59c7=1469598103934665603ull;
    for(float v:{fit59c7.sx,fit59c7.sy,fit59c7.tx,fit59c7.ty,fit59c7.width49,fit59c7.height49,fit59c7.upper49,fit59c7.lower49,fit59c7.iconScale51c3}){
        std::uint32_t bits=0;std::memcpy(&bits,&v,4);ui_hud_work59c7::Mix(key59c7,bits);
    }
    ui_hud_work59c7::Mix(key59c7,HudGeneration());ui_hud_work59c7::Mix(key59c7,g_canvas_lifetime.load());
    // D63: the matching companion authors the six message containers at 50%.
    // Every dynamically attached message inherits that scale from its first frame.
    // Do not also apply the old deferred C++ factor to the same container.
    SetNotificationsRequestedD80(false);
    if(enabled&&fit59c7.valid&&g_parent_ready59c7&&g_parent_key59c7==key59c7)return ApplyNotificationScaleR12();
    g_parent_ready59c7=false;
    g_canvas_requested=enabled;
    HudHolder holder{};void* hud=nullptr;
    if(!AcquireHudDirect(holder,hud)){ReleaseHudHolder(holder);return false;}
    const auto previous=g_canvas_live_hud.exchange(hud);
    if(previous!=hud)++g_canvas_lifetime;
    NativeCanvasBackend io{};io.hud=hud;io.generation=g_canvas_lifetime.load();
    g_canvas_report=g_canvas.Run(io,io.generation,enabled,ui_native_canvas::ComputeAdjustableReadable49(geometry,size,width,height,upper,lower,iconScale));
    const bool parentsComplete=g_canvas_report.complete;
    const bool notificationsOk=ApplyNotificationScaleR12();
    g_canvas_report.complete=g_canvas_report.complete&&notificationsOk;
    if(HudCaptureActive175()){
        char m[256]{};std::snprintf(m,sizeof(m),"R44C2 READABLE_PARENTS complete=%d size=%.2f resolved=%X applied=%X effects=NATIVE lookup=NATIVE_ALIAS_OR_PATH retries=EVENT_ONLY",g_canvas_report.complete,double(size),g_canvas_report.resolved,g_canvas_report.written);
        diagnostic_trace::Write(m);
    }
    ReleaseHudHolder(holder);
    // Canvas lifetime can change on first acquisition; the next event may perform
    // one fresh check, but the cache never conceals a failed transaction.
    if(enabled&&parentsComplete){g_parent_key59c7=key59c7;g_parent_ready59c7=true;}
    return g_canvas_report.complete;
}
