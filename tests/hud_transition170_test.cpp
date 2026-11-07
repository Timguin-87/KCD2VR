#include "../src/ui_hud_transition170.h"
#include <cassert>
#ifdef NDEBUG
#error HUD transition tests require active assertions
#endif
using namespace kcd2vr;
struct Backend {
    std::uint64_t generation=1;bool available=true,fail=false;
    unsigned calls=0,writes=0;std::array<bool,ui_contextual_hud162r1::Clips.size()> visible{},absent{};
    std::array<std::uintptr_t,ui_contextual_hud162r1::Clips.size()> ids{};
    Backend(){visible.fill(true);for(unsigned i=0;i<ids.size();++i)ids[i]=i+1;}
    bool Acquire(){++calls;return available;}
    std::uint64_t Generation(){return generation;}
    void Release(){++calls;}
    bool Resolve(unsigned i,const char*,std::uintptr_t&id){++calls;id=ids[i];return !absent[i];}
    bool Get(unsigned i,bool&v){++calls;v=visible[i];return true;}
    bool Set(unsigned i,bool v){++calls;++writes;if(fail)return false;visible[i]=v;return true;}
};
int main(){
    namespace p=ui_hud_transition170;namespace h=ui_contextual_hud162r1;
    h::Input lost{};auto dismissed=p::VisibleWhileLooking(lost,{true,true,true});
    assert(dismissed.active&&!dismissed.left&&!dismissed.right);
    p::Controller owner;Backend io;
    // Full manual mode: independent disabled crosshair and status preferences.
    auto result=owner.Update(io,{false,h::HideCrosshair163|h::HideStatus163,1},1);
    assert(result.changed&&result.failures==0&&io.visible[0]&&io.visible[1]&&!io.visible[2]&&!io.visible[13]&&!io.visible[14]);
    assert(!io.visible[15]&&!io.visible[19]); // outline + central actioncursor artwork
    assert(!owner.Needs({false,h::HideCrosshair163|h::HideStatus163,1},1));
    assert(owner.Needs({false,0,1},1)); // restoration is a real event
    assert(owner.Needs({false,h::HideCrosshair163|h::HideStatus163,2},1));
    assert(owner.Needs({false,h::HideCrosshair163|h::HideStatus163,1},2));
    const auto calls=io.calls;
    for(unsigned i=0;i<10000;++i)assert(!owner.Update(io,{false,h::HideCrosshair163|h::HideStatus163,1},1).changed);
    assert(io.calls==calls); // no lookups/getters/setters or timer keepalive at steady state
    io.visible[8]=false; // native-hidden notification before contextual entry
    result=owner.Update(io,{true,7,1},1);assert(result.changed&&!io.visible[0]&&!io.visible[1]);
    for(unsigned i:{16u,17u,18u})assert(io.visible[i]); // prompts/tutorial/subtitles untouched
    const auto hiddenCalls=io.calls;
    for(unsigned i=0;i<10000;++i)owner.Update(io,{true,7,1},1);
    assert(io.calls==hiddenCalls);
    owner.Update(io,{true,h::Right|h::Other,1},1);
    assert(io.visible[0]&&!io.visible[1]&&!io.visible[2]); // left palm: compass only
    owner.Update(io,{true,h::Left|h::Other,1},1);
    assert(!io.visible[0]&&io.visible[1]&&io.visible[2]&&!io.visible[3]&&!io.visible[4]); // right palm: health/status only
    const auto revealCalls=io.calls;
    for(unsigned i=0;i<10000;++i)owner.Update(io,{true,h::Left|h::Other,1},1);
    assert(io.calls==revealCalls);
    owner.Update(io,{true,h::Other,1},1);assert(io.visible[0]&&io.visible[1]&&io.visible[2]);
    owner.Update(io,{true,7,1},1);assert(!io.visible[0]&&!io.visible[1]);
    owner.Update(io,{false,h::HideCrosshair163|h::HideStatus163,1},1);
    assert(io.visible[0]&&io.visible[1]&&!io.visible[2]&&!io.visible[8]&&!io.visible[13]&&!io.visible[14]);
    // A native-hidden clip must never be force-shown by enabling a user preference.
    p::Controller nativeOwner; Backend nativeIo; nativeIo.visible[0]=false;
    nativeOwner.Update(nativeIo,{true,0,1},1);assert(!nativeIo.visible[0]);
    nativeOwner.Update(nativeIo,{true,h::Left,1},1);assert(!nativeIo.visible[0]);
    nativeOwner.Update(nativeIo,{true,0,1},1);assert(!nativeIo.visible[0]);
    // Missing HUD, failed setter, absent clip: bounded attempts; no frame retry loop.
    io.available=false;result=owner.Update(io,{true,7,2},1);assert(result.failures==1);
    auto before=io.calls;for(int i=0;i<1000;++i)owner.Update(io,{true,7,2},1);assert(io.calls==before);
    io.available=true;io.fail=true;result=owner.Update(io,{true,7,3},1);assert(result.failures>0);
    before=io.calls;owner.Update(io,{true,7,3},1);assert(io.calls==before);
    io.fail=false;io.absent[2]=true;io.generation=2;io.ids[0]=200;io.visible.fill(true);
    result=owner.Update(io,{true,7,3},2);assert(result.changed&&!io.visible[0]&&io.visible[2]&&result.absent==1);
    owner.Restore(io);assert(io.visible[0]); // current lifetime only
    // Native reacquisition may advance generation; do not perform an extra update.
    p::Controller creation;Backend fresh;fresh.generation=4;
    creation.Update(fresh,{true,7,0},0);before=fresh.calls;
    creation.Update(fresh,{true,7,0},4);assert(fresh.calls==before);
}
