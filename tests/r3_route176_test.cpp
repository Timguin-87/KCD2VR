#include <atomic>
#include <cstdint>
#include <cstdio>
#include <string>
#include <vector>
#include "vrik/combat_rules178r10.h"
#include "vrik/kick_style26.h" // C33: the extracted block reads kick26::nativeSpecial33
using namespace kcd2vr; // the verbatim mod.cpp block names combat_rules178r10 relative to kcd2vr
static std::uint64_t now176=1000;
std::uint64_t GetTickCount64() noexcept {return now176;}
static bool f4176=false,accepted176=true;
static unsigned native176=0,procedural176=0,legKick37=0,checks176=0,failures176=0;
static std::vector<std::string> records176;
namespace frik_pose_probe {
bool CombatExperiment175Enabled() noexcept {return f4176;}
bool RequestProceduralKick176() noexcept {++procedural176;return accepted176;}
bool RequestVrLegKick34() noexcept {++legKick37;return accepted176;} // C37: leg + one kick transaction (ledger tested in kick_tx37)
bool RequestNativeSpecial168() noexcept {++native176;return accepted176;}
void NoteKickRequest26(bool) noexcept {} // C26 receipt only
bool RequestKick59() noexcept {return false;} // R59: armed physical-combat kick is exercised by physical_contract/kick fixtures, not here
}
namespace diagnostic_trace {void Write(const char* s) noexcept {records176.emplace_back(s);}}
static void dispatch176(bool requested){
    struct {bool special_attack_requested;} touch_events{requested};
#include "generated_r3_route176.inc"
}
static void check176(bool ok,const char* m){++checks176;if(!ok){++failures176;std::printf("FAIL %s\n",m);}}
int main(){
    using namespace kcd2vr::combat_rules178r10;
    // Actual production dispatcher, all legacy transports/styles: ONE producer.
    for(int transport:{0,1,2,3})for(int style:{0,1,2})for(bool f4:{false,true})for(bool accepted:{false,true}){
        kickTransport.store(transport);kick26::style.store(style);specialUntil=0;
        f4176=f4;accepted176=accepted;native176=procedural176=legKick37=0;records176.clear();
        dispatch176(false);
        check176(native176==0&&legKick37==0&&records176.empty(),"no click has no request");
        dispatch176(true);
        const bool probe=f4&&style==2;
        check176(legKick37==unsigned(probe)&&native176==unsigned(!probe)&&procedural176==0,"U16 one producer respects selected GAME/VR-game-rules versus contact probe");
        check176(specialUntil==0,"F4 never injects an LT hold");
        check176(records176.size()==1,"one outcome receipt per click");
        if(probe)check176(records176.back().find(accepted?"applications<=1":"state=REFUSED")!=std::string::npos,"probe has no second native special producer");
        else check176(records176.back().find("NATIVE_SPECIAL")!=std::string::npos,"native selection remains explicitly unclaimed");
    }
    std::printf("r3_route176=%u/%u production_dispatch=VERBATIM native_request_leaves=REPLACED\n",checks176-failures176,checks176);
    return failures176?1:0;
}
