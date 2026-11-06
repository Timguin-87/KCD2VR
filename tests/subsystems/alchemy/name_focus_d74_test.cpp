#define main bridge67_original_main
#include "bridge67_test.cpp"
#undef main
#include "../../../src/subsystems/alchemy/native_selection68.h"
#include "../../../src/interaction_alignment.h"
namespace craft68=b;
namespace selection68=kcd2vr::subsystems::alchemy::native_selection68;
using kcd2vr::interaction_alignment::AlchemyHandNameFocusD72;
static bool wanted=true,activity=true,playerClass=true,slotCurrent=true;
static int oldActionType=2,oldActionReads=0;
static b::NativeTable nativeTable{};
static bool AlchemyHandNameWantedD72(){return wanted;}
static bool NomineeActivityCurrent68(const b::NativeTable& t){return activity&&selection68::sameTable(t,nativeTable);}
static bool SelectionVft68(std::uintptr_t ptr,std::uintptr_t vft){return playerClass&&ptr==nativeTable.playerEntity&&vft==0x4072C78;}
template<class T> static bool SelectionRead68(std::uintptr_t at,T& out){
 if(at==nativeTable.instance+0x304){++oldActionReads;out=T(oldActionType);return true;}
 if(at==nativeTable.playerEntity+0xC){out=T(900);return true;}
 for(unsigned s=0;s<nativeTable.refs.size();++s)if(at==nativeTable.instance+0x150+4*s){out=T(slotCurrent?nativeTable.refs[s].entity:999);return true;}
 return false;
}
#include "actual_name_focus_d74.inc"
int main(){
 reset();kcd2vr::minigame_hands45::alchemyMode67.store(kcd2vr::minigame_hands45::SixDofVrik);
 nativeTable=table(1002);b::finish(nativeTable);offer(nativeTable,8,.03,1002);b::finish(nativeTable);
 n::State owner{};auto in=input(1002);b::NativeFrame nf{};b::completed.read(nf);auto c=copy(nf);auto d=owner.step(c,in);
 b::nomination.publish({context(),d,1002});AlchemyHandNameFocusD72 focus{};
 check(::ReadAlchemyHandNameFocusD72(1002,focus)&&focus.entity==108&&focus.type==8&&focus.side==1);
 check(oldActionReads==0); // old mortar action2 must not hide actual herb8 hover
 slotCurrent=false;check(!::ReadAlchemyHandNameFocusD72(1002,focus)&&focus.entity==0);slotCurrent=true;
 playerClass=false;check(!::ReadAlchemyHandNameFocusD72(1002,focus));playerClass=true;
 activity=false;check(!::ReadAlchemyHandNameFocusD72(1002,focus));activity=true;
 wanted=false;check(!::ReadAlchemyHandNameFocusD72(1002,focus));wanted=true;
 check(!::ReadAlchemyHandNameFocusD72(1083,focus));
 // No hand nominee yields the existing clear/restore-native-head-name path.
 d.source=n::Source::Gaze;b::nomination.publish({context(),d,1002});check(!::ReadAlchemyHandNameFocusD72(1002,focus));
 // Exact qualified destination names still work while holding logical herbs.
 nativeTable.handTypesKnown68=true;nativeTable.handType68[0]=7;b::finish(nativeTable);
 offer(nativeTable,6,0,1002);b::finish(nativeTable);
 d={};d.source=n::Source::Right;d.ref=nativeTable.refs[6];d.table=nativeTable.refs;
 d.palm={{0,0,.10},1002,4,5,6,0,0,true};d.proximityCraft1=d.handsOnlyD53=d.pourRecipientD72=true;
 d.heldSourceH21=nativeTable.refs[7];kcd2vr::frik_pose_probe::observedFrame67.hand[0].grip=0;
 b::nomination.publish({context(),d,1002});
 check(::ReadAlchemyHandNameFocusD72(1002,focus)&&focus.entity==106&&focus.type==6);
 nativeTable.handType68[0]=8;b::finish(nativeTable);check(!::ReadAlchemyHandNameFocusD72(1002,focus)&&focus.entity==0);
 check(oldActionReads==0);
 std::printf("D74 actual name callback: %u checks PASS; read-only native leaves mocked, no action grants\n",checks);return 0;
}
