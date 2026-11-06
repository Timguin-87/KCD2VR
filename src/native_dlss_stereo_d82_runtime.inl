// Production NGX routing. Included by the Windows adapter and by the host
// regression fixture, which substitutes only SDK/OS leaves.
using Result=std::uint32_t;
constexpr Result kSuccess=1u,kFailure=0xBAD00000u;
constexpr int kSuperSampling=1; // official NVSDK_NGX_Feature_SuperSampling
using CreateFn=Result(__fastcall*)(void*,int,void*,void**);
using EvaluateFn=Result(__fastcall*)(void*,const void*,void*,void*);
using ReleaseFn=Result(__fastcall*)(void*);
using ShutdownFn=Result(__fastcall*)(void*);
using GetIntFn=Result(__fastcall*)(void*,const char*,int*);
using SetIntFn=void(__fastcall*)(void*,const char*,int);
using GetFloatFn=Result(__fastcall*)(void*,const char*,float*);
using GetResourceFn=Result(__fastcall*)(void*,const char*,void**);

CreateFn g_create=nullptr;
EvaluateFn g_evaluate=nullptr,g_evaluate_c=nullptr;
ReleaseFn g_release=nullptr;
ShutdownFn g_shutdown=nullptr;
GetIntFn g_get_i=nullptr;
SetIntFn g_set_i=nullptr;
GetFloatFn g_get_f=nullptr;
GetResourceFn g_get_resource=nullptr;

struct Pair {
    void* native{};
    void* right{};
    int feature{};
    std::uintptr_t device{},pipeline{};
    std::uint64_t identity{},generation{},epoch{};
    std::uint32_t frame[2]{};
    int nativeReset[2]{}; // the native values for the exact last admitted/read pair
    bool have[2]{},reset[2]{true,true},effectiveReset[2]{};
    bool recoverableCut{};
    std::uint32_t cutFrame{};
};
// NGX is not a concurrently callable SDK. Preserve its serialization while
// protecting the pair registry and the brief scoped Reset parameter mutation.
// No allocation, hash table, or ownership transfer happens during evaluation.
std::recursive_mutex g_mutex;
std::array<Pair,8> g_pairs{};
struct Receipt {
    // The queue hook must never take g_mutex while the SDK may call graphics
    // callbacks under it. Every field is atomic; serial publishes a snapshot.
    std::atomic<std::uint64_t> serial{0},identity{0},generation{0},epoch{0};
    std::atomic<std::uint32_t> frame{0};
};
std::array<Receipt,16> g_receipts{};
unsigned g_receipt_next=0;
std::uint64_t g_next_identity=0,g_receipt_serial=0;
std::atomic<bool> g_installed{false},g_enabled{false};
std::atomic<unsigned> g_live_pairs{0};
std::atomic<Fault> g_fault{Fault::None};
std::atomic<std::uint64_t> g_generation{1};
Status g_stats{}; // g_mutex
thread_local unsigned g_native_depth=0;

struct NativeCall {
    NativeCall() noexcept {++g_native_depth;}
    ~NativeCall(){--g_native_depth;}
};
bool Success(Result r) noexcept {return r==kSuccess;}
Pair* FindPair(const void* h) noexcept {
    for(auto& p:g_pairs)if(p.native==h && h)return &p;
    return nullptr;
}
Pair* FreePair() noexcept {for(auto& p:g_pairs)if(!p.native)return &p;return nullptr;}
void Recount() noexcept {
    unsigned count=0;for(const auto& p:g_pairs)if(p.native&&p.right)++count;
    g_live_pairs.store(count,std::memory_order_release);
}
void Refuse(Fault reason,const char* label) noexcept {
    ++g_stats.refused;
    Fault none=Fault::None;
    if(g_fault.compare_exchange_strong(none,reason,std::memory_order_acq_rel))
        ReportFailure(reason,label); // one bounded message in KCD2VR.log
}
void Invalidate(Pair& p) noexcept {
    p.recoverableCut=false;
    p.have[0]=p.have[1]=false;p.reset[0]=p.reset[1]=true;
    p.effectiveReset[0]=p.effectiveReset[1]=false;
}
void ClearReceipt(Receipt& receipt) noexcept {receipt.serial.store(0,std::memory_order_seq_cst);}
void ClearIdentity(std::uint64_t identity) noexcept {
    for(auto& receipt:g_receipts)if(receipt.identity.load(std::memory_order_seq_cst)==identity)ClearReceipt(receipt);
}
void PublishReceipt(const Pair& pair,std::uint64_t generation,std::uint64_t epoch,std::uint32_t frame) noexcept {
    auto& receipt=g_receipts[g_receipt_next++%g_receipts.size()];
    ClearReceipt(receipt);
    receipt.identity.store(pair.identity,std::memory_order_seq_cst);
    receipt.generation.store(generation,std::memory_order_seq_cst);
    receipt.epoch.store(epoch,std::memory_order_seq_cst);
    receipt.frame.store(frame,std::memory_order_seq_cst);
    receipt.serial.store(++g_receipt_serial,std::memory_order_seq_cst);
}

