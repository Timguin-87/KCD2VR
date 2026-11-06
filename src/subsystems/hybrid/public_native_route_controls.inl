void SetGeometryLiveAction178r35(bool on) noexcept {
    nhp134::geom27::g_live178r35.store(true,std::memory_order_relaxed);
    if(!on)diagnostic_trace::Write("V66.178R51C3 VrGeometryLiveAction=0 ignored: contact-only mode requires the native rules/action route");
}
bool GeometryLiveAction178r35() noexcept {return nhp134::geom27::g_live178r35.load(std::memory_order_relaxed);}
void SetGeometryTelegraph178r34(bool on) noexcept {nhp134::geom27::g_telegraph178r34.store(on,std::memory_order_relaxed);}
void GeometryTelegraph178r34(int phase,int zone,std::uint64_t now) noexcept {nhp134::geom27::TelegraphPublish178r34(phase,zone,now);}
