// R112 HUD: combat crosshair scale-only owner + quick weapon/potion selector fade timing (host test).
#include "ui_hud_r112.h"
#include <cstdio>
#include <cmath>
using namespace kcd2vr;
static int fails=0;
#define CHECK(c) do{if(!(c)){std::printf("FAIL %s:%d %s\n",__FILE__,__LINE__,#c);++fails;}}while(0)
struct FakeCursor {
    ui_native_canvas::Geometry g{-0.7f,162.6f,100.f,100.f,0.f};bool present=true;std::uint64_t revision=1;unsigned sets=0;
    bool Resolve(unsigned i,const char*,std::uintptr_t& id){if(i||!present)return false;id=0x1234;return true;}
    bool Get(unsigned i,ui_native_canvas::Geometry& out){if(i)return false;out=g;return true;}
    bool Set(unsigned i,ui_native_canvas::Geometry v){if(i)return false;++sets;g.xs=v.xs;g.ys=v.ys;return true;} // flags 0x18: scale only
    std::uint64_t Revision(unsigned){return revision;}
};
int main(){
    // 1. crosshair: half size, position stays native, native moves keep the scale, restore gives 100 back
    {ui_hud_r112::CombatCrosshair t;FakeCursor io;
     CHECK(t.Run(io,1,true,.5f));CHECK(std::fabs(io.g.xs-50.f)<.01f&&std::fabs(io.g.ys-50.f)<.01f);
     CHECK(std::fabs(io.g.x+0.7f)<.01f&&std::fabs(io.g.y-162.6f)<.01f);
     io.g.x=300.f;io.g.y=-20.f;const unsigned sets=io.sets;CHECK(t.Run(io,1,true,.5f));CHECK(io.sets==sets); // game moved it: no rewrite
     CHECK(std::fabs(io.g.x-300.f)<.01f);
     io.g.xs=io.g.ys=100.f;CHECK(t.Run(io,1,true,.5f));CHECK(std::fabs(io.g.xs-50.f)<.01f); // native scale reset answered, not 25
     CHECK(t.Run(io,1,true,.3f));CHECK(std::fabs(io.g.xs-30.f)<.01f);                    // slider from native base
     CHECK(t.Run(io,1,false,.3f));CHECK(std::fabs(io.g.xs-100.f)<.01f);CHECK(!t.Active()); // restore
     CHECK(t.Run(io,2,true,9.f));CHECK(std::fabs(io.g.xs-150.f)<.01f);                   // clamp 1.50
     FakeCursor none;none.present=false;ui_hud_r112::CombatCrosshair u;CHECK(u.Run(none,1,true,.5f));CHECK(u.LastReport().resolved==0);}
    // 2. quick slots only while selecting (R115)
    {ui_hud_r112::QuickFade f;ui_hud_r112::QuickInput in{true,false,{0,0},10000,7};
     auto a=f.Targets(in);CHECK(a[0]==0.f&&a[1]==0.f);                     // new HUD / load: hidden
     in.reveal=true;in.now=11000;a=f.Targets(in);CHECK(a[0]==0.f&&a[1]==0.f); // palm does not show them
     in.reveal=false;
     in.input[0]=20000;in.now=20000;a=f.Targets(in);CHECK(a[0]==100.f&&a[1]==0.f); // left = weapon only
     in.now=23000;a=f.Targets(in);CHECK(a[0]==100.f);
     in.now=23350;a=f.Targets(in);CHECK(a[0]>40.f&&a[0]<60.f);             // half way through the 0.7 s fade
     in.now=23700;a=f.Targets(in);CHECK(a[0]==0.f&&a[1]==0.f);             // gone
     in.input[1]=30000;in.now=30001;a=f.Targets(in);CHECK(a[1]==100.f&&a[0]==0.f); // right = food
     in.enabled=false;a=f.Targets(in);CHECK(a[0]<0.f&&a[1]<0.f);            // OFF: always shown (native)
     in.enabled=true;in.hudLifetime=8;in.now=50000;a=f.Targets(in);CHECK(a[0]==0.f&&a[1]==0.f); // save load: hidden
     in.input[0]=999999;in.now=50001;a=f.Targets(in);CHECK(a[0]==0.f);     // future stamp ignored
    }
    std::printf(fails?"hud_r112_test FAILED %d\n":"hud_r112_test PASS\n",fails);return fails?1:0;
}
