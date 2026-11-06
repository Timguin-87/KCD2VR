#pragma once
// D85 UPSCALERS: XeSS per-eye history, same contract as the reviewed D82 FSR
// runtime (fsr_stereo_runtime_d82.h): one native LEFT context, one SDK-created
// RIGHT context, success receipts per (epoch,frame), pair continuity and a
// queue-safe PairReady. The aggregate native_upscaler_stereo_d82 stays the only
// owner of mode/epoch policy; this file only owns XeSS routing.
#include <array>
#include <atomic>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <mutex>

namespace kcd2vr::xess_stereo_runtime_d85 {
// Public XeSS 1.x D3D12 ABI as used by WHGame 0x6A350E20 (libxess.dll import
// table at RVA 0x3A052C0). Only the members the bridge reads/writes are named
// with meaning; the layout sizes are witnessed by the game's own call sites:
//  - execute wrapper 0x181DFFC40 builds a 0x88-byte block at rsp+0x30, reset as
//    uint32 at +0x3C (0x181DFFF8E movzbl [r14+0x48] -> [rsp+0x6C]), input size at
//    +0x40/+0x44, zeroes +0x48..+0x87 (0x181DFFFC3..0x181DFFFF2);
//  - init wrapper 0x181E00170 builds a 0x40-byte block at rsp+0x20 with the
//    pipeline library at +0x38 (0x181E001F1);
//  - SetVelocityScale gets (ctx, xmm1, xmm2) at 0x181E00208..0x181E00215.
using Context=void*;          // xess_context_handle_t
using Result=std::int32_t;    // xess_result_t, XESS_RESULT_SUCCESS == 0
constexpr Result kOk=0,kErrorUnknown=-1000; // XESS_RESULT_ERROR_UNKNOWN
struct Coord {std::uint32_t x,y;};
struct InitParams {
    std::uint32_t outputWidth,outputHeight;std::int32_t quality;std::uint32_t initFlags;
    std::uint32_t creationNodeMask,visibleNodeMask;void* tempBufferHeap;std::uint64_t bufferHeapOffset;
    void* tempTextureHeap;std::uint64_t textureHeapOffset;void* pipelineLibrary;
};
struct ExecuteParams {
    void* color;void* velocity;void* depth;void* exposureScaleTexture;void* responsiveMask;void* output;
    float jitterX,jitterY,exposureScale;std::uint32_t resetHistory;std::uint32_t inputWidth,inputHeight;
    Coord inputColorBase,inputMotionVectorBase,inputDepthBase,inputResponsiveMaskBase,reserved0,outputColorBase;
    void* descriptorHeap;std::uint32_t descriptorHeapOffset;
};
static_assert(sizeof(InitParams)==0x40&&offsetof(InitParams,pipelineLibrary)==0x38);
static_assert(sizeof(ExecuteParams)==0x88&&offsetof(ExecuteParams,jitterX)==0x30);
static_assert(offsetof(ExecuteParams,resetHistory)==0x3C&&offsetof(ExecuteParams,inputWidth)==0x40);
static_assert(offsetof(ExecuteParams,inputColorBase)==0x48&&offsetof(ExecuteParams,descriptorHeap)==0x78);
using PfnCreate=Result(*)(void* device,Context* out);
using PfnInit=Result(*)(Context,const InitParams*);
using PfnExecute=Result(*)(Context,void* commandList,const ExecuteParams*);
using PfnVelocityScale=Result(*)(Context,float,float);
using PfnDestroy=Result(*)(Context);

struct View { bool active=false,valid=false;unsigned eye=0;std::uint64_t epoch=0;std::uint32_t frame=0;std::uintptr_t pipeline=0; };
struct Api { PfnCreate create=nullptr;PfnInit init=nullptr;PfnExecute execute=nullptr;PfnVelocityScale velocity=nullptr;PfnDestroy destroy=nullptr; };
struct Status { std::uint64_t creates=0,twins=0,inits=0,left=0,right=0,refused=0,destroyed=0;unsigned live=0;bool enabled=false; };

class Runtime {
    struct Slot {
        Context native=nullptr,right=nullptr;
        bool initialized=false,valid=true;bool dirty[2]{true,true};std::uint64_t epoch[2]{};
        std::uintptr_t pipeline[2]{};std::uint32_t frame[2]{};bool seen[2]{};
        std::uint64_t generation=0;
    };
    struct Receipt {
        std::uint64_t epoch=0,generation=0;std::uint32_t frame=0;std::uintptr_t pipeline=0;
        unsigned mask=0,resetMask=0;bool failed=false,used=false;
    };
    struct PublishedReceipt {
        std::atomic<std::uint64_t> sequence{0},epoch{0},routingGeneration{0};
        std::atomic<std::uint32_t> frame{0},flags{0};
    };
    Api api_{};std::array<Slot,8> slots_{};std::array<Receipt,32> receipts_{};
    std::array<PublishedReceipt,32> published_{};
    std::atomic<bool> enabled_{false},ready_{false};
    std::atomic<std::uint64_t> routingGeneration_{1};
    std::size_t cursor_=0;std::uint64_t generation_=0;Status status_{};
    mutable std::recursive_mutex mutex_;
    void Publish(const Receipt& r) noexcept {
        auto& p=published_[static_cast<std::size_t>(&r-receipts_.data())];
        p.sequence.fetch_add(1,std::memory_order_seq_cst);
        p.epoch.store(r.epoch,std::memory_order_seq_cst);p.frame.store(r.frame,std::memory_order_seq_cst);
        p.routingGeneration.store(routingGeneration_.load(std::memory_order_seq_cst),std::memory_order_seq_cst);
        p.flags.store(!r.used?0u:((r.mask&3u)|(r.failed?4u:0u)|8u),std::memory_order_seq_cst);
        p.sequence.fetch_add(1,std::memory_order_seq_cst);
    }
    void FailGeneration(const Slot& s) noexcept {for(auto& r:receipts_)if(r.used&&r.generation==s.generation){r.failed=true;Publish(r);}}
    void RefreshReady() noexcept {
        bool found=false;
        if(api_.create&&api_.init&&api_.execute&&api_.destroy)
            for(const auto& s:slots_)found|=s.valid&&s.initialized&&s.native&&s.right&&s.native!=s.right;
        ready_.store(found,std::memory_order_release);
    }
    Slot* Find(Context c) noexcept {for(auto& s:slots_)if(s.native==c&&c)return &s;return nullptr;}
    Receipt& Record(const View& v,const Slot* s) noexcept {
        for(auto& r:receipts_)if(r.used&&r.epoch==v.epoch&&r.frame==v.frame){
            if(!s||r.generation!=s->generation||r.pipeline!=v.pipeline){r.failed=true;Publish(r);}
            return r;
        }
        auto& r=receipts_[cursor_++%receipts_.size()];r={};r.used=true;r.epoch=v.epoch;r.frame=v.frame;r.pipeline=v.pipeline;
        if(s)r.generation=s->generation;
        Publish(r);return r;
    }
    Result Refuse(const View& v,Slot* s) noexcept {
        if(s)s->dirty[0]=s->dirty[1]=true;
        ++status_.refused;if(v.active){auto& r=Record(v,s);r.failed=true;Publish(r);}return kErrorUnknown;
    }
    static bool InputValid(const ExecuteParams& p) noexcept {
        return p.color&&p.velocity&&p.depth&&p.output&&p.color!=p.output&&p.inputWidth&&p.inputHeight
            &&(p.resetHistory==0u||p.resetHistory==1u)
            &&std::isfinite(p.jitterX)&&std::isfinite(p.jitterY)&&std::isfinite(p.exposureScale);
    }
public:
    explicit Runtime(Api api={}) noexcept:api_(api){}
    void SetApi(Api api) noexcept {std::lock_guard lock(mutex_);api_=api;RefreshReady();}
    void SetEnabled(bool enabled) noexcept {
        std::lock_guard lock(mutex_);if(status_.enabled==enabled)return;status_.enabled=enabled;
        enabled_.store(false,std::memory_order_release);
        routingGeneration_.fetch_add(1,std::memory_order_seq_cst);
        receipts_={};cursor_=0;for(const auto& r:receipts_)Publish(r);
        for(auto& s:slots_){s.dirty[0]=s.dirty[1]=true;s.seen[0]=s.seen[1]=false;}
        enabled_.store(enabled,std::memory_order_release);
    }
    bool Ready() const noexcept {return ready_.load(std::memory_order_acquire);}
    Status GetStatus() const noexcept {std::lock_guard lock(mutex_);return status_;}
    bool PairReady(std::uint64_t epoch,std::uint32_t frame) const noexcept {
        // Queue/publication side: atomics only, no SDK call, no registry lock.
        if(!enabled_.load(std::memory_order_acquire))return false;
        const auto generation=routingGeneration_.load(std::memory_order_seq_cst);
        for(const auto& p:published_){
            const auto seq=p.sequence.load(std::memory_order_seq_cst);if(seq&1u)continue;
            const auto e=p.epoch.load(std::memory_order_seq_cst);const auto f=p.frame.load(std::memory_order_seq_cst);
            const auto flags=p.flags.load(std::memory_order_seq_cst);
            const auto receiptGeneration=p.routingGeneration.load(std::memory_order_seq_cst);
            if(seq!=p.sequence.load(std::memory_order_seq_cst))continue;
            if((flags&8u)&&e==epoch&&f==frame)return flags==11u&&receiptGeneration==generation
                &&generation==routingGeneration_.load(std::memory_order_seq_cst)&&enabled_.load(std::memory_order_acquire);
        }
        return false;
    }
    Result Create(void* device,Context* out) noexcept {
        std::lock_guard lock(mutex_);if(!api_.create)return kErrorUnknown;
        const auto result=api_.create(device,out);++status_.creates;
        if(result!=kOk||!out||!*out||!status_.enabled)return result;
        Slot* slot=nullptr;for(auto& s:slots_)if(!s.native&&!s.right){slot=&s;break;}
        if(!slot||Find(*out)){++status_.refused;return result;}
        Context right=nullptr;
        const auto twin=api_.create(device,&right); // same live device, separate SDK context
        if(twin!=kOk||!right||right==*out){
            // An alias is the game's context; never destroy it as our twin.
            if(right&&right!=*out&&api_.destroy)api_.destroy(right);
            ++status_.refused;RefreshReady();return result;
        }
        *slot={};slot->native=*out;slot->right=right;slot->generation=++generation_;
        ++status_.twins;++status_.live;RefreshReady();return result;
    }
    Result Init(Context context,const InitParams* params) noexcept {
        std::lock_guard lock(mutex_);if(!api_.init)return kErrorUnknown;
        const auto result=api_.init(context,params);auto* s=Find(context);
        if(!s||!s->right)return result;
        // (Re)initialization restarts both histories at the game's own point.
        FailGeneration(*s);s->generation=++generation_;s->dirty[0]=s->dirty[1]=true;s->seen[0]=s->seen[1]=false;
        s->initialized=false;
        if(result==kOk){
            const auto r=api_.init(s->right,params); // same live descriptor, separate context
            if(r==kOk){s->initialized=true;s->valid=true;++status_.inits;}
            else{s->valid=false;++status_.refused;}
        }
        RefreshReady();return result;
    }
    Result SetVelocityScale(Context context,float x,float y) noexcept {
        std::lock_guard lock(mutex_);if(!api_.velocity)return kErrorUnknown;
        const auto result=api_.velocity(context,x,y);auto* s=Find(context);
        if(result==kOk&&s&&s->right){
            const auto r=api_.velocity(s->right,x,y);
            if(r!=kOk){s->valid=false;s->dirty[0]=s->dirty[1]=true;FailGeneration(*s);++status_.refused;RefreshReady();}
        }
        return result;
    }
    Result Execute(Context context,void* commandList,const ExecuteParams* params,View view) noexcept {
        std::lock_guard lock(mutex_);if(!api_.execute)return kErrorUnknown;
        Slot* s=Find(context);
        if(!status_.enabled||!view.active){
            // Mono/menu work advances only the game's context; the next
            // immersive pair resets BOTH histories.
            if(s){s->dirty[0]=s->dirty[1]=true;s->seen[0]=s->seen[1]=false;}
            return api_.execute(context,commandList,params);
        }
        if(!view.valid||view.eye>1||!view.epoch||!view.pipeline||!s||!s->valid||!s->initialized||!s->right||!params||!commandList)
            return Refuse(view,s);
        auto& receipt=Record(view,s);
        if(receipt.failed||(receipt.mask&(1u<<view.eye)))return Refuse(view,s);
        if(view.eye==1&&!(receipt.mask&1u))return Refuse(view,s);
        if(!InputValid(*params))return Refuse(view,s);
        // Copy of the documented execute block only; resources, sizes, jitter
        // and exposure stay native. Only a per-eye discontinuity adds a reset.
        ExecuteParams execute=*params;
        const unsigned eye=view.eye;
        if(eye==0){
            bool broken=false;
            for(unsigned e=0;e<2;++e)broken|=s->dirty[e]||!s->seen[e]||s->epoch[e]!=view.epoch
                ||s->pipeline[e]!=view.pipeline||std::uint32_t(s->frame[e]+1u)!=view.frame;
            if(broken)s->dirty[0]=s->dirty[1]=true;
        }
        if(params->resetHistory){
            if(eye==0){s->dirty[0]=s->dirty[1]=true;}
            else if(!(receipt.resetMask&1u)){s->dirty[0]=s->dirty[1]=true;return Refuse(view,s);}
        }
        const bool discontinuity=s->dirty[eye]||!s->seen[eye]||s->epoch[eye]!=view.epoch
            ||s->pipeline[eye]!=view.pipeline||std::uint32_t(s->frame[eye]+1u)!=view.frame;
        execute.resetHistory=(params->resetHistory||discontinuity)?1u:0u;
        const Context chosen=eye?s->right:s->native;
        const auto result=api_.execute(chosen,commandList,&execute);
        if(result!=kOk){s->dirty[0]=s->dirty[1]=true;Refuse(view,s);return result;}
        s->dirty[eye]=false;s->seen[eye]=true;s->epoch[eye]=view.epoch;s->frame[eye]=view.frame;s->pipeline[eye]=view.pipeline;
        receipt.mask|=1u<<eye;if(execute.resetHistory)receipt.resetMask|=1u<<eye;
        Publish(receipt);if(eye)++status_.right;else ++status_.left;
        return result;
    }
    Result Destroy(Context context) noexcept {
        std::lock_guard lock(mutex_);if(!api_.destroy)return kErrorUnknown;
        auto* s=Find(context);
        if(s&&s->right){
            // The game's own destruction point (0x181E11110). The twin's last
            // execute was recorded in the same frame as the game's context, so
            // it never outlives the native context's GPU exposure.
            FailGeneration(*s);
            const auto r=api_.destroy(s->right);
            if(r!=kOk){s->valid=false;++status_.refused;}
            else{++status_.destroyed;s->right=nullptr;}
        }
        const auto result=api_.destroy(context);
        // The game clears plugin+0x718 right after this call whatever it
        // returns (0x181E1113E), so the handle is never used again by WHGame.
        if(s){s->native=nullptr;s->valid=false;s->initialized=false;if(status_.live)--status_.live;if(!s->right)*s={};}
        RefreshReady();
        return result;
    }
};
}
