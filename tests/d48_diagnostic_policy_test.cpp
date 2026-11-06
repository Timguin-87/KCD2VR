#include "diagnostics/capture168.h"
#include "diagnostics/hook_cost178r22.h"
#include "subsystems/ranged_reload/reload_diagnostics_d48.h"
#include <cassert>
#include <cstdio>
#include <string>
#include <vector>
int main(){
    namespace c=kcd2vr::diagnostics::capture168;
    namespace r=kcd2vr::reload_diagnostics_d48;
    namespace h=kcd2vr::hookcost178r22;
    constexpr std::uint64_t now=10000;
    for(const char* line:{"X FAULT callback=bad","KCD2VR BUILD D48","T4 F9 rung=2","C33 state=CHANGED","D48_F11 RELOAD session=1"})
        assert(!c::essential(line)&&!c::accept(line,now));
    c::gate.select(c::Domain::Bug,true,now);
    assert(c::active(c::Domain::Hud,now)&&!c::active(c::Domain::Combat,now));
    assert(c::gate.session[unsigned(c::Domain::Hud)].load()==c::gate.session[unsigned(c::Domain::Bug)].load());
    assert(c::accept("X FAULT callback=bad",now));
    assert(!c::accept("C54R3 HIT_CHAIN reason=INPUT_OWNER_DOOR_FAULT",now));
    c::gate.select(c::Domain::Bug,false,now+1);
    assert(!c::accept("X FAULT callback=bad",now+1));
    assert(c::accept("F2_CAUSAL session=1 stage=2",now+2));
    assert(!c::accept("F2_CAUSAL session=999 stage=2",now+2));
    assert(!c::accept("F2_CAUSAL session=1 stage=2",now+5002));
    c::gate.select(c::Domain::Interaction,true,now);
    assert(c::active(c::Domain::Hands,now));
    assert(c::accept("T18 AMMO stage=D48_CLEANUP_HAND_FREE",now));
    assert(c::accept("R8D HOLSTER_REQUEST reason=FAULT",now));
    for(const char* line:{"D48 HOLSTER_REQUEST reason=HUD_FAULT", "D48 HOLSTER_NOT_ENTERED reason=FAULT",
        "D48 HOLSTER_PRESENTATION mode=CONTROLLER", "D48 HOLSTER_REVEAL entity=1", "D48 FRONT_HOLSTER side=0",
        "HS2 HOLSTER_SAY reason=FAULT", "HS2 WEAPON_SWITCH stagedDraw", "H24 HOLSTER_FENCE nativeBusy=0",
        "V66.116 HOLSTER nativeReturned=0", "V66.126 EQUIPMENT_REJECT reason=LIVE_LOADOUT_SLOT", "C62 EQUIPMENT_REJECT reason=NATIVE_ENTRY_IDENTITY"})
        assert(c::accept(line,now));
    assert(c::accept("C54R3 F5_DOOR_CHAIN reason=BOXING_HUD_FAULT",now));
    assert(!c::accept("C54R3 HIT_CHAIN reason=DOOR_FAULT",now));
    c::gate.select(c::Domain::Interaction,false,now+1);
    c::gate.select(c::Domain::Vrik,true,now);
    assert(c::active(c::Domain::HandReference,now));
    assert(!c::accept("T18 AMMO stage=D48_CLEANUP_ROUTE_END",now));
    for(const char* line:{"V66.89-POSE TEMP_WRITE reason=HUD_FAULT","V66.115R1 HEAD_BODY x=1","V66.128-BODY x=1",
        "V66.137 COMPOSE_SET x=1","V66.89-ANATOMY x=1","V66.92-HIERARCHY x=1","V66.128-LEGS x=1",
        "V66.89-FINGER-TEST x=1","V66.94-FINGER x=1","V66.105-FINGER-TEST x=1","V66.92R1-FINGER-DATA x=1",
        "W1 HAND_ANATOMY x=1","R59.16C8 PALM_AUTO x=1","WS TORSO x=1",
        "V65.09-F10-ORDER x=1","V65.02-SETIK x=1","V65.02-OWNER x=1","V65.47-WEAPON-OWNER LIGHT x=1"})
        assert(c::accept(line,now));
    assert(!c::accept("V66.89-POSE RESTORE_FAILED error=1",now));
    assert(!c::accept("D48 HOLSTER_PRESENTATION mode=CONTROLLER",now));
    const auto serial=c::gate.session[unsigned(c::Domain::Vrik)].load();
    assert(c::gate.cycleVrik(now+1)==2&&c::gate.session[unsigned(c::Domain::Vrik)].load()==serial);
    assert(c::gate.cycleVrik(now+2)==3&&c::active(c::Domain::HandReference,now+2));
    assert(c::gate.cycleVrik(now+3)==0&&!c::active(c::Domain::HandReference,now+3));
    r::recordSource("OFF",now);r::recordStart("OFF",now,42);
    assert(r::next.load()==0);
    c::gate.select(c::Domain::Reload,true,now);
    assert(c::accept("T18 AMMO stage=D48_CLEANUP_ROUTE_END",now));
    assert(c::accept("V65.02-SETIK x=1",now));
    assert(!c::accept("V66.89-POSE TEMP_WRITE x=1",now));
    r::recordSource("NO_NATIVE_WEAPON_SEED",now);
    r::recordStart("NO_SOURCE_PUBLISHED",now,42);
    r::recordSource(nullptr,now);
    r::recordStart("TAKEN",now,43);
    r::recordSource("LATER_SOURCE_CHANGED",now+1);
    std::vector<std::string> rows;
    c::gate.select(c::Domain::Reload,false,now+2);
    r::drain(now+3,[&](const char* s){assert(c::accept(s,now+3));rows.emplace_back(s);});
    assert(rows.size()==2);
    assert(rows[0].find("order=1 ")!=std::string::npos&&rows[1].find("order=2 ")!=std::string::npos);
    assert(rows[0].find("sourceReason=NO_NATIVE_WEAPON_SEED")!=std::string::npos);
    assert(rows[1].find("sourceReason=SOURCE_OK")!=std::string::npos);
    c::gate.select(c::Domain::Reload,true,now+4);
    for(unsigned i=0;i<100;++i)r::recordStart("TEST",now+4,44);
    assert(r::dropped.load()==37); // bounded 64-slot ring reserves one slot
    rows.clear();r::drain(now+4,[&](const char* s){rows.emplace_back(s);});assert(rows.size()==63);
    assert(c::gate.poll(now+4+c::Duration)==(1u<<unsigned(c::Domain::Reload)));
    assert(!c::active(c::Domain::Reload,now+4+c::Duration));
    // Actual timing-accounting rule, including nested hooks inside native code.
    assert(h::exclusive(1000,300,400,0)==300);
    assert(h::exclusive(1000,300,600,200)==300);
    assert(h::exclusive(100,80,80,60)==0);
    assert(h::exclusive(100,200,0,0)==0);
    const auto tick=h::tickNow();c::gate.select(c::Domain::Bug,true,tick);
    {h::Scope outer(h::Alignment);{h::Scope inner(h::AlignmentPolicy);}}
    assert(h::counters[h::Alignment].calls==1&&h::counters[h::AlignmentPolicy].calls==1);
    char output[2048]{};assert(h::summary(output,sizeof(output),1000)>0);
    assert(std::string(output).find("scope=INSTRUMENTED_HOOKS_ONLY nested=EXCLUSIVE")!=std::string::npos);
    std::puts("PASS D48 five recorders, finite receipt drain, coherent F11 start evidence, bounded queue and exclusive timing");
}
