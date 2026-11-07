// Full production weapon owner plus actual typed phase/lease/pair operations.
// Substitute only the native memory/function, native geometry/class and OS
// leaves. The experimental control, source stamp, pair causality and dispatch
// are production code. Native instructions and engine hooks are not executed.
#define KCD2VR_PHYSICS_PHASE_T4_TEST 1
#define KCD2VR_WEAPON_BODY_T4_HOST_TEST 1
#include "../src/native_physics_phase_t4.cpp"
#include "../src/native_weapon_body_t4.cpp"
#include "../src/native_weapon_clearance_t4.cpp"
#include "vrik/native_held_owner_t4_status.h"
#include "vrik/physical_weapon.h"
#include "core/native_grasp13.h"
#include "tracking_sample59c6.h"
#include "../src/subsystems/physics/native_attachment_slots_f2.inl"
#include <map>
#include <vector>
#include <string>
#include <cstdio>
#include <cstdlib>

namespace fixture {
namespace p=kcd2vr::native_physics_phase_t4;namespace w=kcd2vr::native_weapon_body_t4;
namespace ds=kcd2vr::deepseek_h29;
using w::record;using w::selfPairH5;
constexpr std::uintptr_t base=0x10000000;
std::array<unsigned char,0x6100> world{},world2{};
std::array<unsigned char,0x500> owned{};
std::array<unsigned char,0xB00> henry{},nativeItem{};
std::map<std::uintptr_t,std::uintptr_t> slots;
void* selectedWorld=world.data();void* registeredOwned=nullptr;void* registeredHenry=henry.data();
std::uint64_t now=1000;unsigned checks=0,creates=0,adds=0,removes=0,destroys=0,classes0=0,classes4=0;
int ownedRefs=0,livingRefs=0,refusesRef=0,addReturn=77,removeReturn=1;
unsigned pairAdds=0,pairRemoves=0,livingReleases=0,motors=0;
bool geometry=false,collision=false,pairExists=false,pairKnown=true,pairWrong=false,pairAutoPresent=true,
    removeAutoAbsent=true,predicate=true,failClass0=false,failClass4=false,refFault=false,
    addFault=false,releaseFault=false,revokeOnOpen=false,flipOnAdd=false;
bool removeFault=false,detachRefusal=false,revokeOnRetain=false;
bool positiveRefFault=false;
bool offOnClass=false,offInRigidStatus=false,offInMotor=false;int flipInSourceRead=0;
int pinOperation=0;w::Owner pinOwner{};std::uint64_t pinGeneration=0;
w::Pose achieved{};p::V3 linear{},angular{};double mass=2;
int entity=0,itemEntity=0,geometryCookie=0;
int livingId=12,itemBodyId=11;bool capturedMode=false;
bool CapturedSourceCurrent(const w::Input&)noexcept;
std::vector<std::string> events;
template<class T>void set(void* at,std::size_t offset,T value){std::memcpy(static_cast<unsigned char*>(at)+offset,&value,sizeof value);}
template<class T>T get(const void* at,std::size_t offset){T out{};std::memcpy(&out,static_cast<const unsigned char*>(at)+offset,sizeof out);return out;}
void check(bool ok,const char* message){++checks;if(!ok){std::fprintf(stderr,"CHECK %u FAILED: %s\n",checks,message);std::abort();}}
#define CHECK(x) ::fixture::check(bool(x),#x)
bool ReadMemory(const void* at,void* out,std::size_t size){
    const auto address=reinterpret_cast<std::uintptr_t>(at);
    if(address==base+0x492D830&&size==sizeof(void*)){std::memcpy(out,&selectedWorld,size);return true;}
    if(auto i=slots.find(address);i!=slots.end()&&size==sizeof(i->second)){std::memcpy(out,&i->second,size);return true;}
    for(auto range:{std::pair{world.data(),world.size()},std::pair{world2.data(),world2.size()},std::pair{owned.data(),owned.size()},std::pair{henry.data(),henry.size()},std::pair{nativeItem.data(),nativeItem.size()}}){
        auto begin=reinterpret_cast<std::uintptr_t>(range.first);
        if(address>=begin&&address-begin<=range.second&&size<=range.second-(address-begin)){std::memcpy(out,at,size);return true;}
    }return false;
}
void* Lookup(void* wld,int id){CHECK(wld==selectedWorld||wld==world.data());return id==42?registeredOwned:id==livingId?registeredHenry:id==itemBodyId?nativeItem.data():nullptr;}
int Id(void* wld,void* b){CHECK(wld==selectedWorld&&b==owned.data());return 42;}
int AddRef(void* b){CHECK(b==owned.data()||b==henry.data());
    if(b==henry.data()&&refusesRef)return 0;
    if(refFault){p::faults.fetch_add(1);p::nativeFaulted=true;return 0;}
    events.push_back(b==owned.data()?"retain-owned":"retain-henry");
    if(b==henry.data()&&revokeOnRetain)set(henry.data(),0x20,static_cast<void*>(&itemEntity));
    const auto result=b==owned.data()?++ownedRefs:++livingRefs;
    if(b==henry.data()&&positiveRefFault){p::faults.fetch_add(1);p::nativeFaulted=true;}
    return result;}
int Release(void* b){CHECK(b==owned.data()||b==henry.data());
    if(releaseFault){p::faults.fetch_add(1);p::nativeFaulted=true;return 0;}
    events.push_back(b==owned.data()?"release-owned":"release-henry");
    if(b==owned.data()){CHECK(ownedRefs>0);return --ownedRefs;}
    ++livingReleases;CHECK(livingRefs>0);return --livingRefs;}
int Status(void* b,void* status){CHECK(b==owned.data());const auto type=get<int>(status,0);
    if(type==1){if(offInRigidStatus){offInRigidStatus=false;ds::DisableAll();}
        set(status,0x18,p::V3{float(achieved.p.x),float(achieved.p.y),float(achieved.p.z)});set(status,0x3C,p::Q{});set(status,0x4C,1.f);set(status,0x50,2);return 1;}
    if(type==8){set(status,0xC,linear);set(status,0x18,angular);set(status,0x3C,p::V3{});set(status,0x4C,float(mass));return 1;}
    CHECK(type==26&&get<int>(status,4)==77&&get<int>(status,8)==-1);
    if(!pairKnown)return -1;if(!pairExists)return 0;
    set(status,0xC,pairWrong?0x200u:0x300u);set(status,0x38,pairWrong?nativeItem.data():henry.data());return 1;
}
int Action(void* b,void* action,int safe){CHECK(b==owned.data()&&safe==1);
    const auto type=get<int>(action,0);
    if(type==5){++pairAdds;events.push_back("add-pair");
        CHECK(get<std::uintptr_t>(action,8)==reinterpret_cast<std::uintptr_t>(henry.data()));
        CHECK(get<unsigned>(action,0x60)==0x300u&&get<float>(action,0x64)==0);
        CHECK(get<float>(action,0x3C)==1&&get<float>(action,0x4C)==1);
        CHECK(get<float>(action,0x10)==0&&get<float>(action,0x1C)==0);
        if(addFault){p::faults.fetch_add(1);p::nativeFaulted=true;return 0;}
        if(flipOnAdd){ds::SetWeaponNpcContactsH5(false);ds::SetWeaponNpcContactsH5(true);}
        if(addReturn>0&&pairAutoPresent)pairExists=true;return addReturn;
    }
    if(type==6){++pairRemoves;events.push_back("remove-pair");
        CHECK(get<unsigned>(action,4)==77u&&get<int>(action,0x10)==1);
        CHECK(get<unsigned>(owned.data(),0xF0)==4u&&!geometry&&!collision);
        w::ContactOwnerH24 stale{};CHECK(!w::ReadContactOwnerH24(stale));
        if(removeFault){p::faults.fetch_add(1);p::nativeFaulted=true;return 0;}
        if(removeReturn>0&&removeAutoAbsent)pairExists=false;return removeReturn;
    }
    CHECK(type==10);++motors;if(offInMotor){offInMotor=false;ds::DisableAll();}
    linear=get<p::V3>(action,0xC);angular=get<p::V3>(action,0x18);return 1;
}
bool Source(const w::Input& in)noexcept{if(flipInSourceRead){const auto kind=flipInSourceRead;flipInSourceRead=0;ds::DisableAll();if(kind==2)ds::SetWeaponNpcContactsH5(true);}
    return predicate&&(!capturedMode||CapturedSourceCurrent(in));}
void PinAfterOwner(p::Context& c,void*)noexcept {
    if(!pinOperation)return;CHECK(w::PinColliderConstraint(c,pinOwner,pinGeneration,pinOperation>0));pinOperation=0;
}
void PinReset(p::Context&,void*)noexcept {}
void PrepareWorld(void* at){set(at,0,base+0x407FC80);set(at,0x5FDC,0x10000u);set(at,0x5FF4,0x10000u);}
void Init(){
    p::base=base;p::testRead=&ReadMemory;p::api={&Lookup,&Id,&AddRef,&Release,&Status,&Action};
    for(auto row:std::initializer_list<std::array<std::uintptr_t,3>>{
        {0x407FC80,0xB0,0x667418},{0x407FC80,0xA8,0x52950C},
        {0x3A82570,0x10,0x5311C4},{0x3A82570,0x18,0x531F84},
        {0x3A48DC0,0x10,0x5311C4},{0x3A48DC0,0x18,0x531F84},
        {0x3A48DC0,0x30,0x8A6C90},{0x3A48DC0,0x38,0xAC0D18},{0x3A48DC0,0x150,0xBCC44C}})
        slots[base+row[0]+row[1]]=base+row[2];
    PrepareWorld(world.data());PrepareWorld(world2.data());
    set(henry.data(),0,base+0x3A82570);set(henry.data(),0x68,selectedWorld);set(henry.data(),0x20,static_cast<void*>(&entity));
    set(henry.data(),0x28,std::int16_t(2));set(henry.data(),0x47,std::int8_t(3));set(henry.data(),0xEC,4u);
    set(nativeItem.data(),0,base+0x3A578C0);set(nativeItem.data(),0x68,selectedWorld);set(nativeItem.data(),0x20,static_cast<void*>(&itemEntity));
    set(nativeItem.data(),0x28,std::int16_t(2));set(nativeItem.data(),0x47,std::int8_t(-1));set(nativeItem.data(),0x7C,0x100u);
    // Explicit native-hook/OS leaf substitution only. Real dispatch qualification,
    // typed lifetime and client callbacks run unchanged; no optional hidden flag.
    p::installed=true;CHECK(p::RegisterClient({&w::Step,&w::Reset,nullptr}));
    CHECK(p::RegisterClient({&PinAfterOwner,&PinReset,nullptr}));
    CHECK(w::SetSourcePredicate(&Source));w::module=base;
    CHECK(kcd2vr::native_weapon_clearance_t4::Install());w::SetEnabled(true);
}
w::Input Input(){w::Input in{};in.owner={1,2,3,4,0x100,0x200,0x300,reinterpret_cast<std::uintptr_t>(&itemEntity),0x500,0x600,7,0};
    in.weaponWorld.p={10,20,30};in.targetWorld=in.weaponWorld;in.boundsMin={-.04,-.2,-.03};in.boundsMax={.04,.8,.03};in.mass=2;in.scale=1;
    in.sample=1;in.tick=now;in.trackingEpoch=9;in.cancelGeneration=8;in.colliderLease=10;in.classId=1;
    in.allowed=in.tracked=in.emptyLeft=in.nativeColliderExcluded=true;in.nativeBody=reinterpret_cast<std::uintptr_t>(nativeItem.data());in.nativeBodyId=11;in.nativeProxy=0x800;
    in.playerBody=reinterpret_cast<std::uintptr_t>(henry.data());in.playerEntityPointer=reinterpret_cast<std::uintptr_t>(&entity);in.playerBodyId=12;
    ds::StampWeaponNpcContactControlH5(in);return in;
}
void Tick(){p::Access::Dispatch(selectedWorld,p::Phase::Step);}
void Fresh(w::Input& in,bool stamp=true){now+=10;in.tick=now;++in.sample;if(stamp)ds::StampWeaponNpcContactControlH5(in);w::Submit(in);
    // Original native insertion drain runs before this T4 boundary next round.
    if(registeredOwned&&record.lease.Held()&&record.lease.InsertionPending())set(owned.data(),0x84,w::ContactFlagsH24().flagsOR);
    Tick();}
void Active(w::Input& in,bool stamp=true){w::Submit(in);Tick();CHECK(registeredOwned&&ownedRefs==1);
    set(owned.data(),0x84,w::ContactFlagsH24().flagsOR);Fresh(in,stamp);CHECK(geometry&&!collision);Fresh(in,stamp);CHECK(collision);
    w::Achieved out{};CHECK(w::Read(out)&&out.poseValid&&out.state==w::State::Active);CHECK(get<unsigned>(owned.data(),0xF0)==4u);}
void Open(w::Input& in){Active(in);Fresh(in);CHECK(pairAdds==1&&livingRefs==1&&get<unsigned>(owned.data(),0xF0)==4u);
    Fresh(in);w::Achieved out{};CHECK(w::Read(out)&&out.poseValid&&out.npcContactsH5&&get<unsigned>(owned.data(),0xF0)==0u);}
void Drain(w::Input& in,bool stamp=true){for(int n=0;n<5;++n)Fresh(in,stamp);}
bool Before(const char* a,const char* b){auto x=std::find(events.begin(),events.end(),a),y=std::find(events.begin(),events.end(),b);return x!=events.end()&&y!=events.end()&&x<y;}
}
namespace capture_h5 {
#include "generated_weapon_self_pair_capture_h5.inc"
}
namespace fixture {
bool CapturedSourceCurrent(const w::Input& in)noexcept {
    capture_h5::NativeHeldSourceT4 s{};
    return capture_h5::g_native_held_valid_t4&&capture_h5::g_native_held_source_t4.read(s)&&s.valid&&
        capture_h5::currentContext&&s.input.owner==in.owner&&s.input.colliderLease==in.colliderLease&&
        s.input.playerBody==in.playerBody&&s.input.playerBodyId==in.playerBodyId&&
        s.input.playerEntityPointer==in.playerEntityPointer&&s.input.nativeBody==in.nativeBody&&
        s.input.nativeBodyId==in.nativeBodyId&&in.tick>=s.tick&&in.tick-s.tick<=80;
}
w::Input CapturedInput(){
    auto equipment=capture_h5::setup(0);capturedMode=true;livingId=24;itemBodyId=23;
    set(henry.data(),0,base+0x3A82570);set(henry.data(),0x68,selectedWorld);set(henry.data(),0x47,std::int8_t(3));set(henry.data(),0xEC,4u);
    set(nativeItem.data(),0,base+0x3A578C0);set(nativeItem.data(),0x68,selectedWorld);set(nativeItem.data(),0x47,std::int8_t(-1));set(nativeItem.data(),0x7C,0x100u);
    capture_h5::NativeHeldCaptureT4(equipment,now);capture_h5::NativeHeldSourceT4 captured{};
    CHECK(capture_h5::g_native_held_valid_t4&&capture_h5::g_native_held_source_t4.read(captured)&&captured.valid);
    CHECK(captured.input.npcContactControlH5==ds::ReadWeaponNpcContactControlH5());
    auto in=captured.input;in.allowed=in.tracked=true;in.sample=1;in.tick=now;return in;
}
}
namespace kcd2vr::retail_build112 {bool Ready(std::uintptr_t)noexcept{return false;}}
namespace kcd2vr::native_weapon_body_t4 {namespace {
std::uint64_t HostNow()noexcept{return fixture::now;}bool HostInstall()noexcept{return true;}
bool HostBind(phase::Context&,void*& manager)noexcept{manager=&fixture::geometryCookie;return true;}
void* HostCreate(phase::Context& c,const Pose& pose)noexcept{using namespace fixture;++creates;registeredOwned=owned.data();achieved=pose;
    owned.fill(0);set(owned.data(),0,base+0x3A48DC0);set(owned.data(),0x68,c.World());set(owned.data(),0x20,static_cast<void*>(record.foreignCookieH24));
    set(owned.data(),0x28,std::int16_t(foreignType));set(owned.data(),0x47,std::int8_t(2));set(owned.data(),0x84,0x80000000u);set(owned.data(),0x194,std::uint8_t(1));return owned.data();}
void* HostGeometry(phase::Context&,const policy::Shape&,void*)noexcept{return &fixture::geometryCookie;}
bool HostAddGeometry(phase::Context&,void*,void*,const policy::Shape& shape,bool colliding)noexcept{using namespace fixture;++adds;geometry=true;collision=colliding;mass=shape.mass;return true;}
bool HostRemoveGeometry(phase::Context&,void*)noexcept{using namespace fixture;++removes;events.push_back("detach");geometry=collision=false;return !detachRefusal;}
bool HostPart(phase::Context&,void*,void*,bool colliding,double wanted)noexcept{return fixture::geometry&&fixture::collision==colliding&&fixture::mass==wanted;}
bool HostConfigure(phase::Context&,void*)noexcept{fixture::set(fixture::owned.data(),0xEC,0u);fixture::set(fixture::owned.data(),0xF0,4u);return true;}
bool HostFilters(phase::Context&,void*)noexcept{return fixture::get<unsigned>(fixture::owned.data(),0xEC)==0&&fixture::get<unsigned>(fixture::owned.data(),0xF0)==(selfPairH5.open?0u:4u);}
bool HostPairClassH5(phase::Context& c,void* body,unsigned ignore)noexcept{using namespace fixture;CHECK(c.Current()&&body==owned.data());
    if(ignore==0){++classes0;events.push_back("class0");if(revokeOnOpen)predicate=false;if(offOnClass){offOnClass=false;ds::DisableAll();}}else{++classes4;events.push_back("class4");}
    set(body,0xF0,ignore);return ignore==0?!failClass0:!failClass4;}
bool HostUnregister(phase::Context&,void*,void*)noexcept{return true;}
int HostDestroy(phase::Context&,void*)noexcept{++fixture::destroys;fixture::events.push_back("destroy");fixture::registeredOwned=nullptr;return 1;}
}}
namespace kcd2vr::native_weapon_clearance_t4 {namespace {
bool HostClearanceInstall()noexcept{return true;}float HostClearance(native_physics_phase_t4::Context&,Capsule,void*)noexcept{return 0;}
}}
int main(int argc,char** argv){using namespace fixture;const std::string mode=argc>1?argv[1]:"lifecycle";Init();
    CHECK(!ds::WeaponNpcContactsEnabledH5()&&ds::ReadWeaponNpcContactControlH5()==0);
    if(mode=="default-off"){auto in=Input();Active(in);for(int i=0;i<6;++i)Fresh(in);CHECK(pairAdds==0&&livingRefs==0&&classes0==0&&motors>0);CHECK(get<unsigned>(owned.data(),0xF0)==4u);}
    else {
        ds::SetWeaponNpcContactsH5(true);CHECK(ds::WeaponNpcContactsEnabledH5());auto in=Input();
        if(mode=="public-capture"){in=CapturedInput();Active(in,false);Fresh(in,false);Fresh(in,false);w::Achieved out{};
            CHECK(w::Read(out)&&out.npcContactsH5&&out.poseValid&&pairAdds==1&&classes0==1&&livingRefs==1);
            CHECK(capture_h5::g_native_held_lease_t4==in.colliderLease&&selfPairH5.controlWord==in.npcContactControlH5);}
        else if(mode=="pending"){pairAutoPresent=false;Active(in);Fresh(in);Fresh(in);CHECK(pairAdds==1&&livingRefs==1&&classes0==0&&collision&&motors>0);
            ds::DisableAll();Fresh(in);CHECK(pairRemoves==0&&livingRefs==1&&!collision&&!record.destroyRequested);
            pairExists=true;Fresh(in);Drain(in);CHECK(pairRemoves==1&&livingRefs==0&&destroys==1);}
        else if(mode=="refused"||mode=="id-high-bit"){addReturn=mode=="id-high-bit"?std::numeric_limits<int>::min():0;Active(in);Fresh(in);for(int i=0;i<6;++i)Fresh(in);CHECK(pairAdds==1&&livingRefs==0&&collision&&motors>0&&classes0==0);}
        else if(mode=="refused-ref"){refusesRef=1;Active(in);Fresh(in);Fresh(in);CHECK(pairAdds==0&&livingRefs==0&&collision&&motors>0&&classes0==0);}
        else if(mode=="source-stale-word"){in.npcContactControlH5=0;Active(in,false);for(int i=0;i<5;++i)Fresh(in,false);CHECK(classes0==0&&pairAdds==0&&collision&&motors>0);Fresh(in);Fresh(in);CHECK(pairAdds==1&&classes0==1);}
        else if(mode=="retained-revoked"){revokeOnRetain=true;Active(in);Fresh(in);CHECK(pairAdds==0&&livingRefs==0&&livingReleases==1&&classes0==0);}
        else if(mode=="positive-reference-fault"){positiveRefFault=true;Active(in);Fresh(in);
            CHECK(record.quarantined&&selfPairH5.living.Held()&&livingRefs==1&&pairAdds==0&&classes0==0&&livingReleases==0);}
        else if(mode=="pending-timeout"){pairAutoPresent=false;Active(in);Fresh(in);const auto original=selfPairH5.causal.submittedTick;
            now+=1001;Fresh(in);CHECK(record.quarantined&&selfPairH5.causal.submittedTick==original&&livingRefs==1&&pairRemoves==0&&classes0==0);}
        else if(mode=="pending-reset"){pairAutoPresent=false;Active(in);Fresh(in);set(world.data(),0x5FF4,0u);p::Access::Dispatch(world.data(),p::Phase::WorldReset);
            CHECK(record.quarantined&&pairRemoves==0&&livingRefs==1&&ownedRefs==1&&!collision&&classes0==0);}
        else if(mode=="pending-shutdown"){pairAutoPresent=false;Active(in);Fresh(in);p::RequestShutdown();Tick();CHECK(!collision&&livingRefs==1&&ownedRefs==1&&pairRemoves==0&&!record.quarantined);
            pairExists=true;Tick();Tick();CHECK(pairRemoves==1&&livingRefs==0&&ownedRefs==0&&!record.quarantined);}
        else if(mode=="uncertain"){addFault=true;Active(in);Fresh(in);CHECK(record.quarantined&&livingRefs==1&&classes0==0&&pairRemoves==0);Tick();CHECK(pairAdds==1&&pairRemoves==0&&livingReleases==0);}
        else if(mode=="toggle-during-add"){flipOnAdd=true;Active(in);Fresh(in);CHECK(classes0==0&&!collision&&pairAdds==1&&pairRemoves==1&&livingRefs==0);}
        else if(mode=="open-revoke"){revokeOnOpen=true;Active(in);Fresh(in);Fresh(in);CHECK(classes0==1&&get<unsigned>(owned.data(),0xF0)==4u&&!collision&&pairRemoves==1);}
        else if(mode=="class-fault"){failClass0=true;Active(in);Fresh(in);Fresh(in);CHECK(classes0==1&&get<unsigned>(owned.data(),0xF0)==4u&&!collision&&pairRemoves==1);}
        else if(mode=="off-in-class"){Active(in);Fresh(in);offOnClass=true;Fresh(in);w::Achieved out{};
            CHECK(classes0==1&&get<unsigned>(owned.data(),0xF0)==4u&&!collision&&pairRemoves==1&&w::Read(out)&&!out.poseValid);}
        else {
            Open(in);const auto word=in.npcContactControlH5;
            if(mode=="no-absent"){removeAutoAbsent=false;ds::DisableAll();Fresh(in);Fresh(in);CHECK(pairRemoves==1&&livingRefs==1&&!collision&&destroys==0);pairExists=false;Drain(in);CHECK(pairRemoves==1&&livingRefs==0&&destroys==1);}
            else if(mode=="off-in-pair-check"){flipInSourceRead=1;CHECK(!w::PairControlCurrentH5(in,in.npcContactControlH5));
                Fresh(in);CHECK(get<unsigned>(owned.data(),0xF0)==4u&&!collision&&pairRemoves==1&&livingRefs==0);}
            else if(mode=="off-in-read-predicate"||mode=="aba-in-read-predicate"||mode=="off-in-contact-read"||mode=="aba-in-contact-read"){
                flipInSourceRead=mode.starts_with("aba")?2:1;w::Achieved out{};w::ContactOwnerH24 owner{};
                if(mode.ends_with("contact-read"))CHECK(!w::ReadContactOwnerH24(owner));else CHECK(w::Read(out)&&!out.poseValid);
                CHECK(get<unsigned>(owned.data(),0xF0)==0u&&livingRefs==1);Fresh(in);
                CHECK(get<unsigned>(owned.data(),0xF0)==4u&&!collision&&pairRemoves==1&&livingRefs==0);
            }
            else if(mode=="off-in-status"){const auto before=motors;offInRigidStatus=true;Fresh(in);w::Achieved out{};
                CHECK(motors==before&&get<unsigned>(owned.data(),0xF0)==4u&&!collision&&pairRemoves==1&&w::Read(out)&&!out.poseValid);}
            else if(mode=="off-in-motor"){const auto before=motors;offInMotor=true;Fresh(in);w::Achieved out{};w::ContactOwnerH24 owner{};
                CHECK(motors==before+1&&w::Read(out)&&!out.poseValid&&!w::ReadContactOwnerH24(owner));
                CHECK(get<unsigned>(owned.data(),0xF0)==0u&&livingRefs==1);Fresh(in);
                CHECK(get<unsigned>(owned.data(),0xF0)==4u&&!collision&&pairRemoves==1&&livingRefs==0);}
            else if(mode=="long-open-remove"){for(int i=0;i<110;++i)Fresh(in);const auto addedAt=selfPairH5.causal.submittedTick;
                CHECK(now-addedAt>1000&&selfPairH5.open);removeAutoAbsent=false;ds::DisableAll();Fresh(in);
                CHECK(!record.quarantined&&pairRemoves==1&&livingRefs==1&&!collision&&destroys==0);
                CHECK(selfPairH5.causal.submittedTick==addedAt);Fresh(in);CHECK(!record.quarantined&&pairRemoves==1&&livingRefs==1);
                pairExists=false;Fresh(in);CHECK(livingRefs==0&&pairRemoves==1&&!record.quarantined);}
            else if(mode=="remove-timeout"){removeAutoAbsent=false;ds::DisableAll();Fresh(in);const auto original=selfPairH5.removeTick;CHECK(original&&livingRefs==1&&!record.quarantined);
                now+=1001;Fresh(in);CHECK(record.quarantined&&selfPairH5.removeTick==original&&pairRemoves==1&&livingRefs==1&&destroys==0);}
            else if(mode=="double-toggle-removal"){removeAutoAbsent=false;ds::DisableAll();Fresh(in);const auto removal=selfPairH5.removeTick;
                ds::SetWeaponNpcContactsH5(true);ds::SetWeaponNpcContactsH5(false);ds::SetWeaponNpcContactsH5(true);Fresh(in);
                CHECK(pairRemoves==1&&pairAdds==1&&classes0==1&&selfPairH5.removeTick==removal&&!record.quarantined);
                pairExists=false;Fresh(in);CHECK(livingRefs==0&&pairRemoves==1&&selfPairH5.id==0);}
            else if(mode=="remove-refused"){removeReturn=0;ds::DisableAll();Fresh(in);CHECK(record.quarantined&&pairRemoves==1&&livingRefs==1&&!collision&&destroys==0);Tick();CHECK(pairRemoves==1);}
            else if(mode=="remove-fault"){removeFault=true;ds::DisableAll();Fresh(in);CHECK(record.quarantined&&pairRemoves==1&&livingRefs==1&&!collision&&destroys==0);Tick();CHECK(pairRemoves==1);}
            else if(mode=="release-fault"){releaseFault=true;ds::DisableAll();Fresh(in);CHECK(record.quarantined&&pairRemoves==1&&livingRefs==1&&!collision&&destroys==0);}
            else if(mode=="class-close-refused"){failClass4=true;ds::DisableAll();Fresh(in);CHECK(record.quarantined&&pairRemoves==0&&livingRefs==1&&destroys==0&&get<unsigned>(owned.data(),0xF0)==4u);}
            else if(mode=="detach-refused"){detachRefusal=true;ds::DisableAll();Fresh(in);CHECK(record.quarantined&&pairRemoves==0&&livingRefs==1&&destroys==0&&get<unsigned>(owned.data(),0xF0)==4u);}
            else if(mode=="already-absent"){pairExists=false;Fresh(in);CHECK(pairRemoves==0&&livingRefs==0&&!collision&&get<unsigned>(owned.data(),0xF0)==4u);}
            else if(mode=="unknown-present"){pairKnown=false;Fresh(in);CHECK(!collision&&pairRemoves==0&&livingRefs==1&&!record.quarantined);pairKnown=true;Drain(in);CHECK(pairRemoves==1&&livingRefs==0);}
            else if(mode=="external-pin"||mode=="external-pin-reset"){pinOwner=in.owner;pinGeneration=record.incarnation;pinOperation=1;Fresh(in);CHECK(record.constraintPinned);
                if(mode=="external-pin-reset"){
                    // Existing authenticated Reset exception: do not invent an
                    // external constraint's Absent or unpin before this call.
                    const auto destroyedBefore=destroys;set(world.data(),0x5FF4,0u);
                    p::Access::Dispatch(world.data(),p::Phase::WorldReset);
                    CHECK(pairRemoves==1&&livingRefs==0&&ownedRefs==0&&destroys==destroyedBefore&&!record.quarantined);
                    CHECK(!selfPairH5.living.Held()&&selfPairH5.id==0);
                }else{
                    ds::DisableAll();Fresh(in);CHECK(classes4>0&&!record.destroyRequested&&collision&&pairRemoves==0&&livingRefs==1);
                    pinOperation=-1;Fresh(in);CHECK(!record.constraintPinned);Drain(in);CHECK(pairRemoves==1&&livingRefs==0);
                }}
            else if(mode=="reset"){set(world.data(),0x5FF4,0u);p::Access::Dispatch(world.data(),p::Phase::WorldReset);CHECK(pairRemoves==1&&livingRefs==0&&ownedRefs==0&&!record.quarantined);}
            else if(mode=="shutdown"){p::RequestShutdown();Tick();Tick();CHECK(pairRemoves==1&&livingRefs==0&&ownedRefs==0&&!record.quarantined);}
            else if(mode=="replaced"){selectedWorld=world2.data();p::Access::Dispatch(selectedWorld,p::Phase::Step);CHECK(record.quarantined&&pairRemoves==0&&livingReleases==0&&!record.lease.Held()&&!selfPairH5.living.Held());}
            else if(mode=="wrong-pair"){pairWrong=true;Fresh(in);CHECK(record.quarantined&&get<unsigned>(owned.data(),0xF0)==4u&&!collision&&livingRefs==1&&pairRemoves==0);}
            else if(mode=="source-loss"){predicate=false;Fresh(in);CHECK(!collision&&pairRemoves==1&&livingRefs==0&&get<unsigned>(owned.data(),0xF0)==4u);}
            else if(mode=="henry-replaced"){registeredHenry=nullptr;Fresh(in);CHECK(!collision&&pairRemoves==1&&livingRefs==0&&get<unsigned>(owned.data(),0xF0)==4u);}
            else if(mode=="tracking-loss"){in.tracked=false;Fresh(in);CHECK(!collision&&pairRemoves==1&&livingRefs==0);}
            else if(mode=="menu-loss"){in.allowed=false;Fresh(in);CHECK(!collision&&pairRemoves==1&&livingRefs==0);}
            else if(mode=="offhand-item"){in.emptyLeft=false;Fresh(in);CHECK(!collision&&pairRemoves==1&&livingRefs==0);}
            else if(mode=="cancel"){w::Cancel();Tick();CHECK(!collision&&pairRemoves==1&&livingRefs==0);}
            else if(mode=="f9-off"){w::SetEnabled(false);Tick();CHECK(!collision&&pairRemoves==1&&livingRefs==0&&!w::Enabled());}
            else if(mode=="request-change"){++in.owner.request;Fresh(in);CHECK(!collision&&pairRemoves==1&&livingRefs==0);}
            else if(mode=="item-change"){++in.owner.key;++in.owner.generation;Fresh(in);CHECK(!collision&&pairRemoves==1&&livingRefs==0);}
            else if(mode=="body-epoch-change"){++in.owner.epoch;Fresh(in);CHECK(!collision&&pairRemoves==1&&livingRefs==0);}
            else if(mode=="tracking-epoch-change"){++in.trackingEpoch;Fresh(in);CHECK(!collision&&pairRemoves==1&&livingRefs==0);}
            else if(mode=="policy-epoch-change"){++in.cancelGeneration;Fresh(in);CHECK(!collision&&pairRemoves==1&&livingRefs==0);}
            else if(mode=="collider-change"){++in.colliderLease;Fresh(in);CHECK(!collision&&pairRemoves==1&&livingRefs==0);}
            else if(mode=="packet-stale"){now+=81;Tick();CHECK(!collision&&pairRemoves==1&&livingRefs==0);}
            else if(mode=="same-address-aba"){const auto priorIncarnation=record.incarnation;ds::DisableAll();Fresh(in);Drain(in);CHECK(record.incarnation>priorIncarnation&&registeredOwned==owned.data()&&get<unsigned>(owned.data(),0xF0)==4u);
                ds::SetWeaponNpcContactsH5(true);CHECK(ds::WeaponNpcContactsEnabledH5());Fresh(in);CHECK(pairAdds==2&&livingRefs==1&&classes0==1);Fresh(in);CHECK(classes0==2);}
            else if(mode=="aba"){ds::SetWeaponNpcContactsH5(false);ds::SetWeaponNpcContactsH5(true);CHECK(ds::ReadWeaponNpcContactControlH5()!=word);
                Fresh(in,false);Drain(in,false);CHECK(classes0==1&&pairAdds==1&&pairRemoves==1&&livingRefs==0);}
            else if(mode=="lifecycle"){events.clear();ds::DisableAll();w::Achieved immediate{};CHECK(w::Read(immediate)&&!immediate.poseValid);Fresh(in);CHECK(!collision&&pairRemoves==1&&livingRefs==0);
                Drain(in);CHECK(collision&&get<unsigned>(owned.data(),0xF0)==4u&&pairAdds==1);CHECK(Before("class4","detach")&&Before("detach","remove-pair")&&Before("remove-pair","release-henry"));}
            else return 2;
        }
    }
    std::printf("weapon self-pair %s: %u/%u checks PASS\n",mode.c_str(),checks,checks);return 0;
}
