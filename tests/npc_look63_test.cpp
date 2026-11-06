// Execute the actual production LookAt hook with an explicit native original
// double. Native code/AI are never executed by this host fixture.
#define KCD2VR_TEST_HOOK_SPY115
#define KCD2VR_NPC_LOOK_TEST63
#include <intrin.h>
static void* callerD57=nullptr;
static unsigned callerReadsD57=0;
static void* FixtureCallerD57(){++callerReadsD57;return callerD57;}
#define _ReturnAddress FixtureCallerD57
#include "../src/npc_lookat_hmd.cpp"
#undef _ReturnAddress
#include <array>
#include <cassert>
#include <limits>
#include <vector>
#include "dialogue_scene_fixture_d52.h"
static bool retail63=true,hmd63=true,applied63=true,revoke63=false,native63=true,stopInOriginal63=false;
static unsigned originalCalls63=0,hmdCalls63=0,checks63=0;
static bool flipModeD52=false;
static void* player63=nullptr;
static std::array<float,3> final63{.1f,.2f,1.7f};
static std::vector<std::string> receipts63;
namespace kcd2vr::diagnostic_trace {void Write(const char* text) noexcept {receipts63.emplace_back(text);}}
namespace kcd2vr::retail_build112 {bool Verify(std::uintptr_t) noexcept{return retail63;}bool Ready(std::uintptr_t) noexcept{return retail63;}}
namespace kcd2vr::frik_pose_probe {void* LastPlayerEntity() noexcept {return player63;}}
namespace kcd2vr {
bool CameraHook::DialogueEmbodimentApplied59c13(std::uint64_t) {return applied63;}
bool CameraHook::GetLatestVisualPositionWorld(float& x,float& y,float& z,std::uint64_t age){
    ++hmdCalls63;assert(age==60);x=final63[0];y=final63[1];z=final63[2];
    if(revoke63)npc_lookat_hmd::PublishDialogueOwner63({},false);
    if(flipModeD52){npc_look_policy63::setFollowHmd(false);npc_look_policy63::setFollowHmd(true);}
    return hmd63;
}}
using namespace kcd2vr;
using namespace kcd2vr::npc_lookat_hmd;
static bool original63(void*,void*,float* out){++originalCalls63;if(out){out[0]=0;out[1]=0;out[2]=1.6f;}
    if(stopInOriginal63)kcd2vr::npc_lookat_hmd::Uninstall();return native63;}
