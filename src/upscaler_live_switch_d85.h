#pragma once
// D85 UPSCALERS: restart-free upscaler switch.
//
// The game itself switches upscalers live: its options applier 0x181DD95D0
// writes r_SuperResolution_Mode through ICVar::Set (vtable +0x38), the CVar is
// registered at 0x18101341F without a change callback, and each vendor plugin
// creates/releases its context at its own points (XeSS create 0x181E0B750,
// size setter 0x181E10EC0 -> slot 4 teardown 0x181E10B30, DLSS teardown
// 0x182556310 with GPU flush 0x1805087A8 before ReleaseFeature, FSR teardown
// 0x181E10C20 with the same flush before ffxDestroyContext).
//
// The mod never creates or releases a vendor context itself. The per-eye
// bridges (DLSS/FSR D82, XeSS D85) create the RIGHT context inside the game's
// own create call and release it inside the game's own release call. This
// switch only orders the change so it never happens under a running stereo
// pair: stop the stereo run, wait until its native work drained, write the
// selection in the R27 dependency order, then hold the stereo autostart until
// the selected technology has a captured LEFT/RIGHT pair again. Release that
// hold to start a new stereo epoch, and report success only after that epoch
// actually publishes a pair through the graphics queue. If that pair is
// not created live, it says so in one line (menu + KCD2VR.log); the saved
// user.cfg value still applies on the next start.
#include <atomic>
#include <cstdint>
#include <cstdio>

