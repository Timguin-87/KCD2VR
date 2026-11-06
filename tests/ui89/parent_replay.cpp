// Compile against the exact V66.88 parent include directory, then against V66.89
// with -DUI89. This is a routing reproduction, NOT a game/Windows integration test.
#include "ui_menu_router.h"
#include <iostream>
using namespace kcd2vr;
int main(){
    struct Case{const char* name;ui_native_state::Kind kind;};
    const Case cases[]={{"Inventory",ui_native_state::Inventory},{"Shop",ui_native_state::Shop},
        {"LoadingScreen",ui_native_state::Loading},{"AlchemyBook",ui_native_state::AlchemyBook},
        {"BroadCraftingLifecycle",ui_native_state::Crafting},{"ApseCraftingContent",ui_native_state::CraftContent},
        {"ApseCraftingList",ui_native_state::CraftList},{"Progress",ui_native_state::Progress},{"Overlay",ui_native_state::Overlay}};
    std::cout<<"case,owner,full_frame\n";
    for(auto c:cases){
        ui_menu_router::Router router;
        ui_native_state::Snapshot s{};s.known=(1u<<ui_native_state::Count)-1;
        s.visible=ui_native_state::Bit(ui_native_state::Menu);s.epoch=1;router.Resolve(s);
        s.visible=0;s.epoch=2;router.Resolve(s);
        s.visible=ui_native_state::Bit(c.kind);s.epoch=3;
        auto owner=router.Resolve(s);
#ifdef UI89
        std::cout<<c.name<<','<<ui_menu_router::Name(owner)<<','<<router.Read().FullFrame()<<'\n';
#else
        const char* names[]={"GAMEPLAY","INVENTORY_MAP","SHOP","FRONTEND","PAUSE","CONTEXTUAL_FULLSCREEN"};
        std::cout<<c.name<<','<<names[static_cast<int>(owner)]<<','<<(owner!=ui_menu_router::Owner::None)<<'\n';
#endif
    }
}
