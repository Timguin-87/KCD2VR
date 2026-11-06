#define KCD2VR_COMBO_MID_HOST
#define KCD2VR_COMBO_CLEANUP_PREFIX_ONLY
#include "combo_update_cleanup_h10_test.cpp"
namespace DetourModKit {struct Address {std::uintptr_t value;};}
namespace DetourModKit::hook {
enum class Gpr{Rax,Rdi,Rsi};struct MidContext {std::uintptr_t regs[3]{},stack=0,flags=0;};
static std::uintptr_t& gpr(MidContext& c,Gpr r){return c.regs[unsigned(r)];}
static std::uintptr_t stack_pointer(const MidContext& c){return c.stack;}
struct Options {bool fail_if_already_hooked=false;};struct Target {DetourModKit::Address address;};
struct MidRequest {const char* name;Target target;Options options;};
static unsigned createCalls=0,failCreateAt=0,failEnableAt=0,failDisableAt=0;
template<class F>static std::optional<Hook> mid_at(MidRequest,F){
 ++createCalls;if(createCalls==failCreateAt)return std::nullopt;
 Hook h;h.live=false;h.failEnable=createCalls==failEnableAt;h.failDisable=createCalls==failDisableAt;return h;
}
}
namespace retail_build112 {static bool Ready(std::uintptr_t){return true;}}
#include "combo_update_h10_sdk.inc"
static bool pinReadH10(const void* p,void* out,std::size_t size){
 struct Native {std::uintptr_t rva;const char* hex;};
 static constexpr Native pins[]{
  {0x93A236,"488d8f8004000033d2e8f057dfff"},{0x909B01,"4881c180040000488b01ff500884c0740e"},
  {0x908746,"488bcbe84213000084c075ee"},{0x72FA49,"3851087432488b5910885108"}};
 for(const auto& row:pins)if(reinterpret_cast<std::uintptr_t>(p)==g_base+row.rva&&size==std::strlen(row.hex)/2){
  for(std::size_t i=0;i<size;++i){unsigned x=0;std::sscanf(row.hex+2*i,"%2x",&x);static_cast<unsigned char*>(out)[i]=static_cast<unsigned char>(x);}return true;}
 return false;
}
int main(){using namespace DetourModKit::hook;
 for(unsigned scenario=0;scenario<8;++scenario){CleanupH10 f;combo59c17::g_update_hooks_ready_h10=true;
  MidContext cleanup{};gpr(cleanup,Gpr::Rdi)=reinterpret_cast<std::uintptr_t>(state);combo59c17::CleanupMidH10(cleanup);
  rawValue10(state,0x488,0);rawValue10(state,0x38,2);set(actionSlotsH8,0,nullptr);
  MidContext ctx{};ctx.stack=0x99100;ctx.flags=0x246;
  rawValue10(reinterpret_cast<void*>(ctx.stack),0x28,g_base+0x90874E);
  gpr(ctx,Gpr::Rdi)=reinterpret_cast<std::uintptr_t>(f.trigger.data());gpr(ctx,Gpr::Rsi)=reinterpret_cast<std::uintptr_t>(actor);
  gpr(ctx,Gpr::Rax)=0x1234567800;
  if(scenario==1)rawValue10(reinterpret_cast<void*>(ctx.stack),0x28,g_base+0x9111BB);
  if(scenario==2)++inputGenerationH10;
  if(scenario==3)combo59c17::g_update_hooks_ready_h10=false;
  if(scenario==4)combo59c17::g_teardown_pending19=true;
  if(scenario==5)gpr(ctx,Gpr::Rax)|=2;
  if(scenario==6)gpr(ctx,Gpr::Rdi)=0x99200;
  if(scenario==7)gpr(ctx,Gpr::Rsi)=reinterpret_cast<std::uintptr_t>(targetA);
  const auto before=ctx;combo59c17::HitReadMidH10(ctx);
  check10(gpr(ctx,Gpr::Rax)==(scenario==0?0x1234567801:before.regs[0]),"actual SDK wrapper changes only qualified UPDATE AL, never existing native true or foreign caller");
  check10(ctx.flags==before.flags&&ctx.stack==before.stack&&ctx.regs[1]==before.regs[1]&&ctx.regs[2]==before.regs[2],"actual callback preserves captured stack flags and other registers");
  check10(Rd<std::uint8_t>(state,0x488,1)==0&&timers==1,"actual wrapper never writes native480 or starts another timer");
  combo59c17::g_teardown_pending19=false;
 }
 extraSehCopyH10=&pinReadH10;
 for(unsigned failure=0;failure<5;++failure){
  check10(combo59c17::DropUpdateContinuationH10(),"previous optional pair is fully disarmed before activation test");
  createCalls=failCreateAt=failEnableAt=failDisableAt=0;
  if(failure==1)failCreateAt=1;if(failure==2)failCreateAt=2;
  if(failure==3)failEnableAt=1;if(failure==4)failEnableAt=2;
  const bool armed=combo59c17::ArmUpdateContinuationH10();
  check10(armed==(failure==0)&&combo59c17::g_update_hooks_ready_h10.load()==(failure==0),"actual Arm marks pair ready only after both creates and both enables");
  if(failure)check10(!combo59c17::g_cleanup_mid_h10&&!combo59c17::g_hit_read_mid_h10,"partial create/enable failures restore all releasable handles");
  check10(combo59c17::DropUpdateContinuationH10()&&!combo59c17::g_update_hooks_ready_h10.load(),"actual optional pair teardown always withdraws callback ownership");
 }
 createCalls=failCreateAt=0;failEnableAt=2;failDisableAt=1;
 check10(!combo59c17::ArmUpdateContinuationH10()&&!combo59c17::g_update_hooks_ready_h10,"failed rollback never publishes callback ownership");
 check10(combo59c17::g_cleanup_mid_h10&&combo59c17::g_cleanup_mid_h10->is_enabled()&&!combo59c17::g_hit_read_mid_h10,"failed disable retains exact live backend rather than replacing it");
 check10(!combo59c17::ArmUpdateContinuationH10(),"a retained live optional backend cannot be installed over");
 combo59c17::g_cleanup_mid_h10->failDisable=false;
 check10(combo59c17::DropUpdateContinuationH10()&&!combo59c17::g_cleanup_mid_h10,"later disable retry safely retires retained backend");
 combo59c17::g_ready=false;combo59c17::g_teardown_pending19=false;
 installCalls=failInstallAt=0;failNewManagerDisable=false;createCalls=0;failCreateAt=1;failEnableAt=failDisableAt=0;
 check10(combo59c17::Install()&&combo59c17::g_ready&&combo59c17::g_manager_hook&&combo59c17::g_trigger_hook&&!combo59c17::g_update_hooks_ready_h10,
         "optional midhook creation failure leaves both existing combo callbacks active");
 combo59c17::Uninstall();check10(!combo59c17::g_teardown_pending19&&!combo59c17::g_manager_hook&&!combo59c17::g_trigger_hook,
         "existing and optional callback teardown stays retryable after optional activation failure");
 extraSehCopyH10=nullptr;
 std::printf("%s combo SDK %u/%u\n",failed10?"FAIL":"PASS",checks10-failed10,checks10);return failed10?1:0;
}
