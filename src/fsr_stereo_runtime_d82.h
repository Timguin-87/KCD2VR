#pragma once
#include <array>
#include <atomic>
#include <cstddef>
#include <cstdint>
#include <cmath>
#include <mutex>
#define FFX_API_ENTRY
#include "../deps/FidelityFX_API_1_1_3/ffx-api/include/ffx_api/ffx_upscale.h"
#undef FFX_API_ENTRY

namespace kcd2vr::fsr_stereo_runtime_d82 {
// Public FFX API, not the older FSR2 context/scratch-buffer ABI. Layout checked
// against WHGame's real 0x1DFDFF0 dispatch and 0x1DFEC30 creation helpers.
static_assert(sizeof(ffxApiHeader)==16 && sizeof(FfxApiResource)==48);
static_assert(sizeof(ffxCreateContextDescUpscale)==48);
static_assert(sizeof(ffxDispatchDescUpscale)==0x1b0);
static_assert(offsetof(ffxDispatchDescUpscale,jitterOffset)==0x168);
static_assert(offsetof(ffxDispatchDescUpscale,reset)==0x198);
struct View { bool active=false,valid=false;unsigned eye=0;std::uint64_t epoch=0;std::uint32_t frame=0;std::uintptr_t pipeline=0; };
struct Api { PfnFfxCreateContext create=nullptr;PfnFfxDestroyContext destroy=nullptr;PfnFfxDispatch dispatch=nullptr;PfnFfxConfigure configure=nullptr; };
struct Status { std::uint64_t creates=0,twins=0,left=0,right=0,refused=0,destroyed=0,resetCuts=0;unsigned live=0;bool enabled=false; };

class Runtime {
    struct Slot {
        ffxContext native=nullptr,right=nullptr;
        bool valid=true;bool dirty[2]{true,true};std::uint64_t epoch[2]{};
        std::uintptr_t pipeline[2]{};std::uint32_t frame[2]{};bool seen[2]{};
        std::uint64_t generation=0;
    };
    struct Receipt {
        std::uint64_t epoch=0,generation=0;std::uint32_t frame=0;
        std::uintptr_t pipeline=0;ffxContext left=nullptr,right=nullptr;
        unsigned mask=0,resetMask=0;bool failed=false,used=false;
    };
    struct ResetCut {
        std::uint64_t routing=0,generation=0,epoch=0;std::uint32_t frame=0;
        std::uintptr_t pipeline=0;ffxContext left=nullptr,right=nullptr;
    } cut_{};
    struct PublishedReceipt {
        std::atomic<std::uint64_t> sequence{0},epoch{0},routingGeneration{0};
        std::atomic<std::uint32_t> frame{0},flags{0};
    };
    Api api_{};std::array<Slot,8> slots_{};std::array<Receipt,32> receipts_{};
    std::array<PublishedReceipt,32> published_{};
    std::atomic<bool> enabled_{false},ready_{false};
    std::atomic<std::uint64_t> routingGeneration_{1};
    void Publish(const Receipt& r) noexcept {
        auto& p=published_[static_cast<std::size_t>(&r-receipts_.data())];
        // A single global atomic order makes the double sequence read a
        // coherent snapshot even on weak-memory hosts; no non-atomic payload.
        p.sequence.fetch_add(1,std::memory_order_seq_cst);
        p.epoch.store(r.epoch,std::memory_order_seq_cst);p.frame.store(r.frame,std::memory_order_seq_cst);
        p.routingGeneration.store(routingGeneration_.load(std::memory_order_seq_cst),std::memory_order_seq_cst);
        p.flags.store(!r.used?0u:((r.mask&3u)|(r.failed?4u:0u)|8u),std::memory_order_seq_cst);
        p.sequence.fetch_add(1,std::memory_order_seq_cst);
    }
    void RefreshReady() noexcept {
        bool found=false;
        if(api_.create&&api_.dispatch&&api_.destroy)for(const auto& s:slots_)found|=s.valid&&s.native&&s.right&&s.native!=s.right;
        ready_.store(found,std::memory_order_release);
    }
    std::size_t cursor_=0;std::uint64_t generation_=0;Status status_{};
    mutable std::recursive_mutex mutex_;
    Slot* Find(ffxContext c) noexcept {for(auto& s:slots_)if(s.native==c&&c)return &s;return nullptr;}
    Receipt& Record(const View& v,const Slot* s) noexcept {
        for(auto& r:receipts_)if(r.used&&r.epoch==v.epoch&&r.frame==v.frame){
            if(!s||r.generation!=s->generation||r.pipeline!=v.pipeline){r.failed=true;Publish(r);}
            return r;
        }
        auto& r=receipts_[cursor_++%receipts_.size()];r={};r.used=true;r.epoch=v.epoch;r.frame=v.frame;r.pipeline=v.pipeline;
        if(s){r.generation=s->generation;r.left=s->native;r.right=s->right;}
        Publish(r);return r;
    }
    ffxReturnCode_t Refuse(const View& v,Slot* s) noexcept {
        if(s)s->dirty[0]=s->dirty[1]=true;
        ++status_.refused;if(v.active){auto& r=Record(v,s);r.failed=true;Publish(r);}return FFX_API_RETURN_ERROR_RUNTIME_ERROR;
    }
    static bool InputValid(const ffxDispatchDescUpscale& d) noexcept {
        return d.commandList&&d.color.resource&&d.depth.resource&&d.motionVectors.resource&&d.output.resource
            &&d.color.resource!=d.output.resource&&d.renderSize.width&&d.renderSize.height
            &&std::isfinite(d.jitterOffset.x)&&std::isfinite(d.jitterOffset.y)
            &&std::isfinite(d.motionVectorScale.x)&&std::isfinite(d.motionVectorScale.y)
            &&std::isfinite(d.preExposure)&&d.preExposure>0;
    }
public:
    explicit Runtime(Api api={}) noexcept:api_(api){}
    void SetApi(Api api) noexcept {std::lock_guard lock(mutex_);cut_={};api_=api;RefreshReady();}
    void SetEnabled(bool enabled) noexcept {
        std::lock_guard lock(mutex_);if(status_.enabled==enabled)return;cut_={};status_.enabled=enabled;
        enabled_.store(false,std::memory_order_release);
        routingGeneration_.fetch_add(1,std::memory_order_seq_cst);
        receipts_={};cursor_=0;for(const auto& r:receipts_)Publish(r);
        for(auto& s:slots_){s.dirty[0]=s.dirty[1]=true;s.seen[0]=s.seen[1]=false;}
        enabled_.store(enabled,std::memory_order_release);
    }
    bool Ready() const noexcept {return ready_.load(std::memory_order_acquire);}
    Status GetStatus() const noexcept {std::lock_guard lock(mutex_);return status_;}
    bool PairReady(std::uint64_t epoch,std::uint32_t frame) const noexcept {
        // Called under renderer queue/publication locks: no SDK, registry lock,
        // allocation or wait is permitted here.
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
    // Called only after a failed completed native eye pass, not from queue
    // publication. Join the latest exact cut to its still-live context pair.
    bool RecoverableResetCut(std::uint64_t epoch,std::uint32_t frame,std::uintptr_t pipeline) const noexcept {
        std::lock_guard lock(mutex_);
        if(!status_.enabled||!Ready()||!epoch||!pipeline||!cut_.generation||
           cut_.routing!=routingGeneration_.load(std::memory_order_seq_cst)||
           cut_.epoch!=epoch||cut_.frame!=frame||cut_.pipeline!=pipeline)return false;
        for(const auto& s:slots_)if(s.valid&&s.native&&s.right&&s.native!=s.right&&
            s.native==cut_.left&&s.right==cut_.right&&s.generation==cut_.generation&&s.dirty[0]&&s.dirty[1]){
            for(const auto& r:receipts_)if(r.used&&r.failed&&r.mask==1u&&r.resetMask==0u&&
                r.generation==s.generation&&r.epoch==epoch&&r.frame==frame&&r.pipeline==pipeline&&
                r.left==s.native&&r.right==s.right)return true;
        }
        return false;
    }
    ffxReturnCode_t Create(ffxContext* context,ffxCreateContextDescHeader* desc,const ffxAllocationCallbacks* mem) noexcept {
        std::lock_guard lock(mutex_);cut_={};if(!api_.create)return FFX_API_RETURN_ERROR_RUNTIME_ERROR;
        const auto result=api_.create(context,desc,mem);++status_.creates;
        if(result!=FFX_API_RETURN_OK||!context||!*context||!desc||desc->type!=FFX_API_CREATE_CONTEXT_DESC_TYPE_UPSCALE||!status_.enabled)return result;
        Slot* slot=nullptr;for(auto& s:slots_)if(!s.native&&!s.right){slot=&s;break;}
        if(!slot||Find(*context)){++status_.refused;return result;}
        ffxContext right=nullptr;
        const auto twin=api_.create(&right,desc,mem); // same live descriptor chain, separate SDK allocation
        if(twin!=FFX_API_RETURN_OK||!right||right==*context){
            // A returned alias is still native-owned; never destroy it here.
            if(right&&right!=*context&&api_.destroy)api_.destroy(&right,mem);
            ++status_.refused;return result;
        }
        *slot={};slot->native=*context;slot->right=right;slot->generation=++generation_;
        ++status_.twins;++status_.live;RefreshReady();return result;
    }
    ffxReturnCode_t Dispatch(ffxContext* context,const ffxDispatchDescHeader* desc,View view) noexcept {
        std::lock_guard lock(mutex_);cut_={};if(!api_.dispatch)return FFX_API_RETURN_ERROR_RUNTIME_ERROR;
        Slot* s=context?Find(*context):nullptr;
        const bool upscale=desc&&desc->type==FFX_API_DISPATCH_DESC_TYPE_UPSCALE;
        const bool reactive=desc&&desc->type==FFX_API_DISPATCH_DESC_TYPE_UPSCALE_GENERATEREACTIVEMASK;
        if(!status_.enabled||!view.active||(!upscale&&!reactive)){
            // Mono/menu output changes the primary instance's history. The
            // next immersive pair starts a new history in BOTH instances.
            if(s&&upscale){s->dirty[0]=s->dirty[1]=true;s->seen[0]=s->seen[1]=false;}
            return api_.dispatch(context,desc);
        }
        if(!view.valid||view.eye>1||!view.epoch||!view.pipeline||!s||!s->valid||!s->right)return Refuse(view,s);
        ffxContext chosen=view.eye?s->right:s->native;
        if(reactive){
            const auto r=api_.dispatch(&chosen,desc);if(r!=FFX_API_RETURN_OK)Refuse(view,s);return r;
        }
        auto& receipt=Record(view,s);
        if(receipt.failed||(receipt.mask&(1u<<view.eye)))return Refuse(view,s);
        if(view.eye==1&&!(receipt.mask&1u))return Refuse(view,s);
        const auto& input=*reinterpret_cast<const ffxDispatchDescUpscale*>(desc);
        if(!InputValid(input))return Refuse(view,s);
        // This copies a documented dispatch descriptor, never an opaque context.
        // All resources, dimensions, jitter, masks, flags and extension chains
        // remain native. Only a per-eye discontinuity can add a reset.
        ffxDispatchDescUpscale dispatch=input;
        const unsigned eye=view.eye;
        if(eye==0){
            // A missing/refused RIGHT may leave LEFT one frame ahead. Decide
            // continuity for the PAIR before advancing LEFT, so both histories
            // reset together even if the missing RIGHT made no SDK call at all.
            bool broken=false;
            for(unsigned e=0;e<2;++e)broken|=s->dirty[e]||!s->seen[e]||s->epoch[e]!=view.epoch
                ||s->pipeline[e]!=view.pipeline||std::uint32_t(s->frame[e]+1u)!=view.frame;
            if(broken)s->dirty[0]=s->dirty[1]=true;
        }
        if(input.reset){
            // LEFT reset propagates to RIGHT. A reset discovered only after an
            // unreset LEFT evaluation would make this pair asymmetric; reject
            // it and reset both on the next pair instead of certifying it.
            if(eye==0){s->dirty[0]=s->dirty[1]=true;}
            else if(!(receipt.resetMask&1u)){
                const auto result=Refuse(view,s); // rejected pair stays unpublished
                cut_={routingGeneration_.load(std::memory_order_seq_cst),s->generation,
                    view.epoch,view.frame,view.pipeline,s->native,s->right};
                ++status_.resetCuts;return result;
            }
        }
        const bool discontinuity=s->dirty[eye]||!s->seen[eye]||s->epoch[eye]!=view.epoch
            ||s->pipeline[eye]!=view.pipeline||std::uint32_t(s->frame[eye]+1u)!=view.frame;
        dispatch.reset=dispatch.reset||discontinuity;
        const auto result=api_.dispatch(&chosen,&dispatch.header);
        if(result!=FFX_API_RETURN_OK){s->dirty[0]=s->dirty[1]=true;Refuse(view,s);return result;}
        s->dirty[eye]=false;s->seen[eye]=true;s->epoch[eye]=view.epoch;s->frame[eye]=view.frame;s->pipeline[eye]=view.pipeline;
        receipt.mask|=1u<<eye;if(dispatch.reset)receipt.resetMask|=1u<<eye;
        Publish(receipt);if(eye)++status_.right;else ++status_.left;
        return result;
    }
    ffxReturnCode_t Configure(ffxContext* context,const ffxConfigureDescHeader* desc) noexcept {
        std::lock_guard lock(mutex_);cut_={};if(!api_.configure)return FFX_API_RETURN_ERROR_RUNTIME_ERROR;
        const auto result=api_.configure(context,desc);auto* s=context?Find(*context):nullptr;
        if(result==FFX_API_RETURN_OK&&s&&s->right){
            ffxContext right=s->right;const auto r=api_.configure(&right,desc);
            if(r!=FFX_API_RETURN_OK){s->valid=false;s->dirty[0]=s->dirty[1]=true;for(auto& x:receipts_)if(x.generation==s->generation){x.failed=true;Publish(x);}RefreshReady();++status_.refused;return r;}
        }
        return result;
    }
    ffxReturnCode_t Destroy(ffxContext* context,const ffxAllocationCallbacks* mem) noexcept {
        std::lock_guard lock(mutex_);cut_={};if(!api_.destroy)return FFX_API_RETURN_ERROR_RUNTIME_ERROR;
        auto* s=context?Find(*context):nullptr;
        if(s&&s->right){
            // Same native destruction boundary/allocator and existing native GPU
            // drain. Never destroy an in-flight twin during a mode toggle.
            for(auto& r:receipts_)if(r.generation==s->generation){r.failed=true;Publish(r);}
            const auto result=api_.destroy(&s->right,mem);
            if(result!=FFX_API_RETURN_OK){s->valid=false;++status_.refused;}
            else{++status_.destroyed;s->right=nullptr;}
        }
        const auto result=api_.destroy(context,mem);
        if(s&&result==FFX_API_RETURN_OK){s->native=nullptr;s->valid=false;--status_.live;if(!s->right)*s={};}
        RefreshReady();
        return result;
    }
};
}
