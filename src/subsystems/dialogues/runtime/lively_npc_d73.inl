// Included in npc_lookat_hmd.cpp after its existing exact-retail hook owner.
#include "subsystems/dialogues/dialogue_participants_d76.h"
namespace kcd2vr::lively_npc_d73 {
namespace {
core::SnapshotMailbox<Speaker> speakers;
using SubtitlesFn=bool(__fastcall*)(void*,void*,void*,void*,const bool*);
using ClearFn=bool(__fastcall*)(void*,void*);
SubtitlesFn subtitleOriginal=nullptr;ClearFn clearOriginal=nullptr;
std::optional<DetourModKit::hook::Hook> subtitleHook,clearHook;
std::uintptr_t base=0;std::atomic<bool> retired{false};
diagnostics::npc_dialogue_d52::Budget report;
// Published by the existing worker; VM callbacks need no clock/native read
// merely to discover whether optional dialogue work is currently wanted.
std::atomic<bool> dialogueInterest{false};
std::atomic<std::uint64_t> dialogueRevision{1};
template<class T> bool Read(std::uintptr_t at,T& v) noexcept {
    __try {if(!at)return false;std::memcpy(&v,reinterpret_cast<void*>(at),sizeof(v));return true;}
    __except(EXCEPTION_EXECUTE_HANDLER){return false;}
}
bool Named(void* name,std::uintptr_t rva) noexcept {
    std::uintptr_t text=0;return Read(reinterpret_cast<std::uintptr_t>(name),text)&&text==base+rva;
}
bool __fastcall Subtitle(void* hud,void* name,void* text,void* actor,const bool* isPlayer) {
    // Exact retail helper is (this, function-name CryString*, text CryString*,
    // speaker CryString*, IsPlayer bool*). Preserve all arguments and result.
    const auto before=mode.load(std::memory_order_acquire);
    const bool admitted=ready.load(std::memory_order_acquire)&&(before&1)&&dialogueInterest.load(std::memory_order_acquire)&&
        reinterpret_cast<std::uintptr_t>(_ReturnAddress())==base+0x1457A49&&Named(name,0x3B8DBB8);
    unsigned char player=0;const bool known=admitted&&Read(reinterpret_cast<std::uintptr_t>(isPlayer),player)&&player<=1;
    const bool result=subtitleOriginal(hud,name,text,actor,isPlayer);
    if(admitted&&before==mode.load(std::memory_order_acquire)){
        Frame f{};const auto now=std::uint64_t(GetTickCount64());
        if(known&&result&&frames.read(f)&&Current(f,now,before)&&f.dialogue&&f.scene)
            (void)speakers.publish({reinterpret_cast<std::uintptr_t>(hud),f.actor,f.scene,before,now,player!=0});
        else (void)speakers.publish({});
    }
    return result;
}
bool __fastcall Clear(void* hud,void* name) {
    if(!Enabled()||!dialogueInterest.load(std::memory_order_acquire))return clearOriginal(hud,name);
    const auto before=mode.load(std::memory_order_acquire);
    const bool exact=reinterpret_cast<std::uintptr_t>(_ReturnAddress())==base+0x17990F4&&Named(name,0x3C147C0);
    const bool result=clearOriginal(hud,name);
    if(exact){
        Speaker old{};Frame f{};const auto now=std::uint64_t(GetTickCount64());
        if(speakers.read(old)&&old.hud==reinterpret_cast<std::uintptr_t>(hud)){
            if(result&&before==mode.load(std::memory_order_acquire)&&frames.read(f)&&Current(f,now,before)&&SpeakerCurrentD81(old,f,now))
                (void)speakers.publish({old.hud,f.actor,f.scene,before,now,false,true});
            else (void)speakers.publish({});
        }
    }
    return result;
}
bool Wanted() noexcept {return (Enabled()&&dialogueInterest.load(std::memory_order_acquire))||cleanup.load(std::memory_order_acquire);}
void Service(const native_script_t2::Context& ctx) noexcept {
    static std::uint64_t last=0,vm=0,lastMode=0;
    const auto generation=mode.load(std::memory_order_acquire);
    const auto revision=dialogueRevision.load(std::memory_order_acquire);
    if(vm==ctx.generation&&generation==lastMode&&ctx.now>=last&&ctx.now-last<100)return;
    last=ctx.now;vm=ctx.generation;lastMode=generation;
    Frame f{};Speaker s{};
    const bool valid=ready.load(std::memory_order_acquire)&&dialogueInterest.load(std::memory_order_acquire)&&frames.read(f)&&Current(f,ctx.now,generation)&&
        reinterpret_cast<std::uintptr_t>(frik_pose_probe::LastPlayerEntity())==f.actor;
    const bool speechKnown=valid&&speakers.read(s)&&SpeakerCurrentD81(s,f,ctx.now);
    const bool speaking=speechKnown&&Speaking(s,f,ctx.now);
    const bool listening=speechKnown&&ListeningD81(s,f,ctx.now);
    const auto members=valid&&f.dialogue?Members(base,f,[](auto at,auto& value){return Read(at,value);}):MembersD76{};
    const bool dialogue=valid&&f.dialogue&&members.known;
    char memberText[352]{};std::size_t used=0;
    if(dialogue)for(unsigned i=0;i<members.count;++i){
        const int n=std::snprintf(memberText+used,sizeof(memberText)-used,"%s%llu",used?",":"",(unsigned long long)members.ids[i]);
        if(n<=0||std::size_t(n)>=sizeof(memberText)-used){memberText[0]=0;break;}used+=std::size_t(n);
    }
    char input[768]{},output[256]{};
    std::snprintf(input,sizeof(input),"D76|%llu|%llu|%u|%u|%u|%.6f|%.6f|%.6f|%.6f|%.6f|%.6f|%llu|%u|%s",
        (unsigned long long)ctx.generation,(unsigned long long)generation,
        valid&&f.gameplay,dialogue,dialogue&&listening,double(f.x),double(f.y),double(f.z),double(f.fx),double(f.fy),double(f.fz),
        (unsigned long long)(dialogue?f.scene:0),dialogue&&listening&&bool(generation&2),memberText[0]?memberText:"0");
    const bool ok=ctx.CallPacket("kcd2vr_life_d73",input,output,sizeof(output));
    if(ok&&std::strcmp(output,"D76|0")==0){
        cleanup.store(false,std::memory_order_release);
        // An older reply cannot erase an exit/OFF retirement published during
        // the VM call. Recheck AFTER clearing to close the check/store race.
        if(generation!=mode.load(std::memory_order_acquire)||revision!=dialogueRevision.load(std::memory_order_acquire))
            cleanup.store(true,std::memory_order_release);
    }else cleanup.store(true,std::memory_order_release);
    if(report.allow(diagnostics::capture168::Domain::Interaction,ctx.now,1000,512)){
        char line[640]{};std::snprintf(line,sizeof(line),"D76 F5_NPC_LIFE enabled=%d current=%d gameplay=%d dialogue=%d henrySpeaking=%d listening=%d speechKnown=%d packet=%d reply=%s hmdTarget=EXACT_HENRY_ONLY participantBody=EXACT_SCENE_MEMBERS_GATED",
            bool(generation&1),valid,valid&&f.gameplay,valid&&f.dialogue,speaking,listening,speechKnown,ok,output);diagnostic_trace::Write(line);}
}
bool Install() noexcept {
    if(ready.load())return true;if(retired.load())return false;
    base=reinterpret_cast<std::uintptr_t>(GetModuleHandleW(L"WHGame.dll"));
    const unsigned char show[]={0x48,0x89,0x5C,0x24,0x10,0x48,0x89,0x74,0x24,0x18,0x48,0x89,0x7C,0x24,0x20};
    const unsigned char clear[]={0x48,0x89,0x5C,0x24,0x08,0x57,0x48,0x83,0xEC,0x60};
    unsigned char a[sizeof(show)]{},b[sizeof(clear)]{};
    if(!retail_build112::Verify(base)||!Read(base+0x1457A7C,a)||!Read(base+0x566BCC,b)||
       std::memcmp(a,show,sizeof(a))||std::memcmp(b,clear,sizeof(b)))return false;
    HMODULE pinned=nullptr;if(!GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS|GET_MODULE_HANDLE_EX_FLAG_PIN,
        reinterpret_cast<LPCWSTR>(&Subtitle),&pinned)||!pinned)return false;
    auto sh=DetourModKit::hook::inline_at(DetourModKit::hook::InlineRequest{"kcd2vr_d73_subtitle_speaker",
        DetourModKit::hook::Target{DetourModKit::Address{base+0x1457A7C}},DetourModKit::hook::Options{}},&Subtitle);
    if(!sh)return false;subtitleHook=std::move(*sh);subtitleOriginal=subtitleHook->original<SubtitlesFn>();
    auto ch=DetourModKit::hook::inline_at(DetourModKit::hook::InlineRequest{"kcd2vr_d73_subtitle_clear",
        DetourModKit::hook::Target{DetourModKit::Address{base+0x566BCC}},DetourModKit::hook::Options{}},&Clear);
    if(!ch){retired.store(true);return false;}clearHook=std::move(*ch);clearOriginal=clearHook->original<ClearFn>();
    if(!subtitleOriginal||!clearOriginal||!native_script_t2::Register(&Service,&Wanted)||!native_script_t2::Install()||
       !subtitleHook->enable()||!clearHook->enable()){retired.store(true);return false;}
    ready.store(true,std::memory_order_release);return true;
}
void Retire() noexcept {ready.store(false,std::memory_order_release);SetEnabled(false);(void)frames.publish({});(void)speakers.publish({});}
}
bool Prepare() noexcept {return Install();}
void Publish(std::uintptr_t actor,std::uintptr_t scene,bool gameplay,bool dialogue,std::uint64_t now,const dialogue_state59c10r2::State* state) noexcept {
    if(!Enabled())return;
    Frame f{};f.tick=now;f.actor=actor;f.scene=scene;f.generation=mode.load(std::memory_order_acquire);
    (void)gameplay; // call-site compatibility; this feature has no outdoor mode
    f.dialogue=dialogue&&state&&DialogueScene(*state,now)&&state->entity==actor&&state->membership.sceneD52==scene;
    // R163: training fight (practice weapon) = no NPC life / listening animations; a scene that had them gets dialogue=0 below
    // (dialogueInterest drops -> cleanup -> the Lua side gets "dialogue 0" and gives the NPCs back), counted once per scene.
    {static std::atomic<std::uintptr_t> sceneR163{0};
     if(f.dialogue&&kcd2vr::training_vanilla_r163::skipOnce(kcd2vr::training_vanilla_r163::LivelyNpc,sceneR163,scene))f.dialogue=false;}
    if(f.dialogue){
        f.dialogueInstance=state->instance;f.dialogueManager=state->membership.managerD52;f.playerSoul=state->membership.playerSoulD52;
        f.hmd=CameraHook::GetLatestVisualPositionWorld(f.x,f.y,f.z,60)&&CameraHook::GetLatestVisualForwardWorld(f.fx,f.fy,f.fz,60);
    }
    if(frames.publish(f)){
        const bool prior=dialogueInterest.load(std::memory_order_acquire);
        if(prior!=f.dialogue){
            dialogueRevision.fetch_add(1,std::memory_order_acq_rel);
            if(prior)cleanup.store(true,std::memory_order_release);
            dialogueInterest.store(f.dialogue,std::memory_order_release);
        }
        if(!f.dialogue)(void)speakers.publish({});
    }
}
}
