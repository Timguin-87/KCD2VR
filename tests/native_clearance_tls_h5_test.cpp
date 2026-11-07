// Actual clearance Query and actual T4 weapon lifecycle; only native/OS leaves
// are substituted by the existing native_weapon_body_t4 host fixture.
#define main native_weapon_body_t4_legacy_main_h5
#pragma warning(push)
#pragma warning(disable:4715) // renamed legacy main is included, never called
#include "native_weapon_body_t4_test.cpp"
#pragma warning(pop)
#undef main
#include <limits>

namespace {
unsigned checksH5=0;
void CheckH5(bool value,const char* label){++checksH5;if(!value){
    std::printf("FAIL %u %s\n",checksH5,label);std::exit(1);}}
}
int main(){
    using namespace fixture;
    namespace c=kcd2vr::native_weapon_clearance_t4;
    CheckH5(w::SetSourcePredicate(&Source),"actual source predicate");
    CheckH5(w::Install(),"actual public backend Install");
    const c::Capsule volume{{1,2,3},{0,1,0},.06f,.25f};
    for(int index=0;index!=4;++index){
        c::hostCallerIndexH5=index;const auto before=queries;
        CheckH5(c::Query(p::Access::Ctx(),volume,bodyPointer())==c::Result::Clear,"supported caller Clear");
        CheckH5(queries==before+1,"supported caller enters native query once");
    }
    for(int index:{4,-1,5,std::numeric_limits<int>::min(),std::numeric_limits<int>::max()}){
        c::hostCallerIndexH5=index;const auto before=queries;
        CheckH5(c::Query(p::Access::Ctx(),volume,bodyPointer())==c::Result::CallerUnavailableH5,"unsupported caller refused");
        CheckH5(queries==before,"unsupported caller never enters native query");
    }
    c::hostCallerIndexH5=0;
    auto before=queries;current=false;
    CheckH5(c::Query(p::Access::Ctx(),volume,bodyPointer())==c::Result::Unknown,"stale Step refused");current=true;
    CheckH5(queries==before,"stale Step has no native call");
    auto& wrong=p::Access::Ctx();p::Access::PhaseOf(p::Phase::WorldReset);
    CheckH5(c::Query(wrong,volume,bodyPointer())==c::Result::Unknown,"Reset not admitted");
    CheckH5(queries==before,"Reset has no native call");
    auto invalid=volume;invalid.axis={};
    CheckH5(c::Query(p::Access::Ctx(),invalid,bodyPointer())==c::Result::Unknown,"invalid shape refused");
    CheckH5(c::Query(p::Access::Ctx(),volume,nullptr)==c::Result::Unknown,"absent own body refused");
    CheckH5(queries==before,"invalid ownership never queries");
    clearResult=.1f;CheckH5(c::Query(p::Access::Ctx(),volume,bodyPointer())==c::Result::Occupied,"actual positive result remains Occupied");
    clearResult=-1;CheckH5(c::Query(p::Access::Ctx(),volume,bodyPointer())==c::Result::Unknown,"actual negative result remains Unknown");
    clearResult=std::numeric_limits<float>::quiet_NaN();
    CheckH5(c::Query(p::Access::Ctx(),volume,bodyPointer())==c::Result::Unknown,"nonfinite result remains Unknown");
    clearResult=0;
    // Follow the actual backend's three-phase creation/configuration/admission.
    // Default4 may never call the primitive query or enable an owned collider.
    auto in=Input();w::SetEnabled(true);Fresh(in);Tick();
    CheckH5(alive&&pending&&!collision,"actual body created disabled");pending=false;Advance(in);
    CheckH5(geometry&&!collision,"actual geometry configured disabled");
    c::hostCallerIndexH5=4;before=queries;const auto enableBefore=enables;
    Advance(in);w::Achieved out{};CheckH5(w::Read(out),"actual refusal receipt published");
    CheckH5(queries==before&&enables==enableBefore&&!collision,"default4 cannot query or enable");
    CheckH5(out.state==w::State::Retiring&&out.reason==w::Reason::RetirePending,"existing fail-closed retirement retained");
    Advance(in);CheckH5(w::Read(out)&&out.state==w::State::Waiting&&out.reason==w::Reason::QueryCallerUnavailableH5,
        "terminal existing receipt identifies native caller refusal");
    CheckH5(std::string(w::ReasonName(out.reason))=="QUERY_CALLER_UNAVAILABLE_H5","existing F9 reason formatter identifies refusal");
    Cleanup();CheckH5(!alive&&!collision,"refused body follows normal retirement");
    // Return to a proven safe caller: normal F9 backend remains reachable.
    c::hostCallerIndexH5=0;Restart();in=Input();const auto enabled=enables;Active(in);
    CheckH5(collision&&enables==enabled+1,"safe caller enables exactly one actual collider");
    Cleanup();CheckH5(!alive&&!collision,"successful body retires normally");
    std::printf("native_clearance_tls_h5 %u/%u PASS (actual Query/lifecycle; native leaves explicit)\n",checksH5,checksH5);
}
