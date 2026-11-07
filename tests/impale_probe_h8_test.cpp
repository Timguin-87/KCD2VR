#define main H7UnusedMain
#include "corpse_contact_native_h7_test.cpp"
#undef main
#include "vrik/impale47.h"
static std::atomic<void*> g_last_player_entity{nullptr};
static std::uintptr_t proxyBodyH8=rootBody;
static bool NpcBodyRead172(void* e,void*& out){if(e!=reinterpret_cast<void*>(ent))return false;out=reinterpret_cast<void*>(proxyBodyH8);return true;}
namespace kcd2vr::frik_pose_probe {
#include <impale_probe_h8.inc>
}
static void prepare(){setup();f::g_corpse_contact_fault_h7.clear();proxyBodyH8=rootBody;put(rootBody,base+0x3A82570);}
int main(){using namespace kcd2vr;using pa=partanchor59c7g::Identity;
 pa id{1,1,1,456,ent,body,379};std::uintptr_t root=0;const char* why="";
 prepare();claude_h8::impaleMainProbe=false;
 assert(f::ImpaleRoot47(id,root,why)==impale47::Poll::NotYet&&root==0&&queries==0);
 prepare();claude_h8::impaleMainProbe=true;
 assert(f::ImpaleRoot47(id,root,why)==impale47::Poll::NotYet&&root==0&&queries==1);
 assert(!std::strcmp(why,"MAIN_PROBE_READBACK_ONLY_PART_CONTINUITY_UNPROVEN"));
 physical::Pose pose{};double scale=0;const auto old=queries;
 assert(!f::ReadImpalePart59c7g(id,pose,scale)&&queries==old); // No new anchor/driver admission.
 for(int m:{1,2,3,5}){prepare();mode=m;assert(f::ImpaleRoot47(id,root,why)!=impale47::Poll::Ready&&root==0);}
 prepare();pa stale=id;stale.body=oldBody;assert(f::ImpaleRoot47(stale,root,why)!=impale47::Poll::Ready&&oldReads==0);
 prepare();mode=4;assert(f::ImpaleRoot47(id,root,why)==impale47::Poll::Fatal&&f::GeometryCorpseContactFaultH7());
 prepare();proxyBodyH8=body;put(body+0x28,std::int16_t(1));put(body+0x20,ent);put(base+0x3A48B30+0x30,base+0x8A9348);
 assert(f::ImpaleRoot47(id,root,why)==impale47::Poll::Ready&&root==body); // Existing exact root route retained.
}
