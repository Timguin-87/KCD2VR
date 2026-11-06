// Full production animation TU. Native memory/calls below are explicit fixture
// leaves; no game execution, visual result or Windows SEH claim.
#ifndef PRODUCTION_ANIMATION_SOURCE_D50
#define PRODUCTION_ANIMATION_SOURCE_D50 "../src/animation_filter.cpp"
#endif
#include PRODUCTION_ANIMATION_SOURCE_D50
#include <cstdio>
#include <string>
namespace kcd2vr::retail_build112 {
bool verifiedD50=true;
bool Ready(std::uintptr_t module) noexcept {return module&&verifiedD50;}
}
static unsigned bodyGateWritesD50=0;
namespace kcd2vr::diagnostic_trace {void Write(const char* line) noexcept {
    if(line&&std::strstr(line,"V66.178R30C1 BODY_GATE "))++bodyGateWritesD50;
}}
namespace kcd2vr::frik_pose_probe {
std::uint64_t PoseEpochH24() noexcept {return 0;}
bool NativeFreeKickVisualR24() noexcept {return false;}
}
using namespace kcd2vr::animation_filter;
static unsigned checksD50=0,failuresD50=0,callsD50=0;
static void ckD50(bool value,const char* why){++checksD50;if(!value){++failuresD50;std::printf("FAIL %s\n",why);}}
template<class T>static void putD50(unsigned char* at,T value){std::memcpy(at,&value,sizeof(value));}
static bool originalIdD50(void*,int,void*){++callsD50;return false;}
static bool originalNameD50(void*,const char*,void*){++callsD50;return false;}
int main(){
 namespace ride=kcd2vr::riding_vrik_d76;
 std::vector<unsigned char> image(0x1000),anim(0x600),character(0xB00),skeleton(0x300),set(0x40),aliases(4+6*16),queue(4+0x78),transfer(4+0x78);
 hostModuleHandleW=image.data();const auto base=WhBase();
 putD50(anim.data(),base+kCSkeletonAnimVftRva);putD50(anim.data()+0x4E8,character.data());
 putD50(character.data()+0xA70,skeleton.data());putD50(skeleton.data()+0x218,set.data());
 putD50(set.data()+8,aliases.data()+4);putD50(aliases.data(),std::uint32_t(6));
 for(unsigned i=0;i<6;++i)putD50(aliases.data()+4+i*16,std::uint64_t(100+i));
 g_alias_catalog6694={{100,"2d_horse_walk.bspace"},{101,"horse_mount_left_02"},
 {102,"horse_mount_left_02_align"},{103,"horse_dismount_left_02"},
 {104,"2d_horse_gallop.bspace"},{105,"combat_finish_lngsw_01"}};
 g_alias_ready6694=true;g_alias_fault6694=false;g_playback_layout_ready6690=true;g_player_skeleton_anim=anim.data();
 kcd2vr::diagnostics::lab::state=3;kcd2vr::diagnostics::capture168::gate.mask=0;
 ride::Set(true);
 putD50(queue.data(),std::uint32_t(1));putD50(queue.data()+4+0x10,0);putD50(queue.data()+4+0x50,std::uint16_t(1));
 putD50(transfer.data(),std::uint32_t(1));putD50(transfer.data()+4+0x10,1);putD50(transfer.data()+4+0x50,std::uint16_t(1));
 putD50(anim.data()+0x48,queue.data()+4);putD50(anim.data()+0x40+0x48,transfer.data()+4);
 bool riding=false;hostTickCount64=1000;
 ckD50(NativeBodyRequired112(anim.data(),&riding)&&!riding,"mounting stays native despite simultaneous gait");
 hostTickCount64=8000;putD50(transfer.data()+4+0x10,2);
 ckD50(NativeBodyRequired112(anim.data(),&riding)&&!riding,"alignment stays native and does not start completion delay");
 hostTickCount64=10000;putD50(transfer.data()+4+0x50,std::uint16_t(0));
 ckD50(NativeBodyRequired112(anim.data(),&riding)&&!riding,"positive completed seat starts the one-second post-transfer settle");
 hostTickCount64=10999;
 ckD50(NativeBodyRequired112(anim.data())&&NativeBodyRequired112(anim.data(),&riding)&&!riding,"all consumers remain native through 999ms of completed seat");
 hostTickCount64=11000;
 ckD50(!NativeBodyRequired112(anim.data(),&riding)&&riding,"seated admission remains valid with empty hands");
 ckD50(!NativeBodyRequired112(anim.data()),"callers without riding out pointer agree on normal VRIK");
 putD50(queue.data()+4+0x10,4);hostTickCount64=11001;
 ckD50(!NativeBodyRequired112(anim.data(),&riding)&&riding,"gait change does not restart completed deadline");
 putD50(transfer.data()+4+0x10,5);putD50(transfer.data()+4+0x50,std::uint16_t(1));
 ckD50(NativeBodyRequired112(anim.data(),&riding)&&!riding,"real protected action still owns its native body");
 putD50(transfer.data()+4+0x50,std::uint16_t(0));hostTickCount64=11002;
 ckD50(!NativeBodyRequired112(anim.data(),&riding)&&riding,"return from action keeps completed mounted session");
 putD50(transfer.data()+4+0x10,3);putD50(transfer.data()+4+0x50,std::uint16_t(1));
 ckD50(NativeBodyRequired112(anim.data(),&riding)&&!riding,"dismount revokes mounted VRIK immediately");
 putD50(transfer.data()+4+0x10,1);hostTickCount64=12000;
 ckD50(NativeBodyRequired112(anim.data(),&riding)&&!riding,"new mounting occurrence remains native");
 putD50(transfer.data()+4+0x50,std::uint16_t(0));hostTickCount64=15000;
 ckD50(NativeBodyRequired112(anim.data(),&riding)&&!riding,"new mount starts its own settle without an old deadline");
 hostTickCount64=16000;ckD50(!NativeBodyRequired112(anim.data(),&riding)&&riding,"new mount remains active on its own current seat");
 ride::Set(false);ckD50(NativeBodyRequired112(anim.data(),&riding)&&!riding,"explicit riding OFF remains native");
 ride::Set(true);hostTickCount64=17000;ckD50(NativeBodyRequired112(anim.data(),&riding)&&!riding,"re-enabled riding starts a current positive seat settle");
 hostTickCount64=18000;ckD50(!NativeBodyRequired112(anim.data(),&riding)&&riding,"re-enabled riding starts from positive mounted observation");
 putD50(transfer.data()+4+0x10,1);putD50(transfer.data()+4+0x50,std::uint16_t(1));hostTickCount64=18010;
 ckD50(NativeBodyRequired112(anim.data(),&riding)&&!riding,"third native mounting resets completion");
 putD50(transfer.data()+4+0x10,5);hostTickCount64=18020;
 ckD50(NativeBodyRequired112(anim.data(),&riding)&&!riding,"simultaneous protected action remains native at observed mount completion");
 putD50(transfer.data()+4+0x50,std::uint16_t(0));hostTickCount64=19020;
 ckD50(!NativeBodyRequired112(anim.data(),&riding)&&riding,"first post-mount action cannot postpone or restart the completion deadline");
 putD50(queue.data()+4+0x10,44);ckD50(NativeBodyRequired112(anim.data(),&riding)&&!riding,"unknown current alias fails closed and retires session");
 putD50(queue.data()+4+0x10,0);ckD50(NativeBodyRequired112(anim.data(),&riding)&&!riding,"fresh qualified seat starts bounded recovery after unknown playback");
 hostTickCount64+=1000;ckD50(!NativeBodyRequired112(anim.data(),&riding)&&riding,"fresh seat recovers after its one-second settle");
 ckD50(kcd2vr::diagnostics::capture168::gate.mask.load()==0,"repair does not turn on diagnostics");
 std::printf("D81 riding native handoff %u/%u; synthetic native leaves, headset NOT_RUN\n",checksD50-failuresD50,checksD50);
 return failuresD50?1:0;
}
