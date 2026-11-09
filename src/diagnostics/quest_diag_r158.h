#pragma once
// R158 QUEST PROGRESSION RECORDER (Miki 8 Nov: "can you add diagnostic for quest progression to some F5?").
// Shift+F5 with Developer mode, 180 s like every recorder (capture168 Domain::Quest), Shift+F5 again stops it.
// read-only: no game write, no new native hook, no on-screen text, rows only in KCD2VR.log, no INI key.
// two producers:
//   control thread, 4x per second, written on change + every 2 s: R158 QUEST_STATE = what the C++ side already reads
//     (Henry inCombat flag, the native dialogue instance, the full-screen tutorial / hold-to-look action maps, the
//     wh_ui_TutorialsEnabled / wh_cs_ForceBlock cvars, Henry's reciprocal combat opponent, practice weapon in hand).
//   game thread, once per second through the existing native_script_t2 Lua bridge (companion
//     Scripts/Startup/zzkcd2vr_quest_diag_r158.lua): R158 QUEST_HENRY (IsInCombatDanger / IsInTenseCircumstance = the
//     game's own StartDialog precondition, BasicAIActions.lua:120-132) every 2 s, and R158 QUEST_NPC per NPC within
//     25 m on change (dead, unconscious, combat mode, arranged fight, can talk / chat, chat request, waiting reply...),
//     R158 QUEST_NPC_GONE when one leaves the 25 m / despawns.
// this header is the pure part (row gate, NPC table, reply parser, state row) - host test tests/quest_diag_r158_test.cpp.
#include <array>
#include <climits>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstring>
namespace kcd2vr::quest_diag_r158 {
inline constexpr std::uint64_t kHeartbeatMs=2000; // an unchanged row is repeated this often (repeat=1)
inline constexpr std::uint64_t kLuaPeriodMs=1000; // one Lua question per second
inline constexpr std::uint64_t kStatePeriodMs=250; // C++ state sampled 4x per second
inline constexpr std::size_t kRowMax=512;
inline constexpr std::size_t kKeyMax=40;
inline constexpr unsigned kMaxNpcs=16;
inline constexpr char kLuaFunction[]="kcd2vr_quest_diag_r158";

// game text (entity names) into one log row: printable ASCII only, '|' and newlines never pass. returns the length.
inline std::size_t Clean(char* s,std::size_t cap) noexcept {
    if(!s||!cap)return 0;std::size_t n=0;
    for(;n<cap-1&&s[n];++n){const unsigned char c=static_cast<unsigned char>(s[n]);if(c<0x20||c>0x7E||c=='|')s[n]='?';}
    s[n]=0;return n;
}
inline void Copy(char* to,std::size_t cap,const char* from,std::size_t n) noexcept {
    if(!to||!cap)return;if(!from)n=0;if(n>cap-1)n=cap-1;if(n)std::memcpy(to,from,n);to[n]=0;Clean(to,cap);
}

// one row: 1 = changed (or first), 2 = unchanged but the 2 s heartbeat is due, 0 = skip.
struct Gate {
    char last[kRowMax]{};std::uint64_t at=0;bool has=false;
    int Due(const char* text,std::uint64_t now) noexcept {
        if(!text)return 0;
        if(!has||std::strncmp(last,text,kRowMax-1)!=0){Copy(last,sizeof(last),text,std::strlen(text));at=now;has=true;return 1;}
        if(now<at||now-at>=kHeartbeatMs){at=now;return 2;}
        return 0;
    }
    void Reset() noexcept {has=false;at=0;last[0]=0;}
};

// NPCs of one Lua reply, keyed by the entity id text. a row is written when an NPC is new or its text changed; one
// that is missing from a reply (left 25 m, despawned) is reported once as GONE and its slot freed. more than kMaxNpcs
// NPCs: the extra ones are written every reply (untracked=1), never dropped silently.
struct NpcTable {
    struct Entry {char id[kKeyMax]{},name[kKeyMax+8]{},last[kRowMax]{};bool used=false,seen=false;};
    std::array<Entry,kMaxNpcs> e{};
    void Reset() noexcept {for(auto& x:e)x=Entry{};}
    void BeginPass() noexcept {for(auto& x:e)x.seen=false;}
    // 1 new, 2 changed, 0 unchanged, -1 untracked (table full: write it anyway)
    int Observe(const char* id,const char* name,const char* text) noexcept {
        if(!id||!text)return 0;
        for(auto& x:e)if(x.used&&std::strncmp(x.id,id,kKeyMax-1)==0){
            x.seen=true;if(name)Copy(x.name,sizeof(x.name),name,std::strlen(name));
            if(std::strncmp(x.last,text,kRowMax-1)==0)return 0;
            Copy(x.last,sizeof(x.last),text,std::strlen(text));return 2;}
        for(auto& x:e)if(!x.used){x.used=x.seen=true;Copy(x.id,sizeof(x.id),id,std::strlen(id));
            Copy(x.name,sizeof(x.name),name?name:"?",std::strlen(name?name:"?"));Copy(x.last,sizeof(x.last),text,std::strlen(text));return 1;}
        return -1;
    }
    template<class Gone> unsigned EndPass(Gone&& gone) noexcept {
        unsigned n=0;for(auto& x:e)if(x.used&&!x.seen){gone(static_cast<const char*>(x.id),static_cast<const char*>(x.name));x=Entry{};++n;}
        return n;
    }
    unsigned Count() const noexcept {unsigned n=0;for(const auto& x:e)n+=x.used?1u:0u;return n;}
};

// the Lua reply: "Q158|<serial>" then lines "P|<henry fields>", "N|<id>|<name>|<fields>", "E|<reason>".
// sink(kind,id,name,rest) per line (id/name empty for P/E). false = the header does not match this serial
// (old reply, wrong function, BAD/ERROR) - nothing is passed on. lines and fields are bounded, text is cleaned.
template<class Sink> bool ParseReply(const char* reply,std::uint64_t serial,Sink&& sink) noexcept {
    if(!reply)return false;
    char head[40]{};std::snprintf(head,sizeof(head),"Q158|%llu",static_cast<unsigned long long>(serial));
    const std::size_t hn=std::strlen(head);
    if(std::strncmp(reply,head,hn)!=0||(reply[hn]!='\n'&&reply[hn]!=0))return false;
    const char* p=reply+hn;unsigned lines=0;
    while(*p=='\n'&&lines<64){
        ++p;const char* end=std::strchr(p,'\n');const std::size_t n=end?std::size_t(end-p):std::strlen(p);
        ++lines;
        if(n>=2&&p[1]=='|'&&(p[0]=='P'||p[0]=='E')){
            char rest[kRowMax]{};Copy(rest,sizeof(rest),p+2,n-2);sink(p[0],"","",static_cast<const char*>(rest));
        }else if(n>=2&&p[0]=='N'&&p[1]=='|'){
            const char* a=p+2;const char* lim=p+n;
            const char* b=static_cast<const char*>(std::memchr(a,'|',std::size_t(lim-a)));
            const char* c=b?static_cast<const char*>(std::memchr(b+1,'|',std::size_t(lim-(b+1)))):nullptr;
            if(b&&c&&b>a){
                char id[kKeyMax]{},name[kKeyMax+8]{},rest[kRowMax]{};
                Copy(id,sizeof(id),a,std::size_t(b-a));Copy(name,sizeof(name),b+1,std::size_t(c-(b+1)));
                Copy(rest,sizeof(rest),c+1,std::size_t(lim-(c+1)));
                sink('N',static_cast<const char*>(id),static_cast<const char*>(name),static_cast<const char*>(rest));
            }
        }
        if(!end)break;p=end;
    }
    return true;
}

// control-thread state, read from already-verified readers only (see mod.cpp Shift+F5 block).
struct StateIn {
    int henryInCombat=-1;                      // HenryInCombat178r31: -1 not observed (combat hook idle), 0/1
    bool dlgKnown=false,dlgActive=false;       // activity_lens59c10r1::Dialogue (C19C2 HUD_DIALOGUE_STATE source)
    std::uintptr_t dlgInstance=0;unsigned dlgState=0,dlgFlags=0;
    bool dlgParticipant=false,dlgListed=false,dlgInputKnown=false,dlgInputEnabled=false;
    bool tutorialScreen=false,holdLookMap=false; // close_tutorial / infinite_hold_interaction_init maps enabled now
    int tutorialsCvar=INT_MIN,forceBlockCvar=INT_MIN; // INT_MIN = cvar not readable
    bool opponentFresh=false,opponentReciprocal=false,practiceWeapon=false;int opponentSkill=-1;
    std::uintptr_t opponent=0;
    const char* lua="NOT_ASKED";
};
inline void Cvar(char* out,std::size_t cap,int v) noexcept {
    if(v==INT_MIN)std::snprintf(out,cap,"?");else std::snprintf(out,cap,"%d",v);
}
inline int FormatState(char* out,std::size_t cap,const StateIn& s) noexcept {
    char tut[16]{},fb[16]{};Cvar(tut,sizeof(tut),s.tutorialsCvar);Cvar(fb,sizeof(fb),s.forceBlockCvar);
    return std::snprintf(out,cap,
        "henryInCombat=%d dialogue{known=%d active=%d instance=%llX state=%u flags=%X participant=%d listed=%d inputMap=%s} "
        "tutorialScreen=%d holdLookPrompt=%d wh_ui_TutorialsEnabled=%s wh_cs_ForceBlock=%s "
        "opponent{seen=%d id=%llX reciprocal=%d skill=%d} practiceWeapon=%d lua=%s",
        s.henryInCombat,s.dlgKnown?1:0,s.dlgActive?1:0,static_cast<unsigned long long>(s.dlgInstance),s.dlgState,s.dlgFlags,
        s.dlgParticipant?1:0,s.dlgListed?1:0,!s.dlgInputKnown?"?":s.dlgInputEnabled?"ENABLED":"DISABLED",
        s.tutorialScreen?1:0,s.holdLookMap?1:0,tut,fb,
        s.opponentFresh?1:0,static_cast<unsigned long long>(s.opponentFresh?s.opponent:0),s.opponentFresh&&s.opponentReciprocal?1:0,
        s.opponentFresh?s.opponentSkill:-1,s.practiceWeapon?1:0,s.lua?s.lua:"?");
}
} // namespace kcd2vr::quest_diag_r158
