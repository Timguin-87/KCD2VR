#include "../src/flashui_probe.cpp"
#include "../src/ui_hud_experiment36c5.h"
#include <cassert>
#include <cstdio>
namespace kcd2vr::diagnostic_trace{void Write(const char*)noexcept{}}
#ifdef _MSC_VER
// Explicit unused native leaves retained by MSVC. No retail code is executed.
namespace kcd2vr::retail_build112 {
bool Verify(std::uintptr_t) noexcept {return false;}
bool Ready(std::uintptr_t) noexcept {return false;}
}
namespace kcd2vr::ui_native_state {void ObserveElement(void*) noexcept {std::abort();}}
namespace kcd2vr::action_map_probe {
bool WeaponQamOpen171() noexcept {return false;}
bool FoodQamOpen171() noexcept {return false;}
}
#endif
namespace {
unsigned originalCalls175=0,destroys175=0;bool delivered175=false;
unsigned displayCalls54=0;float deliveredAlpha54=0;std::array<unsigned char,0x38> deliveredInfo54{};
std::uint64_t observedNotificationEpochD59R1=0;
bool Display54(void*,void* info){observedNotificationEpochD59R1=kcd2vr::flashui_probe::g_notification_children_r12.Epoch();++displayCalls54;std::memcpy(deliveredInfo54.data(),info,0x38);std::memcpy(&deliveredAlpha54,deliveredInfo54.data()+0x14,4);return true;}
void* returned175=nullptr;
void* Find175(void*,const char*,void*){return returned175;}
void Destroy175(void*){++destroys175;}
void Visible175(void*,bool request){++originalCalls175;delivered175=request;}
template<class T,std::size_t N>void Put175(std::array<unsigned char,N>&a,unsigned offset,T value){
    assert(offset+sizeof(value)<=a.size());std::memcpy(a.data()+offset,&value,sizeof(value));}
}
int main(){
    using namespace kcd2vr;using namespace flashui_probe;
    alignas(16)std::array<unsigned char,0x70> wrapper{};
    alignas(16)std::array<unsigned char,0x20> handle{};
    alignas(16)std::array<unsigned char,0x200> object{},vtable{};
    const std::uintptr_t base=0x50000000;
    const auto wp=reinterpret_cast<std::uintptr_t>(wrapper.data());
    const auto hp=reinterpret_cast<std::uintptr_t>(handle.data());
    const auto op=reinterpret_cast<std::uintptr_t>(object.data());
    Put175(wrapper,0,base+0x3A55BA0);Put175(wrapper,0x10,8u);Put175(wrapper,0x18,hp);
    Put175(handle,8,op);Put175(object,0,reinterpret_cast<std::uintptr_t>(vtable.data()));
    Put175(object,0xD0,hp);Put175(vtable,0x198,base+0x1C51240);
    auto*hud=reinterpret_cast<void*>(0x1010);g_last_hud.store(hud);g_hud_generation.store(5);
    g_hud_native_base175.store(base);g_hud_repair_ready175.store(true);g_hud_repair175.store(true);
    visibility175::find=Find175;visibility175::destroy=Destroy175;visibility175::visible=Visible175;
    returned175=wrapper.data();ui_hud_visibility175::Key key{};
    assert(ReadHudKey175(wrapper.data(),hud,5,key));assert(key.object==op&&key.handle==hp);
    g_transition_hidden170.store(ui_contextual_hud162r1::HideCompass163);
    NativeFindHudChild175(hud,"Compass",nullptr);
    diagnostics::capture168::gate.select(diagnostics::capture168::Domain::Hud,false,hostTickCount64);
    const auto calls=g_native_vis_calls175.load();
    for(unsigned i=0;i<1000;++i)NativeHudVisible175(object.data(),true);
    assert(originalCalls175==1000&&!delivered175&&g_native_vis_calls175.load()==calls);
    bool native=false;assert(g_native_visibility175.NativeMeaning(0,key,false,native)&&native);
    g_hud_owned_write175=1;NativeHudVisible175(object.data(),false);g_hud_owned_write175=0;
    assert(g_native_visibility175.NativeMeaning(0,key,false,native)&&native); // internal hide is not native intent
    NativeHudVisible175(object.data(),false);
    assert(g_native_visibility175.NativeMeaning(0,key,true,native)&&!native);
    diagnostics::capture168::gate.select(diagnostics::capture168::Domain::Hud,true,hostTickCount64);
    NativeHudVisible175(object.data(),true);assert(!delivered175&&g_native_vis_suppressed175.load()==1);
    g_hud_repair175.store(false);NativeHudVisible175(object.data(),true);assert(delivered175);
    g_hud_repair175.store(true);
    // Exact wrapper, native back-link and writer-vslot identity must all agree.
    Put175(object,0xD0,hp+8);NativeHudVisible175(object.data(),true);assert(delivered175);
    Put175(object,0xD0,hp);Put175(vtable,0x198,base+0x1C51241);
    NativeHudVisible175(object.data(),true);assert(delivered175);
    Put175(vtable,0x198,base+0x1C51240);NativeHudVisible175(object.data(),true);assert(!delivered175);
    NativeDestroyHudWrapper175(wrapper.data());assert(destroys175==1);
    NativeHudVisible175(object.data(),true);assert(delivered175); // no stale address match
    NativeFindHudChild175(hud,"Compass",nullptr);NativeHudVisible175(object.data(),true);assert(!delivered175);
    g_hud_generation.store(6);NativeHudVisible175(object.data(),true);assert(delivered175);
    NativeDestroyHudWrapper175(wrapper.data());
    g_transition_hidden170.store(0);g_corrected36.store(true);g_vitals_pin59c7.store(true);
    NativeFindHudChild175(hud,"Stats",nullptr);auto statsEpoch=g_children59c7.Epoch();
    NativeFindHudChild175(hud,"Stats",nullptr);assert(g_children59c7.Epoch()==statsEpoch);
    auto count=originalCalls175;NativeHudVisible175(object.data(),false);
    assert(originalCalls175==count+1&&delivered175); // one original setter, explicit Stats display
    ui_hud_visibility175::Key statsKey{};assert(ReadHudKey175(wrapper.data(),hud,6,statsKey));
    bool statsNative=true;assert(g_native_visibility175.NativeMeaning(1,statsKey,true,statsNative)&&!statsNative);
    // Contextual preference can own exact Stats visibility even with ordinary
    // corrected HUD off. The hook never writes alpha or enables other clips.
    g_corrected36.store(false);NativeHudVisible175(object.data(),false);assert(delivered175);
    g_corrected36.store(true);
    g_vitals_pin59c7.store(false);NativeHudVisible175(object.data(),false);assert(!delivered175);
    g_vitals_pin59c7.store(true);g_transition_hidden170.store(ui_contextual_hud162r1::HideLower163);
    NativeHudVisible175(object.data(),true);assert(!delivered175); // explicit hide still wins
    // C19C2: native alpha may be restored independently of native _visible.
    // The exact visible writer must respect a palm-hidden Stats child even if
    // an earlier vital pin is still observed by the render/native thread.
    using namespace ui_contextual_hud162r1;
    g_transition_hidden170.store(ui_hud_experiment36c5::VisibilityMask(HideCrosshair163,{true,false,false}));
    g_corrected36.store(false); // checkbox-only immersive; no F2 mode required
    NativeHudVisible175(object.data(),true);assert(!delivered175);
    NativeHudVisible175(object.data(),false);assert(!delivered175);
    g_transition_hidden170.store(ui_hud_experiment36c5::VisibilityMask(HideCrosshair163,{true,false,true}));
    NativeHudVisible175(object.data(),true);assert(delivered175);
    NativeDestroyHudWrapper175(wrapper.data());NativeFindHudChild175(hud,"Compass",nullptr);
    NativeHudVisible175(object.data(),true);assert(!delivered175); // only right palm was revealed
    g_transition_hidden170.store(ui_hud_experiment36c5::VisibilityMask(HideCrosshair163,{true,true,false}));
    NativeHudVisible175(object.data(),true);assert(delivered175);
    NativeDestroyHudWrapper175(wrapper.data());NativeFindHudChild175(hud,"QAMWeapon",nullptr);
    g_transition_hidden170.store(ui_hud_experiment36c5::VisibilityMask(HideCrosshair163,{true,false,false}));
    NativeHudVisible175(object.data(),true);assert(delivered175); // Palm mask cannot hide native selector
    NativeHudVisible175(object.data(),false);assert(!delivered175); // no unconditional force-show
    for(const char* name:{"Buffs","Crime","Wanted","Trespassing"}){
        NativeDestroyHudWrapper175(wrapper.data());NativeFindHudChild175(hud,name,nullptr);
        g_transition_hidden170.store(ui_hud_experiment36c5::VisibilityMask(HideStatus163,{true,true,true}));
        NativeHudVisible175(object.data(),true);assert(!delivered175); // palms never unhide user-hidden status
    }
    // Explicit recording ownership follows HUD, not combat. Nothing is logged
    // when F2 is off, and observing it never changes the gameplay owner.
    assert(diagnostics::capture168::domain("R44C2 PALM_ALPHA active=1")==diagnostics::capture168::Domain::Hud);
    assert(diagnostics::capture168::domain("R44C2 PALM_GAZE reveal=0/0")==diagnostics::capture168::Domain::Hud);
    visibility175::display54=Display54;
    NativeDestroyHudWrapper175(wrapper.data());NativeFindHudChild175(hud,"Compass",nullptr);
    alignas(16) std::array<unsigned char,0x38> info54{};const std::uint16_t alphaFlag54=0x20;const float nativeAlpha54=37.f;
    std::memcpy(info54.data()+0x34,&alphaFlag54,2);std::memcpy(info54.data()+0x14,&nativeAlpha54,4);info54[3]=91;info54[9]=42;
    g_transition_hidden170.store(Left);SetHudSemanticOwner54(true,false,false,false);
    assert(NativeHudDisplay54(wrapper.data(),info54.data())&&displayCalls54==1&&deliveredAlpha54==0);
    assert(deliveredInfo54[3]==91&&deliveredInfo54[9]==42);
    for(unsigned i=0;i<info54.size();++i)if(i<0x14||i>=0x18)assert(deliveredInfo54[i]==info54[i]);
    g_transition_hidden170.store(Right);SetHudSemanticOwner54(true,true,false,false);
    NativeHudDisplay54(wrapper.data(),info54.data());assert(deliveredAlpha54==100);
    NativeHudVisible175(object.data(),false);assert(delivered175);
    ui_hud_visibility175::Key compassKey54{};assert(ReadHudKey175(wrapper.data(),hud,6,compassKey54));float remembered54=0;
    assert(g_native_visibility175.NativeAlphaMeaning54(0,compassKey54,100,remembered54)&&remembered54==37);
    g_hud_owned_write175=1;NativeHudDisplay54(wrapper.data(),info54.data());g_hud_owned_write175=0;assert(deliveredAlpha54==37);
    NativeDestroyHudWrapper175(wrapper.data());NativeFindHudChild175(hud,"Cursor",nullptr);
    g_transition_hidden170.store(0);SetHudSemanticOwner54(false,false,false,true);
    NativeHudVisible175(object.data(),false);assert(delivered175);
    NativeHudDisplay54(wrapper.data(),info54.data());assert(deliveredAlpha54==100);
    SetHudSemanticOwner54(false,false,false,false);g_transition_hidden170.store(HideCrosshair163);
    NativeHudVisible175(object.data(),true);assert(!delivered175);
    NativeHudDisplay54(wrapper.data(),info54.data());assert(deliveredAlpha54==37);
    // Root/effects are never alpha gates, and a replaced wrapper cannot borrow a cursor key.
    for(const char* name:{"MasterSwitch","fader","vignette","bbc"}){
        NativeDestroyHudWrapper175(wrapper.data());NativeFindHudChild175(hud,name,nullptr);
        SetHudSemanticOwner54(true,false,false,false);g_transition_hidden170.store(Left|Right);
        NativeHudDisplay54(wrapper.data(),info54.data());assert(deliveredAlpha54==37);
    }
    // D59R1: execute the real native display detour. The original callback must
    // run exactly once BEFORE the retry epoch becomes dirty; our own writes and
    // alpha-only native writes must not requeue notification scaling.
    NativeDestroyHudWrapper175(wrapper.data());
    for(unsigned i=0;i<kNotificationNamesR12.size();++i){
        NativeFindHudChild175(hud,kNotificationNamesR12[i],nullptr);
        const auto epoch=g_notification_children_r12.Epoch();
        const auto revision=g_notification_children_r12.Revision(i);
        const auto callsBefore=displayCalls54;
        const std::uint16_t flags=0x18;std::memcpy(info54.data()+0x34,&flags,2);
        NativeHudDisplay54(wrapper.data(),info54.data());
        assert(displayCalls54==callsBefore+1&&observedNotificationEpochD59R1==epoch);
        assert(g_notification_children_r12.Epoch()==epoch+1&&g_notification_children_r12.Revision(i)==revision);
        assert(deliveredInfo54==info54); // observation cannot change native fields
        auto stable=g_notification_children_r12.Epoch();
        g_hud_owned_write175=1;NativeHudDisplay54(wrapper.data(),info54.data());g_hud_owned_write175=0;
        assert(g_notification_children_r12.Epoch()==stable);
        std::memcpy(info54.data()+0x34,&alphaFlag54,2);NativeHudDisplay54(wrapper.data(),info54.data());
        assert(g_notification_children_r12.Epoch()==stable);
        NativeDestroyHudWrapper175(wrapper.data());stable=g_notification_children_r12.Epoch();
        std::memcpy(info54.data()+0x34,&flags,2);NativeHudDisplay54(wrapper.data(),info54.data());
        assert(g_notification_children_r12.Epoch()==stable);
    }
    SetHudExperiment36(false,false);assert(!g_vitals_pin59c7.load());
    std::puts("PASS complete production flashui native-resolution/destruction/visibility detours with synthetic identities + original-call spy; zero OFF diagnostic increments; native bytes verified separately");
}
