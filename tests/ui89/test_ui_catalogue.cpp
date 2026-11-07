#include "ui_native_catalogue.h"
#include "ui_menu_router.h"
#include "ui_visibility_registry.h"
#include "ui_menu_policy.h"
#include "ui_surface_geometry.h"
#include <cstdlib>
#include <iostream>
#include <random>
#include <set>
#include <string>

using namespace kcd2vr;
using namespace ui_native_state;
using ui_menu_router::Decision;
using ui_menu_router::Input;
using ui_menu_router::Owner;
using ui_menu_router::Presentation;
using ui_native_catalogue::Role;
unsigned checks=0;
#define CHECK(x) do{++checks;if(!(x)){std::cerr<<"FAIL line "<<__LINE__<<": " #x "\n";std::abort();}}while(0)
struct World {
    ui_visibility_registry::Registry registry;
    ui_menu_router::Router router;
    Snapshot s;
    void Event(Kind k,bool visible,std::uintptr_t owner=0,bool destroy=false){
        if(!owner) owner=(static_cast<std::uintptr_t>(k)+1u)*256u;
        CHECK(registry.Apply({owner,unsigned(k),visible,destroy},ui_native_catalogue::OwnershipMask));
        const auto state=registry.Read(); s.known=state.known;s.visible=state.visible;s.epoch=state.epoch;
        router.Resolve(s);
    }
    const Decision& D()const {return router.Read();}
    World(){CHECK(router.Resolve(s)==Owner::Frontend);Event(Menu,true);Event(Menu,false);CHECK(D().owner==Owner::None);}
};
int main(){
    CHECK(Count==30);CHECK(ui_native_catalogue::Entries.size()==28);
    CHECK(Bit(Count)==0);CHECK(Bit(Pickpocketing)==(std::uint32_t{1}<<29));
    std::set<unsigned> kinds;std::set<std::string> names;unsigned full=0,fixed=0,centered=0;
    for(const auto& e:ui_native_catalogue::Entries){
        CHECK(kinds.insert(unsigned(e.kind)).second);CHECK(names.insert(e.name).second);
        CHECK(ui_native_catalogue::Find(e.name)==&e);CHECK(ui_native_catalogue::Find(e.kind)==&e);
        CHECK(e.stage_width>0&&e.stage_height>0);
        using A=ui_native_catalogue::Alignment;
        full+=e.alignment==A::FullScreen;fixed+=e.alignment==A::FixedTexture;centered+=e.alignment==A::CenteredDynamic;
        World w;w.Event(e.kind,true);const auto& d=w.D();
        switch(e.role){
        case Role::Hud:case Role::ApseChild:case Role::WorldTexture:case Role::FieldEffect:case Role::PassiveOverlay:
            CHECK(d.owner==Owner::None);CHECK(!d.FullFrame());CHECK(!d.MenuAdapter());CHECK(!d.BlocksWorldInteraction());break;
        case Role::ApseRoot:CHECK(d.owner==Owner::InventoryMap);CHECK(d.presentation==Presentation::ApseTheater);CHECK(d.PointerAllowed());break;
        case Role::CraftRoot:CHECK(d.owner==Owner::CraftingMenu);CHECK(d.presentation==Presentation::Immersive);CHECK(d.PointerAllowed());break;
        case Role::FullMenu:CHECK(d.owner==Owner::Pause);CHECK(d.presentation==Presentation::PauseOverlay);CHECK(!d.FullFrame());CHECK(d.MenuAdapter()&&!d.PointerAllowed());break;
        case Role::PadMenu:CHECK(d.owner==Owner::GameOver);CHECK(d.presentation==Presentation::DeathOverlay);CHECK(!d.FitCompleteFrame());CHECK(d.MenuAdapter()&&!d.PointerAllowed());break;
        case Role::Modal:CHECK(d.owner==Owner::ContextualMenu);CHECK(!d.FullFrame());CHECK(d.PointerAllowed());break;
        case Role::ContextMenu:CHECK(d.owner==Owner::ContextualMenu);CHECK(d.PointerAllowed());
            CHECK(!d.FullFrame());break;
        case Role::NativeContext:CHECK(d.owner==Owner::NativeContext);CHECK(!d.FullFrame());CHECK(d.NativeContextControls());CHECK(!d.MenuAdapter()&&!d.PointerAllowed());break;
        case Role::PassiveBlocking:CHECK(d.owner==Owner::LoadingScreen);CHECK(d.FitCompleteFrame());CHECK(d.PassiveBackOnly());CHECK(d.MenuAdapter()&&!d.PointerAllowed());break;
        }
        w.Event(e.kind,false);CHECK(w.D().owner==Owner::None);
    }
    CHECK(full==9&&fixed==15&&centered==4);
    CHECK(ui_native_catalogue::Find("HUD")->kind==Hud);
    CHECK(ui_native_catalogue::Find("apSeMap")->stage_width==4096);
    CHECK(ui_native_catalogue::Find("MenuLikeUnknown")==nullptr);
    const auto* book=ui_native_catalogue::Find(AlchemyBook);
    CHECK(book->texture_width==256&&book->stage_width==2048);
    CHECK(book->mouse==-1&&book->controller==-1);
    CHECK(ui_native_catalogue::Find(GameOver)->controller==1&&ui_native_catalogue::Find(GameOver)->mouse==-1);
    CHECK(ui_surface_geometry::StageWidth==1920&&ui_surface_geometry::StageHeight==1080);

    // Parent/child nesting, tab replacement and legitimate pass-through world surfaces.
    { World w; w.Event(Inventory,true);const auto first=w.D().context;
      w.Event(InventoryInfo,true);CHECK(w.D().context==first); // auxiliary texture cannot steal/rearm focus
      w.Event(Modal,true);CHECK(w.D().owner==Owner::InventoryMap);CHECK(w.D().context>first);
      w.Event(Modal,false);CHECK(w.D().owner==Owner::InventoryMap);
      w.Event(Character,true);w.Event(Inventory,false);CHECK(w.D().owner==Owner::InventoryMap);
      w.Event(Character,false);CHECK(w.D().owner==Owner::None); // info texture alone is not a latch
      w.Event(Shop,true);CHECK(w.D().owner==Owner::Shop);
      w.Event(Modal,true);w.Event(Modal,false);CHECK(w.D().owner==Owner::Shop);
      w.s.known&=~Bit(Shop);w.s.visible&=~Bit(Shop);w.router.Resolve(w.s);
      CHECK(w.D().owner==Owner::Shop&&w.D().uncertain==Bit(Shop));
      w.Event(Shop,false);CHECK(w.D().owner==Owner::None);
    }
    { World w;w.Event(Menu,true);auto context=w.D().context;
      for(int i=0;i<2000;++i){w.Event(Hud,(i&1)!=0);w.Event(Overlay,(i&1)!=0);w.Event(AlchemyBook,(i&1)!=0);CHECK(w.D().context==context);CHECK(w.D().owner==Owner::Pause);}
      // Back from a child never toggles the parent presentation on an input edge.
      w.Event(Modal,true);w.Event(Modal,false);CHECK(w.D().owner==Owner::Pause);
      w.Event(Menu,false);CHECK(w.D().owner==Owner::None);
    }
    { World w;w.Event(Crafting,true);CHECK(w.D().owner==Owner::None);
      w.Event(AlchemyBook,true);w.Event(CraftContent,true);CHECK(w.D().owner==Owner::None);
      w.Event(CraftList,true);CHECK(w.D().owner==Owner::CraftingMenu);
      w.Event(CraftList,false);CHECK(w.D().owner==Owner::None);
      w.Event(CodexList,true);CHECK(w.D().owner==Owner::InventoryMap);
    }
    // Pause -> Loading -> native parent or immersive; generic Progress does not own presentation.
    { World w;w.Event(Menu,true);w.Event(Loading,true);CHECK(w.D().input==Input::PassiveBack);
      w.Event(Progress,true);w.Event(Loading,false);CHECK(w.D().owner==Owner::Pause);
      w.Event(Progress,false);CHECK(w.D().owner==Owner::Pause);
      w.Event(Loading,true);w.Event(Menu,false);w.Event(Loading,false);CHECK(w.D().owner==Owner::None);
      w.Event(Overlay,true);CHECK(w.D().owner==Owner::None);
    }
    { World w;w.Event(LockPicking,true);CHECK(w.D().NativeContextControls());
      w.Event(Modal,true);CHECK(w.D().MenuAdapter()&&w.D().PointerAllowed());
      w.Event(Modal,false);CHECK(w.D().NativeContextControls()&&!w.D().MenuAdapter());
      w.Event(LockPicking,false);CHECK(w.D().owner==Owner::None);
    }
    // New upper catalogue bits must survive the registry and same-kind instance destruction.
    { World w;w.Event(Pickpocketing,true,0x100000);w.Event(Pickpocketing,true,0x200000);
      w.Event(Pickpocketing,false,0x100000,true);CHECK(w.D().owner==Owner::NativeContext);
      w.Event(Pickpocketing,false,0x200000,true);CHECK(w.D().owner==Owner::None);
      CHECK(!w.registry.Apply({0x300000,unsigned(Count),true,false},~0u));
    }
    // A panic is not undone by a HUD or generic overlay visibility callback.
    { World w;w.Event(Inventory,true);w.router.Panic(w.s);CHECK(w.D().owner==Owner::None);
      w.Event(Hud,true);w.Event(Overlay,true);w.Event(GeneralBook,true);CHECK(w.D().owner==Owner::None);
      w.Event(Inventory,false);w.Event(Inventory,true);CHECK(w.D().owner==Owner::InventoryMap);
    }
    // Registering an unrelated hidden element must not rearm a held shop superaction.
    { World w;w.Event(Shop,true);const auto context=w.D().context;
      w.Event(HorseInspect,false);w.Event(Pickpocketing,false);CHECK(w.D().context==context);
      w.Event(Shop,true,0xAB000);CHECK(w.D().context>context); // new live owner is meaningful
    }
    { using namespace ui_menu_policy;OwnerState state;Frame f{};f.menu=f.foreground=true;
      f.hands[0].tracked=f.hands[1].tracked=true;f.pointer_allowed=false;
      CHECK(state.Update(f).nav);f.hands[0].grip=1;f.hands[1].grip=1;CHECK(state.Update(f).nav);CHECK(state.Update(f).hand==-1);
      f.pointer_allowed=true;++f.context;CHECK(state.Update(f).hand==1);
      GripRearm g;std::array<Hand,2> h{{{true,1,0},{true,0,0}}};
      CHECK(g.Update(true,h)==1);CHECK(g.Update(false,h)==1);
      h[0].tracked=false;h[0].grip=0;CHECK(g.Update(false,h)==1); // unknown tracking is not release
      h[0].tracked=true;CHECK(g.Update(false,h)==0);
      h[1].grip=1;CHECK(g.Update(true,h)==2);h[0].grip=1;CHECK(g.Update(false,h)==2); // independent left hand not consumed
      h[1].grip=.39f;CHECK(g.Update(false,h)==0);
    }
    // Random catalogue events cannot grant pointer solely to HUD, book, effects,
    // a passive loading screen, or a contextual game with no modal.
    { std::mt19937 random(6689);World w;
      for(unsigned i=0;i<100000;++i){auto k=static_cast<Kind>(random()%Count);w.Event(k,(random()&1)!=0);
        const auto& d=w.D();CHECK(!d.PointerAllowed()||d.MenuAdapter());
        if(d.supported&ui_native_catalogue::PassiveMask){CHECK(d.input==Input::PassiveBack);CHECK(!d.PointerAllowed());}
        if(d.NativeContextControls()){CHECK(!d.MenuAdapter());CHECK(!d.FullFrame());}
        CHECK((d.supported&~ui_native_catalogue::OwnershipMask)==0);
      }
    }
    std::cout<<"PASS "<<checks<<" catalogue/regression assertions; 28 role checks; 100000 random native-state events\n";
}