Result __fastcall CreateDetour(void* command,int feature,void* parameters,void** out) {
    if(!g_create)return kFailure;
    if(g_native_depth)return g_create(command,feature,parameters,out);
    std::lock_guard<std::recursive_mutex> lock(g_mutex);
    NativeCall native;
    const Result result=g_create(command,feature,parameters,out);
    const auto nativeError=SaveNativeError();
    if(feature==kSuperSampling)++g_stats.creates;
    if(!Success(result) || !g_installed.load(std::memory_order_acquire)) {RestoreNativeError(nativeError);return result;}
    void* original=ReadHandle(out);
    if(!original) {Refuse(Fault::MissingCreate,"create-returned-no-handle");RestoreNativeError(nativeError);return result;}
    if(FindPair(original)) {Refuse(Fault::AliasedHandle,"native-create-reused-live-handle");RestoreNativeError(nativeError);return result;}
    Pair* p=FreePair();
    if(!p) {
        if(feature==kSuperSampling && g_enabled.load(std::memory_order_acquire))Refuse(Fault::Capacity,"paired-feature-capacity");
        RestoreNativeError(nativeError);return result;
    }
    *p={};p->native=original;p->feature=feature;p->identity=++g_next_identity;
    p->device=CommandDevice(command);p->generation=g_generation.load(std::memory_order_acquire);
    if(feature!=kSuperSampling || !g_enabled.load(std::memory_order_acquire)) {RestoreNativeError(nativeError);return result;}
    if(!p->device) {Refuse(Fault::Parameters,"creation-device-unavailable");RestoreNativeError(nativeError);return result;}
    // Duplicate only the real, still-live creation call. Parameters are opaque;
    // no inferred struct, dictionary copy, new app ID or reinitialization.
    void* right=nullptr;
    const Result second=g_create(command,feature,parameters,&right);
    if(!Success(second) || !right || right==original) {
        // A returned alias belongs to the game; never release it as our twin.
        if(right && right!=original && g_release)(void)g_release(right);
        Refuse(right==original?Fault::AliasedHandle:Fault::TwinCreate,
            right==original?"twin-handle-alias":"right-feature-create-failed");
        RestoreNativeError(nativeError);return result;
    }
    p->right=right;
    ++g_stats.twins;Recount();
    ReportPairCreated(original,right);
    RestoreNativeError(nativeError);
    // Preserve the original SDK result and the original handle exposed to game.
    return result;
}

