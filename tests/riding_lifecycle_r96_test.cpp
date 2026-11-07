// Actual animation_filter.cpp entry points with bounded in-memory retail leaves.
// The assertions concern ownership/lifecycle, not game execution or headset display.
#define main prior_riding_handoff_fixture_r96
#include "riding_handoff_d81_test.cpp"
#undef main
#include "../src/subsystems/throw_drop/inventory_lifecycle_t8.h"

int main() {
 namespace ride=kcd2vr::riding_vrik_d76;
 namespace load=kcd2vr::inventory_lifecycle_t8;
 checksD50=failuresD50=0;
 std::vector<unsigned char> image(0x1000),anim(0x600),character(0xB00),skeleton(0x300),set(0x40),aliases(4+5*16),queue(4+0x78),transfer(4+0x78);
 hostModuleHandleW=image.data();const auto base=WhBase();
 putD50(anim.data(),base+kCSkeletonAnimVftRva);putD50(anim.data()+0x4E8,character.data());
 putD50(character.data()+0xA70,skeleton.data());putD50(skeleton.data()+0x218,set.data());
 putD50(set.data()+8,aliases.data()+4);putD50(aliases.data(),std::uint32_t(5));
 for(unsigned i=0;i<5;++i)putD50(aliases.data()+4+i*16,std::uint64_t(200+i));
 g_alias_catalog6694={{200,"relaxed_idle_rider_01"},{201,"horse_mount_left_02"},
  {202,"horse_dismount_left_02"},{203,"2d_combat_rg_horse_walk_sword.bspace"},{204,"combat_finish_lngsw_01"}};
 g_alias_ready6694=true;g_alias_fault6694=false;g_playback_layout_ready6690=true;g_player_skeleton_anim=anim.data();
 kcd2vr::diagnostics::lab::state=3;kcd2vr::diagnostics::capture168::gate.mask=0;
 ride::Set(true);g_riding_session_d81={};load::loadEpisode18=1;
 putD50(queue.data(),std::uint32_t(1));putD50(queue.data()+4+0x10,0);putD50(queue.data()+4+0x50,std::uint16_t(1));
 putD50(transfer.data(),std::uint32_t(1));putD50(transfer.data()+4+0x10,1);putD50(transfer.data()+4+0x50,std::uint16_t(1));
 putD50(anim.data()+0x48,queue.data()+4);putD50(anim.data()+6*0x40+0x48,transfer.data()+4);
 bool riding=false;std::uint64_t lifetime=0;
 const auto scan=[&](std::uint64_t now){hostTickCount64=now;return NativeBodyRequired112(anim.data(),&riding,&lifetime);};
 ckD50(scan(1000)&&!riding&&!lifetime,"native transfer on any inspected layer cannot be overridden by seated idle");
 ckD50(scan(9000)&&!riding,"elapsed mounting time never counts toward the post-transfer settle");
 putD50(transfer.data()+4+0x50,std::uint16_t(0));
 ckD50(scan(10000)&&!riding&&!lifetime,"first completed seated observation starts the requested one-second settle");
 ckD50(scan(10999)&&!riding&&!lifetime,"999ms of stable seat remains entirely native");
 ckD50(NativeBodyRequired112(anim.data()),"consumer without optional output agrees during settling");
 ckD50(!scan(11000)&&riding&&lifetime,"1000ms admits full tracked upper body even with empty hands");
 const auto visit1=lifetime;
 ckD50(!NativeBodyRequired112(anim.data()),"consumer without optional output agrees after handover");
 putD50(queue.data()+4+0x10,3);
 ckD50(!scan(11010)&&riding&&lifetime==visit1,"sword gait retains the admitted visit without a second settle");
 putD50(transfer.data()+4+0x10,4);putD50(transfer.data()+4+0x50,std::uint16_t(1));
 ckD50(scan(11020)&&!riding&&!lifetime,"protected native action keeps its semantic body owner");
 putD50(transfer.data()+4+0x50,std::uint16_t(0));
 ckD50(!scan(11030)&&riding&&lifetime==visit1,"completed same seat resumes after a protected overlay without a new mount delay");
 putD50(transfer.data()+4+0x10,2);putD50(transfer.data()+4+0x50,std::uint16_t(1));
 ckD50(scan(12000)&&!riding&&!lifetime,"dismount immediately revokes the old seat");
 putD50(transfer.data()+4+0x10,1);
 ckD50(scan(13000)&&!riding,"remount stays native");
 putD50(transfer.data()+4+0x50,std::uint16_t(0));
 ckD50(scan(14000)&&!riding,"remount cannot reuse the previous completed deadline");
 ckD50(!scan(15000)&&riding&&lifetime!=visit1,"remount gets a fresh seat lifetime after its own settle");
 const auto visit2=lifetime;
 // A save can reuse the same character, skeleton pointer, pose epoch and alias.
 // Its independently observed load episode must still retire the old seat.
 load::loadEpisode18.fetch_add(1);
 ckD50(scan(15010)&&!riding&&!lifetime,"same-address save/load starts a new seat even when pose epoch did not change");
 ckD50(!scan(16010)&&riding&&lifetime!=visit2,"save/load reentry recovers from a current seat after one second");
 const auto visit3=lifetime;
 kcd2vr::diagnostics::lab::state=1;
 ckD50(!scan(17000)&&!riding&&!lifetime,"native-reference policy never grants mounted VRIK");
 kcd2vr::diagnostics::lab::state=3;
 ckD50(scan(18000)&&!riding,"return from native-reference mode cannot retain a stale seated lease");
 ckD50(!scan(19000)&&riding&&lifetime!=visit3,"mode return recovers without restarting the process");
 ride::Set(false);ckD50(scan(20000)&&!riding,"riding option OFF remains native");
 ride::Set(true);ckD50(scan(21000)&&!riding,"re-enabled riding waits on its current seat");
 ckD50(!scan(22000)&&riding,"re-enabled riding recovers after settle");
 putD50(queue.data()+4+0x10,44);
 ckD50(scan(23000)&&!riding,"unknown playback retires that visit");
 putD50(queue.data()+4+0x10,0);
 ckD50(scan(23001)&&!riding,"fresh known playback starts a bounded reentry");
 ckD50(!scan(24001)&&riding,"unknown playback never creates a permanent native fallback latch");
 ckD50(scan(100)&&!riding,"backward time cannot reuse a completed seat");
 ckD50(!scan(1100)&&riding,"clock discontinuity recovers on the next completed settle");
 ckD50(kcd2vr::diagnostics::capture168::gate.mask.load()==0,"repair never arms diagnostics");
 std::printf("R96 riding lifecycle %u/%u; actual native adapter; memory and clock leaves synthetic; game/HMD NOT_RUN\n",checksD50-failuresD50,checksD50);
 return failuresD50?1:0;
}
