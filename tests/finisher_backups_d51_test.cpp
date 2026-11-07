#include "combat_backups_d51_host.h"
#include "subsystems/masterstrike_combos/physical_finisher_cb1.h"
#include "diagnostics/combo_snapshot.h"
#include "diagnostics/capture168.h"
#include "vrik/visible_failure.h"
#include <cstdio>
#include <string>
#include <vector>
namespace kcd2vr {
namespace diagnostic_trace {inline unsigned writes=0;inline std::vector<std::string> lines;
void Write(const char* s) noexcept {++writes;lines.emplace_back(s);}}
namespace frik_pose_probe {
inline std::uintptr_t expectedEntity=0x40000;
void* LastPlayerEntity() noexcept {return reinterpret_cast<void*>(expectedEntity);}
inline bool pathKnownD62=false;
bool WeaponPathWorldCB4(bool pommel,float* tip,float* was) noexcept {
    if(!pathKnownD62||pommel)return false;
    const float p[3]{.5f,0.f,1.62f},q[3]{.6f,0.f,1.62f};
    std::memcpy(tip,p,sizeof(p));std::memcpy(was,q,sizeof(q));return true;
}
}
std::atomic<std::uintptr_t> g_combo_base6691{0x10000000};
static bool ComboRead6691(std::uintptr_t address,void* out,std::size_t count) noexcept {return hostD51::read(address,out,count);}
#include "finisher_callbacks_d51_production.inc"
}
using namespace kcd2vr;
namespace cb=kcd2vr::finisher_cb1;
namespace hook=DetourModKit::hook;
static constexpr std::uintptr_t manager=0x10000,actor=0x20000,player=0x30000,entity=0x40000,
    state=0x50000,pair=0x60000,recipe=0x70000;
static unsigned checks=0,failures=0,originalCalls=0,nativeOnlyCalls=0;
static int passedZone=-1,depth=0;
static bool returnTechnique=true,disableAtCallback=false,disableDuringRead=false,nestedNone=false,nestedDisabled=false,staleCandidate=false;
static bool outerScopeRestored=false,nestedWasUnfiltered=false;
static bool stopCapture=false,restartCapture=false,returnNullPointer=false,skipNativeCallbackR94=false;
static hook::MidContext lastContext{};
static void check(bool ok,const char* why){++checks;if(!ok){++failures;std::printf("FAIL %s\n",why);}}
static hook::MidContext candidateContext(){hook::MidContext c{};c.ip=0x93999B;
    hook::gpr(c,hook::Gpr::Rdi)=manager;hook::gpr(c,hook::Gpr::R13)=state;hook::gpr(c,hook::Gpr::Rsi)=pair;return c;}
