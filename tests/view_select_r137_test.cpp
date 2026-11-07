// R137 host test: VRIK object selection source (headset view / index-finger point / palm) - vrik/view_select_r137.h.
// g++ -std=c++20 -I src tests/view_select_r137_test.cpp && ./a.out
#include "vrik/view_select_r137.h"
#include "vrik/palm_selection59c9.h"
#include <cassert>
#include <cmath>
#include <cstdio>
int main(){
    using namespace kcd2vr::view_select_r137;
    // --- source per candidate ---
    // R136 log: hands empty + open, object 1.5 m in front where you look, no palm takes it -> headset view (was refused)
    assert(decide(false,-1,-1,0)==Source::Head);
    // object 0.44 m from the right palm that the palm selects (median INTERACTOR_CANDIDATE distance) -> palm path
    assert(decide(false,-1,0,.44f)==Source::Palm);
    // palm cone reaches 2 m: a palm "hit" 1.2 m away no longer steals the view
    assert(decide(false,-1,1,1.2f)==Source::Head);
    // a held/locked palm transaction always stays palm, even while pointing
    assert(decide(true,0,-1,0)==Source::Palm);
    // pointing overrides the view and a far palm
    assert(decide(false,0,-1,0)==Source::Finger);
    assert(decide(false,0,1,.30f)==Source::Finger);
    assert(decide(false,0,0,.30f)==Source::Finger);
    // an object touching the pointing hand's palm (<=16 cm) stays the hand's
    assert(decide(false,0,0,.10f)==Source::Palm);
    // the OTHER hand touching it does not beat the point
    assert(decide(false,1,0,.10f)==Source::Finger);
    assert(decide(false,-1,0,NAN)==Source::Head);
    // --- ranking: palm (<=56) < finger touch... head picks rank after palm picks ---
    float x=0,y=0;rank(Source::Head,50.f,50.f,x,y);assert(x>56.f&&y==50.f);
    float hx=0;rank(Source::Head,60.f,50.f,hx,y);assert(hx>x); // closer to your view centre wins
    rank(Source::Finger,50.f,50.f,x,y);assert(x>51.6f&&x<56.f);
    // --- head projection with the INI values (0.21 m, 10 deg): centre of view selected, 1 m beside it not ---
    kcd2vr::interaction112::Ray head{};head.ox=0;head.oy=0;head.oz=1.7f;head.x=0;head.y=1;head.z=0;
    auto in=kcd2vr::interaction57c1::Project(head,0.1f,2.f,1.7f,20.f,.21f,10.f);assert(in.inside);
    auto out=kcd2vr::interaction57c1::Project(head,1.0f,2.f,1.7f,20.f,.21f,10.f);assert(!out.inside);
    // --- point pose ---
    Head h{};h.valid=true;h.eye[0]=0;h.eye[1]=0;h.eye[2]=1.7f;h.fwd[0]=0;h.fwd[1]=1;h.fwd[2]=0;
    Hand p{};p.valid=true;p.empty=true;p.grip=.9f;p.trigger=0;p.touchValid=true;p.touch=false;
    p.hand[0]=.15f;p.hand[1]=.45f;p.hand[2]=1.4f;p.dir[0]=0;p.dir[1]=.95f;p.dir[2]=.3f;
    assert(pose(p,h));
    {Hand q=p;q.touch=true;assert(!pose(q,h));}          // index on the trigger = fist/grab, not a point
    {Hand q=p;q.touchValid=false;assert(!pose(q,h));}     // no capacitive touch -> never a point
    {Hand q=p;q.grip=.3f;assert(!pose(q,h));}             // open hand = palm hover, not a point
    {Hand q=p;q.trigger=.5f;assert(!pose(q,h));}
    {Hand q=p;q.empty=false;assert(!pose(q,h));}          // holding a weapon
    {Hand q=p;q.hand[1]=.05f;assert(!pose(q,h));}         // hand at the chest, not in front
    {Hand q=p;q.dir[1]=-.95f;assert(!pose(q,h));}         // pointing back at you
    {Hand q=p;q.dir[0]=NAN;assert(!pose(q,h));}
    {Head g=h;g.valid=false;assert(!pose(p,g));}
    // --- hold 120 ms, release at once, no latch ---
    State s{};bool posed[2]{true,false};
    assert(update(s,posed,1000)==-1);
    assert(update(s,posed,1119)==-1);
    assert(update(s,posed,1120)==0);
    posed[0]=false;assert(update(s,posed,1130)==-1);
    posed[0]=true;assert(update(s,posed,1140)==-1);assert(update(s,posed,1260)==0); // works again
    bool both[2]{true,true};State t{};update(t,both,0);both[0]=false;update(t,both,10);both[0]=true;
    assert(update(t,both,130)==1); // the hand that started first owns it
    // --- finger ray from the tip along the hand ---
    float o[3],d[3];assert(fingerRay(p,o,d));
    const float len=std::sqrt(d[0]*d[0]+d[1]*d[1]+d[2]*d[2]);assert(std::fabs(len-1.f)<1e-4f);
    assert(o[1]>p.hand[1]+.09f);
    // a finger ray selects the object it points at and not the one beside it (5 deg, 6 cm)
    kcd2vr::interaction112::Ray f{};f.ox=o[0];f.oy=o[1];f.oz=o[2];f.x=d[0];f.y=d[1];f.z=d[2];
    const float tx=o[0]+d[0]*2.f,ty=o[1]+d[1]*2.f,tz=o[2]+d[2]*2.f;
    assert(kcd2vr::interaction57c1::Project(f,tx,ty,tz,kPointReachM,kPointRadiusM,kPointHalfDeg).inside);
    assert(!kcd2vr::interaction57c1::Project(f,tx+.5f,ty,tz,kPointReachM,kPointRadiusM,kPointHalfDeg).inside);
    {Hand q=p;q.dir[0]=q.dir[1]=q.dir[2]=0;assert(!fingerRay(q,o,d));}
    std::puts("view_select_r137_test OK");
}
