#include <cstdio>
#include "../src/vrik/impale_hold60.h"
namespace ih=kcd2vr::impale_hold60;namespace cp=kcd2vr::constraint_packet60;
static int fails=0;
#define CK(x) do{if(!(x)){std::printf("FAIL %s:%d %s\n",__FILE__,__LINE__,#x);++fails;}}while(0)
static ih::Request base(){
    ih::Request r{};r.serial=7;r.ragdollBody=0x1000;r.weaponBody=0x2000;r.part=5;
    const double g[3]{0,0,1},t[3]{0,1.0,1},w[3]{0.01,0.95,1.0};
    for(int i=0;i<3;++i){r.grip[i]=g[i];r.tip[i]=t[i];r.wound[i]=w[i];}
    return r;
}
int main(){
    {auto p=ih::plan(base());CK(p.ok&&p.why==ih::Why::None);
     CK(cp::get<std::uint32_t>(p.add,0)==5);CK(cp::get<std::uintptr_t>(p.add,8)==0x2000);
     CK(cp::get<std::uint32_t>(p.add,0x28)==5&&cp::get<std::uint32_t>(p.add,0x2C)==cp::kIntUnset);
     CK((cp::get<std::uint32_t>(p.add,0x60)&0x100u)==0); // never inactive
     CK(cp::get<std::uint32_t>(p.add,0x04)==cp::kIntUnset); // id allocated by the engine
     CK(cp::get<float>(p.add,0x10)==cp::get<float>(p.add,0x1C)); // pt0 == pt1 (world, created at rest)
     CK(cp::get<std::uint64_t>(p.add,0x80)==0xFFFFFFFFFFFFFFFFull);
     CK(std::fabs(p.anchor[1]-0.95)<1e-6||std::fabs(p.anchor[1]-1.0)<=1.0);} // inside the blade
    {auto r=base();r.weaponBody=0;CK(ih::plan(r).why==ih::Why::NoBuddy);}
    {auto r=base();r.ragdollBody=0;CK(ih::plan(r).why==ih::Why::NoBody);}
    {auto r=base();r.part=-1;CK(ih::plan(r).why==ih::Why::NoPart);}
    {auto r=base();r.serial=0;CK(ih::plan(r).why==ih::Why::BadInput);}
    {auto r=base();r.wound[0]=NAN;CK(ih::plan(r).why==ih::Why::BadInput);}
    {auto r=base();r.wound[1]=3.0;CK(!ih::plan(r).ok);}   // wound far past the tip
    {auto r=base();r.wound[0]=1.5;r.wound[1]=0.8;CK(ih::plan(r).why==ih::Why::OffBlade||ih::plan(r).why==ih::Why::TipFar);} // wound off the blade line
    // lenient: blade moved 20 cm after the receipt (tip is 20 cm past the wound) still holds
    {auto r=base();r.wound[1]=0.8;auto p=ih::plan(r);CK(p.ok);CK(p.depth>=0.10&&p.depth<=0.40);}
    // hold lifecycle
    {ih::Hold h;CK(!h.active());CK(h.held(7,1000,0)==ih::Why::BadInput&&!h.active()); // refused add: stay idle -> velocity hang continues
     CK(h.held(7,1000,42)==ih::Why::None&&!h.constraintDrives()&&h.motorBlocked());
     CK(h.present(7,42)&&h.constraintDrives());
     CK(h.held(8,1001,43)==ih::Why::AlreadyHeld);
     CK(h.beginRelease()&&!h.constraintDrives());CK(!h.beginRelease());
     CK(!h.removed(false)&&h.phase==ih::Phase::Quarantined&&h.id==42&&h.motorBlocked());
     CK(!h.beginRelease()&&!h.removed(true)&&h.id==42);
     CK(!h.absent(8,42)&&!h.absent(7,43));CK(h.absent(7,42)&&!h.active());}
    {ih::Hold h;(void)h.held(1,0,9);(void)h.beginRelease();bool done=false;for(int i=0;i<20&&!done;++i)done=h.removed(false);CK(!done&&h.active()&&h.id==9&&h.motorBlocked());} // never forget uncertain entered remove
    {ih::Hold h;CK(h.held(2,100,99)==ih::Why::None);CK(!h.constraintDrives()&&h.motorBlocked());
     CK(h.present(2,99)&&h.beginRelease());CK(!h.removed(true)&&h.id==99&&h.motorBlocked());CK(h.absent(2,99)&&!h.active());}
    if(fails)return 1;std::puts("impale hold60 OK");return 0;
}