bool ReadEvaluation(void* parameters,int& reset) noexcept {
    if(!parameters || !g_get_i || !g_get_f || !g_get_resource || !g_set_i)return false;
    if(!Success(g_get_i(parameters,"Reset",&reset)) || (reset!=0 && reset!=1))return false;
    const char* const resources[]{"Color","Output","Depth","MotionVectors"};
    for(const char* key:resources) {
        void* resource=nullptr;
        if(!Success(g_get_resource(parameters,key,&resource)) || !resource)return false;
    }
    const char* const values[]{"Jitter.Offset.X","Jitter.Offset.Y","MV.Scale.X","MV.Scale.Y"};
    for(const char* key:values) {
        float value=0;
        if(!Success(g_get_f(parameters,key,&value)) || !std::isfinite(value))return false;
    }
    return true;
}
Result Evaluate(EvaluateFn fn,void* command,const void* handle,void* parameters,void* callback) {
    if(!fn)return kFailure;
    if(g_native_depth)return fn(command,handle,parameters,callback);
    std::lock_guard<std::recursive_mutex> lock(g_mutex);
    NativeCall native;
    Pair* p=FindPair(handle);
    if(p)p->recoverableCut=false; // only the immediately completed attempt may request a retry
    if(p && p->feature!=kSuperSampling)return fn(command,handle,parameters,callback);
    if(!g_installed.load(std::memory_order_acquire) || !g_enabled.load(std::memory_order_acquire)) {
        if(p)Invalidate(*p);
        return fn(command,handle,parameters,callback);
    }
    native_stereo_preflight::UpscalerView view{};
    const bool scoped=native_stereo_preflight::InUpscalerView();
    const bool valid=native_stereo_preflight::CurrentUpscalerView(view);
    if(!scoped) {
        // Menus/loading/ordinary mono may still use the game's LEFT handle.
        // A subsequent stereo pass must discard that unrelated history.
        if(p)Invalidate(*p);
        ++g_stats.unscoped;return fn(command,handle,parameters,callback);
    }
    if(!valid || view.eye>1) {Refuse(Fault::ViewIdentity,"unqualified-native-eye");return kFailure;}
    if(!p) {Refuse(Fault::MissingCreate,"evaluate-without-captured-create");return kFailure;}
    if(!p->right || p->right==p->native) {Refuse(Fault::MissingTwin,"missing-distinct-right-feature");return kFailure;}
    if(g_fault.load(std::memory_order_acquire)!=Fault::None)return kFailure;
    int nativeReset=0;
    if(!ReadEvaluation(parameters,nativeReset)) {Refuse(Fault::Parameters,"unreadable-native-evaluation-inputs");return kFailure;}
    const auto generation=g_generation.load(std::memory_order_acquire);
    if(view.eye==1 && p->pipeline && p->pipeline!=view.pipeline) {
        Refuse(Fault::ViewIdentity,"right-pipeline-differs-from-left");Invalidate(*p);return kFailure;
    }
    if(p->generation!=generation || p->epoch!=view.epoch || p->pipeline!=view.pipeline) {
        Invalidate(*p);p->generation=generation;p->epoch=view.epoch;p->pipeline=view.pipeline;
    }
    const unsigned eye=view.eye;
    p->nativeReset[eye]=nativeReset;
    if(p->have[eye] && p->frame[eye]==view.frame) {
        Refuse(Fault::DuplicateEye,"duplicate-history-advance-in-one-eye-frame");return kFailure;
    }
    if(eye==1 && (!p->have[0] || p->frame[0]!=view.frame)) {
        Refuse(Fault::MissingLeft,"right-evaluation-without-current-left");return kFailure;
    }
    // LEFT opens the pair. A missing RIGHT on the previous frame must reset
    // LEFT now, before it advances with a history older/newer than its twin.
    const bool pairGap=p->reset[0] || p->reset[1] || !p->have[0] || !p->have[1] ||
        std::uint32_t(view.frame-p->frame[0])!=1u || std::uint32_t(view.frame-p->frame[1])!=1u;
    if(eye==0 && pairGap)p->reset[0]=p->reset[1]=true;
    // A gap means native previous-frame data may refer to a frame DLSS did not
    // consume. Reset that eye rather than projecting an older temporal state.
    if(p->have[eye] && std::uint32_t(view.frame-p->frame[eye])!=1u)p->reset[eye]=true;
    if(nativeReset && eye==0)p->reset[0]=p->reset[1]=true;
    const bool effectiveReset=p->reset[eye] || nativeReset!=0;
    // A cut first discovered by RIGHT cannot retroactively reset LEFT. Do not
    // publish two successful SDK calls whose temporal ages disagree.
    if(eye==1 && p->effectiveReset[0]!=effectiveReset) {
        // D85R6: this is a recoverable camera cut, not an NGX/ABI fault.
        // LEFT already consumed its old history; discard this pair and force
        // BOTH histories to reset at the next LEFT. The previous implementation
        // latched g_fault forever here, so one right-only cut stopped every
        // later evaluation until stereo was disabled/re-enabled.
        const auto cuts=++g_stats.resetCuts;
        if(cuts<=3 || cuts%256==0)
            ReportResetCut(view.epoch,view.frame,p->frame[0],p->frame[1],
                p->nativeReset[0],nativeReset,p->effectiveReset[0],effectiveReset,cuts);
        ReportEvaluationInputs(eye,view.epoch,view.frame,parameters,nativeReset,effectiveReset);
        Invalidate(*p);
        ClearIdentity(p->identity);
        p->recoverableCut=true;p->cutFrame=view.frame;
        ++g_stats.refused;
        return kFailure;
    }
    const bool overrideReset=p->reset[eye] && nativeReset==0;
    if(overrideReset) {
        g_set_i(parameters,"Reset",1);int observed=0;
        if(!Success(g_get_i(parameters,"Reset",&observed)) || observed!=1) {
            g_set_i(parameters,"Reset",nativeReset);
            Refuse(Fault::Parameters,"reset-write-not-observed");return kFailure;
        }
    }
    const void* selected=eye?p->right:p->native;
    const Result result=fn(command,selected,parameters,callback);
    const auto nativeError=SaveNativeError();
    bool restored=true;
    if(overrideReset) {
        g_set_i(parameters,"Reset",nativeReset);int observed=-1;
        restored=Success(g_get_i(parameters,"Reset",&observed)) && observed==nativeReset;
    }
    RestoreNativeError(nativeError);
    if(!restored) {Refuse(Fault::RestoreReset,"native-reset-restore-failed");Invalidate(*p);return kFailure;}
    if(!Success(result)) {Refuse(Fault::Evaluate,"native-evaluation-failed");Invalidate(*p);return result;}
    if(generation!=g_generation.load(std::memory_order_acquire)) {Invalidate(*p);return result;}
    p->have[eye]=true;p->frame[eye]=view.frame;p->reset[eye]=false;p->effectiveReset[eye]=effectiveReset;
    if(eye)++g_stats.rightEvaluations;else ++g_stats.leftEvaluations;
    if(overrideReset || nativeReset)++g_stats.resets;
    ReportEvaluationInputs(eye,view.epoch,view.frame,parameters,nativeReset,effectiveReset);
    if(eye && p->have[0] && p->frame[0]==view.frame) {
        ++g_stats.pairs;g_stats.lastEpoch=view.epoch;g_stats.lastFrame=view.frame;
        PublishReceipt(*p,generation,view.epoch,view.frame);
        ReportPairEvaluated(view.epoch,view.frame,p->native,p->right,g_stats.pairs);
    }
    return result;
}
Result __fastcall EvaluateDetour(void* c,const void* h,void* p,void* callback) {return Evaluate(g_evaluate,c,h,p,callback);}
Result __fastcall EvaluateCDetour(void* c,const void* h,void* p,void* callback) {return Evaluate(g_evaluate_c,c,h,p,callback);}

