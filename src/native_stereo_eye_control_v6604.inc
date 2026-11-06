// Included in the preflight TU's private namespace. Six hooks total: the existing
// byte-preserving Standard observer plus Pipeline, Build, GBuffer, Opaque, Ranges.
// One extra *native pipeline* call, before the command's original finalizer.
// F11 is zero-separation proof. After that proof only, F9 progresses through a
// symmetric runtime-IPD frame, one preserved L/R pair, a 30-frame live burst and
// finally continuous live stereo. COUNT2 remains disabled throughout.
namespace ec=eye_control_model;
using PipelineFn=void(__fastcall*)(void*,void*);
using FinalComposeFn=void(__fastcall*)(void*,void*);
using ComposeCallFn=void(__fastcall*)(void*,void*,void*);
using VoidOneFn=void(__fastcall*)(void*);
using CameraSetterFn=void(__fastcall*)(void*,const void*,std::int32_t);
using CameraAssignFn=void*(__fastcall*)(void*,const void*);
using RangeFn=void(__fastcall*)(void*,void*,std::uint8_t,std::int32_t,std::int32_t);
std::optional<DetourModKit::hook::Hook> g_pipeline_hook,g_final_compose_hook,g_compose_call_hook,g_builder_hook,g_gbuffer_hook,g_opaque_hook,g_range_hook;
std::atomic<PipelineFn> g_pipeline_original{nullptr};
std::atomic<FinalComposeFn> g_final_compose_original{nullptr};
std::atomic<ComposeCallFn> g_compose_call_original{nullptr};
std::atomic<VoidOneFn> g_builder_original{nullptr},g_gbuffer_original{nullptr},g_opaque_original{nullptr};
std::atomic<RangeFn> g_range_original{nullptr};
std::atomic<unsigned> g_pipeline_inflight{0},g_eye_control_attempts{0},g_eye_control_calls{0},g_eye_control_returns{0};
std::atomic<unsigned> g_eye_gbuffer_returns{0},g_eye_opaque_returns{0},g_eye_nonempty_requests{0};
std::atomic<bool> g_eye_decision_used{false};
// outcome: 0 waiting, 1 blocked, 2 in flight, 3 CPU eye-1 returned, 4 unconfirmed,
// 5 camera/build gate fell back to mono, 6 restoration unconfirmed.
std::atomic<unsigned> g_eye_outcome{0};
enum class ControlPass : unsigned { Normal, Eye1, Restore };
struct EyeFlow {
    std::uintptr_t renderer{},pipeline{},crv{};
    std::uint64_t epoch{};
    std::uint32_t frame{};
    ControlPass pass{ControlPass::Normal};
    ec::Work normal{},eye{};
    ec::CameraSet cameras{};
    model::Snapshot checkpoint{};rm::Evidence resourceCheckpoint{};
    bool camerasChanged{},selected{},fallback{},ownedMemberChanged{};
    int designStage{};
    float designIpd{};
    bool leftPrepared{},captureLeftRecorded{},captureRightRecorded{};
    std::uint64_t captureSerial{};
};
thread_local EyeFlow* t_eye_flow=nullptr;
// Only the verified renderer/pipeline FinalCompose owner can authorize a D82
// temporal output scope. A shared helper call elsewhere in Standard is not one.
thread_local EyeFlow* t_d82_final_compose_flow=nullptr;
thread_local unsigned t_opaque_depth=0;
#include "native_stereo_eye_signatures_v6604.inc"
const char* EyeOutcomeName(unsigned v) noexcept {
    switch(v) {
    case 1:return "BLOCKED";case 2:return "IN-FLIGHT";case 3:return "CPU-EYE1-RETURNED";
    case 4:return "CPU-POSTCONDITION-UNCONFIRMED";case 5:return "CAMERA-GATE-MONO-FALLBACK";
    case 6:return "RESTORE-UNCONFIRMED";default:return "NOT-ATTEMPTED";
    }
}
bool InEyeControl() noexcept {return t_eye_flow && t_eye_flow->pass!=ControlPass::Normal;}
bool IsStereoDesign(const EyeFlow& f) noexcept {return f.designStage>=1 && f.designStage<=4;}
// V66.18: stage 4 uses only the proven queue-boundary preservation path.
// Do not let the older in-command-list capture race the same owned L/R textures.
bool CapturesStereoOutput(const EyeFlow& f) noexcept {return f.designStage>=2 && f.designStage<=3;}
bool RepeatsStereoFrames(const EyeFlow& f) noexcept {return f.designStage>=3 && f.designStage<=4;}
ec::Work* CurrentWork() noexcept {
    if(!t_eye_flow||t_eye_flow->pass==ControlPass::Restore)return nullptr;
    return t_eye_flow->pass==ControlPass::Normal?&t_eye_flow->normal:&t_eye_flow->eye;
}
void NativeOne(VoidOneFn fn,void* self,unsigned stage) {
    const unsigned saved=t_native_stage;t_native_stage=stage;
    __try {fn(self);} __except(RecordFault(GetExceptionInformation())) { }
    t_native_stage=saved;
}
void NativePipeline(void* renderer,void* crv) {
    const unsigned saved=t_native_stage;t_native_stage=3;
    __try {g_pipeline_original.load(std::memory_order_acquire)(renderer,crv);}
    __except(RecordFault(GetExceptionInformation())) { }
    t_native_stage=saved;
}
void NativeSetCamera(std::uintptr_t crv,unsigned bank,const ec::CameraBytes& camera) {
    const unsigned saved=t_native_stage;t_native_stage=4;
    const auto fn=reinterpret_cast<CameraSetterFn>(g_wh+(bank?ec::kPreviousSetterRva:ec::kCurrentSetterRva));
    __try {fn(reinterpret_cast<void*>(crv),camera.bytes,1);}
    __except(RecordFault(GetExceptionInformation())) { }
    t_native_stage=saved;
}
void* NativeAssignCamera(void* dst,const ec::CameraBytes& camera) {
    const unsigned saved=t_native_stage;t_native_stage=8;void* result=nullptr;
    const auto fn=reinterpret_cast<CameraAssignFn>(g_wh+ec::kCameraAssignRva);
    __try {result=fn(dst,camera.bytes);} __except(RecordFault(GetExceptionInformation())) { }
    t_native_stage=saved;return result;
}
void StoreSelector(std::uintptr_t crv,std::uint32_t eye) {
    const unsigned saved=t_native_stage;t_native_stage=4;
    __try {std::memcpy(reinterpret_cast<void*>(crv+0x8EE0),&eye,sizeof(eye));}
    __except(RecordFault(GetExceptionInformation())) { }
    t_native_stage=saved;
}
void StoreRendererResult(std::uintptr_t renderer,std::uint8_t value) {
    const unsigned saved=t_native_stage;t_native_stage=8;
    __try {std::memcpy(reinterpret_cast<void*>(renderer+0xF2),&value,sizeof(value));}
    __except(RecordFault(GetExceptionInformation())) { }
    t_native_stage=saved;
}

