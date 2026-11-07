// Executes the actual read-only callbacks on synthetic retail memory/registers.
// Native code and the detour backend are substitutes, not live game validation.
#include <array>
#include <atomic>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <optional>
#include <string>
#include <vector>
#include "../src/diagnostics/reload_lifecycle178r36c2.h"
static int checks=0;
#define CHECK(x) do{++checks;if(!(x)){std::fprintf(stderr,"FAIL %d: %s\n",__LINE__,#x);std::exit(1);}}while(false)
namespace DetourModKit {struct Address {std::uintptr_t value;};namespace hook {
enum class Gpr {Rax,Rbx,Rcx,Rdx,Rsi,Rdi,Rbp,R8,R9,R15};
struct MidContext {std::array<std::uintptr_t,10> regs{};std::uintptr_t sp=0,ip=0;};
inline std::uintptr_t& instruction_pointer(MidContext& c){return c.ip;}
inline std::uintptr_t gpr(MidContext& c,Gpr r){return c.regs[unsigned(r)];}
inline std::uintptr_t stack_pointer(MidContext& c){return c.sp;}
struct Options {bool fail_if_already_hooked=false;};
struct Target {Address address;};struct MidRequest {std::string name;Target target;Options options;};
inline int installs=0,enables=0,disables=0,failAt=-1;
struct Hook {bool enable(){++enables;return true;}bool disable(){++disables;return true;}};
template<class F> std::optional<Hook> mid_at(MidRequest,F){if(++installs==failAt)return {};return Hook{};}
}}
static constexpr std::uintptr_t base=0x10000000;
static std::uintptr_t WhBase(){return base;}
static bool recording=true;
static std::uint64_t tick=1000;
static std::uint64_t GetTickCount64(){return tick;}
static bool ReloadDiagnostics175(){return recording;}
namespace retail_build112 {inline bool correct=true;inline bool Ready(std::uintptr_t){return correct;}inline bool Verify(std::uintptr_t){return correct;}}
struct ReadScope101R2 {};
static std::atomic<void*> g_last_player_entity{nullptr};
namespace diagnostic_trace {inline std::vector<std::string> logs;inline void Write(const char* m){logs.emplace_back(m);}}
struct Region {std::uintptr_t at;unsigned char* data;std::size_t size;};
static std::vector<Region> regions;
static int reads=0;
template<class T> static bool Read(const void* p,T& value) noexcept {
    ++reads;const auto address=reinterpret_cast<std::uintptr_t>(p);
    for(const auto& r:regions)if(address>=r.at&&address-r.at<=r.size&&sizeof(T)<=r.size-(address-r.at)){
        std::memcpy(&value,r.data+address-r.at,sizeof(T));return true;
    }
    return false;
}
static std::uintptr_t manualAction596=0,manualPlayer596=0;
static bool ManualAction596(std::uintptr_t a) noexcept {return retail_build112::correct&&a&&a==manualAction596;}
static bool ManualTransfer596(void* p) noexcept {return retail_build112::correct&&p&&reinterpret_cast<std::uintptr_t>(p)==manualPlayer596;}
static std::atomic<bool> g_manual_bow_fault596{false};
static bool ManualAutomaticFault32() noexcept {return g_manual_bow_fault596.load();}
static bool StagedBowEnabled59() noexcept {return true;}
static bool ManualAutomaticReload19c3(void* p,const char** reason) noexcept {if(reason)*reason="FIXTURE_OWNER";return retail_build112::correct&&p&&reinterpret_cast<std::uintptr_t>(p)==manualPlayer596;}
static bool ManualActionAllocation64(void* p,const char** why) noexcept {if(why)*why="FIXTURE_OWNER";return retail_build112::correct&&reinterpret_cast<std::uintptr_t>(p)==manualAction596;}
static void DeferredReload596(void*,void*) noexcept {}
#include "../src/diagnostics/reload_lifecycle178r36c2.inl"
template<std::size_t N> static std::uintptr_t memory(std::array<unsigned char,N>& b){
    const auto a=reinterpret_cast<std::uintptr_t>(b.data());regions.push_back({a,b.data(),N});return a;
}
template<class T> static void put(std::uintptr_t at,T value){
    for(const auto& r:regions)if(at>=r.at&&at-r.at<=r.size&&sizeof(T)<=r.size-(at-r.at)){
        std::memcpy(r.data+at-r.at,&value,sizeof(T));return;
    }CHECK(false);
}
static ReloadContext36 context(std::uintptr_t sp,std::initializer_list<std::pair<ReloadReg36,std::uintptr_t>> regs){
    ReloadContext36 c{};c.sp=sp;for(auto [r,v]:regs)c.regs[unsigned(r)]=v;return c;
}
static void run(void(*fn)(ReloadContext36&) noexcept,ReloadContext36 c){
    const auto before=c;const auto lines=diagnostic_trace::logs.size();fn(c);
    CHECK(c.sp==before.sp&&c.regs==before.regs&&c.ip==before.ip);CHECK(lines==diagnostic_trace::logs.size());
}
static rl36::Event pop(){rl36::Event e{};CHECK(g_reload_queue36.pop(e));return e;}
static void empty(){rl36::Event e{};CHECK(!g_reload_queue36.pop(e));}
int main(){
    std::array<std::array<unsigned char,16>,kReloadSites36.size()> pins{};
    for(std::size_t i=0;i<kReloadSites36.size();++i){pins[i]=kReloadSites36[i].bytes;regions.push_back({base+kReloadSites36[i].rva,pins[i].data(),16});}
    retail_build112::correct=false;CHECK(!ArmReloadNative36());CHECK(DetourModKit::hook::installs==0);
    retail_build112::correct=true;pins[8][0]^=1;CHECK(!ArmReloadNative36());CHECK(DetourModKit::hook::installs==0);
    ShutdownReloadNative36();pins[8][0]^=1;
    DetourModKit::hook::failAt=4;CHECK(!ArmReloadNative36());CHECK(DetourModKit::hook::disables==3);CHECK(!g_reload_ready36);
    ShutdownReloadNative36();DetourModKit::hook::failAt=-1;
    CHECK(BeginReloadNative36(1));CHECK(DetourModKit::hook::installs==4+int(kReloadSites36.size()));
    std::array<unsigned char,0xA0> action{},player{},entity{},other{};
    std::array<unsigned char,0x100> weapon{},ammo{},missile{},event{};
    std::array<unsigned char,0x800> stack{};
    const auto a=memory(action),p=memory(player),ent=memory(entity),npc=memory(other),w=memory(weapon),am=memory(ammo),m=memory(missile),ev=memory(event),s=memory(stack)+0x500;
    put(a,base+0x3E78CF8);put(p,base+0x3AA4B98);put(p+0x38,ent);put(ent,base+0x4072C78);put(ent+0xC,std::uint32_t(7));g_last_player_entity=reinterpret_cast<void*>(ent);
    put(w,base+0x3A2DA88);put(w+0x30,std::uint64_t(100));put(am,base+0x3A2DA88);put(am+0x30,std::uint64_t(200));put(m,base+0x123456);
    put(ev+0x1C,std::uint32_t(0x34857AFA));
    run(ReloadDispatch36,context(s,{{ReloadReg36::Rcx,a},{ReloadReg36::Rdx,ev}}));
    auto dispatch=pop();CHECK(dispatch.kind==rl36::Kind::Dispatch&&!dispatch.henry&&dispatch.hash==0x34857AFA);
    CHECK(std::string(rl36::event_name(dispatch.hash))=="LoadReachedAmmo");
    put(s,base+0x289CDAC);
    run(ReloadHandBegin36,context(s,{{ReloadReg36::Rcx,a},{ReloadReg36::Rdx,1}}));empty();
    put(s-0x30,base+0x289599D);
    run(ReloadHandCreate36,context(s-0x30,{{ReloadReg36::Rdx,p},{ReloadReg36::R8,a+0x88}}));
    auto start=pop();CHECK(start.henry==7&&start.handRead&&start.handId==0&&start.create==1);
    run(ReloadHandItems36,context(s-0x78,{{ReloadReg36::Rsi,a+0x88},{ReloadReg36::Rdi,p},{ReloadReg36::Rbp,w},{ReloadReg36::R15,am}}));
    auto items=pop();CHECK(items.call==start.call&&items.weaponKey==100&&items.ammoKey==200&&items.weaponRead&&items.ammoRead);
    put(a+0x88,std::uint32_t(42));put(a+0x8C,1);
    run(ReloadHandEnd36,context(s-0x28,{{ReloadReg36::Rdi,a}}));auto end=pop();CHECK(end.call==start.call&&end.handId==42&&end.side==1);
    // LoadReachedWeapon: actual removal helper -> separate nocked object.
    put(s,base+0x289C940);
    run(ReloadHandBegin36,context(s,{{ReloadReg36::Rcx,a},{ReloadReg36::Rdx,0}}));
    put(s-0x30,base+0x28959C7);
    run(ReloadHandRemove36,context(s-0x30,{{ReloadReg36::Rdx,p},{ReloadReg36::R8,a+0x88}}));auto remove=pop();CHECK(remove.handId==42&&remove.create==0);
    put(a+0x88,std::uint32_t(0));run(ReloadHandEnd36,context(s-0x28,{{ReloadReg36::Rdi,a}}));CHECK(pop().handId==0);
    put(s,base+0x289C957);
    run(ReloadTransfer36,context(s,{{ReloadReg36::Rbx,a},{ReloadReg36::Rdx,p}}));auto transfer=pop();CHECK(transfer.call==remove.call&&transfer.henry==7);
    put(s-0x30,base+0x289C9E0);
    run(ReloadAmmoBind36,context(s-0x30,{{ReloadReg36::Rcx,m},{ReloadReg36::Rdx,am},{ReloadReg36::Rdi,w}}));auto bind=pop();CHECK(bind.call==remove.call&&bind.weaponKey==100&&bind.ammoKey==200&&bind.nockRead&&bind.nockId==0);
    put(s-0x60,base+0x2A8A101);
    run(ReloadNockBegin36,context(s-0x60,{{ReloadReg36::Rcx,m},{ReloadReg36::Rdx,am}}));auto nock=pop();CHECK(nock.call==remove.call&&nock.receiver==m);
    put(m+0xDC,std::uint32_t(99));run(ReloadNockEnd36,context(s-0xB8,{{ReloadReg36::Rbx,m}}));auto nocked=pop();CHECK(nocked.call==remove.call&&nocked.nockRead&&nocked.nockId==99&&nocked.handId==0);
    put(a+0x98,std::uint8_t(0));put(a+0x99,std::uint8_t(1));put(a+0x9A,std::uint8_t(1));
    run(ReloadFinalize47C2,context(s,{{ReloadReg36::Rcx,a}}));
    const auto finish=pop();CHECK(finish.kind==rl36::Kind::Finalize&&!finish.henry&&finish.abort==0&&finish.finishNock==1&&finish.finishCock==1);
    put(a,base+0xBAD);run(ReloadFinalize47C2,context(s,{{ReloadReg36::Rcx,a}}));empty();put(a,base+0x3E78CF8);
    rl36::Ledger59c1 ledger;
    CHECK(ledger.accept(dispatch)==nullptr);
    CHECK(ledger.accept(remove));CHECK(ledger.accept(transfer));CHECK(ledger.accept(bind));
    CHECK(ledger.accept(nock));auto joined=ledger.accept(nocked);
    CHECK(joined&&joined->handBefore==42&&joined->nockAfter==99&&joined->ammo==200&&joined->weapon==100);
    auto conflict=nocked;conflict.ammoKey=201;CHECK(!ledger.accept(conflict)&&joined->identityConflict);
    auto separate=nocked;separate.call+=100;CHECK(ledger.accept(separate)!=joined);
    separate=nocked;++separate.session;CHECK(ledger.accept(separate)!=joined);
    // Wider action coverage is diagnostic only. Reused action pointers need a
    // new constructor lifetime; unknown dispatch cannot invent Henry ownership.
    rl36::ActionCoverage59c10r3 coverage;
    CHECK(!coverage.accept(dispatch));
    auto construct=dispatch;construct.kind=rl36::Kind::Construct;
    auto row=coverage.accept(construct);CHECK(row&&row->constructed&&!row->henry);
    CHECK(coverage.accept(items)==row&&row->weapon==100&&row->henry==7);
    auto tool=dispatch;tool.hash=0xAFE0B3FF;tool.lever=0xFEED;
    CHECK(coverage.accept(tool)==row&&row->dispatchMask==8&&row->lever==0xFEED);
    CHECK(coverage.accept(nocked)==row&&row->nockRead&&row->nock==99);
    auto destroy=dispatch;destroy.kind=rl36::Kind::DestroyBegin;
    CHECK(coverage.accept(destroy)==row&&row->ended);
    CHECK(!coverage.accept(dispatch));
    auto again=coverage.accept(construct);CHECK(again&&again!=row&&!again->henry&&again->serial!=row->serial);
    CHECK(coverage.accept(items)==again);auto wrongWeapon=items;wrongWeapon.weaponKey=999;
    CHECK(coverage.accept(wrongWeapon)==again&&again->identityConflict&&again->ended);
    rl36::ActionCoverage59c10r3 bounded;
    for(unsigned i=0;i<bounded.rows.size();++i){auto c=construct;c.action+=i;CHECK(bounded.accept(c));}
    construct.action+=100;CHECK(!bounded.accept(construct)&&bounded.overflow==1);
    // NPC and wrong caller cannot acquire a Henry call; rejected scopes do not
    // inherit the last successful reload at the same stack address.
    put(npc,base+0x3AA4B98);put(npc+0x38,npc);
    put(s,base+0x289C940);run(ReloadHandBegin36,context(s,{{ReloadReg36::Rcx,a},{ReloadReg36::Rdx,0}}));
    put(s-0x30,base+0x28959C7);run(ReloadHandRemove36,context(s-0x30,{{ReloadReg36::Rdx,npc},{ReloadReg36::R8,a+0x88}}));
    run(ReloadHandEnd36,context(s-0x28,{{ReloadReg36::Rdi,a}}));empty();
    run(ReloadNockEnd36,context(s-0xB8,{{ReloadReg36::Rbx,m}}));empty();
    put(s-0x30,base+0xBAD);run(ReloadHandRemove36,context(s-0x30,{{ReloadReg36::Rdx,p},{ReloadReg36::R8,a+0x88}}));empty();
    // Old capture generation, invalid fingerprint, OFF all fail closed.
    put(s,base+0x289C940);run(ReloadHandBegin36,context(s,{{ReloadReg36::Rcx,a},{ReloadReg36::Rdx,0}}));
    g_reload_generation36=2;put(s-0x30,base+0x28959C7);run(ReloadHandRemove36,context(s-0x30,{{ReloadReg36::Rdx,p},{ReloadReg36::R8,a+0x88}}));empty();
    recording=false;const int beforeReads=reads;run(ReloadDispatch36,context(s,{{ReloadReg36::Rcx,a},{ReloadReg36::Rdx,ev}}));CHECK(reads==beforeReads);empty();recording=true;
    retail_build112::correct=false;run(ReloadDispatch36,context(s,{{ReloadReg36::Rcx,a},{ReloadReg36::Rdx,ev}}));CHECK(reads==beforeReads);retail_build112::correct=true;
    // Unreadable is explicitly different from a valid zero visual ID.
    rl36::Event bad{};bad.receiver=1;ReloadNockFields36(bad);CHECK(!bad.nockRead);
    rl36::Queue<3> q;CHECK(q.push(start)&&q.push(end));CHECK(!q.push(end)&&q.dropped==1);rl36::Event out{};CHECK(q.pop(out)&&out.call==start.call);CHECK(q.pop(out));CHECK(!q.pop(out));
    q.lock.test_and_set();CHECK(!q.push(start)&&q.dropped==2);q.lock.clear();
    rl36::Scopes<2> scopes;start.session=2;CHECK(scopes.put(1,start)&&scopes.put(2,start));CHECK(!scopes.put(3,start)&&scopes.overflow==1);CHECK(!scopes.find(1,3));scopes.erase(1);CHECK(scopes.put(3,start));
    g_reload_events36=4096;ReloadEmit36(start);CHECK(g_reload_budget36==1);empty();
    EndReloadNative36("TEST");CHECK(!g_reload_enabled36);
    // Native stage gates survive recorder expiry. These mocks represent the
    // separate exact-owner bridge; all other registers/stack remain unchanged.
    manualAction596=a;manualPlayer596=p;recording=false;
    auto gate=context(s,{{ReloadReg36::Rcx,a},{ReloadReg36::Rdx,1}});const auto beforeGate=gate;
    ReloadHandBegin36(gate);CHECK(gate.ip==reinterpret_cast<std::uintptr_t>(&DeferredReload596));
    CHECK(gate.regs==beforeGate.regs&&gate.sp==beforeGate.sp);
    run(ReloadHandBegin36,context(s,{{ReloadReg36::Rcx,a},{ReloadReg36::Rdx,0}})); // cleanup never suppressed
    run(ReloadHandBegin36,context(s,{{ReloadReg36::Rcx,npc},{ReloadReg36::Rdx,1}}));
    put(s,base+0x289C957);gate=context(s,{{ReloadReg36::Rdx,p}});ReloadTransfer36(gate);
    CHECK(gate.ip==reinterpret_cast<std::uintptr_t>(&DeferredReload596));
    put(s,base+0xBAD);run(ReloadTransfer36,context(s,{{ReloadReg36::Rdx,p}}));
    retail_build112::correct=false;run(ReloadHandBegin36,beforeGate);retail_build112::correct=true;
    ShutdownReloadNative36();CHECK(!g_reload_ready36);run(ReloadHandBegin36,beforeGate);
    // C37: caller-context pins gate ONLY the expansion/origin join; the 15
    // hook pins and C32 2AAA012 path arm independently of them.
    CHECK(!g_reload_callers37);
    static std::vector<std::vector<unsigned char>> callerPins;callerPins.clear();
    for(const auto& p:kReloadCallerPins37){std::vector<unsigned char> b;for(std::size_t j=0;p.hex[2*j];++j)b.push_back((unsigned char)std::stoul(std::string(p.hex+2*j,2),nullptr,16));callerPins.push_back(b);}
    for(std::size_t i=0;i<callerPins.size();++i)regions.push_back({base+kReloadCallerPins37[i].rva,callerPins[i].data(),callerPins[i].size()});
    static std::array<std::array<unsigned char,8>,2> slots{};
    for(std::size_t i=0;i<2;++i){const std::uintptr_t v=base+kReloadSlotPins37[i].target;std::memcpy(slots[i].data(),&v,8);regions.push_back({base+kReloadSlotPins37[i].slot,slots[i].data(),8});}
    auto pinAt40=[&](std::uint32_t rva)->const std::vector<unsigned char>& {for(std::size_t i=0;i<callerPins.size();++i)if(kReloadCallerPins37[i].rva==rva)return callerPins[i];CHECK(false);return callerPins[0];};
    const auto& primary40=pinAt40(0x1F24800);const auto& pending40=pinAt40(0x222CB16);
    CHECK(callerPins.size()==14&&primary40.size()==69&&primary40[0x31]==0xE8); // 1F24831 call 2AAA4C0
    CHECK(pending40.size()==48&&pending40[7]==0&&pending40[0x25]==0xE8&&pending40[0x2A]==0x90); // +5C clear, request, unused return
    callerPins[1][3]^=1;CHECK(ArmReloadNative36()&&g_reload_ready36&&!g_reload_callers37); // pin mismatch: hooks still arm, expansion off
    ShutdownReloadNative36();callerPins[1][3]^=1;
    slots[0][0]^=1;CHECK(ArmReloadNative36()&&!g_reload_callers37);ShutdownReloadNative36();slots[0][0]^=1;
    recording=true;CHECK(BeginReloadNative36(3)&&g_reload_callers37);
    // F11 rung-1 join: construct at 2886BCE -> 2886B24 ret 2896192 at sp+28h,
    // 2896174 ret 1846A54 at sp+58h, 18468BC entry sp+88h holds the original caller.
    const auto cs=s-0x300;put(cs+0x28,base+0x2896192);put(cs+0x58,base+0x1846A54);put(cs+0x88,base+0x1F24836);
    run(ReloadConstruct36,context(cs,{{ReloadReg36::Rbx,a}}));
    auto born=pop();CHECK(born.kind==rl36::Kind::Construct&&born.origin==0x1F24836&&born.originStack==cs+0x88&&!born.henry);
    CHECK(std::string(rl36::origin_name(born.origin))=="BOW_PRIMARY_PRESS");
    put(cs+0x88,base+0x33A33A4);run(ReloadConstruct36,context(cs,{{ReloadReg36::Rbx,a}}));
    CHECK(std::string(rl36::origin_name(pop().origin))=="ATTEMPT_COUNTER"); // excluded callers still name themselves when native allocates
    put(cs+0x58,base+0x1846A55);run(ReloadConstruct36,context(cs,{{ReloadReg36::Rbx,a}}));
    auto loose=pop();CHECK(loose.origin==0&&loose.originStack==0&&std::string(rl36::origin_name(loose.origin))=="UNJOINED");
    put(cs+0x58,base+0x1846A54);put(cs+0x28,base+0x2896197);run(ReloadConstruct36,context(cs,{{ReloadReg36::Rbx,a}}));CHECK(pop().origin==0);
    put(cs+0x28,base+0x2896192);put(cs+0x88,std::uintptr_t(0x1234));run(ReloadConstruct36,context(cs,{{ReloadReg36::Rbx,a}}));CHECK(pop().origin==0); // foreign return stays unjoined
    put(cs+0x88,base+0x2AAA012);g_reload_callers37=false;run(ReloadConstruct36,context(cs,{{ReloadReg36::Rbx,a}}));CHECK(pop().origin==0);g_reload_callers37=true;
    run(ReloadConstruct36,context(cs,{{ReloadReg36::Rbx,a}}));auto retry=pop();CHECK(retry.origin==0x2AAA012);
    // one action identity: constructor origin survives hand/nock/finalize/destroy rows
    rl36::ActionCoverage59c10r3 joined37;auto row37=joined37.accept(born);CHECK(row37&&row37->origin==0x1F24836&&row37->originStack==cs+0x88);
    auto items37=items,nocked37=nocked;items37.session=nocked37.session=born.session;
    CHECK(joined37.accept(items37)==row37&&joined37.accept(nocked37)==row37&&row37->henry==7&&row37->nock==99&&row37->origin==0x1F24836);
    auto gone=born;gone.kind=rl36::Kind::DestroyBegin;gone.origin=0;CHECK(joined37.accept(gone)==row37&&row37->ended&&row37->origin==0x1F24836);
    auto reborn=joined37.accept(retry);CHECK(reborn&&reborn!=row37&&reborn->origin==0x2AAA012);
    auto late=items37;late.origin=0x1F2469E;late.action=a+0x10;CHECK(joined37.accept(late)&&joined37.rows[2].origin==0); // non-constructor cannot claim an origin
    recording=false;const int offReads=reads;run(ReloadConstruct36,context(cs,{{ReloadReg36::Rbx,a}}));CHECK(reads==offReads);empty();recording=true;
    put(cs+0x88,base+0x1F24836);run(ReloadConstruct36,context(cs,{{ReloadReg36::Rbx,a}}));ReloadEmit36(items37);ReloadEmit36(nocked37);
    EndReloadNative36("TEST37");
    bool sawOrigin=false,sawStage=false;for(const auto& l:diagnostic_trace::logs){
        if(l.find("ACTION_COVERAGE")!=std::string::npos&&l.find("origin=1F24836 originKind=BOW_PRIMARY_PRESS")!=std::string::npos&&l.find("henry=7")!=std::string::npos)sawOrigin=true;
        if(l.find("stage=ACTION_CONSTRUCT")!=std::string::npos&&l.find("originKind=BOW_PRIMARY_PRESS")!=std::string::npos)sawStage=true;}
    CHECK(sawOrigin&&sawStage);
    ShutdownReloadNative36();CHECK(!g_reload_callers37);
    std::printf("PASS: %d reload callback/guard/queue checks; no native code executed.\n",checks);
}
