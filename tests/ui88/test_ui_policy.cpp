#include "ui_menu_policy.h"
#include "ui_menu_router.h"
#include "ui_surface_geometry.h"
#include "ui_visibility_registry.h"
#include "ui_hud_geometry.h"
#include <atomic>
#include <cassert>
#include <cstdlib>
#include <cmath>
#include <cstdint>
#include <iostream>
#include <limits>
#include <random>
#include <thread>
#include <unordered_set>
#include <vector>
static int checks=0;
#define CHECK(x) do {++checks;if(!(x)){std::cerr<<"FAIL "<<__LINE__<<": "<<#x<<"\n";std::exit(1);}}while(0)
int main(){
    using namespace kcd2vr;
    { using namespace ui_menu_policy; OwnerState p; Frame f{};f.menu=f.foreground=true;f.hands[0].tracked=f.hands[1].tracked=true;
      CHECK(p.Update(f).nav);f.hands[0].grip=.8f;auto d=p.Update(f);CHECK(d.hand==0&&!d.nav);
      f.hands[1].grip=.9f;CHECK(p.Update(f).hand==0);
      f.hands[0].grip=.45f;CHECK(p.Update(f).hand==0);
      f.hands[0].grip=.3f;CHECK(p.Update(f).hand==1);
      f.hands[1].trigger=.9f;f.hands[1].grip=0;CHECK(p.Update(f).hand==1);
      f.hands[1].trigger=0;CHECK(p.Update(f).nav);
      f.buttons=0x4000;CHECK(p.Update(f).nav);f.hands[1].grip=.9f;CHECK(p.Update(f).nav);
      CHECK(MakePad(f,true).buttons&0x4000);f.buttons=0;CHECK(p.Update(f).hand==1);
      f.hands[1].tracked=false;CHECK(p.Update(f).owner==Owner::Blocked);
      f.foreground=false;CHECK(p.Update(f).owner==Owner::Blocked);
      f.menu=false;CHECK(p.Update(f).owner==Owner::Outside);
      f.menu=f.foreground=true;f.hands[1].tracked=true;f.hands[1].grip=0;f.buttons=0x1000;
      CHECK(!p.Update(f).nav);f.buttons=0;CHECK(p.Update(f).nav);
      f.axes[2]=.8f;auto pad=MakePad(f,true);CHECK(pad.buttons&0x200);CHECK(pad.rx==0);
      f.axes[2]=-.8f;pad=MakePad(f,true);CHECK(pad.buttons&0x100);CHECK(!(pad.buttons&0x200));
      CHECK(Axis(-1)==-32767&&Axis(1)==32767&&Axis(9)==32767&&Axis(NAN)==0);
      CHECK(Trigger(9)==255&&Trigger(-2)==0&&Trigger(NAN)==0);
      f.hands[0].grip=NAN;CHECK(p.Update(f).owner==Owner::Blocked);
    }
    { using namespace ui_native_state;using Owner=ui_menu_router::Owner;ui_menu_router::Router r;Snapshot s{};
      CHECK(r.Resolve(s)==Owner::Frontend);
      s.known=s.visible=Bit(Menu);s.epoch=1;CHECK(r.Resolve(s)==Owner::Frontend);
      s.visible|=Bit(Modal);s.known|=Bit(Modal);++s.epoch;CHECK(r.Resolve(s)==Owner::Frontend);
      s.visible&=~Bit(Modal);++s.epoch;CHECK(r.Resolve(s)==Owner::Frontend);
      s.visible=0;++s.epoch;CHECK(r.Resolve(s)==Owner::None);
      r.Request(Owner::Pause);CHECK(r.Resolve(s)==Owner::Pause); // old closed sample is not close ACK
      s.visible=Bit(Menu);++s.epoch;CHECK(r.Resolve(s)==Owner::Pause);
      CHECK(r.Resolve(s)==Owner::Pause); // nested settings/back with same native root
      s.visible=0;++s.epoch;CHECK(r.Resolve(s)==Owner::None);
      s.known|=Bit(Inventory)|Bit(Shop);s.visible=Bit(Inventory)|Bit(Shop);++s.epoch;CHECK(r.Resolve(s)==Owner::Shop);
      s.known&=~Bit(Shop);s.visible=Bit(Inventory)|Bit(Modal);s.known|=Bit(Modal);++s.epoch;CHECK(r.Resolve(s)==Owner::Shop);
      s.visible=0;++s.epoch;CHECK(r.Resolve(s)==Owner::Shop); // unknown parent cannot be lost with child close
      s.known|=Bit(Shop);++s.epoch;CHECK(r.Resolve(s)==Owner::None);
      s.visible=Bit(Shop);++s.epoch;CHECK(r.Resolve(s)==Owner::Shop);
      r.Panic(s);CHECK(r.Resolve(s)==Owner::None);CHECK(r.Resolve(s)==Owner::None);
      s.visible=0;++s.epoch;CHECK(r.Resolve(s)==Owner::None);
      s.visible=Bit(Inventory);++s.epoch;CHECK(r.Resolve(s)==Owner::InventoryMap);
    }
    { ui_visibility_registry::Registry r;using ui_visibility_registry::Event;
      CHECK(r.Apply({0x1000,0,true,false},0xffff));CHECK(r.Read().visible==1);
      CHECK(r.Apply({0x2000,0,true,false},0xffff));CHECK(r.Apply({0x1000,0,false,true},0xffff));CHECK(r.Read().visible==1);
      CHECK(r.Apply({0x2000,0,false,true},0xffff));CHECK(r.Read().visible==0&&r.Read().known==1);
      auto epoch=r.Read().epoch;CHECK(r.Apply({0x3000,7,true,false},1));CHECK(r.Read().epoch==epoch);
      CHECK(!r.Apply({0,0,true,false},0xffff));CHECK(!r.Apply({1,static_cast<unsigned>(ui_native_state::Count),true,false},0xffff));
      ui_visibility_registry::Queue q;Event e{};CHECK(!q.Pop(e));
      for(int i=0;i<512;++i)CHECK(q.Push({static_cast<std::uintptr_t>(i+1),0,true,false}));
      CHECK(!q.Push({9999,0,true,false}));for(int i=0;i<512;++i){CHECK(q.Pop(e));CHECK(e.owner==static_cast<std::uintptr_t>(i+1));}CHECK(!q.Pop(e));
    }
    { ui_visibility_registry::Queue q;std::atomic<unsigned> done{0},accepted{0};std::vector<std::thread> writers;
      for(unsigned w=0;w<4;++w)writers.emplace_back([&,w]{for(unsigned i=0;i<20000;++i)if(q.Push({1+w*20000+i,w,true,false}))++accepted;++done;});
      std::unordered_set<std::uintptr_t> seen;ui_visibility_registry::Event e{};
      for(;;){if(q.Pop(e)){CHECK(seen.insert(e.owner).second);}else if(done.load()==4)break;else std::this_thread::yield();}
      for(auto& w:writers)w.join();
      while(q.Pop(e))CHECK(seen.insert(e.owner).second);
      CHECK(seen.size()==accepted.load());
    }
    { using namespace ui_surface_geometry;std::array<Eye,2> eyes{};eyes[0].position.x=-.033f;eyes[1].position.x=.033f;
      for(float aspect:{.75f,1.f,16.f/9.f,2.4f}){
        auto h=ComputeHud(eyes,aspect);CHECK(h.surface.valid);CHECK(std::fabs(h.surface.width/h.surface.height-aspect)<1e-5f);
        CHECK(h.safe.left>=0&&h.safe.right<=1920&&h.safe.top>=0&&h.safe.bottom<=1080);
        const float w=(h.safe.right-h.safe.left)/1920*h.surface.width,hh=(h.safe.bottom-h.safe.top)/1080*h.surface.height;
        CHECK(BoxInside(eyes,w,hh,1.55f,0,0,.88f,.78f));auto fit=Fit(eyes,aspect,1.5f,2.65f);CHECK(fit.valid&&BoxInside(eyes,fit.width,fit.height,1.5f,0,0,.72f,.60f));
      }
      eyes[0].orientation={0,std::sin(.12f),0,std::cos(.12f)};eyes[1].orientation={0,-std::sin(.12f),0,std::cos(.12f)};
      CHECK(ComputeHud(eyes,1.78f).surface.valid);
      CHECK(!ComputeHud(eyes,NAN).surface.valid);eyes[0].fov.left=NAN;CHECK(!ComputeHud(eyes,1.78f).surface.valid);
    }
    { auto packed=ui_hud_geometry::Pack(12,-20,-55,120);std::uint16_t flags=0;std::memcpy(&flags,packed.data()+0x34,2);CHECK(flags==0x1B);
      packed[0x34]=0xff;packed[0x35]=0xff;packed[0x08]=7;auto restored=ui_hud_geometry::Restore(packed);
      std::memcpy(&flags,restored.data()+0x34,2);CHECK(flags==0x1B);CHECK(restored[0x08]==0);float xs=0;std::memcpy(&xs,restored.data()+0x0c,4);CHECK(xs==-55);
    }
    { using namespace ui_menu_policy;std::mt19937 rng(6688);OwnerState owner;Frame f{};
      for(unsigned i=0;i<100000;++i){f.menu=(rng()%30)!=0;f.foreground=(rng()%40)!=0;f.valid=(rng()%200)!=0;
        if(rng()%23==0)++f.context;
        f.buttons=(rng()%5==0)?0x4000:0;
        for(auto&h:f.hands){h.tracked=(rng()%30)!=0;h.grip=float(rng()%101)/100;h.trigger=float(rng()%101)/100;}
        for(auto&a:f.axes)a=(int(rng()%201)-100)/100.f;
        auto d=owner.Update(f);CHECK(!d.nav||d.hand<0);CHECK(d.hand<0||f.hands[d.hand].tracked);CHECK(f.menu||d.owner==Owner::Outside);
      }
    }
    std::cout<<"PASS "<<checks<<" assertions; 100000 randomized input frames; 80000 multi-producer queue attempts\n";
}
