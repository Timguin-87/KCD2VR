#include "ui_hud_experiment36c5.h"
#include "ui_hud_transition170.h"
#include "ui_palm_alpha44c2.h"
#include "ui_gameplay_hud54.h"
#include <cassert>
#include <cstdio>
using namespace kcd2vr;
struct Graph {
 ui_hud_visibility175::Registry registry;ui_hud_transition170::Controller visibility;ui_palm_alpha44c2::Controller alphaOwner;
 ui_hud_experiment36c5::PalmReveal palms;std::array<ui_hud_visibility175::Key,22> keys{};
 std::array<bool,22> visible{};std::array<float,22> alpha{};unsigned calls=0,hidden=0;bool left=false,right=false,gaze=false,active=true;
 std::uint64_t generation=1;
 Graph(){visible.fill(true);alpha.fill(100);visible[0]=visible[1]=false;alpha[0]=alpha[1]=0;
  for(unsigned i=0;i<22;++i)keys[i]={100+i,200+i,300+i,999,generation};}
 bool Acquire(){++calls;return true;}void Release(){++calls;}std::uint64_t Generation(){return generation;}
 bool Resolve(unsigned i,const char*,std::uintptr_t& id){++calls;id=keys[i].wrapper;return registry.Bind(i,keys[i],true,false);}
 bool Get(unsigned i,bool& value){++calls;value=visible[i];return true;}
 bool Set(unsigned i,bool value){++calls;visible[i]=value;return true;}
 bool NativeVisibility175(unsigned i,std::uintptr_t,bool current,bool& native){return registry.NativeMeaning(i,keys[i],current,native);}
 bool NativeRestriction175(unsigned){return false;}
 struct AlphaIO {
  Graph& g;static unsigned map(unsigned i){constexpr unsigned m[]{0,1,2,5,6,7};return i<6?m[i]:i;}
  bool Acquire(){return g.Acquire();}void Release(){g.Release();}
  bool Resolve(unsigned i,const char*){++g.calls;const auto n=map(i);return g.registry.Bind(n,g.keys[n],true,false);}
  bool Read(unsigned i,float& value){++g.calls;value=g.alpha[map(i)];return true;}
  bool Write(unsigned i,float value){++g.calls;g.alpha[map(i)]=value;return true;}
  bool NativeAlpha54(unsigned i,float value,float& native){const auto n=map(i);return g.registry.NativeAlphaMeaning54(n,g.keys[n],value,native);}
 };
 ui_contextual_hud162r1::Output Step(ui_contextual_hud162r1::Input in,std::array<bool,2> open,bool gazeAlchemy=false){
  auto reveal=palms.Update(in,open);active=reveal.active;left=reveal.left;right=reveal.right;gaze=gazeAlchemy;
  const unsigned preferences=ui_gameplay_hud54::Preferences(0,true,gazeAlchemy);
  hidden=ui_hud_experiment36c5::VisibilityMask(preferences,reveal);
  const auto event=registry.Revision54();visibility.Update(*this,{false,hidden,event,false,false,true,right,left,gaze},generation);
  AlphaIO io{*this};alphaOwner.Update(io,{active,left,right,false,generation*1000+registry.Revision54(),generation});return reveal;
 }
 void NativeVisible(unsigned i,bool wanted){auto d=registry.NativeRequest(keys[i].object,wanted,999,generation,hidden,true,
  [&](const auto& k){return ui_hud_visibility175::Same(k,keys[i]);},true,false);
  visible[i]=d.visible;if(d.qualified&&((i==0&&left&&!(hidden&ui_contextual_hud162r1::Left))||(i==19&&gaze)))visible[i]=true;}
 void NativeAlpha(unsigned i,float wanted){auto d=registry.NativeAlphaRequest54(keys[i],wanted,hidden,true,active,left,right,
  [&](const auto& k){return ui_hud_visibility175::Same(k,keys[i]);},gaze);alpha[i]=d.value;}
 bool Pixels(unsigned i)const{return visible[i]&&alpha[i]>0;}
};
int main(){
 ui_contextual_hud162r1::Input in{};in.enabled=in.gameplay=in.headValid=true;in.headOrientation={0,0,0,1};
 const float q=std::sqrt(.5f);in.palm[0]={{0,0,-.5f},{0,-q,0,q},true};in.palm[1]={{0,0,-.5f},{0,q,0,q},true};in.now=100;
 Graph g;g.Step(in,{false,false});assert(!g.Pixels(0)&&!g.Pixels(1));
 in.now=200;assert(!g.Step(in,{true,false}).left);in.now=1199;assert(!g.Step(in,{true,false}).left);
 in.now=1200;assert(g.Step(in,{true,false}).left&&g.Pixels(0)&&!g.Pixels(1));
 g.NativeVisible(0,false);g.NativeAlpha(0,0);assert(g.Pixels(0));
 in.headOrientation={0,1,0,0};in.now=1300;g.Step(in,{false,false});assert(g.Pixels(0));
 const auto revision=g.registry.Revision54();g.registry.RemoveWrapper(g.keys[0].wrapper);g.keys[0]={501,502,503,999,1};
 g.registry.Bind(0,g.keys[0],true,false);g.visible[0]=false;g.alpha[0]=0;assert(g.registry.Revision54()>revision);
 in.now=1400;g.Step(in,{false,false});assert(g.Pixels(0));
 in.now=11200;g.Step(in,{false,false});assert(!g.Pixels(0));
 g.NativeVisible(0,true);g.NativeAlpha(0,100);assert(!g.Pixels(0));
 in.headOrientation={0,0,0,1};in.now=12000;g.Step(in,{false,true});in.now=13000;assert(g.Step(in,{false,true}).right&&g.Pixels(1));
 in.headOrientation={0,1,0,0};in.now=13100;g.Step(in,{false,false});assert(g.Pixels(1));
 in.now=23000;g.Step(in,{false,false});assert(!g.Pixels(1));
 for(unsigned i:{13u,14u,15u,19u}){g.NativeVisible(i,true);assert(!g.visible[i]);}
 in.now=23100;g.Step(in,{false,false},true);g.NativeVisible(19,false);g.NativeAlpha(19,0);assert(g.Pixels(19));
 in.now=23200;g.Step(in,{false,false},false);g.NativeVisible(19,true);assert(!g.visible[19]);
 in.now=23300;g.Step(in,{false,false});in.now=23400;g.Step(in,{false,false});const auto cost=g.calls;
 for(unsigned i=0;i<10000;++i)g.Step(in,{false,false});assert(g.calls==cost);
 // Unrelated child reconstruction and exit may share a frame. Event revision
 // must not discard the restoration lease of unchanged Compass/Stats gates.
 Graph exit;exit.alpha[0]=65;exit.alpha[1]=80;exit.visible[0]=exit.visible[1]=true;
 in.enabled=true;in.now=30000;exit.Step(in,{false,false});assert(exit.alpha[0]==0&&exit.alpha[1]==0);
 exit.registry.RemoveWrapper(exit.keys[19].wrapper);exit.keys[19]={901,902,903,999,1};
 exit.registry.Bind(19,exit.keys[19],true,false);
 in.enabled=false;in.now=30100;exit.Step(in,{false,false});assert(exit.alpha[0]==65&&exit.alpha[1]==80);
 assert(ui_gameplay_hud54::Preferences(0,false,false)==0);
 std::puts("PASS C54 full palm/visibility/alpha/native-writer graph: hidden startup, 1s/10s, same-HUD replacement, alchemy cursor, no steady polling");
}