bool RestoreCameraBanks(EyeFlow& f) {
    if(!f.camerasChanged)return true;
    // Restore both physical banks for the real-IPD F9 design, only physical eye1
    // for the F11 zero-separation proof. Native assignment owns/refcounts the
    // CCamera string member; raw memcpy is intentionally not used.
    for(unsigned bank=0;bank<2;++bank) {
        ec::CameraBytes liveRight{};
        if(!ec::ReadCamera(g_read,f.crv,bank,1,liveRight)||
           ec::Field<std::uintptr_t>(liveRight,ec::kOwnedMember)!=
           ec::Field<std::uintptr_t>(f.cameras.savedRight[bank],ec::kOwnedMember)) {
            f.ownedMemberChanged=true;return false;
        }
        if(IsStereoDesign(f)) {
            ec::CameraBytes liveLeft{};
            if(!ec::ReadCamera(g_read,f.crv,bank,0,liveLeft)||
               ec::Field<std::uintptr_t>(liveLeft,ec::kOwnedMember)!=
               ec::Field<std::uintptr_t>(f.cameras.source[bank],ec::kOwnedMember)) {
                f.ownedMemberChanged=true;return false;
            }
        }
    }
    bool ok=true;
    for(unsigned bank=0;bank<2;++bank) {
        if(IsStereoDesign(f)) {
            void* leftDst=reinterpret_cast<void*>(f.crv+model::kCameraBanks[bank]);
            if(NativeAssignCamera(leftDst,f.cameras.source[bank])!=leftDst)ok=false;
            ec::CameraBytes liveLeft{};
            if(!ec::ReadCamera(g_read,f.crv,bank,0,liveLeft)||!ec::EqualAssigned(liveLeft,f.cameras.source[bank]))ok=false;
        }
        void* rightDst=reinterpret_cast<void*>(f.crv+model::kCameraBanks[bank]+model::kCameraSize);
        if(NativeAssignCamera(rightDst,f.cameras.savedRight[bank])!=rightDst)ok=false;
        ec::CameraBytes liveRight{};
        if(!ec::ReadCamera(g_read,f.crv,bank,1,liveRight)||!ec::EqualAssigned(liveRight,f.cameras.savedRight[bank]))ok=false;
    }
    if(ok){f.camerasChanged=false;f.leftPrepared=false;}
    return ok;
}

void PrintCameraControl(const EyeFlow& f,const char* phase) noexcept {
    for(unsigned b=0;b<2;++b) {
        const auto& src=f.cameras.source[b];const auto& right=f.cameras.prepared[b];
        const auto& left=IsStereoDesign(f)?f.cameras.preparedLeft[b]:f.cameras.source[b];
        Log("V66.12-EYE CAMERA phase=%s epoch=%llu frame=%u bank=%s designStage=%d ipd=%.6f "
            "sourceTag=%u leftTag=%u rightTag=%u sourceHash=%016llX leftHash=%016llX rightHash=%016llX "
            "sourcePosition=%.6f,%.6f,%.6f rightOwnedMemberPreserved=%d previousFrameProducer=UNOBSERVED",
            phase,(unsigned long long)f.epoch,f.frame,b?"PREVIOUS":"CURRENT",f.designStage,double(f.designIpd),
            ec::Field<std::uint32_t>(src,model::kCameraEyeTag),ec::Field<std::uint32_t>(left,model::kCameraEyeTag),
            ec::Field<std::uint32_t>(right,model::kCameraEyeTag),
            (unsigned long long)model::Hash(src.bytes,sizeof(src.bytes)),
            (unsigned long long)model::Hash(left.bytes,sizeof(left.bytes)),
            (unsigned long long)model::Hash(right.bytes,sizeof(right.bytes)),
            static_cast<double>(ec::Field<float>(src,0xC)),static_cast<double>(ec::Field<float>(src,0x1C)),
            static_cast<double>(ec::Field<float>(src,0x2C)),
            ec::Field<std::uintptr_t>(right,ec::kOwnedMember)==ec::Field<std::uintptr_t>(f.cameras.savedRight[b],ec::kOwnedMember)?1:0);
    }
}
bool PrepareStereoDesignLeft(EyeFlow& f) {
    if(!IsStereoDesign(f)||f.leftPrepared)return f.leftPrepared;
    if(!ec::CaptureRawCameras(g_read,f.crv,f.cameras))return false;

    // V66.39R1: proof-gated pair-center ShadowMap preparation.
    // Before the normal LEFT shift, the CRenderView still contains the game's unshifted
    // source/center camera. When F9 requests the experiment, ask the exact native camera
    // builder to materialize record0 once from that center camera, publish only the
    // statically proven ShadowMap::Update input fields, then let the normal LEFT build
    // immediately overwrite record0. No center record is used unless all readbacks succeed.
    if(native_stereo_outer_lab::V6639R1CenterShadowPrepRequested()) {
        bool centerOk=false; std::uint32_t sel=~0u,count=0,frame=0;
        ec::CameraBytes liveCenter{};
        const bool liveOk=ec::ReadCamera(g_read,f.crv,0,0,liveCenter)&&ec::EqualAssigned(liveCenter,f.cameras.source[0]);
        const bool stateOk=model::Read(g_read,f.crv,0x8EE0,sel)&&sel==0u &&
                           model::Read(g_read,f.crv,0x2C,frame)&&frame==f.frame;
        const auto builder=g_builder_original.load(std::memory_order_acquire);
        bool postLiveOk=false;
        if(liveOk&&stateOk&&builder) {
            NativeOne(builder,reinterpret_cast<void*>(f.crv),9);
            std::array<unsigned char,0x400> centerRecord{};
            ec::CameraBytes postCenter{};
            postLiveOk=ec::ReadCamera(g_read,f.crv,0,0,postCenter)&&ec::EqualAssigned(postCenter,f.cameras.source[0]);
            const bool recordOk=postLiveOk&&model::Read(g_read,f.crv,0xA330,count)&&count==1u &&
                g_read(f.crv+0x9B30,centerRecord.data(),centerRecord.size());
            const auto sourceHash=model::Hash(f.cameras.source[0].bytes,sizeof(f.cameras.source[0].bytes));
            centerOk=recordOk&&native_stereo_outer_lab::PublishV6639R1CenterShadowRecord(
                reinterpret_cast<void*>(f.crv),f.frame,centerRecord.data(),centerRecord.size(),sourceHash);
        }
        Log("V66.39R1-CENTER-BUILD-GATE frame=%u crv=%p selector=%u liveCenter=%d state=%d postLiveCenter=%d builtCount=%u publish=%d result=%s",
            f.frame,reinterpret_cast<void*>(f.crv),unsigned(sel),liveOk?1:0,stateOk?1:0,postLiveOk?1:0,unsigned(count),
            centerOk?1:0,centerOk?"CENTER-RECORDED":"NOT-ACTIVE");
    }

    if(!ec::MakeStereoDesign(f.cameras,f.designIpd))return false;
    f.camerasChanged=true;
    for(unsigned bank=0;bank<2;++bank)NativeSetCamera(f.crv,bank,f.cameras.preparedLeft[bank]);
    for(unsigned bank=0;bank<2;++bank) {
        ec::CameraBytes left{},right{};
        if(!ec::ReadCamera(g_read,f.crv,bank,0,left)||!ec::EqualAssigned(left,f.cameras.preparedLeft[bank])||
           !ec::ReadCamera(g_read,f.crv,bank,1,right)||!ec::EqualAssigned(right,f.cameras.savedRight[bank]))return false;
    }
    f.leftPrepared=true;PrintCameraControl(f,"LEFT-PREPARE");return true;
}
bool PrepareCameraControl(EyeFlow& f) {
    std::uint32_t frame=0,selector=~0u;std::uintptr_t owner=0;
    if(!model::Read(g_read,f.crv,0x2C,frame)||frame!=f.frame||!model::Read(g_read,f.crv,0x8EE0,selector)||selector!=0||
       !model::Read(g_read,f.crv,0x8D60,owner)||owner!=f.pipeline)return false;
    // Revalidate after the second invocation's native output setup, immediately
    // before touching cameras. No pointer snapshot is a GPU lifetime guarantee.
    const auto resources=model::Capture(g_read,f.pipeline,g_wh);
    const auto evidence=rm::CaptureEvidence(g_read,resources,g_wh,static_cast<std::uint32_t>(GetCurrentThreadId()));
    if(ec::RejectResources(resources,evidence,g_wh,2)||
       !ec::SamePersistentResourceCheckpoint(f.checkpoint,f.resourceCheckpoint,resources,evidence))return false;
    if(IsStereoDesign(f)) {
        if(!f.leftPrepared)return false;
        // The left bank was deliberately shifted before the first native build.
        // Keep the right camera derived from the original unshifted game camera;
        // never recalculate it from the already-shifted left bank.
        for(unsigned bank=0;bank<2;++bank) {
            ec::CameraBytes left{},right{};
            if(!ec::ReadCamera(g_read,f.crv,bank,0,left)||!ec::EqualStereoInputsAfterBuild(left,f.cameras.preparedLeft[bank])||
               !ec::ReadCamera(g_read,f.crv,bank,1,right)||!ec::EqualAssigned(right,f.cameras.savedRight[bank]))return false;
        }
        PrintCameraControl(f,"RIGHT-PREPARE");
    } else {
        ec::CameraSet latest{};
        if(!ec::CaptureCameras(g_read,f.crv,latest))return false;
        for(unsigned b=0;b<2;++b)if(!ec::SameGeometry(latest.source[b],f.cameras.source[b]))return false;
        f.cameras=latest;PrintCameraControl(f,"PREPARE");
    }
    // Setters select by the tag in each record; count=1 here registers only slot1.
    f.camerasChanged=true;
    for(unsigned bank=0;bank<2;++bank)NativeSetCamera(f.crv,bank,f.cameras.prepared[bank]);
    for(unsigned bank=0;bank<2;++bank) {
        ec::CameraBytes right{},left{};
        const auto& expectedLeft=IsStereoDesign(f)?f.cameras.preparedLeft[bank]:f.cameras.source[bank];
        if(!ec::ReadCamera(g_read,f.crv,bank,1,right)||!ec::EqualAssigned(right,f.cameras.prepared[bank])||
           !ec::ReadCamera(g_read,f.crv,bank,0,left)||!ec::EqualStereoInputsAfterBuild(left,expectedLeft))return false;
    }
    StoreSelector(f.crv,1);f.selected=true;
    return model::Read(g_read,f.crv,0x8EE0,selector)&&selector==1;
}

