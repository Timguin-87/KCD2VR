// Execute the actual D62C/D63 Queue and Update adapter with bounded fake native
// storage and the production membership walk. Rows replay the uploaded run's
// victim-before-attacker queue ordering, entities and coarse tick schedule.
#include "subsystems/masterstrike_combos/ms_reaction_owner_d62c.h"
#include "subsystems/masterstrike_combos/paired_signals_d62c.h"
#include "subsystems/masterstrike_combos/native_kill_anim_nf.h"
#include "vrik/semi_smith_native46.h"
#include <atomic>
#include <cstdio>
#include <vector>
#include <string>
#include <cstring>
#include <limits>
static std::uint64_t nowD63=66290609;
static bool captureD63=true;
static unsigned readsD63=0,writesD63=0,checksD63=0,failuresD63=0;
static std::vector<std::pair<std::uintptr_t,std::size_t>> regionsD63;
static std::vector<std::string> traceD63;
static std::uint64_t GetTickCount64(){return nowD63;}
namespace kcd2vr::diagnostics::capture168 {
enum class Domain{Combat};
static bool active(Domain,std::uint64_t){return captureD63;}
}
namespace diagnostics=kcd2vr::diagnostics;
namespace diagnostic_trace {static void Write(const char* p){traceD63.emplace_back(p);}}
static bool mappedD63(const void* p,std::size_t n){
    const auto a=reinterpret_cast<std::uintptr_t>(p);
    for(const auto& r:regionsD63)if(a>=r.first&&n<=r.second&&a-r.first<=r.second-n)return true;
    return false;
}
static bool SehCopy(const void* p,void* dst,std::size_t n){++readsD63;if(!mappedD63(p,n))return false;std::memcpy(dst,p,n);return true;}
template<class T>static T Rd(const void* p,std::ptrdiff_t offset,T fallback){T out{};
    return p&&SehCopy(static_cast<const unsigned char*>(p)+offset,&out,sizeof(out))?out:fallback;}
template<class T>static bool Wr(void* p,std::ptrdiff_t offset,T value){
    auto* to=static_cast<unsigned char*>(p)+offset;if(!mappedD63(to,sizeof(value)))return false;
    ++writesD63;std::memcpy(to,&value,sizeof(value));return true;
}
constexpr std::uintptr_t kCtrlVft=0x40519E8ull;
constexpr std::ptrdiff_t kCtrlContext=0x20,kCtrlEntityId=0x30,kActFragId=0x38,kActTags=0x3C,
    kActOption=0x50,kActToken=0x54,kActSpeedBias=0x5C;
static std::atomic<std::uintptr_t> g_base{0x10000000};
#ifndef PRODUCTION_PAIRED_ADAPTER_D63
#define PRODUCTION_PAIRED_ADAPTER_D63 "../src/subsystems/masterstrike_combos/ms_reaction_owner_d62c.inl"
#endif
#include PRODUCTION_PAIRED_ADAPTER_D63
struct Memory {
    std::vector<unsigned char> bytes;
    explicit Memory(std::size_t n):bytes(n){regionsD63.emplace_back(reinterpret_cast<std::uintptr_t>(bytes.data()),n);}
    unsigned char* p(){return bytes.data();}
    template<class T>void put(std::size_t at,T value){std::memcpy(p()+at,&value,sizeof(value));}
};
struct NativeActor {
    Memory ctrl{0xB0},context{16},definition{0x40},fragmentList{16},entries{4+950*0x20},scope{0x168},action{0x100};
    explicit NativeActor(std::uint32_t entity,int fragment){
        ctrl.put(0,g_base.load()+kCtrlVft);ctrl.put(0x20,context.p());ctrl.put(0x30,entity);
        context.put(0,definition.p());definition.put(0x18,fragmentList.p());fragmentList.put(8,entries.p()+4);
        entries.put(0,std::int32_t(950));
        entries.put(4+921*0x20+0x10,std::uint32_t(0x17106D4F));
        entries.put(4+922*0x20+0x10,std::uint32_t(0xBF91494F));
        entries.put(4+923*0x20+0x10,std::uint32_t(0xF3D43F01));
        entries.put(4+924*0x20+0x10,std::uint32_t(0xD62EA2F9));
        action.put(0x38,fragment);action.put(0x50,-2);action.put(0x5C,1.f);
    }
    void root(){ctrl.put(0x34,1u);ctrl.put(0x38,scope.p());scope.put(0x148,action.p());action.put(0x30,scope.p());}
    float speed(){return Rd<float>(action.p(),0x5C,-1.f);}
};
static void ck(bool b,const char* m){++checksD63;if(!b){++failuresD63;std::printf("FAIL %s\n",m);}}
static void reset(){g_ms_table_d62c={};g_ms_live_d62c=false;g_ms_lock_d62c.clear();g_ms_ids_next_d62c=0;
    for(auto& x:g_ms_ids_d62c)x={};writesD63=readsD63=0;traceD63.clear();captureD63=true;
    sig62c::enabled=true;sig62c::speed=2.f;sig62c::live=false;kcd2vr::native_kill_nf::SetEnabled(true);}
