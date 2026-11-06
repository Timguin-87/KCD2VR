// Executes the ACTUAL owned native adapter + clock client. Only retail leaves
// and memory transport are explicit mocks; never loads/executes WHGame.dll.
#include "../src/subsystems/ranged_reload/crossbow_native_xr34.h"
#include "../src/diagnostics/activity_links_d77.h"
#include "../src/core/mutual_admission_t25.h"
#include "../src/subsystems/throw_drop/hand_claim_t24.h"
#include "../src/native_weapon_body_t4.h"
#include <array>
#include <atomic>
#include <cassert>
#include <cstring>
#include <cstdlib>
#include <map>
#include <vector>
#include <iostream>
#include <mutex>
#include <thread>
#include <limits>
// Actual production read-only getter with explicit fixture atomic storage.
// Do not substitute a constant-free decision: bit4 must stay distinct from8.
namespace kcd2vr::native_weapon_body_t4 {
namespace {std::atomic<unsigned> control{0};}
// BEGIN ACTUAL_H36_BIT4_GETTER -- mechanically compared by external runner
bool EquipmentAdmissionSuspendedH36() noexcept {return (control.load(std::memory_order_acquire)&4u)!=0;}
// END ACTUAL_H36_BIT4_GETTER
}
static void equipmentSuspendedH36Fixture(bool value) noexcept {
    auto& word=kcd2vr::native_weapon_body_t4::control;
    if(value)word.fetch_or(4u,std::memory_order_release);else word.fetch_and(~4u,std::memory_order_release);
}
// Storage/native-lifetime fields are explicit fixture leaves; the actual
// query policy below is copied verbatim, including its original-owner join.
namespace kcd2vr::frik_pose_probe {
enum class ThrowAuthorityT22 : unsigned char { Unknown, Allowed, Denied };
struct LeftEquipmentObligationH36 {struct {int side=1;std::uint64_t bodyEpoch=0,epoch=0;} command;
    void* player=nullptr;void* manager=nullptr;std::uintptr_t character=0;bool active=false;};
static std::atomic<void*> g_henry_item_attach_manager{},g_committed_character6689{};
static std::atomic<std::uintptr_t> g_player_char37{};
static std::atomic<std::uint64_t> g_pose_epoch6689{},g_weapon_request_epoch6694{};
static std::recursive_mutex g_phys_native_mutex6698;
static LeftEquipmentObligationH36 g_left_equipment_obligation_h36;
static std::array<LeftEquipmentObligationH36,7> g_left_equipment_successors_h36{};
static bool g_left_equipment_overflow_h36=false;
static std::atomic<bool> g_native_held_equipment_pending_h36{};
// BEGIN ACTUAL_RETAINED_EQUIPMENT_QUERY -- mechanically compared by runner
ThrowAuthorityT22 ThrowEquipmentAdmissionT25(int side,std::uintptr_t player,std::uintptr_t manager,
    std::uintptr_t character,std::uint64_t body,std::uint64_t request) noexcept {
    if(side<0||side>1||!player||!manager||!character||!body||!request)return ThrowAuthorityT22::Unknown;
    const auto current=[&]() noexcept {
        return manager==reinterpret_cast<std::uintptr_t>(g_henry_item_attach_manager.load(std::memory_order_acquire))&&
            character==reinterpret_cast<std::uintptr_t>(g_committed_character6689.load(std::memory_order_acquire))&&
            character==g_player_char37.load(std::memory_order_acquire)&&
            body==g_pose_epoch6689.load(std::memory_order_acquire)&&request==g_weapon_request_epoch6694.load(std::memory_order_acquire);
    };
    if(!current())return ThrowAuthorityT22::Unknown;
    // H36 currently owns LEFT Draw/Stow only; do not mask the opposite hand.
    if(side==0)return current()?ThrowAuthorityT22::Allowed:ThrowAuthorityT22::Unknown;
    std::unique_lock<std::recursive_mutex> lock(g_phys_native_mutex6698,std::try_to_lock);
    if(!lock.owns_lock()||!current())return ThrowAuthorityT22::Unknown;
    bool debt=false,unknown=g_left_equipment_overflow_h36;
    const auto observe=[&](const LeftEquipmentObligationH36& o) noexcept {
        if(!o.active)return;
        if(o.command.side!=1||!o.player||!o.manager||!o.character){unknown=true;return;}
        if(reinterpret_cast<std::uintptr_t>(o.player)==player&&reinterpret_cast<std::uintptr_t>(o.manager)==manager)debt=true;
        else unknown=true;
    };
    observe(g_left_equipment_obligation_h36);for(const auto& o:g_left_equipment_successors_h36)observe(o);
    const bool pending=g_native_held_equipment_pending_h36.load(std::memory_order_acquire);
    if(!current())return ThrowAuthorityT22::Unknown;
    if(debt)return ThrowAuthorityT22::Denied; // old body/request cannot retire original work
    if(unknown||pending)return ThrowAuthorityT22::Unknown;
    return ThrowAuthorityT22::Allowed;
}
// END ACTUAL_RETAINED_EQUIPMENT_QUERY
}
#define KCD2VR_XR34_HOST_TEST 1
namespace x=kcd2vr::crossbow_native_xr34;
struct Fixture;
static Fixture* f=nullptr;
static constexpr std::uintptr_t base=0x180000000ull;
static void currentEquipmentH36Fixture(const x::Source& s) noexcept {
    namespace h=kcd2vr::frik_pose_probe;
    h::g_henry_item_attach_manager=reinterpret_cast<void*>(s.manager);
    h::g_committed_character6689=reinterpret_cast<void*>(s.character);h::g_player_char37=s.character;
    h::g_pose_epoch6689=s.body;h::g_weapon_request_epoch6694=s.request;
}
static void clearEquipmentH36Fixture(const x::Source& s) noexcept {
    namespace h=kcd2vr::frik_pose_probe;currentEquipmentH36Fixture(s);
    h::g_left_equipment_obligation_h36={};h::g_left_equipment_successors_h36={};
    h::g_left_equipment_overflow_h36=false;h::g_native_held_equipment_pending_h36=false;
    kcd2vr::native_weapon_body_t4::control=0;
    namespace a=kcd2vr::ammo_carry_t18;a::hands=0;for(auto& claim:a::claims)claim.publish({});
}