void __fastcall BuilderDetour(void* crv) {
    const DWORD enteringError=GetLastError();const auto savedStage=t_native_stage;
    auto* f=t_eye_flow;auto* work=CurrentWork();
    const bool matched=f && work && f->crv==reinterpret_cast<std::uintptr_t>(crv);
    if(matched) {
        ++work->builds;
        if(f->pass==ControlPass::Normal && IsStereoDesign(*f)) {
            if(work->builds!=1 || !PrepareStereoDesignLeft(*f)) {
                f->fallback=true;
                const bool restored=RestoreCameraBanks(*f);
                Log("V66.12-DESIGN LEFT-CAMERA-BLOCKED epoch=%llu frame=%u restore=%d extraEyeWillRun=0",
                    (unsigned long long)f->epoch,f->frame,restored?1:0);
            } else {
                stereo_design_capture::SetOutputCensusEye(0,f->frame);
                Log("V66.12-DESIGN SELECT epoch=%llu frame=%u selector=0 side=LEFT ipd=%.6f "
                    "placement=BEFORE-NATIVE-CAMERA-BUILD count=1 outputCensusPhase=L",
                    (unsigned long long)f->epoch,f->frame,double(f->designIpd));
            }
        } else if(f->pass==ControlPass::Eye1) {
            if(work->builds!=1||!PrepareCameraControl(*f)) {
                // Fail closed before native per-view Update. This additional
                // pipeline invocation becomes a mono control, not eye1 success.
                if(f->selected)StoreSelector(f->crv,0);
                f->selected=false;f->fallback=true;
                const bool restored=RestoreCameraBanks(*f);
                if(!restored)g_eye_outcome.store(6,std::memory_order_release);
                Log("V66.12-EYE CAMERA-BLOCKED epoch=%llu fallbackSelector=0 cameraRestore=%d "
                    "nativeEye1Admitted=0 countWrites=0",(unsigned long long)f->epoch,restored?1:0);
            } else {
                Log("V66.12-EYE SELECT epoch=%llu frame=%u selector=1 nativeCountUnmodified=1 "
                    "placement=BEFORE-NATIVE-CAMERA-BUILD designStage=%d ipd=%.6f",
                    (unsigned long long)f->epoch,f->frame,f->designStage,double(f->designIpd));
            }
        }
    }
    SetLastError(enteringError);
    NativeOne(g_builder_original.load(std::memory_order_acquire),crv,5);
    const DWORD nativeError=GetLastError();
    if(matched) {
        ++work->buildReturns;std::uint64_t hash=0;
        const unsigned expectedEye=f->pass==ControlPass::Eye1 && f->selected?1u:0u;
        work->builtPointerValid=ec::CheckBuilt(g_read,f->crv,expectedEye,hash);
        if(f->pass==ControlPass::Normal) {
            if(IsStereoDesign(*f)) {
                work->cameraSourceValid=f->leftPrepared&&!f->fallback;
                for(unsigned bank=0;bank<2 && work->cameraSourceValid;++bank) {
                    ec::CameraBytes live{};
                    work->cameraSourceValid=ec::ReadCamera(g_read,f->crv,bank,0,live)&&
                        ec::EqualStereoInputsAfterBuild(live,f->cameras.preparedLeft[bank])&&
                        ec::Field<std::uint32_t>(live,model::kCameraEyeTag)==0;
                }
                Log("V66.12-DESIGN BUILD-RETURN epoch=%llu frame=%u side=LEFT count=1 nativeCameraPointerMatch=%d record0Hash=%016llX",
                    (unsigned long long)f->epoch,f->frame,work->builtPointerValid?1:0,(unsigned long long)hash);
            } else work->cameraSourceValid=ec::CaptureCameras(g_read,f->crv,f->cameras);
        } else {
            work->cameraSourceValid=f->selected&&!f->fallback;
            for(unsigned bank=0;bank<2 && work->cameraSourceValid;++bank) {
                ec::CameraBytes live{};
                work->cameraSourceValid=ec::ReadCamera(g_read,f->crv,bank,1,live)&&
                    ec::EqualStereoInputsAfterBuild(live,f->cameras.prepared[bank])&&
                    ec::Field<std::uint32_t>(live,model::kCameraEyeTag)==1;
            }
            Log("V66.12-EYE BUILD-RETURN epoch=%llu frame=%u selector=%u count=1 "
                "nativeCameraPointerMatch=%d record0Hash=%016llX block1Forcing=0 GPUbinding=UNOBSERVED",
                (unsigned long long)f->epoch,f->frame,expectedEye,work->builtPointerValid?1:0,(unsigned long long)hash);
        }
    }
    t_native_stage=savedStage;SetLastError(nativeError);
}

