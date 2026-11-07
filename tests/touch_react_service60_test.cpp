// DS-F97/F101: touch-reaction service (pure + seams, host-tested). The owner
// (Henry's character pointer, a real-width address) is DISTINCT from the
// contacted target entity - the Codex touch-identity correction.
#include "../src/subsystems/physics/touch_react_service60.h"
#include <cstdio>
#include <cmath>
#include <cstdint>
namespace ts=kcd2vr::touch_react_service60;namespace tr=kcd2vr::touch_react60;namespace p=kcd2vr::passive169r1;
static int fails=0;
#define CK(c) do{if(!(c)){std::printf("FAIL %s:%d %s\n",__FILE__,__LINE__,#c);++fails;}}while(0)
static int fleshCalls=0;
static void flesh(const float*pos,const float*nrm,std::uint32_t e,float str) noexcept{
    CK(pos&&nrm&&e==77&&str>.0f&&str<1.f);++fleshCalls;}
// Henry's character pointer: a REAL-WIDTH owner address (0x1F2E3D4C5B6A7980),
// deliberately distinct from the 32-bit target entity id 77.
static constexpr std::uint64_t kHenryCharacter=0x1F2E3D4C5B6A7980ull;
static p::Sample handContact(){p::Sample s{};s.source=p::Source::HandWorld;s.status=p::Status::Contact;
    s.side=0;s.actor=kHenryCharacter;s.point={0,1,0};s.normal={0,0,1};s.tick=5000;return s;}
int main(){
    ts::fleshEffect=&flesh;
    {ts::Witness w{};w.living=true;w.softMask=tr::kChest;w.part=1;
     auto r=ts::service(handContact(),77,w,5000);
     CK(r.fired&&r.p.ok&&fleshCalls==1);} // one flesh effect + the haptic plan
    {ts::Witness w{};w.living=true;w.softMask=tr::kChest;
     auto r=ts::service(handContact(),0,w,5200);CK(!r.fired);} // no proven target: nothing fires
    {ts::Witness w{};w.living=false; // a corpse touch: no soft reaction
     auto r=ts::service(handContact(),77,w,5400);CK(!r.fired&&r.p.why==tr::Why::NotLiving);}
    {p::Sample s=handContact();s.status=p::Status::Clear; // no contact: nothing
     ts::Witness w{};w.living=true;w.softMask=tr::kChest;
     auto r=ts::service(s,77,w,5600);CK(!r.fired);}
    {p::Sample s=handContact();s.source=p::Source::WeaponWorld; // a weapon touch is combat, not a touch
     ts::Witness w{};w.living=true;w.softMask=tr::kChest;
     auto r=ts::service(s,77,w,5800);CK(!r.fired);}
    // ACTUAL production publisher/drain: contact edge haptic is default even
    // with no soft-anatomy mapping and F9 OFF. It never calls flesh/native FX.
    kcd2vr::haptics101::enabled=true;kcd2vr::haptics101::allowed=true;
    auto s=handContact();s.eligible=s.worldCoherent=true;s.epoch=1;s.request=2;s.sequence=1;s.tick=6000;
    kcd2vr::npc_contact172::Witness w{};w.body=kcd2vr::npc_contact172::Body::Living;w.entityId=77;w.collider=0x100;w.owner=0x200;w.ownerRead=true;
    ts::softMaskFor=nullptr;const auto beforeFlesh=fleshCalls;
    ts::publishPlan(s,w);ts::drain(6000);kcd2vr::haptics101::Pulse pulse{};CK(kcd2vr::haptics101::queue[0].read(pulse)&&pulse.tick==6000);
    const auto sequence=pulse.sequence;ts::drain(6010);CK(kcd2vr::haptics101::queue[0].read(pulse)&&pulse.sequence==sequence);
    ++s.sequence;s.tick=6020;ts::publishPlan(s,w);ts::drain(6020);CK(kcd2vr::haptics101::queue[0].read(pulse)&&pulse.sequence==sequence);
    ++s.sequence;s.tick=6030;s.status=p::Status::Clear;ts::publishPlan(s,w);ts::drain(6030);
    ++s.sequence;s.tick=6400;s.status=p::Status::Contact;ts::publishPlan(s,w,true);ts::drain(6400);CK(kcd2vr::haptics101::queue[0].read(pulse)&&pulse.sequence==sequence);
    ++s.sequence;s.tick=6500;ts::publishPlan(s,w);ts::drain(6581);CK(kcd2vr::haptics101::queue[0].read(pulse)&&pulse.sequence==sequence); // stale
    ++s.sequence;s.tick=6600;ts::publishPlan(s,w);ts::drain(6600);CK(kcd2vr::haptics101::queue[0].read(pulse)&&pulse.sequence!=sequence);
    CK(fleshCalls==beforeFlesh);
    if(fails)return 1;std::puts("touch react service + actual default fresh edge haptic OK; no native FX");return 0;
}