struct Fixture {
    std::array<unsigned char,0x200> controller{},context{},scope{},main{},parent{},child{},equipment{},item{},skeleton{},set{};
    std::array<unsigned char,0xC00> character{};
    std::array<unsigned char,0x100> player{},caf{};
    std::array<unsigned char,0x110> entryStorage{};
    std::uintptr_t entry() const {return p(entryStorage)+16;}
    std::array<unsigned char,0x160> eventStorage{};
    std::array<std::uintptr_t,1> queued{};
    std::vector<std::pair<std::uintptr_t,std::size_t>> blocks;
    std::map<std::uintptr_t,std::vector<unsigned char>> pins;
    x::Source source{};x::Spatial spatial{};
    std::uint64_t now=1000;float remaining=5;
    bool current=true,ownerThreadD72=true,readFailure=false,writeFailure=false,partialWrite=false;
    int writes=0,starts=0,stops=0,spawns=0,cocks=0,loads=0,finals=0,scans=0,originalEvents=0;
    bool traceReadsD69=false;std::vector<std::pair<std::uintptr_t,std::size_t>> readsD69;
    unsigned resolveCallsD69=0,equipmentCallsD69=0;bool actorOKD69=true,equipmentOKD69=true;
    unsigned spawnBoltIdD87=0;
    std::uintptr_t p(const auto& a)const{return reinterpret_cast<std::uintptr_t>(a.data());}
    template<class T>void put(std::uintptr_t at,T v){std::memcpy(reinterpret_cast<void*>(at),&v,sizeof(v));}
    template<class T>T get(std::uintptr_t at)const{T v{};std::memcpy(&v,reinterpret_cast<void*>(at),sizeof(v));return v;}
    template<class A>void block(A& a){blocks.emplace_back(p(a),sizeof(a));}
    void pin(std::uintptr_t at,const char* text){std::vector<unsigned char> v;unsigned z=0;
        while(*text){if(*text==' '){++text;continue;}char b[3]{text[0],text[1],0};z=unsigned(std::strtoul(b,nullptr,16));v.push_back((unsigned char)z);text+=2;}pins.emplace(at,std::move(v));}
    void event(unsigned index,float time,unsigned crc){const auto at=p(eventStorage)+16+index*0x50;
        put(at,time);put(at+8,crc);put(at+0xC,0);put(at+0x28,p(player)+0xF0);put(at+0x48,std::uintptr_t(0));}
    void events(bool load=false){const auto ep=p(eventStorage)+16;put(ep-4,load?2u:3u);
        if(load){event(0,.01f,0x34857AFA);event(1,.75f,0xEF667DEA);}
        else{event(0,.2f,0xAFE0B3FF);event(1,.8f,0x8ABB129A);event(2,.95f,0x3D6870EB);}}
    Fixture(){f=this;block(controller);block(context);block(scope);block(main);block(parent);block(child);block(equipment);
        block(item);block(skeleton);block(set);block(character);block(player);block(caf);block(entryStorage);block(eventStorage);block(queued);
        source.player=p(player);source.equipment=p(equipment);source.manager=p(parent)+0xF0;source.character=p(character);
        source.weaponItem=p(item);source.ammoItem=p(item)+0x80;source.ammoKey=81;
        put(p(equipment)+0x48,source.ammoItem);put(source.ammoItem,base+0x3A2DA88);put(source.ammoItem+0x30,source.ammoKey);
        source.key=44;source.generation=2;source.body=3;source.tracking=4;source.request=5;source.sample=1;source.tick=now;
        source.actor=30583;source.primary=1;source.classId=14;source.allowed=true;
        // Native-adapter fixtures model an explicit physical reload request.
        // Native button/no-origin cases clear these fields in focused tests.
        source.entryGripD72=2;source.entryGripTickD72=now;
        put(p(player),base+0x3AA4B98);put(p(player)+0x30,source.actor);
        put(p(equipment)+0x40,p(item));put(p(equipment)+0x50,p(main));put(p(item),base+0x3A2DA88);put(p(item)+0x30,source.key);
        put(p(main),base+0x3E7B1B0);put(p(main)+0x98,p(parent));put(p(parent),base+0x3E78CF8);put(p(parent)+0x80,p(main));put(p(parent)+0x70,p(child));
        put(p(child),base+0x3A450B0);put(p(child)+8,p(context));put(p(child)+0x28,1);put(p(child)+0x54,17u);put(p(child)+0x5C,2.5f);
        put(p(controller),base+0x40519E8);put(p(controller)+0x20,p(context));put(p(controller)+0x28,p(context));
        put(p(context)+0x10,p(character));put(p(controller)+0x34,1u);put(p(controller)+0x38,p(scope));
        queued[0]=p(child);put(p(controller)+0x48,p(queued));put(p(controller)+0x50,p(queued)+8);put(p(controller)+0x9C,1.f);
        put(p(scope),base+0x3AEA100);put(p(scope)+0x20,p(context));put(p(scope)+0x30,0u);put(p(scope)+0x34,1u);
        put(p(character),base+0x40602F8);put(p(character)+0xA70,p(skeleton));put(p(skeleton)+0x218,p(set));put(p(set),base+0x406BB38);
        put(p(character)+0xA9C,1.f);put(p(character)+0x110+0x48,entry());put(entry()-4,1u);
        put(entry(),base+0x3A70A60);put(entry()+0x10,5u);put(entry()+0x2C,1.f);put(entry()+0x34,5.f);
        put(p(caf)+0x30,5.f);put(p(caf)+0x34,5.f);put(p(caf)+0xA4,(unsigned char)1);put(p(caf)+0x64,1.f);
        put(p(caf)+0x48,base+0x3A7D9D0);put(p(caf)+0x50,p(eventStorage)+16);events();
        spatial.qualified=true;spatial.toolBound=true;spatial.toolVisible=false;spatial.toolPoseReady=true;
        spatial.axis={0,-1,0};spatial.travel=.1315;
        pin(base+0x2897C05,"f30f11435c");pin(base+0x4B3F12,"0f28cff30f594b5c488bcbff5030");
        std::vector<unsigned char> value(8);auto target=base+0xAA78BC;std::memcpy(value.data(),&target,8);pins[base+0x406BB38+0x68]=value;
        target=base+0x955DF4;std::memcpy(value.data(),&target,8);pins[base+0x406BB38+0xF0]=value;
        clearEquipmentH36Fixture(source);
    }
    bool read(std::uintptr_t at,void* dst,std::size_t n){
        if(traceReadsD69)readsD69.emplace_back(at,n);
        if(readFailure)return false;
        for(auto [start,size]:blocks)if(at>=start&&at-start<=size&&n<=size-(at-start)){std::memcpy(dst,reinterpret_cast<void*>(at),n);return true;}
        auto it=pins.find(at);if(it!=pins.end()&&it->second.size()>=n){std::memcpy(dst,it->second.data(),n);return true;}return false;
    }
    void input(double grip,x::Vec position={}){now+=20;source.tick=now;++source.sample;source.grip[0]=grip;source.hand[0]=position;x::PublishSource(source);}
    void activate(){put(p(controller)+0x50,p(queued));put(p(scope)+0x148,p(child));put(p(child)+0x30,p(scope));put(p(child)+0x28,2);}
};
static bool SehCopy(const void* p,void* out,std::size_t n)noexcept{return f&&f->read(reinterpret_cast<std::uintptr_t>(p),out,n);}
template<class T>static T Rd(const void* p,std::ptrdiff_t off,T fallback)noexcept{T v{};return p&&SehCopy(static_cast<const unsigned char*>(p)+off,&v,sizeof(v))?v:fallback;}
template<class T>static bool Wr(void* p,std::ptrdiff_t off,T v)noexcept{if(!f||(!f->partialWrite&&f->writeFailure))return false;
    ++f->writes;f->put(reinterpret_cast<std::uintptr_t>(p)+off,v);return !f->writeFailure;}
