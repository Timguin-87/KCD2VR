// Actual complete T4 owner/leases/status/action wrappers. Only native calls,
// module memory and current thread are test replacements, never a policy copy.
#define KCD2VR_PHYSICS_PHASE_T4_TEST 1
#include "../src/native_physics_phase_t4.cpp"
#include <cassert>
#include <cstdio>
#include <map>
#include <vector>
#include <functional>
namespace pp=kcd2vr::native_physics_phase_t4;
namespace pkt=kcd2vr::constraint_packet60;
namespace {
constexpr std::uintptr_t b=0x10000000;
std::array<unsigned char,0x6100> world{},otherWorld{};
std::array<unsigned char,0x600> corpse{},weapon{},replacement{};
int ownerCookie=0,foreignCookie=0;
void* currentWorld=world.data();void* registryCorpse=corpse.data();void* registryWeapon=weapon.data();
std::map<std::uintptr_t,std::vector<unsigned char>> module;
unsigned checks=0,failed=0,artStatus=0,artActions=0,rigidActions=0,ids=0,retains=0,releases=0;
int corpseRefs=0,weaponRefs=0,partReturn=1,constraintReturn=1,addReturn=77,removeReturn=1;
bool changeForeign=false,changePart=false,changeIndex=false,malformedPose=false,changeOnRef=false,actionFault=false,statusFault=false;
void* statusBuddy=weapon.data();
template<class T> void put(void* p,std::size_t off,T v){std::memcpy(static_cast<unsigned char*>(p)+off,&v,sizeof(v));}
template<class T> T get(const void* p,std::size_t off){T v{};std::memcpy(&v,static_cast<const unsigned char*>(p)+off,sizeof(v));return v;}
void ck(bool yes,const char* text){++checks;if(!yes){++failed;std::printf("FAIL %s\n",text);}}
template<class T> void mod(std::uintptr_t rva,const T& v){const auto* p=reinterpret_cast<const unsigned char*>(&v);module[b+rva]={p,p+sizeof(v)};}
bool read(const void* p,void* out,std::size_t n){
    const auto addr=reinterpret_cast<std::uintptr_t>(p);
    if(addr==b+0x492D830&&n==sizeof(void*)){std::memcpy(out,&currentWorld,n);return true;}
    if(auto it=module.find(addr);it!=module.end()&&it->second.size()==n){std::memcpy(out,it->second.data(),n);return true;}
    for(auto range:{std::pair{world.data(),world.size()},std::pair{otherWorld.data(),otherWorld.size()},std::pair{corpse.data(),corpse.size()},std::pair{weapon.data(),weapon.size()},std::pair{replacement.data(),replacement.size()}}){
        auto lo=reinterpret_cast<std::uintptr_t>(range.first);
        if(addr>=lo&&addr-lo<=range.second&&n<=range.second-(addr-lo)){std::memcpy(out,p,n);return true;}}
    return false;
}
void* lookup(void* w,int id){assert(w==currentWorld);return id==17?registryCorpse:id==18?registryWeapon:nullptr;}
int id(void* w,void* p){assert(w==currentWorld);++ids;return p==corpse.data()?17:p==weapon.data()?18:-1;}
int addref(void* p){++retains;int& refs=p==corpse.data()?corpseRefs:weaponRefs;++refs;
    if(changeOnRef&&p==corpse.data())put(corpse.data(),0x20,static_cast<void*>(&foreignCookie));return refs;}
int release(void* p){++releases;int& refs=p==corpse.data()?corpseRefs:weaponRefs;assert(refs>0);return --refs;}
int rigidStatus(void* p,void* s){assert(p==weapon.data());const auto type=get<int>(s,0);
    if(type==1){put(s,0x18,pp::V3{});put(s,0x3C,pp::Q{});put(s,0x4C,1.f);put(s,0x50,2);return 1;}
    assert(type==8);put(s,0xC,pp::V3{});put(s,0x18,pp::V3{});put(s,0x3C,pp::V3{});put(s,0x4C,2.f);return 1;}
int rigidAction(void*,void*,int){++rigidActions;return -1;}
int articulatedStatus(void* p,void* s){assert(p==corpse.data());++artStatus;
    if(statusFault){pp::faults.fetch_add(1);pp::nativeFaulted=true;return 0;}
    const auto type=get<int>(s,0);
    if(type==1){assert(get<int>(s,4)==379&&get<int>(s,8)==-1&&get<float>(s,0x78)==0);
        put(s,0x18,pp::V3{1,2,3});put(s,0x3C,pp::Q{});put(s,0x4C,1.f);put(s,0x50,2);
        if(changeForeign)put(corpse.data(),0x20,static_cast<void*>(&foreignCookie));
        if(changePart)put(s,4,179);if(changeIndex)put(s,8,3);if(malformedPose)put(s,0x3C,pp::Q{0,0,0,0});return partReturn;}
    assert(type==26&&get<int>(s,4)==77&&get<int>(s,8)==-1);
    if(constraintReturn>0){put(s,0xC,0x42u);put(s,0x38,statusBuddy);}return constraintReturn;
}
int articulatedAction(void* p,void* a,int safe){assert(p==corpse.data()&&safe==1);++artActions;
    if(actionFault){pp::faults.fetch_add(1);pp::nativeFaulted=true;return 77;}
    if(get<int>(a,0)==5){assert(get<int>(a,0x28)==379&&get<std::uintptr_t>(a,8)==reinterpret_cast<std::uintptr_t>(weapon.data()));return addReturn;}
    assert(get<int>(a,0)==6&&get<unsigned>(a,4)==77&&get<unsigned>(a,0x10)==1);return removeReturn;
}
std::function<void(pp::Context&)> operation;
void callback(pp::Context& c,void*) noexcept {operation(c);}
void resetCallback(pp::Context& c,void*) noexcept {operation(c);}
void initialize(){
    pp::nativeFaulted=false;pp::shutdown=false;pp::testThread=1;
    changeForeign=changePart=changeIndex=malformedPose=changeOnRef=actionFault=statusFault=false;
    partReturn=constraintReturn=removeReturn=1;addReturn=77;statusBuddy=weapon.data();
    registryCorpse=corpse.data();registryWeapon=weapon.data();currentWorld=world.data();
    corpse.fill(0);weapon.fill(0);
    for(auto body:{corpse.data(),weapon.data()}){
        put(body,0x68,currentWorld);put(body,0x50,0);put(body,0x47,std::int8_t(2));put(body,0x194,std::uint8_t(1));
        put(body,0x20,static_cast<void*>(&ownerCookie));put(body,0x28,std::int16_t(1));}
    put(corpse.data(),0,b+0x3A48B30);put(weapon.data(),0,b+0x3A48DC0);
    put(world.data(),0,b+0x407FC80);put(world.data(),0x5FDC,0x10000u);put(world.data(),0x5FF4,0x10000u);
    put(world.data(),0x10D0,0u);put(world.data(),0x5FD4,0);
}
pp::ArticulatedSource source(){return {-1,379,corpse.data(),&ownerCookie,1};}
auto packet(){auto p=pkt::defaults();pkt::setBuddy(p,reinterpret_cast<std::uintptr_t>(weapon.data()));pkt::setPartIds(p,379,pkt::kIntUnset);return p;}
void run(std::function<void(pp::Context&)> f){operation=std::move(f);pp::Access::Dispatch(currentWorld,pp::Phase::Step);}
void pins(){
    for(auto [table,off,target]:std::initializer_list<std::array<std::uintptr_t,3>>{
        {0x407FC80,0xB0,0x667418},{0x407FC80,0xA8,0x52950C},
        {0x3A48DC0,0x10,0x5311C4},{0x3A48DC0,0x18,0x531F84},{0x3A48DC0,0x30,0x8A6C90},{0x3A48DC0,0x38,0xAC0D18},
        {0x3A48B30,8,0x1A72E50},{0x3A48B30,0x10,0x5311C4},{0x3A48B30,0x18,0x531F84},{0x3A48B30,0x30,0x8A9348},{0x3A48B30,0x38,0xA99DF0},{0x3A48B30,0x50,0x5D6F44}})mod(table+off,b+target);
    mod(0x8A9348,std::array<unsigned char,11>{0x48,0x8B,0xC4,0x48,0x89,0x58,0x08,0x48,0x89,0x70,0x18});
    mod(0xA99DF0,std::array<unsigned char,11>{0x48,0x8B,0xC4,0x48,0x89,0x58,0x08,0x48,0x89,0x50,0x10});
    mod(0xA99E5F,std::array<unsigned char,6>{0x0F,0x84,0xC5,0x5C,0x7D,0x01});mod(0x226FB2A,std::array<unsigned char,5>{0xE8,0xE9,0x11,0x85,0xFE});
    mod(0xA9A978,std::array<unsigned char,5>{0xE8,0x9B,0x63,0x02,0});mod(0x8A9CC0,std::array<unsigned char,5>{0xE8,0xCB,0xCF,0xFF,0xFF});
}
}
int main(){
    pp::base=b;pp::testRead=&read;pp::api={&lookup,&id,&addref,&release,&rigidStatus,&rigidAction,&articulatedStatus,&articulatedAction};
    pins();initialize();ck(pp::RegisterClient({&callback,&resetCallback,nullptr}),"one actual T4 client registers");pp::installed=true;
    pp::ArticulatedLease art;pp::Lease buddy;
    run([&](pp::Context& c){
        ck(pp::AcquireArticulated(c,source(),art)&&art.Held()&&art.Id()==17&&art.Part()==379&&ids==1,"native world id and exact lookup acquire a retained articulated part");
        ck(corpseRefs==1,"actual AddRef retained exactly once");pp::RigidSnapshot wrong{};pp::Lease no;
        ck(!pp::AcquireRigid(c,17,corpse.data(),no),"rigid acquisition still rejects articulated bodies");
        ck(!pp::AcquireArticulated(c,source(),art)&&corpseRefs==1,"held articulated lease cannot acquire twice");
        pp::ArticulatedSnapshot pose{};ck(pp::ReadArticulatedPart(c,art,pose)&&pose.part==379&&pose.position.z==3,"actual articulated GetStatus exact raw part decodes current pose");
        ck(pp::AcquireRigid(c,18,weapon.data(),buddy)&&weaponRefs==1,"buddy is a separately retained rigid body");
        auto p=packet();auto added=pp::AddArticulatedConstraint(c,art,buddy,p);
        ck(added.kind==pp::ConstraintResult::Kind::Submitted&&added.id==77&&artActions==1&&!rigidActions,"typed articulated Action receives type5 immediately, never rigid dispatch");
        pp::ConstraintSnapshot k{};ck(pp::ReadArticulatedConstraint(c,art,77,k)&&k.exists&&k.buddy==weapon.data(),"type26 Present reads original id and native buddy");
        auto removed=pp::RemoveArticulatedConstraint(c,art,buddy,77);ck(removed.kind==pp::ConstraintResult::Kind::Submitted&&!removed.id,"type6 positive return is status, never fabricated id/Absent");
        ck(pp::ReadArticulatedConstraint(c,art,77,k)&&k.exists,"remove return is NOT Absent acknowledgment");
        constraintReturn=0;ck(pp::ReadArticulatedConstraint(c,art,77,k)&&!k.exists&&!k.buddy,"only actual status0 acknowledges original constraint Absent");constraintReturn=1;
    });
    run([&](pp::Context& c){
        const auto before=artActions;statusBuddy=replacement.data();ck(pp::RemoveArticulatedConstraint(c,art,buddy,77).kind==pp::ConstraintResult::Kind::Refused&&artActions==before,"wrong current buddy cannot remove a reused id");statusBuddy=weapon.data();
        constraintReturn=-1;pp::ConstraintSnapshot k{};ck(!pp::ReadArticulatedConstraint(c,art,77,k)&&!k.exists,"negative native status stays unknown, not Absent");constraintReturn=1;
        removeReturn=0;ck(pp::RemoveArticulatedConstraint(c,art,buddy,77).kind==pp::ConstraintResult::Kind::Refused,"native remove refusal stays refusal");removeReturn=1;
        ck(pp::ReadArticulatedConstraint(c,art,77,k)&&k.exists,"refused removal keeps current id/status available to its owner");
        auto p=packet();pkt::setPartIds(p,179,pkt::kIntUnset);ck(pp::AddArticulatedConstraint(c,art,buddy,p).kind==pp::ConstraintResult::Kind::Refused,"cannot redirect original articulated part");
        p=packet();pkt::setId(p,77);ck(pp::AddArticulatedConstraint(c,art,buddy,p).kind==pp::ConstraintResult::Kind::Refused,"new articulated Add cannot inject an existing constraint id");
        p=packet();pkt::setIgnorePairOnly(p);ck(pp::AddArticulatedConstraint(c,art,buddy,p).kind==pp::ConstraintResult::Kind::Refused,"inactive ignore-pair packet is not a joint");
        p=packet();pkt::setBuddy(p,1);ck(pp::AddArticulatedConstraint(c,art,buddy,p).kind==pp::ConstraintResult::Kind::Refused,"arbitrary buddy pointer cannot bypass retained rigid lease");
        pp::testThread=2;const auto statusBefore=artStatus;pp::ArticulatedSnapshot out{};
        ck(!pp::ReadArticulatedPart(c,art,out)&&artStatus==statusBefore,"wrong native callback thread cannot call status");pp::testThread=1;
        for(int mode=0;mode<4;++mode){partReturn=mode==0?0:1;changePart=mode==1;changeIndex=mode==2;malformedPose=mode==3;
            ck(!pp::ReadArticulatedPart(c,art,out)&&out.part==-1,"missing/remapped/indexed/malformed part cannot authorize a pose");}
        partReturn=1;changePart=changeIndex=malformedPose=false;
        put(corpse.data(),0x20,static_cast<void*>(&foreignCookie));ck(!pp::ReadArticulatedPart(c,art,out),"changed foreign owner revokes part admission");
        ck(pp::ReadArticulatedConstraint(c,art,77,k)&&k.exists,"retained-body cleanup still reads own constraint after foreign owner changes");
        ck(pp::RemoveArticulatedConstraint(c,art,buddy,77).kind==pp::ConstraintResult::Kind::Submitted,"current matching buddy permits cleanup after semantic owner revocation");
        put(corpse.data(),0x20,static_cast<void*>(&ownerCookie));partReturn=0;
        ck(pp::RemoveArticulatedConstraint(c,art,buddy,77).kind==pp::ConstraintResult::Kind::Submitted,"cleanup does not require deleted part to remain admissible");partReturn=1;
        registryWeapon=replacement.data();ck(pp::RemoveArticulatedConstraint(c,art,buddy,77).kind==pp::ConstraintResult::Kind::Submitted,"opaque independently retained buddy identity survives registry retirement for cleanup only");registryWeapon=weapon.data();
        registryCorpse=replacement.data();ck(pp::ObserveRegistry(c,art)==pp::Registry::Retired&&!pp::ReadArticulatedConstraint(c,art,77,k),"source registry retirement is unknown for mutation, never fabricated Absent");registryCorpse=corpse.data();
        ck(pp::Release(c,art)&&pp::Release(c,buddy)&&!art.Held()&&!corpseRefs&&!weaponRefs,"both independent refs release in current owner context");
    });
    for(int mode=0;mode<9;++mode){initialize();run([&](pp::Context& c){auto s=source();const auto old=retains;
        if(mode==0)s.part=-1;if(mode==1)s.foreign=&foreignCookie;if(mode==2)s.foreignType=2;if(mode==3)registryCorpse=replacement.data();
        if(mode==4)put(corpse.data(),0,b+0x3A82570);if(mode==5)put(corpse.data(),0x50,1);if(mode==6)put(corpse.data(),0x84,0x80000000u);
        if(mode==7){put(world.data(),0x10D0,1u);put(world.data(),0x5FD4,1);}if(mode==8)changeForeign=true;
        pp::ArticulatedLease denied;ck(!pp::AcquireArticulated(c,s,denied)&&!denied.Held()&&retains==old,"bad current source/owner/part/insertion/reference gate refuses BEFORE retain");
    });}
    initialize();run([&](pp::Context& c){changeOnRef=true;ck(!pp::AcquireArticulated(c,source(),art)&&art.Held()&&corpseRefs==1,"successful AddRef followed by owner change stays retained for cleanup");changeOnRef=false;ck(pp::Release(c,art)&&!corpseRefs,"partial acquisition's actual reference is retired, not forgotten");});
    initialize();run([&](pp::Context& c){
        auto original=module[b+0x3A48B30+0x38];mod(0x3A48B30+0x38,b+0xAC0D18);const auto before=artStatus;
        ck(!pp::AcquireArticulated(c,source(),art)&&artStatus==before,"articulated source cannot use a substituted rigid Action slot");module[b+0x3A48B30+0x38]=original;
        pp::ArticulatedSource rigid=source();rigid.body=weapon.data();rigid.physicsId=18;
        ck(!pp::AcquireArticulated(c,rigid,art),"rigid body cannot be acquired through articulated API");
        auto explicitId=source();explicitId.physicsId=17;const auto idCount=ids;
        ck(pp::AcquireArticulated(c,explicitId,art)&&ids==idCount,"explicit known physics id still requires actual lookup, no guessed offsets");
        ck(pp::AcquireRigid(c,18,weapon.data(),buddy),"shutdown test buddy acquired");
    });
    operation=[&](pp::Context& c){
        ck(c.Kind()==pp::Phase::Shutdown,"actual shutdown dispatch supplies locked cleanup context");
        auto p=packet();const auto before=artActions;
        ck(pp::AddArticulatedConstraint(c,art,buddy,p).kind==pp::ConstraintResult::Kind::Refused&&artActions==before,"shutdown cannot admit new articulated constraints");
        ck(pp::RemoveArticulatedConstraint(c,art,buddy,77).kind==pp::ConstraintResult::Kind::Submitted,"shutdown can drain an exact current owned constraint");
        ck(pp::Release(c,art)&&pp::Release(c,buddy),"shutdown current-world cleanup can release retained refs");
    };pp::RequestShutdown();pp::Access::Dispatch(currentWorld,pp::Phase::Step);
    initialize();run([&](pp::Context& c){ck(pp::AcquireArticulated(c,source(),art)&&pp::AcquireRigid(c,18,weapon.data(),buddy),"fault test leases retained");actionFault=true;auto r=pp::AddArticulatedConstraint(c,art,buddy,packet());
        ck(r.kind==pp::ConstraintResult::Kind::Uncertain&&!r.id&&r.nativeReturn==77,"native action exception cannot invent an accepted id");auto count=artActions;
        ck(pp::AddArticulatedConstraint(c,art,buddy,packet()).kind==pp::ConstraintResult::Kind::Refused&&artActions==count,"latched native fault cannot replay uncertain add");
        ck(!pp::Release(c,art)&&art.Held(),"faulted native release is not claimed as completed");});
    // Native world replacement revokes old references without touching old memory.
    put(otherWorld.data(),0,b+0x407FC80);put(otherWorld.data(),0x5FDC,0x10000u);put(otherWorld.data(),0x5FF4,0x10000u);currentWorld=otherWorld.data();
    operation=[&](pp::Context& c){if(c.Kind()!=pp::Phase::WorldReplaced)return;auto count=releases;
        ck(!pp::Release(c,art)&&pp::AbandonInvalidated(c,art)&&!art.Held(),"new world abandons unconfirmed old articulated reference explicitly");
        ck(pp::AbandonInvalidated(c,buddy)&&releases==count,"replacement never calls old-world native release");};
    pp::Access::Dispatch(currentWorld,pp::Phase::Step);
    std::printf("H21 actual articulated T4 bridge=%u/%u native_leaves=REPLACED headset=UNTESTED\n",checks-failed,checks);return failed?1:0;
}