static void runRecorded(bool kill){
    reset();NativeActor henry(30583,kill?921:922),victim(kill?45102:45105,kill?923:924);
    nowD63=kill?66290609:66312968;
    MsReactionQueueD62C(victim.ctrl.p(),victim.action.p(),false);
    MsReactionQueueD62C(henry.ctrl.p(),henry.action.p(),true);
    ck(g_ms_table_d62c.next==3,"both recorded native rows observed");
    ck(g_ms_table_d62c.rows[0].pair==0&&g_ms_table_d62c.rows[1].pair==0,"reverse queue cannot invent native pair proof");
    henry.root();victim.root();nowD63+=16;
    MsReactionUpdateD62C(henry.ctrl.p());MsReactionUpdateD62C(victim.ctrl.p());
    // The original observer would borrow this contact even with pair=0.
    sig62c::NoteVrContact(kill?45102:45105,nowD63+50);
    for(auto elapsed:{100u,203u,375u,703u,1100u}){
        nowD63=(kill?66290609:66312968)+elapsed;
        MsReactionUpdateD62C(henry.ctrl.p());MsReactionUpdateD62C(victim.ctrl.p());
        ck(henry.speed()==1.f&&victim.speed()==1.f,"saved ON never accelerates either unverified actor");
    }
    ck(writesD63==0,"recorded queue/update replay performs zero native speed writes");
    ck(!sig62c::live.load(),"unverified observer does not authorize contact/hit signal collection");
}
int main(){
    runRecorded(true);runRecorded(false);
    reset();NativeActor henry(30583,922),other(45105,924);
    nowD63=70000000;MsReactionQueueD62C(henry.ctrl.p(),henry.action.p(),true);nowD63+=10;
    MsReactionQueueD62C(other.ctrl.p(),other.action.p(),false);
    ck(!g_ms_table_d62c.rows[0].pair&&!g_ms_table_d62c.rows[1].pair,"forward queue proximity cannot bind an unrelated victim");
    henry.root();other.root();nowD63+=20;MsReactionUpdateD62C(henry.ctrl.p());MsReactionUpdateD62C(other.ctrl.p());
    nowD63+=300;MsReactionUpdateD62C(henry.ctrl.p());MsReactionUpdateD62C(other.ctrl.p());
    ck(writesD63==0&&henry.speed()==1.f&&other.speed()==1.f,"unverified forward order is also read-only");
    reset();NativeActor victimHenry(30583,924),attacker(45102,922);
    MsReactionQueueD62C(victimHenry.ctrl.p(),victimHenry.action.p(),true);
    MsReactionQueueD62C(attacker.ctrl.p(),attacker.action.p(),false);
    victimHenry.root();attacker.root();nowD63+=20;
    MsReactionUpdateD62C(victimHenry.ctrl.p());MsReactionUpdateD62C(attacker.ctrl.p());
    nowD63+=400;MsReactionUpdateD62C(victimHenry.ctrl.p());MsReactionUpdateD62C(attacker.ctrl.p());
    ck(writesD63==0&&victimHenry.speed()==1.f&&attacker.speed()==1.f,"NPC attacks on Henry remain unchanged");
    reset();captureD63=false;const auto before=readsD63;
    MsReactionQueueD62C(henry.ctrl.p(),henry.action.p(),true);
    ck(readsD63==before&&g_ms_table_d62c.next==1,"capture OFF causes no paired observer native reads");
    reset();nowD63+=100;MsReactionQueueD62C(henry.ctrl.p(),henry.action.p(),true);
    captureD63=false;const auto activeReads=readsD63;MsReactionUpdateD62C(henry.ctrl.p());
    ck(readsD63==activeReads&&!g_ms_live_d62c.load(),"capture expiry retires existing unleased observer without native reads");
    reset();kcd2vr::native_kill_nf::SetEnabled(false);nowD63+=100;
    MsReactionQueueD62C(henry.ctrl.p(),henry.action.p(),true);MsReactionQueueD62C(other.ctrl.p(),other.action.p(),false);
    MsReactionUpdateD62C(henry.ctrl.p());MsReactionUpdateD62C(other.ctrl.p());nowD63+=300;
    MsReactionUpdateD62C(henry.ctrl.p());MsReactionUpdateD62C(other.ctrl.p());
    ck(writesD63==0,"native animation OFF cannot be overridden by paired timing saved ON");
    std::printf("D63 actual paired runtime: %u/%u; native membership/storage modeled; game NOT_RUN\n",checksD63-failuresD63,checksD63);
    return failuresD63?1:0;
}
