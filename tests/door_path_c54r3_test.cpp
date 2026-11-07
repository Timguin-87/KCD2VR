// Actual production F5 observer adapters; only native reads, hook patching,
// time and queued logger are fixture leaves. No native game code is executed.
#define NOMINMAX
#include <windows.h>
#include <algorithm>
#include <array>
#include <atomic>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <optional>
#include <string>
#include <vector>
#include <cstdlib>
#include "dmk_error_result_fixture.h"
#include "diagnostics/door_path_c54r3.h"
#include "diagnostics/capture168.h"
#include "diagnostics/interaction_cycle_h4.h"
#include "core/door_collector_c54.h"
#include "core/door_origin_h12.h"
#include "diagnostics/door_area_observer_h29.h"
namespace door19c4=kcd2vr::door19c4;namespace door_near_area_c54=kcd2vr::door_near_area_c54;
namespace diagnostics=kcd2vr::diagnostics;namespace door_collector_c54=kcd2vr::door_collector_c54;namespace door_hold65=kcd2vr::door_hold65;
static unsigned checks=0,reads=0,pins=0,creates=0,enables=0,disables=0;static bool badPin=false,badRetail=false,failDisable=false;static unsigned failCreate=0,failEnable=0;
static DetourModKit::ErrorCode createError3=DetourModKit::ErrorCode::BackendFailed;
static std::uint64_t now=1000;static std::uintptr_t g_wh_base=0x180000000ull;static std::atomic<bool> g_selection_ready68{true};
static void ck(bool ok,const char* text){++checks;if(!ok){std::fprintf(stderr,"FAIL %u %s\n",checks,text);std::exit(2);}}
static std::uint64_t Clock3(){return now;}
struct Region {std::uintptr_t address;std::vector<unsigned char> bytes;};static std::vector<Region> memory;
static void put(std::uintptr_t a,const void* b,std::size_t n){for(auto& m:memory)if(a>=m.address&&a+n<=m.address+m.bytes.size()){std::memcpy(m.bytes.data()+a-m.address,b,n);return;}ck(false,"write mapped fixture memory");}
template<class T>static void put(std::uintptr_t a,T b){put(a,&b,sizeof(b));}
static bool SelectionCopy68(std::uintptr_t a,void* b,std::size_t n) noexcept {++reads;for(const auto& m:memory)if(a>=m.address&&n<=m.bytes.size()&&a-m.address<=m.bytes.size()-n){std::memcpy(b,m.bytes.data()+a-m.address,n);return true;}return false;}
template<class T>static bool SelectionRead68(std::uintptr_t a,T& b) noexcept{return SelectionCopy68(a,&b,sizeof(b));}
static bool SelectionVft68(std::uintptr_t p,std::uintptr_t rva) noexcept{std::uintptr_t vf=0;return SelectionRead68(p,vf)&&vf==g_wh_base+rva;}
static bool MatchEntry(std::uintptr_t,const unsigned char*,std::size_t){return !badPin;}
namespace retail_build112 {static bool Ready(std::uintptr_t){return !badRetail;}}
static BOOL Module3(DWORD flags,LPCWSTR,HMODULE* out){ck((flags&GET_MODULE_HANDLE_EX_FLAG_PIN)!=0,"retained callback module pinned");++pins;*out=reinterpret_cast<HMODULE>(1);return TRUE;}
namespace kcd2vr::diagnostic_trace {static std::vector<std::string> lines;static void Write(const char* s) noexcept{lines.emplace_back(s);}}
namespace diagnostic_trace=kcd2vr::diagnostic_trace;
namespace DetourModKit {struct Address{std::uintptr_t value;};namespace hook {
enum class Gpr {Rax,Rbx,Rbp,Rsi,R12,R15,Rdx,R8,R9,Rcx};
struct XmmView {float value=0;template<class T>T lane(std::size_t)const{return T(value);}};
struct MidContext {std::uintptr_t regs[10]{},stack=0;XmmView xmms[16]{};};
static std::uintptr_t& gpr(MidContext& c,Gpr r){return c.regs[unsigned(r)];}static std::uintptr_t stack_pointer(MidContext& c){return c.stack;}static XmmView xmm(MidContext& c,unsigned i){return c.xmms[i];}
struct Options{bool fail_if_already_hooked=false;};struct Target{Address address;};struct MidRequest{const char* name;Target target;Options options;};
struct Hook {unsigned id=0;bool live=false;bool is_enabled()const{return live;}kcd2vr_test::DmkOperation enable(){++enables;if(id==failEnable)return DetourModKit::Error{DetourModKit::ErrorCode::EnableFailed,"fixture::enable",0x1234,id};live=true;return {};}bool disable(){++disables;if(failDisable)return false;live=false;return true;}};
static kcd2vr_test::DmkResult<Hook> mid_at(MidRequest q,void(*)(MidContext&)){++creates;ck(q.options.fail_if_already_hooked,"MID refuses foreign hook");ck(pins>0,"pin before hook publish");if(creates==failCreate)return DetourModKit::Error{createError3,"fixture::create",q.target.address.value,creates};return Hook{creates,false};}
}}
#define GetTickCount64 Clock3
#define GetModuleHandleExW Module3
#include "../src/interaction_door_diagnostics_c54r3.inl"
#undef GetModuleHandleExW
#undef GetTickCount64
static void reset(){DoorDiagDisable3();for(auto& h:g_door_diag_hooks3)h.reset();g_door_diag_failed3=g_door_diag_prepared3=false;g_door_diag_failure3="NONE";g_door_diag_failure_rva3=0;DoorDiagClearBackend3();g_door_diag_pass3={};g_door_diag_counts3={};diagnostic_trace::lines.clear();g_door_diag_entered3=g_door_diag_completed3=0;pins=creates=enables=disables=reads=0;badPin=badRetail=failDisable=false;failCreate=failEnable=0;createError3=DetourModKit::ErrorCode::BackendFailed;now=1000;diagnostics::capture168::gate.select(diagnostics::capture168::Domain::Interaction,true,now);door_collector_c54::current={0x1000,0x2000,0x3000,17};}
using namespace DetourModKit::hook;
int main(){
 reset();ck(DoorDiagEnable3()&&creates==9&&enables==9,"all9 pinned diagnostic seams enable transactionally");DoorDiagDisable3();ck(!g_door_diag_ready3&&disables==9,"capture stop disables every diagnostic patch");ck(DoorDiagEnable3()&&creates==9&&enables==18,"new session reenables same retained handles, no reinstall");
 reset();badPin=true;ck(!DoorDiagEnable3()&&creates==0&&pins==0,"byte mismatch creates no hooks");
 reset();badRetail=true;ck(!DoorDiagEnable3()&&creates==0,"retail fingerprint mismatch refuses");
 for(unsigned fail=1;fail<=9;++fail){reset();failCreate=fail;ck(!DoorDiagEnable3()&&!g_door_diag_ready3&&g_door_diag_failed3,"partial create never publishes ready");for(const auto& h:g_door_diag_hooks3)ck(!h||!h->live,"partial create remains inactive");}
 for(unsigned fail=1;fail<=9;++fail){reset();failEnable=fail;ck(!DoorDiagEnable3()&&!g_door_diag_ready3,"partial enable disables all");for(const auto& h:g_door_diag_hooks3)ck(!h||!h->live,"partial enable rollback retains inactive handles");}
 reset();failCreate=3;createError3=DetourModKit::ErrorCode::MidHookCapacityExhausted;
 ck(!DoorDiagEnable3()&&g_door_diag_failure_rva3==0x640131,"third-site failure retains exact target");
 ck(std::strcmp(g_door_diag_backend_name3,"MidHookCapacityExhausted")==0&&g_door_diag_backend_detail3==g_wh_base+0x640131&&g_door_diag_backend_extra3==3,"actual Result error fields survive create rollback");
 DoorDiagReportReady3(false,0);ck(diagnostic_trace::lines.back().find("backend=MidHookCapacityExhausted")!=std::string::npos&&diagnostic_trace::lines.back().find("backendWhere=fixture::create")!=std::string::npos,"existing F5 report names backend cause instead of inferring overlap");
 const auto retainedCreates=creates;ck(!DoorDiagEnable3()&&creates==retainedCreates&&std::strcmp(g_door_diag_backend_name3,"MidHookCapacityExhausted")==0,"retired retry preserves first backend reason");
 reset();failEnable=3;ck(!DoorDiagEnable3()&&std::strcmp(g_door_diag_backend_name3,"EnableFailed")==0&&g_door_diag_backend_detail3==0x1234&&g_door_diag_backend_extra3==3,"actual enable error retained separately from create");
 reset();badPin=true;ck(!DoorDiagEnable3()&&std::strcmp(g_door_diag_backend_name3,"NONE")==0,"byte mismatch does not invent a backend error");
 reset();ck(DoorDiagEnable3(),"clean activation after fixture reset");DoorDiagReportReady3(true,0x1ff);ck(diagnostic_trace::lines.back().find("ready=1")!=std::string::npos&&diagnostic_trace::lines.back().find("backend=NONE")!=std::string::npos,"successful capture reports no stale backend error");
 reset();ck(DoorDiagEnable3(),"ready fixture");MidContext ctx{};ctx.regs[unsigned(Gpr::R15)]=0x1000;ctx.regs[unsigned(Gpr::Rbp)]=0x6000;ctx.regs[unsigned(Gpr::Rbx)]=0x8000;ctx.stack=0x5000;
 memory={{0x5000,std::vector<unsigned char>(0x2000)},{0x8000,std::vector<unsigned char>(0x100)},{0x9000,std::vector<unsigned char>(0x100)},{0xa000,std::vector<unsigned char>(0x100)},{0xb000,std::vector<unsigned char>(0x100)},{0xc000,std::vector<unsigned char>(0x100)}};
 put(0x8000,std::int32_t(5));put(0x8008,std::uintptr_t(0x9000));put(0x9000,g_wh_base+0x4072C78);put(0x900c,std::uint32_t(42));put(0x9020,std::uintptr_t(0xa000));put(0xa000,g_wh_base+0x3B282A0);put(0xa010,std::uintptr_t(0xb000));put(0xb000,"AnimDoor",9);
 DoorDiagBegin3(0x1000,1,g_wh_base+0x7F5727,0);DoorDiagMid3<dd3::Enumerated>(ctx);ck(g_door_diag_pass3.count==1&&g_door_diag_pass3.visited==1,"actual enumerated AnimDoor bound to invocation and ordinal");
 const float box[]{-.5f,-.5f,-.5f,.5f,.5f,.5f},segment[]{0,0,0,0,1,0};put(0x6018,box,sizeof(box));put(0x5fb8,segment,sizeof(segment));DoorDiagBounds3(0x9000,0x6018);
 ctx.regs[unsigned(Gpr::Rax)]=1;put(0x5040,.125f);DoorDiagMid3<dd3::Inside>(ctx);ck(g_door_diag_pass3.rows[0].pointInside==1&&g_door_diag_pass3.rows[0].distanceKnown&&g_door_diag_pass3.rows[0].centreDistanceSq==.125f,"inside branch records original written distance only");
 DoorDiagPredicate3(dd3::Facing,0,1);ck(g_door_diag_pass3.rows[0].facing==1,"existing facing adapter distinction recorded without changing return");
 struct RankRecord{std::int32_t word;float screenSq,worldSq,pad;std::uint32_t target;};RankRecord rank{5,0,.1f,0,42};put(0xc000,&rank,sizeof(rank));put(0x5fa0,std::uintptr_t(0xc000));put(0x5fa8,std::uintptr_t(0xc038));DoorDiagMid3<dd3::Sorted>(ctx);
 const auto before=memory;DoorDiagEnd3(0x1000,0,g_door_diag_activation3.load());ck(diagnostic_trace::lines.size()>=4,"full observed order, candidate and frequency emitted");bool row=false,order=false;for(const auto& line:diagnostic_trace::lines){ck(line.size()<2047,"complete bounded log line");row|=line.find("target=42 ordinal=1")!=std::string::npos;order|=line.find("rank=0 total=1 target=42")!=std::string::npos;}ck(row&&order,"actual target+ordinal and sorted target emitted");for(unsigned i=0;i<memory.size();++i)ck(memory[i].bytes==before[i].bytes,"observer never mutates native fixture bytes");
 DoorDiagBegin3(0x1000,1,g_wh_base+0x7F5727,0);door_collector_c54::current.serial=18;const auto priorReads=reads;DoorDiagMid3<dd3::Enumerated>(ctx);ck(reads==priorReads,"reentrant or changed collector cannot borrow ancestor scope");door_collector_c54::current.serial=17;
 diagnostics::capture168::gate.select(diagnostics::capture168::Domain::Interaction,false,now);const auto quietReads=reads;const auto quietLines=diagnostic_trace::lines.size();DoorDiagMid3<dd3::Enumerated>(ctx);DoorDiagBegin3(0x1000,1,g_wh_base+0x7F5727,0);DoorDiagEnd3(0x1000,0,g_door_diag_activation3.load());ck(reads==quietReads&&diagnostic_trace::lines.size()==quietLines,"capture off does no added native reads or formatting");
 reset();ck(DoorDiagEnable3(),"activation fixture ready");const auto oldActivation=DoorDiagBegin3(0x1000,1,g_wh_base+0x7F5727,0);DoorDiagDisable3();ck(DoorDiagEnable3(),"next capture ready");const auto done=g_door_diag_completed3.load(),boundary=g_door_diag_boundary3.load();DoorDiagEnd3(0x1000,0,oldActivation);ck(g_door_diag_completed3==done&&g_door_diag_boundary3==boundary+1,"old selector completion cannot join new activation");
 DoorDiagBegin3(0x1000,1,g_wh_base+0x7F5727,0);for(unsigned i=0;i<dd3::Capacity+3;++i)DoorDiagMid3<dd3::Enumerated>(ctx);ck(g_door_diag_pass3.count==dd3::Capacity&&g_door_diag_pass3.dropped==3&&g_door_diag_pass3.visited==dd3::Capacity+3,"bounded door rows count every omitted candidate");
 put(0x5fa8,std::uintptr_t(0xc039));const auto logStart=diagnostic_trace::lines.size();DoorDiagMid3<dd3::Sorted>(ctx);ck(diagnostic_trace::lines.size()==logStart+1&&diagnostic_trace::lines.back().find("NATIVE_VECTOR_LAYOUT_OR_READ")!=std::string::npos,"invalid native vector refuses without walking arbitrary memory");
 door_collector_c54::current={};const auto scopeStart=diagnostic_trace::lines.size();DoorDiagBegin3(0x1000,1,g_wh_base+0x7F5727,0,"CURRENT_NATIVE_PLAYER_POLICY_GRAPH_REFUSED");ck(diagnostic_trace::lines.size()==scopeStart+1&&diagnostic_trace::lines.back().find("noCandidateAbsenceClaim=1")!=std::string::npos,"scope refusal names exact already observed proof gap");
 dd3::Counter counter;ck(counter.begin(1,1000,true,false,true),"first pass sampled");for(unsigned i=0;i<3;++i)ck(counter.begin(1,1000,true,false,true),"firstfour preserved");ck(!counter.begin(1,1100,true,false,true)&&counter.calls==5&&counter.suppressed==1,"coalesced invocation counted");ck(counter.begin(1,1250,true,false,true),"250ms sample resumes");for(unsigned i=0;i<dd3::LineLimit;++i)(void)counter.line();ck(!counter.line()&&counter.summary(1250),"detail budget cannot suppress reserved health summary");
 ck(dd3::before({1,0,2},{2,1,1}),"direct epsilon group first");ck(dd3::before({5,0,2},{3,0,1}),"zero screen group rank word descending");ck(dd3::before({3,4,5},{0,2,1}),"nonzero screen weighted by positive rank+1");ck(dd3::before({1,4,1},{0,2,2}),"equal weighted screen tie breaks by world distance");
 std::printf("door_path_c54r3=%u/%u production_adapters=ACTUAL native_leaf_calls=FIXTURE_ONLY writes=NONE\n",checks,checks);
}
