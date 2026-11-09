#pragma once
// R158 Shift+F5 quest progression recorder: the two writers (see quest_diag_r158.h for what each row means).
// included by mod.cpp only. every row goes through diagnostic_trace::WriteFor(Domain::Quest) = only while Shift+F5
// records; the end receipt goes through WriteReceiptFor (5 s grace of the same session). no game write.
#include "quest_diag_r158.h"
#include "capture168.h"
#include "../native_script_t2.h"
#include "../diagnostic_trace.h"
#include <windows.h>
#include <atomic>
#include <cstdio>
namespace kcd2vr::quest_diag_r158 {
// Lua side state for the control-thread row (game thread writes, control thread reads).
enum LuaState : int {LuaNotAsked=0,LuaOk=1,LuaNoReply=2,LuaBadReply=3,LuaUnavailable=4};
inline std::atomic<int> luaState{LuaNotAsked};
inline std::atomic<std::uint64_t> luaReplies{0},luaFailures{0},npcRows{0};
inline const char* LuaName(int s) noexcept {
    return s==LuaOk?"OK":s==LuaNoReply?"NO_REPLY":s==LuaBadReply?"BAD_REPLY":s==LuaUnavailable?"BRIDGE_UNAVAILABLE":"NOT_ASKED";
}
inline std::uint64_t Session() noexcept {
    return diagnostics::capture168::gate.session[unsigned(diagnostics::capture168::Domain::Quest)].load(std::memory_order_acquire);
}
inline bool Wanted() noexcept { // native_script_t2 intent: atomics + tick only
    return diagnostics::capture168::active(diagnostics::capture168::Domain::Quest,std::uint64_t(GetTickCount64()));
}
inline void Write(const char* line) noexcept {diagnostic_trace::WriteFor(diagnostics::capture168::Domain::Quest,line);}

// game thread, inside the original-first native script update (native_script_t2), at most once per second.
inline void LuaService(const native_script_t2::Context& c) noexcept {
    namespace cap=diagnostics::capture168;
    if(!cap::active(cap::Domain::Quest,c.now))return;
    static std::uint64_t seenSession=0,lastAsk=0,serial=0;static Gate henry;static NpcTable npcs;static int lastLua=-1;
    const auto session=Session();
    if(session!=seenSession){seenSession=session;lastAsk=0;henry.Reset();npcs.Reset();lastLua=-1;}
    if(lastAsk&&c.now>=lastAsk&&c.now-lastAsk<kLuaPeriodMs)return;
    lastAsk=c.now;++serial;
    char input[40]{};std::snprintf(input,sizeof(input),"Q158|%llu",static_cast<unsigned long long>(serial));
    static char reply[12288];reply[0]=0;
    int state=LuaNoReply;
    if(c.CallPacket(kLuaFunction,input,reply,sizeof(reply))){
        state=LuaBadReply;
        npcs.BeginPass();
        const bool parsed=ParseReply(reply,serial,[&](char kind,const char* id,const char* name,const char* rest) noexcept {
            char line[kRowMax+160]{};
            if(kind=='P'){
                const int due=henry.Due(rest,c.now);if(!due)return;
                std::snprintf(line,sizeof(line),"R158 QUEST_HENRY session=%llu %s%s",static_cast<unsigned long long>(session),rest,due==2?" repeat=1":"");
                Write(line);
            }else if(kind=='E'){
                std::snprintf(line,sizeof(line),"R158 QUEST_LUA session=%llu note=%s",static_cast<unsigned long long>(session),rest);Write(line);
            }else if(kind=='N'){
                const int r=npcs.Observe(id,name,rest);if(!r)return;
                std::snprintf(line,sizeof(line),"R158 QUEST_NPC session=%llu id=%s name=%s %s%s%s",static_cast<unsigned long long>(session),id,name,rest,
                    r==1?" new=1":"",r<0?" untracked=1":"");
                Write(line);npcRows.fetch_add(1,std::memory_order_relaxed);
            }
        });
        if(parsed){
            state=LuaOk;luaReplies.fetch_add(1,std::memory_order_relaxed);
            npcs.EndPass([&](const char* id,const char* name) noexcept {
                char line[200]{};std::snprintf(line,sizeof(line),"R158 QUEST_NPC_GONE session=%llu id=%s name=%s (left 25 m, despawned or no longer an NPC)",
                    static_cast<unsigned long long>(session),id,name);Write(line);});
        }
    }
    if(state!=LuaOk)luaFailures.fetch_add(1,std::memory_order_relaxed);
    luaState.store(state,std::memory_order_release);
    if(state!=lastLua){lastLua=state;char line[300]{};
        std::snprintf(line,sizeof(line),"R158 QUEST_LUA session=%llu state=%s%s",static_cast<unsigned long long>(session),LuaName(state),
            state==LuaOk?"":state==LuaNoReply?" (no answer from kcd2vr_quest_diag_r158: companion pak without zzkcd2vr_quest_diag_r158.lua, or the script VM refused the call)":
            " (answer did not match the question)");
        Write(line);}
}
// control thread, on Shift+F5 ON: the Lua bridge (existing native_script_t2 hook, idempotent). retried on every ON.
inline bool EnsureLua() noexcept {
    const bool ok=native_script_t2::Register(&LuaService,&Wanted)&&native_script_t2::Install();
    if(!ok)luaState.store(LuaUnavailable,std::memory_order_release);
    else if(luaState.load(std::memory_order_acquire)==LuaUnavailable)luaState.store(LuaNotAsked,std::memory_order_release);
    return ok;
}
// control thread, every loop. read() fills StateIn from the verified readers; called only while recording, 4x/s.
template<class Read> void StateService(std::uint64_t now,Read&& read) noexcept {
    namespace cap=diagnostics::capture168;
    static bool was=false;static std::uint64_t seenSession=0,lastSample=0,rows=0;static Gate gate;
    const bool active=cap::active(cap::Domain::Quest,now);const auto session=Session();
    if(was&&(!active||session!=seenSession)){
        char line[300]{};std::snprintf(line,sizeof(line),"R158 QUEST_END session=%llu stateRows=%llu npcRows=%llu luaReplies=%llu luaFailures=%llu",
            static_cast<unsigned long long>(seenSession),static_cast<unsigned long long>(rows),
            static_cast<unsigned long long>(npcRows.load(std::memory_order_relaxed)),static_cast<unsigned long long>(luaReplies.load(std::memory_order_relaxed)),
            static_cast<unsigned long long>(luaFailures.load(std::memory_order_relaxed)));
        diagnostic_trace::WriteReceiptFor(cap::Domain::Quest,seenSession,line);was=false;
    }
    if(!active)return;
    if(!was||session!=seenSession){was=true;seenSession=session;lastSample=0;rows=0;gate.Reset();
        npcRows.store(0,std::memory_order_relaxed);luaReplies.store(0,std::memory_order_relaxed);luaFailures.store(0,std::memory_order_relaxed);}
    if(lastSample&&now>=lastSample&&now-lastSample<kStatePeriodMs)return;
    lastSample=now;
    StateIn in{};read(in);in.lua=LuaName(luaState.load(std::memory_order_acquire));
    char text[kRowMax]{};if(FormatState(text,sizeof(text),in)<=0)return;
    const int due=gate.Due(text,now);if(!due)return;
    char line[kRowMax+80]{};std::snprintf(line,sizeof(line),"R158 QUEST_STATE session=%llu %s%s",static_cast<unsigned long long>(session),text,due==2?" repeat=1":"");
    Write(line);++rows;
}
} // namespace kcd2vr::quest_diag_r158
