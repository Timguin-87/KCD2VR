// Actual native announcement lease; native owner/slot reads are explicit leaves.
#include <atomic>
#include <cstdint>
#include <cstdio>
#include <functional>
struct NativeOwner104{void* player=nullptr;void* entity=nullptr;void* actor=nullptr;void* state=nullptr;void* soul=nullptr;std::uint64_t epoch=0;};
namespace ph {struct Command {bool kick59=false,controlledHoldH26=false,cutFeint62=false,feintFE=false;std::uint64_t gesture59c3=0,key=0,generation=0,epoch=0,bodyEpoch=0;};}
namespace kcd2vr::combo_zone_assist_cz{inline std::atomic<bool> enabled{true};}
static std::atomic<std::uint64_t> g_weapon_request_epoch6694{4},g_pose_epoch6689{5};
static std::uint64_t g_input_source_generation_h14=6;
struct Prep{NativeOwner104 owner{};std::uint64_t source=0,gesture=0,key=0,generation=0,request=0;};static Prep g_prepared_input_r18{};
struct Physical{bool attackPrepared=false;std::uint64_t prepareGestureC10R18=0;};static Physical g_phys_native6698{};
struct NativeParryOpponent176{void* actor=nullptr;void* state=nullptr;void* entity=nullptr;std::uint32_t id=0;};
static NativeParryOpponent176 enemy{};static bool enemyOkay=true;
static bool NativeParryOpponentRead176(const NativeOwner104&,NativeParryOpponent176& out){out=enemy;return enemyOkay;}
static NativeOwner104 live{};static bool readable=true;static unsigned char slot=1;static int zone=2;static std::function<void()> during;
static bool NativeResolve104(void* p,NativeOwner104& out){out=live;if(during){auto f=std::move(during);during={};f();}return readable&&p==live.player;}
template<class T>static bool Read(void* p,T& out){const auto off=reinterpret_cast<std::uintptr_t>(p)-reinterpret_cast<std::uintptr_t>(live.state);
 if(!readable)return false;if(off==0x868)out=T(slot);else if(off==0xF8)out=T(zone);else return false;return true;}
#include "subsystems/masterstrike_combos/combo_zone_owner_r94.inl"
static unsigned checks=0,failures=0;static ph::Command c{};
static void ck(bool x,const char* why){++checks;if(!x){++failures;std::printf("FAIL %s\n",why);}}
static void setup(){live={reinterpret_cast<void*>(0x100),reinterpret_cast<void*>(0x150),reinterpret_cast<void*>(0x200),reinterpret_cast<void*>(0x10000),reinterpret_cast<void*>(0x300),5};
 enemy={reinterpret_cast<void*>(0x400),reinterpret_cast<void*>(0x500),reinterpret_cast<void*>(0x600),42};enemyOkay=true;
 c={false,false,false,false,10,20,30,4,5};g_weapon_request_epoch6694=4;g_pose_epoch6689=5;g_input_source_generation_h14=6;
 g_prepared_input_r18={live,6,10,20,30,4};g_phys_native6698={true,10};readable=true;slot=1;zone=2;during={};
 kcd2vr::combo_zone_assist_cz::enabled=true;g_combo_zone_owner_r94={};}
static void remember(){NativeRememberComboZoneR94(live,c,2,enemy);}
static void no(const char* why){ck(!NativePreserveComboZoneR94(live,c),why);ck(!g_combo_zone_owner_r94.gesture,"refusal retires lease");}
int main(){
 setup();ck(!NativePreserveComboZoneR94(live,c),"no announced lease never preserves");remember();ck(NativePreserveComboZoneR94(live,c),"exact successful prepare retains recipe zone");
 setup();zone=1;remember();ck(!g_combo_zone_owner_r94.gesture,"unobserved zone never publishes");
 setup();g_prepared_input_r18={};remember();ck(!g_combo_zone_owner_r94.gesture,"unobserved prepare never publishes");
 setup();g_prepared_input_r18.source=5;remember();ck(!g_combo_zone_owner_r94.gesture,"stale source never publishes");
 setup();remember();c.key++;no("weapon replacement");
 setup();remember();c.generation++;no("generation replacement");
 setup();remember();c.gesture59c3++;no("next gesture");
 setup();remember();g_input_source_generation_h14++;no("new input source");
 setup();remember();g_weapon_request_epoch6694++;no("request revoked");
 setup();remember();g_pose_epoch6689++;no("body epoch revoked");
 setup();remember();live.actor=reinterpret_cast<void*>(0x300);no("native actor replaced");
 setup();remember();live.state=reinterpret_cast<void*>(0x20000);no("native state replaced");
 setup();remember();live.epoch++;no("native epoch replaced");
 setup();remember();g_prepared_input_r18.gesture++;no("native prepare replaced");
 setup();remember();slot=0;no("native slot closed");
 setup();remember();zone=1;no("native changed announced zone");
 setup();remember();kcd2vr::combo_zone_assist_cz::enabled=false;no("option OFF immediate");
 setup();remember();c.controlledHoldH26=true;no("controlled heavy retains original path");
 setup();remember();c.kick59=true;no("kick retains original path");
 setup();remember();g_phys_native6698.attackPrepared=false;no("prepare ended");
 setup();remember();g_phys_native6698.prepareGestureC10R18++;no("physical prepare replaced");
 setup();remember();readable=false;no("native read failure");
 setup();during=[](){g_input_source_generation_h14++;};remember();ck(!g_combo_zone_owner_r94.gesture,"source invalidated during native observation not published");
 setup();remember();during=[](){kcd2vr::combo_zone_assist_cz::enabled=false;};no("option disabled during observation");
 setup();remember();during=[](){g_weapon_request_epoch6694++;};no("request invalidated during observation");
 setup();remember();c.cutFeint62=true;no("deliberate cut feint retains native handler");
 setup();remember();c.feintFE=true;no("committed feint retains native handler");
 setup();remember();enemy.id++;no("opponent ID replacement");
 setup();remember();enemy.actor=reinterpret_cast<void*>(0x900);no("opponent actor replacement");
 setup();remember();enemy.state=reinterpret_cast<void*>(0x900);no("opponent state replacement");
 setup();remember();enemy.entity=reinterpret_cast<void*>(0x900);no("same ID different opponent entity");
 setup();remember();enemyOkay=false;no("unproved native opponent resumes ordinary path");
 setup();remember();live.entity=reinterpret_cast<void*>(0x900);no("Henry entity replacement");
 setup();remember();live.soul=reinterpret_cast<void*>(0x900);no("Henry soul replacement");
 setup();remember();during=[](){enemy.id++;};no("opponent replaced during owner revalidation");
 std::printf("combo_zone_owner_r94 %u/%u\n",checks-failures,checks);return failures?1:0;
}