static std::atomic<std::uintptr_t> g_base{base};
static std::atomic<void*>g_henry_ctrl{nullptr};
static bool OnOwnerThread13()noexcept{return f&&f->current&&f->ownerThreadD72;}
static bool IsHenryController(void* ctrl)noexcept{return f&&f->current&&ctrl==f->controller.data();}
static std::uint64_t GetTickCount64()noexcept{return f?f->now:0;}
static bool MatchBytes(std::uintptr_t p,const unsigned char* expected,std::size_t n)noexcept{std::array<unsigned char,64>b{};return n<=b.size()&&SehCopy(reinterpret_cast<void*>(p),b.data(),n)&&!std::memcmp(b.data(),expected,n);}
namespace retail_build112{static bool Ready(std::uintptr_t b)noexcept{return b==base;}}
static void* HostXR34ResolveMainOwner(std::uintptr_t main)noexcept{if(f)++f->resolveCallsD69;return f&&f->actorOKD69&&main==f->p(f->main)?f->player.data():nullptr;}
static void* HostXR34Equipment(void* actor)noexcept{if(f)++f->equipmentCallsD69;return f&&f->equipmentOKD69&&actor==f->player.data()?f->equipment.data():nullptr;}
static bool HostXR34ClipHeader(std::uintptr_t set,int id,std::uintptr_t& h,float& d)noexcept{if(!f||set!=f->p(f->set)||id!=5)return false;h=f->p(f->caf);d=5;return true;}
static bool HostXR34Remaining(std::uintptr_t scope,float& remaining)noexcept{if(!f||scope!=f->p(f->scope))return false;remaining=f->remaining;return true;}
// Explicit host scheduler leaf: production uses its nonblocking owner lock.
// D82 also exercises a scan arriving while that lock is held elsewhere.
static bool hostOwnerTransactionAvailableD82=true;
struct OwnerTransaction19c4 {explicit operator bool()const{return hostOwnerTransactionAvailableD82;}};
#include "../src/subsystems/ranged_reload/crossbow_native_xr34.inl"
static bool spatial(const x::Owner& o,const x::Source& accepted,x::Spatial& s)noexcept{s=f->spatial;s.serial=o.serial;s.sample=accepted.sample;s.tick=accepted.tick;return true;}
static bool FixtureRegistrationXR34() noexcept{return true;}
static bool FixtureIdentityXR34(const x::Owner& o) noexcept{return f&&f->current&&f->source.allowed&&x::sameSource(f->source,o.source);}
static void setup(Fixture& a,bool load=false,bool reset=true){if(reset){g_crossbow_xr34=x::Client{};g_crossbow_base_xr34=0;x::revokedNativeLease=0;}
    g_henry_ctrl.store(a.controller.data());
    x::registrationReader=&FixtureRegistrationXR34;x::identityReader=&FixtureIdentityXR34;x::RefreshRegistration();x::SetSpatialReader(&spatial);x::PublishSource(a.source);a.events(load);
    // Native original executes FIRST and applies its real post-Queue stat seed.
    ++a.starts;a.put(a.p(a.child)+0x5C,2.5f);XR34PostStartNative(a.parent.data(),true);
    assert(a.get<float>(a.p(a.child)+0x5C)==0&&g_crossbow_xr34.pending());a.activate();
    // Original clip metadata records finish flags before our observer reads it.
    a.put(a.p(a.parent)+(load?0x99:0x9A),(unsigned char)1);XR34PostMetadataNative(a.parent.data(),a.child.data(),5,0);
    assert(g_crossbow_xr34.status().phase==x::Phase::Setup);
    // Existing clock-contract tests supply completed wrist transport explicitly.
    a.source.completedLease=g_crossbow_xr34.status().owner.serial;x::PublishSource(a.source);
}
static void tick(Fixture& a){XR34PreControllerNative(a.controller.data(),.02f);XR34PostControllerNative(a.controller.data());}
// Explicit native controller/animation/event/finalizer leaves. This deliberately
// calls the ACTUAL pre-owner and scan observer before native clock/event work;
// no test writes a loaded/cocked flag as if the adapter had done it.
static void nativeFrame(Fixture& a){
    XR34PreControllerNative(a.controller.data(),.02f);const float rate=a.get<float>(a.p(a.child)+0x5C);
    const float prior=a.get<float>(a.entry()+0x44);
    const float advance=.02f*rate;
    const float after=std::min(1.f,prior+advance/5.f);
    if(after>prior){
        struct Info {std::uintptr_t list,path;int first,animation,last;float weight;};
        Info info{a.p(a.caf)+0x48,base+0x406B7C0,0,5,int(a.get<unsigned>(a.p(a.eventStorage)+12))-1,1};
        a.blocks.emplace_back(reinterpret_cast<std::uintptr_t>(&info),sizeof(info));
        XR34ScanNative observer(a.character.data()+0x110,false,prior,after,&info,0);++a.scans;
        if(!observer.current&&g_crossbow_xr34.status().clockOwned){x::Source diagnostic{};
            std::cerr<<"scan refused phase="<<unsigned(g_crossbow_xr34.status().phase)<<" reason="<<unsigned(g_crossbow_xr34.status().reason)
                <<" from="<<prior<<" to="<<after<<" source="<<x::ReadSource(diagnostic)<<" registered="<<x::Registered()
                <<" current="<<XR34Current(g_crossbow_xr34.status().owner)<<" tick="<<a.now<<" sourceTick="<<diagnostic.tick<<'\n';}
        assert(!g_crossbow_xr34.status().clockOwned||observer.current);a.blocks.pop_back();
        for(unsigned i=0;i<a.get<unsigned>(a.p(a.eventStorage)+12);++i){const auto event=a.p(a.eventStorage)+16+i*0x50;
            const float at=a.get<float>(event);if(at>prior&&at<=after){++a.originalEvents;const auto crc=a.get<unsigned>(event+8);
                std::array<unsigned char,0x58> instance{};const auto e=reinterpret_cast<std::uintptr_t>(instance.data());
                a.put(e,at);a.put(e+0x10,info.path);a.put(e+0x18,5);a.put(e+0x1C,crc);a.put(e+0x50,event);
                a.blocks.emplace_back(e,instance.size());XR34BeforeEventNative(a.parent.data(),instance.data());a.blocks.pop_back();
                if(crc==0xEF667DEA)assert(!g_crossbow_xr34.status().placeBolt);
                if(crc==0x3D6870EB)assert(!g_crossbow_xr34.status().placeTool);
                if(crc==0xAFE0B3FF){++a.spawns;a.spatial.toolVisible=true;}
                if(crc==0x34857AFA){a.spatial.handBoltBound=true;a.spatial.boltPoseReady=true;
                    if(a.spawnBoltIdD87)a.spatial.boltId=a.spawnBoltIdD87;
                    if(a.spatial.boltId)a.put(a.p(a.parent)+0x88,a.spatial.boltId);}
                if(crc==0x8ABB129A){++a.cocks;a.spatial.nativeCocked=true;}
                if(crc==0xEF667DEA){++a.loads;a.spatial.nativeLoaded=true;a.spatial.nockBound=true;a.spatial.handBoltBound=false;}
            }}
    }
    a.put(a.entry()+0x44,after);
    a.put(a.p(a.child)+0x10,a.get<float>(a.p(a.child)+0x10)+advance);
    if(advance>a.remaining){XR34BeforeCompleteNative(a.parent.data(),a.child.data());++a.finals;
        if(a.get<unsigned char>(a.p(a.parent)+0x9A)&&!a.spatial.nativeCocked){++a.cocks;a.spatial.nativeCocked=true;}
        if(a.get<unsigned char>(a.p(a.parent)+0x99)&&!a.spatial.nativeLoaded){++a.loads;a.spatial.nativeLoaded=a.spatial.nockBound=true;}
        a.remaining=0;
    }else a.remaining-=advance;
    XR34PostControllerNative(a.controller.data());
}
static void nativeUntil(Fixture& a,float normalized){for(int n=0;n<400&&a.get<float>(a.entry()+0x44)<normalized;++n){a.input(0);nativeFrame(a);}}
#ifndef KCD2VR_XR34_NATIVE_NO_MAIN
int main(){
    {Fixture a;setup(a);tick(a);assert(a.get<float>(a.p(a.child)+0x5C)>0);assert(a.cocks==0&&a.loads==0);
        a.put(a.entry()+0x44,.20001f);a.spatial.toolVisible=true;a.input(0);tick(a);assert(a.get<float>(a.p(a.child)+0x5C)==0);
        a.input(.8);tick(a);a.input(.8,{0,-.06575,0});tick(a);assert(g_crossbow_xr34.status().progress>.49&&g_crossbow_xr34.status().progress<.51);
        const float speed=a.get<float>(a.p(a.child)+0x5C);assert(speed>0&&speed<=2.5);assert(a.cocks==0);
        // Same source sample cannot create a new physical stroke or grant a finalizer.
        tick(a);assert(g_crossbow_xr34.status().progress<1);a.input(.8,{0,-.1315,0});tick(a);
        assert(g_crossbow_xr34.status().phase==x::Phase::Tail);XR34BeforeCompleteNative(a.parent.data(),a.child.data());assert(g_crossbow_xr34.pending());
        ++a.finals;if(a.get<unsigned char>(a.p(a.parent)+0x9A))a.spatial.nativeCocked=true;
        a.input(.8);tick(a);assert(g_crossbow_xr34.status().reason==x::Reason::NativeAck);
        XR34BeforeStopNative(a.parent.data());++a.stops;assert(a.get<float>(a.p(a.child)+0x5C)==2.5f&&!g_crossbow_xr34.pending());
        // SAME weapon gets an entirely new child token/session, not H21 pass-through.
        a.put(a.p(a.child)+0x54,18u);a.put(a.p(a.child)+0x28,1);a.put(a.p(a.scope)+0x148,std::uintptr_t(0));
        a.put(a.p(a.controller)+0x50,a.p(a.queued)+8);a.put(a.p(a.child)+0x10,0.f);
        const auto priorSerial=g_crossbow_xr34.status().owner.serial;setup(a,false,false);
        assert(g_crossbow_xr34.pending()&&g_crossbow_xr34.status().owner.serial>priorSerial);}
    {Fixture a;setup(a,true);a.put(a.entry()+0x44,.01001f);a.spatial.handBoltBound=a.spatial.boltPoseReady=true;
        a.input(0);tick(a);a.input(.8);tick(a);a.input(0);tick(a);assert(g_crossbow_xr34.status().phase!=x::Phase::Tail);
        a.input(.8);tick(a);a.spatial.handBoltPlaced=true;a.input(0);tick(a);assert(g_crossbow_xr34.status().phase==x::Phase::Tail);
        assert(a.loads==0); // hand placement grants time, NEVER writes the native load state
        a.spatial.nativeLoaded=true;a.spatial.nockBound=false;a.input(0);tick(a);assert(g_crossbow_xr34.status().reason!=x::Reason::NativeAck);
        a.spatial.nockBound=true;a.input(0);tick(a);assert(g_crossbow_xr34.status().reason==x::Reason::NativeAck);}
    {Fixture a;setup(a);a.source.allowed=false;x::PublishSource(a.source);tick(a);assert(!g_crossbow_xr34.pending()&&a.get<float>(a.p(a.child)+0x5C)==2.5f);}
    {Fixture a;setup(a);a.put(a.p(a.child)+0x54,999u);tick(a);assert(!g_crossbow_xr34.pending()&&a.get<float>(a.p(a.child)+0x5C)==0);}
    {Fixture a;setup(a);a.put(a.p(a.child)+0x5C,.75f);XR34BeforeStopNative(a.parent.data());assert(!g_crossbow_xr34.pending()&&a.get<float>(a.p(a.child)+0x5C)==.75f);}
    {Fixture a;setup(a);a.readFailure=true;XR34BeforeStopNative(a.parent.data());assert(g_crossbow_xr34.pending());
        a.readFailure=false;XR34BeforeStopNative(a.parent.data());assert(!g_crossbow_xr34.pending());}
    {Fixture a;setup(a);assert(!g_crossbow_xr34.interval(f->p(f->caf)+0x48,5,0,0,.81f));assert(!g_crossbow_xr34.pending());}
    {Fixture a;setup(a);XR34BeforeCompleteNative(a.parent.data(),a.child.data());assert(!g_crossbow_xr34.pending()&&a.loads==0&&a.cocks==0);}
    {Fixture a;setup(a);a.source.body++;x::PublishSource(a.source);tick(a);assert(!g_crossbow_xr34.pending());}
    {Fixture a;setup(a);a.current=false;tick(a);assert(!g_crossbow_xr34.pending());}
    {Fixture a;setup(a);a.put(a.p(a.scope)+0x60,.01f);tick(a);assert(!g_crossbow_xr34.pending());}
    // Actual selected interval controls real mocked-native event leaves: setup
    // may spawn a tool, but no cock/finalizer can run before physical work.
    {Fixture a;setup(a);nativeUntil(a,.200001f);assert(a.spawns==1&&a.cocks==0&&a.finals==0);
        for(int i=0;i<40;++i){a.input(0);nativeFrame(a);}assert(a.cocks==0&&a.finals==0);
        a.input(.8);nativeFrame(a);a.input(.8,{0,-.1315,0});nativeFrame(a);
        x::Passenger currentPassenger{};assert(x::ReadPassenger(currentPassenger,a.now));
        assert(currentPassenger.frame.tick==a.source.tick&&currentPassenger.workingHand==0&&currentPassenger.childStatus==2);
        for(int i=0;i<300&&a.finals==0;++i){a.input(.8,{0,-.1315,0});nativeFrame(a);}
        assert(a.cocks==1&&a.finals==1&&a.loads==0&&a.originalEvents==3);
        x::Passenger p{};a.input(.8);tick(a);assert(!x::ReadPassenger(p,a.now));
        a.now+=81;assert(!x::ReadPassenger(p,a.now));
        XR34BeforeStopNative(a.parent.data());assert(!x::ReadPassenger(p,a.now));}
    {Fixture a;setup(a,true);nativeUntil(a,.010001f);assert(a.spatial.handBoltBound&&a.loads==0);
        a.input(0);nativeFrame(a);a.input(.8);nativeFrame(a);a.input(0);nativeFrame(a);
        assert(a.loads==0&&a.finals==0);
        a.input(.8);nativeFrame(a);a.spatial.handBoltPlaced=true;a.input(0);nativeFrame(a);
        for(int i=0;i<300&&a.finals==0;++i){a.input(0);nativeFrame(a);}
        assert(a.loads==1&&a.finals==1&&a.cocks==0);}
    // Foreign semantic activities, paired/cutscene/native inventory admission
    // cannot inherit the reload child merely because Henry is the actor.
    for(unsigned foreign=0;foreign<6;++foreign){Fixture a;setup(a);const int before=a.writes;
        a.put(a.p(a.parent),base+0x4000000+foreign*8);tick(a);
        assert(!g_crossbow_xr34.pending()&&a.writes==before+1&&a.cocks==0&&a.loads==0);}
    {Fixture a;setup(a);a.put(a.p(a.child)+0x28,3);tick(a);assert(!g_crossbow_xr34.pending());}
    {Fixture a;setup(a);a.source.ammoKey++;x::PublishSource(a.source);tick(a);assert(!g_crossbow_xr34.pending());}
    {Fixture a;setup(a);a.partialWrite=true;a.writeFailure=true;
        XR34BeforeStopNative(a.parent.data());assert(g_crossbow_xr34.pending());
        a.writeFailure=false;XR34BeforeStopNative(a.parent.data());assert(!g_crossbow_xr34.pending());}
    {Fixture a;setup(a);a.put(a.p(a.child)+0x28,1);a.put(a.p(a.scope)+0x148,std::uintptr_t(0));
        a.put(a.p(a.controller)+0x50,a.p(a.queued)+8);a.now+=1001;a.source.tick=a.now;x::PublishSource(a.source);
        tick(a);assert(!g_crossbow_xr34.pending());}
    {Fixture a;setup(a);a.source.allowed=false;x::PublishSource(a.source);x::Source current{};assert(!x::ReadSource(current));}
    {Fixture a;setup(a);const auto oldSpeed=a.get<float>(a.p(a.child)+0x5C);
        XR34BeforeCompleteNative(a.parent.data(),a.child.data()+8);assert(g_crossbow_xr34.pending()&&a.get<float>(a.p(a.child)+0x5C)==oldSpeed);}
    {Fixture a;setup(a,true);std::array<unsigned char,0x58> event{};a.blocks.emplace_back(a.p(event),event.size());
        XR34BeforeEventNative(a.parent.data(),event.data());assert(!g_crossbow_xr34.pending()&&a.loads==0);}
    {Fixture a;setup(a);a.input(0);tick(a);x::Status state{};assert(x::ReadStatus(state));
        x::RevokeNativeParent(a.parent.data());assert(!x::ReadStatus(state));tick(a);assert(!g_crossbow_xr34.pending());}
    {Fixture a;setup(a);x::Source broken=a.source;broken.world[7]=0;assert(!x::fresh(broken,a.now));
        broken=a.source;broken.allowedWrist[0].q[0]=100;assert(!x::fresh(broken,a.now));}
    // D74: retail CAF has one segment and two endpoints for these reload clips.
    {Fixture a;setup(a);x::Clip clip{};auto owner=g_crossbow_xr34.status().owner;
        assert(XR34NativeClip(owner,5,0,clip));a.put(a.p(a.caf)+0xA4,(unsigned char)2);
        assert(!XR34NativeClip(owner,5,0,clip));}
    for(auto offset:{0x2C,0x30,0x34}){Fixture a;setup(a);x::Clip clip{};auto owner=g_crossbow_xr34.status().owner;
        a.put(a.p(a.caf)+offset,std::numeric_limits<float>::quiet_NaN());assert(!XR34NativeClip(owner,5,0,clip));}
    {Fixture a;setup(a);auto state=g_crossbow_xr34.status();x::Clip clip{};assert(XR34NativeClip(state.owner,5,0,clip));
        a.put(a.entry()+0x34,std::numeric_limits<float>::quiet_NaN());x::Clock clock{};
        assert(!XR34NativeClock(state.owner,clip,clock));}
    std::cout<<"XR34 actual native adapter/clock contracts PASS (explicit native leaves only; no headset/game claim)\n";
}

#endif