void ObserveStandard(void* pipeline,bool enter,std::uint8_t result) noexcept {
    auto* f=t_eye_flow;auto* w=CurrentWork();if(!f||!w||f->pipeline!=reinterpret_cast<std::uintptr_t>(pipeline))return;
    if(enter) {
        ++w->standard;std::uint32_t frame=0,sel=~0u;std::uint64_t hash=0;
        const unsigned expected=f->pass==ControlPass::Eye1 && f->selected?1u:0u;
        w->standardStateValid=model::Read(g_read,f->crv,0x2C,frame)&&frame==f->frame&&
            model::Read(g_read,f->crv,0x8EE0,sel)&&sel==expected&&ec::CheckBuilt(g_read,f->crv,expected,hash);
        // INVESTIGATE_D67R1: command 1DF78B0 calls palette upload through
        // 7C2D40 -> 505DA8 BEFORE our 864728 PipelineDetour (return 1DF7928).
        // The extra eye below reuses 864728 without another upload. A pose or
        // attachment edit at this stage cannot be assumed to refresh the
        // already-uploaded bulk palette. Observe the ACTUAL upload-pool ID
        // (CRV+8D40), not the unrelated view ID (+2C). Next proof: join the
        // producer's data/output/transfer key to Henry's selected draw entry
        // and current/previous GPU bindings for BOTH eyes. Success is equal
        // body/prop publication in both draws with preceding drawn history.
        // This read-only receipt does not dereference retained job pointers,
        // traverse native lists, call native code, wait, or claim final pixels.
        if(diagnostics::native_palette_d67r1::view::enabled())
            diagnostics::native_palette_d67r1::view::observe(g_read,g_wh,f->renderer,f->crv,f->pipeline,
                f->frame,expected,f->epoch,GetTickCount64(),GetCurrentThreadId());
        if(f->pass==ControlPass::Eye1)Log("V66.12-EYE STANDARD-ENTER epoch=%llu frame=%u selector=%u "
            "stateAndBuiltPointerMatch=%d countExpected=1 CPUonly=1",(unsigned long long)f->epoch,frame,sel,w->standardStateValid?1:0);
    } else {++w->standardReturns;w->returnByte=result;}
}
bool StageMatches(void* stage) noexcept {
    if(!t_eye_flow||!CurrentWork())return false;
    const auto p=reinterpret_cast<std::uintptr_t>(stage);std::uintptr_t vt=0,owner=0,crv=0;
    return model::Read(g_read,p,0,vt)&&vt==g_wh+ec::kGBufferVtableRva&&
        model::Read(g_read,p,8,owner)&&owner==t_eye_flow->pipeline&&
        model::Read(g_read,p,0x28,crv)&&crv==t_eye_flow->crv;
}
void StageLog(const char* name,bool returned) noexcept {
    if(!t_eye_flow||t_eye_flow->pass!=ControlPass::Eye1)return;
    std::uint32_t sel=~0u;(void)model::Read(g_read,t_eye_flow->crv,0x8EE0,sel);
    Log("V66.12-EYE %s-%s epoch=%llu frame=%u selector=%u returnedNormally=%d CPUonly=1",
        name,returned?"RETURN":"ENTER",(unsigned long long)t_eye_flow->epoch,t_eye_flow->frame,sel,returned?1:0);
}
void __fastcall GBufferDetour(void* stage) {
    const DWORD error=GetLastError();const bool match=StageMatches(stage);auto* w=match?CurrentWork():nullptr;
    if(w){++w->gbuffer;StageLog("GBUFFER",false);}SetLastError(error);
    NativeOne(g_gbuffer_original.load(std::memory_order_acquire),stage,6);
    const DWORD after=GetLastError();if(w){++w->gbufferReturns;StageLog("GBUFFER",true);}SetLastError(after);
}
void __fastcall OpaqueDetour(void* stage) {
    const DWORD error=GetLastError();const bool match=StageMatches(stage);auto* w=match?CurrentWork():nullptr;
    bool returned=false;DWORD after=error;
    if(w){++w->opaque;++t_opaque_depth;StageLog("OPAQUE",false);}
    __try {SetLastError(error);NativeOne(g_opaque_original.load(std::memory_order_acquire),stage,7);after=GetLastError();returned=true;}
    __finally {if(w)--t_opaque_depth;}
    if(returned&&w){++w->opaqueReturns;StageLog("OPAQUE",true);}SetLastError(after);
}
void __fastcall RangeDetour(void* pass,void* crv,std::uint8_t list,std::int32_t start,std::int32_t end) {
    const DWORD error=GetLastError();auto* w=CurrentWork();
    if(w&&t_opaque_depth&&t_eye_flow->crv==reinterpret_cast<std::uintptr_t>(crv)) {
        ++w->ranges;const auto span=static_cast<long long>(end)-static_cast<long long>(start);
        if(start>=0&&span>0&&span<=10000000ll){++w->nonempty;w->requestedItems+=static_cast<std::uint64_t>(span);}
    }
    SetLastError(error);g_range_original.load(std::memory_order_acquire)(pass,crv,list,start,end);
}
void PrintWork(const EyeFlow& f,const char* pass,const ec::Work& w) noexcept {
    Log("V66.12-EYE WORK epoch=%llu frame=%u pass=%s buildEntries=%u buildReturns=%u "
        "standardEntries=%u standardReturns=%u returnByte=%u gbufferEntries=%u gbufferReturns=%u "
        "opaqueEntries=%u opaqueReturns=%u ranges=%u nonemptyRequests=%u requestedItems=%llu "
        "cameraSourceValid=%d builtPointerValid=%d standardStateValid=%d completedGpuDraws=UNOBSERVED",
        (unsigned long long)f.epoch,f.frame,pass,w.builds,w.buildReturns,w.standard,w.standardReturns,w.returnByte,
        w.gbuffer,w.gbufferReturns,w.opaque,w.opaqueReturns,w.ranges,w.nonempty,(unsigned long long)w.requestedItems,
        w.cameraSourceValid?1:0,w.builtPointerValid?1:0,w.standardStateValid?1:0);
}
void MaybeRunEyeControl(EyeFlow& f) {
    if(g_state.load(std::memory_order_acquire)!=Running)return;
    const unsigned required=IsStereoDesign(f)?4u:ec::kMinimumCompletedSamples;
    if(g_samples.load(std::memory_order_acquire)<required)return;
    bool fresh=false;if(!g_eye_decision_used.compare_exchange_strong(fresh,true,std::memory_order_acq_rel))return;
    PrintWork(f,"NORMAL",f.normal);
    const auto s=model::Capture(g_read,f.pipeline,g_wh);
    const auto e=rm::CaptureEvidence(g_read,s,g_wh,static_cast<std::uint32_t>(GetCurrentThreadId()));
    const char* reject=ec::RejectPersistentResources(s,e,g_wh,2);
    if(!reject&&(!ec::NormalComplete(f.normal)||s.crv!=f.crv||s.frame!=f.frame))reject="normal-native-consumer-chain-unconfirmed";
    if(!reject&&(g_pipeline_inflight.load(std::memory_order_acquire)!=1||g_inflight.load(std::memory_order_acquire)!=0))reject="overlapping-native-scope";
    if(!reject&&e.rt.renderer!=f.renderer)reject="renderer-owner-mismatch";
    std::uint8_t savedReturn=0;
    if(!reject&&!model::Read(g_read,f.renderer,0xF2,savedReturn))reject="normal-renderer-result-unreadable";
    if(reject) {
        if(f.camerasChanged)(void)RestoreCameraBanks(f);
        g_eye_outcome.store(1,std::memory_order_release);g_last_run_passed.store(false,std::memory_order_release);
        if(g_run_kind.load(std::memory_order_acquire)==1)g_f11_outcome.store(1,std::memory_order_release);
        Log("V66.12-EYE BLOCKED epoch=%llu runKind=%d designStage=%d reason=%s extraPipelineCalls=0 selectorWrites=0 noRetryThisRung=1",
            (unsigned long long)f.epoch,g_run_kind.load(),f.designStage,reject);
        if(IsStereoDesign(f))g_state.store(Done,std::memory_order_release);
        return;
    }
    f.checkpoint=s;f.resourceCheckpoint=e;
    int expected=Running;
    if(!g_state.compare_exchange_strong(expected,Witnessing,std::memory_order_acq_rel))return;
    g_eye_control_attempts.fetch_add(1,std::memory_order_acq_rel);g_eye_outcome.store(2,std::memory_order_release);

    if(CapturesStereoOutput(f)) {
        const auto seq=g_design_serial_counter.fetch_add(1,std::memory_order_acq_rel)+1;
        f.captureSerial=0xF968000000000000ull|(seq&0x0000FFFFFFFFFFFFull);
        if(RepeatsStereoFrames(f))stereo_design_capture::PrepareStreamingPair(f.captureSerial);
        else stereo_design_capture::PrepareCertifiedPair(f.captureSerial);
        f.captureLeftRecorded=stereo_design_capture::RecordCurrentMainTarget(f.captureSerial,0);
        Log("V66.12-DESIGN CAPTURE-LEFT epoch=%llu frame=%u designStage=%d serial=%llu recorded=%d boundary=AFTER-LEFT-PIPELINE stream=%d",
            (unsigned long long)f.epoch,f.frame,f.designStage,(unsigned long long)f.captureSerial,
            f.captureLeftRecorded?1:0,RepeatsStereoFrames(f)?1:0);
    }

    Log("V66.12-EYE CALL-BEGIN epoch=%llu frame=%u runKind=%d designStage=%d attemptsTotal=%u pipelineRva=0x864728 "
        "placement=AFTER-NORMAL-PIPELINE-BEFORE-COMMAND-FINALIZER separationMetres=%.6f "
        "count2=0 forcedBlock1=0 capture=%d risk=EXPERIMENTAL-NATIVE-REENTRY",
        (unsigned long long)f.epoch,f.frame,g_run_kind.load(),f.designStage,g_eye_control_attempts.load(),
        double(IsStereoDesign(f)?f.designIpd:0.0f),CapturesStereoOutput(f)?1:0);
    f.pass=ControlPass::Eye1;
    stereo_design_capture::SetOutputCensusEye(1,f.frame);
    g_eye_control_calls.fetch_add(1,std::memory_order_acq_rel);
    // V66.38 F9 optional Crysis-style inter-eye state reset. Reuse only the fresh
    // same-frame CSystem::RenderBegin argument snapshot; do not call CSystem::Render
    // again and do not regenerate a second game frame. RIGHT selector/camera setup
    // still occurs normally inside the pipeline builder after this reset.
    if(native_stereo_outer_lab::V6638InterEyeRenderBeginReset()) {
        (void)native_stereo_outer_lab::RunV6638InterEyeRenderBeginReset();
    }
    // Preserve the proven V66.26+ RIGHT pipeline behavior after the optional reset.
    NativePipeline(reinterpret_cast<void*>(f.renderer),reinterpret_cast<void*>(f.crv));
    // V66.54: put the LEFT SVO history back right after the RIGHT pass.
    ::kcd2vr::native_svo_right_v6654::OnRightPassEnd();
    g_eye_control_returns.fetch_add(1,std::memory_order_acq_rel);
    const bool temporalPairD82=native_upscaler_stereo_d82::PairReady(f.epoch,f.frame);
    const bool temporalCutR92=!temporalPairD82 && !f.fallback && f.designStage==4 &&
        native_upscaler_stereo_d82::RecoverableResetCut(f.epoch,f.frame,f.pipeline);
    if(!temporalPairD82) {
        // A known right-only reset discarded this pair, not the native eye
        // selection. Keep unrelated camera/build failures terminal as before.
        if(!temporalCutR92)f.fallback=true;
        Log("D82 UPSCALER_PAIR withheld epoch=%llu frame=%u reason=missing-successful-per-eye-evaluations",
            (unsigned long long)f.epoch,f.frame);
    }
    stereo_design_capture::SetOutputCensusEye(-1,f.frame);
    PrintWork(f,"EYE1-CONTROL",f.eye);
    g_eye_gbuffer_returns.store(f.eye.gbufferReturns);g_eye_opaque_returns.store(f.eye.opaqueReturns);
    g_eye_nonempty_requests.store(f.eye.nonempty);

    if(CapturesStereoOutput(f) && f.captureSerial) {
        f.captureRightRecorded=temporalPairD82 && stereo_design_capture::RecordCurrentMainTarget(f.captureSerial,1);
        Log("V66.12-DESIGN CAPTURE-RIGHT epoch=%llu frame=%u designStage=%d serial=%llu recorded=%d boundary=AFTER-RIGHT-PIPELINE stream=%d",
            (unsigned long long)f.epoch,f.frame,f.designStage,(unsigned long long)f.captureSerial,
            f.captureRightRecorded?1:0,RepeatsStereoFrames(f)?1:0);
        if(f.captureLeftRecorded&&f.captureRightRecorded)g_design_pair_serial.store(f.captureSerial,std::memory_order_release);
        else stereo_design_capture::AbortSerial(f.captureSerial);
    }

    // Normal-return cleanup only. Never execute guessed native cleanup or another
    // pipeline during exception unwinding. GPU/history rollback is NOT claimed.
    if(f.selected)StoreSelector(f.crv,0);
    const bool camerasRestored=RestoreCameraBanks(f);
    f.pass=ControlPass::Restore;
    NativeOne(g_builder_original.load(std::memory_order_acquire),reinterpret_cast<void*>(f.crv),8);
    std::uint64_t rebuiltHash=0;std::uint32_t selector=~0u,phase=0;std::uint8_t schedule=1;
    const bool builtRestored=ec::CheckBuilt(g_read,f.crv,0,rebuiltHash);
    const bool selectorRead=model::Read(g_read,f.crv,0x8EE0,selector);
    const bool phaseRead=model::Read(g_read,f.pipeline,0x1974,phase);
    const bool scheduleRead=model::Read(g_read,f.pipeline,0x1A0,schedule);
    bool restored=camerasRestored&&builtRestored&&selectorRead&&selector==0&&phaseRead&&phase==2&&scheduleRead&&schedule==0;
    StoreRendererResult(f.renderer,savedReturn);
    std::uint8_t resultAfter=0;
    const bool resultRestored=model::Read(g_read,f.renderer,0xF2,resultAfter)&&resultAfter==savedReturn;
    restored=restored&&resultRestored;
    const bool cpu=f.selected&&!f.fallback&&ec::NormalComplete(f.eye);
    const bool captureOk=!CapturesStereoOutput(f)||(f.captureLeftRecorded&&f.captureRightRecorded);
    unsigned outcome=!restored?6u:(f.fallback?5u:(cpu?3u:4u));
    g_eye_outcome.store(outcome,std::memory_order_release);
    const bool passed=outcome==3u&&captureOk&&temporalPairD82;
    // Retry only after every original native cleanup/CPU postcondition passed.
    // No success receipt, completion count or image publication is fabricated.
    const bool retryCutR92=temporalCutR92 && outcome==3u && restored &&
        !g_first_fault.load(std::memory_order_acquire) && !g_velocity_resize_failed.load(std::memory_order_acquire) &&
        native_upscaler_stereo_d82::RecoverableResetCut(f.epoch,f.frame,f.pipeline);
    g_last_run_passed.store(passed,std::memory_order_release);
    const int kind=g_run_kind.load(std::memory_order_acquire);
    if(kind==1)g_f11_outcome.store(outcome,std::memory_order_release);
    if(kind==1 && passed)g_eye1_verified.store(true,std::memory_order_release);
    if(IsStereoDesign(f)&&passed&&!RepeatsStereoFrames(f))
        g_last_design_stage_passed.store(f.designStage,std::memory_order_release);
    Log("V66.12-EYE RESULT epoch=%llu frame=%u runKind=%d designStage=%d outcome=%s passed=%d extraPipelineReturned=1 eye1Selected=%d "
        "cameraBanksRestored=%d selectorAfter=%u built0NativeLeft=%d scheduleAfter=%u pipelinePhaseAfter=%u "
        "ownedMemberChanged=%d countWrites=0 rendererResultRestored=%d originalResultByte=%u captureLR=%d/%d captureSerial=%llu "
        "historyRollback=NOT-ATTEMPTED rightPrep7C2D40=RETIRED v6625SharedWorkFreeze=RETIRED GPUcompletion=UNOBSERVED stereoSuccess=%s",
        (unsigned long long)f.epoch,f.frame,kind,f.designStage,EyeOutcomeName(outcome),passed?1:0,f.selected?1:0,camerasRestored?1:0,
        selector,builtRestored?1:0,static_cast<unsigned>(schedule),phase,f.ownedMemberChanged?1:0,resultRestored?1:0,static_cast<unsigned>(savedReturn),
        f.captureLeftRecorded?1:0,f.captureRightRecorded?1:0,(unsigned long long)f.captureSerial,
        CapturesStereoOutput(f)&&passed?(RepeatsStereoFrames(f)?"LIVE-PAIR-RECORDED-AWAITING-DIRECT-SUBMIT":"PAIR-RECORDED-AWAITING-DIRECT-SUBMIT"):"NOT-CLAIMED");
    if(!restored)g_state.store(Faulted,std::memory_order_release);
    else if(RepeatsStereoFrames(f)) {
        if(!passed) {
            if(retryCutR92) {
                Log("R92 CONTINUOUS FRAME-DISCARDED epoch=%llu frame=%u reason=right-reset-cut retry=NEXT-NATIVE-PAIR stereoStopped=0 pairPublished=0",
                    (unsigned long long)f.epoch,f.frame);
                g_eye_decision_used.store(false,std::memory_order_release);
                g_eye_outcome.store(0,std::memory_order_release);
                int witnessing=Witnessing;(void)g_state.compare_exchange_strong(witnessing,Running,std::memory_order_acq_rel);
            } else g_state.store(Done,std::memory_order_release);
        } else {
            const unsigned completed=g_repeat_completed.fetch_add(1,std::memory_order_acq_rel)+1;
            bool finish=false;
            if(f.designStage==3) {
                unsigned before=g_repeat_remaining.load(std::memory_order_acquire);
                while(before>0 && !g_repeat_remaining.compare_exchange_weak(before,before-1,std::memory_order_acq_rel)) {}
                const unsigned remaining=before>0?before-1:0;
                if(remaining==0) {
                    g_last_design_stage_passed.store(3,std::memory_order_release);
                    Log("V66.12-DESIGN BURST-COMPLETE completedFrames=%u target=%u lastPairSerial=%llu",
                        completed,kStereoBurstFrames,(unsigned long long)g_design_pair_serial.load());
                    finish=true;
                } else if(completed<=3 || (completed%10)==0) {
                    Log("V66.12-DESIGN BURST-PROGRESS completedFrames=%u remaining=%u lastPairSerial=%llu",
                        completed,remaining,(unsigned long long)g_design_pair_serial.load());
                }
            } else {
                if(completed==1)g_last_design_stage_passed.store(4,std::memory_order_release);
                if(completed<=3 || (completed%120)==0)
                    Log("V66.12-CONTINUOUS FRAME completed=%u lastPairSerial=%llu",
                        completed,(unsigned long long)g_design_pair_serial.load());
            }
            if(finish)g_state.store(Done,std::memory_order_release);
            else {
                g_eye_decision_used.store(false,std::memory_order_release);
                g_eye_outcome.store(0,std::memory_order_release);
                int witnessing=Witnessing;(void)g_state.compare_exchange_strong(witnessing,Running,std::memory_order_acq_rel);
            }
        }
    } else if(IsStereoDesign(f))g_state.store(Done,std::memory_order_release);
    else {int witnessing=Witnessing;(void)g_state.compare_exchange_strong(witnessing,Running,std::memory_order_acq_rel);}
    f.pass=ControlPass::Normal;
}

