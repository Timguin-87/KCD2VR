// Actual production observer and actual pre-submit caller. Native GetCharacter
// and readable memory are explicit doubles. No physics/part getter is doubled:
// the observation must never call one or dereference the saved collider.
#include "vrik/part_namespace62.h"
#include "diagnostics/capture168.h"
#include "diagnostics/owned_character_entry63.h"
#include <atomic>
#include <cassert>
#include <cstdio>
#include <cstring>
#include <string>
#include <unordered_map>
#include <vector>
namespace n=kcd2vr::part_namespace62;
namespace cap=kcd2vr::diagnostics::capture168;
constexpr std::uintptr_t base62=0x10000000,entity62=0x1000,proxy62=0x3000,character62=0x4000,model62=0x6000,table62=0x7000,oldCollider62=0xDEAD0000;
static std::unordered_map<std::uintptr_t,unsigned char> memory62;
static std::uint64_t now62=1000;
static unsigned reads62=0,oldReads62=0,characters62=0,original62=0,started62=0;
static int mutate62=0;static bool build62=true,admit62=true;
static std::vector<std::string> lines62;
static std::atomic<std::uint64_t> g_pose_epoch6689{2},g_weapon_request_epoch6694{3};
static kcd2vr::character_entry63::OwnedEntry ownedCharacter63;
template<class T> static void put62(std::uintptr_t p,const T& v){const auto* b=reinterpret_cast<const unsigned char*>(&v);for(unsigned i=0;i<sizeof(v);++i)memory62[p+i]=b[i];}
template<class T> static bool Read(void* p,T& v)noexcept{
 const auto a=reinterpret_cast<std::uintptr_t>(p);++reads62;
 if(a>=oldCollider62&&a<oldCollider62+0x1000){++oldReads62;return false;}
 auto* b=reinterpret_cast<unsigned char*>(&v);
 for(unsigned i=0;i<sizeof(v);++i){const auto it=memory62.find(a+i);if(it==memory62.end())return false;b[i]=it->second;}return true;
}
static bool CopyOwned63(std::uintptr_t at,void* out,std::size_t size) noexcept {
 auto* bytes=static_cast<unsigned char*>(out);
 for(std::size_t i=0;i<size;++i)if(!Read(reinterpret_cast<void*>(at+i),bytes[i]))return false;
 return true;
}
namespace kcd2vr::combat_diag_probe {
bool NativeCharacterEntryQualified63(std::uintptr_t base) noexcept {
 return build62&&base==base62&&ownedCharacter63.qualifies(base,CopyOwned63);
}
}
static std::uint64_t GetTickCount64(){return now62;}
static std::uintptr_t WhBase(){return base62;}
static bool SupportedWhBuild(std::uintptr_t b){return build62&&b==base62;}
static void* CharacterForEntity6689(void* entity){
 assert(reinterpret_cast<std::uintptr_t>(entity)==entity62);++characters62;
 if(characters62==1&&mutate62==1)put62(proxy62+0x38,std::uintptr_t(0xBAD000));
 if(characters62==1&&mutate62==2)put62(character62+0xA70,std::uintptr_t(model62+8));
 if(characters62==2&&mutate62==3)++g_pose_epoch6689;
 return reinterpret_cast<void*>(character62);
}
namespace kcd2vr::diagnostic_trace {static void Write(const char* s){lines62.emplace_back(s);}}
namespace kcd2vr::frik_pose_probe {
#include "../src/vrik/part_namespace62.inl"
}
namespace frik_pose_probe=kcd2vr::frik_pose_probe;
struct Reservation62{bool tryEnter(){return admit62;}void nativeCallStarted(){++started62;}};
static bool CallSubmit(int,int,int*,bool*,int,bool,int,int,bool& ok){++original62;ok=true;return true;}
static n::Ticket ticket62(){return {12,42,now62,100,200,2,3,456,0x8888,entity62,oldCollider62,379};}
static void fixture62(){
 ownedCharacter63.clear();memory62.clear();lines62.clear();reads62=oldReads62=characters62=original62=started62=0;mutate62=0;build62=admit62=true;
 now62+=1000;g_pose_epoch6689=2;g_weapon_request_epoch6694=3;cap::gate.select(cap::Domain::Combat,true,now62);
 put62(base62+0x6411AC,n::characterEntry);put62(base62+0x1AA6BC0,n::poseGetter);put62(base62+0x1A77FD0,n::physicsGetter);put62(base62+0x1AAF020,n::countGetter);
 put62(base62+0x6411AC+15,std::uint8_t(0x4C));
 put62(entity62,base62+0x4072C78);put62(entity62+0xC,std::uint32_t(456));put62(base62+0x4072C78+0x328,base62+0x6411AC);
 put62(entity62+0xA8,std::uintptr_t(0x2000));put62(0x2008,std::uintptr_t(0x2100));put62(0x2119,std::uint8_t(0));put62(0x2120,std::uint32_t(1));put62(0x2128,proxy62);
 put62(proxy62,base62+0x3AC3478);put62(proxy62+0x30,entity62);put62(proxy62+0x38,oldCollider62);
 put62(character62,base62+0x40602F8);put62(base62+0x40602F8+0x38,base62+0x1AA6BC0);
 put62(character62+0x698,base62+0x3A71028);put62(base62+0x3A71028+0x30,base62+0x1A77FD0);
 put62(character62+0x8A8,character62);put62(character62+0x7A0,std::uintptr_t(0xBEEF0000));
 put62(character62+0x8E0,base62+0x3A71510);put62(character62+0x940,std::uint32_t(500));put62(character62+0xA70,model62);
 put62(model62,base62+0x406B930);put62(base62+0x406B930+8,base62+0x1AAF020);put62(model62+8,table62);put62(table62-4,std::uint32_t(500));
}
static void actualCaller62(){
 const auto t=ticket62();struct Pending{std::uint64_t serial,sample,tick,key,generation,epoch,request;std::uint32_t entityId;struct{std::uintptr_t collider;}metadata59c7g;int part;};
 const Pending p{t.serial,t.sample,t.tick,t.key,t.generation,t.epoch,t.request,t.target,{t.collider},t.rawPart};
 const auto target=reinterpret_cast<void*>(t.actor),te=reinterpret_cast<void*>(t.entity);
 Reservation62 serialReservationC2,claimReservationC2,hybridReservation54;bool vrDirect596=true,post=false,ok=false;
 int rpg=0,hitInfo=0,strength=1,request=0,type=0,meleeDetails=0;bool apply=true;
#include "pre_submit_namespace62.inc"
 assert(survived==admit62&&ok==admit62);
}
int main(){unsigned checks=0;const auto expect=[&](bool b){++checks;assert(b);};
 fixture62();actualCaller62();expect(original62==1&&started62==2&&characters62==2&&lines62.size()==1);expect(oldReads62==0);
 expect(lines62.back().find("colliderEqualsRoot=1 colliderEqualsMain=0 rootEqualsMain=0")!=std::string::npos);
 expect(lines62.back().find("rawPart=379 nativePart=NOT_PRODUCED")!=std::string::npos);expect(lines62.back().find("authority=NONE writes=NONE")!=std::string::npos);
 fixture62();cap::gate.select(cap::Domain::Combat,false,now62);actualCaller62();expect(original62==1&&reads62==0&&characters62==0&&lines62.empty());
 fixture62();admit62=false;actualCaller62();expect(original62==0&&started62==0&&reads62==0&&lines62.empty());
 fixture62();build62=false;actualCaller62();expect(original62==1&&characters62==0&&lines62.back().find("RETAIL_CONTRACT")!=std::string::npos);
 fixture62();put62(base62+0x6411AC,std::uint8_t(0));actualCaller62();expect(original62==1&&characters62==0&&lines62.back().find("RETAIL_CONTRACT")!=std::string::npos);
 // C63 actual observer: our exact installed passive hook is legitimate for
 // this read-only witness, while foreign/retired/mutated hooks stay refused.
 fixture62();put62(base62+0x6411AC,std::uint8_t(0xE9));
 expect(ownedCharacter63.publish(base62,0xBEEF,true,CopyOwned63));actualCaller62();
 expect(original62==1&&characters62==2&&oldReads62==0&&lines62.back().find("result=CURRENT_IDENTITIES_OBSERVED")!=std::string::npos);
 expect(lines62.back().find("namespace=UNPROVEN")!=std::string::npos&&lines62.back().find("authority=NONE writes=NONE")!=std::string::npos);
 fixture62();put62(base62+0x6411AC,std::uint8_t(0xE9));expect(ownedCharacter63.publish(base62,0xBEEF,true,CopyOwned63));
 put62(base62+0x6411AC+9,std::uint8_t(0xAA));actualCaller62();
 expect(original62==1&&characters62==0&&lines62.back().find("RETAIL_CONTRACT")!=std::string::npos);
 fixture62();put62(base62+0x6411AC,std::uint8_t(0xE9));expect(ownedCharacter63.publish(base62,0xBEEF,true,CopyOwned63));
 ownedCharacter63.clear();actualCaller62();expect(original62==1&&characters62==0&&lines62.back().find("RETAIL_CONTRACT")!=std::string::npos);
 for(auto pin:{0x1AA6BC0u,0x1A77FD0u,0x1AAF020u}){
  fixture62();put62(base62+0x6411AC,std::uint8_t(0xE9));expect(ownedCharacter63.publish(base62,0xBEEF,true,CopyOwned63));
  put62(base62+pin,std::uint8_t(0));actualCaller62();
  expect(original62==1&&characters62==0&&lines62.back().find("RETAIL_CONTRACT")!=std::string::npos);
 }
 fixture62();auto read63=[](std::uintptr_t at,auto& value){return Read(reinterpret_cast<void*>(at),value);};
 put62(base62+0x6411AC,std::uint8_t(0xE9));expect(!n::pins(base62,read63)); // pure-header default is fail closed
 fixture62();put62(entity62+0xC,std::uint32_t(999));actualCaller62();expect(original62==1&&characters62==0&&lines62.back().find("CURRENT_ENTITY_OR_ROOT")!=std::string::npos);
 fixture62();put62(proxy62+0x30,std::uintptr_t(1));actualCaller62();expect(original62==1&&characters62==0&&lines62.back().find("CURRENT_ENTITY_OR_ROOT")!=std::string::npos);
 fixture62();put62(character62+0x8A8,std::uintptr_t(1));actualCaller62();expect(original62==1&&lines62.back().find("CURRENT_CHARACTER_OR_SKELETON")!=std::string::npos);
 fixture62();put62(table62-4,std::uint32_t(501));actualCaller62();expect(original62==1&&lines62.back().find("CURRENT_CHARACTER_OR_SKELETON")!=std::string::npos);
 fixture62();mutate62=1;actualCaller62();expect(original62==1&&lines62.back().find("CURRENT_IDENTITY_CHANGED")!=std::string::npos&&oldReads62==0);
 fixture62();mutate62=2;actualCaller62();expect(original62==1&&lines62.back().find("CURRENT_CHARACTER_OR_SKELETON")!=std::string::npos);
 fixture62();mutate62=3;actualCaller62();expect(original62==1&&lines62.back().find("SOURCE_CONTEXT_CHANGED")!=std::string::npos);
 fixture62();put62(character62+0x7A0,oldCollider62);actualCaller62();expect(oldReads62==0&&lines62.back().find("colliderEqualsMain=1 rootEqualsMain=1")!=std::string::npos);
 fixture62();auto t=ticket62();t.rawPart=999999;frik_pose_probe::ObserveHitPartNamespace62(t);expect(oldReads62==0&&lines62.back().find("namespace=UNPROVEN")!=std::string::npos);
 fixture62();t=ticket62();t.epoch=1;frik_pose_probe::ObserveHitPartNamespace62(t);expect(reads62==0&&characters62==0&&lines62.empty());
 fixture62();t=ticket62();t.collider=0;for(unsigned i=0;i<100;++i){++t.serial;++t.sample;frik_pose_probe::ObserveHitPartNamespace62(t);now62+=250;}
 expect(reads62==0&&characters62==0&&lines62.empty());t.collider=oldCollider62;frik_pose_probe::ObserveHitPartNamespace62(t);expect(lines62.size()==1&&characters62==2);
 fixture62();t=ticket62();for(unsigned i=0;i<100;++i){t.serial=100+i;t.sample=100+i;frik_pose_probe::ObserveHitPartNamespace62(t);now62+=250;}expect(lines62.size()==96&&characters62==192&&oldReads62==0);
 fixture62();t=ticket62();frik_pose_probe::ObserveHitPartNamespace62(t);++t.serial;++t.sample;now62+=249;frik_pose_probe::ObserveHitPartNamespace62(t);expect(lines62.size()==1);++now62;frik_pose_probe::ObserveHitPartNamespace62(t);expect(lines62.size()==2);now62+=250;frik_pose_probe::ObserveHitPartNamespace62(t);expect(lines62.size()==2);
 fixture62();t=ticket62();now62+=cap::Duration;frik_pose_probe::ObserveHitPartNamespace62(t);expect(reads62==0&&characters62==0&&lines62.empty());
 expect(!cap::essential("C62 IMPALE_NAMESPACE result=CURRENT_IDENTITIES_OBSERVED"));expect(cap::domain("C62 IMPALE_NAMESPACE") == cap::Domain::Combat);
 std::printf("C62 actual pre-submit namespace observer: %u checks PASS\n",checks);
}
