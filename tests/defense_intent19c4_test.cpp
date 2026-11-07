#include <array>
#include <atomic>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <string>
#include <limits>
#include <cassert>
#include "diagnostics/native_defense_intent19c4.h"
#include "vrik/contact_authority176.h"
namespace mo=kcd2vr::motion59c4;
constexpr std::uintptr_t g_base=0x180000000ull;
constexpr unsigned kDefenseZone=0x150,kAttackType=0x210,kAttackZone=0xF0,kCommittedZone=0x180;
std::array<unsigned char,0x500> actorMemory{},targetMemory{};
std::array<unsigned char,0x300> playerMemory{};
std::array<unsigned char,0xE00> stateMemory{},targetStateMemory{};
std::array<unsigned char,0x60> triggerMemory{},handlerMemory{};
std::uint64_t hostNow=1000;bool frameOK=true,nativeAction=false,paired=false,readFail=false;
int phaseCalls=0,boolWrites=0,leadCalls=0;bool sawIntent=false,nativeEligible=true,nativeOpened=false;
unsigned hostReadCalls=0;
bool takeState=false,takeTarget=false,takeBool=false,takeAction=false,failRestore=false;
float leadResult=-1;std::uintptr_t hostCaller=g_base+0x9142E4;
static std::uintptr_t Tr(){return reinterpret_cast<std::uintptr_t>(triggerMemory.data());}
static std::uintptr_t H(){return reinterpret_cast<std::uintptr_t>(handlerMemory.data());}
static std::uintptr_t A(){return reinterpret_cast<std::uintptr_t>(actorMemory.data());}
static std::uintptr_t P(){return reinterpret_cast<std::uintptr_t>(playerMemory.data());}
static std::uintptr_t S(){return reinterpret_cast<std::uintptr_t>(stateMemory.data());}
static std::uintptr_t T(){return reinterpret_cast<std::uintptr_t>(targetMemory.data());}
static std::uintptr_t TS(){return reinterpret_cast<std::uintptr_t>(targetStateMemory.data());}
template<class V>void Put(std::uintptr_t p,const V& v){std::memcpy(reinterpret_cast<void*>(p),&v,sizeof(v));}
static bool HostRead(std::uintptr_t p,void* out,std::size_t n){
 ++hostReadCalls;
 if(readFail)return false;
 for(auto range:{std::pair{A(),actorMemory.size()},std::pair{P(),playerMemory.size()},std::pair{S(),stateMemory.size()},std::pair{T(),targetMemory.size()},std::pair{TS(),targetStateMemory.size()},std::pair{Tr(),triggerMemory.size()},std::pair{H(),handlerMemory.size()}})
  if(p>=range.first&&p-range.first<=range.second&&n<=range.second-(p-range.first)){std::memcpy(out,reinterpret_cast<void*>(p),n);return true;}
 return false;
}
template<class V>V Rd(const void* p,std::size_t off,V fallback){V v{};return HostRead(reinterpret_cast<std::uintptr_t>(p)+off,&v,sizeof(v))?v:fallback;}
static int I(const void* p,unsigned o){return Rd<int>(p,o+8,-99);}
static float F(const void* p,unsigned o){return Rd<float>(p,o+8,-99.f);}
static std::uint64_t GetTickCount64(){return hostNow;}
namespace frik_pose_probe {static int GeometryOwnerStage49(){return paired?6:0;}static bool GeometryOwnerValid178r30(std::uint64_t,std::uint64_t,std::uint64_t,std::uint64_t){return frameOK;}}
namespace diagnostics::capture168 {enum class Domain{Combat};static bool active(Domain,std::uint64_t){return false;}}
namespace diagnostic_trace {static void Write(const char*){}}
static bool CombatActor(const void* actor,const void*& entity,const void*& state,bool){if(actor!=reinterpret_cast<void*>(A()))return false;entity=actor;state=reinterpret_cast<void*>(S());return true;}
static bool IsPlayer(const void* p){return p==reinterpret_cast<void*>(A());}
static void HostPhaseNotify();
namespace geom27 {
constexpr unsigned kActionPhaseProp39=0x30;
static const void* LiveAttackAction178r35(const void*){return nativeAction?reinterpret_cast<void*>(0x42):nullptr;}
namespace vrstate596 {
static std::atomic<bool> g_consequence{true},g_fault{false},g_reset{false};
static const void* g_owner_state=nullptr;static const void* g_defense_row=reinterpret_cast<void*>(0x99);
static bool g_defense_owned=true,g_clock_owned=true;static int g_written_defense=1;
static float g_written_clock=.14f;constexpr unsigned kHitRemaining=0x450;
static mo::Key g_key5912{};struct Prediction{mo::Key key{};std::uint64_t deadline=1140;}g_prediction19c3;
static std::atomic<const void*> g_henry_actor{nullptr};
static std::atomic<bool> g_telegraph_strike59c10{true};static std::atomic<int> g_stage{4};static std::atomic<unsigned> g_forward_offers18{0};
struct TelegraphOffer{mo::Key key{};std::uint64_t tick=0;bool pending=false,open=false;};static TelegraphOffer g_telegraph_offer;
static void Log(const char*,int,int,const char*){}
static float PredictionRemaining19c3(const Prediction& p,std::uint64_t now){return now<p.deadline?float(p.deadline-now)*.001f:0.f;}
static bool Publish(const void* p,int phase,int,int){Put(reinterpret_cast<std::uintptr_t>(p)+kActionPhaseProp39+8,phase);g_stage=phase;return true;}

struct OwnPhaseWrite19{const void* state=nullptr;int attackType=-2;};static OwnPhaseWrite19 t_own_phase_write19;
struct OwnPhaseWriteScope19{OwnPhaseWrite19 old;OwnPhaseWriteScope19(const void* p,int t):old(t_own_phase_write19){t_own_phase_write19={p,t};}~OwnPhaseWriteScope19(){t_own_phase_write19=old;}};
static int g_last_set=0;
static void Fault(const char*){g_fault=true;}
static bool SetBool(const void* p,unsigned o,bool v){++boolWrites;if(failRestore&&!v)return false;Put(reinterpret_cast<std::uintptr_t>(p)+o+8,static_cast<unsigned char>(v));return true;}
static bool SetInt(const void* p,unsigned o,int v){++phaseCalls;Put(reinterpret_cast<std::uintptr_t>(p)+o+8,v);HostPhaseNotify();return true;}
}}
#define _ReturnAddress() reinterpret_cast<void*>(hostCaller)
#include "defense_intent_adapter19c4.inc"
#undef _ReturnAddress
static float NativeLead(const void*,const void*){++leadCalls;return .15f;}
static void HostPhaseNotify(){
 namespace vs=geom27::vrstate596;auto* s=vs::t_defense_intent19c4;sawIntent=s&&s->qualified;
 leadResult=defense_offer19c4::LeadDetour(nullptr,reinterpret_cast<void*>(A()));
 // Native eligibility stays a separate decision. The adapter has no path
 // setting this flag or writing an NPC phase/block outcome.
 nativeOpened=nativeEligible&&kcd2vr::defense_intent19c4::Bool(HostRead,g_base,S(),0x5A0)==1;
 Put(Tr()+0x19,static_cast<unsigned char>(nativeOpened));
 (void)defense_offer19c4::CaptureOffer(s,reinterpret_cast<void*>(A()),reinterpret_cast<void*>(Tr()),reinterpret_cast<void*>(H()),.3f);
 if(takeState)Put(A()+0x210,TS());
 if(takeTarget)Put(S()+0xCF8,std::uintptr_t(0));
 if(takeBool)Put(S()+0x5A0+8,static_cast<unsigned char>(0));
 if(takeAction)nativeAction=true;
}
static void Reset(){
 actorMemory.fill(0);playerMemory.fill(0);targetMemory.fill(0);stateMemory.fill(0);targetStateMemory.fill(0);
 triggerMemory.fill(0);handlerMemory.fill(0);hostNow=1000;frameOK=true;nativeAction=paired=readFail=false;phaseCalls=boolWrites=leadCalls=0;sawIntent=nativeOpened=false;nativeEligible=true;
 takeState=takeTarget=takeBool=takeAction=failRestore=false;leadResult=-1;hostCaller=g_base+0x9142E4;
 namespace vs=geom27::vrstate596;vs::g_consequence=true;vs::g_fault=false;vs::g_reset=false;
 vs::g_owner_state=reinterpret_cast<void*>(S());vs::g_defense_row=reinterpret_cast<void*>(0x99);vs::g_defense_owned=vs::g_clock_owned=true;
 vs::g_telegraph_strike59c10=true;vs::g_stage=4;vs::g_forward_offers18=0;vs::g_last_offer19c4={};
 vs::g_defense_transitions19c4={};
 vs::g_written_defense=1;vs::g_written_clock=.14f;vs::g_henry_actor=reinterpret_cast<void*>(A());vs::g_defense_intent_hooks19c4=true;
 defense_offer19c4::g_pending=false;defense_offer19c4::g_lead_original=&NativeLead;mo::selected=true;mo::attempts.clear();
 vs::g_key5912={42,11,2,3,4,kcd2vr::contact176::recoveryEpoch55c1.load(),P()};
 vs::g_prediction19c3={vs::g_key5912,1140};assert(mo::attempts.event(vs::g_key5912,mo::Event::Prepare,hostNow,hostNow,1,0));
 Put(A(),g_base+0x3A80610);Put(P(),g_base+0x3AA4B98);Put(P()+0x278,A());
 Put(T(),g_base+0x3E3FAC8);Put(A()+0x210,S());Put(T()+0x210,TS());
 Put(A()+0x1F8,vs::g_key5912.player);Put(S()+0xCE0,A());Put(TS()+0xCE0,T());Put(S()+0xCF8,T());Put(TS()+0xCF8,A());
 Put(Tr(),g_base+0x3B813F8);Put(Tr()+0x38,A());Put(Tr()+8,T());Put(H()+8,T());Put(Tr()+0x1B,static_cast<unsigned char>(1));Put(Tr()+0x1C,.14f);
 vs::g_telegraph_offer={vs::g_key5912,hostNow,true,false};
 Put(S()+kAttackType+8,1);Put(S()+kAttackZone+8,1);Put(S()+kCommittedZone+8,1);
 Put(S()+0x30+8,1);Put(S()+0x150+8,1);Put(S()+0x450+8,.14f);
 for(unsigned o:{0x4B0u,0x4E0u,0x5A0u}){Put(S()+o,g_base+0x3B0A6A0);Put(S()+o+8,static_cast<unsigned char>(o==0x4B0));}
}
int main(){namespace vs=geom27::vrstate596;namespace di=kcd2vr::defense_intent19c4;
 unsigned checks=0;auto run=[&]{++checks;return vs::PublishPhase19c4(reinterpret_cast<void*>(S()),4,1);};
 Reset();assert(run()&&sawIntent&&nativeOpened&&leadResult==0.f&&boolWrites==2&&phaseCalls==1&&leadCalls==1);
 assert(di::Bool(HostRead,g_base,S(),0x5A0)==0); // temporary only, no persistent readiness
 // C4R1's fixture incorrectly modelled Henry as the NPC/base type. The
 // actual derived Henry above must qualify; that old artificial layout and
 // either broken C_Player backlink must never publish the scoped intent.
 for(int why=0;why<4;++why){Reset();switch(why){
  case 0:Put(A(),g_base+0x3E3FAC8);break;
  case 1:Put(P(),g_base+0x3A80610);break;
  case 2:Put(P()+0x278,T());break;
  case 3:Put(A()+0x1F8,T());break;}
  assert(run()&&!sawIntent&&!nativeOpened&&boolWrites==0&&leadResult==.15f);}
 Reset();nativeEligible=false;assert(run()&&sawIntent&&!nativeOpened); // no forced block outcome
 Reset();Put(S()+0x5A0+8,static_cast<unsigned char>(1));assert(run()&&sawIntent&&boolWrites==0);
 Reset();takeBool=true;assert(run()&&boolWrites==1&&di::Bool(HostRead,g_base,S(),0x5A0)==0);
 Reset();takeAction=true;assert(run()&&boolWrites==1&&di::Bool(HostRead,g_base,S(),0x5A0)==1); // new native action wins
 Reset();takeState=true;assert(!run()&&vs::g_fault&&boolWrites==1);assert(di::Bool(HostRead,g_base,TS(),0x5A0)==-1);
 Reset();takeTarget=true;assert(run()&&boolWrites==2); // restore our unchanged actor-local input, never opponent data
 Reset();failRestore=true;assert(!run()&&vs::g_fault);
 for(int why=0;why<30;++why){Reset();switch(why){
  case 0:vs::g_defense_intent_hooks19c4=false;break;case 1:vs::g_consequence=false;break;
  case 2:vs::g_fault=true;break;case 3:vs::g_reset=true;break;case 4:mo::selected=false;break;
  case 5:vs::g_owner_state=reinterpret_cast<void*>(TS());break;case 6:vs::g_defense_owned=false;break;
  case 7:vs::g_written_defense=-1;Put(S()+0x150+8,-1);break;case 8:vs::g_clock_owned=false;break;
  case 9:++vs::g_prediction19c3.key.gesture;break;case 10:++vs::g_key5912.recovery;break;
  case 11:paired=true;break;case 12:frameOK=false;break;case 13:vs::g_henry_actor=nullptr;break;
  case 14:nativeAction=true;break;case 15:Put(S()+0x150+8,2);break;case 16:Put(S()+0x450+8,.13f);break;
  case 17:mo::attempts.clear();break;case 18:hostNow=1140;break;case 19:Put(S()+0x30+8,4);break;
  case 20:Put(A(),std::uintptr_t(0));break;case 21:Put(A()+0x210,TS());break;
  case 22:Put(S()+0xCE0,T());break;case 23:Put(A()+0x1F8,std::uintptr_t(0x999));break;
  case 24:Put(S()+0xCF8,A());break;case 25:Put(TS()+0xCF8,T());break;
  case 26:Put(S()+0x4B0+8,static_cast<unsigned char>(0));break;
  case 27:Put(S()+0x4E0,std::uintptr_t(0));break;case 28:Put(S()+0x5A0+8,static_cast<unsigned char>(2));break;
  case 29:readFail=true;break;}
  assert(run()&&!sawIntent&&boolWrites==0&&leadResult==.15f);
 }
 Reset();Put(S()+0x4B0+8,static_cast<unsigned char>(0));Put(S()+0x4E0+8,static_cast<unsigned char>(1));assert(run()&&sawIntent);
 Reset();hostCaller++;assert(run()&&sawIntent&&leadResult==.15f); // exact return site only
 Reset();assert(vs::PublishPhase19c4(reinterpret_cast<void*>(S()),8,1)&&!sawIntent&&boolWrites==0);++checks;
 Reset();mo::attempts.event(vs::g_key5912,mo::Event::Cancel,hostNow,hostNow,1,0);assert(run()&&!sawIntent&&boolWrites==0);
 // Actual LeadResult/Current owner checks after a synchronous native call.
 for(int why=0;why<19;++why){Reset();vs::DefenseIntentScope19c4 s{};assert(vs::PrepareDefenseIntent19c4(reinterpret_cast<void*>(S()),4,hostNow,s));
  Put(S()+0x30+8,4);vs::OwnPhaseWriteScope19 own(reinterpret_cast<void*>(S()),1);
  switch(why){case 0:vs::g_defense_intent_hooks19c4=false;break;case 1:defense_offer19c4::g_pending=true;break;
   case 2:mo::selected=false;break;case 3:vs::g_fault=true;break;case 4:vs::g_reset=true;break;
   case 5:vs::g_henry_actor=nullptr;break;case 6:++vs::g_key5912.gesture;break;case 7:++vs::g_prediction19c3.key.request;break;
   case 8:paired=true;break;case 9:frameOK=false;break;case 10:nativeAction=true;break;case 11:hostNow=1140;break;
   case 12:Put(S()+0x30+8,8);break;case 13:Put(S()+0x150+8,5);break;case 14:Put(S()+0x450+8,.1f);break;
   case 15:Put(S()+0xCF8,A());break;case 16:Put(TS()+0xCE0,A());break;case 17:Put(TS()+0xCF8,T());break;
   case 18:Put(S()+0x5A0+8,static_cast<unsigned char>(0));break;}
  assert(defense_offer19c4::LeadResult(&s,reinterpret_cast<void*>(A()),g_base+0x9142E4,.15f)==.15f);++checks;
 }
 Reset();vs::DefenseIntentScope19c4 s{};assert(vs::PrepareDefenseIntent19c4(reinterpret_cast<void*>(S()),4,hostNow,s));
 Put(S()+0x30+8,4);vs::OwnPhaseWriteScope19 own(reinterpret_cast<void*>(S()),1);
 assert(defense_offer19c4::LeadResult(&s,reinterpret_cast<void*>(T()),g_base+0x9142E4,.15f)==.15f);++checks;
 assert(defense_offer19c4::LeadResult(&s,reinterpret_cast<void*>(A()),g_base+0x9142E4,-1.f)==-1.f);++checks;
 assert(std::isnan(defense_offer19c4::LeadResult(&s,reinterpret_cast<void*>(A()),g_base+0x9142E4,std::numeric_limits<float>::quiet_NaN())));++checks;
 // A null authored row with a current native-qualified VR zone now reaches
 // the exact same intent transaction, without a fabricated table row.
 for(int z:{0,1,2,5}){Reset();vs::g_defense_row=nullptr;Put(S()+kAttackZone+8,z);Put(S()+kCommittedZone+8,z);int value=-1;
  assert(vs::FallbackDefense19c4(reinterpret_cast<void*>(S()),z,0,hostNow,value)&&value==z);++checks;
  vs::g_written_defense=z;Put(S()+kDefenseZone+8,z);assert(run()&&sawIntent);}
 Reset();Put(S()+kAttackType+8,0);Put(S()+kAttackZone+8,5);Put(S()+kCommittedZone+8,5);int value=-1;
 assert(vs::FallbackDefense19c4(reinterpret_cast<void*>(S()),5,1,hostNow,value)&&value==5);++checks;
 for(int z:{-1,3,4,6}){Reset();int v=-1;assert(!vs::FallbackDefense19c4(reinterpret_cast<void*>(S()),z,0,hostNow,v));++checks;}
 Reset();Put(S()+kAttackZone+8,2);assert(!vs::FallbackDefense19c4(reinterpret_cast<void*>(S()),1,0,hostNow,value));++checks;
 Reset();Put(TS()+0xCF8,T());assert(!vs::FallbackDefense19c4(reinterpret_cast<void*>(S()),1,0,hostNow,value));++checks;
 Reset();mo::attempts.clear();assert(!vs::FallbackDefense19c4(reinterpret_cast<void*>(S()),1,0,hostNow,value));++checks;
 // H21 readiness observation requires the native active bit; Henry stays4.
 Reset();assert(run());assert(vs::ObserveActiveOfferH21(reinterpret_cast<void*>(S()),vs::g_key5912,1000,1,0,hostNow));
 assert(I(reinterpret_cast<void*>(S()),geom27::kActionPhaseProp39)==4&&!vs::g_telegraph_offer.open&&!vs::g_telegraph_offer.pending&&vs::g_forward_offers18==1);++checks;
 for(int why=0;why<17;++why){Reset();if(why==0)nativeEligible=false;assert(run());switch(why){
  case 1:vs::g_last_offer19c4.offerObserved=false;break;case 2:Put(Tr()+0x19,static_cast<unsigned char>(0));break;
  case 3:Put(Tr()+8,A());break;case 4:Put(Tr()+0x38,T());break;case 5:Put(Tr(),std::uintptr_t(0));break;
  case 6:hostNow=1140;break;case 7:Put(S()+0x30+8,2);break;case 8:Put(S()+0x150+8,-1);break;
  case 9:++vs::g_key5912.gesture;break;case 10:Put(TS()+0xCF8,T());break;case 11:frameOK=false;break;
  case 12:paired=true;break;case 13:nativeAction=true;break;case 14:vs::g_telegraph_strike59c10=false;break;
  case 15:mo::attempts.event(vs::g_key5912,mo::Event::Cancel,hostNow,hostNow,1,0);break;
  case 16:vs::g_telegraph_offer.pending=false;break;}
  const auto before=I(reinterpret_cast<void*>(S()),geom27::kActionPhaseProp39);
  assert(!vs::ObserveActiveOfferH21(reinterpret_cast<void*>(S()),vs::g_key5912,1000,1,0,hostNow));
  assert(I(reinterpret_cast<void*>(S()),geom27::kActionPhaseProp39)==before&&!vs::g_telegraph_offer.open&&vs::g_forward_offers18==0);++checks;
 }
 // Restoring an owned phase keeps native setter notifications, but cannot
 // lease the motion input or replace the lead/activation receipt a second time.
 for(auto purpose:{vs::PhasePurpose19c4::GuardReclaim,vs::PhasePurpose19c4::BorrowedContextRestore}){
  Reset();assert(run());const auto offered=vs::g_last_offer19c4;const int oldWrites=boolWrites,oldPhases=phaseCalls;
  hostNow=1010;Put(S()+0x30+8,2);
  assert(vs::PublishPhase19c4(reinterpret_cast<void*>(S()),4,1,purpose));
  assert(boolWrites==oldWrites&&phaseCalls==oldPhases+1&&!sawIntent&&leadResult==.15f);
  assert(vs::g_last_offer19c4.c.now==offered.c.now&&mo::same(vs::g_last_offer19c4.c.key,offered.c.key));++checks;
 }
 Reset();Put(S()+0x5A0+8,static_cast<unsigned char>(1));
 assert(vs::PublishPhase19c4(reinterpret_cast<void*>(S()),4,1,vs::PhasePurpose19c4::GuardReclaim));
 assert(nativeOpened&&!sawIntent&&boolWrites==0&&leadResult==.15f&&!vs::g_last_offer19c4.offerObserved);++checks;
 // Replayed prepare after forgetting/phase recovery has the same full-key
 // transition receipt. Changed direction/technique remains a genuine offer.
 Reset();assert(run());const auto originalDeadline=vs::g_prediction19c3.deadline;
 Put(S()+0x30+8,2);const int originalWrites=boolWrites;assert(run()&&!sawIntent&&boolWrites==originalWrites);
 for(int z:{2,1}){hostNow++;Put(S()+0x30+8,2);Put(S()+0x150+8,z);vs::g_written_defense=z;
  assert(run()&&sawIntent&&vs::g_prediction19c3.deadline==originalDeadline);}
 hostNow++;Put(S()+0x30+8,2);Put(S()+kAttackType+8,0);assert(run()&&sawIntent);
 // A foreign B followed by A restoration cannot erase A's already-issued
 // transition. Full-key generation/recovery remain separate, not gesture-only.
 Reset();assert(run());const auto keyA=vs::g_key5912;
 auto keyB=keyA;++keyB.gesture;vs::g_key5912=keyB;vs::g_prediction19c3.key=keyB;
 assert(mo::attempts.event(keyB,mo::Event::Prepare,hostNow,hostNow,1,0));Put(S()+0x30+8,2);assert(run()&&sawIntent);
 vs::g_key5912=keyA;vs::g_prediction19c3.key=keyA;Put(S()+0x30+8,2);assert(run()&&!sawIntent);
 for(unsigned field=0;field<7;++field){Reset();assert(run());auto changed=vs::g_key5912;
  switch(field){case 0:++changed.gesture;break;case 1:++changed.weapon;break;case 2:++changed.generation;break;
   case 3:++changed.body;break;case 4:++changed.request;break;case 5:++changed.recovery;kcd2vr::contact176::recoveryEpoch55c1.store(changed.recovery);break;
   case 6:++changed.player;break;}
  vs::g_key5912=changed;vs::g_prediction19c3.key=changed;mo::attempts.clear();
  assert(mo::attempts.event(changed,mo::Event::Prepare,hostNow,hostNow,1,0));Put(S()+0x30+8,2);
  assert(run()&&(field==6?!sawIntent:sawIntent));
 }
 Reset();vs::g_defense_intent_hooks19c4=false;vs::DefenseIntentScope19c4 missing{};
 assert(!vs::PrepareDefenseIntent19c4(reinterpret_cast<void*>(S()),4,hostNow,missing));
 assert(std::string(missing.reason)=="HOOK_CAPABILITY_UNAVAILABLE");++checks;
 Reset();hostNow=1140;vs::DefenseIntentScope19c4 expired{};
 assert(!vs::PrepareDefenseIntent19c4(reinterpret_cast<void*>(S()),4,hostNow,expired));
 assert(std::string(expired.reason)=="PREDICTION_EXPIRED");++checks;
 // The fast path has no A73A04 witness and must not even read rbp+48.
 Reset();assert(run());auto allowanceScope=vs::g_last_offer19c4;Put(S()+0x5A0+8,static_cast<unsigned char>(1));
 vs::OwnPhaseWriteScope19 allowanceOwner(reinterpret_cast<void*>(S()),1);
 allowanceScope.offsetAdapted=false;Put(H()+0x48,123456.f);const auto readsBefore=hostReadCalls;
 const float unavailable=defense_offer19c4::ReadAllowance19c4(&allowanceScope,reinterpret_cast<void*>(H()));
 assert(unavailable==-1.f&&hostReadCalls==readsBefore);
 assert(defense_offer19c4::CaptureOffer(&allowanceScope,reinterpret_cast<void*>(A()),reinterpret_cast<void*>(Tr()),reinterpret_cast<void*>(H()),123456.f));
 assert(!allowanceScope.allowanceKnown&&allowanceScope.nativeAllowance==-1.f);++checks;
 allowanceScope.offsetAdapted=true;Put(H()+0x48,.3f);
 const float allowance=defense_offer19c4::ReadAllowance19c4(&allowanceScope,reinterpret_cast<void*>(H()));
 assert(defense_offer19c4::CaptureOffer(&allowanceScope,reinterpret_cast<void*>(A()),reinterpret_cast<void*>(Tr()),reinterpret_cast<void*>(H()),allowance));
 assert(allowanceScope.allowanceKnown&&allowanceScope.nativeAllowance==.3f);++checks;
 for(float unknown:{-1.f,std::numeric_limits<float>::quiet_NaN()}){
  assert(defense_offer19c4::CaptureOffer(&allowanceScope,reinterpret_cast<void*>(A()),reinterpret_cast<void*>(Tr()),reinterpret_cast<void*>(H()),unknown));
  assert(!allowanceScope.allowanceKnown&&allowanceScope.nativeAllowance==-1.f);++checks;
 }
 // Actual retirement helper against a fake backend: failed disarm stays
 // live; successful disarm releases ownership without freeing a trampoline.
 struct FakeHook {bool enabled=true,canDisable=true,released=false;unsigned disables=0;
  bool is_enabled()const{return enabled;}bool disable(){++disables;if(!canDisable)return false;enabled=false;return true;}
  void release(){released=true;}};
 struct FakeHolder {FakeHook hook{};bool present=true,freed=false;explicit operator bool()const{return present;}
  FakeHook* operator->(){return &hook;}void reset(){freed=!hook.released;present=false;}};
 FakeHolder holder;bool retired=false;holder.hook.canDisable=false;
 assert(!defense_offer19c4::RetireInline19c4(holder,retired)&&holder.present&&!retired&&!holder.freed);++checks;
 holder.hook.canDisable=true;const auto originalFunction=defense_offer19c4::g_lead_original;
 assert(defense_offer19c4::RetireInline19c4(holder,retired)&&retired&&!holder.present&&!holder.freed&&holder.hook.released);
 assert(defense_offer19c4::g_lead_original==originalFunction&&originalFunction(nullptr,nullptr)==.15f);++checks;
 assert(defense_offer19c4::RetireInline19c4(holder,retired)&&!holder.freed);++checks;
 std::printf("PASS %u production phase/intent/offer ownership scenarios; native eligibility stays authoritative.\n",checks);
}