void TraceSemanticOutputBoundary(EyeFlow& f) noexcept {
    if(!IsStereoDesign(f) || (f.pass!=ControlPass::Normal && f.pass!=ControlPass::Eye1))return;
    const int eye=f.pass==ControlPass::Eye1?1:0;
    std::uintptr_t renderOutput=0,texture=0,backing=0;std::uint16_t width=0,height=0;
    const bool base=model::Read(g_read,f.crv,0x8D78,renderOutput)&&renderOutput&&
        model::Read(g_read,renderOutput,0x40,texture)&&texture&&
        model::Read(g_read,texture,0x80,backing)&&backing&&
        model::Read(g_read,texture,0x90,width)&&model::Read(g_read,texture,0x92,height);
    rm::TextureMeta meta{};rm::Views views{};
    if(base){meta=rm::CaptureMeta(g_read,texture);views=rm::CaptureViews(g_read,backing);}
    const bool valid=base&&meta.readable&&meta.stable&&meta.vtable==g_wh+rm::kTextureVtableRva&&width&&height;
    Log("V66.12-F9-SEMANTIC-BOUNDARY eye=%c frame=%u crv=%p renderOutput=%p texture=%p backing=%p valid=%d "
        "name=%s size=%ux%u sourceFmt=%u deviceFmt=%u flags=0x%08X viewHeaderRead=%d viewBounded=%d viewCount=%u nativeRtvView=%p nativeRtvKey=0x%016llX "
        "boundary=WHGame+8645F8-BEFORE-FINAL-COMPOSE mutation=NONE",
        eye?'R':'L',f.frame,reinterpret_cast<void*>(f.crv),reinterpret_cast<void*>(renderOutput),reinterpret_cast<void*>(texture),
        reinterpret_cast<void*>(backing),valid?1:0,(meta.readable&&meta.name[0])?meta.name:"<unreadable>",(unsigned)width,(unsigned)height,
        (unsigned)meta.sourceFormat,(unsigned)meta.deviceFormat,meta.flags,views.headerRead?1:0,views.bounded?1:0,views.count,
        reinterpret_cast<void*>(views.rtv),(unsigned long long)views.rtvKey);
    if(valid)stereo_design_capture::RecordSemanticOutput(eye,f.frame,renderOutput,texture,backing,views.rtv,views.rtvKey,
        width,height,meta.sourceFormat,meta.deviceFormat,meta.flags,meta.name);
}

