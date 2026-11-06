// Executes the actual matcher, helper wrapper, final callsite callbacks and
// install/shutdown path. Native interpolation/application are explicit doubles.
#define KCD2VR_TEST_HOOK_SPY115
#define KCD2VR_NPC_LOOK_TEST63
#include <windows.h>
#include <DetourModKit/hook.hpp>
#include "../src/core/door_motion_native66.h"
#include <array>
#include <atomic>
#include <cassert>
#include <cstdio>
#include <cstring>
#include <optional>
#include <string>
#include <vector>
static std::uintptr_t base66=0;static bool retail66=true,owned66=true,stop66=false,expire66=false,nest66=false;
static unsigned checks66=0,original66=0,animated66=0,entity66=0,events66=0;
static int mutation66=0;
static std::uint64_t serial66=19;static std::atomic<void*> g_last_player_entity{nullptr};
static std::vector<std::string> receipts66;
static std::uintptr_t WhBase(){return base66;}static bool SupportedWhBuild(std::uintptr_t b){return b==base66&&retail66;}
namespace retail_build112 {static bool Ready(std::uintptr_t b){return b==base66&&retail66;}}
namespace diagnostic_trace {static void Write(const char* s){receipts66.emplace_back(s);}}
namespace kcd2vr::door66 {
bool OwnedMotionFor(std::uintptr_t p,std::uint32_t id,std::uintptr_t target,std::uint64_t now,std::uint64_t* serial) noexcept {
    if(serial)*serial=serial66;return owned66&&p==reinterpret_cast<std::uintptr_t>(g_last_player_entity.load())&&id==42&&target&&now==hostTickCount64;
}}
static void* hostReturn66(){return nullptr;}
#define _ReturnAddress hostReturn66
#include "../src/vrik/door_motion_native66.inl"
#undef _ReturnAddress
template<class T>static void put66(void* p,std::size_t offset,T value){std::memcpy(static_cast<unsigned char*>(p)+offset,&value,sizeof(value));}
template<std::size_t N>static std::uintptr_t at66(std::array<unsigned char,N>& a){return reinterpret_cast<std::uintptr_t>(a.data());}
static void ck66(bool b){++checks66;assert(b);}
struct Native66 {
    std::array<unsigned char,0x180> clip{},scope{},action{};
    std::array<unsigned char,0x100> controller{},entity{},target{},actor{};
    std::array<unsigned char,0x200> animated{};
    std::array<unsigned char,0x80> helper{},scopeContext{};
    std::array<unsigned char,0x40> context{},definition{},fragments{},entries{},character{},parent{};
    char label[11]="door_r_f_o";
    void reset(){
        *this=Native66{};const auto ent=at66(entity),sc=at66(scope),ctrl=at66(controller),act=at66(action),ctx=at66(context);
        put66(clip.data(),0,base66+0x3AB60A8);put66(clip.data(),8,ent);put66(clip.data(),0x10,at66(character));put66(clip.data(),0x18,sc);
        put66(clip.data(),0xA0,at66(animated));
        put66(scope.data(),0,base66+0x3AEA100);put66(scope.data(),0x20,at66(scopeContext));put66(scope.data(),0x28,ctrl);put66(scope.data(),0x148,act);
        put66(scopeContext.data(),0x20,ent);put66(scopeContext.data(),0x10,at66(character));
        put66(controller.data(),0,base66+0x40519E8);put66(controller.data(),0x20,ctx);put66(controller.data(),0x30,17u);
        put66(controller.data(),0x34,1u);put66(controller.data(),0x38,sc);
        put66(entity.data(),0,base66+0x4072C78);put66(entity.data(),0xC,17u);put66(target.data(),0,base66+0x4072C78);put66(target.data(),0xC,42u);
        put66(action.data(),0,base66+0x3ADA108);put66(action.data(),8,ctx);put66(action.data(),0x28,2);put66(action.data(),0x30,sc);
        put66(action.data(),0x38,0);put66(action.data(),0x54,9u);put66(action.data(),0x90,at66(actor));put66(action.data(),0x98,at66(target));
        put66(action.data(),0xA0,42u);put66(action.data(),0xC0,at66(helper));put66(actor.data(),0x38,ent);
        put66(helper.data(),0,std::uint64_t(10));put66(helper.data(),0x10,reinterpret_cast<std::uintptr_t>(label));
        put66(helper.data(),0x54,42u);put66(helper.data(),0x60,act);put66(animated.data(),0,base66+0x3A49468);
        put66(context.data(),0,at66(definition));put66(definition.data(),0x18,at66(fragments));put66(fragments.data(),8,at66(entries)+4);
        put66(entries.data(),0,1);put66(entries.data(),0x14,dmn66::AnimationControlledCrc);
        g_last_player_entity=reinterpret_cast<void*>(ent);owned66=true;serial66=19;
    }
};
static Native66 native66;
static void originalApply66(void* state,void* entity,float dt,bool,void*){
    ++original66;++events66;float elapsed=0;std::memcpy(&elapsed,static_cast<unsigned char*>(state)+0x54,4);elapsed+=dt;
    std::memcpy(static_cast<unsigned char*>(state)+0x54,&elapsed,4);
    if(stop66)DoorMotionNativeShutdown66();if(expire66)owned66=false;
    if(mutation66==1)put66(native66.action.data(),0x54,10u);
    if(mutation66==2)++serial66;
    if(mutation66==3)put66(native66.helper.data(),0x60,std::uintptr_t(0));
    if(mutation66==4)put66(native66.action.data(),0xA0,43u);
    if(nest66){nest66=false;DoorMotionApplyBody66(state,entity,dt,false,nullptr,base66+0x36E5655);}
    using namespace DetourModKit::hook;
    MidContext a{};gpr(a,Gpr::Rcx)=at66(native66.animated);gpr(a,Gpr::Rax)=base66+0x3A49468;
    gpr(a,Gpr::Rdi)=reinterpret_cast<std::uintptr_t>(state);gpr(a,Gpr::Rsi)=reinterpret_cast<std::uintptr_t>(entity);
    DoorMotionAnimatedMid66(a);if(a.ip!=base66+dmn66::AnimatedAfter)++animated66;
    MidContext e{};gpr(e,Gpr::Rcx)=gpr(e,Gpr::Rsi)=reinterpret_cast<std::uintptr_t>(entity);gpr(e,Gpr::Rbx)=base66+0x509810;
    DoorMotionEntityMid66(e);if(e.ip!=base66+dmn66::EntityAfter)++entity66;
}
int main(int argc,char** argv){
    const bool fail=argc>1&&std::strcmp(argv[1],"failed-enable")==0;
    std::vector<unsigned char> image(0x4280000);base66=reinterpret_cast<std::uintptr_t>(image.data());hostModuleHandleW=image.data();
    for(const auto& p:dmn66::Pins)std::memcpy(image.data()+p.rva,p.bytes,p.size);
    put66(image.data(),0x3A49468+0x2F0,base66+0x8447B0);
    DetourModKit::hook::hostOriginal115=[](const auto& r)->std::uintptr_t{return r.name=="kcd2vr_c66_door_adjustment_context"?reinterpret_cast<std::uintptr_t>(&originalApply66):0;};
    native66.reset();auto read=DoorMotionCopy66;dmn66::Root r{};
    ck66(dmn66::readRoot(read,base66,at66(native66.clip)+0x40,at66(native66.entity),r));
    ck66(r.actor==at66(native66.actor)&&r.entity==at66(native66.entity)&&r.target==42&&r.targetEntity==at66(native66.target));
    // Exact layout/source negatives execute the production matcher.
    for(int bad=0;bad<24;++bad){native66.reset();
        if(bad==0)put66(native66.clip.data(),0,base66+1);if(bad==1)put66(native66.scope.data(),0,base66+1);
        if(bad==2)put66(native66.controller.data(),0,base66+1);if(bad==3)put66(native66.action.data(),0,base66+1);
        if(bad==4)put66(native66.action.data(),0x28,1);if(bad==5)put66(native66.action.data(),0x30,std::uintptr_t(0));
        if(bad==6)put66(native66.action.data(),8,std::uintptr_t(0));if(bad==7)put66(native66.action.data(),0x38,1);
        if(bad==8)put66(native66.action.data(),0xA0,43u);if(bad==9)put66(native66.target.data(),0xC,43u);
        if(bad==10)put66(native66.actor.data(),0x38,at66(native66.target));if(bad==11)put66(native66.helper.data(),0x60,std::uintptr_t(0));
        if(bad==12)put66(native66.helper.data(),0x54,43u);if(bad==13)native66.label[0]='k';
        if(bad==14)put66(native66.helper.data(),0,std::uint64_t(9));if(bad==15)put66(native66.scopeContext.data(),0x20,at66(native66.target));
        if(bad==16)put66(native66.scopeContext.data(),0x10,std::uintptr_t(0));if(bad==17)put66(native66.controller.data(),0x30,43u);
        if(bad==18)put66(native66.controller.data(),0x34,0u);if(bad==19)put66(native66.entries.data(),0x14,0x03E46E25u);
        if(bad==20)put66(native66.animated.data(),0,base66+1);
        if(bad==21){put66(native66.entity.data(),0x98,at66(native66.parent));put66(native66.parent.data(),0x18,at66(native66.target));}
        if(bad==22)put66(native66.action.data(),0xC0,std::uintptr_t(0));if(bad==23)put66(native66.scope.data(),0x148,std::uintptr_t(0));
        ck66(!dmn66::readRoot(read,base66,at66(native66.clip)+0x40,at66(native66.entity),r));
    }
    native66.reset();std::uintptr_t fault=at66(native66.action)+0xC0;
    auto failRead=[&](std::uintptr_t a,void* out,std::size_t n){return a!=fault&&DoorMotionCopy66(a,out,n);};
    ck66(!dmn66::readRoot(failRead,base66,at66(native66.clip)+0x40,at66(native66.entity),r));
    DoorMotionNativeShutdown66();ck66(!g_door_motion_retired66&&!g_door_motion_original66&&!g_door_motion_apply_hook66);
    retail66=false;ck66(!DoorMotionNativeArm66());retail66=true;image[dmn66::AnimatedCall]^=1;
    ck66(!DoorMotionNativeArm66());image[dmn66::AnimatedCall]^=1;hostModulePinAllowed63=false;ck66(!DoorMotionNativeArm66());hostModulePinAllowed63=true;
    if(fail)DetourModKit::hook::hostFailEnableNpc63=true;
    ck66(DoorMotionNativeArm66()!=fail);ck66(g_door_motion_original66==&originalApply66);
    auto run=[&](unsigned expected,std::uintptr_t caller=0){const auto beforeA=animated66,beforeE=entity66,beforeO=original66,beforeV=events66;
        float beforeT=0;std::memcpy(&beforeT,native66.clip.data()+0x94,4);
        DoorMotionApplyBody66(native66.clip.data()+0x40,native66.entity.data(),.016f,false,nullptr,caller?caller:base66+dmn66::ApplyCaller);
        float afterT=0;std::memcpy(&afterT,native66.clip.data()+0x94,4);
        ck66(original66==beforeO+1&&events66==beforeV+1);ck66(afterT>beforeT);ck66(animated66==beforeA+expected&&entity66==beforeE+expected);ck66(!g_door_motion_tls66.owned);};
    if(fail){ck66(g_door_motion_retired66&&!g_door_motion_enabled66);run(1);ck66(!DoorMotionNativeArm66());DoorMotionNativeShutdown66();ck66(g_door_motion_original66==&originalApply66);}
    else{
        run(0);owned66=false;run(1);owned66=true;run(1,base66+0x36E5655);
        expire66=true;run(1);expire66=false;owned66=true;
        put66(native66.action.data(),0x28,1);run(1);put66(native66.action.data(),0x28,2);
        for(mutation66=1;mutation66<=4;++mutation66){native66.reset();run(1);}mutation66=0;native66.reset();
        // Final callsites themselves cannot authorize an unrelated register
        // recipient or method, even with an otherwise current wrapper lease.
        ck66(DoorMotionRoot66(at66(native66.clip)+0x40,at66(native66.entity),g_door_motion_tls66.root,g_door_motion_tls66.serial));
        g_door_motion_tls66.owned=true;
        for(int bad=0;bad<4;++bad){using namespace DetourModKit::hook;MidContext c{};
            gpr(c,Gpr::Rcx)=at66(native66.animated);gpr(c,Gpr::Rax)=base66+0x3A49468;
            gpr(c,Gpr::Rdi)=at66(native66.clip)+0x40;gpr(c,Gpr::Rsi)=at66(native66.entity);
            if(bad<2){const Gpr field[]{Gpr::Rdi,Gpr::Rsi};gpr(c,field[bad])=0;DoorMotionAnimatedMid66(c);}
            else{gpr(c,Gpr::Rcx)=gpr(c,Gpr::Rsi)=at66(native66.entity);gpr(c,Gpr::Rbx)=base66+0x509810;
                const Gpr field[]{Gpr::Rsi,Gpr::Rbx};gpr(c,field[bad-2])=0;DoorMotionEntityMid66(c);}
            ck66(c.ip==0);
        }g_door_motion_tls66={};
        // A nested foreign helper cannot inherit the outer accepted Door TLS.
        const auto a=animated66,e=entity66,o=original66;nest66=true;
        DoorMotionApplyBody66(native66.clip.data()+0x40,native66.entity.data(),.016f,false,nullptr,base66+dmn66::ApplyCaller);
        ck66(original66==o+2&&animated66==a+1&&entity66==e+1&&!g_door_motion_tls66.owned);
        DetourModKit::hook::hostFailDisable63=true;stop66=true;run(1);stop66=false;
        ck66(!g_door_motion_enabled66&&g_door_motion_retired66&&g_door_motion_apply_hook66.has_value()&&g_door_motion_original66==&originalApply66);
        run(1);ck66(!DoorMotionNativeArm66());
    }
    std::printf("door_motion_native66: %u actual matcher/helper/callsite/lifecycle checks PASS; no native game execution\n",checks66);
}
