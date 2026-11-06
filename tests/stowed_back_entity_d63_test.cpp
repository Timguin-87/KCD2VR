// R123: R121 moved the two INI-only hide globals to kcd2vr::stowed_hide_hw (live GEAR page 2 switches).
#define g_hide_back_hw kcd2vr::stowed_hide_hw::hideBackSetting
#define g_hide_at_stations_hw kcd2vr::stowed_hide_hw::hideAtStationsSetting
// Executes the same production tick and post-native attachment callback as D44.
#define main retained_stowed_d44_main
#include "stowed_attachments_d44_test.cpp"
#undef main
int main(int argc,char** argv) {
    (void)retained_stowed_d44_main(1,argv);
    fixture();childEntitiesD56.clear();childHiddenD56.clear();
    kcd2vr::activity_input163::witness175.active[0]=0;g_hide_back_hw=true;
    g_henry_char_attachment_manager=reinterpret_cast<void*>(character+8);
    const auto att=reinterpret_cast<std::uintptr_t>(&attachments[3]);
    const std::uintptr_t object=0x710000,child=0x810000,name=0x910000;
    put(att,WhBase()+0x3A657C0);put(att+0x28,character+8);put(att+0x20,object);put(att+0x10,name);
    const char* slotName="weapon_bow";
    for(unsigned j=0;j<=std::strlen(slotName);++j)put(name+j,slotName[j]);
    put(object,WhBase()+0x3A68F68);put(object+8,std::uint32_t(556));
    childEntitiesD56[556]=reinterpret_cast<void*>(child);childHiddenD56[reinterpret_cast<void*>(child)]=false;
    tick();
    check((attachments[3].flags&0x70000)==0x70000,"ordinary back holder remains hidden");
    check(childHiddenD56[reinterpret_cast<void*>(child)],"ordinary back entity follows the same hide policy as its holder");
    childHiddenD56[reinterpret_cast<void*>(child)]=false;
    const auto before=entityHideCallsD56;
    StationWeaponAfterAttachmentD56(reinterpret_cast<void*>(object),&attachments[3]);
    check(childHiddenD56[reinterpret_cast<void*>(child)]&&entityHideCallsD56==before+1,"native attack callback reshow is suppressed immediately outside crafting");
    StationWeaponAfterAttachmentD56(reinterpret_cast<void*>(object),&attachments[3]);
    check(entityHideCallsD56==before+1,"an already hidden child is not written on every callback");
    put(att+0x20,std::uintptr_t(0));tick();
    check(!childHiddenD56[reinterpret_cast<void*>(child)],"draw detach restores only the hide owned by this exact back binding");
    put(att+0x20,object);tick();
    check(childHiddenD56[reinterpret_cast<void*>(child)],"re-stow earns fresh child ownership");
    g_hide_back_hw=false;tick();
    check(!childHiddenD56[reinterpret_cast<void*>(child)],"disabling back hide restores the exact owned entity");
    childHiddenD56[reinterpret_cast<void*>(child)]=true;g_hide_back_hw=true;tick();g_hide_back_hw=false;tick();
    check(childHiddenD56[reinterpret_cast<void*>(child)],"native-prehidden child is never adopted or exposed");
    childHiddenD56[reinterpret_cast<void*>(child)]=false;g_hide_back_hw=true;
    const auto old=entityHideCallsD56;const char* hand="hand_left";
    for(unsigned j=0;j<=std::strlen(hand);++j)put(name+j,hand[j]);
    StationWeaponAfterAttachmentD56(reinterpret_cast<void*>(object),&attachments[3]);
    check(entityHideCallsD56==old&&!childHiddenD56[reinterpret_cast<void*>(child)],"held hand sockets remain outside ordinary back-slot suppression");
    // R120: per-frame entity enforcement between the 250 ms holder ticks.
    for(unsigned j=0;j<=std::strlen(slotName);++j)put(name+j,slotName[j]);
    childHiddenD56[reinterpret_cast<void*>(child)]=false;g_hide_back_hw=true;traces.clear();g_station_notes_r120={};tick();
    check(childHiddenD56[reinterpret_cast<void*>(child)],"R120 back entity hidden on the holder tick");
    bool namedR120=false;for(const auto& l:traces)namedR120|=l.find("R120 STOWED_ENTITY slot=weapon_bow entityId=556 result=HIDDEN")!=std::string::npos;
    check(namedR120,"R120 names the entity route once in KCD2VR.log");
    childHiddenD56[reinterpret_cast<void*>(child)]=false; // the game re-shows it (rebind / weapon action)
    tick(10);
    check(childHiddenD56[reinterpret_cast<void*>(child)],"R120 game re-show is hidden again on the next frame, not 250 ms later");
    bool reshowR120=false;for(const auto& l:traces)reshowR120|=l.find("R120 STOWED_ENTITY_RESHOW slot=weapon_bow entityId=556 count=1")!=std::string::npos;
    check(reshowR120,"R120 re-show is counted");
    const std::uintptr_t object2=0x720000;put(object2,WhBase()+0x3A68F68);put(object2+8,std::uint32_t(556));put(att+0x20,object2);
    const auto showsR120=entityShowCallsD56;tick(10);tick();
    check(childHiddenD56[reinterpret_cast<void*>(child)]&&entityShowCallsD56==showsR120,"R120 same entity rebound on the same back socket stays hidden without a show blink");
    put(att+0x20,std::uintptr_t(0));tick(10);
    check(!childHiddenD56[reinterpret_cast<void*>(child)],"R120 drawn from the back: entity shown again on the next frame");
    tick();
    check(!childHiddenD56[reinterpret_cast<void*>(child)],"R120 a back socket without our entity stays shown on the holder tick");
    // R120 draw guard: drawing from the back while the guard is armed must not show the entity at the native hand pose.
    put(att+0x20,object);g_hide_back_hw=true;tick();
    check(childHiddenD56[reinterpret_cast<void*>(child)],"R120 guard fixture: back entity hidden");
    kcd2vr::draw_guard_r120::guards[0].id.store(556);
    const auto showsGuard=entityShowCallsD56;put(att+0x20,std::uintptr_t(0));tick(10);
    check(childHiddenD56[reinterpret_cast<void*>(child)]&&entityShowCallsD56==showsGuard,"R120 back-to-hand draw: D56 hands its hidden write to the draw guard, no native-pose show");
    check(kcd2vr::draw_guard_r120::guards[0].ours.load(),"R120 the draw guard now owns the show at the first VR placement");
    kcd2vr::draw_guard_r120::guards[0].id.store(0);kcd2vr::draw_guard_r120::guards[0].ours.store(false);
    childHiddenD56[reinterpret_cast<void*>(child)]=false;
    namespace dg=kcd2vr::draw_guard_r120;
    check(dg::decide(false,1000,0,1100,true)==dg::Step::Hide,"R120 guard hides a visible drawn weapon before any VR placement");
    check(dg::decide(true,1000,1010,1020,false)==dg::Step::Show,"R120 guard shows at the first written VR placement");
    check(dg::decide(false,1000,1010,1161,false)==dg::Step::Show,"R120 guard shows 150 ms after the first hand callback whatever happened");
    check(dg::decide(false,1000,0,2001,false)==dg::Step::Show,"R120 guard without any hand callback shows after 1 s");
    check(dg::decide(false,1000,1010,1030,true,2,true)==dg::Step::Show,"R121 grip ready: hide capped at 2 native frames after the first hand callback");
    check(dg::decide(false,1000,1010,1030,true,1,true)==dg::Step::Hide,"R121 grip ready: still hidden in the bind frame (placement comes next callback)");
    check(dg::decide(false,1000,1010,1030,true,5,false)==dg::Step::Hide,"R121 grip still learning: placement impossible, keep the R120 wait");
    check(dg::decide(false,1000,0,1030,true,9,true)==dg::Step::Hide,"R121 frame cap counts only after the first hand callback");
    dg::guards[1].id.store(77);dg::markPlaced(77);check(dg::guards[1].placed.load(),"R120 placement write marks the guard");dg::guards[1].id.store(0);dg::guards[1].placed.store(false);
    g_hide_back_hw=false;tick(10);tick();
    std::printf("D63 stowed entity routing %u/%u; native leaves=SIMULATED\n",checks-failures,checks);
    return failures?1:0;
}