struct ComposeTextureInfo {
    std::uintptr_t texture=0,backing=0,rtv=0;std::uint64_t rtvKey=0;
    std::uint16_t width=0,height=0;std::uint8_t sourceFormat=0,deviceFormat=0;std::uint32_t flags=0;
    bool valid=false;char name[96]{};
};
ComposeTextureInfo ReadComposeTexture(void* value) noexcept {
    ComposeTextureInfo out{};out.texture=reinterpret_cast<std::uintptr_t>(value);if(!out.texture)return out;
    if(!model::Read(g_read,out.texture,0x80,out.backing)||!out.backing||
       !model::Read(g_read,out.texture,0x90,out.width)||!model::Read(g_read,out.texture,0x92,out.height))return out;
    const auto meta=rm::CaptureMeta(g_read,out.texture);const auto views=rm::CaptureViews(g_read,out.backing);
    out.valid=meta.readable&&meta.stable&&meta.vtable==g_wh+rm::kTextureVtableRva&&out.width&&out.height;
    out.sourceFormat=meta.sourceFormat;out.deviceFormat=meta.deviceFormat;out.flags=meta.flags;out.rtv=views.rtv;out.rtvKey=views.rtvKey;
    std::snprintf(out.name,sizeof(out.name),"%s",(meta.readable&&meta.name[0])?meta.name:"<unreadable>");
    return out;
}

