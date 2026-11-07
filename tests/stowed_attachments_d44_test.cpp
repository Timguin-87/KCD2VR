// R123: R121 moved the two INI-only hide globals to kcd2vr::stowed_hide_hw (live GEAR page 2 switches).
#define g_hide_back_hw kcd2vr::stowed_hide_hw::hideBackSetting
#define g_hide_at_stations_hw kcd2vr::stowed_hide_hw::hideAtStationsSetting
// Actual character resolver + production station attachment tick. Native leaves
// are simulated, while the full routing/ownership/policy code is included.
#include "subsystems/ranged_reload/stowed_hide_hw.h"
#include "subsystems/ranged_reload/draw_guard_r120.h"
#include "core/player_identity37.h"
#include <atomic>
#include <algorithm>
#include <array>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <string>
#include <vector>
#include <map>
#include <mutex>
#include <fstream>
namespace sh=kcd2vr::stowed_hide_hw;
static std::uint64_t now=1000;static std::uint64_t GetTickCount64(){return now;}
static std::vector<std::string> traces;
namespace diagnostic_trace{static void Write(const char*s){traces.emplace_back(s);}}
static bool hostReference=false,hostMenu=false;static int hostPoseMode=4;
namespace diagnostics::lab{static bool NativeReference(){return hostReference;}}
namespace kcd2vr::diagnostics::capture168{enum class Domain{Bug,Interaction};static bool captureD45=true;static bool active(Domain,std::uint64_t){return captureD45;}}
static bool nativeReady=true;
namespace retail_build112{static bool Ready(std::uintptr_t){return nativeReady;}}
static std::map<std::uintptr_t,unsigned char> memory;
template<class T>static void put(std::uintptr_t at,const T&v){const auto*p=reinterpret_cast<const unsigned char*>(&v);for(std::size_t i=0;i<sizeof(v);++i)memory[at+i]=p[i];}
template<class T>static bool Read(const void*at,T&v){auto p=reinterpret_cast<std::uintptr_t>(at);auto*d=reinterpret_cast<unsigned char*>(&v);for(std::size_t i=0;i<sizeof(v);++i){auto x=memory.find(p+i);if(x==memory.end())return false;d[i]=x->second;}return true;}
static constexpr std::uintptr_t character=0x100000,entity=0x200000,player=0x300000,itemManager=0x400000;
static unsigned getCharacterCalls=0;static void* NativeGetCharacter(void*,int){++getCharacterCalls;return reinterpret_cast<void*>(character);}
static std::uintptr_t WhBase(){return reinterpret_cast<std::uintptr_t>(&NativeGetCharacter)-0x6411AC;}
static constexpr std::uintptr_t kCEntityVftRva=0x4072C78;
static bool PhysVft6698(void*p,std::uintptr_t r){std::uintptr_t v=0;return p&&Read(p,v)&&v==WhBase()+r;}
static std::atomic<void*>g_henry_item_attach_manager{nullptr},g_last_player_entity{nullptr};
static std::atomic<void*>g_player_module_d76{nullptr};
static std::atomic<std::uintptr_t>g_player_char37{0};static std::atomic<std::uint64_t>g_player_publication37{2},g_player_lease37{0};
static std::atomic<std::uint64_t>g_player_lifetime_d44{1},g_pose_epoch6689{1};
namespace pid37=kcd2vr::player_identity37;
static pid37::Ledger g_player_ledger37;
static std::atomic_flag g_player_ledger_lock37=ATOMIC_FLAG_INIT;
static std::atomic<void*>g_last_player_pose{nullptr},g_henry_char_attachment_manager{nullptr};
static std::atomic<std::uint64_t>g_henry_skinning_tick{0};static std::atomic<unsigned>g_henry_skinning_thread{0};
namespace animation_filter{static void ObservePlayerSkinning(void*,void*,void*){}}
#include "stowed_identity_actual.inc"
static bool PlayerIdentityAccepts37(const void*c,std::uint64_t t){return !(g_player_publication37.load()&1)&&reinterpret_cast<std::uintptr_t>(c)==g_player_char37.load()&&g_player_lease37.load()&&t>=g_player_lease37.load()&&t-g_player_lease37.load()<=250;}
#define __fastcall
#define __try try
#define __except(x) catch(...)
#include "stowed_character_actual.inc"
static std::atomic<int>g_cal_state{2};static std::atomic<bool>g_ctx_cutscene{false};
static std::atomic<int>g_hide_stowed_bow59c14{0};static std::atomic<float>g_hide_bow_pitch_deg59c14{20},g_hmd_pitch_deg59c14{0};
static std::atomic<int>g_bow_att_count59c15{1};static char g_bow_att_names59c15[4][32]{{"weapon_bow"}};
static std::atomic<void*>g_hidden_bow_entity59c14{nullptr};
static bool StowedBowRelease59c14(std::uint64_t){return true;}
static bool StowedBowImmersive54(){return hostPoseMode>=3&&g_cal_state.load()==2&&!g_ctx_cutscene.load()&&!hostMenu&&!hostReference;}
namespace kcd2vr::activity_input163{
 enum class Kind175{Alchemy,Smithing};struct Witness{std::atomic<bool>ready{true};std::array<std::atomic<std::uintptr_t>,2>active{};};static Witness witness175;
}
struct Attachment{unsigned flags=0;};static std::array<Attachment,sh::kCount> attachments;
static bool pins=true;static bool BowAttachPins59c15(){return pins;}
static void* CharAttachment59c15(void*c,const char*n){if(reinterpret_cast<std::uintptr_t>(c)!=character)return nullptr;for(std::size_t i=0;i<sh::kCount;++i)if(!std::strcmp(n,sh::kSlots[i].name))return &attachments[i];return nullptr;}
static bool AttachmentHidden59c15(void*a,bool&v){if(!a)return false;v=(static_cast<Attachment*>(a)->flags&0x10000)!=0;return true;}
static bool AttachmentFlags59c15(void*a,unsigned&v){if(!a)return false;v=static_cast<Attachment*>(a)->flags;return true;}
static unsigned hideCalls=0,showCalls=0;
static bool AttachmentHide59c15(void*a,bool h){if(!a)return false;auto&p=*static_cast<Attachment*>(a);if(h){p.flags|=0x70000;++hideCalls;}else{p.flags&=~0x70000;++showCalls;}return true;}
static std::map<std::uint32_t,void*> childEntitiesD56;
static std::map<void*,bool> childHiddenD56;
static unsigned entityHideCallsD56=0,entityShowCallsD56=0;
static void* EntityById6694(std::uint32_t id){auto p=childEntitiesD56.find(id);return p==childEntitiesD56.end()?nullptr:p->second;}
static bool PhysEntityIdentity135(void* p,std::uint32_t id){return p&&EntityById6694(id)==p;}
static bool EntityHiddenFlag59c14(void* p,bool& hidden){auto x=childHiddenD56.find(p);if(x==childHiddenD56.end())return false;hidden=x->second;return true;}
static bool EntityHide59c14(void* p,bool hidden,bool* invoked=nullptr){if(invoked)*invoked=false;if(!childHiddenD56.count(p))return false;
 if(invoked)*invoked=true;childHiddenD56[p]=hidden;if(hidden)++entityHideCallsD56;else ++entityShowCallsD56;return true;}
