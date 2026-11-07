#include "diagnostics/master_eligibility_h21.h"
#include <atomic>
#include <array>
#include <optional>
#include <vector>
#include <string>
#include <cstring>
#include <cstdio>
#include <cassert>
#include <functional>
#include <new>
static std::uintptr_t returnAt=0;static void*HostReturnAddress(){return reinterpret_cast<void*>(returnAt);}
#define _ReturnAddress HostReturnAddress
namespace DetourModKit::hook {struct Hook{bool live=true,fail=false;unsigned disables=0;bool is_enabled()const noexcept{return live;}bool disable(){++disables;if(fail)return false;live=false;return true;}};enum class Gpr{Rdi,Rax,Rbx};struct MidContext{std::uintptr_t di=0,ax=0,bx=0;};static std::uintptr_t gpr(MidContext& c,Gpr r){return r==Gpr::Rdi?c.di:r==Gpr::Rax?c.ax:c.bx;}}
static bool recording=true,resolve=true;static std::uint64_t now=1000;
namespace diagnostics::capture168 {enum class Domain{Combat};static bool active(Domain,std::uint64_t){return recording;}struct Gate {std::atomic<std::uint64_t>session[1]{{1}};};static Gate gate;}
static std::uintptr_t WhBase(){return 0x180000000ull;}static std::uint64_t GetTickCount64(){return now;}
struct ReadScope101R2{};
static unsigned char trigger[0x50]{},npcTrigger[0x50]{},actor[0x300]{},npcActor[0x300]{},state[0x800]{},otherState[0x800]{},opponentState[0x200]{},player[0x80]{};
struct NativeOwner104 {void*actor=nullptr;void*state=nullptr;};
static bool NativeResolve104(void*p,NativeOwner104&o){o={actor,state};return resolve&&p==player;}
template<class T>static bool Read(void*p,T&o){if(!p)return false;std::memcpy(&o,p,sizeof(o));return true;}
template<class T>static void put(unsigned char*p,T v){std::memcpy(p,&v,sizeof(v));}
namespace diagnostic_trace {static std::vector<std::string>lines;static void Write(const char*p){lines.emplace_back(p);}}
#include "master_eligibility_h21_actual.inc"
static unsigned checks=0,originals=0,contexts=0;static int failAt=-1;static bool nested=false,changeState=false;
static void ck(bool b){++checks;if(!b){std::printf("FAIL%u\n",checks);std::abort();}}
static bool ContextLeaf(void*){++contexts;return failAt!=6;}
static bool Original(void*t){++originals;
 if(t==npcTrigger)return false;
 if(nested){const auto save=returnAt;returnAt=WhBase()+0x909C6D;ck(!MasterEligibilityHookH21(npcTrigger));returnAt=save;}
 DetourModKit::hook::MidContext c{};c.di=reinterpret_cast<std::uintptr_t>(t);
 for(unsigned term=0;term<8;++term){const bool pass=int(term)!=failAt;
  switch(term){case 0:c.ax=pass;MasterTermH21(c,0);break;case 1:c.ax=pass;MasterTermH21(c,1);break;
  case 2:c.ax=pass;MasterTermH21(c,2);break;case 3:c.ax=!pass;MasterTermH21(c,3);break;
  case 4:c.ax=0;MasterTermH21(c,4);c.ax=!pass;if(pass)MasterTermH21(c,5);break;
  case 5:c.ax=pass;MasterTermH21(c,6);break;
  case 6:{const auto save=returnAt;returnAt=WhBase()+0x90A6B5;const bool v=MasterContextHookH21(actor);returnAt=save;ck(v==pass);break;}
  case 7:c.ax=0;MasterTermH21(c,7);c.ax=pass?1:2;c.bx=2;if(pass)MasterTermH21(c,8);break;}
  if(!pass)return false;
 }
 if(changeState)put(actor+0x210,static_cast<void*>(otherState));return failAt!=8;
}
static void setup(){recording=resolve=true;originals=contexts=0;failAt=-1;nested=changeState=false;diagnostic_trace::lines.clear();++diagnostics::capture168::gate.session[0];
 put(trigger,WhBase()+0x3B81528);put(trigger+8,static_cast<void*>(actor));put(trigger+0x38,static_cast<void*>(npcActor));
 put(npcTrigger,WhBase()+0x3B81528);put(npcTrigger+8,static_cast<void*>(npcActor));put(npcTrigger+0x38,static_cast<void*>(actor));
 put(actor+0x1F8,static_cast<void*>(player));put(actor+0x210,static_cast<void*>(state));put(npcActor+0x1F8,reinterpret_cast<void*>(0x1234));put(npcActor+0x210,static_cast<void*>(opponentState));
 put(state+0x7D8,0);put(state+0xF8,2);put(opponentState+0x158,1);put(otherState+0x7D8,99);
 g_master_eligibility_ready_h21=true;g_master_eligibility_original_h21=&Original;g_master_context_original_h21=&ContextLeaf;returnAt=WhBase()+0x909C6D;
}
int main(){for(int failure=0;failure<9;++failure){setup();failAt=failure;ck(!MasterEligibilityHookH21(trigger));ck(originals==1);ck(contexts==(failure>=6?1:0));ck(diagnostic_trace::lines.size()==1);
 const auto& line=diagnostic_trace::lines[0];ck(line.find("first="+std::string(failure==4||failure==7?"UNKNOWN_OBSERVATION":me21::name(me21::Term(failure))))!=std::string::npos);ck(line.find("source=AUTHORED_EVENT")!=std::string::npos);}
 setup();ck(MasterEligibilityHookH21(trigger));ck(originals==1&&contexts==1);ck(diagnostic_trace::lines[0].find("candidates=PASS")!=std::string::npos);
 setup();recording=false;ck(MasterEligibilityHookH21(trigger));ck(originals==1&&contexts==1&&diagnostic_trace::lines.empty());
 setup();ck(!MasterEligibilityHookH21(npcTrigger));ck(originals==1&&contexts==0&&diagnostic_trace::lines.empty());
 setup();nested=true;ck(MasterEligibilityHookH21(trigger));ck(originals==2&&contexts==1&&diagnostic_trace::lines.size()==1);ck(g_master_scope_h21==nullptr);
 setup();changeState=true;ck(MasterEligibilityHookH21(trigger));ck(diagnostic_trace::lines[0].find("ms7D8=0/-1")!=std::string::npos);
 setup();resolve=false;ck(MasterEligibilityHookH21(trigger));ck(diagnostic_trace::lines.empty()&&originals==1);
 setup();returnAt=WhBase()+0x909EA0;ck(MasterEligibilityHookH21(trigger));ck(MasterEligibilityHookH21(trigger));ck(originals==2&&diagnostic_trace::lines.size()==1);
 setup();MasterEligibilityScopeH21 manual{};manual.active=true;manual.attempt.actor=reinterpret_cast<std::uintptr_t>(actor);manual.attempt.trigger=reinterpret_cast<std::uintptr_t>(trigger);g_master_scope_h21=&manual;
 returnAt=WhBase()+0x1234;ck(MasterContextHookH21(actor));ck(contexts==1&&manual.attempt.result[unsigned(me21::Term::Context)]==me21::Result::NotReached);
 DetourModKit::hook::MidContext foreign{};foreign.di=reinterpret_cast<std::uintptr_t>(npcTrigger);foreign.ax=1;MasterTermH21(foreign,0);ck(manual.attempt.result[0]==me21::Result::NotReached);g_master_scope_h21=nullptr;
 // Shared accepted entries have no dependency on AL/EAX/EBX. Rejected
 // branches never run them and must stay UNKNOWN rather than become FAIL.
 setup();MasterEligibilityScopeH21 accepted{};accepted.active=true;accepted.attempt.trigger=reinterpret_cast<std::uintptr_t>(trigger);g_master_scope_h21=&accepted;
 DetourModKit::hook::MidContext shared{};shared.di=accepted.attempt.trigger;shared.ax=shared.bx=0;
 MasterTermH21(shared,5);MasterTermH21(shared,8);
 ck(accepted.attempt.result[unsigned(me21::Term::OwnPhase)]==me21::Result::Pass);
 ck(accepted.attempt.result[unsigned(me21::Term::OwnerActionExclusion)]==me21::Result::Pass);g_master_scope_h21=nullptr;
 me21::Attempt missing{};missing.observed(me21::Term::Zone,true);missing.complete(false);ck(missing.result[0]==me21::Result::Unknown);
 me21::Attempt unobserved{};unobserved.complete(true);ck(unobserved.result[0]==me21::Result::Unknown);
 setup();g_master_eligibility_hook_h21.emplace();g_master_context_hook_h21.emplace();g_master_terms_h21[0].emplace();g_master_terms_h21[0]->fail=true;
 ShutdownMasterEligibilityH21();ck(!g_master_eligibility_ready_h21&&g_master_terms_h21[0].has_value());
 ck(g_master_eligibility_original_h21==&Original&&g_master_context_original_h21==&ContextLeaf);
 ck(MasterEligibilityHookH21(trigger)&&originals==1&&diagnostic_trace::lines.empty());
 g_master_terms_h21[0]->fail=false;ShutdownMasterEligibilityH21();ck(!g_master_terms_h21[0]&&!g_master_eligibility_original_h21&&!g_master_context_original_h21);
 g_master_terms_h21[0].emplace();g_master_terms_h21[0]->live=false;g_master_terms_h21[0]->fail=true;ShutdownMasterEligibilityH21();ck(!g_master_terms_h21[0]);
 // Preserve every pre-publication cleanup assertion above, then separately
 // model actual Arm's inline-enable fence and terminal executable retirement.
 setup();g_master_inline_retirement_h21=new(std::nothrow) MasterInlineRetirementH21;ck(g_master_inline_retirement_h21!=nullptr);
 g_master_inline_enable_attempted_h21=true;
 g_master_eligibility_hook_h21.emplace();g_master_context_hook_h21.emplace();g_master_terms_h21[0].emplace();g_master_terms_h21[0]->fail=true;
 ShutdownMasterEligibilityH21();ck(!g_master_eligibility_ready_h21&&g_master_terms_h21[0].has_value());
 ck(g_master_inline_retired_h21&&!g_master_eligibility_hook_h21&&!g_master_context_hook_h21);
 ck(g_master_inline_retirement_h21->outer.has_value()&&g_master_inline_retirement_h21->context.has_value());
 ck(!g_master_inline_retirement_h21->outer->live&&!g_master_inline_retirement_h21->context->live);
 ck(g_master_eligibility_original_h21==&Original&&g_master_context_original_h21==&ContextLeaf);
 ck(MasterEligibilityHookH21(trigger)&&originals==1&&diagnostic_trace::lines.empty());
 g_master_terms_h21[0]->fail=false;ShutdownMasterEligibilityH21();ck(!g_master_terms_h21[0]);
 ck(g_master_inline_retirement_h21->outer.has_value()&&g_master_inline_retirement_h21->context.has_value());
 ck(g_master_eligibility_original_h21==&Original&&g_master_context_original_h21==&ContextLeaf);
 std::printf("PASS %u actual master outer/mid/context checks; native eligibility original mocked once, no predicates recalled\n",checks);
}