// Re-read the native scheduling predicate at the exact call, not only at the
// earlier stereo checkpoint. WHGame+501240 defers only when both owner+1A0
// and its cached scheduler setting +18F4 are nonzero; deferred work has no TLS
// draw here and is deliberately not certified by this synchronous contract.
bool DirectFinalCompositionD82(const EyeFlow& f,void* context) noexcept {
    const auto address=reinterpret_cast<std::uintptr_t>(context);
    std::uintptr_t expected=0,owner=0,schedulerOwner=0;
    if(!address||!model::Read(g_read,f.pipeline,0x18,expected)||expected!=address||
       !model::Read(g_read,address,0x2C0,owner))return false;
    if(!owner)return true;
    std::uint8_t active=0;std::uint32_t deferred=0;
    if(owner!=f.pipeline||!model::Read(g_read,owner,0x1A8,schedulerOwner)||schedulerOwner!=f.pipeline||
       !model::Read(g_read,owner,0x1A0,active)||!model::Read(g_read,schedulerOwner,0x18F4,deferred))return false;
    return !(active&&deferred);
}

void __fastcall ComposeCallDetour(void* context,void* inputTexture,void* outputTexture) {
    const auto caller=reinterpret_cast<std::uintptr_t>(_ReturnAddress());
    const DWORD entering=GetLastError();auto* f=t_eye_flow;bool scoped=false,temporalScoped=false;DWORD after=entering;bool returned=false;
    if(f && IsStereoDesign(*f) && (f->pass==ControlPass::Normal || f->pass==ControlPass::Eye1)) {
        const int eye=f->pass==ControlPass::Eye1?1:0;
        const auto in=ReadComposeTexture(inputTexture);const auto out=ReadComposeTexture(outputTexture);
        Log("V66.12-F9-COMPOSE-ARGS eye=%c frame=%u context=%p inputTexture=%p inputBacking=%p inputValid=%d inputName='%s' inputSize=%ux%u inputSourceFmt=%u inputDeviceFmt=%u inputRtvView=%p "
            "outputTexture=%p outputBacking=%p outputValid=%d outputName='%s' outputSize=%ux%u outputSourceFmt=%u outputDeviceFmt=%u outputRtvView=%p "
            "helper=WHGame+2543A6C roleContract=EXACT-STATIC:{RDX=input->WHGame+42BBF8,R8=output->WHGame+23CA7F8} mutation=NONE",
            eye?'R':'L',f->frame,context,inputTexture,reinterpret_cast<void*>(in.backing),in.valid?1:0,in.name,(unsigned)in.width,(unsigned)in.height,
            (unsigned)in.sourceFormat,(unsigned)in.deviceFormat,reinterpret_cast<void*>(in.rtv),outputTexture,reinterpret_cast<void*>(out.backing),out.valid?1:0,out.name,
            (unsigned)out.width,(unsigned)out.height,(unsigned)out.sourceFormat,(unsigned)out.deviceFormat,reinterpret_cast<void*>(out.rtv));
        stereo_design_capture::BeginCompositionTrace(eye,f->frame,in.texture,in.backing,in.rtv,out.texture,out.backing,out.rtv);scoped=true;
        // Exact-image signature below validates CALL at0x864654 ->0x2543A6C.
        // SDK evaluation returns in Standard before this native FinalCompose.
        // Existing diagnostic trace becomes inactive after bootstrap; the new
        // lightweight scope must remain live on every temporal frame.
        UpscalerView view{};
        if(f->designStage==4&&eye==1&&t_d82_final_compose_flow==f&&
           caller==g_wh+0x864659&&in.valid&&out.valid&&
           native_upscaler_stereo_d82::TemporalActive()&&DirectFinalCompositionD82(*f,context)&&CurrentUpscalerView(view)&&
           view.eye==1&&view.epoch==f->epoch&&view.frame==f->frame&&view.pipeline==f->pipeline)
            temporalScoped=stereo_design_capture::BeginTemporalCompositionD82(view.epoch,view.frame);
    }
    __try {SetLastError(entering);g_compose_call_original.load(std::memory_order_acquire)(context,inputTexture,outputTexture);after=GetLastError();returned=true;}
    __finally {
        if(temporalScoped)stereo_design_capture::EndTemporalCompositionD82();
        if(scoped)stereo_design_capture::EndCompositionTrace();SetLastError(returned?after:entering);
    }
}

