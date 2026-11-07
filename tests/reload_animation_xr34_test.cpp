#define main RetainedAnimationMainXR34
#include "animation_ownership112_bridge.cpp"
#undef main
namespace kcd2vr::frik_pose_probe {std::uint64_t PoseEpochH24() noexcept{return 1;}}
int main(){
    RetainedAnimationMainXR34();checks=0;
    std::vector<unsigned char> image(0x1000),anim(0x600),character(0xB00),skeleton(0x300),set(0x40),aliases(0x40),queue(4+0x78),other(4+0x78);
    hostModuleHandleW=image.data();const auto base=WhBase();
    put(anim.data(),base+kCSkeletonAnimVftRva);put(anim.data()+0x4E8,character.data());
    put(character.data()+0xA70,skeleton.data());put(skeleton.data()+0x218,set.data());
    put(set.data()+8,aliases.data()+4);put(aliases.data(),std::uint32_t(2));
    put(aliases.data()+4,std::uint64_t(123));put(aliases.data()+20,std::uint64_t(456));
    g_alias_catalog6694={{123,"current_exact_reload"},{456,"combat_finish_lngsw_01"}};
    g_alias_ready6694=true;g_alias_fault6694=false;g_playback_layout_ready6690=true;g_player_skeleton_anim=anim.data();
    put(anim.data()+0x48,queue.data()+4);put(queue.data(),1u);put(queue.data()+4+0x10,0);put(queue.data()+4+0x50,std::uint16_t(1));
    const auto entry=reinterpret_cast<std::uintptr_t>(queue.data()+4);kcd2vr::retail_build112::hostVerified112=true;nativeBodyOwned112=true;
    kcd2vr::native_passenger_a34::witness.publish({});
    ck(ReloadPassengerOnlyXR34(anim.data(),entry,0)&&nativeBodyOwned112.load(),"exact CAF query grants no general body ownership");
    ck(!ReloadPassengerOnlyXR34(anim.data(),entry+8,0),"same alias cannot borrow another entry identity");
    ck(!ReloadPassengerOnlyXR34(anim.data(),entry,1),"same entry cannot borrow another selected CAF id");
    put(queue.data()+4+0x50,std::uint16_t(0));ck(!ReloadPassengerOnlyXR34(anim.data(),entry,0),"inactive selected CAF is not a passenger source");
    put(queue.data()+4+0x50,std::uint16_t(1));
    put(anim.data()+4*0x40+0x48,other.data()+4);put(other.data(),1u);put(other.data()+4+0x10,1);put(other.data()+4+0x50,std::uint16_t(1));
    ck(!ReloadPassengerOnlyXR34(anim.data(),entry,0),"concurrent protected paired/finisher FIFO retains all native ownership");
    put(other.data()+4+0x10,999);ck(!ReloadPassengerOnlyXR34(anim.data(),entry,0),"unknown simultaneous native clip fails closed");
    put(anim.data()+4*0x40+0x48,static_cast<void*>(nullptr));
    for(auto crc:{0xeed2e729u,0x591f4107u,0x735721aau,0x04ab0eefu,0xc630055bu,0xd860d990u}){
        kcd2vr::native_passenger_a34::Root r{};r.controller=1;r.character=reinterpret_cast<std::uintptr_t>(anim.data())-0x110;
        r.action=2;r.scope=3;r.context=4;r.epoch=1;r.sourceTick=r.tick=hostTickCount64;r.serial=1;r.crc=crc;r.fragment=1;r.status=2;r.layer=0;
        kcd2vr::native_passenger_a34::witness.publish(r);
        ck(!ReloadPassengerOnlyXR34(anim.data(),entry,0),"each existing A34 activity refuses the reload-only exception");
    }
    kcd2vr::native_passenger_a34::witness.publish({});kcd2vr::retail_build112::hostVerified112=false;
    ck(!ReloadPassengerOnlyXR34(anim.data(),entry,0),"retail mismatch refuses the passenger query");kcd2vr::retail_build112::hostVerified112=true;
    g_playback_layout_ready6690=false;ck(!ReloadPassengerOnlyXR34(anim.data(),entry,0),"unqualified FIFO schema refuses the passenger query");
    std::printf("PASS %d production XR34 animation exclusion checks\n",checks);
}
