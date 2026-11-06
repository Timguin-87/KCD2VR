#include "ui_modal_overlay43.h"
#include "ui_scene_presentation.h"
#include "dialogue_original43.h"
#include "vrik/bow_camera35.h"
#include "vrik/physical_weapon.h"
#include <cassert>
#include <map>
using namespace kcd2vr;
struct IO {
    ui_native_canvas::Geometry g{960,540,100,100};int writes=0;bool fail=false;
    bool Resolve(const char*,std::uintptr_t& p){p=9;return true;}
    bool Get(ui_native_canvas::Geometry& v){v=g;return true;}
    bool Set(ui_native_canvas::Geometry v){++writes;if(fail){fail=false;return false;}g=v;return true;}
};
int main(){
    ui_surface_geometry::Hud hud{};hud.surface={1.6f,.9f,1.55f,0,0,true};hud.safe={240,135,1680,945};
    IO io;const auto native=io.g;ui_modal_overlay43::Transaction tx;
    assert(tx.Run(io,1,true,hud,1)&&io.g.xs<100&&io.g.ys<100);
    const int writes=io.writes;
    for(int i=0;i<10000;++i)assert(tx.Run(io,1,true,hud,1));
    assert(io.writes==writes);assert(tx.Run(io,1,false,hud,1)&&ui_native_canvas::Same(io.g,native));
    io.fail=true;assert(!tx.Run(io,1,true,hud,1)&&ui_native_canvas::Same(io.g,native));
    assert(tx.Run(io,1,true,hud,1));
    io.g={960,900,166.66667f,166.66667f};assert(tx.Run(io,1,true,hud,1));
    assert(std::fabs(io.g.xs-75)<.02&&std::fabs(io.g.ys-125)<.02);
    using namespace ui_native_state;using namespace ui_menu_router;
    Router router;Snapshot s{};s.known=Bit(Menu)|Bit(Hud)|Bit(Modal);s.visible=Bit(Hud);router.Resolve(s);
    s.visible|=Bit(Menu);router.Resolve(s);assert(router.Current()==Owner::Pause);
    s.visible=Bit(Hud)|Bit(Modal);router.Resolve(s);
    assert(router.Current()==Owner::Pause&&!ui_scene_presentation::Resolve(router.Read(),true).theater);
    s.visible=Bit(Hud);router.Resolve(s);assert(router.Current()==Owner::None);
    dialogue_state59c10r2::State view{};view.tick=100;view.known=true;view.entity=1;
    view.camera=dialogue_state59c10r2::Camera::FirstPerson;
    assert(ui_scene_presentation::SmithingImmersive43(true,true,true,view,100));
    assert(!ui_scene_presentation::SmithingImmersive43(true,true,true,view,181));
    view.camera=dialogue_state59c10r2::Camera::Dialogue;
    assert(!ui_scene_presentation::SmithingImmersive43(true,true,true,view,100));
    view.camera=dialogue_state59c10r2::Camera::FirstPerson;
    assert(!ui_scene_presentation::SmithingImmersive43(true,true,false,view,100));
    // Actual identity helper: NPC, foreign caller, alias/unknown entity never changes presentation.
    constexpr std::uintptr_t b=0x10000000,a=0x20000000,c=0x21000000,t=0x22000000,e=0x23000000,z=0x24000000;
    std::map<std::uintptr_t,std::uintptr_t> mem{{a,b+0x3AA4B98},{a+0x40,b+0x3A81028},
        {a+0x278,c},{c,b+0x3A80610},{c+0x1F8,a},{a+0x38,e},{t+0x38,z},
        {e,b+0x4072C78},{z,b+0x4072C78},{b+0x4072C78+0x208,b+0x9DFC5C}};
    auto read=[&](std::uintptr_t p,std::uintptr_t& out){auto i=mem.find(p);if(i==mem.end())return false;out=i->second;return true;};
    dialogue_original43::Pair pair{};
    assert(dialogue_original43::pair(b,b+0xC89C1B,true,true,a,t,read,pair)&&pair.original==e&&pair.twin==z);
    assert(!dialogue_original43::pair(b,b+0xC89C1B,false,true,a,t,read,pair));
    assert(!dialogue_original43::pair(b,b+0xC89C1B,true,false,a,t,read,pair));
    assert(!dialogue_original43::pair(b,b+0xC89C1C,true,true,a,t,read,pair));
    mem[a]=b+0x3A3B398;assert(!dialogue_original43::pair(b,b+0xC89C1B,true,true,a,t,read,pair));
    mem[a]=b+0x3AA4B98;mem[t+0x38]=e;assert(!dialogue_original43::pair(b,b+0xC89C1B,true,true,a,t,read,pair));
    // Release/nock animation cannot become the next neutral camera anchor.
    namespace bc=bow_camera35;bc::State state{};bc::Input in{};
    in.owner={1,2,3,4,5,6};in.tick=1000;in.sample=1;in.qualified=true;
    pose_core::Vec3 correction{};
    assert(!bc::correct(state,in,{0,0,1.75},1000,correction));
    in.drawing=true;++in.sample;++in.tick;
    assert(bc::correct(state,in,{0,.08,1.65},1001,correction)&&std::fabs(correction.z-.1)<1e-8);
    in.drawing=false;++in.sample;++in.tick;
    assert(bc::correct(state,in,{0,.12,1.6},1002,correction)&&std::fabs(correction.z-.15)<1e-8);
    in.drawing=true;++in.sample;++in.tick;
    assert(bc::correct(state,in,{0,.08,1.65},1003,correction)&&std::fabs(correction.z-.1)<1e-8);
    namespace ph=kcd2vr::physical;ph::WeaponState weapon{};ph::Profile profile{};
    weapon.ready=weapon.bowHeld=true;weapon.key=profile.key=20;weapon.generation=profile.generation=3;
    weapon.bowFraction=.7;weapon.maxDraw=.55;weapon.nock={.05,.03,0};weapon.bowAcquireOffset121={.05,.02,0};
    profile.classId=9;profile.bowNockQualified40=true;profile.grip[0].p={0,.04,0};profile.axis={0,1,0};
    assert(ph::upgrade_bow_nock43(weapon,profile));
    assert(weapon.bowNockQualified43&&std::fabs(weapon.nock.x)<1e-9&&std::fabs(weapon.bowAcquireOffset121.x)<1e-9);
    assert(weapon.bowFraction==.7&&weapon.maxDraw==.55&&weapon.bowHeld);
    assert(!ph::upgrade_bow_nock43(weapon,profile));
    profile.bowGeometryRank48=2;profile.bowDrawLength166=.50;profile.axis={.10,.9949874371,0};
    weapon.bowGrip38=true;assert(ph::upgrade_bow_nock43(weapon,profile));
    assert(weapon.bowGeometryRank48==2&&!weapon.bowGrip38&&weapon.maxDraw==.50&&weapon.bowFraction==.7);
    assert(!ph::upgrade_bow_nock43(weapon,profile)); // measured geometry cannot restart every frame
    weapon.bowNockQualified43=false;profile.generation=4;assert(!ph::upgrade_bow_nock43(weapon,profile));
    profile.generation=3;weapon.carry146=true;assert(!ph::upgrade_bow_nock43(weapon,profile));

}
