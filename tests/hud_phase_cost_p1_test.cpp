#include "../src/flashui_probe.cpp"
#include <cassert>
#include <cstdio>
namespace kcd2vr::diagnostic_trace{void Write(const char*)noexcept{}}
#ifdef _MSC_VER
namespace kcd2vr::retail_build112 {bool Verify(std::uintptr_t)noexcept{return false;}bool Ready(std::uintptr_t)noexcept{return false;}}
namespace kcd2vr::ui_native_state {void ObserveElement(void*)noexcept{std::abort();}}
namespace kcd2vr::action_map_probe {bool WeaponQamOpen171()noexcept{return false;}bool FoodQamOpen171()noexcept{return false;}}
#endif
namespace {
unsigned calls=0;bool measurableNativeCost=false;
bool Display(void*,void* info){++calls;if(measurableNativeCost){const auto end=std::chrono::steady_clock::now()+std::chrono::milliseconds(1);while(std::chrono::steady_clock::now()<end){}}float x=100;std::memcpy(static_cast<unsigned char*>(info)+12,&x,4);std::memcpy(static_cast<unsigned char*>(info)+16,&x,4);return true;}
}
int main(){
    using namespace kcd2vr;using namespace flashui_probe;using diagnostics::capture168::Domain;
    const auto now=hookcost178r22::tickNow();
    diagnostics::capture168::gate.mask.store(0);
    std::atomic<std::uint64_t> counter{77};
    for(unsigned n=0;n<1000;++n){CanvasPhaseCostP1 p;assert(!p.capture.serial);assert(p.begin==std::chrono::steady_clock::time_point{});assert(!p.Record(counter));}
    assert(counter.load()==77);
    NativeCanvasBackend io;io.hud=reinterpret_cast<void*>(0x1000);io.generation=2;
    g_canvas_live_hud.store(io.hud);g_canvas_lifetime.store(2);
    io.handles[0].clip=reinterpret_cast<void*>(0x2000);io.handles[0].get=io.handles[0].set=reinterpret_cast<void*>(&Display);
    g_canvas_get_us178r27.store(123);g_canvas_set_us178r27.store(456);
    ui_native_canvas::Geometry geometry{};
    for(unsigned n=0;n<1000;++n){assert(io.Get(0,geometry));assert(io.Set(0,geometry));}
    assert(calls==2000&&g_canvas_get_us178r27.load()==123&&g_canvas_set_us178r27.load()==456);
    // D61: the existing manual canvas owner has the same finite instrumentation gate.
    // Native calls and bytes remain live while optional phase measurement is OFF.
    g_corrected36.store(true);alignas(16) unsigned char manualInfo[0x38]{};
    for(unsigned n=0;n<1000;++n){assert(ManualDisplay36(reinterpret_cast<void*>(&Display),io.handles[0].clip,manualInfo,true));assert(ManualDisplay36(reinterpret_cast<void*>(&Display),io.handles[0].clip,manualInfo,false));}
    assert(calls==4000&&g_canvas_get_us178r27.load()==123&&g_canvas_set_us178r27.load()==456);

    diagnostics::capture168::gate.select(Domain::Bug,true,now);
    CanvasPhaseCostP1 active;assert(active.capture.serial&&active.begin!=std::chrono::steady_clock::time_point{});
    active.begin-=std::chrono::milliseconds(1);assert(active.Record(counter));assert(counter.load()>=1077);
    measurableNativeCost=true;
    assert(io.Get(0,geometry)&&io.Set(0,geometry));
    assert(ManualDisplay36(reinterpret_cast<void*>(&Display),io.handles[0].clip,manualInfo,true));
    assert(ManualDisplay36(reinterpret_cast<void*>(&Display),io.handles[0].clip,manualInfo,false));
    measurableNativeCost=false;
    assert(calls==4004&&g_canvas_get_us178r27.load()>=2123&&g_canvas_set_us178r27.load()>=2456);

    const auto after=counter.load();CanvasPhaseCostP1 expired;assert(expired.capture.serial);
    diagnostics::capture168::gate.select(Domain::Bug,false,hookcost178r22::tickNow());assert(!expired.Record(counter)&&counter.load()==after);
    diagnostics::capture168::gate.select(Domain::Bug,true,hookcost178r22::tickNow());CanvasPhaseCostP1 replaced;
    diagnostics::capture168::gate.select(Domain::Bug,false,hookcost178r22::tickNow());diagnostics::capture168::gate.select(Domain::Bug,true,hookcost178r22::tickNow());
    assert(!replaced.Record(counter)&&counter.load()==after);
    diagnostics::capture168::gate.select(Domain::Bug,false,hookcost178r22::tickNow());
    diagnostics::capture168::gate.select(Domain::Hud,true,hookcost178r22::tickNow());CanvasPhaseCostP1 hud;assert(hud.capture.serial);
    hud.begin-=std::chrono::milliseconds(1);assert(hud.Record(counter)&&counter.load()>after);
    diagnostics::capture168::gate.select(Domain::Hud,false,hookcost178r22::tickNow());
    std::puts("PASS complete production FlashUI TU: 1000OFFphase samples0clock-starts/0counterwrites; 4000native and manual Get/Set leaves unchanged; ON accounting4native calls; finite current/expired/replaced capture leases");
}