Result __fastcall ReleaseDetour(void* handle) {
    if(!g_release)return kFailure;
    if(g_native_depth)return g_release(handle);
    std::lock_guard<std::recursive_mutex> lock(g_mutex);NativeCall native;
    Pair* p=FindPair(handle);
    if(p)ClearIdentity(p->identity);
    // Follow the game's release boundary. Do not free a history resource from
    // the input thread, a quality-setting callback, or while GPU work is live.
    if(p && p->right) {
        const Result rightResult=g_release(p->right);
        if(!Success(rightResult))Refuse(Fault::Release,"right-release-failed");
        p->right=nullptr;Invalidate(*p);
    }
    const Result result=g_release(handle);
    if(p) {
        if(Success(result)) {
            *p={};
        }
        else Refuse(Fault::Release,"native-release-failed");
        Recount();
    }
    ++g_stats.releases;return result;
}
Result __fastcall ShutdownDetour(void* device) {
    if(!g_shutdown)return kFailure;
    if(g_native_depth)return g_shutdown(device);
    std::lock_guard<std::recursive_mutex> lock(g_mutex);NativeCall native;
    const Result result=g_shutdown(device);
    if(Success(result)) {
        // NGX Shutdown1 retires every feature on the supplied device, including
        // our twins. Releasing them again after shutdown would be a double free.
        for(auto& p:g_pairs)if(!device || p.device==reinterpret_cast<std::uintptr_t>(device)) {
            ClearIdentity(p.identity);
            p={};
        }
        Recount();
    } else Refuse(Fault::Shutdown,"native-shutdown-failed");
    ++g_stats.shutdowns;return result;
}
