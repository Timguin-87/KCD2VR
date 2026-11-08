#include "subsystems/hud_ui_controls/prompt_policy_d80.h"
#include "ui_prompt_scale120r3.h"
#include <cassert>
#include <cmath>
#include <limits>
using namespace kcd2vr::prompt_policy_d80;
struct Cvar {int value=1,reads=0,writes=0;bool available=true;
 bool Read(int& out)noexcept{++reads;out=value;return available;}
 bool Write(int in)noexcept{++writes;value=in;return available;}};
struct PromptChildren {
 kcd2vr::ui_native_canvas::Geometry geometry[2]{};int reads=0,writes=0;
 bool Resolve(unsigned i,const char*,std::uintptr_t& id){id=i+1;return true;}
 bool Get(unsigned i,kcd2vr::ui_native_canvas::Geometry& out){++reads;out=geometry[i];return true;}
 bool Set(unsigned i,kcd2vr::ui_native_canvas::Geometry in){++writes;geometry[i]=in;return true;}
};
int main(){
    assert(minigameScale.load()==.45f&&Frame{}.scale==.45f);
    {MovieLease lease;PromptChildren io;kcd2vr::ui_prompt_scale120r3::Transaction oldOwner;
     auto ordinary=[&](){const bool enable=lease.NativeEnabled(true);
       if(enable||oldOwner.Active())assert(oldOwner.Run(io,1,enable,.64f,.70f));};
     lease.Prepare(false,1);ordinary();assert(io.geometry[0].xs==64.f);
     lease.Prepare(true,1);assert(!lease.CanDispatch(true));ordinary();assert(io.geometry[0].xs==100.f);
     lease.NativeRelinquished(lease.Epoch());assert(lease.CanDispatch(true)); // relinquish before movie
     const auto epoch=lease.Epoch();lease.Completed(epoch,true,true,false);
     io.geometry[0].xs=30.f;io.geometry[0].ys=30.f; // native movie callback already applied
     const auto writes=io.writes;ordinary();assert(io.geometry[0].xs==30.f&&io.writes==writes);
     lease.Prepare(false,1);ordinary();assert(io.geometry[0].xs==30.f); // cleanup must go first
     io.geometry[0].xs=100.f;io.geometry[0].ys=100.f;lease.Completed(epoch,false,true,false);
     ordinary();assert(io.geometry[0].xs==64.f); // ordinary 64%, not 19.2%
     lease.Prepare(true,1);lease.Completed(epoch,false,true,false);
     assert(!lease.NativeEnabled(true)); // stale exit cannot release a renewed station
     ordinary();assert(!oldOwner.Active());lease.NativeRelinquished(epoch);assert(lease.CanDispatch(true));
     lease.Completed(epoch,true,true,false); // renewed station actually dispatched before failed cleanup
     lease.Prepare(false,1);lease.Completed(epoch,false,false,true);
     assert(!lease.NativeEnabled(true)); // failed cleanup retains exact restore debt
     lease.Prepare(false,2);assert(lease.NativeEnabled(true)); // new HUD cannot inherit old owner
     lease.Completed(epoch,true,true,false);assert(lease.NativeEnabled(true)); // old HUD receipt ignored
    }
    {MovieLease lease;lease.Prepare(true,1);lease.Prepare(false,1);
     lease.Completed(lease.Epoch(),false,false,true);assert(lease.NativeEnabled(true)); // missing companion, no write
     assert(!lease.NativeEnabled(false));}

    {Publication p;Presentation off{},on{true,true,30};
     assert(!p.Needed(off,1,1));assert(p.Needed(on,1,1));p.Published(on,1,1);
     assert(!p.Needed(on,1,1));assert(p.Needed(off,1,1));p.Published(off,1,1);
     assert(!p.Needed(off,1,1));assert(p.Needed(off,1,2)); // explicit same-HUD retry, no idle poll
     p.Published(off,1,2);assert(!p.Needed(off,1,2));}
    // R145: at an active station the same request goes out again every second, never while off or before the first send.
    {Publication p;Presentation off{},on{true,true,45};
     assert(!p.Reassert(on,5000));p.Published(on,1,1,5000);
     assert(!p.Reassert(on,5999));assert(p.Reassert(on,6000));
     p.Published(on,1,1,6000);assert(!p.Reassert(on,6500));
     assert(p.Reassert(on,10)); // clock went back: send, never wait forever
     p.Published(off,1,1,7000);assert(!p.Reassert(off,20000)&&!p.Reassert(on,20000)); // left the station: Needed() decides
     assert(p.Needed(on,1,1));}
    // R145: the yellow message line follows the hints slider on top of the authored 50 %, never bigger, gives it back.
    {struct InfoIO{kcd2vr::ui_native_canvas::Geometry g{0,0,50,50,0};int writes=0;
       bool Resolve(unsigned,const char*,std::uintptr_t& id){id=7;return true;}
       bool Get(unsigned,kcd2vr::ui_native_canvas::Geometry& out){out=g;return true;}
       bool Set(unsigned,kcd2vr::ui_native_canvas::Geometry in){++writes;g=in;return true;}}io;
     kcd2vr::ui_prompt_scale120r3::InfoTextR145 info;
     assert(info.Run(io,1,true,.51f)&&std::fabs(io.g.xs-25.5f)<.01f&&std::fabs(io.g.ys-25.5f)<.01f);
     const int writes=io.writes;assert(info.Run(io,1,true,.51f)&&io.writes==writes); // ours already: no write
     assert(info.Run(io,1,true,1.4f)&&std::fabs(io.g.xs-50.f)<.01f); // >100 % = authored size, never bigger
     assert(info.Run(io,1,true,.5f)&&std::fabs(io.g.xs-25.f)<.01f);
     io.g.xs=io.g.ys=50.f;assert(info.Run(io,1,true,.5f)&&std::fabs(io.g.xs-25.f)<.01f); // native reset: once, not twice
     assert(info.Run(io,1,false,.5f)&&std::fabs(io.g.xs-50.f)<.01f&&!info.Active()); // window closed: authored size back
     assert(kcd2vr::ui_prompt_scale120r3::InfoTextR145::Clamp(std::numeric_limits<float>::quiet_NaN())==1.f);}
    {HintsLease hints;Cvar io;hints.Update(io,true,false,1);assert(io.value==0);
     const auto reads=io.reads;for(int i=0;i<100;++i)hints.Update(io,true,false,1);assert(io.reads==reads);
     hints.Update(io,true,true,1);assert(io.value==1);hints.Update(io,false,true,2);assert(io.value==1);
     hints.Update(io,true,false,3);io.value=2;hints.Update(io,false,false,4);assert(io.value==2);}
    {HintsLease hints;Cvar io;io.value=0;hints.Update(io,true,true,1);assert(io.value==1);
     hints.Update(io,false,true,2);assert(io.value==0);}

    // Ingredient descriptions use the same movie children as prompts: alchemy
    // must stay visible even with timing enabled, without repeated dispatches.
    {State alchemy;Frame f{true,false,true,55,100,3,.45f,true};Publication published;
     auto view=alchemy.update(f);assert(view.active&&view.visible&&view.percent==45);
     published.Published(view,1,1);
     for(auto now:{10100ULL,60100ULL,3600100ULL}){f.now=now;view=alchemy.update(f);
         assert(view.visible&&view.percent==45&&!published.Needed(view,1,1));}
     f.palm=true;++f.now;assert(alchemy.update(f).visible);
     f.now+=3000;assert(alchemy.update(f).visible); // open-palm timer cannot hide names
     f.active=false;view=alchemy.update(f);assert(!view.active&&view.visible&&view.percent==100);
     f.active=true;f.alchemy=false;f.palm=false;++f.now;const auto start=f.now;
     assert(alchemy.update(f).visible);
     f.now=start+9999;assert(alchemy.update(f).visible);
     f.now=start+10000;assert(!alchemy.update(f).visible); // smithing policy unchanged
     f.palm=true;assert(alchemy.update(f).visible);
     f.now+=2999;assert(alchemy.update(f).visible);
     ++f.now;assert(!alchemy.update(f).visible);
     f.alchemy=true;++f.now;assert(alchemy.update(f).visible); // same HUD restores immediately
     f.alchemy=false;++f.now;assert(alchemy.update(f).visible); // timer restarts on switch
     f.now+=10000;assert(!alchemy.update(f).visible);
     f.lifetime=0;assert(!alchemy.update(f).active); // invalid station never owns presentation
    }
    State s;Frame f{true,false,true,11,100,2,.30f};
    auto r=s.update(f);assert(r.active&&r.visible&&r.percent==30);
    f.now=10099;assert(s.update(f).visible);
    f.now=10100;assert(!s.update(f).visible);
    f.palm=true;assert(s.update(f).visible);
    f.now=20100;assert(!s.update(f).visible); // open palm held is not an endless reveal
    f.palm=false;++f.now;assert(!s.update(f).visible);
    f.palm=true;++f.now;assert(s.update(f).visible);
    f.now+=10000;assert(!s.update(f).visible);
    ++f.phase;assert(s.update(f).visible); // actual native next step
    f.now+=10000;assert(!s.update(f).visible);
    ++f.lifetime;assert(s.update(f).visible); // same-address/new station visit
    f.timed=false;f.now+=20000;assert(s.update(f).visible);
    f.timed=true;assert(s.update(f).visible);
    f.scale=0;r=s.update(f);assert(r.percent==25);
    f.scale=2;r=s.update(f);assert(r.percent==100);
    f.scale=std::numeric_limits<float>::quiet_NaN();assert(s.update(f).percent==45);
    f.active=false;r=s.update(f);assert(!r.active&&r.visible&&r.percent==100);
    f.active=true;f.lifetime=0;assert(!s.update(f).active);
    f.lifetime=12;f.now=900;assert(s.update(f).visible);
    f.now=1;assert(s.update(f).visible); // clock reset cannot strand hidden UI
}
