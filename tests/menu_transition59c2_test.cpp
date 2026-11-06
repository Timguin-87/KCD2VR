#include "../src/ui_menu_transition59c2.h"
#include <cassert>
#include <thread>
using namespace kcd2vr;
using namespace ui_menu_transition59c2;
int main(){
    ui_menu_router::Router router;Gate gate;Mailbox camera;
    using namespace ui_native_state;using O=ui_menu_router::Owner;
    Snapshot ui{};ui.known=Bit(Menu)|Bit(Hud);ui.visible=Bit(Hud);
    auto theater=[&]{router.Resolve(ui);const auto d=router.Read();
        return gate.Theater(d,ui_scene_presentation::Resolve(d,false).theater,camera.Read(true));};
    camera.Publish(Camera::Other);assert(!theater());
    router.Request(O::InventoryMap);assert(!theater());
    // Texture appears early; input must work while gameplay remains immersive.
    ui.known|=Bit(Inventory);ui.visible|=Bit(Inventory);assert(!theater());
    assert(router.Read().MenuAdapter());assert(router.Read().BlocksWorldInteraction());
    for(int frame=0;frame<45;++frame){camera.Publish(Camera::Other);assert(!theater());}
    camera.Publish(Camera::Shop);assert(theater()); // Inventory root in trading, captured C51R1 mismatch.
    camera.Publish(Camera::Apse);assert(theater());
    // Child confirmation and tab changes cannot reset the active camera gate.
    ui.known|=Bit(Modal);ui.visible|=Bit(Modal);assert(theater());
    ui.visible&=~Bit(Modal);assert(theater());
    // Camera returns to gameplay while the native texture remains visible.
    camera.Publish(Camera::Other);assert(!theater());
    for(int frame=0;frame<45;++frame)assert(!theater());
    ui.visible&=~Bit(Inventory);assert(!theater());
    // A stale active sample from a closed menu must not admit its next opening.
    camera.Publish(Camera::Apse);assert(!theater());
    ui.visible|=Bit(Inventory);assert(!theater());
    camera.Publish(Camera::Apse);assert(theater());
    ui.visible&=~Bit(Inventory);assert(!theater());
    ui.known|=Bit(Shop);ui.visible|=Bit(Shop);assert(!theater());
    camera.Publish(Camera::Apse);assert(theater()); // Shop may reuse the APSE camera.
    camera.Publish(Camera::Shop);assert(theater());
    camera.Publish(Camera::Other);assert(!theater());
    ui.visible&=~Bit(Shop);assert(!theater());
    // Observer failure preserves legacy behavior, and other owners are untouched.
    ui_menu_router::Decision d{};d.owner=O::Shop;
    assert(gate.Theater(d,true,{}));
    assert(gate.Theater(d,true,{true,false,Camera::Other,0}));
    for(auto owner:{O::Pause,O::GameOver,O::ContextualMenu,O::NativeContext,O::CraftingMenu}){
        d.owner=owner;assert(!gate.Theater(d,false,camera.Read(true)));
    }
    d.owner=O::None;assert(gate.Theater(d,true,camera.Read(true))); // real cutscene

    // Simulate a UI edge during a queued copy: the frame keeps its source/layer
    // policy even if every legacy setter has since switched to the next scene.
    FramePolicy legacy{2,true,true,false,false}; // deliberately torn old setters
    auto pending=ResolveFrame(ProductionWord(true,true),legacy);
    assert(pending.stereo==0&&pending.flat&&pending.native_menu&&pending.drain);
    auto next=ResolveFrame(ProductionWord(true,false),{0,true,true,false,true});
    assert(next.stereo==2&&!next.flat&&!next.native_menu&&!next.drain);
    assert(pending.stereo==0&&pending.flat); // copied policy, not a live reference
    auto manual=ResolveFrame(0,{7,false,false,true,false});
    assert(manual.stereo==7&&manual.physical_map&&!manual.flat);
    Production production;production.Publish(true,false);const auto gameplay=production.Read();
    production.Publish(true,false);assert(production.Current(gameplay)); // ordinary tick
    production.Publish(true,true);assert(!production.Current(gameplay)); // cancel before copy
    const auto menu=production.Read();production.Publish(true,false);
    assert(!production.Current(menu)); // drop after GPU completion, before layer submission
    production.Publish(true,true);assert(!production.Current(menu)); // close/reopen ABA
    std::atomic<unsigned> word{1};std::atomic<bool> done{false};
    std::thread writer([&]{for(int i=0;i<100000;++i)word.store(ProductionWord(true,(i&1)!=0));done=true;});
    do {auto f=ResolveFrame(word.load(),legacy);assert(f.flat==(f.stereo==0));assert(f.flat==f.drain);}
    while(!done.load());writer.join();
}