void __fastcall FinalComposeDetour(void* renderer,void* pipeline) {
    const DWORD entering=GetLastError();DWORD after=entering;bool returned=false;
    auto* f=t_eye_flow;auto* savedFinal=t_d82_final_compose_flow;
    const bool owned=f&&f->renderer==reinterpret_cast<std::uintptr_t>(renderer)&&f->pipeline==reinterpret_cast<std::uintptr_t>(pipeline);
    t_d82_final_compose_flow=owned?f:nullptr;
    if(owned)TraceSemanticOutputBoundary(*f);
    __try {
        SetLastError(entering);g_final_compose_original.load(std::memory_order_acquire)(renderer,pipeline);
        after=GetLastError();returned=true;
    }
    __finally {t_d82_final_compose_flow=savedFinal;SetLastError(returned?after:entering);}
}

void __fastcall PipelineDetour(void* renderer,void* crv) {
    const DWORD enteringError=GetLastError();g_pipeline_inflight.fetch_add(1,std::memory_order_acq_rel);
    EyeFlow flow{};auto* savedFlow=t_eye_flow;
    const auto savedPipeline=t_pipeline,savedCrv=t_crv;const auto savedEpoch=t_epoch;const auto savedStage=t_native_stage;
    bool returned=false;DWORD normalError=enteringError;
    const auto caller=reinterpret_cast<std::uintptr_t>(_ReturnAddress());
    flow.renderer=reinterpret_cast<std::uintptr_t>(renderer);flow.crv=reinterpret_cast<std::uintptr_t>(crv);
    flow.epoch=g_epoch.load(std::memory_order_acquire);
    const int runKind=g_run_kind.load(std::memory_order_acquire);
    flow.designStage=runKind>=2?runKind-1:0;
    // V66.48: zero-IPD diagnostic keeps BOTH passes running, with identical center cameras.
    flow.designIpd=flow.designStage?(g_v6648_zero_ipd.load(std::memory_order_acquire)?0.0f:g_design_ipd.load(std::memory_order_acquire)):0.0f;
    std::uintptr_t chosen=0;
    const bool tracked=!savedFlow&&!g_eye_decision_used.load(std::memory_order_acquire)&&Active(g_state.load(std::memory_order_acquire))&&caller==g_wh+ec::kCommandReturnRva&&
        model::Read(g_read,flow.crv,0x8D60,flow.pipeline)&&Qualify(flow.pipeline,chosen)&&chosen==flow.crv&&
        model::Read(g_read,flow.crv,0x2C,flow.frame);
    if(tracked){t_eye_flow=&flow;t_pipeline=flow.pipeline;t_crv=flow.crv;t_epoch=flow.epoch;}
    __try {
        SetLastError(enteringError);NativePipeline(renderer,crv);normalError=GetLastError();returned=true;
        if(tracked)MaybeRunEyeControl(flow);
    } __finally {
        t_eye_flow=savedFlow;t_pipeline=savedPipeline;t_crv=savedCrv;t_epoch=savedEpoch;t_native_stage=savedStage;
        g_pipeline_inflight.fetch_sub(1,std::memory_order_acq_rel);
        if(returned)SetLastError(normalError);
    }
}
bool CheckEyeContracts() noexcept {
    for(const auto& s:kEyeSignatures) {
        unsigned char b[32]{};
        if(!g_read(g_wh+s.rva,b,s.size)||std::memcmp(b,s.bytes,s.size)) {
            Log("V66.12-EYE REFUSED reason=instruction-contract rva=0x%llX",(unsigned long long)s.rva);return false;
        }
    }
    for(const auto& s:kEyeRanges) {
        unsigned char b[4096]{};
        if(s.size>sizeof(b)||!g_read(g_wh+s.rva,b,s.size)||model::Hash(b,s.size)!=s.hash) {
            Log("V66.12-EYE REFUSED reason=code-range rva=0x%llX",(unsigned long long)s.rva);return false;
        }
    }
    return true;
}
template<class Fn>
bool CreateEyeHook(std::optional<DetourModKit::hook::Hook>& handle,std::atomic<Fn>& original,
                   const char* name,std::uintptr_t rva,Fn detour) noexcept {
    DetourModKit::hook::InlineRequest request{name,DetourModKit::hook::Target{DetourModKit::Address{g_wh+rva}},DetourModKit::hook::Options{}};
    auto h=DetourModKit::hook::inline_at(std::move(request),detour);if(!h)return false;
    handle=std::move(*h);original.store(handle->original<Fn>(),std::memory_order_release);
    return original.load(std::memory_order_acquire)!=nullptr;
}
bool PrepareEyeHooks() noexcept {
    // Called before any hook is enabled. All original pointers are published
    // before enabling; failed setup is process-terminal, no live unload/reset.
    return CreateEyeHook(g_builder_hook,g_builder_original,"v6604_camera_consumer",ec::kBuilderRva,static_cast<VoidOneFn>(&BuilderDetour))&&
        CreateEyeHook(g_gbuffer_hook,g_gbuffer_original,"v6604_gbuffer_return",ec::kGBufferRva,static_cast<VoidOneFn>(&GBufferDetour))&&
        CreateEyeHook(g_opaque_hook,g_opaque_original,"v6604_opaque_return",ec::kOpaqueRva,static_cast<VoidOneFn>(&OpaqueDetour))&&
        CreateEyeHook(g_range_hook,g_range_original,"v6604_opaque_ranges",ec::kRangesRva,static_cast<RangeFn>(&RangeDetour))&&
        CreateEyeHook(g_compose_call_hook,g_compose_call_original,"v6612_exact_final_compose_call",0x2543A6C,static_cast<ComposeCallFn>(&ComposeCallDetour))&&
        CreateEyeHook(g_final_compose_hook,g_final_compose_original,"v6610_final_compose_semantic_owner",0x8645F8,static_cast<FinalComposeFn>(&FinalComposeDetour))&&
        CreateEyeHook(g_pipeline_hook,g_pipeline_original,"v6604_pipeline_eye_control",ec::kPipelineRva,static_cast<PipelineFn>(&PipelineDetour));
}
bool EnableEyeHooks() noexcept {
    return static_cast<bool>(g_builder_hook->enable())&&static_cast<bool>(g_gbuffer_hook->enable())&&
        static_cast<bool>(g_opaque_hook->enable())&&static_cast<bool>(g_range_hook->enable())&&
        static_cast<bool>(g_compose_call_hook->enable())&&static_cast<bool>(g_final_compose_hook->enable())&&static_cast<bool>(g_pipeline_hook->enable());
}
