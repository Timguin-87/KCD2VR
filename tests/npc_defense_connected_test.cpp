// Executes the actual native callbacks. Only native memory and native leaf
// calls are substituted; no game, native patch, or headset result is simulated.
#define KCD2VR_NPC_DEFENSE_HOST_TEST 1
#include <array>
#include <atomic>
#include <cmath>
#include <cstdio>
#include <cstring>
#include <stdexcept>
#include <unordered_map>
#include <vector>
#include <cstdint>
static std::unordered_map<std::uintptr_t,unsigned char> memory;
static bool HostNpcDefenseRead(std::uintptr_t at,void* value,std::size_t n){
    auto* bytes=static_cast<unsigned char*>(value);
    for(std::size_t i=0;i<n;++i){const auto it=memory.find(at+i);if(it==memory.end())return false;bytes[i]=it->second;}
    return true;
}
static std::uintptr_t testReturnPc=0;
static std::uintptr_t HostNpcDefenseReturnAddress(){return testReturnPc;}
#include "subsystems/direct/npc_defense.cpp"

namespace nd=kcd2vr::npc_defense;
using Address=std::uintptr_t;
constexpr Address base=0x180000000ull,npc=0x10000000,state=0x10010000,timingClock=0x10020000,
    manager=0x10030000,slots=0x10040000,action=0x10050000,animation=0x10060000,
    henry=0x11000000,henryState=0x11010000,controller=0x12000000;
