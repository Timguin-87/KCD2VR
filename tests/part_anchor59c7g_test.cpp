#include "../src/vrik/part_anchor59c7g.h"
#include <cassert>
#include <cstring>
#include <memory>
using namespace kcd2vr::partanchor59c7g;
#if defined(_MSC_VER)
__declspec(noinline)
#else
__attribute__((noinline))
#endif
static float nativeTailRead(const void* packet){
 // Retail CRigidEntity::GetStatus +8A6E2D reads this input after the base
 // pose query succeeds, including through CArticulatedEntity's fallback.
 float timeBack=-1;
 std::memcpy(&timeBack,static_cast<const unsigned char*>(packet)+0x78,sizeof(timeBack));
 return timeBack;
}
int main(){
 // Heap allocation makes the former 0x78-byte packet's native tail overread
 // visible to ASan; ordinary pose decoding alone did not exercise this ABI.
 auto packet=std::make_unique<Status>(12);
 assert(nativeTailRead(packet.get())==0.f);
 Status s(12);p::Pose part{};double scale=0;
 assert(!decode(s,12,part,scale));
 s.position[0]=1;s.position[1]=2;s.position[2]=3;s.rotation[0]=s.rotation[1]=s.rotation[2]=0;s.rotation[3]=1;s.scale=2;
 assert(decode(s,12,part,scale)&&scale==2);assert(!decode(s,11,part,scale));
 s.rotation[3]=2;assert(!decode(s,12,part,scale));s.rotation[3]=1;assert(decode(s,12,part,scale));
 Identity id{1,2,3,4,5,6,12};Anchor a;p::Vec3 hit{1.1,2,3},next{};
 assert(a.acquire(id,part,scale,hit));part.p.x+=.1;
 assert(a.update(id,part,scale,next)&&std::abs(next.x-1.2)<1e-8);
 // Rotation follows the struck part, not the actor's root.
 part.q={0,0,std::sqrt(.5),std::sqrt(.5)};
 assert(a.update(id,part,scale,next)&&std::abs(next.x-1.1)<1e-8&&std::abs(next.y-2.1)<1e-8);
 auto other=id;other.body=7;assert(!a.update(other,part,scale,next)&&!a.active);
 assert(a.acquire(id,part,scale,hit));part.p.x+=1;assert(!a.update(id,part,scale,next));
 assert(a.acquire(id,part,scale,part.p));assert(!a.update(id,part,1,next));
 id.part=-1;assert(!a.acquire(id,part,scale,part.p));
}
