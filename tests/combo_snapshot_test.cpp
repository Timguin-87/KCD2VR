// Executes the active decoder and callback wrapper against synthetic memory.
// These tests do NOT validate the retail ABI or run KCD2/MSVC/Windows hooks.
#include "../src/diagnostics/combo_observe.h"
#include <vector>
#include <string>
#include <cstdio>
using namespace kcd2vr::diagnostics::combo;
static int total=0,passed=0;
static void Check(bool yes,const char*name){++total;passed+=yes;std::printf("%s %s\n",yes?"PASS":"FAIL",name);}
struct Memory {
    std::vector<unsigned char> bytes=std::vector<unsigned char>(0x20000);
    Layout layout{0x180000000ull};
    static constexpr Address manager=0x1000,actor=0x2000,player=0x3000,entity=0x4000,states=0x5000,candidates=0x6000,recipe=0x8000,name=0xA000,steps=0xB000;
    bool deny=false,race=false;int vectorReads=0;
    template<class T> void put(Address a,const T&v){std::memcpy(bytes.data()+a,&v,sizeof(v));}
    Memory(){put(manager,layout.base+layout.managerVft);put(manager+8,actor);put(actor,layout.base+layout.actorVft);put(actor+0x1F8,player);put(player,layout.base+layout.playerVft);put(player+0x38,entity);put(entity,layout.base+layout.entityVft);put(entity+0xC,std::uint32_t(123));
        put(manager+0xC0,Vector{states,states+40,states+40});put(manager+0xA8,Vector{candidates,candidates+24,candidates+24});put(states+8,std::uint32_t(1));put(states+0x10,Vector{candidates,candidates+24,candidates+24});
        put(candidates,recipe);put(candidates+8,std::uint32_t(3));put(recipe+8,std::uint32_t(12));put(recipe+0x10,name);put(recipe+0x40,Vector{steps,steps+96,steps+96});std::memcpy(bytes.data()+name,"C12",4);
    }
    bool operator()(Address a,void*p,std::size_t n) noexcept {if(deny||a>bytes.size()||n>bytes.size()-a)return false;if(a==manager+0xC0&&++vectorReads==2&&race){Vector v{states+40,states+80,states+80};std::memcpy(p,&v,n);return true;}std::memcpy(p,bytes.data()+a,n);return true;}
    Snapshot snap(Address expected=entity){vectorReads=0;return Capture(*this,layout,manager,expected);}
};
int main(){
 Memory m;auto s=m.snap();
 Check(s.status==Status::Ok&&s.identity.entityId==123,"exact player chain and entity ID");
 Check(s.statesCopied==1&&s.states[0].cursor==1&&s.stateCount==1,"native state cursor captured");
 Check(s.candidatesCopied==1&&s.candidates[0].requiredSteps==3&&s.candidates[0].stepCount==3,"required steps separate from state cursor");
 Check(s.candidates[0].nameExact&&std::string(s.candidates[0].name.data())=="C12","candidate name read without native string function");
 Check(Comparable(s,s)&&!ObservedFieldsChanged(s,s),"unchanged complete snapshot is not advancement");
 m.put(Memory::states+8,std::uint32_t(2));auto t=m.snap();Check(Comparable(s,t)&&ObservedFieldsChanged(s,t),"cursor change observed without claiming hit acceptance");
 m.put(Memory::manager+0xC0,Vector{});t=m.snap();Check(t.status==Status::Ok&&t.stateCount==0&&ObservedFieldsChanged(s,t),"native reset represented as empty state not finished combo");
 m=Memory{};Check(m.snap(0x4444).status==Status::IdentityMismatch,"known Henry mismatch rejects record");
 m.put(Memory::player,Address(0xDEAD));Check(m.snap(0).status==Status::IdentityMismatch,"NPC or secondary player vtable rejected even without cached Henry");
 m=Memory{};m.put(Memory::actor,Address(0xDEAD));Check(m.snap().status==Status::IdentityMismatch,"combat actor layer validated not just candidate name");
 m=Memory{};m.put(Memory::manager,Address(0xDEAD));Check(m.snap().status==Status::IdentityMismatch,"manager type gate");
 m=Memory{};m.put(Memory::entity+0xC,std::uint32_t(0));Check(m.snap().status==Status::IdentityMismatch,"zero entity ID rejected");
 m=Memory{};m.deny=true;Check(m.snap().status!=Status::Ok,"unreadable memory never reported as valid empty vector");
 m=Memory{};m.put(Memory::manager+0xC0,Vector{0,40,40});Check(m.snap().status==Status::InvalidVector,"null begin with nonnull end rejected");
 m=Memory{};m.put(Memory::manager+0xC0,Vector{Memory::states,Memory::states+41,Memory::states+41});Check(m.snap().status==Status::InvalidVector,"nonintegral state stride rejected");
 m=Memory{};m.put(Memory::manager+0xC0,Vector{Memory::states+40,Memory::states,Memory::states+40});Check(m.snap().status==Status::InvalidVector,"reversed vector rejected without unsigned underflow");
 m=Memory{};m.put(Memory::manager+0xC0,Vector{Memory::states,Memory::states+65*40,Memory::states+65*40});Check(m.snap().status==Status::InvalidVector,"hostile unbounded native vector rejected");
 m=Memory{};m.race=true;Check(m.snap().status==Status::ChangedDuringCopy,"state vector replacement rejects mixed observation");
 m=Memory{};m.put(Memory::states+0x10,Vector{Memory::candidates,Memory::candidates+25,Memory::candidates+25});Check(m.snap().status==Status::InvalidVector,"candidate stride validated independently");
 m=Memory{};m.put(Memory::recipe+0x40,Vector{Memory::steps,Memory::steps+33,Memory::steps+33});Check(m.snap().status==Status::InvalidRecipe,"recipe step stride validated");
 m=Memory{};m.put(Memory::candidates,Address(0));Check(m.snap().status==Status::InvalidRecipe,"missing recipe is not invented");
 m=Memory{};m.put(Memory::recipe+0x10,Address(0));t=m.snap();Check(t.status==Status::Ok&&!t.candidates[0].nameExact,"unavailable name leaves valid raw state unmapped");
 m=Memory{};std::memcpy(m.bytes.data()+Memory::name,"C\"x",4);t=m.snap();Check(!t.candidates[0].nameExact&&t.candidates[0].name[0]==0,"unsafe log token rejected not silently exact joined");
 m=Memory{};m.put(Memory::manager+0xC0,Vector{Memory::states,Memory::states+5*40,Memory::states+5*40});t=m.snap();Check(t.status==Status::Ok&&t.truncated&&t.statesCopied==4&&!Comparable(s,t),"bounded states explicitly truncated no full comparison");
 m=Memory{};for(unsigned i=1;i<9;++i)std::memcpy(m.bytes.data()+Memory::candidates+i*24,m.bytes.data()+Memory::candidates,24);m.put(Memory::states+0x10,Vector{Memory::candidates,Memory::candidates+9*24,Memory::candidates+9*24});t=m.snap();Check(t.truncated&&t.candidatesCopied==8&&!Comparable(s,t),"bounded candidate detail not reported complete");
 m=Memory{};auto immutable=m.bytes;s=m.snap();Check(m.bytes==immutable,"decoder writes no native memory");
 Identity id{};Check(!Identify(m,m.layout,UINT64_MAX-4,0,id),"overflow address rejected");
 std::uint32_t count=0;Check(VectorCount(Vector{},40,64,count)&&count==0,"all-null native vector valid empty");
 s=m.snap();t=s;t.identity.entityId=999;Check(!Comparable(s,t),"same candidate name on different entity cannot form progression");
 int calls=0,captures=0;auto e=Around(false,[&]{++calls;},[&]{++captures;return m.snap();});Check(calls==1&&captures==0&&!e.captured,"OFF forwards exactly once without diagnostic reads");
 calls=captures=0;e=Around(true,[&]{++calls;m.put(Memory::states+8,std::uint32_t(2));},[&]{++captures;return m.snap();});Check(calls==1&&captures==2&&e.comparable&&e.changed,"actual wrapper captures before and after original exactly once");
 calls=0;m.deny=true;e=Around(true,[&]{++calls;},[&]{return m.snap();});Check(calls==1&&!e.comparable&&!e.changed,"failed decoding cannot suppress original or claim change");
 m=Memory{};IdentityTrace trace{};Identity identity{};
 Check(Identify(m,m.layout,Memory::manager,Memory::entity,identity,&trace)&&trace.gate==IdentityGate::Ok,"identity trace leaves exact native chain acceptance unchanged");
 m.put(Memory::actor,Address(0xBAD));
 Check(!Identify(m,m.layout,Memory::manager,Memory::entity,identity,&trace)&&trace.gate==IdentityGate::ActorType&&trace.partial.actor==Memory::actor&&trace.observedVtable==0xBAD,"first failing actor gate retains exact observed pointer and vtable");
 m=Memory{};Check(!Identify(m,m.layout,Memory::manager,Memory::entity+16,identity,&trace)&&trace.gate==IdentityGate::NotHenry,"other actor is diagnosed but not admitted as Henry");
 m=Memory{};m.put(Memory::actor,m.layout.base+m.layout.playerActorVft);
 Check(m.snap().status==Status::Ok,"retail C_CombatPlayer derived vtable passes the full owner/entity decoder");
 m.put(Memory::player,m.layout.base+0x3B71668);
 Check(m.snap().status==Status::IdentityMismatch,"derived combat owner does not admit C_Dog as C_Player");
 m=Memory{};m.put(Memory::actor,m.layout.base+m.layout.playerActorVft);
 Check(m.snap(Memory::entity+16).status==Status::IdentityMismatch,"derived player still must match the exact cached Henry entity");
 m.put(Memory::actor,m.layout.base+m.layout.playerActorVft+8);
 Check(m.snap().status==Status::IdentityMismatch,"adjacent unknown combat vtable is not accepted by range");
 const std::uintptr_t recheck[]{0xC5B45D,0x91496D,0x2256E33};
 auto close=InspectCloseStack(recheck,3);Check(close.callerRva==0x2256E33&&close.matches==1&&std::string(ClosePath(close))=="UPDATE_ELIGIBILITY_FALSE","exact native close return frame joins immediate caller");
 const std::uintptr_t timer[]{0x91496D,0x9087C5};close=InspectCloseStack(timer,2);Check(std::string(ClosePath(close))=="UPDATE_TIMER_EXPIRED","native clock expiry differs from update predicate close");
 const std::uintptr_t phase4[]{0x91496D,0x9111B4};close=InspectCloseStack(phase4,2);Check(std::string(ClosePath(close))=="PHASE4_CLOSE","phase4 close return has its own label");
 const std::uintptr_t phase1[]{0x91496D,0x911248};close=InspectCloseStack(phase1,2);Check(std::string(ClosePath(close))=="PHASE1_OR64_CLOSE","shared phase1/64 return does not invent which phase fired");
 const std::uintptr_t other[]{0x91496D,0x9144E8};close=InspectCloseStack(other,2);Check(close.callerRva==0x9144E8&&std::string(ClosePath(close))=="OTHER_NATIVE_CLOSE","unknown native close site retains its raw return RVA");
 const std::uintptr_t truncated[]{0x91496D};close=InspectCloseStack(truncated,1);Check(!close.callerRva&&std::string(ClosePath(close))=="CLOSE_CALLER_UNAVAILABLE","truncated stack cannot guess the caller");
 const std::uintptr_t foreign[]{0x91496D,0};close=InspectCloseStack(foreign,2);Check(!close.callerRva,"foreign immediate caller is not skipped to find a later convenient native frame");
 const std::uintptr_t recursive[]{0x91496D,0x2256E33,0x91496D,0x9087C5};close=InspectCloseStack(recursive,4);Check(!close.callerRva&&close.matches==2&&std::string(ClosePath(close))=="AMBIGUOUS_CLOSE_STACK","nested native closes remain ambiguous");
 const std::uintptr_t adjacent[]{0x91496C,0x2256E33};close=InspectCloseStack(adjacent,2);Check(!close.matches&&std::string(ClosePath(close))=="NO_CLOSE_FRAME","adjacent unknown return address is not accepted by range");
 Check(!InspectCloseStack(nullptr,0).callerRva,"unavailable unwind has no caller");
 std::printf("combo_snapshot=%d/%d; runtime=NOT_TESTED\n",passed,total);return passed==total?0:1;
}