#include "subsystems/ranged_reload/stowed_bow_attachment.inl"
static unsigned checks=0,failures=0;
static void check(bool v,const char*s){++checks;if(!v){++failures;std::printf("FAIL %s\n",s);}}
static void fixture(){
 ResetPlayerIdentity37();
 memory.clear();auto b=WhBase();put(character,b+0x40602F8);put(character+8,b+0x4060548);put(character+8+0x70,character);
 put(itemManager,b+0x3EC9958);put(itemManager+0x38,player);put(player,b+0x3AA4B98);put(player+0x38,entity);put(player+0x9D0,itemManager);
 put(entity,b+kCEntityVftRva);put(b+kCEntityVftRva+0x328,b+0x6411AC);
 g_henry_item_attach_manager=reinterpret_cast<void*>(itemManager);g_last_player_entity=reinterpret_cast<void*>(entity);
 g_player_char37=character;g_player_publication37=2;g_player_lease37=now;
 (void)ObservePlayerIdentity37(pid37::Source::Prime,true,reinterpret_cast<void*>(entity),reinterpret_cast<void*>(character),nullptr,now);
 for(auto&a:attachments)a.flags=0;g_hide_back_hw=false;g_hide_at_stations_hw=true;
 kcd2vr::activity_input163::witness175.active[0]=1;kcd2vr::activity_input163::witness175.active[1]=0;
}
static void tick(unsigned dt=300){now+=dt;g_player_lease37=now;StowedBowAttachmentTick59c15(now);}
int main(int argc,char**argv){
 fixture();g_henry_item_attach_manager=nullptr;
 check(HenryCharacter59c15()==reinterpret_cast<void*>(character),"fresh canonical Henry character does not depend on inventory manager availability");
 check(getCharacterCalls==0,"fresh character owner avoids redundant native GetCharacter call");
 g_player_publication37=3;check(!HenryCharacter59c15(),"in-flight player identity publication cannot authorize an attachment");g_player_publication37=2;
 put(character+8+0x70,character+32);check(!HenryCharacter59c15(),"canonical character embedded manager backlink must match");put(character+8+0x70,character);
 fixture();tick();check(hideCalls==sh::kCount,"station tick hides all known visible stowed attachments");
 tick(10);bool allHidden=true;for(auto&a:attachments)allHidden&=(a.flags&0x10000)!=0;
 check(allHidden,"station suppression retains all owned stowed hides");
 tick();
 // A read/identity miss is unknown, not a proof that the character was replaced.
 g_henry_item_attach_manager=nullptr;g_player_char37=0;tick();
 g_henry_item_attach_manager=reinterpret_cast<void*>(itemManager);g_player_char37=character;
 kcd2vr::activity_input163::witness175.active[0]=0;tick();
 bool allVisible=true;for(auto&a:attachments)allVisible&=(a.flags&0x70000)==0;
 check(allVisible,"temporary unknown character does not strand the owner's hidden attachments on station exit");
 attachments[3].flags=0x10000;tick();check(attachments[3].flags==0x10000,"game-hidden bow is never unhidden by station owner");
 g_hide_back_hw=true;tick();check((attachments[4].flags&0x70000)==0x70000,"normal gameplay still suppresses back equipment");
 tick(10);check((attachments[4].flags&0x70000)==0x70000,"ordinary play has no provisional global draw reveal");
 const auto showsBeforeMenu=showCalls;hostMenu=true;tick();
 check(showCalls==showsBeforeMenu&&(attachments[4].flags&0x70000)==0x70000,"current back holders remain hidden while the menu releases pose ownership");
 hostPoseMode=0;hostMenu=false;tick();
 check(showCalls==showsBeforeMenu&&(attachments[4].flags&0x70000)==0x70000,"native pose selection does not release back-hide preference");
 check(attachments[0].flags==0&&attachments[1].flags==0&&attachments[2].flags==0,"front weapon holders stay visible outside stations");
 check(!StowedInputsD63().immersive&&StowedInputsD63().backVisibility,"production input separates visibility from pose admission");
 g_ctx_cutscene=true;tick();check(attachments[4].flags==0,"cutscene restores our back-holder visibility");
 g_ctx_cutscene=false;tick();check((attachments[4].flags&0x70000)==0x70000,"cutscene exit reapplies explicit back preference");
 hostReference=true;tick();check(attachments[4].flags==0,"native reference restores our back-holder visibility");
 hostReference=false;g_cal_state=1;tick();check(attachments[4].flags==0,"uncalibrated view leaves native visibility");
 g_cal_state=2;hostPoseMode=4;tick();
 g_hide_back_hw=false;attachments[3].flags=0x20000;kcd2vr::activity_input163::witness175.active[0]=1;tick();
 check(attachments[3].flags==0x20000,"station owner never adopts game-owned partial visibility bits");
 attachments[4].flags=0x40000;tick();check(attachments[4].flags==0x40000,"changed partial native visibility retires the old full-hide receipt");
 kcd2vr::activity_input163::witness175.active[0]=0;tick();
 check(attachments[3].flags==0x20000,"station exit never clears game-owned shadow or recursion hide bits");
 check(attachments[4].flags==0x40000,"station exit cannot clear a native visibility change after our hide");
 for(auto&a:attachments)a.flags=0;tick(); // native clear isolates the next lifetime scenario
 // Ordinary publication/F4 changes are not a new C37 owner lifetime.
 kcd2vr::activity_input163::witness175.active[0]=1;tick();
 (void)ObservePlayerIdentity37(pid37::Source::Prime,true,reinterpret_cast<void*>(entity),reinterpret_cast<void*>(character),nullptr,now);++g_pose_epoch6689;
 kcd2vr::activity_input163::witness175.active[0]=0;tick();check(attachments[0].flags==0,"C37 refresh and F4 pose epoch preserve the actual hide receipt");
 // A true clear/requalification can reuse the exact character/attachment address.
 kcd2vr::activity_input163::witness175.active[0]=1;tick();
 (void)ObservePlayerIdentity37(pid37::Source::Prime,true,nullptr,nullptr,nullptr,now);attachments[1].flags=0;const auto calls=hideCalls+showCalls;tick();
 check(hideCalls+showCalls==calls,"proved player absence prevents stale legacy-manager visibility calls");
 (void)ObservePlayerIdentity37(pid37::Source::Prime,true,reinterpret_cast<void*>(entity),reinterpret_cast<void*>(character),nullptr,now);attachments[0].flags=0x10000;
 kcd2vr::activity_input163::witness175.active[0]=0;tick();check(attachments[0].flags==0x10000,"same-address character requalification cannot borrow old hide ownership");
 kcd2vr::diagnostics::capture168::captureD45=false;fixture();traces.clear();tick();
 check(traces.empty(),"capture off suppresses source/state/hide formatting paths while visibility still updates");
 check((attachments[0].flags&0x70000)==0x70000,"capture off never disables station visibility correction");
 // D56: holder visibility and an entity-backed attachment are separate
 // consumers. Production tick + post-callback route must cover both.
 fixture();childEntitiesD56.clear();childHiddenD56.clear();
 const auto attD56=reinterpret_cast<std::uintptr_t>(&attachments[0]);
 const std::uintptr_t objectD56=0x700000,childD56=0x800000,nameD56=0x900000;
 put(attD56,WhBase()+0x3A657C0);put(attD56+0x28,character+8);put(attD56+0x20,objectD56);
 put(attD56+0x10,nameD56);for(unsigned j=0;j<=std::strlen(sh::kSlots[0].name);++j)put(nameD56+j,sh::kSlots[0].name[j]);
 put(objectD56,WhBase()+0x3A68F68);put(objectD56+8,std::uint32_t(555));
 g_henry_char_attachment_manager=reinterpret_cast<void*>(character+8);
 childEntitiesD56[555]=reinterpret_cast<void*>(childD56);childHiddenD56[reinterpret_cast<void*>(childD56)]=false;
 tick();check(childHiddenD56[reinterpret_cast<void*>(childD56)],"station hides current entity-backed stowed binding as well as holder");
 childHiddenD56[reinterpret_cast<void*>(childD56)]=false;
 StationWeaponAfterAttachmentD56(reinterpret_cast<void*>(objectD56),&attachments[0]);
 check(childHiddenD56[reinterpret_cast<void*>(childD56)],"native callback reshow is suppressed in the same frame");
 put(attD56+0x20,std::uintptr_t(0));tick();
 check(!childHiddenD56[reinterpret_cast<void*>(childD56)],"native detach restores only our entity hide before a drawn tool can use it");
 put(attD56+0x20,objectD56);childHiddenD56[reinterpret_cast<void*>(childD56)]=true;tick();
 kcd2vr::activity_input163::witness175.active[0]=0;tick();
 check(childHiddenD56[reinterpret_cast<void*>(childD56)],"native-prehidden entity is not adopted or revealed on station exit");
 childHiddenD56[reinterpret_cast<void*>(childD56)]=false;kcd2vr::activity_input163::witness175.active[0]=1;tick();
 check(childHiddenD56[reinterpret_cast<void*>(childD56)],"new visible stowed entity earns its own hide receipt");
 kcd2vr::activity_input163::witness175.active[0]=0;tick();
 check(!childHiddenD56[reinterpret_cast<void*>(childD56)],"station exit restores current owned entity");
 const auto priorD56=entityHideCallsD56;const char* handNameD56="hand_left";
 for(unsigned j=0;j<=std::strlen(handNameD56);++j)put(nameD56+j,handNameD56[j]);
 kcd2vr::activity_input163::witness175.active[0]=1;
 StationWeaponAfterAttachmentD56(reinterpret_cast<void*>(objectD56),&attachments[0]);
 check(entityHideCallsD56==priorD56&&!childHiddenD56[reinterpret_cast<void*>(childD56)],"native hand/tool sockets cannot borrow station weapon suppression");
 if(argc>1){std::ofstream f(argv[1]);for(const auto&s:traces)f<<s<<'\n';}
 std::printf("D44 stowed attachment production %u/%u; native_calls=SIMULATED\n",checks-failures,checks);return failures?1:0;
}
