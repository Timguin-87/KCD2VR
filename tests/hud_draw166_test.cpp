#include "ui_hud_draw166.h"
#include "vrik/support_grip166.h"
#include "vrik/native_transform166.h"
#include <cassert>
#include <cstdio>
using namespace kcd2vr::ui_hud_draw166;
struct Backend {
    std::array<Display,Clips.size()> value{};
    int fail=-1,missing=-1;bool failed=false;unsigned writes=0;
    bool Resolve(unsigned i,const char*){return int(i)!=missing;}
    bool ReadDisplay(unsigned i,Display& d){d=value[i];return true;}
    bool WriteDisplay(unsigned i,Display d,unsigned fields){
        ++writes;if(fields&1)value[i].visible=d.visible;
        if(fields&2){assert(d.xs!=0&&d.ys!=0);value[i].xs=d.xs;value[i].ys=d.ys;}
        if(int(i)==fail&&!failed){failed=true;return false;}return true;
    }
};
int main(){
    Backend io;io.value[1].visible=false;
    {Mask<Backend> mask(io);assert(mask.Begin(Left,{1,1,1}));assert(!io.value[1].visible&&!io.value[2].visible);
     assert(io.value[2].xs==100);assert(mask.Restore());assert(!io.value[1].visible&&io.value[2].visible);
     assert(mask.writes==2*(mask.reads-1));}
    {Mask<Backend> mask(io);assert(mask.Begin(HideStatus163,{1,1,1}));assert(!io.value[2].visible&&io.value[3].visible);}
    assert(io.value[2].visible);
    {Mask<Backend> mask(io);assert(mask.Begin(HideLower163,{1,1,1}));assert(io.value[2].visible&&!io.value[3].visible);}
    {Mask<Backend> mask(io);assert(mask.Begin(HideCrosshair163,{1,1,1}));assert(!io.value[19].visible&&io.value[0].visible);}
    io.value[16].xs=180;io.value[18].ys=90;
    {Mask<Backend> mask(io);assert(mask.Begin(0,{.5f,1,.7f}));assert(io.value[16].xs==90&&io.value[18].ys==63);}
    assert(io.value[16].xs==180&&io.value[18].ys==90);
    io.fail=3;{Mask<Backend> mask(io);assert(!mask.Begin(Left,{1,1,1}));}
    assert(io.value[2].visible&&io.value[3].visible&&!io.value[1].visible);
    io.missing=19;{Mask<Backend> mask(io);assert(mask.Begin(HideCrosshair163,{1,1,1}));assert(io.value[19].visible);}
    {Mask<Backend> mask(io);const auto before=io.writes;assert(!mask.Begin(Left,{0,1,1}));assert(io.writes==before);}
    using namespace kcd2vr::support_grip166;
    assert(OnHilt({},{0,1,0},{0,-.09,0}));assert(!OnHilt({},{0,1,0},{0,.09,0}));
    assert(!OnHilt({},{0,1,0},{.11,-.09,0}));
    double lo[]{-.05,-.1,-.05},hi[]{.05,1,.05};
    auto h=Fallback({},{0,1,0},lo,hi,.12);assert(h&&h->y<-.09&&h->y>=-.1);
    lo[1]=-.03;assert(!Fallback({},{0,1,0},lo,hi,.12)); // never place it on the blade instead
    using namespace kcd2vr::native_transform166;
    kcd2vr::physical::Pose w{{0,0,.70710678*1.000002,.70710678*1.000002},{3194.63,495.68,42.53}};
    kcd2vr::physical::Pose inverse{m::conjugate(w.q),Rotate(m::conjugate(w.q),w.p*-1)};
    auto result=InversePair(w,inverse);assert(result.valid&&result.legacyClosure>.002);
    inverse.p.x+=.003;assert(!InversePair(w,inverse).valid); // no tolerance expansion
    inverse.p.x-=.003;inverse.q={};assert(!InversePair(w,inverse).valid);
    w.q.w=2;assert(!InversePair(w,inverse).valid);
    std::puts("PASS retail inverse-pair arithmetic, far-origin rounding and corrupt-transform rejection");
    std::puts("PASS HUD exact restoration/native-hidden/partial failure/separate preferences/scales and longsword hilt admission");
}
