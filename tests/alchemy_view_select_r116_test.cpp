// R116 host test: alchemy selection follows the headset view / the reaching hand (head_select_r116.h).
// geometry from the R114 log (01:27:25 C71 F5_CRAFT_SOURCE rows, herbs slot 7/8/9, carafe slot 16).
#include "subsystems/alchemy/head_select_r116.h"
#include <cassert>
#include <cstdio>
namespace h=kcd2vr::subsystems::alchemy::head_select_r116;
namespace g=kcd2vr::subsystems::alchemy::geometry68;
static g::Bounds box(double x,double y,double z,float x0,float y0,float z0,float x1,float y1,float z1){
    g::Bounds b{};float l[6]{x0,y0,z0,x1,y1,z1};for(int i=0;i<6;++i)b.local[i]=l[i];
    float w[12]{1,0,0,float(x),0,1,0,float(y),0,0,1,float(z)};for(int i=0;i<12;++i)b.world[i]=w[i];return b;}
static g::Point to(g::Point a,g::Point b){auto d=g::sub(b,a);double n=std::sqrt(g::dot(d,d));return g::mul(d,1./n);}
int main(){
    const g::Bounds herb7=box(733.463989,3366.262451,143.556061,-.122185f,-.093188f,-.000984f,.114148f,.090695f,.193953f);
    const g::Bounds herb8=box(733.513062,3366.011963,143.550537,-.122708f,-.102684f,.003529f,.127634f,.073231f,.228232f);
    const g::Bounds herb9=box(733.513123,3365.812988,143.557190,-.122185f,-.093188f,-.000984f,.114148f,.090695f,.193953f);
    const g::Bounds carafe16=box(734.762390,3365.010010,143.566498,-.112747f,-.101171f,.000340f,.102379f,.101172f,.275678f);
    const g::Bounds* all[4]{&herb7,&herb8,&herb9,&carafe16};
    const g::Point eye{734.45,3366.30,143.95}; // standing eye above Henry's root (734.56,3366.06,142.19)
    auto pick=[&](g::Point fwd){int best=-1;h::Score s{};for(int i=0;i<4;++i){auto c=h::score(*all[i],eye,fwd);if(h::better(c,s)){s=c;best=i;}}return best;};
    // looking straight at each item picks exactly that item, also the far-left herb and the far-right carafe
    assert(pick(to(eye,{733.46,3366.26,143.65}))==0);
    assert(pick(to(eye,{733.51,3366.01,143.65}))==1);
    assert(pick(to(eye,{733.51,3365.81,143.65}))==2);
    assert(pick(to(eye,{734.76,3365.01,143.70}))==3);
    // just beside a box edge (no exact hit) still picks it inside 6 deg, but not 15 deg away
    {const g::Point c{733.462971,3366.260204,143.652546};const auto base=to(eye,c); // herb7 box centre
     g::Point a=g::cross(base,{0,0,1});a=g::mul(a,1./std::sqrt(g::dot(a,a)));const g::Point b=g::cross(a,base);
     int misses=0;
     for(int k=0;k<36;++k){const double t=k*3.14159265/18.,r=std::tan(5.8*3.14159265/180.);
      const auto d=to({0,0,0},g::add(base,g::add(g::mul(a,r*std::cos(t)),g::mul(b,r*std::sin(t)))));
      const auto s=h::score(herb7,eye,d);assert(s.valid);if(!s.hit){++misses;assert(s.key<6.);}
      if(!s.hit&&std::abs(std::cos(t))>.9)assert(pick(d)==0);} // sideways near-miss still picks herb7
     assert(misses>0);}
    {auto d=to(eye,{734.10,3367.20,143.20});assert(pick(d)==-1);}
    // a 30-degree sideways look (the R114 native-camera offset) is not a pick of the item it misses
    {auto c=h::score(herb7,eye,to(eye,{734.76,3365.01,143.70}));assert(!c.hit);}
    // R118: aiming at the centre of the rightmost herb while a neighbour box in front is clipped at its
    // edge by the same ray -> the herb you aim at wins (R116 picked the nearer clipped box).
    {const g::Point target{733.462971,3366.260204,143.652546};const auto fwd=to(eye,target);
     const auto mid=g::add(eye,g::mul(fwd,.55)); // a box whose edge the ray grazes halfway there
     const g::Bounds front=box(mid.x,mid.y-.17,mid.z-.05,-.10f,-.18f,-.10f,.10f,.18f,.10f);
     const auto f=h::score(front,eye,fwd),t=h::score(herb7,eye,fwd);
     assert(f.valid&&f.hit&&t.valid&&t.hit&&h::better(t,f));}
    // hand visibility ray: R114 right palm 6 cm beside herb7 -> starts 2 cm before herb7's box,
    // not inside the hand; a palm already inside the box keeps the palm.
    const g::Point palm{733.643127,3366.262207,143.667709};
    const g::Point endpoint{733.463989,3366.262451,143.65};
    const auto o=h::losOrigin(herb7,palm,endpoint);
    const auto entry=g::ray(herb7,o,to(o,endpoint),1.);
    assert(entry.valid&&entry.distance>.015&&entry.distance<.025);
    assert(g::dot(g::sub(o,palm),g::sub(o,palm))>.03*.03); // moved away from the palm
    const g::Point inside{733.47,3366.26,143.60};
    const auto same=h::losOrigin(herb7,inside,endpoint);
    assert(same.x==inside.x&&same.y==inside.y&&same.z==inside.z);
    std::puts("alchemy_view_select_r116_test OK");
    return 0;
}