static void ck63(bool yes){++checks63;assert(yes);}
template<class T>static void put63(void* p,const T& v){std::memcpy(p,&v,sizeof v);}
int main(int argc,char** argv){
    const bool failEnable63=argc>1&&std::strcmp(argv[1],"enable-fail")==0;
    npc_look_policy63::setFollowHmd(true);
    std::vector<unsigned char> image63(0x4080000);hostModuleHandleW=image63.data();
    const auto base=reinterpret_cast<std::uintptr_t>(image63.data());
    auto* dos=reinterpret_cast<IMAGE_DOS_HEADER*>(image63.data());dos->e_magic=IMAGE_DOS_SIGNATURE;dos->e_lfanew=0x100;
    auto* nt=reinterpret_cast<IMAGE_NT_HEADERS64*>(image63.data()+0x100);nt->Signature=IMAGE_NT_SIGNATURE;nt->FileHeader.TimeDateStamp=0x6A350E20;
    const unsigned char entry63[]{0x48,0x89,0x5C,0x24,0x08,0x48,0x89,0x74,0x24,0x10,0x57,0x48,0x83,0xEC,0x30};
    const unsigned char getter63[]{0x48,0x8B,0x41,0x38,0xC3};
    std::memcpy(image63.data()+0x59B0F0,entry63,sizeof entry63);std::memcpy(image63.data()+0x602360,getter63,sizeof getter63);
    DetourModKit::hook::hostOriginal115=[](const auto& r)->std::uintptr_t{
        return r.name=="kcd2vr_v6510_dialogue_player_look_point"?reinterpret_cast<std::uintptr_t>(&original63):0;};
    retail63=false;ck63(!Install());retail63=true;image63[0x59B0F0]^=1;ck63(!Install());image63[0x59B0F0]^=1;
    hostModulePinAllowed63=false;ck63(!Install()&&!g_original);hostModulePinAllowed63=true;
    if(failEnable63){
        DetourModKit::hook::hostFailEnableNpc63=true;
        SetEnabled(Install());
        ck63(!g_installed&&!g_enabled&&g_retired63&&g_hook.has_value()&&g_original==&original63);
        ck63(image63[0x59B0F0]==0xCC); // failed enable may leave a reachable patch
        dialogue_state59c10r2::State state{};state.tick=hostTickCount64;state.entity=1;state.player=2;state.instance=3;
        state.known=state.active=state.membership.known=state.membership.active=state.inputOwner.known=state.inputOwner.enabled=true;
        state.camera=dialogue_state59c10r2::Camera::FirstPerson;PublishDialogueOwner63(state,true);
        SetEnabled(true);ck63(!Enabled());
        float point[3]{};const auto before=originalCalls63;
        ck63(ResolveLookTargetWorldPosDetour(nullptr,nullptr,point)&&originalCalls63==before+1&&hmdCalls63==0&&point[2]==1.6f);
        Uninstall();SetEnabled(true);ck63(!Enabled()&&g_original==&original63&&g_hook.has_value()&&!Install());
        std::printf("npc_look63 failed-enable: %u actual startup/retired reenable checks PASS; native injection not executed\n",checks63);
        return 0;
    }
    ck63(Install()&&g_installed&&g_original==&original63);ck63(image63[0x59B0F0]==0xCC);
    ck63(hostModulePinAddress63==reinterpret_cast<LPCWSTR>(&ResolveLookTargetWorldPosDetour));
    alignas(8)std::array<unsigned char,0x40> entity63{},other63{},puppet63{},wrapper63{};
    put63(entity63.data(),base+0x4072C78);put63(other63.data(),base+0x4072C78);player63=entity63.data();
    put63(puppet63.data(),base+npc_target47c1::AiPuppet);put63(puppet63.data()+0x38,player63);
    put63(image63.data()+npc_target47c1::AiPuppet+0x18,base+0x602360);
    put63(image63.data()+npc_target47c1::SOPuppet+0x18,base+0x602360);
    void* payload63=puppet63.data();put63(wrapper63.data()+0x18,payload63);
    DialogueSceneFixtureD52 memory(base,reinterpret_cast<std::uintptr_t>(player63));
    auto s=memory.observe(hostTickCount64);
    ck63(s.known&&s.active&&s.membership.sceneD52&&s.membership.playerSoulD52==123);
    auto run63=[&](bool expected){float out[3]{9,9,9};const auto before=originalCalls63;
        ck63(ResolveLookTargetWorldPosDetour(reinterpret_cast<void*>(0x42),wrapper63.data(),out)==native63);
        ck63(originalCalls63==before+1);ck63((out[0]==final63[0]&&out[1]==final63[1]&&out[2]==final63[2])==expected);
        if(!expected)ck63(out[0]==0&&out[1]==0&&out[2]==1.6f);};
    SetEnabled(true);PublishDialogueOwner63(s,true);run63(true);ck63(g_overrides==1);
    // Original false, missing/foreign target, stale/missing native owner and
    // an unclassified camera preserve the native point.
    native63=false;run63(false);native63=true;
    for(int bad=0;bad<11;++bad){auto wrong=s;
        if(bad==0)wrong.tick=hostTickCount64-81;if(bad==1)wrong.known=false;if(bad==2)wrong.membership.active=false;
        if(bad==3)wrong.camera=dialogue_state59c10r2::Camera::Unknown;if(bad==4)wrong.entity=0;
        if(bad==5)wrong.player=0;if(bad==6)wrong.instance=0;if(bad==7)wrong.membership.known=false;
        if(bad==8)wrong.membership.sceneD52=0;if(bad==9)wrong.inputOwner.known=false;if(bad==10)wrong.membership.playerSoulD52=0;
        PublishDialogueOwner63(wrong,true);run63(false);}
    // Gaze is independent of body/embodiment; no body or camera changes occur.
    PublishDialogueOwner63(s,false);run63(true);applied63=false;run63(true);applied63=true;
    hmd63=false;run63(false);hmd63=true;
    SetEnabled(false);run63(false);SetEnabled(true);
    final63[0]=3;run63(false);final63[0]=.1f;
    final63[0]=std::numeric_limits<float>::quiet_NaN();run63(false);final63[0]=.1f;
    void* otherPlayer63=other63.data();put63(puppet63.data()+0x38,otherPlayer63);run63(false);put63(puppet63.data()+0x38,player63);
    put63(puppet63.data(),base+npc_target47c1::SOPuppet);run63(true);
    put63(puppet63.data(),base+0x3a3bc90);run63(false);
    payload63=player63;put63(wrapper63.data()+0x18,payload63);run63(true);
    player63=other63.data();run63(false);player63=entity63.data();
    // Genuine Native is pass-through and makes no HMD query. Scene observation
    // remains independent so switching back during the same conversation works.
    npc_look_policy63::setFollowHmd(false);const auto hmdBefore=hmdCalls63;
    run63(false);ck63(hmdCalls63==hmdBefore);
    const auto modeBefore=npc_look_policy63::modeD52.load();npc_look_policy63::setFollowHmd(false);
    ck63(modeBefore==npc_look_policy63::modeD52.load());
    npc_look_policy63::setFollowHmd(true);run63(true);
    // Actual production observer: disabled speech input means active=false,
    // but same-scene admission survives on fresh membership only.
    memory.mem[DialogueSceneFixtureD52::cameras+0x30]=DialogueSceneFixtureD52::cameras+0x318;
    memory.mem[dialogue_input_fixture19c3::map+9]=0;
    auto d=memory.observe(hostTickCount64);ck63(!d.active&&d.membership.active&&d.inputOwner.known);
    PublishDialogueOwner63(d,false);run63(true);ck63(!d.active);
    // Input-disabled from the start is an unclassified bark/scene, not permission.
    PublishDialogueOwner63({},false);PublishDialogueOwner63(d,false);run63(false);
    // New instance/scene/soul cannot inherit prior conversation admission.
    for(int changed=0;changed<4;++changed){PublishDialogueOwner63(s,true);auto wrong=d;
        if(changed==0)++wrong.instance;if(changed==1)++wrong.membership.sceneD52;
        if(changed==2)++wrong.membership.playerSoulD52;if(changed==3)wrong.inputOwner.known=false;
        PublishDialogueOwner63(wrong,false);run63(false);}
    PublishDialogueOwner63(s,true);PublishDialogueOwner63(d,false);run63(true);
    flipModeD52=true;run63(false);flipModeD52=false;run63(true);
    // Capture Native as Native; enabling diagnostics never turns on HMD behavior.
    using namespace diagnostics::capture168;
    gate.mask=0;auto receiptCount=receipts63.size();const auto callerOffD57=callerReadsD57;
    run63(true);ck63(receipts63.size()==receiptCount);ck63(callerReadsD57==callerOffD57);
    callerD57=reinterpret_cast<void*>(base+0x123456);
    gate.select(Domain::Interaction,true,hostTickCount64);
    npc_look_policy63::setFollowHmd(false);run63(false);
    ck63(receipts63.back().find("reason=MODE_NATIVE")!=std::string::npos);
    ck63(callerReadsD57==callerOffD57+1);
    char nativeFieldsD57[100]{};std::snprintf(nativeFieldsD57,sizeof(nativeFieldsD57),"nativeReturn=0x%llX whBase=0x%llX",
        (unsigned long long)(base+0x123456),(unsigned long long)base);
    ck63(receipts63.back().find(nativeFieldsD57)!=std::string::npos);
    ck63(receipts63.back().find("finalNpcPose=NOT_OBSERVED")!=std::string::npos);
    ck63(accept(receipts63.back().c_str(),hostTickCount64));
    gate.mask=0;gate.select(Domain::Bug,true,hostTickCount64);
    npc_look_policy63::setFollowHmd(true);run63(true);
    ck63(receipts63.back().find("reason=APPLIED")!=std::string::npos);
    ck63(callerReadsD57==callerOffD57+1); // F2 summary does not observe native caller
    ck63(accept(receipts63.back().c_str(),hostTickCount64));
    ck63(!accept("D52 F5_DIALOGUE_GAZE reason=APPLIED",hostTickCount64));
    gate.mask=0;PublishDialogueOwner63(s,true);
    // Revocation during the fresh HMD read cannot borrow the old permission.
    revoke63=true;run63(false);revoke63=false;PublishDialogueOwner63(s,true);run63(true);
    const auto retainedTick=s.tick;hostTickCount64+=81;PublishDialogueOwner63(s,true);ck63(s.tick==retainedTick);run63(false);
    hostTickCount64=s.tick;PublishDialogueOwner63(s,true);
    DetourModKit::hook::hostFailDisable63=argc>1;stopInOriginal63=true;run63(false);stopInOriginal63=false;
    ck63(!g_installed&&!g_enabled&&g_original==&original63&&g_retired63&&g_hook.has_value());
    ck63(image63[0x59B0F0]==(argc>1?0xCC:entry63[0]));
    npc_look_policy63::Owner empty63{};ck63(g_owner63.read(empty63)&&!empty63.entity);
    ck63(!Install());run63(false); // retained reachable callback still forwards exactly once
    SetEnabled(true);ck63(!Enabled());PublishDialogueOwner63(s,true);run63(false);
    std::printf("npc_look63/D52: %u production observer, Native/FollowHMD, speech lifetime, final-write and capture checks PASS; AI native execution untested\n",checks63);
}
