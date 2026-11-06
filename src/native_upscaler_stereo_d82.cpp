#include "native_upscaler_stereo_d82.h"
#include "native_dlss_stereo_d82.h"
#include "native_fsr_stereo_d82.h"
#include "native_xess_stereo_d85.h" // D85 UPSCALERS: XeSS behind the same owner
#include "diagnostic_trace.h"
#include <atomic>
#include <cstdio>

namespace kcd2vr::native_upscaler_stereo_d82 { namespace {
std::atomic<bool> stereo{false};
std::atomic<int> technology{-1};
std::atomic<std::uint64_t> epoch{0};
std::atomic<int> epochTechnology{-1};
std::atomic<std::uint64_t> publicationSequence{0},publicationEpoch{0};
std::atomic<std::uint32_t> publicationFrame{0};
std::atomic<int> publicationTechnology{-1};
using Reader=int(*)() noexcept;
std::atomic<Reader> reader{nullptr};
int ActualTechnology() noexcept {const auto read=reader.load(std::memory_order_acquire);return read?read():technology.load(std::memory_order_acquire);}
}
void InstallEarly(bool wanted) noexcept {
    stereo.store(wanted,std::memory_order_release);
    native_dlss_stereo_d82::SetEnabled(wanted);
    native_fsr_stereo_d82::SetEnabled(wanted);
    native_xess_stereo_d85::SetEnabled(wanted);
    (void)native_dlss_stereo_d82::Install();
    (void)native_fsr_stereo_d82::Install();
    (void)native_xess_stereo_d85::Install();
}
void SetMode(bool wanted,int tech) noexcept {
    technology.store(tech,std::memory_order_release);
    stereo.store(wanted,std::memory_order_release);
    // Capture a future native creation in either backend, including a choice
    // made in the game's settings. No backend SDK is initialized by the mod.
    native_dlss_stereo_d82::SetEnabled(wanted);
    native_fsr_stereo_d82::SetEnabled(wanted);
    native_xess_stereo_d85::SetEnabled(wanted);
}
void SetTechnologyReader(Reader read) noexcept {reader.store(read,std::memory_order_release);}
bool CanEnterStereo(int tech) noexcept {
    if(tech==0)return true;
    if(tech==2)return native_dlss_stereo_d82::ReadyForStereo();
    if(tech==1)return native_fsr_stereo_d82::ReadyForStereo();
    if(tech==4)return native_xess_stereo_d85::ReadyForStereo();
    return false;
}
bool TemporalActive() noexcept {return ActualTechnology()!=0;}
bool PairReady(std::uint64_t pairEpoch,std::uint32_t frame) noexcept {
    const int tech=ActualTechnology();const auto bound=epoch.load(std::memory_order_acquire);
    // Changing the game's technology cannot turn an incomplete temporal pair
    // into an accepted Native pair. A mismatch ends the existing stereo run;
    // its normal zero-inflight restart captures the new technology/epoch.
    if(bound&&(pairEpoch!=bound||tech!=epochTechnology.load(std::memory_order_acquire)))return false;
    if(tech==0)return true;
    if(!stereo.load(std::memory_order_acquire)||!pairEpoch||pairEpoch!=bound)return false;
    if(tech==2)return native_dlss_stereo_d82::PairReady(pairEpoch,frame);
    if(tech==1)return native_fsr_stereo_d82::PairReady(pairEpoch,frame);
    if(tech==4)return native_xess_stereo_d85::PairReady(pairEpoch,frame);
    return false;
}
bool RecoverableResetCut(std::uint64_t pairEpoch,std::uint32_t frame,std::uintptr_t pipeline) noexcept {
    // Do not turn an unknown SDK failure or a mode/epoch change into a retry.
    const int tech=ActualTechnology();
    if(!pairEpoch || !pipeline || (tech!=1&&tech!=2) ||
       epochTechnology.load(std::memory_order_acquire)!=tech ||
       pairEpoch!=epoch.load(std::memory_order_acquire) || !stereo.load(std::memory_order_acquire))return false;
    const bool cut=tech==2?native_dlss_stereo_d82::RecoverableResetCut(pairEpoch,frame,pipeline):
        native_fsr_stereo_d82::RecoverableResetCut(pairEpoch,frame,pipeline);
    return cut && ActualTechnology()==tech && epochTechnology.load(std::memory_order_acquire)==tech &&
        pairEpoch==epoch.load(std::memory_order_acquire) && stereo.load(std::memory_order_acquire);
}
bool FrameReady(std::uint32_t frame) noexcept {return PairReady(epoch.load(std::memory_order_acquire),frame);}
std::uint64_t CurrentEpoch() noexcept {return epoch.load(std::memory_order_acquire);}
int BoundTechnology() noexcept {return epochTechnology.load(std::memory_order_acquire);}
bool MayPresent(PairOwner owner) noexcept {
    const int tech=ActualTechnology();
    // Preserve the proven Native0 cache policy, including native-only re-entry.
    if(tech==0&&owner.technology==0)return true;
    if(tech!=owner.technology||!owner.epoch||
       owner.epoch!=epoch.load(std::memory_order_acquire)||
       tech!=epochTechnology.load(std::memory_order_acquire)||
       !stereo.load(std::memory_order_acquire)||!CanEnterStereo(tech))return false;
    // Readiness is atomic in each backend. No vendor lock, wait, frame-age
    // timer, GPU release or finite SDK-receipt-ring lookup belongs here.
    return owner.epoch==epoch.load(std::memory_order_acquire)&&
        tech==ActualTechnology()&&stereo.load(std::memory_order_acquire);
}
void NotifyPublished(std::uint64_t value,std::uint32_t frame) noexcept {
    // DeviceHook serializes writers around the real queue publication.
    publicationSequence.fetch_add(1,std::memory_order_seq_cst);
    publicationEpoch.store(value,std::memory_order_seq_cst);
    publicationFrame.store(frame,std::memory_order_seq_cst);
    publicationTechnology.store(ActualTechnology(),std::memory_order_seq_cst);
    publicationSequence.fetch_add(1,std::memory_order_seq_cst);
}
Publication LastPublication() noexcept {
    for(unsigned attempt=0;attempt<3;++attempt){
        const auto serial=publicationSequence.load(std::memory_order_seq_cst);
        if(!serial||(serial&1u))continue;
        Publication p{serial,publicationEpoch.load(std::memory_order_seq_cst),
            publicationFrame.load(std::memory_order_seq_cst),publicationTechnology.load(std::memory_order_seq_cst)};
        if(serial==publicationSequence.load(std::memory_order_seq_cst))return p;
    }
    return {};
}
std::uint64_t PublicationSerial() noexcept {return LastPublication().serial;}
void BeginEpoch(std::uint64_t value) noexcept {
    // Called by the existing stereo start gate after prior CPU work has drained.
    epochTechnology.store(ActualTechnology(),std::memory_order_release);
    epoch.store(value,std::memory_order_release);
}
void EndEpoch(std::uint64_t expectedEpoch) noexcept {
    if(expectedEpoch)(void)epoch.compare_exchange_strong(expectedEpoch,0,std::memory_order_acq_rel);
}
void Stop() noexcept {stereo.store(false,std::memory_order_release);native_dlss_stereo_d82::Stop();native_fsr_stereo_d82::Stop();native_xess_stereo_d85::Stop();}
void LogStatus(const char* reason) noexcept {
    char line[256]{};std::snprintf(line,sizeof(line),"D82 TEMPORAL_STEREO reason=%s technology=%d stereo=%d epoch=%llu acceptance=SAME_EPOCH_FRAME_TWO_DISTINCT_VENDOR_HISTORIES",
        reason?reason:"status",ActualTechnology(),stereo.load(std::memory_order_acquire),static_cast<unsigned long long>(epoch.load(std::memory_order_acquire)));
    diagnostic_trace::Write(line);native_dlss_stereo_d82::LogStatus(reason);native_fsr_stereo_d82::LogStatus(reason);native_xess_stereo_d85::LogStatus(reason);
}
}
