// Compile the production menu-state, fill/apply and cycle leaves, not a duplicate policy.
#include <algorithm>
#include <atomic>
#include <cstdio>
#include <cstring>
#include <cwchar>
#include <map>
#include <string>
namespace kcd2vr {
namespace gorepak50 {std::atomic<bool> dismember{false};std::atomic<int> state{0};}
namespace gore32 {std::atomic<bool> decap{false};std::atomic<int> decapStyle44{2};int clampDecapStyle46(int x){return std::clamp(x,0,2);}}
namespace impale40 {std::atomic<int> mode{0};}
}
static std::atomic<bool> g_decapitation59c7e{false};
static std::atomic<int> g_impale_hang32{2},g_impale_hang_on45{2};
static float g_decapitation_min_energy59c7e=.6f;
static void ApplyImpaleHang32(int x){g_impale_hang32=x;}
namespace combat_diag_probe {static bool enabled=false;static float energy=0;void SetDecapitation59c7e(bool v,float e){enabled=v;energy=e;}}
struct VrMenuState {
#define KCD2VR_M2_HOST_SLOT 277
#include "subsystems/gore/host.inl"
#undef KCD2VR_M2_HOST_SLOT
int exp_impale40=0;
};
#define KCD2VR_M2_HOST_SLOT 5022
#include "subsystems/gore/host.inl"
#undef KCD2VR_M2_HOST_SLOT
static VrMenuState Fill(){VrMenuState out;
#define KCD2VR_M2_HOST_SLOT 30
#include "subsystems/gore/host.inl"
#undef KCD2VR_M2_HOST_SLOT
return out;}
static void Apply(const VrMenuState& state){
#define KCD2VR_M2_HOST_SLOT 78
#include "subsystems/gore/host.inl"
#undef KCD2VR_M2_HOST_SLOT
#define KCD2VR_M2_HOST_SLOT 82
#include "subsystems/gore/host.inl"
#undef KCD2VR_M2_HOST_SLOT
}
static unsigned checks=0,failures=0;
static void ck(bool ok,const char* why){++checks;if(!ok){++failures;std::printf("FAIL %s\n",why);}}
int main(){
    VrMenuState initial;ck(!initial.decapitation&&!initial.dismemberment_d80,"both missing preferences default OFF");
    for(int style=0;style<3;++style){
        auto s=initial;s.decap_style44=style;
        CycleDecap44(s);ck(s.decapitation&&!s.dismemberment_d80,"OFF to HEADS ONLY");
        ck(std::wcscmp(DecapLabel44(s),L"GORE: HEADS ONLY")==0,"heads label");
        Apply(s);auto out=Fill();ck(out.decapitation&&!out.dismemberment_d80,"heads choice reaches runtime and returns in menu");
        ck(combat_diag_probe::enabled&&combat_diag_probe::energy==.6f,"existing decap delivery and energy preserved");
        CycleDecap44(s);ck(s.decapitation&&s.dismemberment_d80,"HEADS ONLY to HEADS AND LIMBS");
        ck(std::wcscmp(DecapLabel44(s),L"GORE: HEADS + LIMBS")==0,"combined label");
        Apply(s);out=Fill();ck(out.decapitation&&out.dismemberment_d80,"combined choice reaches runtime and returns in menu");
        ck(std::strstr(DecapTrace46(s,"TEST"),"VrDismemberment=ON")!=nullptr,"trace names actual saved limb control");
        CycleDecap44(s);ck(!s.decapitation&&!s.dismemberment_d80,"combined to OFF disables both");
        Apply(s);out=Fill();ck(!out.decapitation&&!out.dismemberment_d80&&!combat_diag_probe::enabled,"OFF reaches both runtime gates");
        ck(s.decap_style44==style&&out.decap_style44==style,"all three existing head presentation styles survive cycle");
    }
    for(bool decap:{false,true})for(bool limbs:{false,true}){
        g_decapitation59c7e=decap;kcd2vr::gorepak50::dismember=limbs;
        auto s=Fill();ck(s.decapitation==decap&&s.dismemberment_d80==limbs,"existing independent preferences read unchanged");
        Apply(s);ck(g_decapitation59c7e==decap&&kcd2vr::gorepak50::dismember==limbs,"unrelated menu apply does not reset existing independent values");
        if(!decap&&limbs){ck(std::wcscmp(DecapLabel44(s),L"LIMBS ONLY (SAVED)")==0,"legacy limbs-only state never falsely claims OFF");CycleDecap44(s);ck(s.decapitation&&!s.dismemberment_d80,"explicit click normalizes legacy state into ordinary cycle");}
    }
    kcd2vr::gorepak50::state=2;
    VrMenuState missing;ck(std::wcscmp(DecapLabel44(missing),L"GORE: OFF")==0,"missing optional pak never enables gore");
    CycleDecap44(missing);ck(std::wcsstr(DecapLabel44(missing),L"HEADS ONLY")&&std::wcsstr(DecapLabel44(missing),L"NO GORE PAK"),"missing-pak warning keeps selected mode visible");
    CycleDecap44(missing);ck(std::wcsstr(DecapLabel44(missing),L"HEADS + LIMBS")&&std::wcsstr(DecapLabel44(missing),L"NO GORE PAK"),"combined missing-pak warning truthful");
    std::printf("gore_menu_d80 %u/%u checks PASS\n",checks-failures,checks);return failures?1:0;
}
