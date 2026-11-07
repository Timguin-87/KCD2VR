// Actual production receipt callbacks; native actor/memory/clock/fixtureCaller27 leaves only.
#include <array>
#include <atomic>
#include <cassert>
#include <cstdint>
#include <cstring>
#include <cstdio>
#include <mutex>
#include <vector>
#include <functional>
#include <optional>
#include "../src/vrik/motion_owned59c4.h"
static bool recording=true,keyKnown=true;static std::uintptr_t fixtureCaller27=0;static unsigned reads=0,calls=0,checks=0;
static std::uint64_t now=1000;static constexpr std::uintptr_t g_base=0x180000000ull,kCombatActorVft=0x3E3FAC8,kCombatPlayerVft=0x3A80610;
static std::array<unsigned char,0x500> npc{},henry{},rpgMemory{},incomingRpg{},action{},otherAction{};
static std::vector<std::pair<std::uintptr_t,std::size_t>> regions;
static std::function<void()> callback;
static void* selected=nullptr;static bool nullSelection=false,wrongHolder=false,nativeFault=false;
#if defined(_MSC_VER)
extern "C" __declspec(dllimport) void __stdcall RaiseException(unsigned long,unsigned long,unsigned long,const std::uintptr_t*);
#endif
#define KCD2VR_COMBO_CALLER() fixtureCaller27
static bool Active() noexcept{return recording;}
static std::uint64_t GetTickCount64() noexcept{return now;}
static unsigned GetCurrentThreadId() noexcept{return 17;}
template<class T> static T Rd(const void* p,std::uintptr_t off,T fallback) noexcept {
    ++reads;const auto start=reinterpret_cast<std::uintptr_t>(p)+off;
    for(const auto& r:regions)if(p&&start>=r.first&&start+sizeof(T)<=r.first+r.second){T out{};std::memcpy(&out,reinterpret_cast<const void*>(start),sizeof(out));return out;}
    return fallback;
}
template<class T> static void Put(void* p,std::size_t off,T value){std::memcpy(static_cast<unsigned char*>(p)+off,&value,sizeof(value));}
static bool CombatActor(const void* a,const void*& e,const void*& s,bool) noexcept {e=a;s=a;return a==npc.data()||a==henry.data();}
static bool IsPlayer(const void* p) noexcept{return p==henry.data();}
namespace diagnostics::capture168 {enum class Domain {Combat};struct Gate {std::array<std::atomic<std::uint64_t>,1> session{{7}};};static Gate gate;}
namespace diagnostic_trace {static void Write(const char*) noexcept {}}
#include "../src/diagnostics/native_defense_receipt27.inl"
namespace defense27 {bool CopyDirectKey(const void*,mo::Key& key) noexcept {key=keyKnown?mo::Key{1,2,3,4,5,6,7}:mo::Key{};return keyKnown;}}
static void* __fastcall Original(const void*,void** out,const void*,int,int) {
    ++calls;if(callback){auto cb=std::move(callback);callback={};cb();}
    if(out)*out=nullSelection?nullptr:selected;
#if defined(_MSC_VER)
    if(nativeFault)RaiseException(0xE1230027,0,0,nullptr);
#endif
    return wrongHolder?nullptr:out;
}
static void Check(bool ok){++checks;assert(ok);}
static void Reset(){
    recording=true;keyKnown=true;defense27::enabled=true;reads=calls=0;fixtureCaller27=g_base+0x14837B4;callback={};nullSelection=wrongHolder=nativeFault=false;
    defense27::head=defense27::count=0;defense27::counterFrame=nullptr;defense27::candidateFrame=nullptr;
    regions.clear();for(auto* b:{&npc,&henry,&rpgMemory,&incomingRpg,&action,&otherAction}){b->fill(0);regions.emplace_back(reinterpret_cast<std::uintptr_t>(b->data()),b->size());}
    Put(npc.data(),0,g_base+kCombatActorVft);Put(henry.data(),0,g_base+kCombatPlayerVft);
    Put(npc.data(),0x3A8,rpgMemory.data());Put(rpgMemory.data(),8,npc.data());Put(incomingRpg.data(),8,henry.data());
    Put(npc.data(),0x200,reinterpret_cast<void*>(0x12345));Put(action.data(),0,g_base+0x3A89688);
    Put(action.data(),0x40,reinterpret_cast<void*>(0x12345));Put(action.data(),0x58,reinterpret_cast<void*>(0x54321));
    selected=action.data();defense27::candidateOriginal=Original;
}
static void Evaluate(bool ok=true){
    std::array<unsigned char,64> request{},details{};Put(request.data(),0x10,npc.data());details[1]=1;details[3]=1;
    regions.emplace_back(reinterpret_cast<std::uintptr_t>(request.data()),request.size());regions.emplace_back(reinterpret_cast<std::uintptr_t>(details.data()),details.size());
    defense27::EvaluationScope e(incomingRpg.data(),request.data(),false,g_base+0x173BF58);e.Finish(ok,details.data());
    regions.resize(regions.size()-2);
}
static void* Candidate(void*& out){regions.emplace_back(reinterpret_cast<std::uintptr_t>(&out),sizeof(out));
    auto* ret=defense27::CandidateDetour(rpgMemory.data(),&out,henry.data(),1,2);regions.pop_back();return ret;}
