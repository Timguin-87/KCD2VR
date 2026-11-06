#include "ui_helmet_overlay63.h"
#include "vrik/left_holding_b2.h"
#include "subsystems/base/view_defaults_b2.h"
#include <cassert>
#include <cmath>
#include <cstdio>
#include <string_view>
using namespace kcd2vr;
struct IO {
 ui_native_canvas::Geometry v{960,540,100,180,0};std::uintptr_t id=1;int writes=0;bool missing=false,fail=false;
 bool Resolve(const char* name,std::uintptr_t& out){assert(std::string_view(name)=="overlay0");out=id;return !missing;}
 bool Get(ui_native_canvas::Geometry& out){out=v;return true;}
 bool Set(ui_native_canvas::Geometry g){++writes;if(fail)return false;v=g;return true;}
};
int main(){
 ui_helmet_overlay63::Transaction t;IO io;
 assert(t.Run(io,1,true,4));assert(io.v.x==960&&io.v.y==540&&std::fabs(io.v.xs-95.04f)<.01&&std::fabs(io.v.ys-142.56f)<.01);
 assert(t.Run(io,1,true,4)&&io.writes==1);
 assert(t.Run(io,1,true,2)&&io.writes==1);
 io.v.y=545;io.v.ys=200;assert(t.Run(io,1,true,2)&&io.v.y==545&&std::fabs(io.v.ys-158.4f)<.01);
 assert(t.Run(io,1,false,2)&&io.v.x==960&&io.v.ys==200&&io.v.xs==100);
 assert(!t.Run(io,1,true,0));assert(!t.Run(io,1,true,NAN));
 io.missing=true;assert(!t.Run(io,1,true,4));io.missing=false;
 io.id=2;io.v={1000,545,100,180,0};assert(t.Run(io,1,true,4)&&io.v.x==1000);
 t.Forget();io.v={960,540,100,180,0};assert(t.Run(io,2,true,4)&&io.v.x==960);
 io.fail=true;assert(!t.Run(io,2,false,4));
 assert(vr_combat111::LeftHoldingIdleB2("holditem_idle_torch_lhand_over",3));
 assert(vr_combat111::LeftHoldingIdleB2("holditem_idle_shield_lhand",3));
 // R108: the game's combat-stance torch hold (HoldItemLeft "l_torch+combat") keeps VRIK on layer 3 only;
 // the same clip on the mercy finisher's FullBody layer 1 stays native.
 assert(vr_combat111::LeftHoldingIdleB2("combat_idle01_shsw_torch_over",3));
 for(int layer:{0,1,2,4,5,6,7,8})assert(!vr_combat111::LeftHoldingIdleB2("combat_idle01_shsw_torch_over",layer));
 for(int layer:{0,1,2,4,5,6,7,8})assert(!vr_combat111::LeftHoldingIdleB2("holditem_idle_torch_lhand_over",layer));
 assert(!vr_combat111::LeftHoldingIdleB2("unknown_torch_action",3));
 assert(!vr_combat111::LeftHoldingIdleB2(nullptr,3));
 assert(std::fabs(view_defaults_b2::Get("VrFlatScreenScale",1.7f)-1.05f)<.001);
 assert(std::fabs(view_defaults_b2::Get("VrGameFov",100.f)-94.9999f)<.001);
 assert(view_defaults_b2::Get("VrFpsCapEnabled",true)==false);
 assert(view_defaults_b2::Get("PrivateArmCalibration",42)==42);
 std::puts("PASS B2 helmet scale/native center/lifetime/restore; layer-specific holding; view defaults");
}
