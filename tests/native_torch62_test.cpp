// Executes the production request, identity/filter adapter and native-frame
// consumer. Only exact native function leaves and established external owner
// services are doubled; never executes game code in this host test.
#include <windows.h>
#include <array>
#include <atomic>
#include <vector>
#include <string>
#include <cassert>
#include <cstdio>
#include "../src/core/snapshot_mailbox.h"
#include "../src/vrik/native_torch62.h"
#include "../src/diagnostics/owned_character_entry63.h"
#include "../src/diagnostics/capture168.h"
static std::vector<unsigned char> image(0x549D330),player(0xC00),entity(0x80),attachment(0x80),input(0x30),cry(0x538),game(0xA48),maps(0x20),actor(0x20),state(0x20);
static unsigned char character[16];
static bool verified=true,identity=true,resolve=true,calibrated=false,head=true,hand=true;
static int leftItem=0,physicalLeft=0,filterMode=0,calls=0,filterCalls=0;static std::string lastLine;
static std::uintptr_t WhBase(){return reinterpret_cast<std::uintptr_t>(image.data());}
template<class T> static void put(void* p,const T& v){std::memcpy(p,&v,sizeof(v));}
template<class T> static bool Read(const void* p,T& value){
 if(!p)return false;auto a=reinterpret_cast<std::uintptr_t>(p);
 const auto in=[&](const void* b,std::size_t n){const auto x=reinterpret_cast<std::uintptr_t>(b);return a>=x&&a-x<=n&&sizeof(T)<=n-(a-x);};
 bool ok=in(image.data(),image.size())||in(player.data(),player.size())||in(entity.data(),entity.size())||in(attachment.data(),attachment.size())||in(input.data(),input.size())||in(cry.data(),cry.size())||in(maps.data(),maps.size())||in(game.data(),game.size());
 if(!ok)return false;std::memcpy(&value,p,sizeof(value));return true;
}
static bool PhysVft6698(void* p,std::uintptr_t rva){std::uintptr_t v=0;return Read(p,v)&&v==WhBase()+rva;}
constexpr std::uintptr_t kCEntityVftRva=0x4072C78;
static std::atomic<std::uint64_t> g_pose_epoch6689{7},g_weapon_request_epoch6694{4},g_tracking_tick6689{10000};
static std::atomic<std::uintptr_t> g_player_char37{reinterpret_cast<std::uintptr_t>(character)};
static std::atomic<void*> g_last_player_entity{entity.data()},g_henry_item_attach_manager{attachment.data()};
static std::atomic<bool> g_ctx_menu{false},g_ctx_cutscene{false},g_ctx_dialogue{false},g_ctx_scripted{false},g_gameplay_allowed{true},g_left_valid{true};
namespace animation_filter {std::atomic<bool> nativeBodyOwned112{false};}
namespace retail_build112 {static bool Ready(std::uintptr_t b){return b==WhBase()&&verified;}}
namespace diagnostic_trace {static void Write(const char* s){lastLine=s;}}
struct NativeOwner104{void* player=nullptr;void* entity=nullptr;void* actor=nullptr;void* state=nullptr;std::uint64_t epoch=0;};
static bool NativeResolve104(void* p,NativeOwner104& o){if(!resolve||p!=player.data()||!PhysVft6698(p,kcd2vr::torch62::kPlayerVft))return false;o={p,entity.data(),actor.data(),state.data(),g_pose_epoch6689.load()};return true;}
static bool PlayerIdentityAccepts37(const void* p,std::uint64_t){return identity&&p==character;}
static bool CalibrationReady(){return calibrated;}
static bool TrackingHeadUsable111(){return head;}
static bool TrackingHandUsable111(int,std::uint64_t){return hand;}
static int UtilityLeftItem54(){return leftItem;}
static int UtilityLeftItemR114(std::uint64_t){return leftItem;}
static int ReadPhysicalHandHeld55(int,std::uint64_t){return physicalLeft;}
static kcd2vr::character_entry63::OwnedEntry ownedCharacter63;
static bool copyCharacter63(std::uintptr_t at,void* out,std::size_t n) noexcept {
    const auto b=WhBase();if(at<b||at-b>image.size()||n>image.size()-(at-b))return false;
    std::memcpy(out,reinterpret_cast<const void*>(at),n);return true;
}
namespace kcd2vr::combat_diag_probe {
bool NativeCharacterEntryQualified63(std::uintptr_t base) noexcept {return verified&&ownedCharacter63.qualifies(base,copyCharacter63);}
}
static void InvalidateReadCache101R2(){}
#include "../src/vrik/native_torch62.inl"
struct Leaves: TorchNativeLeaves62 {
 static bool character(std::uintptr_t b,void* e,void*& out){assert(b==WhBase()&&e==entity.data());out=filterMode==9?nullptr:filterMode==10?::character+1: ::character;return out!=nullptr;}
 static bool filter(std::uintptr_t,void* m,bool& deny){++filterCalls;assert(m==maps.data());deny=filterMode==1;
   if(filterMode==3)put(input.data()+0x10,static_cast<void*>(nullptr));
   if(filterMode==4)g_ctx_menu=true;if(filterMode==5)hostTickCount64+=121;
   if(filterMode==6)g_gameplay_allowed=false;if(filterMode==7)g_left_valid=false;
   return filterMode!=2;}
 static bool invoke(std::uintptr_t b,void* p){assert(b==WhBase()&&p==input.data());++calls;return filterMode!=8;}
};
static unsigned checks=0;
static void check(bool b,const char* n){++checks;if(!b){std::printf("FAIL %s line=%s\n",n,lastLine.c_str());std::abort();}}
static void reset(){
 using namespace kcd2vr::torch62;ownedCharacter63.clear();hostTickCount64+=500;g_tracking_tick6689=hostTickCount64;
 verified=identity=resolve=head=hand=true;calibrated=false;leftItem=physicalLeft=filterMode=0;
 g_pose_epoch6689=7;g_weapon_request_epoch6694=4;g_player_char37=reinterpret_cast<std::uintptr_t>(character);
 g_ctx_menu=g_ctx_cutscene=g_ctx_dialogue=g_ctx_scripted=animation_filter::nativeBodyOwned112=false;
 g_gameplay_allowed=g_left_valid=true;g_torch_fault62=false;
 put(player.data(),WhBase()+kPlayerVft);put(entity.data(),WhBase()+kCEntityVftRva);put(entity.data()+0xC,std::uint32_t(123));
 put(attachment.data(),WhBase()+std::uintptr_t(0x3EC9958));put(attachment.data()+0x38,static_cast<void*>(player.data()));
 put(player.data()+0xB88,static_cast<void*>(input.data()));put(input.data(),WhBase()+kInputVft);put(input.data()+0x10,static_cast<void*>(player.data()));
 put(image.data()+kCryGlobal,static_cast<void*>(cry.data()));put(cry.data(),WhBase()+kCryVft);put(cry.data()+0x88,static_cast<void*>(game.data()));put(game.data(),WhBase()+kActionGameVft);put(game.data()+0xA40,static_cast<void*>(player.data()));put(cry.data()+0x530,static_cast<void*>(maps.data()));put(maps.data(),WhBase()+kMapVft);
 put(image.data()+kMapVft+0x108,WhBase()+kFilter);put(image.data()+kKeyboardClosureVft+0x10,WhBase()+kClosureThunk);put(image.data()+kXiClosureVft+0x10,WhBase()+kClosureThunk);
 put(image.data()+kCEntityVftRva+0x328,WhBase()+kGetCharacter);std::memcpy(image.data()+kGetCharacter,kGetCharacterEntry,sizeof(kGetCharacterEntry));
 std::memcpy(image.data()+kHandler,kHandlerEntry,sizeof(kHandlerEntry));std::memcpy(image.data()+kClosureThunk,kThunkEntry,sizeof(kThunkEntry));std::memcpy(image.data()+kFilter,kFilterEntry,sizeof(kFilterEntry));
}
static void tick(){TorchNativeTick62<Leaves>(hostTickCount64);}
static void offer(bool shoulder=false){check(RequestTorch62(shoulder,hostTickCount64),"request accepted with current immutable body");}
int main(){
 reset();g_henry_item_attach_manager=nullptr;offer();int old=calls;tick();check(calls==old+1,"uncalibrated X invokes full native callback once");tick();check(calls==old+1,"same request cannot duplicate");
 check(lastLine.find("UNOBSERVED")!=std::string::npos,"return does not fabricate inventory/equip success");
 // Reproduce C62: startup has already patched GetCharacter with our observer.
 reset();image[kcd2vr::torch62::kGetCharacter]=0xE9;offer();old=calls;tick();
 check(calls==old&&lastLine.find("GET_CHARACTER_ENTRY")!=std::string::npos,"unowned GetCharacter jump remains refused with exact subreason");
 check(ownedCharacter63.publish(WhBase(),0x123456,true,copyCharacter63),"capture exact post-enable passive-hook receipt");
 offer();old=calls;tick();check(calls==old+1,"our exact installed GetCharacter observer no longer blocks native torch");
 image[kcd2vr::torch62::kGetCharacter+9]^=1;offer();old=calls;tick();
 check(calls==old&&lastLine.find("GET_CHARACTER_ENTRY")!=std::string::npos,"foreign modification of owned receipt refuses");
 image[kcd2vr::torch62::kGetCharacter+9]^=1;ownedCharacter63.clear();offer();old=calls;tick();
 check(calls==old,"retired observer receipt cannot authorize patched entry");
 reset();check(!ownedCharacter63.publish(WhBase(),0x123456,true,copyCharacter63),"pristine code cannot be published as a hook receipt");
 reset();image[kcd2vr::torch62::kGetCharacter]=0xE9;
 check(!ownedCharacter63.publish(WhBase(),0x123456,false,copyCharacter63),"disabled hook cannot publish receipt");
 check(!ownedCharacter63.publish(WhBase(),0,true,copyCharacter63),"absent native trampoline cannot publish receipt");
 reset();offer();filterMode=1;old=calls;tick();check(calls==old&&lastLine.find("NO_TORCH_FILTER")!=std::string::npos,"native no_torch filter blocks");filterMode=0;tick();check(calls==old,"filter refusal is not queued retry");
 for(int f=2;f<=7;++f){reset();offer();filterMode=f;old=calls;tick();check(calls==old,"unknown filter or changed owner/context/input during native query refuses");}
 reset();offer();hostTickCount64+=121;g_tracking_tick6689=hostTickCount64;old=calls;tick();check(calls==old,"expired original request refuses even current input");
 reset();offer();hostTickCount64+=81;old=calls;tick();check(calls==old,"stale current input refuses before native filter");
 for(int n=0;n<15;++n){reset();offer();old=calls;switch(n){
 case 0:g_pose_epoch6689=8;break;case 1:g_weapon_request_epoch6694=5;break;case 2:g_player_char37=0;break;
 case 3:g_ctx_menu=true;break;case 4:g_ctx_dialogue=true;break;case 5:g_ctx_cutscene=true;break;case 6:g_ctx_scripted=true;break;
 case 7:animation_filter::nativeBodyOwned112=true;break;case 8:g_gameplay_allowed=false;break;case 9:g_left_valid=false;break;
 case 10:identity=false;break;case 11:resolve=false;break;case 12:verified=false;break;case 13:put(input.data()+0x10,static_cast<void*>(nullptr));break;case 14:put(player.data()+0xB88,static_cast<void*>(nullptr));break;}
 tick();check(calls==old,"changed ownership/real gameplay context cannot dispatch");reset();tick();check(calls==old,"refused input never revives");}
 for(int n=0;n<11;++n){reset();offer();old=calls;using namespace kcd2vr::torch62;
 if(n==0)image[kHandler]^=1;if(n==1)image[kClosureThunk]^=1;if(n==2)image[kFilter]^=1;
 if(n==3)put(image.data()+kXiClosureVft+0x10,std::uintptr_t(0));if(n==4)put(image.data()+kMapVft+0x108,std::uintptr_t(0));
 if(n==5)put(maps.data(),std::uintptr_t(0));if(n==6)put(input.data(),std::uintptr_t(0));
 if(n==7)put(image.data()+kCEntityVftRva+0x328,std::uintptr_t(0));if(n==8)image[kGetCharacter]^=1;
 if(n==9)filterMode=9;if(n==10)filterMode=10;
 tick();check(calls==old,"foreign callback/vft/filter entry fails closed");}
 for(int n=0;n<6;++n){reset();calibrated=true;offer(true);old=calls;if(n==0)calibrated=false;if(n==1)head=false;if(n==2)hand=false;if(n==3)leftItem=3;if(n==4)physicalLeft=1;
 tick();check(calls==old+(n==5?1:0),"only spatial gesture requires calibration/head/hand/free slot");}
 reset();calibrated=true;leftItem=11;physicalLeft=1;offer(true);old=calls;tick();check(calls==old+1,"existing held native torch allows shoulder stow without Grip");
 reset();leftItem=3;offer();old=calls;tick();check(calls==old+1,"explicit button delegates other equipment admission to native callback");
 reset();offer();filterMode=8;old=calls;tick();check(calls==old+1&&g_torch_fault62.load(),"native exception latches without fallback XI");check(!RequestTorch62(false,hostTickCount64),"native fault cannot retrigger from new request");
 check(lastLine.find("PUBLISH_REFUSED")!=std::string::npos&&lastLine.find("NATIVE_FAULT_LATCH")!=std::string::npos,"faulted publication has exact bounded refusal receipt");
 for(int n=0;n<4;++n){reset();old=calls;const char* why=n==0?"NO_SOURCE_TICK":n==1?"NO_BODY_EPOCH":n==2?"NO_REQUEST_GENERATION":"NO_CURRENT_CHARACTER";
   if(n==1)g_pose_epoch6689=0;if(n==2)g_weapon_request_epoch6694=0;if(n==3)g_player_char37=0;
   check(!RequestTorch62(false,n==0?0:hostTickCount64)&&calls==old&&lastLine.find(why)!=std::string::npos&&lastLine.find("PUBLISH_REFUSED")!=std::string::npos,"missing source owner publication reports exact reason without native call");}
 using namespace kcd2vr::diagnostics::capture168;check(accept("C62 TORCH_INPUT X_PRESS route=3",1)&&accept("C62 TORCH_NATIVE reason=NO_TORCH_FILTER",1),"rare torch decisions survive F3 off");
 std::printf("PASS native torch actual consumer %u checks\n",checks);
}
