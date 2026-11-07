#include "ui_hud_work59c7.h"
#include "ui_hud_transition170.h"
#include "ui_hud_elements171.h"
#include "ui_palm_alpha44c2.h"
#include <cassert>
#include <cstdio>
using namespace kcd2vr;
struct Vis {
 std::array<bool,22>value{};unsigned calls=0;Vis(){value.fill(true);value[1]=false;}
 bool Acquire(){++calls;return true;}void Release(){++calls;}unsigned Generation(){return 1;}
 bool Resolve(unsigned i,const char*,std::uintptr_t&id){++calls;id=i+1;return true;}
 bool Get(unsigned i,bool&b){++calls;b=value[i];return true;}
 bool Set(unsigned i,bool b){++calls;value[i]=b;return true;}
};
struct VitalIO {
 std::array<ui_hud_work59c7::Display,2>v{{{false,0},{false,0}}};unsigned sets=0;
 bool present=true,native=false;std::uintptr_t offset=0;int fail=-1;
 bool Resolve(unsigned i,std::uintptr_t&id){id=1+i+offset;return present;}
 bool Get(unsigned i,ui_hud_work59c7::Display&d){d=v[i];return true;}
 unsigned alphaSets=0,visibleSets=0;
 bool Set(unsigned i,ui_hud_work59c7::Display d,unsigned flags){++sets;
   if(flags&0x400){++visibleSets;v[i].visible=d.visible;}
   if(flags&0x20){++alphaSets;v[i].alpha=d.alpha;}return int(i)!=fail;}
 bool NativeVisible(unsigned,bool fallback){return native?true:fallback;}
};
// Production Vitals + palm-alpha consumers share the same Stats display, as in
// GFx: visible HealthBar/StaminaBar children cannot render through hidden Stats.
struct PalmIO {
 VitalIO& stats;std::array<float,11> other{};unsigned calls=0;
 explicit PalmIO(VitalIO& s):stats(s){other.fill(100);}
 bool Acquire(){++calls;return true;}void Release(){++calls;}
 bool Resolve(unsigned,const char*){++calls;return true;}
 bool Read(unsigned i,float& a){++calls;a=i==1?stats.v[1].alpha:other[i];return true;}
 bool Write(unsigned i,float a){++calls;if(i==1)stats.v[1].alpha=a;else other[i]=a;return true;}
 bool VitalsPixels()const{return stats.v[0].visible&&stats.v[1].visible&&stats.v[0].alpha>0&&stats.v[1].alpha>0;}
};
int main(){
 ui_hud_transition170::Controller vc;Vis io;
 ui_hud_transition170::Request req{false,0,1,false,false,true,true};vc.Update(io,req,1);
 assert(io.value[1]&&io.value[3]&&io.value[4]);auto calls=io.calls;
 for(int i=0;i<10000;++i){req.weaponQam=!req.weaponQam;req.foodQam=!req.foodQam;vc.Update(io,req,1);}assert(io.calls==calls);
 req.vitals=false;vc.Update(io,req,1);assert(!io.value[1]);
 req.vitals=true;req.hidden=ui_contextual_hud162r1::HideLower163;vc.Update(io,req,1);
 assert(!io.value[1]&&io.value[3]&&io.value[4]);
 ui_hud_work59c7::Children c;ui_hud_visibility175::Key a{1,2,3,4,5};auto e=c.Epoch();
 c.Observe(1,a);assert(c.Epoch()==e+1);for(int i=0;i<10000;++i)c.Observe(1,a);assert(c.Epoch()==e+1);
 a.handle=6;c.Observe(1,a);assert(c.Epoch()==e+2);c.Remove(999);assert(c.Epoch()==e+2);c.Remove(1);assert(c.Epoch()==e+3);
 ui_hud_work59c7::Retry retry;assert(retry.Due(1,1000));retry.Complete(false);
 assert(!retry.Due(1,1001));assert(retry.Due(1,1200));assert(retry.Due(1,1400));assert(!retry.Due(1,10000));assert(retry.Due(2,10001));
 retry.Complete(true);assert(!retry.Due(2,20000));
 ui_hud_work59c7::Vitals v;VitalIO vi;
 auto r=v.Update(vi,true,1);assert(r.complete&&r.written==3&&vi.v[0].alpha==100&&vi.v[1].visible);
 auto n=vi.sets;r=v.Update(vi,true,1);assert(vi.sets==n);
 r=v.Update(vi,false,1);assert(r.complete&&!vi.v[0].visible&&vi.v[0].alpha==0&&!vi.v[1].visible);
 r=v.Update(vi,true,1);vi.v[1].alpha=42;vi.native=true;r=v.Update(vi,false,1);
 assert(vi.v[1].visible&&vi.v[1].alpha==42); // native change wins restoration
 vi.present=false;r=v.Update(vi,true,1);assert(!r.complete&&r.failed==3);
 vi.present=true;vi.offset=10;vi.v={{{false,13},{false,17}}};r=v.Update(vi,false,1);
 assert(vi.v[0].alpha==13&&vi.v[1].alpha==17); // no old restore on replacement
 // Reproduce 2026-10-14 00:32:39/00:32:54: parent visible100,
 // Stats invisible (alpha0 or100), HealthBar/StaminaBar themselves visible100.
 for(float statsNativeAlpha:{0.f,100.f}){
   VitalIO live;live.v={{{true,100},{false,statsNativeAlpha}}};PalmIO palmIo(live);
   ui_hud_work59c7::Vitals vitals;ui_palm_alpha44c2::Controller palm;
   assert(!palmIo.VitalsPixels());
   assert(vitals.Update(live,true,7,false).complete);
   assert(live.v[0].visible&&live.v[1].visible&&live.alphaSets==0);
   palm.Update(palmIo,{true,false,false,false,7});assert(!palmIo.VitalsPixels());
   palm.Update(palmIo,{true,false,true,false,7});assert(palmIo.VitalsPixels());
   const auto steadyCalls=palmIo.calls;
   for(int frame=0;frame<10000;++frame)palm.Update(palmIo,{true,false,true,false,7});
   assert(palmIo.calls==steadyCalls); // no new steady native reads
   palm.Update(palmIo,{true,false,false,false,7});assert(!palmIo.VitalsPixels()); // expiry
   assert(live.v[1].visible&&live.v[1].alpha==0&&live.alphaSets==0);
   // Exit ordering matches mod.cpp: release palm, then ordinary/native vitals.
   palm.Update(palmIo,{false,false,false,false,7});
   vitals.Update(live,false,7,false);
   assert(!live.v[1].visible&&live.v[1].alpha==statsNativeAlpha&&live.alphaSets==0);
 }
 // Moving ordinary -> palm relinquishes ONLY the old alpha lease, once;
 // a following reveal cannot be overwritten by a visibility-only refresh.
 VitalIO transition;transition.v={{{true,100},{false,0}}};PalmIO transitionPalm(transition);
 ui_hud_work59c7::Vitals tv;ui_palm_alpha44c2::Controller tp;
 tv.Update(transition,true,8);assert(transition.v[1].alpha==100);
 tv.Update(transition,true,8,false);assert(transition.v[1].alpha==0);
 tp.Update(transitionPalm,{true,false,true,false,8});assert(transitionPalm.VitalsPixels());
 auto alphaSets=transition.alphaSets;tv.Update(transition,true,8,false);
 assert(transition.alphaSets==alphaSets&&transition.v[1].alpha==100);
 tp.Update(transitionPalm,{false,false,false,false,8});tv.Update(transition,true,8);
 assert(transitionPalm.VitalsPixels()); // ordinary owner runs after palm restore
 // A native alpha rewrite on exit is not overwritten by a visibility lease.
 tv.Update(transition,true,8,false);transition.v[1].alpha=37;
 tv.Update(transition,false,8,false);assert(transition.v[1].alpha==37);
 puts("PASS explicit vital preference, no selector-edge queries, native restoration, binding epochs, bounded retries, replacement/absence");
}
