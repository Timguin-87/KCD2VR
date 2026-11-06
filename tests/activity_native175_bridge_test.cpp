#include "../src/activity_input163.cpp"
#include "../src/activity_eye59c19.h"
#include <cassert>
#include <cstdio>
namespace {unsigned lines175=0;}
namespace kcd2vr::diagnostic_trace{void Write(const char*)noexcept{++lines175;}}
#ifdef _MSC_VER
// MSVC retains installer COMDAT references; this fixture never installs hooks.
namespace kcd2vr::retail_build112 {
 bool Verify(std::uintptr_t) noexcept {return false;}
 bool Ready(std::uintptr_t) noexcept {return false;}
}
// Installer-only native leaves are not exercised by this lifecycle fixture.
namespace kcd2vr::frik_pose_probe {
 bool ReadSmithContactHammer68(kcd2vr::subsystems::smithing::contact68::HammerFrame&,std::uint64_t) noexcept {return false;}
 std::uintptr_t FindSmithContactEntity68(std::uint32_t) noexcept {return 0;}
}
#endif
namespace {
unsigned starts175=0,stops175=0,destroys175=0,target175=0;
void*started175=nullptr;bool accept175=true;unsigned smithCallsC1=0;
bool retailNestedStop175=false;
void SmithSpyC1(void*){++smithCallsC1;}
void StartSpy175(void*self,unsigned target){
    assert(kcd2vr::activity_input163::witness175.transitionDepth175.load()>=1);
    ++starts175;started175=self;target175=target;
    // Retail9F0983 ->226506A ->derived vft+28 ->9F1150 BEFORE Start reactivates.
    if(retailNestedStop175&&static_cast<unsigned char*>(self)[0x2C])
        kcd2vr::activity_input163::NativeStop175(self);
    if(accept175)static_cast<unsigned char*>(self)[0x2C]=1;
}
void StopSpy175(void*self){assert(kcd2vr::activity_input163::witness175.transitionDepth175.load()>=1);++stops175;static_cast<unsigned char*>(self)[0x2C]=0;}
void DestroySpy175(void*self){assert(kcd2vr::activity_input163::witness175.transitionDepth175.load()>=1);++destroys175;std::memset(self,0,0x70);}
template<class T,std::size_t N>void Put175(std::array<unsigned char,N>&a,unsigned offset,T value){
    assert(offset+sizeof(value)<=a.size());std::memcpy(a.data()+offset,&value,sizeof(value));
}
}
int main(){
    using namespace kcd2vr;using namespace activity_input163;using namespace ui_native_state;
    alignas(16)std::array<unsigned char,0x70> player{},otherPlayer{};
    alignas(16)std::array<unsigned char,0xA0> alchemy{},alchemyNext{},smithing{},reading{},unknown{};
    alignas(16)std::array<unsigned char,0x300> smithContext{};
    alignas(16)std::array<unsigned char,0x38> smithListener{};
    constexpr std::uintptr_t base=0x50000000;
    Put175(player,0,base+0x3AA4B98);Put175(player,0x38,std::uintptr_t(0x1230));
    Put175(otherPlayer,0,base+0x3AA4B99);Put175(otherPlayer,0x38,std::uintptr_t(0x2340));
    const auto prepare=[&](auto&instance,std::uintptr_t vt){
        Put175(instance,0,base+vt);Put175(instance,0x18,reinterpret_cast<std::uintptr_t>(player.data()));};
    prepare(alchemy,0x3F60B08);prepare(alchemyNext,0x3F60B08);
    prepare(smithing,0x3A6A390);prepare(reading,0x3F749E0);prepare(unknown,0x3F60B09);
    base175.store(base);witness175.Clear();witness175.ready.store(true);
    originalStart175=StartSpy175;originalStop175=StopSpy175;originalDestroy175=DestroySpy175;
    diagnostics::capture168::gate.select(diagnostics::capture168::Domain::Interaction,false,hostTickCount64);
    Snapshot ui{};ui.known=ui.visible=Bit(AlchemyBook)|Bit(GeneralBook);
    assert(!Read(ui).nativeControls); // a lingering book texture is not an activity
    assert(Read(ui,true).nativeControls); // explicit fallback remains available
    NativeStart175(alchemy.data(),177);
    assert(starts175==1&&started175==alchemy.data()&&target175==177&&Read(ui).nativeControls);
    NativeStart175(alchemyNext.data(),178);NativeStop175(alchemy.data());
    assert(stops175==1&&Read(ui).nativeControls); // old stop cannot release its replacement
    NativeDestroy175(alchemy.data());assert(destroys175==1&&Read(ui).nativeControls);
    NativeStop175(alchemyNext.data());assert(!Read(ui).nativeControls);
    // Actual native start must have acquired active+2C. A rejected request,
    // another subtype, an NPC/unknown player, or absent entity cannot own input.
    prepare(alchemy,0x3F60B08);accept175=false;NativeStart175(alchemy.data(),0);
    assert(!Read(ui).nativeControls);accept175=true;
    NativeStart175(unknown.data(),0);assert(!Read(ui).nativeControls);
    Put175(alchemy,0x18,reinterpret_cast<std::uintptr_t>(otherPlayer.data()));
    NativeStart175(alchemy.data(),0);assert(!Read(ui).nativeControls);
    Put175(alchemy,0x18,reinterpret_cast<std::uintptr_t>(player.data()));Put175(player,0x38,std::uintptr_t(0));
    NativeStart175(alchemy.data(),0);assert(!Read(ui).nativeControls);Put175(player,0x38,std::uintptr_t(0x1230));
    NativeStart175(smithing.data(),70);NativeStart175(reading.data(),80);
    NativeStop175(smithing.data());assert(Read(ui).nativeControls);
    NativeDestroy175(reading.data());assert(!Read(ui).nativeControls); // unload/save teardown
    prepare(unknown,0x3F74BA0);NativeStart175(unknown.data(),81);
    assert(Read(ui).nativeControls&&witness175.active[unsigned(Kind175::Sharpening)].load());
    NativeStop175(unknown.data());assert(!Read(ui).nativeControls);
    NativeStart175(unknown.data(),82);NativeDestroy175(unknown.data());assert(!Read(ui).nativeControls);
    NativeStart175(alchemy.data(),90);assert(Read(ui).nativeControls);
    Put175(alchemy,0x18,reinterpret_cast<std::uintptr_t>(otherPlayer.data()));
    NativeStart175(alchemy.data(),91);assert(!Read(ui).nativeControls); // reused self, invalid new owner
    // The real smithing callback publishes only its exact current player instance.
    originalSmithMapC1=SmithSpyC1;
    NativeStart175(smithing.data(),92);
    const auto smithSelf=reinterpret_cast<std::uintptr_t>(smithing.data());
    Put175(smithing,0x68,base+0x3A6A320);Put175(smithing,0x98,reinterpret_cast<std::uintptr_t>(smithContext.data()));
    Put175(smithContext,0,base+0x3A6A280);Put175(smithContext,0x80,base+0x3A6A208);
    Put175(smithListener,0,base+0x3F67880);Put175(smithListener,8,smithSelf+0x68);
    assert(witness175.SmithPhaseC1()==-1);
    for(int phase=0;phase<=20;++phase){
        smithContext[0x88]=static_cast<unsigned char>(phase);NativeSmithMapC1(smithListener.data());
        assert(witness175.SmithPhaseC1()==phase);
        assert(AnvilPhaseC1(phase)==(phase==11||phase==14||phase==16||phase==17));
    }
    assert(smithCallsC1==21); // every original forwarded once, even for ineligible stages
    smithContext[0x88]=11;NativeSmithMapC1(smithListener.data());assert(AnvilPhaseC1(witness175.SmithPhaseC1()));
    Put175(smithListener,8,smithSelf+0x70);NativeSmithMapC1(smithListener.data());
    assert(witness175.SmithPhaseC1()==11); // unrelated listener cannot replace the witness
    Put175(smithListener,8,smithSelf+0x68);Put175(smithContext,0x80,base+0x3A6A209);
    NativeSmithMapC1(smithListener.data());assert(witness175.SmithPhaseC1()==-1); // current object's invalid layout revokes permission
    Put175(smithContext,0x80,base+0x3A6A208);smithContext[0x88]=255;
    NativeSmithMapC1(smithListener.data());assert(witness175.SmithPhaseC1()==-1);
    smithContext[0x88]=11;NativeSmithMapC1(smithListener.data());
    const auto oldSample=witness175.smithSampleC1.load();NativeStop175(smithing.data());
    assert(witness175.SmithPhaseC1()==-1);NativeStart175(smithing.data(),93);
    witness175.smithSampleC1.store(oldSample);assert(witness175.SmithPhaseC1()==-1); // same-address lifetime reuse
    NativeSmithMapC1(smithListener.data());assert(witness175.SmithPhaseC1()==11);
    NativeDestroy175(smithing.data());assert(witness175.SmithPhaseC1()==-1);
    prepare(smithing,0x3A6A390);
    assert(lines175==0); // capture OFF does not format/write callback diagnostics
    diagnostics::capture168::gate.select(diagnostics::capture168::Domain::Interaction,true,hostTickCount64);
    NativeStart175(smithing.data(),92);assert(Read(ui).nativeControls&&lines175==1);
    UninstallNative175();assert(!witness175.ready.load()&&!Read(ui).nativeControls);
    assert(!witness175.active[0].load()&&!witness175.active[1].load()&&!witness175.active[2].load());
    NativeStart175(smithing.data(),93);assert(!Read(ui).nativeControls); // in-flight shutdown event cannot reacquire
    { // R59.16C10: the held-B stage epoch follows witnessed activity lifetimes only (zero without the witness).
        assert(StageEpoch59c10()==0);
        witness175.Clear();witness175.ready.store(true);const auto idle=StageEpoch59c10();assert(idle!=0);
        witness175.Begin(Kind175::Alchemy,0x1000);const auto alchemy=StageEpoch59c10();assert(alchemy!=idle);
        assert(StageEpoch59c10()==alchemy); // stable while nothing changes (no per-frame rearm)
        witness175.Begin(Kind175::Reading,0x2000);const auto reading=StageEpoch59c10();assert(reading!=alchemy); // alchemy book opened inside alchemy = a stage
        assert(witness175.End(0x2000)==Kind175::Reading&&StageEpoch59c10()==alchemy); // back to the bench = the previous stage value
        witness175.Begin(Kind175::Smithing,0x3000);const auto smith=StageEpoch59c10();witness175.smithEpochC1.fetch_add(1);assert(StageEpoch59c10()!=smith);
        witness175.Clear();witness175.ready.store(false);assert(StageEpoch59c10()==0);
    }
    assert(witness175.transitionDepth175.load()==0);
    {TransitionScope19 one;assert(witness175.transitionDepth175.load()==1);
        {TransitionScope19 two;assert(witness175.transitionDepth175.load()==2);}
        assert(witness175.transitionDepth175.load()==1);}
    assert(witness175.transitionDepth175.load()==0);
    try{TransitionScope19 guarded;throw 1;}catch(int){}
    assert(witness175.transitionDepth175.load()==0); // exception cannot poison neutral learning
    { // Exercise actual production hooks with the independently verified native
      // restart ordering, not a helper-only fake that skips its nested Stop.
        diagnostics::capture168::gate.select(diagnostics::capture168::Domain::Interaction,false,hostTickCount64);
        witness175.Clear();witness175.ready=true;prepare(reading,0x3F749E0);reading[0x2C]=0;
        retailNestedStop175=true;NativeStart175(reading.data(),101);
        const auto life=witness175.lifetime175.load();
        NativeStart175(reading.data(),102);
        assert(witness175.lifetime175.load()==life+2);
        assert(witness175.active[unsigned(Kind175::Reading)].load()==reinterpret_cast<std::uintptr_t>(reading.data()));
        activity_eye59c19::State previous{},next{};
        previous.entity=next.entity=reinterpret_cast<std::uintptr_t>(player.data());
        previous.instances[2]=next.instances[2]=reinterpret_cast<std::uintptr_t>(reading.data());
        previous.lifetime=life;next.lifetime=witness175.lifetime175.load();
        assert(!activity_eye59c19::nestedContinuation(previous,next));
        assert(witness175.transitionDepth175.load()==0);
        NativeStop175(reading.data());retailNestedStop175=false;witness175.ready=false;
    }
    { // Construction uses the real existing Start/Stop/dtor callbacks. A
      // lingering ForgeBuilderPlan texture cannot acquire this witness.
        witness175.Clear();witness175.ready=true;
        prepare(unknown,0x3F6FA30);unknown[0x2C]=0;
        const auto idle=StageEpoch59c10();
        NativeStart175(unknown.data(),110);
        assert(ForgeBuilderActive()&&Read(ui).nativeControls&&!TrackedHands59c7f());
        assert(StageEpoch59c10()!=idle);
        for(const auto& slot:witness175.active)assert(!slot.load());
        const auto life=witness175.lifetime175.load();
        const auto oldStage=StageEpoch59c10();
        retailNestedStop175=true;NativeStart175(unknown.data(),111);
        assert(ForgeBuilderActive()&&witness175.lifetime175.load()==life+2);
        assert(StageEpoch59c10()!=oldStage); // same-address native restart is a new exit stage.
        retailNestedStop175=false;
        NativeStop175(unknown.data());assert(!ForgeBuilderActive()&&!Read(ui).nativeControls);
        accept175=false;NativeStart175(unknown.data(),112);assert(!ForgeBuilderActive());accept175=true;
        Put175(unknown,0x18,reinterpret_cast<std::uintptr_t>(otherPlayer.data()));
        NativeStart175(unknown.data(),113);assert(!ForgeBuilderActive());
        prepare(unknown,0x3F6FA30);NativeStart175(unknown.data(),114);assert(ForgeBuilderActive());
        NativeDestroy175(unknown.data());assert(!ForgeBuilderActive()&&!Read(ui).nativeControls);
        prepare(unknown,0x3F6FA30);NativeStart175(unknown.data(),115);
        {TransitionScope19 transition;assert(!ForgeBuilderActive());}
        UninstallNative175();assert(!ForgeBuilderActive()&&!witness175.forgeBuilder.load());
    }
    witness175.Clear();witness175.ready=true;base175.store(base);
    originalStart175=StartSpy175;originalStop175=StopSpy175;originalDestroy175=DestroySpy175;
    prepare(unknown,0x3F72098);unknown[0x2C]=0;
    NativeStart175(unknown.data(),99);
    assert(witness175.dice.load()==reinterpret_cast<std::uintptr_t>(unknown.data()));
    assert(Read(ui).nativeControls&&!TrackedHands59c7f());
    NativeStop175(unknown.data());assert(!witness175.dice.load()&&!Read(ui).nativeControls);
    NativeStart175(unknown.data(),100);NativeDestroy175(unknown.data());
    assert(!witness175.dice.load()&&!Read(ui).nativeControls);
    std::puts("PASS complete production minigame native callbacks: accepted state, exact owner/type, stop/swap, destruction/save teardown, unknown owner, shutdown and F5-OFF quiet; offline retail pins separate, no game/headset execution");
}