static void offDuringRead(std::uintptr_t address,std::size_t){if(disableDuringRead&&address==pair+8)cb::enabled=false;}
static void** original(void* mgr,void** out,int zone){
    ++originalCalls;passedZone=zone;const auto* outer=g_query_scope_cb1;
    if(!outer)++nativeOnlyCalls;
    if(depth==0&&(nestedNone||nestedDisabled)){
        const auto oldKind=cb::activeRelease;const bool oldEnabled=cb::enabled.load();
        cb::activeRelease=nestedNone?cb::Kind::None:oldKind;if(nestedDisabled)cb::enabled=false;
        ++depth;void* nested=nullptr;FinisherQueryDetourCB1(mgr,&nested,zone);--depth;
        nestedWasUnfiltered=lastContext.ip==0x93999B;
        outerScopeRestored=g_query_scope_cb1==outer;
        cb::activeRelease=oldKind;cb::enabled=oldEnabled;
    }
    if(disableAtCallback)cb::enabled=false;
    if(staleCandidate)hostD51::put(state+8,std::uint32_t(4));
    lastContext=candidateContext();hostD51::afterRead=&offDuringRead;if(!skipNativeCallbackR94)CandidateMidCB1(lastContext);hostD51::afterRead=nullptr;
    if(out)*out=returnTechnique?reinterpret_cast<void*>(0x12345678):nullptr;
    if(stopCapture)diagnostics::capture168::gate.select(diagnostics::capture168::Domain::Combat,false,hostD51::tick);
    if(restartCapture)diagnostics::capture168::gate.select(diagnostics::capture168::Domain::Combat,true,hostD51::tick);
    return returnNullPointer?nullptr:out;
}
static void setup(){
    hostD51::memory.clear();hostD51::reads=0;hostD51::afterRead=nullptr;
    const auto b=g_combo_base6691.load();
    hostD51::zero(manager,0x100);hostD51::zero(actor,0x220);hostD51::zero(player,0x48);hostD51::zero(entity,0x20);
    hostD51::zero(state,40);hostD51::zero(pair,24);hostD51::zero(recipe,0x60);
    hostD51::put(manager,b+0x3B0A9A0);hostD51::put(manager+8,actor);hostD51::put(actor,b+0x3A80610);
    hostD51::put(actor+0x1F8,player);hostD51::put(player,b+0x3AA4B98);hostD51::put(player+0x38,entity);
    hostD51::put(entity,b+0x4072C78);hostD51::put(entity+0xC,std::uint32_t(1));
    hostD51::put(manager+0xC0,diagnostics::combo::Vector{state,state+40,state+40});
    hostD51::put(state+8,std::uint32_t(2));hostD51::put(state+0x10,diagnostics::combo::Vector{pair,pair+24,pair+24});
    hostD51::put(pair,recipe);hostD51::put(pair+8,std::uint32_t(3));
    std::uint8_t guid[16]{};cb::parseGuid(cb::rows[5].technique,guid);hostD51::put(recipe+0x28,guid); // C08 thrust, no height requirement
    g_finisher_ready_cb1=true;g_finisher_original_cb1=&original;g_query_scope_cb1=nullptr;
    cb::nativeValidationTestD61=false;cb::enabled=true;cb::activeRelease=cb::Kind::Cut;cb::activeGestureD60=7001;originalCalls=nativeOnlyCalls=0;depth=0;returnTechnique=true;
    disableAtCallback=disableDuringRead=nestedNone=nestedDisabled=staleCandidate=false;frik_pose_probe::pathKnownD62=false;
    outerScopeRestored=nestedWasUnfiltered=false;diagnostics::capture168::gate.mask=0;
    stopCapture=restartCapture=returnNullPointer=skipNativeCallbackR94=false;diagnostic_trace::writes=0;diagnostic_trace::lines.clear();
}
static void capture(){diagnostics::capture168::gate.select(diagnostics::capture168::Domain::Combat,true,hostD51::tick);}
static bool has(const char* token){for(const auto& line:diagnostic_trace::lines)if(line.find(token)!=std::string::npos)return true;return false;}
int main(){
    setup();void* out=nullptr;auto reader=ComboRead6691;
    const auto snapshot=diagnostics::combo::Capture(reader,diagnostics::combo::Layout{g_combo_base6691.load()},manager,entity);
    check(snapshot.status==diagnostics::combo::Status::Ok&&snapshot.candidatesCopied==1,"actual native snapshot decoder accepts the fixture identity and recipe");
    auto before=hostD51::memory;
    check(FinisherQueryDetourCB1(reinterpret_cast<void*>(manager),&out,2)==&out&&out==reinterpret_cast<void*>(0x12345678)&&originalCalls==1&&passedZone==2,
          "physical mode invokes original once and preserves native pointer and zone");
    check(lastContext.ip==g_combo_base6691+0x939946,"physical cut still rejects a native thrust finisher candidate");
    check(hostD51::memory==before&&g_query_scope_cb1==nullptr,"filter changes only continuation and retires query scope");
    for(bool nativeResult:{false,true}){setup();cb::enabled=false;returnTechnique=nativeResult;
        check(FinisherQueryDetourCB1(reinterpret_cast<void*>(manager),&out,5)==&out&&originalCalls==1&&passedZone==5,
              "directions backup preserves original invocation and requested native direction");
        check(bool(out)==nativeResult&&lastContext.ip==0x93999B&&hostD51::reads==0,
              "directions backup preserves native success/refusal without reading or filtering recipe");}
    setup();disableAtCallback=true;FinisherQueryDetourCB1(reinterpret_cast<void*>(manager),&out,2);
    check(lastContext.ip==0x93999B&&originalCalls==1,"turning physical filter OFF before entered candidate callback revokes rejection");
    setup();disableDuringRead=true;FinisherQueryDetourCB1(reinterpret_cast<void*>(manager),&out,2);
    check(lastContext.ip==0x93999B&&originalCalls==1,"OFF during native candidate reads is rechecked before continuation write");
    for(bool disabled:{false,true}){setup();nestedDisabled=disabled;nestedNone=!disabled;
        FinisherQueryDetourCB1(reinterpret_cast<void*>(manager),&out,1);
        check(originalCalls==2&&nativeOnlyCalls==1&&nestedWasUnfiltered,"nested native-only invocation cannot borrow outer physical query");
        check(outerScopeRestored&&lastContext.ip==g_combo_base6691+0x939946&&g_query_scope_cb1==nullptr,
              "return from nested native-only query restores outer physical scope only until its original returns");}
    setup();staleCandidate=true;FinisherQueryDetourCB1(reinterpret_cast<void*>(manager),&out,2);
    check(lastContext.ip==0x93999B,"changed native candidate cursor remains unfiltered");
    setup();frik_pose_probe::expectedEntity=entity+8;FinisherQueryDetourCB1(reinterpret_cast<void*>(manager),&out,2);
    check(lastContext.ip==0x93999B&&originalCalls==1,"non-Henry identity never inherits physical candidate scope");
    frik_pose_probe::expectedEntity=entity;
    check(diagnostic_trace::writes==0,"disabled F3 emits no candidate or query diagnostics");
    // D60: actual selector callbacks, not a second implementation of policy.
    setup();capture();returnTechnique=false;before=hostD51::memory;
    FinisherQueryDetourCB1(reinterpret_cast<void*>(manager),&out,2);
    check(has("FINISHER_QUERY_D60")&&has("nativeResult=NO_REFERENCE")&&has("gesture=7001"),"native negative result now has exact stack gesture receipt");
    check(has("candidateCalls=1 matched=1 policyPass=0 rejected=1")&&has("WRONG_MOTION"),"summary separates physical rejection from native refusal");
    check(hostD51::memory==before&&originalCalls==1,"F3 query receipt does not change native memory or repeat selector");
    const auto query=diagnostic_trace::lines.empty()?std::string::npos:diagnostic_trace::lines.front().find("query=");
    const auto queryEnd=query==std::string::npos?query:diagnostic_trace::lines.front().find(' ',query);
    const auto queryToken=query==std::string::npos?"MISSING_QUERY":diagnostic_trace::lines.front().substr(query,queryEnd-query);
    check(query!=std::string::npos&&diagnostic_trace::lines.size()==2&&diagnostic_trace::lines.back().find(queryToken)!=std::string::npos,"candidate and return share the same query id");
    setup();capture();cb::activeRelease=cb::Kind::Thrust;FinisherQueryDetourCB1(reinterpret_cast<void*>(manager),&out,2);
    check(has("policyPass=1 rejected=0")&&has("nativeResult=REFERENCE")&&lastContext.ip==0x93999B,"valid motion keeps native selection and distinguishes precontact pass");
    setup();capture();cb::enabled=false;FinisherQueryDetourCB1(reinterpret_cast<void*>(manager),&out,5);
    check(has("FILTER_DISABLED_NATIVE_PASS")&&has("snapshot=NOT_READ")&&hostD51::reads==0&&originalCalls==1,"F3 disabled-filter query adds zero native reads and no new blocker");
    check(has("nativeCandidateCalls=1")&&has("candidateObservation=OBSERVED")&&has("candidateCalls=0"),"native-only callbacks are observed independently of filter scope");
    setup();capture();cb::enabled=false;skipNativeCallbackR94=true;FinisherQueryDetourCB1(reinterpret_cast<void*>(manager),&out,5);
    check(has("nativeCandidateCalls=0")&&has("candidateObservation=NONE_AT_INSTALLED_BOUNDARY"),"installed boundary with no native callback is distinguished from no filter candidate");
    setup();capture();g_finisher_ready_cb1=false;FinisherQueryDetourCB1(reinterpret_cast<void*>(manager),&out,5);
    check(has("candidateObservation=UNOBSERVED_HOOK_UNAVAILABLE"),"missing observation hook never claims no native candidate");
    setup();capture();cb::activeRelease=cb::Kind::None;FinisherQueryDetourCB1(reinterpret_cast<void*>(manager),&out,1);
    check(has("NO_OWNED_RELEASE_NATIVE_PASS")&&has("gesture=0")&&lastContext.ip==0x93999B,"native-only query cannot claim prior physical gesture");
    setup();capture();staleCandidate=true;FinisherQueryDetourCB1(reinterpret_cast<void*>(manager),&out,1);
    check(has("candidateGap=CURSOR_OR_RECIPE_CHANGED")&&has("rejected=0")&&lastContext.ip==0x93999B,"changed native state has a diagnostic gap and stays native");
    setup();capture();frik_pose_probe::expectedEntity=entity+8;FinisherQueryDetourCB1(reinterpret_cast<void*>(manager),&out,2);
    check(has("SNAPSHOT_FAILED_NATIVE_PASS")&&has("snapshot=IDENTITY_MISMATCH")&&lastContext.ip==0x93999B,"wrong actor is reported without rejection");
    frik_pose_probe::expectedEntity=entity;
    setup();capture();returnNullPointer=true;FinisherQueryDetourCB1(reinterpret_cast<void*>(manager),&out,2);
    check(has("nativeResult=NULL_RESULT_POINTER"),"null selector result pointer does not disappear from F3 evidence");
    setup();capture();FinisherQueryDetourCB1(reinterpret_cast<void*>(manager),nullptr,2);
    check(has("NULL_OUTPUT_NOT_INSPECTED")&&hostD51::reads==0&&originalCalls==1,"null output passes through exactly once without observation read");
    for(bool restart:{false,true}){setup();capture();restartCapture=restart;stopCapture=!restart;
        FinisherQueryDetourCB1(reinterpret_cast<void*>(manager),&out,2);
        check(!has("FINISHER_QUERY_D60")&&originalCalls==1,"stopped/replaced F3 lease cannot emit old query summary");}
    setup();capture();cb::enabled=false;
    for(unsigned n=0;n<30;++n)FinisherQueryDetourCB1(reinterpret_cast<void*>(manager),&out,5);
    check(diagnostic_trace::writes==8&&originalCalls==30&&hostD51::reads==0,"F3 summaries share existing eight-line per-second cap; native work unthrottled");
    setup();{cb::ReleaseScope outer(cb::Kind::Thrust,9);
        {cb::ReleaseScope native(cb::Kind::None,99);check(cb::activeRelease==cb::Kind::None&&cb::activeGestureD60==0,"native-only nested scope clears gesture");}
        check(cb::activeRelease==cb::Kind::Thrust&&cb::activeGestureD60==9,"nested return restores exact outer gesture");}
    check(cb::activeRelease==cb::Kind::Cut&&cb::activeGestureD60==7001,"scope exit restores original kind and gesture");
    setup();capture();cb::nativeValidationTestD61=true;before=hostD51::memory;
    FinisherQueryDetourCB1(reinterpret_cast<void*>(manager),&out,2);
    check(has("D61_NATIVE_FINISHER_VALIDATION_TEST")&&originalCalls==1&&nativeOnlyCalls==1&&hostD51::reads==0&&lastContext.ip==0x93999B,"D61 native comparison removes only physical candidate veto and reads no native snapshot");
    check(hostD51::memory==before&&out==reinterpret_cast<void*>(0x12345678),"D61 comparison preserves native output and memory");
    setup();capture();cb::nativeValidationTestD61=true;returnTechnique=false;
    FinisherQueryDetourCB1(reinterpret_cast<void*>(manager),&out,2);
    check(out==nullptr&&originalCalls==1&&has("nativeResult=NO_REFERENCE"),"D61 comparison never fabricates an unavailable combo");
    setup();capture();cb::nativeValidationTestD61=true;cb::nativeValidationTestD61=false;
    FinisherQueryDetourCB1(reinterpret_cast<void*>(manager),&out,2);
    check(lastContext.ip==g_combo_base6691+0x939946&&has("rejected=1"),"D61 comparison OFF restores existing physical filter");
    // D62 connected counterexample: the real selector + snapshot + candidate
    // callback + measured-path projection + table classification execute.
    for(const char* label:{"Blunt_C09","BluntShield_C09"}){
        setup();capture();frik_pose_probe::pathKnownD62=true;
        const cb::Row* row=nullptr;for(const auto& r:cb::rows)if(std::string(r.name)==label)row=&r;
        std::uint8_t guid[16]{};cb::parseGuid(row->technique,guid);hostD51::put(recipe+0x28,guid);
        constexpr std::uintptr_t combatState=0xB0000,enemy=0x80000,enemyOwner=0x90000,enemyEntity=0xA0000;
        hostD51::put(actor+0x210,combatState);hostD51::put(combatState+0xCF8,enemy);
        hostD51::put(enemy,g_combo_base6691.load()+0x3E3FAC8);hostD51::put(enemy+0x1F8,enemyOwner);
        hostD51::put(enemyOwner+0x38,enemyEntity);hostD51::put(enemyEntity,g_combo_base6691.load()+0x4072C78);
        const float transform[12]{1,0,0,0,0,1,0,0,0,0,1,0};hostD51::put(enemyEntity+0x58,transform);
        before=hostD51::memory;FinisherQueryDetourCB1(reinterpret_cast<void*>(manager),&out,1);
        check(lastContext.ip==0x93999B&&originalCalls==1&&has("policyPass=1 rejected=0")&&has("verdict=OK"),
              "D62 actual selector admits swung weapon-head C09 physical candidate without bypassing native selector");
        check(hostD51::memory==before&&out==reinterpret_cast<void*>(0x12345678),
              "D62 weapon-head correction preserves native memory and selector result");
        returnTechnique=false;FinisherQueryDetourCB1(reinterpret_cast<void*>(manager),&out,1);
        check(out==nullptr&&originalCalls==2&&has("nativeResult=NO_REFERENCE"),
              "D62 physically eligible weapon-head finisher cannot force native acceptance");
    }
    std::printf("D51 production finisher callbacks: %u/%u; native selector is a boundary double; game NOT_RUN\n",checks-failures,checks);
    return failures?1:0;
}
