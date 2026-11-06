#include "../src/dialogue_participants_d52.h"
#include "../src/npc_look_policy63.h"
#include "dialogue_scene_fixture_d52.h"
#include <cstdio>

int main(){
    using namespace kcd2vr;namespace p=dialogue_participants_d52;namespace c=diagnostics::capture168;
    unsigned checks=0,failures=0;auto check=[&](bool ok,const char* what){++checks;if(!ok){++failures;std::printf("FAIL %s\n",what);}};
    DialogueSceneFixtureD52 memory(0x180000000,0x30000);
    std::uint64_t now=1000;auto state=memory.observe(now);p::Observation result{};
    diagnostics::npc_dialogue_d52::Budget budget;unsigned published=0;
    auto run=[&]{return p::capture(state,memory.base,now,budget,
        [&](auto a,auto& v){return memory.read(a,v);},[&](const auto& r){++published;result=r;return true;});};
    c::gate.mask=0;memory.reads=0;const auto mode=npc_look_policy63::modeD52.load();
    check(!run()&&!memory.reads&&!published,"quiet producer performs zero participant reads");
    c::gate.select(c::Domain::Bug,true,now);
    check(!run()&&!memory.reads,"F2 does not run detailed participant reads");
    c::gate.select(c::Domain::Interaction,true,now);
    check(run()&&result.reason==p::Reason::Observed&&result.count==2&&result.total==2,"actual production observer -> capture -> membership revalidation");
    check(result.people[0].soul==123&&result.people[1].soul==456&&result.playerSoul==123,"soul IDs and Henry identity are observed, not inferred entities");
    check(result.scene==state.membership.sceneD52&&result.tick==state.tick&&result.session!=0,"identity and source-time receipt");
    check(c::accept("D52 F5_DIALOGUE_PARTICIPANTS sourceTick=1000",now),"F5 participant route");
    const auto reads=memory.reads;check(!run()&&memory.reads==reads,"duplicate callback throttled before reads");
    now+=250;memory.mem[dialogue_input_fixture19c3::map+9]=0;state=memory.observe(now);
    check(!state.active&&state.membership.active&&run()&&result.reason==p::Reason::Observed,"native diagnostics operate with disabled dialogue input");
    check(npc_look_policy63::modeD52.load()==mode,"capture does not alter gaze mode");
    now+=250;state=memory.observe(now);memory.mem[DialogueSceneFixtureD52::manager+0x90]=0;
    check(run()&&result.reason==p::Reason::Changed,"retired scene cannot be reported as current participant snapshot");
    memory.mem[DialogueSceneFixtureD52::manager+0x90]=DialogueSceneFixtureD52::instance;
    now+=250;state=memory.observe(now);memory.mem.erase(DialogueSceneFixtureD52::participants+0x38);
    check(run()&&result.reason==p::Reason::Unavailable,"missing participant byte is unavailable, not default active");
    memory.mem[DialogueSceneFixtureD52::participants+0x38]=1;
    now+=250;state=memory.observe(now);unsigned readsOfSecond=0;
    check(p::capture(state,memory.base,now,budget,[&](auto a,auto& v){
        if(a==DialogueSceneFixtureD52::participants+0x20&&++readsOfSecond==2)memory.mem[a]=789;
        return memory.read(a,v);},[&](const auto& r){result=r;return true;})&&result.reason==p::Reason::Changed,"participant replacement during capture rejected");
    memory.mem[DialogueSceneFixtureD52::participants+0x20]=456;
    now+=250;memory.mem[DialogueSceneFixtureD52::scene+8]=DialogueSceneFixtureD52::participants+0x20*10;
    for(unsigned i=2;i<10;++i){memory.mem[DialogueSceneFixtureD52::participants+i*0x20]=500+i;memory.mem[DialogueSceneFixtureD52::participants+i*0x20+0x18]=1;}
    state=memory.observe(now);check(run()&&result.reason==p::Reason::Observed&&result.total==10&&result.count==8,"bounded participant truncation is explicit");
    c::gate.mask=0;memory.reads=0;now+=250;check(!run()&&!memory.reads,"capture stop immediately retires additional reads");
    c::gate.select(c::Domain::Interaction,true,now);now+=c::Duration;memory.reads=0;
    check(!run()&&!memory.reads,"expired F5 does not poll participants");
    check(!c::accept("D52 F5_DIALOGUE_PARTICIPANTS sourceTick=1000",now),"expired receipt routing");
    std::printf("D52 participant capture %u/%u; native/AI/headset not executed\n",checks-failures,checks);
    return failures?1:0;
}
