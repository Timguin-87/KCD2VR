// Production entry qualification, hook lifecycle, rigid pins and static
// pickup getter execute below. Native memory, hook patching and getter result
// are explicit leaves; no game DLL is loaded.
#include "../src/vrik/owned_interaction_body_entry_r93.h"
#include <array>
#include <map>
#include <vector>
#include <optional>
#include <string>
#include <iostream>
#include <cstring>
#include <utility>
namespace entry=kcd2vr::interaction_body_entry_r93;
static constexpr std::uintptr_t baseR93=0x180000000ull;
static constexpr std::uintptr_t kCEntityVftRva=0x4072C78;
static std::map<std::uintptr_t,std::vector<unsigned char>> memoryR93;
static bool fingerprintR93=true,enableR93=true;
static unsigned getterCallsR93=0;
static void* returnedBodyR93=nullptr;
static std::uintptr_t WhBase(){return baseR93;}
namespace retail_build112 {static bool Ready(std::uintptr_t b){return fingerprintR93&&b==baseR93;}}
static bool copyR93(std::uintptr_t address,void* out,std::size_t n){
    for(auto& [at,bytes]:memoryR93)if(address>=at&&address-at<=bytes.size()&&n<=bytes.size()-(address-at)){
        std::memcpy(out,bytes.data()+(address-at),n);return true;}
    return false;
}
template<class T>static bool Read(const void* p,T& value){return copyR93(reinterpret_cast<std::uintptr_t>(p),&value,sizeof(value));}
static bool PhysVft6698(void* p,std::uintptr_t rva){std::uintptr_t v=0;return p&&Read(p,v)&&v==baseR93+rva;}
static void bytesR93(std::uintptr_t p,const void* bytes,std::size_t n){memoryR93[p]=std::vector<unsigned char>(static_cast<const unsigned char*>(bytes),static_cast<const unsigned char*>(bytes)+n);}
template<class T>static void valueR93(std::uintptr_t p,T value){bytesR93(p,&value,sizeof(value));}
static void* __fastcall originalR93(void*){++getterCallsR93;return returnedBodyR93;}
static void* __fastcall otherR93(void*){return nullptr;}
static void* __fastcall InteractionBodyDetour122(void*);
namespace DetourModKit {
struct Address{std::uintptr_t value;};
namespace hook {
struct Options{bool fail_if_already_hooked=false;};
struct Target{Address address;};
struct InlineRequest{const char* name;Target target;Options options;};
struct Hook {
    bool on=false;
    template<class T>T original(){return reinterpret_cast<T>(&originalR93);}
    bool enable(){if(!enableR93)return false;on=true;
        unsigned char patch[16]{0xFF,0x25,0,0,0,0,0x19,0x21,0x34,0x56,0,0,0,0,0x90,0x90};
        bytesR93(baseR93+entry::kEntry,patch,sizeof(patch));return true;}
    bool is_enabled()const{return on;}
    bool disable(){on=false;bytesR93(baseR93+entry::kEntry,entry::kPristine,sizeof(entry::kPristine));return true;}
};
static std::optional<Hook> inline_at(const InlineRequest& request,void*(__fastcall*)(void*)){
    if(request.target.address.value!=baseR93+entry::kEntry||!request.options.fail_if_already_hooked||!entry::pristine(baseR93,copyR93))return {};
    return Hook{};
}
}}
struct WatchR93{};static kcd2vr::core::SnapshotMailbox<WatchR93> g_interaction_watch122;
// Generated unchanged from current production functions; only the native
// SmallPickupBody leaf is replaced explicitly by originalR93 for this host.
#include "interaction_body_entry_r93_actual.inc"
static void* __fastcall InteractionBodyDetour122(void* p){return g_interaction_body_original122(p);}
static unsigned checksR93=0;
#define C93(c) do{++checksR93;if(!(c)){std::cerr<<"FAIL_R93 line="<<__LINE__<<" " #c "\n";return 1;}}while(0)
static void seedR93(){
    memoryR93.clear();fingerprintR93=true;enableR93=true;getterCallsR93=0;returnedBodyR93=nullptr;
    for(const auto& pin:kPropPhysics5916C1)bytesR93(baseR93+pin.rva,pin.bytes.data(),pin.size);
    bytesR93(baseR93+entry::kCallsite,entry::kCall,sizeof(entry::kCall));
    valueR93(baseR93+entry::kMethodSlot,baseR93+entry::kEntry);
    valueR93(0x11110000,baseR93+kCEntityVftRva);
    valueR93(0x22220000,baseR93+0x3A578C0); // static CPhysicalEntity
    valueR93(0x33330000,baseR93+0x3A48DC0); // rigid CRigidEntity
}
int main(){
    seedR93();void* result=reinterpret_cast<void*>(1);
    C93(InteractionBodyEntryQualifiedR93(baseR93)&&PropPhysicsPins5916C1());
    C93(!InteractionBodyEntryQualifiedR93(0)&&!InteractionBodyEntryQualifiedR93(baseR93+0x1000));
    returnedBodyR93=reinterpret_cast<void*>(0x22220000);
    C93(SmallPickupBodyR24(reinterpret_cast<void*>(0x11110000),result)&&result==returnedBodyR93);
    C93(ArmInteractionBody122()&&g_interaction_body_hook122->is_enabled());
    C93(!entry::pristine(baseR93,copyR93));
    // The armed hook remains authentic whether F5 is currently recording or
    // idle. No diagnostic setting is authority in these production functions.
    for(int diagnostics:{0,1,2,0}){(void)diagnostics;
        C93(InteractionBodyEntryQualifiedR93(baseR93)&&PropPhysicsPins5916C1());
        returnedBodyR93=reinterpret_cast<void*>(0x22220000);
        C93(SmallPickupBodyR24(reinterpret_cast<void*>(0x11110000),result)&&result==returnedBodyR93);
        returnedBodyR93=reinterpret_cast<void*>(0x33330000);
        C93(SmallPickupBodyR24(reinterpret_cast<void*>(0x11110000),result)&&result==returnedBodyR93&&PropPhysicsPins5916C1());
    }
    const auto owned=memoryR93[baseR93+entry::kEntry];
    memoryR93[baseR93+entry::kEntry][7]^=0x40;
    C93(!InteractionBodyEntryQualifiedR93(baseR93)&&!PropPhysicsPins5916C1()&&!ArmInteractionBody122());
    const auto calls=getterCallsR93;
    C93(!SmallPickupBodyR24(reinterpret_cast<void*>(0x11110000),result)&&!result&&getterCallsR93==calls);
    memoryR93[baseR93+entry::kEntry]=owned;
    g_interaction_body_original122=&otherR93;C93(!InteractionBodyEntryQualifiedR93(baseR93));
    g_interaction_body_original122=&originalR93;
    g_interaction_body_hook122->on=false;C93(!InteractionBodyEntryQualifiedR93(baseR93));g_interaction_body_hook122->on=true;
    fingerprintR93=false;C93(!InteractionBodyEntryQualifiedR93(baseR93)&&!PropPhysicsPins5916C1());fingerprintR93=true;
    valueR93(baseR93+entry::kMethodSlot,baseR93+entry::kEntry+1);
    C93(!InteractionBodyEntryQualifiedR93(baseR93));valueR93(baseR93+entry::kMethodSlot,baseR93+entry::kEntry);
    memoryR93[baseR93+entry::kCallsite][1]^=1;
    C93(!InteractionBodyEntryQualifiedR93(baseR93));memoryR93[baseR93+entry::kCallsite][1]^=1;
    for(const auto& pin:kPropPhysics5916C1)if(pin.rva!=entry::kEntry){
        memoryR93[baseR93+pin.rva][pin.size-1]^=1;C93(!PropPhysicsPins5916C1());memoryR93[baseR93+pin.rva][pin.size-1]^=1;
    }
    C93(PropPhysicsPins5916C1());ShutdownInteractionBody122();
    C93(!g_interaction_body_hook122&&!g_interaction_body_original122&&entry::pristine(baseR93,copyR93));
    C93(InteractionBodyEntryQualifiedR93(baseR93)&&PropPhysicsPins5916C1());
    memoryR93[baseR93+entry::kEntry]=owned; // same-looking bytes without OUR receipt
    C93(!InteractionBodyEntryQualifiedR93(baseR93)&&!PropPhysicsPins5916C1()&&!ArmInteractionBody122());
    seedR93();C93(ArmInteractionBody122());g_interaction_body_entry_r93.clear();
    C93(!InteractionBodyEntryQualifiedR93(baseR93));ShutdownInteractionBody122();
    // No published receipt if hook enable failed. The disabled handle itself
    // must never qualify arbitrary bytes; shutdown restores the pristine path.
    seedR93();enableR93=false;C93(!ArmInteractionBody122());
    C93(!InteractionBodyEntryQualifiedR93(baseR93));ShutdownInteractionBody122();
    C93(InteractionBodyEntryQualifiedR93(baseR93));
    std::cout<<"R93 owned body getter: "<<checksR93<<" checks passed; native memory/hook/getter are explicit leaves\n";
}
