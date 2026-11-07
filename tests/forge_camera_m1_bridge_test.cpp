// Actual production Update/View/read API; only native memory/clock and external
// hooks are host fixtures. No game function or DLL is executed.
#include "../src/activity_lens59c10r1.cpp"
#include <array>
#include <cassert>
#include <vector>
namespace kcd2vr::retail_build112 {
bool Verify(std::uintptr_t) noexcept {return false;}
bool Ready(std::uintptr_t) noexcept {return true;}
}
namespace kcd2vr::diagnostic_trace {void Write(const char*) noexcept {}}
namespace kcd2vr::frik_pose_probe {
void ObserveNativeDialogueHead28(const dialogue_state59c10r2::State&,std::uint64_t) noexcept {}
}
namespace {
unsigned originals=0;
void NativeView(void*,void* output){++originals;const float native=.872664626f;std::memcpy(static_cast<unsigned char*>(output)+0x30,&native,4);}
template<class T,std::size_t N>void Put(std::array<unsigned char,N>& bytes,std::size_t offset,T value){
    assert(offset+sizeof(value)<=N);std::memcpy(bytes.data()+offset,&value,sizeof(value));
}
template<std::size_t N>std::uintptr_t Addr(std::array<unsigned char,N>& bytes){return reinterpret_cast<std::uintptr_t>(bytes.data());}
}
int main(){
    using namespace kcd2vr;using namespace activity_lens59c10r1;
    // Only the actual pinned global offset needs a large image, zero initialized
    // as plain data. Every pointer followed by View is owned fixture memory.
    std::vector<std::uintptr_t> image((0x549D390/sizeof(std::uintptr_t))+1);
    base=reinterpret_cast<std::uintptr_t>(image.data());
    std::array<unsigned char,0x900> player{};std::array<unsigned char,0x220> actor{};
    std::array<unsigned char,0x160> game{};std::array<unsigned char,0x30> module{};
    std::array<unsigned char,0x100> dialogs{};std::array<unsigned char,0x400> cameras{};
    std::array<unsigned char,0x40> forge{},output{};
    Put(player,0,base+0x3AA4B98);Put(player,0x40,base+0x3A81028);
    Put(player,0x38,std::uintptr_t(0x1234));Put(player,0x278,Addr(actor));
    Put(actor,0,base+0x3A80610);Put(actor,0x1F8,Addr(player));
    image[0x549D388/8]=Addr(game);Put(game,0x150,Addr(module));Put(game,0x38,Addr(cameras));
    Put(module,0,base+0x3E64C38);Put(module,0x10,Addr(dialogs));Put(dialogs,0,base+0x3E64C90);
    Put(cameras,0,base+0x408E560);Put(cameras,0x30,Addr(cameras)+0x1E8);
    Put(cameras,0x1E8,base+0x3A5A0F8);Put(cameras,0x338,base+0x408EB60);
    Put(cameras,0x350,base+0x3A5A3D0);
    Put(forge,0,base+0x3F6FA30);Put(forge,0x18,Addr(player));Put(forge,0x2C,std::uint8_t(1));
    auto& w=activity_input163::witness175;w.Clear();w.ready=true;
    w.Begin(activity_input163::Kind175::ForgeBuilder,Addr(forge),hostTickCount64);
    original=&NativeView;
    forgeObserverReady.store(true); // Model an already installed callback, never install a native hook.
    diagnostics::capture168::gate.select(diagnostics::capture168::Domain::Hud,false,hostTickCount64);
    diagnostics::capture168::gate.select(diagnostics::capture168::Domain::Interaction,false,hostTickCount64);
    const auto frame=[&]{View(reinterpret_cast<void*>(Addr(player)+0x40),output.data());};
    const auto fov=[&]{float value=0;std::memcpy(&value,output.data()+0x30,4);return value;};
    Update(false,97,hostTickCount64);frame();
    assert(originals==1&&ForgeCameraCurrent(hostTickCount64)); // Initial Theater policy cannot block positive observation.
    assert(fov()==.872664626f); // Observation itself never changes FOV.
    Update(true,97,hostTickCount64);frame();
    assert(originals==2&&std::fabs(fov()-97*.01745329251994329577f)<1e-6f);
    Put(cameras,0x30,Addr(cameras)+0x350);frame();
    assert(originals==3&&!ForgeCameraCurrent(hostTickCount64)&&fov()==.872664626f);
    Put(cameras,0x30,Addr(cameras)+0x338);Update(false,97,hostTickCount64);frame();
    assert(ForgeCameraCurrent(hostTickCount64)); // Real Minigame provider is independently qualified.
    assert(!ForgeCameraCurrent(hostTickCount64+81));
    w.End(Addr(forge));w.Begin(activity_input163::Kind175::ForgeBuilder,Addr(forge),hostTickCount64);
    assert(!ForgeCameraCurrent(hostTickCount64)); // Same-address lifetime replacement cannot reuse the receipt.
    frame();assert(ForgeCameraCurrent(hostTickCount64));
    Put(forge,0x18,Addr(player)+8);frame();assert(!ForgeCameraCurrent(hostTickCount64));
    Put(forge,0x18,Addr(player));frame();assert(ForgeCameraCurrent(hostTickCount64));
    { // Native scalar qualification can succeed while its separate lifecycle
      // changes. Exercise the actual final production write gate, not a second
      // test-side approximation of its atomic owner checks.
        const auto expectedLife=w.lifetime175.load();
        NativeView(nullptr,output.data());
        auto racingRead=[&](std::uintptr_t address,auto& value){
            const bool ok=Read(address,value);
            if(address==Addr(cameras)+0x338)w.lifetime175.fetch_add(1);
            return ok;
        };
        const auto qualified=qualify(base,Addr(player)+0x40,{},racingRead,Addr(forge));
        assert(qualified.camera==Owner::Minigame&&!ForgeWriteCurrent(Addr(forge),expectedLife));
        if(ForgeWriteCurrent(Addr(forge),expectedLife))WriteFov(output.data(),1.7f);
        assert(fov()==.872664626f);
        const auto currentLife=w.lifetime175.load();
        auto shutdownRead=[&](std::uintptr_t address,auto& value){
            const bool ok=Read(address,value);
            if(address==Addr(cameras)+0x338)w.ready.store(false);
            return ok;
        };
        assert(qualify(base,Addr(player)+0x40,{},shutdownRead,Addr(forge)).camera==Owner::Minigame);
        assert(!ForgeWriteCurrent(Addr(forge),currentLife));
        if(ForgeWriteCurrent(Addr(forge),currentLife))WriteFov(output.data(),1.7f);
        assert(fov()==.872664626f);w.ready=true;
    }
    Uninstall();assert(!ForgeCameraCurrent(hostTickCount64));
    std::puts("PASS actual ForgeBuilder camera production Update/View/read: policy-off observation, final FOV, immediate invalid-owner revoke, stale/lifetime/shutdown refusal; no game execution");
}