namespace kcd2vr::upscaler_live_switch_d85 {
struct Selection {int tech=-1,quality=0,preset=0;};
enum class Phase : int {Idle=0,Stopping=1,Waiting=2,Done=3,Failed=4,StartingStereo=5};
struct Publication {std::uint64_t serial{},epoch{},currentEpoch{};int technology{-1};};
enum class Why : int {None=0,StopTimeout=1,WriteRefused=2,PairNotCreated=3,OutputNotConfirmed=4};
// Time the game gets to render with the new value before an already existing
// pair is accepted. It is not a correctness gate: a pair the game releases
// later is withdrawn by the bridges' release hooks and PairReady refuses it.
constexpr std::uint64_t kSettleMs=750,kStopTimeoutMs=5000,kCreateTimeoutMs=8000;

class Switch {
    std::atomic<std::uint64_t> pending_{0}; // bit 63 set = request; tech/quality/preset packed
    std::atomic<int> phase_{0},why_{0},tech_{-1};
    std::atomic<bool> hold_{false};
    Selection current_{};std::uint64_t deadline_=0,settle_=0,publicationBaseline_=0,epochBaseline_=0;bool stopIssued_=false;std::atomic<bool> stereoExpected_{false};
    static std::uint64_t Pack(Selection s) noexcept {
        return (1ull<<63)|(std::uint64_t(std::uint8_t(s.tech))<<16)|(std::uint64_t(std::uint8_t(s.quality))<<8)|std::uint8_t(s.preset);
    }
    static Selection Unpack(std::uint64_t v) noexcept {
        return {int(std::int8_t(v>>16)),int(std::int8_t(v>>8)),int(std::int8_t(v))};
    }
    template<class Log> void Finish(Phase p,Why w,Log&& log) noexcept {
        phase_.store(int(p),std::memory_order_release);why_.store(int(w),std::memory_order_release);
        hold_.store(false,std::memory_order_release);
        char line[200]{};std::snprintf(line,sizeof(line),"D85 UPSCALER_LIVE_SWITCH tech=%d quality=%d preset=%d result=%s reason=%s savedForRestart=1",
            current_.tech,current_.quality,current_.preset,p==Phase::Done?(stereoExpected_.load(std::memory_order_acquire)?"STEREO_PUBLICATION_CONFIRMED":"SETTING_APPLIED"):"OUTPUT_UNCONFIRMED",ReasonText(w));
        log(line);
    }
public:
    static const char* ReasonText(Why w) noexcept {
        return w==Why::StopTimeout?"stereo-run-did-not-stop":w==Why::WriteRefused?"game-refused-setting"
            :w==Why::PairNotCreated?"per-eye-pair-not-created-live":w==Why::OutputNotConfirmed?"setting-applied-stereo-output-unconfirmed":"none";
    }
    // Menu thread. The newest request wins; nothing native happens here.
    void Request(Selection s) noexcept {pending_.store(Pack(s),std::memory_order_release);}
    bool HoldsStereo() const noexcept {return hold_.load(std::memory_order_acquire)||(pending_.load(std::memory_order_acquire)>>63);}
    Phase CurrentPhase() const noexcept {return Phase(phase_.load(std::memory_order_acquire));}
    Why LastWhy() const noexcept {return Why(why_.load(std::memory_order_acquire));}
    int Technology() const noexcept {return tech_.load(std::memory_order_acquire);}
    // Worker thread only (the Mod::Run loop, same thread as the stereo
    // autostart). Leaves: busy() = stereo run or its drain active; stop() =
    // existing native run stop request; write(sel) = CVar Set in R27 order with
    // read-back; ready(tech) = aggregate CanEnterStereo; log(line).
    template<class Busy,class Stop,class Write,class Ready,class Published,class Log>
    void Step(std::uint64_t nowMs,bool stereoWanted,Busy&& busy,Stop&& stop,Write&& write,Ready&& ready,Published&& publication,Log&& log) noexcept {
        const auto phase=CurrentPhase();
        if(phase!=Phase::Stopping&&phase!=Phase::Waiting&&phase!=Phase::StartingStereo){
            const auto request=pending_.exchange(0,std::memory_order_acq_rel);
            if(!(request>>63))return;
            current_=Unpack(request);stereoExpected_=stereoWanted;tech_.store(current_.tech,std::memory_order_release);
            hold_.store(true,std::memory_order_release); // before any native step
            why_.store(0,std::memory_order_release);stopIssued_=false;deadline_=nowMs+kStopTimeoutMs;
            phase_.store(int(Phase::Stopping),std::memory_order_release);
        }
        if(CurrentPhase()==Phase::Stopping){
            if(busy()){
                if(!stopIssued_){stop();stopIssued_=true;}
                if(nowMs>=deadline_)Finish(Phase::Failed,Why::StopTimeout,log);
                return;
            }
            if(!write(current_)){Finish(Phase::Failed,Why::WriteRefused,log);return;}
            if(!stereoWanted){Finish(Phase::Done,Why::None,log);return;}
            settle_=nowMs+kSettleMs;deadline_=nowMs+kCreateTimeoutMs;
            phase_.store(int(Phase::Waiting),std::memory_order_release);return;
        }
        if(CurrentPhase()==Phase::Waiting){
            if(nowMs>=settle_&&ready(current_.tech)){
                const auto before=publication();publicationBaseline_=before.serial;epochBaseline_=before.currentEpoch;
                deadline_=nowMs+kCreateTimeoutMs;
                phase_.store(int(Phase::StartingStereo),std::memory_order_release);
                // Autostart must run before a real stereo pair can arrive.
                hold_.store(false,std::memory_order_release);return;
            }
            if(nowMs>=deadline_)Finish(Phase::Failed,Why::PairNotCreated,log);
        }
        if(CurrentPhase()==Phase::StartingStereo){
            const auto visible=publication();
            if(visible.serial>publicationBaseline_&&visible.epoch&&visible.epoch!=epochBaseline_&&
               visible.epoch==visible.currentEpoch&&visible.technology==current_.tech){
                Finish(Phase::Done,Why::None,log);return;
            }
            if(nowMs>=deadline_)Finish(Phase::Failed,Why::OutputNotConfirmed,log);
        }
    }
    // One short line for the Graphics page; nullptr when there is nothing new.
    const wchar_t* MenuNote() const noexcept {
        switch(CurrentPhase()){
        case Phase::Stopping:case Phase::Waiting:return L"SWITCHING UPSCALER LIVE...";
        case Phase::StartingStereo:return L"SETTING APPLIED: WAITING FOR STEREO OUTPUT";
        case Phase::Done:return stereoExpected_.load(std::memory_order_acquire)?L"LIVE STEREO OUTPUT CONFIRMED":L"SETTING APPLIED";
        case Phase::Failed:
            return LastWhy()==Why::OutputNotConfirmed?L"SETTING APPLIED: STEREO OUTPUT UNCONFIRMED":LastWhy()==Why::PairNotCreated?L"NOT SWITCHED LIVE: RESTART THE GAME"
                :LastWhy()==Why::WriteRefused?L"GAME REFUSED SETTING: RESTART THE GAME":L"STEREO DID NOT STOP: RESTART THE GAME";
        default:return nullptr;
        }
    }
};
inline Switch& Instance() noexcept {static Switch instance;return instance;}
}
