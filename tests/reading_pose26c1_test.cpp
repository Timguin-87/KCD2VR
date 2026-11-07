#include "vrik/reading_pose26c1.h"
#include <cassert>
#include <cstdio>
using namespace kcd2vr;
int main() {
    ui_menu_router::Decision ui{};
    auto permit=[&](bool witnessed=true,bool selected=true,bool theater=false,
                   bool matrix=false,bool menu=false,bool dialogue=false,
                   bool cinematic=false,bool reference=false) {
        return reading_pose26c1::allow(witnessed,selected,theater,matrix,menu,
                                     dialogue,cinematic,reference,ui);
    };
    assert(permit()); // exact reading witness, free immersive UI
    ui.owner=ui_menu_router::Owner::NativeContext;
    ui.input=ui_menu_router::Input::PreserveNative;
    assert(permit()); // reading input stays native, visuals may run
    for(auto owner:{ui_menu_router::Owner::InventoryMap,ui_menu_router::Owner::Shop,
        ui_menu_router::Owner::Frontend,ui_menu_router::Owner::Pause,
        ui_menu_router::Owner::ContextualMenu,ui_menu_router::Owner::CraftingMenu,
        ui_menu_router::Owner::LoadingScreen,ui_menu_router::Owner::GameOver}) {
        ui.owner=owner;assert(!permit()); // a retained Reading witness cannot overrule UI
    }
    ui={};ui.uncertain=1;assert(!permit());ui={};
    assert(!permit(false));assert(!permit(true,false));assert(!permit(true,true,true));
    assert(!permit(true,true,false,true));assert(!permit(true,true,false,false,true));
    assert(!permit(true,true,false,false,false,true));
    assert(!permit(true,true,false,false,false,false,true));
    assert(!permit(true,true,false,false,false,false,false,true));
    assert(permit()); // fresh witnessed read after native/menu exit
    std::puts("PASS reading visual ownership: native input retained; all real UI/cinematic/reference/unavailable owners refuse");
}
