// Actual ServiceDiceMarkerR92 + lease predicate, extracted unchanged; only the
// Flash native call/identity leaves and time are substituted.
#include "../src/subsystems/dice/selection_marker_r92.h"
#include "../src/ui_hud_visibility175.h"
#include <mutex>
#include <cstring>
#include <cstdio>
#include <cstdlib>
namespace kcd2vr {
static std::uint64_t now=1000,generation=1;
static std::uint64_t GetTickCount64(){return now;}
static std::uint64_t HudGeneration(){return generation;}
static std::recursive_mutex g_hud_draw_mutex162r1;
static std::atomic<bool> g_hud_repair_ready175{true},g_diceNativeVisibleR92{false};
static core::SnapshotMailbox<ui_hud_visibility175::Key> g_diceKeyR92;
static bool nativeVisible=true,setSucceeds=true,identityValid=true,findSucceeds=true;
static unsigned sets=0;
static bool DiceKeyR92(ui_hud_visibility175::Key& k){return identityValid&&g_diceKeyR92.read(k)&&k.wrapper&&k.generation==generation;}
struct HudHolder{};
static bool AcquireHudDirect(HudHolder&,void*& h){h=reinterpret_cast<void*>(1);return true;}
static void ReleaseHudHolder(HudHolder&){}
static bool FindMovieClip(void*,const char*,void*& c,void*& get,void*& set){c=reinterpret_cast<void*>(2);get=set=reinterpret_cast<void*>(3);return findSucceeds;}
static bool ReadHudKey175(void*,void*,std::uint64_t gen,ui_hud_visibility175::Key& k){k={2,3,4,1,gen};return true;}
static bool CallFlashDisplayFnSEH(void*,void*,unsigned char* info){std::uint16_t flags=0x400;std::memcpy(info+0x34,&flags,2);info[0x28]=nativeVisible;return true;}
static bool SetHudMovieClipVisible(const char*,bool v,std::uint64_t gen){++sets;if(!setSucceeds||gen!=generation)return false;nativeVisible=v;return true;}
#ifndef KCD2VR_DICE_HANDOVER_R92_EXTRACT
#error Extract the unchanged production Service/lease functions.
#endif
#include KCD2VR_DICE_HANDOVER_R92_EXTRACT
}
using namespace kcd2vr;
namespace dm=kcd2vr::dice_marker_r92;
static unsigned checks=0;
static void check(bool ok,const char* why){++checks;if(!ok){std::fprintf(stderr,"dice marker handover: %s\n",why);std::abort();}}
static dm::Selection s{};
static void current(bool prepared=true){s.tick=now;dm::selected.publish(s);dm::prepared.publish(prepared?dm::Presented{s.instance,s.visit,s.lifetime,now,s.index,true}:dm::Presented{});}
static void step(bool prepared=true){now+=16;current(prepared);ServiceDiceMarkerR92();}
int main(){
 s.instance=11;s.visit=3;s.lifetime=7;s.index=1;s.valid=true;
 current(false);ServiceDiceMarkerR92();
 check(nativeVisible&&!dm::flatOwnerReady&&!DiceMarkerLeaseR92()&&sets==0,"binding alone never hides native or grants XR");
 step();check(!nativeVisible&&dm::flatOwnerReady&&DiceMarkerLeaseR92()&&sets==1,"successful native hide precedes replacement grant");
 dm::Presented h{};check(dm::hidden.read(h)&&h.index==1,"hide receipt contains selected native die identity");
 s.index=0;setSucceeds=false;step();
 check(!dm::flatOwnerReady&&!DiceMarkerLeaseR92(),"failed new-index hide cannot reuse previous die grant");
 setSucceeds=true;step();check(dm::flatOwnerReady&&DiceMarkerLeaseR92()&&!nativeVisible,"current-index hide success restores replacement");
 setSucceeds=false;step(false);check(!dm::flatOwnerReady&&!DiceMarkerLeaseR92()&&!nativeVisible,"loss of prepared frame withdraws XR even if native restore fails");
 setSucceeds=true;step(false);check(nativeVisible&&!dm::flatOwnerReady,"failed restore is retried without retaining marker grant");
 step();check(!nativeVisible&&dm::flatOwnerReady,"a later prepared frame can acquire again");
 // Exit is distinct from a one-frame missing pose. Failure must retain cleanup.
 setSucceeds=false;now+=16;dm::selected.publish({});ServiceDiceMarkerR92();
 ui_hud_visibility175::Key key{};
 check(!dm::flatOwnerReady&&!DiceMarkerLeaseR92()&&DiceKeyR92(key),"failed exit restore retains exact native key for retry");
 setSucceeds=true;now+=16;ServiceDiceMarkerR92();check(nativeVisible&&!DiceKeyR92(key),"successful exit restores native and drops key");
 // A selected die never authorizes suppression if its map/lifetime is gone.
 step();check(DiceMarkerLeaseR92(),"fresh same-session reentry succeeds");
 now+=81;check(!DiceMarkerLeaseR92(),"stale prepared/selected/hidden receipts cannot keep a native filter alive");
 step();++s.visit;now+=16;s.tick=now;dm::selected.publish(s);ServiceDiceMarkerR92();
 check(!dm::flatOwnerReady&&nativeVisible,"new native visit cannot borrow old selection receipt");
 g_diceNativeVisibleR92=false;step();step(false);check(!nativeVisible&&!dm::flatOwnerReady,"restore preserves latest native request rather than forcing visibility on");
 std::printf("R92 actual dice marker Service/lease: %u checks PASS (native Flash leaves substituted)\n",checks);
}
