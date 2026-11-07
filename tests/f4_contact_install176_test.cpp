// Real installer/readiness with a synthetic patching hook and retail verifier.
// Proves fail-closed prerequisites, not loaded retail instructions or damage.
#define KCD2VR_TEST_HOOK_SPY115
#include "../src/combat_diag_probe.cpp"
#include <vector>
namespace kcd2vr::diagnostic_trace {void Write(const char*) noexcept {}}
namespace kcd2vr::retail_build112 {
bool verified176=true;
bool Verify(std::uintptr_t b) noexcept {return b&&verified176;}
bool Ready(std::uintptr_t b) noexcept {return b&&verified176;}
}
namespace kcd2vr::frik_pose_probe {
// This passive native probe fixture has no physical fist owner.
int FistContactAdmissionH12(const void*,const float*,int,const void*) noexcept {return -1;}

bool CombatExperiment175Enabled() noexcept {return false;}
bool PhysicalCombat177Enabled() noexcept {return false;} // R55C2 host stub (GNU suite)
bool CombatIntegrated178r22c2Enabled() noexcept {return false;}
int F4ContactGateMode178r26() noexcept {return 1;}
bool GeometryAutoLock178r29() noexcept {return false;} // R29 stub
bool GeometryOwnerValid178r30(std::uint64_t,std::uint64_t,std::uint64_t,std::uint64_t) noexcept {return true;} // R30 stub
bool GeometryAdmittedRelease51(const kcd2vr::contact176::Ticket&,const void*) noexcept {return false;} // installation fixture owns no admitted action
bool GeometryNativeAnswer19c4r2(const void*,const void*,int,kcd2vr::answer59c4r2::Lineage&) noexcept {return false;}
bool GuardOwnedByUs59c7e(const void*) noexcept {return false;}
int GeometryOwnerStage49() noexcept {return 0;} // R49 stub
bool PhysicalWeaponInputOwned6698() noexcept {return false;}
bool ReadF4ContactLive176(kcd2vr::contact176::Live&) noexcept {return false;}
void* LastPlayerEntity() noexcept {return nullptr;}
int WeaponControlMode() noexcept {return 7;}
int FocusedTestMode167() noexcept {return 0;}
void* WeaponControlEntity(int) noexcept {return nullptr;}
void ObserveCollisionAnimationCharacter(void*,std::uint32_t,std::uint64_t) noexcept {}
void NotifyNativeWeaponImpact136R2(float,float,float,bool,bool,bool,std::uint32_t) noexcept {}
}
namespace kcd2vr::animation_filter {
void MarkContactForPlayback(std::uint64_t) noexcept {}
PlaybackStamp DiagnosticPlaybackStamp() noexcept {return {};}
}
using namespace kcd2vr::combat_diag_probe;
static unsigned checks176=0,failures176=0;
static void ck176(bool b,const char* m){++checks176;if(!b){++failures176;std::printf("FAIL %s\n",m);}}
static void Collision176(void*,const void*,const void*,const void*){}
static bool Enrich176(void*,void*,const void*,const void*,const void*,const void*){return false;}
static bool Process176(void*,void*,const void*,void*){return false;}
int main(){
    std::vector<unsigned char> image(0x4200000);hostModuleHandleW=image.data();
    const auto base=reinterpret_cast<std::uintptr_t>(image.data());
    auto* dos=reinterpret_cast<IMAGE_DOS_HEADER*>(image.data());dos->e_magic=IMAGE_DOS_SIGNATURE;dos->e_lfanew=0x100;
    auto* nt=reinterpret_cast<IMAGE_NT_HEADERS64*>(image.data()+0x100);nt->Signature=IMAGE_NT_SIGNATURE;nt->FileHeader.TimeDateStamp=kTimestamp;
    std::memcpy(image.data()+kOnCollisionRva,kEntry,sizeof(kEntry));
    const unsigned char enrichment[]{0x48,0x8B,0xC4,0x48,0x89,0x58,0x10,0x48,0x89,0x70,0x18,0x48,0x89,0x78,0x20,0x48};
    std::memcpy(image.data()+0xD51B38,enrichment,sizeof(enrichment));
    std::memcpy(image.data()+nhp134::kProcessHit,nhp134::kProcessHitEntry,sizeof(nhp134::kProcessHitEntry));
    for(const auto& pin:kcd2vr::contact176::retailPins)std::memcpy(image.data()+pin.rva,pin.bytes.data(),pin.bytes.size());
    DetourModKit::hook::hostOriginal115=+[](const DetourModKit::hook::InlineRequest& r)->std::uintptr_t {
        if(r.name=="kcd2vr_v6689_combat_collision")return reinterpret_cast<std::uintptr_t>(&Collision176);
        if(r.name=="kcd2vr_v6689_collision_enrichment")return reinterpret_cast<std::uintptr_t>(&Enrich176);
        if(r.name=="kcd2vr_v66134_process_collision_hit")return reinterpret_cast<std::uintptr_t>(&Process176);
        return 0;
    };
    namespace cap=kcd2vr::diagnostics::capture168;cap::gate.mask=0;
    kcd2vr::retail_build112::verified176=false;
    ck176(!ArmF4ContactGate176()&&!F4ContactGateReady176(),"full retail verifier rejection prevents F4 admission installation");
    kcd2vr::retail_build112::verified176=true;
    image[0xC52534]^=1;
    ck176(!ArmF4ContactGate176()&&!F4ContactGateReady176(),"wrong native false branch prevents F4 despite optional observers installing");
    ck176((ReadinessMask168r1()&16u)!=0,"Process observer alone is insufficient admission readiness");
    Uninstall();image[0xC52534]^=1;
    ck176(ArmF4ContactGate176()&&F4ContactGateReady176(),"exact publisher/caller pins and owned Process hook arm F4 without F3");
    ck176(cap::gate.mask.load()==0&&!Enabled(),"gameplay hook readiness never starts a recorder");
    for(const auto& pin:kcd2vr::contact176::retailPins){
        image[pin.rva]^=1;ck176(!F4ContactGateReady176(),"every native call-chain pin is rechecked at F4 activation");
        image[pin.rva]^=1;ck176(F4ContactGateReady176(),"restored exact pin retains ready owned hook");
    }
    image[nhp134::kProcessHit]^=1;ck176(!F4ContactGateReady176(),"foreign Process patch cannot impersonate owned gate");
    image[nhp134::kProcessHit]^=1;
    auto original=nhp134::g_process_original;nhp134::g_process_original=nullptr;
    ck176(!F4ContactGateReady176(),"missing original trampoline prevents readiness");nhp134::g_process_original=original;
    hostModuleHandleW=nullptr;ck176(!F4ContactGateReady176(),"different or absent game module cannot reuse contract");hostModuleHandleW=image.data();
    kcd2vr::retail_build112::verified176=false;ck176(!F4ContactGateReady176(),"revoked retail qualification prevents readiness");
    kcd2vr::retail_build112::verified176=true;
    ck176(F4ContactGateReady176()&&nhp134::g_base==base,"current exact module remains qualified");
    Uninstall();ck176(!F4ContactGateReady176(),"uninstall revokes gameplay gate readiness");
    ck176(!std::memcmp(image.data()+nhp134::kProcessHit,nhp134::kProcessHitEntry,sizeof(nhp134::kProcessHitEntry)),"uninstall restores the owned native Process entry");
    std::printf("f4_contact_install176=%u/%u production_installer=YES retail_and_hook_leaves=REPLACED\n",checks176-failures176,checks176);
    return failures176?1:0;
}