static defense27::Receipt Pop(){defense27::Receipt r{};Check(defense27::Pop(r));return r;}
namespace defense27 {
static bool allowDisable=true;static unsigned releaseCalls=0,freed=0;
struct FakeHook {bool active=true,owned=true;~FakeHook(){if(owned)++freed;}bool is_enabled()const{return active;}
 bool disable(){if(!allowDisable)return false;active=false;return true;}void release(){owned=false;++releaseCalls;}};
static std::optional<FakeHook> candidateHook;static bool retired=false;
#include "native_defense_retirement27.inc"
}
static void* __fastcall CounterBody(void*,void** out,unsigned char,int,bool,int){Candidate(*out);return out;}
#if defined(_MSC_VER)
static bool ProbeSeh(){void* out=nullptr;
    __try{defense27::CounterInvoke(CounterBody,npc.data(),&out,6,1,false,2);}
    __except(1){return true;}return false;}
#endif
int main(){
    Reset();recording=false;void* out=nullptr;{defense27::CounterScope c(npc.data(),6);Candidate(out);
        defense27::EvaluationScope e(nullptr,nullptr,false,g_base+0x173BF58);e.Finish(true,nullptr);
        defense27::DispatchScope d(npc.data(),&out,g_base+0x1483924);d.Finish(true);}
    Check(calls==1&&reads==0&&defense27::count==0);
    Reset();{defense27::CounterScope c(npc.data(),6);callback=[](){Evaluate();};out=nullptr;Check(Candidate(out)==&out);auto r=Pop();
        Check(r.keyKnown&&r.evaluations==1&&r.evaluationOk==1&&r.predictedPerfect==1&&r.typeKnown&&r.managerKnown&&r.action==reinterpret_cast<std::uintptr_t>(action.data()));
        regions.emplace_back(reinterpret_cast<std::uintptr_t>(&out),sizeof(out));defense27::DispatchScope d(npc.data(),&out,g_base+0x1483924);
        // Both source holder and action can become unreadable during native dispatch.
        out=nullptr;regions.resize(4);const auto before=reads;d.Finish(true);Check(reads==before);r=Pop();
        Check(r.keyKnown&&!r.ambiguous&&r.dispatchReturned&&r.candidate!=0&&r.predictedPerfect==1);}
    Check(Pop().candidates==1&&calls==1);
    Reset();{defense27::CounterScope c(npc.data(),6);fixtureCaller27=123;Candidate(out);Check(defense27::count==0);}Check(Pop().candidates==0);
    Reset();{defense27::CounterScope c(npc.data(),5);Candidate(out);Check(defense27::count==0);}Check(calls==1);
    Reset();{defense27::CounterScope c(npc.data(),6);nullSelection=true;Candidate(out);auto r=Pop();Check(!r.action&&!r.typeKnown);}
    (void)Pop();
    Reset();{defense27::CounterScope c(npc.data(),6);callback=[](){Evaluate();Evaluate();};Candidate(out);auto r=Pop();Check(r.ambiguous&&r.evaluations==2&&r.predictedPerfect==-1);}(void)Pop();
    Reset();{defense27::CounterScope c(npc.data(),6);keyKnown=false;Candidate(out);Check(!Pop().keyKnown);}(void)Pop();
    Reset();{defense27::CounterScope c(npc.data(),6);Candidate(out);(void)Pop();void* mismatch=otherAction.data();regions.emplace_back(reinterpret_cast<std::uintptr_t>(&mismatch),sizeof(mismatch));
        defense27::DispatchScope d(npc.data(),&mismatch,g_base+0x1483924);d.Finish(false);auto r=Pop();Check(r.ambiguous&&!r.keyKnown&&!r.candidate&&!r.dispatchReturned);}(void)Pop();
    Reset();{defense27::CounterScope c(npc.data(),6);callback=[](){auto* old=defense27::candidateFrame;
        {defense27::CounterScope nested(henry.data(),6);Check(!defense27::counterFrame);}
        Check(defense27::candidateFrame==old);Evaluate();};Candidate(out);Check(Pop().evaluationOk==1);}(void)Pop();
    Reset();{defense27::CounterScope c(npc.data(),6);callback=[](){recording=false;};const auto before=reads;Candidate(out);Check(defense27::count==0&&reads>before);}
    Check(Pop().candidates==1); // copied counter summary needs no native reads
    Reset();{defense27::CounterScope c(npc.data(),6);defense27::enabled=false;const auto before=reads;Candidate(out);Check(reads==before&&defense27::count==0);}(void)Pop();
    Reset();{defense27::CounterScope c(npc.data(),6);callback=[](){keyKnown=false;Evaluate();};Candidate(out);Check(Pop().keyKnown);}(void)Pop();
    Reset();{defense27::CounterScope c(npc.data(),6);callback=[](){defense27::CounterScope nested(henry.data(),6);Evaluate();};Candidate(out);Check(Pop().evaluations==0);}(void)Pop();
    Reset();{defense27::CounterScope c(npc.data(),6);for(unsigned i=0;i<5;++i){Candidate(out);(void)Pop();}
        regions.emplace_back(reinterpret_cast<std::uintptr_t>(&out),sizeof(out));defense27::DispatchScope d(npc.data(),&out,g_base+0x1483924);d.Finish(true);
        auto r=Pop();Check(r.ambiguous&&!r.candidate&&!r.keyKnown);}(void)Pop();
    Reset();{defense27::CounterScope c(npc.data(),6);Candidate(out);(void)Pop();regions.emplace_back(reinterpret_cast<std::uintptr_t>(&out),sizeof(out));
        defense27::DispatchScope d(npc.data(),&out,g_base+0x1483924);d.Finish(false);Check(!Pop().dispatchReturned);}(void)Pop();
    Reset();{defense27::CounterScope c(npc.data(),6);Put(action.data(),0,g_base+123);Candidate(out);auto r=Pop();Check(!r.typeKnown&&!r.managerKnown&&!r.row);}(void)Pop();
    Reset();{defense27::CounterScope c(npc.data(),6);Put(action.data(),0x40,reinterpret_cast<void*>(789));Candidate(out);auto r=Pop();Check(r.typeKnown&&!r.managerKnown);}(void)Pop();
    Reset();{defense27::CounterScope c(npc.data(),6);for(unsigned i=0;i<70;++i)Candidate(out);Check(defense27::count==64&&defense27::dropped.load()>0);}
    Reset();{defense27::CounterScope c(npc.data(),6);wrongHolder=true;Candidate(out);auto r=Pop();Check(r.ambiguous&&!r.keyKnown&&!r.action);}(void)Pop();
    Reset();{defense27::CounterScope c(npc.data(),6);callback=[](){diagnostics::capture168::gate.session[0].fetch_add(1);};Candidate(out);auto r=Pop();Check(r.ambiguous&&!r.keyKnown&&!r.action);}(void)Pop();
    for(unsigned changed=0;changed<2;++changed){Reset();{
        defense27::CounterScope c(npc.data(),6);unsigned afterInvalidation=0;
        callback=[&](){
            if(changed==0)defense27::enabled=false;
            else diagnostics::capture168::gate.session[0].fetch_add(1);
            afterInvalidation=reads;Evaluate();Check(reads==afterInvalidation);
        };
        Candidate(out);auto r=Pop();Check(reads==afterInvalidation&&r.ambiguous&&!r.keyKnown&&!r.action);
    }(void)Pop();}
    for(unsigned changed=0;changed<3;++changed){Reset();{
        defense27::CounterScope c(npc.data(),6);Candidate(out);(void)Pop();
        regions.emplace_back(reinterpret_cast<std::uintptr_t>(&out),sizeof(out));
        defense27::DispatchScope d(npc.data(),&out,g_base+0x1483924);
        if(changed==0)recording=false;
        if(changed==1)diagnostics::capture168::gate.session[0].fetch_add(1);
        if(changed==2)defense27::enabled=false;
        const auto before=reads;d.Finish(true);auto r=Pop();
        Check(reads==before&&r.dispatchReturned&&r.ambiguous&&!r.keyKnown&&!r.candidate);
    }(void)Pop();}
#if defined(_MSC_VER)
    Reset();nativeFault=true;Check(ProbeSeh());Check(!defense27::candidateFrame&&!defense27::counterFrame);Check(Pop().ambiguous);
#endif
    Reset();out=nullptr;regions.emplace_back(reinterpret_cast<std::uintptr_t>(&out),sizeof(out));
    Check(defense27::CounterInvoke(CounterBody,npc.data(),&out,6,1,false,2)==&out);(void)Pop();Check(Pop().candidates==1);
    defense27::head=defense27::count=0;defense27::Drain();
    {using namespace defense27;candidateOriginal=Original;candidateHook.emplace();allowDisable=false;enabled=true;
     Uninstall();Check(!enabled&&candidateHook.has_value()&&!retired&&candidateOriginal==Original&&releaseCalls==0);
     allowDisable=true;Uninstall();Check(!candidateHook.has_value()&&retired&&candidateOriginal==Original&&releaseCalls==1&&freed==0);
     Uninstall();Check(releaseCalls==1&&candidateOriginal==Original);}
    std::printf("native_defense_receipt27: %u checks PASS\n",checks);
}
