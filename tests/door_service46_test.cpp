// Execute the complete worker lifecycle include against the real private door
// channel. Only hook installation/readiness and native Stop/Start are leaves.
#include "../src/core/door_use46.h"
#include "../src/core/door_channel19c4r4.h"
#include "../src/subsystems/doors/module.cpp"
#include <cassert>
#include <cstdio>
#include <string>
#include <vector>
static std::uint64_t now46=1000;
static std::uint64_t GetTickCount64(){return now46;}
static int interaction46=0;
static bool calibrated46=true,actionHook46=true,retail46=true,ownerHook46=true;
static bool stopped46=true,installAllowed46=true,installed46=false;
static unsigned starts46=0,stops46=0,installs46=0;
static std::vector<std::string> logs46;
static int MiscInteractionDiagMode(){return interaction46;}
static bool CalibrationReady(){return calibrated46;}
static bool InteractionActionReady175(){return actionHook46;}
static std::uintptr_t WhBase(){return 0x10000000;}
namespace retail_build112 {static bool Ready(std::uintptr_t){return retail46;}}
namespace action_time_owner59c11 {static bool Installed(){return ownerHook46;}}
namespace diagnostic_trace {static void Write(const char* message) noexcept{logs46.emplace_back(message);}}
namespace kcd2vr::door_signal19c4r4 {
static door_channel19c4r4::Channel channel;
static bool Ready() noexcept{return installed46&&channel.enabled();}
static bool SetEnabled(bool on) noexcept{
    if(on&&!installed46){++installs46;if(!installAllowed46)return false;installed46=true;}
    if(channel.enabled()!=on)channel.enabled(on);
    return !on||installed46;
}
}
static void StartNativeDoor19c4() noexcept{++starts46;stopped46=false;}
static bool StopNativeDoor19c4() noexcept{++stops46;stopped46=true;
    (void)kcd2vr::door_signal19c4r4::SetEnabled(false);
    return !kcd2vr::door_signal19c4r4::channel.pending();
}
static void Service46(){
#include "../src/vrik/door_service46.inl"
}
static unsigned checks46=0;
static void check46(bool value){++checks46;assert(value);}
int main(){
    using namespace kcd2vr;
    auto& channel=door_signal19c4r4::channel;
    Service46();check46(!stopped46&&channel.enabled()&&installs46==1);
    interaction46=1;Service46();check46(stopped46&&!channel.enabled());
    interaction46=2;Service46();check46(!stopped46&&channel.enabled()&&installs46==1);
    // No recording/experiment state is needed by this production include.
    const unsigned stopBaseline=stops46;
    const auto normalReceipts=logs46.size();
    for(int i=0;i<5;++i){now46+=20;Service46();check46(!stopped46&&channel.enabled()&&stops46==stopBaseline&&installs46==1);}
    check46(logs46.size()==normalReceipts);
    // OFF publishes no new command, but cannot abandon a possibly applied pause.
    check46(channel.publish({2,1,42,1,.2f},now46,0x1234));check46(channel.claim(2,1,now46));
    door_use46::SetEnabled(false);Service46();check46(stopped46&&!channel.enabled()&&channel.pending());
    now46+=1000;check46(channel.acknowledge(2,1,0,200000,now46)&&!channel.pending());
    door_use46::SetEnabled(true);Service46();check46(!stopped46&&channel.enabled()&&installs46==1);
    // Each independently lost prerequisite revokes the channel without uninstall.
    for(int lost=0;lost<5;++lost){
        if(lost==0)interaction46=1;if(lost==1)calibrated46=false;if(lost==2)actionHook46=false;
        if(lost==3)retail46=false;if(lost==4)ownerHook46=false;
        Service46();check46(stopped46&&!channel.enabled()&&installs46==1);
        interaction46=2;calibrated46=actionHook46=retail46=ownerHook46=true;
        Service46();check46(!stopped46&&channel.enabled()&&installs46==1);
    }
    // Pin/hook refusal keeps native doors available and retries at most 1 Hz.
    interaction46=1;Service46();installed46=false;installAllowed46=false;interaction46=2;
    const auto retryReceipts=logs46.size();
    Service46();check46(stopped46&&!channel.enabled()&&installs46==2);
    check46(logs46.size()==retryReceipts+1&&logs46.back().find("bridge=UNAVAILABLE")!=std::string::npos);
    now46+=999;Service46();check46(stopped46&&installs46==2);
    ++now46;Service46();check46(stopped46&&installs46==3);
    check46(logs46.size()==retryReceipts+1); // retries are bounded; the same refusal is not logged again
    installAllowed46=true;now46+=1000;Service46();check46(!stopped46&&channel.enabled()&&installs46==4);
    check46(logs46.size()==retryReceipts+2&&logs46.back().find("bridge=READY")!=std::string::npos);
    std::printf("door_service46: %u checks PASS; actual normal-mode lifecycle and retained Lua cleanup channel\n",checks46);
}