static unsigned checks=0,enterCalls=0;
static std::vector<float> rates;
static bool nativeAccepted=true,insideSetter=false,insideFollower=false,throwNative=false,stopInside=false,replaceInside=false;
template<class T> static void Put(Address at,const T& value){const auto* p=reinterpret_cast<const unsigned char*>(&value);for(std::size_t i=0;i<sizeof(T);++i)memory[at+i]=p[i];}
static void Bytes(Address at,const void* value,std::size_t n){const auto* p=static_cast<const unsigned char*>(value);for(std::size_t i=0;i<n;++i)memory[at+i]=p[i];}
static void Check(bool ok,const char* what){++checks;if(!ok){std::fprintf(stderr,"FAIL %u: %s\n",checks,what);std::exit(3);}}
static float ReadFloat(Address at){float f=-999;Check(HostNpcDefenseRead(at,&f,4),"fixture source float exists");return f;}
static void __fastcall NativeRate(void*,float rate){rates.push_back(rate);Put(controller+0x9C,rate);}
static bool __fastcall NativeEnter(void*){
    ++enterCalls;
    if(throwNative)throw std::runtime_error("native exception");
    if(insideSetter){testReturnPc=base+0x1648C08;nd::SetRate(reinterpret_cast<void*>(animation),1.25f);}
    if(insideFollower){testReturnPc=base+0x9E3D38;nd::SetRate(reinterpret_cast<void*>(animation),1.f);}
    if(replaceInside){Put(slots,Address(action+0x1000));Put(action+0x1040,manager);}
    if(stopInside)nd::Shutdown();
    return nativeAccepted;
}
static void Reset(){
    memory.clear();rates.clear();enterCalls=0;nativeAccepted=true;insideSetter=insideFollower=throwNative=stopInside=replaceInside=false;
    nd::image=base;nd::ready=true;nd::speed=1.5f;nd::blockSpeed=1.4f;nd::parrySpeed=1.33f;nd::dodgeSpeed=1.25f;nd::raiseExperiment=false;nd::enterOriginal=&NativeEnter;nd::rateOriginal=&NativeRate;
    Put(npc,Address(base+0x3E3FAC8));Put(npc+0x210,state);Put(state+0xCE0,npc);Put(npc+0x200,manager);
    Put(npc+0x3C0,timingClock);Put(timingClock+8,npc);Put(timingClock+0x10,1.25f);Put(npc+0x3A0,animation);Put(animation,npc);
    Put(manager+0x38,slots);Put(manager+0x40,slots+8);Put(slots,action);Put(action+0x40,manager);
    Put(action,Address(base+0x3A89688));Put(action+0x68,npc);Put(action+0x88,npc);Put(state+0x38,128);
    Put(state+0xCF8,henry);Put(henry,Address(base+0x3A80610));Put(henry+0x210,henryState);Put(henryState+0xCE0,henry);
    const std::array<unsigned char,16> prop{0,0,0,0,0,0,0,0,0,0,0,0,0x11,0x22,0x33,0x44};
    Bytes(henryState+0x450,prop.data(),prop.size());Put(henryState+0x450,Address(base+0x3B0A790));Put(henryState+0x458,.3f);
    Put(controller,Address(base+0x40519E8));Put(controller+0x9C,1.25f);
}
static DetourModKit::hook::MidContext InitialContext(){
    DetourModKit::hook::MidContext c{};using namespace DetourModKit::hook;
    for(unsigned i=0;i<12;++i)c.registers[i]=0x70000000+i;
    gpr(c,Gpr::Rdi)=timingClock;gpr(c,Gpr::Rbx)=henry;gpr(c,Gpr::Rcx)=henryState+0x450;gpr(c,Gpr::Rax)=base+0x3B0A790;
    for(auto& x:c.vectors)for(auto& b:x.bytes)b=0x6A;
    c.stack=0x123456;c.ip=base+0xD4D632;return c;
}
static float NativePureGetter(Address property){
    // The pinned native getter is movss xmm0,[rcx+8]; ret. This executes its
    // exact source-address semantics for either native or callback-owned view.
    if(property==henryState+0x450)return ReadFloat(property+8);
    float v=0;std::memcpy(&v,reinterpret_cast<const void*>(property+8),4);return v;
}
int main(){
    using namespace DetourModKit::hook;
    Reset();
    for(const auto& pin:nd::pins)Bytes(base+pin.rva,pin.bytes,pin.size);
    Put(base+0x3B0A798,base+0x1A7DE00);Put(base+0x3B925F0,base+0x1648A28);
    Put(base+0x3B925F8,base+0x19BE238);Put(base+0x4051AC0,base+0xD4E34C);
    Check(nd::Contract(base),"all real callback pins and getter/vft edges required");
    memory[base+0x1A7DE00]^=1;Check(!nd::Contract(base),"changed pure getter refuses native activation");
    Reset();auto c=InitialContext(),old=c;nd::InitialEta(c);
    Check(gpr(c,Gpr::Rcx)!=gpr(old,Gpr::Rcx),"selected native block receives private ETA view");
    Check(std::abs(NativePureGetter(gpr(c,Gpr::Rcx))-.2f)<.000001f,"1.5 factor shortens native read once");
    Check(ReadFloat(henryState+0x458)==.3f,"Henry original ETA property never written");
    auto clone=nd::initialView.bytes;std::array<unsigned char,16> original{};HostNpcDefenseRead(henryState+0x450,original.data(),16);
    for(unsigned i=0;i<16;++i)if(i<8||i>=12)Check(clone[i]==original[i],"all other property bytes preserved");
    for(unsigned i=0;i<12;++i)if(i!=unsigned(Gpr::Rcx))Check(c.registers[i]==old.registers[i],"other captured GPR unchanged");
    Check(std::memcmp(c.vectors,old.vectors,sizeof(c.vectors))==0&&c.ip==old.ip&&c.stack==old.stack,"SIMD, instruction and stack unchanged");
    c=InitialContext();nd::InitialEta(c);Check(std::abs(NativePureGetter(gpr(c,Gpr::Rcx))-.2f)<.000001f,"repeated reads never compound private ETA");
    c=InitialContext();gpr(c,Gpr::Rbx)=timingClock;c.ip=base+0x9E3D87;nd::FollowEta(c);
    Check(std::abs(NativePureGetter(gpr(c,Gpr::Rcx))-.2f)<.000001f,"follower uses same raw original and factor");
    Check(gpr(c,Gpr::Rcx)!=reinterpret_cast<Address>(&nd::initialView),"initial and follower have separate immediate views");
    Put(action,Address(base+0x3BE12C0));c=InitialContext();nd::InitialEta(c);
    Check(gpr(c,Gpr::Rcx)!=henryState+0x450,"ordinary native PB raise shares factor");
    for(const int phase:{4,8,256,512}){Put(state+0x38,phase);c=InitialContext();nd::InitialEta(c);Check(gpr(c,Gpr::Rcx)==henryState+0x450,"only native raise prefix, other phase original");}
    Reset();Put(action,Address(base+0x3BE1FFF));c=InitialContext();nd::InitialEta(c);Check(gpr(c,Gpr::Rcx)==henryState+0x450,"unknown/sync family not claimed by shared helper");
    Reset();Put(slots,Address(action+0x1000));c=InitialContext();nd::InitialEta(c);Check(gpr(c,Gpr::Rcx)==henryState+0x450,"replaced current action not retimed");
    Reset();Put(timingClock+8,Address(henry));c=InitialContext();nd::InitialEta(c);Check(gpr(c,Gpr::Rcx)==henryState+0x450,"Henry timing remains original");
    Reset();nd::speed=1;c=InitialContext();nd::InitialEta(c);Check(gpr(c,Gpr::Rcx)==henryState+0x450,"factor1 preserves current inputs");
    Reset();nd::ready=false;c=InitialContext();nd::InitialEta(c);Check(gpr(c,Gpr::Rcx)==henryState+0x450,"unavailable hook never changes native read");
    for(const float factor:{0.f,-1.f,3.1f,NAN}){Reset();nd::speed=factor;c=InitialContext();nd::InitialEta(c);Check(gpr(c,Gpr::Rcx)==henryState+0x450,"invalid setting is native passthrough");}

    // Production-composed landmark algebra: authored motion and native ready
    // share one native rate built from this earlier incoming hit time.
    Reset();for(const float eta:{.15f,.3f,.6f})for(const float factor:{1.f,1.5f,2.f,3.f}){
        nd::speed=factor;Put(henryState+0x458,eta);c=InitialContext();nd::InitialEta(c);
        const float view=NativePureGetter(gpr(c,Gpr::Rcx));
        const float th=.733333f,ts=.437626f,rate=th/view;
        Check(std::abs(ts/rate-(ts/th*eta/factor))<.000001f,"native prefix motion and ready time scale together");
    }
    Reset();Put(action,Address(base+0x3B92428));Put(state+0x38,512);
    Check(nd::DodgeEnter(reinterpret_cast<void*>(action))&&enterCalls==1,"accepted Dodge native original exactly once");
    Check(rates.size()==1&&rates[0]==1.875f,"no-native-setter entry uses raw native working rate once");
    Check(ReadFloat(timingClock+0x10)==1.25f,"native raw timing rate remains untouched");
    rates.clear();Check(nd::DodgeEnter(reinterpret_cast<void*>(action))&&rates.size()==1&&rates[0]==1.875f,"repeated accepted entry does not multiply own controller readback");
    Reset();Put(action,Address(base+0x3B92428));Put(state+0x38,512);insideSetter=true;
    Check(nd::DodgeEnter(reinterpret_cast<void*>(action))&&rates.size()==1&&rates[0]==1.875f,"native entry rate already scaled, no post-entry second factor");
    Reset();Put(action,Address(base+0x3B92428));Put(state+0x38,512);insideFollower=true;
    Check(nd::DodgeEnter(reinterpret_cast<void*>(action))&&rates.size()==1&&rates[0]==1.5f,"nested native follower marks same entry, no double factor");
    rates.clear();testReturnPc=base+0x9E3D38;nd::SetRate(reinterpret_cast<void*>(animation),1.f);nd::SetRate(reinterpret_cast<void*>(animation),1.f);
    Check(rates==std::vector<float>({1.5f,1.5f}),"each raw follower request gets one factor");
    testReturnPc=base+0x19BE26B;nd::SetRate(reinterpret_cast<void*>(animation),1.f);
    Check(rates.back()==1.f,"native Dodge exit restore remains1");
    testReturnPc=base+0x12345;nd::SetRate(reinterpret_cast<void*>(animation),.8f);Check(rates.back()==.8f,"unknown rate writer untouched");
    Put(slots,Address(action+0x1000));testReturnPc=base+0x9E3D38;nd::SetRate(reinterpret_cast<void*>(animation),1.f);Check(rates.back()==1.f,"replacement current action gets native rate");
    Reset();Put(action,Address(base+0x3B92428));Put(state+0x38,512);nativeAccepted=false;
    Check(!nd::DodgeEnter(reinterpret_cast<void*>(action))&&enterCalls==1&&rates.empty(),"native refusal preserved, no optional rate");
    Reset();Put(action,Address(base+0x3B92428));Put(state+0x38,512);replaceInside=true;
    Check(nd::DodgeEnter(reinterpret_cast<void*>(action))&&enterCalls==1&&rates.empty(),"native accepted but replacement during callback gets no old entry write");
    Reset();Put(action,Address(base+0x3B92428));Put(state+0x38,512);Put(manager+0x40,slots);
    Check(nd::DodgeEnter(reinterpret_cast<void*>(action))&&rates.size()==1,"native entry allowed before channel installation");
    Reset();Put(action,Address(base+0x3B92428));Put(state+0x38,512);Put(slots,Address(action+0x1000));
    Put(action+0x1040,manager);
    Check(nd::DodgeEnter(reinterpret_cast<void*>(action))&&rates.size()==1,"native entry allowed while unchanged prior action awaits publication");
    Reset();Put(action,Address(base+0x3B92428));Put(state+0x38,512);Put(npc,Address(base+0x3A80610));
    Check(nd::DodgeEnter(reinterpret_cast<void*>(action))&&enterCalls==1&&rates.empty(),"Henry native acceptance preserved without NPC rate");
    Reset();Put(action,Address(base+0x3B92428));Put(state+0x38,512);throwNative=true;
    bool thrown=false;try{nd::DodgeEnter(reinterpret_cast<void*>(action));}catch(const std::runtime_error&){thrown=true;}
    Check(thrown&&enterCalls==1&&rates.empty()&&!nd::entering,"native exception propagates once; stack scope retired");
    Reset();Put(action,Address(base+0x3B92428));Put(state+0x38,512);stopInside=true;
    Check(nd::DodgeEnter(reinterpret_cast<void*>(action))&&enterCalls==1&&rates.empty(),"shutdown during entered native original preserves its return");
    Check(nd::enterOriginal.load()==&NativeEnter&&nd::rateOriginal.load()==&NativeRate,"shutdown retains original native routes");
    testReturnPc=base+0x9E3D38;nd::SetRate(reinterpret_cast<void*>(animation),1.f);Check(rates.back()==1.f,"postshutdown setter forwards raw native exactly once");
    Check(nd::DodgeEnter(reinterpret_cast<void*>(action))&&enterCalls==2,"postshutdown Enter forwards native exactly once");
    // D34 per-type defaults (legacy VrNpcDefenseSpeed=1). Native follower 9E3B80
    // retargets the kind1 peak record: rec+0xc=ETA read via [henryState+450]->vf+8,
    // rec+4=ETA+clock+54, so contact lands at the ETA the NPC reads. Model it.
    auto contactAt=[](float etaRead,float clock54){const float rec4=etaRead+clock54;return rec4-clock54;};
    for(const Address family:{Address(base+0x3A89688),Address(base+0x3BE12C0)})for(const float eta:{.15f,.3f,.43f}){
        Reset();nd::speed=1.f;Put(action,family);Put(henryState+0x458,eta);c=InitialContext();nd::InitialEta(c);
        Check(gpr(c,Gpr::Rcx)==henryState+0x450,"D34 default: block/PB raise reads native property");
        Check(contactAt(NativePureGetter(gpr(c,Gpr::Rcx)),.07f)==eta,"D34 default: contact == native block_time_to_hit");
        c=InitialContext();gpr(c,Gpr::Rbx)=timingClock;c.ip=base+0x9E3D87;nd::FollowEta(c);
        Check(gpr(c,Gpr::Rcx)==henryState+0x450,"D34 default: follower retarget reads native ETA");
    }
    Reset();nd::speed=1.f;Check(nd::DodgeFactor()==1.25f&&nd::RaiseFactor(false)==1.f&&nd::RaiseFactor(true)==1.f&&nd::Wanted(),"D34 defaults: dodge only, install wanted");
    Reset();nd::speed=1.f;Put(action,Address(base+0x3B92428));Put(state+0x38,512);
    Check(nd::DodgeEnter(reinterpret_cast<void*>(action))&&rates.size()==1&&rates[0]==1.25f*1.25f,"D34 dodge entry rate x1.25");
    rates.clear();testReturnPc=base+0x9E3D38;nd::SetRate(reinterpret_cast<void*>(animation),1.f);Check(rates==std::vector<float>({1.25f}),"D34 dodge follower rate x1.25 whole clip");
    testReturnPc=base+0x19BE26B;nd::SetRate(reinterpret_cast<void*>(animation),1.f);Check(rates.back()==1.f,"D34 dodge exit restore native");
    Reset();nd::speed=1.f;nd::dodgeSpeed=1.f;Check(!nd::Wanted()&&nd::DodgeFactor()==1.f,"all native: no install");
    for(const float bad:{0.f,.5f,3.5f,NAN,INFINITY}){Reset();nd::speed=1.f;nd::dodgeSpeed=bad;Check(nd::DodgeFactor()==1.f,"invalid dodge key native");}
    Reset();nd::speed=1.f;nd::raiseExperiment=true;c=InitialContext();nd::InitialEta(c);
    Check(std::abs(NativePureGetter(gpr(c,Gpr::Rcx))-.3f/1.4f)<.000001f,"experiment: block uses VrNpcBlockAnimSpeed");
    Put(action,Address(base+0x3BE12C0));c=InitialContext();nd::InitialEta(c);
    Check(std::abs(NativePureGetter(gpr(c,Gpr::Rcx))-.3f/1.33f)<.000001f,"experiment: parry uses VrNpcParryAnimSpeed");
    Check(contactAt(NativePureGetter(gpr(c,Gpr::Rcx)),.07f)<.3f,"experiment moves contact earlier (why it is off)");
    Reset();nd::speed=1.f;nd::raiseExperiment=false;nd::blockSpeed=3.f;nd::parrySpeed=3.f;
    Check(nd::RaiseFactor(false)==1.f&&nd::RaiseFactor(true)==1.f,"block/parry keys inert without experiment");
    Reset();nd::speed=2.f;Check(nd::DodgeFactor()==2.f&&nd::RaiseFactor(true)==2.f,"legacy VrNpcDefenseSpeed>1 keeps old shared path");
    std::printf("PASS %u production callback assertions\n",checks);return 0;
}
