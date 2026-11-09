// Executes the production CVar lease. Native branch fixture is pinned against
// retail LTup27BCE04, Update22630A0 and expiry8BAE94; it never runs game code.
#include <cmath>
#include <cstdio>
#include <cstdint>
#include <cstring>
#include <algorithm>
static bool present=true,setOkay=true,notified=false;static float cvar=.2f;static unsigned writes=0;
static void* SafeGetCryConsole(){return present?&cvar:nullptr;}
static void* SafeConsoleGetCVar(void*,const char* name){return std::strcmp(name,"wh_cs_BlockCooldown")==0?&cvar:nullptr;}
static float SafeCVarGetFloat(void* p,float fallback){return p?*static_cast<float*>(p):fallback;}
static bool SafeCVarSetFloat(void* p,float v){++writes;if(!setOkay)return false;*static_cast<float*>(p)=v;return true;}
static bool CheatCVarSetFloat143(void* p,float v){return SafeCVarSetFloat(p,v);} // R143: the dev-mode fallback is host-tested in tests/cheat_cvar_write_r143_test.cpp
namespace frik_pose_probe {static void SetBlockCooldownLease59c17(bool active){notified=active;}}
namespace diagnostic_trace {static void Write(const char*){}}
#include "subsystems/masterstrike_combos/block_cooldown_lease_h24.inl"
static int n=0,bad=0;static void ck(bool x,const char* s){++n;bad+=!x;std::printf("%s %s\n",x?"PASS":"FAIL",s);}
struct NativeReleaseBranch {
 std::int64_t timer=-100000;bool pending=false;unsigned char inputContext=1,foreignContext=0;
 void release(float delay,std::int64_t now){
  // 27BCE1A..3F: strict >0 arms; <=0 leaves the old timer unchanged.
  if(delay>0.f)timer=now+std::int64_t(delay*100000.f); // 9D6D5F..C6
  pending=true; // 27BCE46
 }
 void update(std::int64_t now){
  // 8BAE94: invalid negative is FALSE, not 'already expired'.
  if(pending&&timer>=0&&now>=timer){inputContext=0;pending=false;} // 22630BD/C2 original leaf
 }
 unsigned char maximum()const{return std::max(inputContext,foreignContext);}
};
int main(){char msg[320]{};BlockCooldownLease59c17 lease;
 ck(lease.Apply(true,msg,sizeof(msg))&&cvar==BlockCooldownLease59c17::requestedH24&&notified,"actual production lease writes and acknowledges minimal positive native delay");
 const unsigned written=writes;ck(!lease.Apply(true,msg,sizeof(msg))&&writes==written,"unchanged owning mode does not repeatedly overwrite CVar");
 NativeReleaseBranch old;old.release(0.f,1000000);for(int i=0;i<1000;++i)old.update(1000000+i*1000);
 ck(old.pending&&old.inputContext==1&&old.timer==-100000,"retail zero-delay branch with invalid timer reproduces permanent missing release ACK");
 NativeReleaseBranch fixed;fixed.release(cvar,1000000);fixed.update(1000000);
 ck(fixed.pending&&fixed.inputContext==1&&fixed.timer>1000000,"positive lease initializes original native timer without prematurely clearing input");
 fixed.update(1000100);ck(!fixed.pending&&fixed.inputContext==0,"one native update after original timer expiry clears release request and own context");
 NativeReleaseBranch foreign;foreign.foreignContext=2;foreign.release(cvar,1000000);foreign.update(1001000);
 ck(foreign.inputContext==0&&foreign.maximum()==2,"own release ACK does not clear foreign block contexts or award master readiness");
 ck(lease.Apply(false,msg,sizeof(msg))&&cvar==.2f&&!notified,"actual production lease restores original value when still owned");
 cvar=.07f;lease.Apply(true,msg,sizeof(msg));cvar=.13f;const unsigned beforeExternal=writes;lease.Apply(false,msg,sizeof(msg));
 ck(cvar==.13f&&writes==beforeExternal&&!notified,"external/shared rule value changed while leased is preserved on release");
 present=false;BlockCooldownLease59c17 missing;const unsigned beforeMissing=writes;missing.Apply(true,msg,sizeof(msg));
 ck(!missing.had&&!notified&&writes==beforeMissing,"missing native CVar cannot claim functioning lease");missing.Apply(false,msg,sizeof(msg));present=true;
 setOkay=false;BlockCooldownLease59c17 refused;cvar=.2f;refused.Apply(true,msg,sizeof(msg));
 ck(!refused.had&&!notified&&cvar==.2f,"failed native CVar write does not advertise active cooldown repair");refused.Apply(false,msg,sizeof(msg));setOkay=true;
 cvar=0.f;BlockCooldownLease59c17 oldSaved;oldSaved.Apply(true,msg,sizeof(msg));
 ck(cvar>0.f&&oldSaved.original==0.f,"saved zero value is safely leased positive only while selected mode owns it");oldSaved.Apply(false,msg,sizeof(msg));
 ck(cvar==0.f&&!notified,"mode exit restores a user's original zero without permanent setting mutation");
 std::printf("%d checks %d failures\n",n,bad);return bad?1:0;
}
