// R149 alternate-eye frame dump. Included inside namespace
// kcd2vr::native_dlss_stereo_d82 { namespace { after native_dlss_aer_d82.inl.
//
// Captures N consecutive live AFR eye frames at the NGX boundary (colour, depth,
// game vectors, the vectors DLSS actually received, DLSS output, both CRenderView
// camera banks, the retained same-eye camera, identities, parameters), plus the
// XR composition of those frames (post-DLSS cached eye image and both warped eye
// images with their warp constants). Readbacks are recorded on the command lists
// that already touch the resources; files are written by a background thread
// after a fence on the game DIRECT queue. Nothing runs unless a dump is armed.

struct AerDumpTexture {
    std::string name;
    ComPtr<ID3D12Resource> readback;
    D3D12_PLACED_SUBRESOURCE_FOOTPRINT footprint{};
    UINT rows{};UINT64 rowBytes{},total{};
    UINT width{},height{};
    DXGI_FORMAT format{},sourceFormat{};
    std::string state;
};
struct AerDumpRecord {
    std::string kind;   // "dlss" or "xr"
    unsigned index{};
    unsigned eye{};
    std::string json;   // object members without braces
    std::vector<AerDumpTexture> textures;
};
struct AerDumpXrCaptureBody {AerDumpRecord record;};

enum : int {kAerDumpIdle=0,kAerDumpArmed=1,kAerDumpCapturing=2,kAerDumpFlushing=3};
struct AerDumpSession {
    std::mutex mutex;
    std::atomic<int> phase{kAerDumpIdle};
    std::atomic<bool> xrWanted{false};
    std::uint64_t requestTick{},startTick{},captureTick{},dlssDoneTick{};
    unsigned framesWanted{4},dlssCaptured{},xrBlits{},xrNewFrames{},xrMaxBlits{8},presentTicksAfterDone{};
    bool dlssDone{};
    std::vector<AerDumpRecord> records;
    std::string global;
    ComPtr<ID3D12Resource> scratch[3];D3D12_RESOURCE_DESC scratchDesc[3]{};
    ComPtr<ID3D12Fence> fence;std::uint64_t fenceValue{};
    std::vector<ComPtr<ID3D12Resource>> neverRelease; // readbacks of a list whose completion was not observed
    unsigned session{};
};
AerDumpSession g_dump{};

// ---------------------------------------------------------------- JSON
void J(std::string& s,const char* key,const char* fmt,...) noexcept {
    char value[512]{};va_list args;va_start(args,fmt);std::vsnprintf(value,sizeof(value),fmt,args);va_end(args);
    if(!s.empty()&&s.back()!='{'&&s.back()!='[')s+=',';
    s+='"';s+=key;s+="\":";s+=value;
}
void JStr(std::string& s,const char* key,const char* text) noexcept {
    if(!s.empty()&&s.back()!='{'&&s.back()!='[')s+=',';
    s+='"';s+=key;s+="\":\"";
    for(const char* c=text?text:"";*c;++c){if(*c=='"'||*c=='\\')s+='\\';if(static_cast<unsigned char>(*c)>=0x20)s+=*c;}
    s+='"';
}
void JFloats(std::string& s,const char* key,const float* v,unsigned n) noexcept {
    if(!s.empty()&&s.back()!='{'&&s.back()!='[')s+=',';
    s+='"';s+=key;s+="\":[";char x[48]{};
    for(unsigned i=0;i<n;++i){std::snprintf(x,sizeof(x),"%s%.9g",i?",":"",std::isfinite(v[i])?double(v[i]):0.0);s+=x;}
    s+=']';
}
void JDoubles(std::string& s,const char* key,const double* v,unsigned n) noexcept {
    if(!s.empty()&&s.back()!='{'&&s.back()!='[')s+=',';
    s+='"';s+=key;s+="\":[";char x[48]{};
    for(unsigned i=0;i<n;++i){std::snprintf(x,sizeof(x),"%s%.12g",i?",":"",std::isfinite(v[i])?v[i]:0.0);s+=x;}
    s+=']';
}
void JOpen(std::string& s,const char* key) noexcept {
    if(!s.empty()&&s.back()!='{'&&s.back()!='[')s+=',';
    s+='"';s+=key;s+="\":{";
}
void JClose(std::string& s) noexcept {s+='}';}
const char* AerFormatName(DXGI_FORMAT f) noexcept {
    switch(f){
    case DXGI_FORMAT_R32G32B32A32_FLOAT:return "R32G32B32A32_FLOAT";case DXGI_FORMAT_R16G16B16A16_FLOAT:return "R16G16B16A16_FLOAT";
    case DXGI_FORMAT_R16G16B16A16_TYPELESS:return "R16G16B16A16_TYPELESS";case DXGI_FORMAT_R11G11B10_FLOAT:return "R11G11B10_FLOAT";
    case DXGI_FORMAT_R10G10B10A2_UNORM:return "R10G10B10A2_UNORM";case DXGI_FORMAT_R10G10B10A2_TYPELESS:return "R10G10B10A2_TYPELESS";
    case DXGI_FORMAT_R8G8B8A8_UNORM:return "R8G8B8A8_UNORM";case DXGI_FORMAT_R8G8B8A8_UNORM_SRGB:return "R8G8B8A8_UNORM_SRGB";
    case DXGI_FORMAT_R8G8B8A8_TYPELESS:return "R8G8B8A8_TYPELESS";case DXGI_FORMAT_B8G8R8A8_UNORM:return "B8G8R8A8_UNORM";
    case DXGI_FORMAT_B8G8R8A8_UNORM_SRGB:return "B8G8R8A8_UNORM_SRGB";case DXGI_FORMAT_B8G8R8A8_TYPELESS:return "B8G8R8A8_TYPELESS";
    case DXGI_FORMAT_R32G32_FLOAT:return "R32G32_FLOAT";case DXGI_FORMAT_R16G16_FLOAT:return "R16G16_FLOAT";
    case DXGI_FORMAT_R16G16_TYPELESS:return "R16G16_TYPELESS";case DXGI_FORMAT_R32G32_TYPELESS:return "R32G32_TYPELESS";
    case DXGI_FORMAT_R32_FLOAT:return "R32_FLOAT";case DXGI_FORMAT_R32_TYPELESS:return "R32_TYPELESS";case DXGI_FORMAT_D32_FLOAT:return "D32_FLOAT";
    case DXGI_FORMAT_R24G8_TYPELESS:return "R24G8_TYPELESS";case DXGI_FORMAT_D24_UNORM_S8_UINT:return "D24_UNORM_S8_UINT";
    case DXGI_FORMAT_R32G8X24_TYPELESS:return "R32G8X24_TYPELESS";case DXGI_FORMAT_D32_FLOAT_S8X24_UINT:return "D32_FLOAT_S8X24_UINT";
    case DXGI_FORMAT_R16_FLOAT:return "R16_FLOAT";case DXGI_FORMAT_R16_UNORM:return "R16_UNORM";case DXGI_FORMAT_D16_UNORM:return "D16_UNORM";
    default:return "OTHER";
    }
}
void JDesc(std::string& s,const char* key,const D3D12_RESOURCE_DESC& d) noexcept {
    JOpen(s,key);J(s,"width","%llu",(unsigned long long)d.Width);J(s,"height","%u",d.Height);
    J(s,"format","%u",unsigned(d.Format));JStr(s,"formatName",AerFormatName(d.Format));
    J(s,"flags","%u",unsigned(d.Flags));J(s,"mips","%u",unsigned(d.MipLevels));J(s,"array","%u",unsigned(d.DepthOrArraySize));JClose(s);
}
void JCamera(std::string& s,const char* key,const AerCamera& c,bool raw) noexcept {
    JOpen(s,key);J(s,"valid","%s",c.valid?"true":"false");JStr(s,"failure",c.failure);
    JDoubles(s,"rotation",c.rot,9);JDoubles(s,"position",c.pos,3);
    // Raw-decoded fields can be non-finite when a layout check failed; keep JSON strict.
    auto fin=[](float v){return std::isfinite(v)?double(v):0.0;};
    J(s,"fov","%.9g",fin(c.fov));J(s,"near","%.9g",fin(c.nearZ));J(s,"far","%.9g",fin(c.farZ));
    J(s,"tanX","%.9g",fin(c.tanX));J(s,"tanY","%.9g",fin(c.tanY));J(s,"width","%d",c.width);J(s,"height","%d",c.height);
    J(s,"projRatio","%.9g",fin(c.projRatio));J(s,"pixelAspect","%.9g",fin(c.pixelAspect));
    JFloats(s,"edgeNearLeftTop",c.nlt,3);JFloats(s,"edgeProjLeftTop",c.plt,3);JFloats(s,"edgeFarLeftTop",c.flt,3);
    J(s,"eyeTag","%u",c.eyeTag);
    if(raw){
        std::string hex;hex.reserve(kAerCameraBytes*2);static const char digits[]="0123456789abcdef";
        for(std::size_t i=0;i<kAerCameraBytes;++i){hex+=digits[c.raw[i]>>4];hex+=digits[c.raw[i]&15];}
        JStr(s,"rawHex",hex.c_str());
    }
    JClose(s);
}
void JEye(std::string& s,const char* key,const AerEye& e) noexcept {
    JOpen(s,key);J(s,"valid","%s",e.valid?"true":"false");J(s,"frame","%u",e.frame);
    JDoubles(s,"rotation",e.rot,9);JDoubles(s,"position",e.pos,3);
    auto fin=[](float v){return std::isfinite(v)?double(v):0.0;};
    J(s,"tanX","%.9g",fin(e.tanX));J(s,"tanY","%.9g",fin(e.tanY));J(s,"near","%.9g",fin(e.nearZ));J(s,"far","%.9g",fin(e.farZ));
    float jit[2]{e.jitterX,e.jitterY};JFloats(s,"jitter",jit,2);JClose(s);
}
void JIdentity(std::string& s,const char* key,const AerRenderIdentity& id,bool known) noexcept {
    JOpen(s,key);J(s,"known","%s",known?"true":"false");J(s,"source","%d",id.source);
    JStr(s,"sourceName",id.source==1?"exact-publication-match":id.source==2?"render-frame-parity":id.source==3?"main-thread-fallback":"none");
    J(s,"eye","%d",id.eye);J(s,"publishSerial","%llu",(unsigned long long)id.publishSerial);J(s,"poseFrame","%llu",(unsigned long long)id.poseFrame);
    J(s,"mainSerial","%llu",(unsigned long long)id.mainSerial);J(s,"mainEye","%d",id.mainEye);J(s,"renderFrame","%u",id.renderFrame);
    J(s,"matchPosM","%.9g",std::isfinite(id.matchPosM)?double(id.matchPosM):-1.0);J(s,"matchRot","%.9g",std::isfinite(id.matchRot)?double(id.matchRot):-1.0);J(s,"tickMs","%llu",(unsigned long long)id.tickMs);
    JClose(s);
}
std::string AerFrameJson(const AerFrameInfo& f) noexcept {
    std::string s;s.reserve(8192);
    J(s,"eye","%u",f.eye);J(s,"frame","%u",f.frame);J(s,"epoch","%llu",(unsigned long long)f.epoch);
    J(s,"pipeline","\"0x%llX\"",(unsigned long long)f.pipeline);J(s,"crv","\"0x%llX\"",(unsigned long long)f.crv);J(s,"renderFrame","%u",f.renderFrame);
    JIdentity(s,"identity",f.identity,f.identityKnown);
    JOpen(s,"mainThread");J(s,"serial","%llu",(unsigned long long)FrameSync::CurrentFrameSerial());J(s,"eye","%d",FrameSync::CurrentEye());JClose(s);
    J(s,"nativeReset","%d",f.nativeReset);J(s,"createFlags","%d",f.createFlags);J(s,"motionMode","%d",f.motionMode);
    J(s,"depthMode","%d",f.depthMode);JStr(s,"depthModeName",AerDepthName(f.depthMode));JStr(s,"decision",f.decision);
    J(s,"continuity","%s",f.continuity?"true":"false");J(s,"duplicate","%s",f.duplicate?"true":"false");
    J(s,"sameEyeHistory","%s",f.sameEyeHistory?"true":"false");J(s,"cut","%s",f.cut?"true":"false");
    J(s,"historyJumpM","%.9g",std::isfinite(f.jumpM)?f.jumpM:0.0);J(s,"historyJumpDeg","%.9g",std::isfinite(f.jumpDeg)?f.jumpDeg:0.0);
    J(s,"dispatched","%s",f.dispatched?"true":"false");J(s,"modelValid","%s",f.modelValid?"true":"false");
    J(s,"substituted","%s",f.substituted?"true":"false");J(s,"effectiveReset","%s",f.effectiveReset?"true":"false");
    JOpen(s,"params");
    float jit[2]{f.in.jitterX,f.in.jitterY},scale[2]{f.in.mvScaleX,f.in.mvScaleY};
    JFloats(s,"jitter",jit,2);JFloats(s,"mvScale",scale,2);
    J(s,"preExposure","%.9g",double(f.in.preExposure));J(s,"exposureScale","%.9g",double(f.in.exposureScale));J(s,"sharpness","%.9g",double(f.in.sharpness));
    J(s,"renderSubrect","[%u,%u]",f.in.subW,f.in.subH);J(s,"colorBase","[%u,%u]",f.in.colorX,f.in.colorY);
    J(s,"depthBase","[%u,%u]",f.in.depthX,f.in.depthY);J(s,"mvBase","[%u,%u]",f.in.mvX,f.in.mvY);J(s,"outputBase","[%u,%u]",f.in.outX,f.in.outY);
    J(s,"widthHeight","[%u,%u]",f.in.width,f.in.height);J(s,"outWidthHeight","[%u,%u]",f.in.outWidth,f.in.outHeight);
    J(s,"render","[%u,%u]",f.in.renderW,f.in.renderH);J(s,"mvActive","[%u,%u]",f.in.mvW,f.in.mvH);
    J(s,"inputsOk","%s",f.in.ok?"true":"false");JClose(s);
    JOpen(s,"resources");
    if(f.in.color)JDesc(s,"color",f.in.colorDesc);if(f.in.depth)JDesc(s,"depth",f.in.depthDesc);
    if(f.in.motion)JDesc(s,"motion",f.in.motionDesc);if(f.in.output)JDesc(s,"output",f.in.outputDesc);
    JClose(s);
    J(s,"camerasOk","%s",f.camerasOk?"true":"false");JStr(s,"cameraFailure",f.cameraFailure);
    JCamera(s,"cameraCurrent",f.cur,true);JCamera(s,"cameraPrevious",f.prev,true);JEye(s,"cameraHistory",f.history);
    JFloats(s,"rowsCurrentToPrevious",f.prevRows,12);JFloats(s,"rowsCurrentToHistory",f.histRows,12);
    JFloats(s,"jitterFix",f.jitterFix,2);J(s,"objectGain","%.9g",double(f.objectGain));J(s,"gateFactor","%.9g",double(f.gateFactor));
    const auto& m=g_aer_model;
    JOpen(s,"model");J(s,"valid","%s",m.valid?"true":"false");J(s,"depthMode","%d",m.depthMode);JFloats(s,"scores",m.score,4);JFloats(s,"precise",m.precise,4);
    J(s,"readbacks","%llu",(unsigned long long)m.readbacks);J(s,"switches","%llu",(unsigned long long)m.switches);JClose(s);
    XrPose head{};const bool haveHead=CameraHook::CopyHeadPoseR112(head);
    JOpen(s,"hmd");J(s,"valid","%s",haveHead?"true":"false");
    float hp[3]{head.pos_x,head.pos_y,head.pos_z},hq[4]{head.qx,head.qy,head.qz,head.qw};JFloats(s,"position",hp,3);JFloats(s,"quaternion",hq,4);
    J(s,"afrEyeOffsetM","%.9g",double(CameraHook::GetAfrEyeOffsetMeters()));J(s,"afrEye","%d",CameraHook::GetAfrEye());
    J(s,"lastPoseFrame","%llu",(unsigned long long)CameraHook::LastPoseFrameIndex());JClose(s);
    LARGE_INTEGER qpc{};QueryPerformanceCounter(&qpc);J(s,"qpc","%lld",(long long)qpc.QuadPart);
    J(s,"tickMs","%llu",(unsigned long long)GetTickCount64());
    return s;
}
std::string AerGlobalJson() noexcept {
    std::string s;
    J(s,"motionMode","%d",g_aer_motion_mode.load(std::memory_order_relaxed));
    const auto& st=g_aer_stats;
    JOpen(s,"stats");
    J(s,"evaluations","%llu",(unsigned long long)st.evaluations);J(s,"reprojected","%llu",(unsigned long long)st.reprojected);
    J(s,"resets","%llu",(unsigned long long)st.resets);J(s,"nativeResets","%llu",(unsigned long long)st.nativeResets);
    J(s,"cuts","%llu",(unsigned long long)st.cuts);J(s,"duplicates","%llu",(unsigned long long)st.duplicates);
    J(s,"noHistory","%llu",(unsigned long long)st.noHistory);J(s,"modelPending","%llu",(unsigned long long)st.modelPending);
    J(s,"passThrough","%llu",(unsigned long long)st.passThrough);JStr(s,"lastPassThrough",st.lastPassThrough);
    J(s,"cameraFailures","%llu",(unsigned long long)st.cameraFailures);JStr(s,"lastCameraFailure",st.lastCameraFailure);
    J(s,"nativeFailures","%llu",(unsigned long long)st.nativeFailures);J(s,"restoreFailures","%llu",(unsigned long long)st.restoreFailures);
    JClose(s);
    std::uint64_t id[6]{};FrameSync::AerIdentityStats(id);
    JOpen(s,"identityStats");J(s,"exact","%llu",(unsigned long long)id[0]);J(s,"parity","%llu",(unsigned long long)id[1]);
    J(s,"mainThreadFallback","%llu",(unsigned long long)id[2]);J(s,"mainEyeDisagreements","%llu",(unsigned long long)id[3]);
    J(s,"ambiguous","%llu",(unsigned long long)id[4]);J(s,"publications","%llu",(unsigned long long)id[5]);JClose(s);
    for(unsigned eye=0;eye<2;++eye){
        // The last 64 live jitter offsets of each eye, oldest first: AFR halves
        // the game's sequence per eye; a one-sided subsequence shows up here.
        float seq[128]{};const unsigned n=std::min(st.jitterCount[eye],64u);
        for(unsigned i=0;i<n;++i){const unsigned at=(st.jitterCount[eye]-n+i)%64;seq[i*2]=st.jitter[eye][at][0];seq[i*2+1]=st.jitter[eye][at][1];}
        JFloats(s,eye?"jitterHistoryRight":"jitterHistoryLeft",seq,n*2);
    }
    const auto& m=g_aer_model;
    JOpen(s,"model");J(s,"valid","%s",m.valid?"true":"false");J(s,"depthMode","%d",m.depthMode);JStr(s,"depthModeName",AerDepthName(m.depthMode));
    JFloats(s,"scores",m.score,4);JFloats(s,"precise",m.precise,4);J(s,"readbacks","%llu",(unsigned long long)m.readbacks);J(s,"switches","%llu",(unsigned long long)m.switches);
    J(s,"validations","%llu",(unsigned long long)m.validations);J(s,"invalidations","%llu",(unsigned long long)m.invalidations);
    double bins[32]{};for(int k=0;k<4;++k)for(int b=0;b<8;++b)bins[k*8+b]=m.counts[k][b];
    JDoubles(s,"residualHistogram",bins,32);JClose(s);
    JStr(s,"gpu",g_aer_gpu.broken?g_aer_gpu.brokenReason:"ok");
    // Recent render identities (oldest first): eye actually rendered vs the
    // main thread's eye at that moment, and how the eye was established.
    AerRenderIdentity history[48]{};const unsigned n=FrameSync::AerIdentityHistory(history,48);
    if(!s.empty())s+=',';
    s+="\"identityHistory\":[";
    for(unsigned i=0;i<n;++i){
        const auto& h=history[i];char row[256]{};
        std::snprintf(row,sizeof(row),"%s{\"renderFrame\":%u,\"eye\":%d,\"mainEye\":%d,\"source\":%d,\"publishSerial\":%llu,\"mainSerial\":%llu,\"poseFrame\":%llu,\"matchPosM\":%.6g}",
            i?",":"",h.renderFrame,h.eye,h.mainEye,h.source,(unsigned long long)h.publishSerial,(unsigned long long)h.mainSerial,
            (unsigned long long)h.poseFrame,std::isfinite(h.matchPosM)?double(h.matchPosM):-1.0);
        s+=row;
    }
    s+=']';
    return s;
}

// ---------------------------------------------------------------- GPU readback
bool AerDumpReadback(ID3D12Device* device,const D3D12_RESOURCE_DESC& source,DXGI_FORMAT format,AerDumpTexture& t) noexcept {
    D3D12_RESOURCE_DESC desc=source;desc.MipLevels=1;desc.DepthOrArraySize=1;desc.Format=format;
    device->GetCopyableFootprints(&desc,0,1,0,&t.footprint,&t.rows,&t.rowBytes,&t.total);
    if(!t.total||!t.rows)return false;
    auto heap=AerHeap(D3D12_HEAP_TYPE_READBACK);auto buffer=AerBufferDesc(t.total);
    if(FAILED(device->CreateCommittedResource(&heap,D3D12_HEAP_FLAG_NONE,&buffer,D3D12_RESOURCE_STATE_COPY_DEST,nullptr,IID_PPV_ARGS(&t.readback))))return false;
    t.width=static_cast<UINT>(desc.Width);t.height=desc.Height;t.format=format;
    return true;
}
void AerDumpRecordCopy(ID3D12GraphicsCommandList* list,ID3D12Resource* source,D3D12_RESOURCE_STATES state,AerDumpTexture& t) noexcept {
    AerBarrier(list,source,state,D3D12_RESOURCE_STATE_COPY_SOURCE);
    D3D12_TEXTURE_COPY_LOCATION dst{};dst.pResource=t.readback.Get();dst.Type=D3D12_TEXTURE_COPY_TYPE_PLACED_FOOTPRINT;dst.PlacedFootprint=t.footprint;
    D3D12_TEXTURE_COPY_LOCATION src{};src.pResource=source;src.Type=D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX;src.SubresourceIndex=0;
    list->CopyTextureRegion(&dst,0,0,0,&src,nullptr);
    AerBarrier(list,source,D3D12_RESOURCE_STATE_COPY_SOURCE,state);
}
// Direct copy of a single-plane resource in a known state.
bool AerDumpDirect(ID3D12Device* device,ID3D12GraphicsCommandList* list,ID3D12Resource* source,D3D12_RESOURCE_STATES state,
                   const char* name,const char* stateName,std::vector<AerDumpTexture>& out) noexcept {
    if(!source)return false;
    const auto desc=source->GetDesc();
    if(desc.Dimension!=D3D12_RESOURCE_DIMENSION_TEXTURE2D||desc.SampleDesc.Count!=1)return false;
    AerDumpTexture t{};t.name=name;t.sourceFormat=desc.Format;t.state=stateName;
    if(!AerDumpReadback(device,desc,desc.Format,t))return false;
    AerDumpRecordCopy(list,source,state,t);
    out.push_back(std::move(t));return true;
}
// Game DLSS inputs are shader-readable at the NGX boundary but their exact
// state flags are the game's. Read them through an SRV into our own texture
// (never transitioning the game resource), then copy ours to a readback.
bool AerDumpViaCompute(ID3D12Device* device,ID3D12GraphicsCommandList* list,ID3D12Resource* source,unsigned kind,
                       const char* name,std::vector<AerDumpTexture>& out) noexcept {
    if(!source||!AerGpuInit(device))return false;
    auto& g=g_aer_gpu;auto& d=g_dump;
    const auto desc=source->GetDesc();
    if(desc.Dimension!=D3D12_RESOURCE_DIMENSION_TEXTURE2D||desc.SampleDesc.Count!=1||desc.Width>16384||desc.Height>16384)return false;
    const DXGI_FORMAT view=AerViewFormat(desc.Format);
    const DXGI_FORMAT scratchFormat=kind==1?DXGI_FORMAT_R32_FLOAT:kind==2?(view==DXGI_FORMAT_R32G32_FLOAT?DXGI_FORMAT_R32G32_FLOAT:DXGI_FORMAT_R16G16_FLOAT):
        (view==DXGI_FORMAT_R32G32B32A32_FLOAT?DXGI_FORMAT_R32G32B32A32_FLOAT:DXGI_FORMAT_R16G16B16A16_FLOAT);
    auto& scratch=d.scratch[kind];auto& sd=d.scratchDesc[kind];
    if(!scratch||sd.Width!=desc.Width||sd.Height!=desc.Height||sd.Format!=scratchFormat){
        if(scratch)d.neverRelease.push_back(scratch); // may still be referenced by an earlier dump list
        scratch.Reset();
        D3D12_RESOURCE_DESC s{};s.Dimension=D3D12_RESOURCE_DIMENSION_TEXTURE2D;s.Width=desc.Width;s.Height=desc.Height;s.DepthOrArraySize=1;
        s.MipLevels=1;s.Format=scratchFormat;s.SampleDesc.Count=1;s.Flags=D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;
        auto heap=AerHeap(D3D12_HEAP_TYPE_DEFAULT);
        if(FAILED(device->CreateCommittedResource(&heap,D3D12_HEAP_FLAG_NONE,&s,D3D12_RESOURCE_STATE_UNORDERED_ACCESS,nullptr,IID_PPV_ARGS(&scratch))))return false;
        sd=s;
    }
    AerDumpTexture t{};t.name=name;t.sourceFormat=desc.Format;t.state="SRV-COPY";
    if(!AerDumpReadback(device,sd,scratchFormat,t))return false;
    const AerSlot slot=AerNextSlot();
    AerSrv(source,view,AerCpu(slot,0));AerSrv(nullptr,DXGI_FORMAT_R32_FLOAT,AerCpu(slot,1));
    AerTextureUav(scratch.Get(),scratchFormat,AerCpu(slot,2));AerTextureUav(nullptr,DXGI_FORMAT_R32_FLOAT,AerCpu(slot,3));
    ID3D12DescriptorHeap* heaps[]{g.heap.Get()};list->SetDescriptorHeaps(1,heaps);
    list->SetComputeRootSignature(g.root.Get());
    list->SetPipelineState(g.copy[kind==1?0:kind==2?1:2].Get());
    const std::uint32_t size[4]{static_cast<std::uint32_t>(desc.Width),desc.Height,0,0};
    list->SetComputeRoot32BitConstants(0,4,size,0);
    list->SetComputeRootDescriptorTable(1,AerGpuHandle(slot,0));list->SetComputeRootDescriptorTable(2,AerGpuHandle(slot,2));
    list->Dispatch((static_cast<UINT>(desc.Width)+7)/8,(desc.Height+7)/8,1);
    AerDumpRecordCopy(list,scratch.Get(),D3D12_RESOURCE_STATE_UNORDERED_ACCESS,t);
    out.push_back(std::move(t));return true;
}

// ---------------------------------------------------------------- DLSS side
bool AerDumpArmedForDlss() noexcept {
    const int phase=g_dump.phase.load(std::memory_order_acquire);
    return phase==kAerDumpArmed||phase==kAerDumpCapturing;
}
void* AerDumpDlssBefore(ID3D12GraphicsCommandList* list,const AerFrameInfo& f,ID3D12Resource* corrected) noexcept {
    std::lock_guard<std::mutex> lock(g_dump.mutex);
    auto& d=g_dump;const auto now=static_cast<std::uint64_t>(GetTickCount64());
    if(d.phase.load()==kAerDumpArmed){
        if(now<d.startTick)return nullptr;
        d.phase.store(kAerDumpCapturing,std::memory_order_release);d.captureTick=now;
        d.global=AerGlobalJson();d.xrWanted.store(true,std::memory_order_release);
    }
    if(d.phase.load()!=kAerDumpCapturing||d.dlssDone)return nullptr;
    ID3D12Device* raw=nullptr;if(FAILED(list->GetDevice(IID_PPV_ARGS(&raw)))||!raw)return nullptr;
    ComPtr<ID3D12Device> device;device.Attach(raw);
    AerDumpRecord rec{};rec.kind="dlss";rec.index=d.dlssCaptured;rec.eye=f.eye;
    (void)AerDumpViaCompute(device.Get(),list,f.in.color,0,"color_in",rec.textures);
    (void)AerDumpViaCompute(device.Get(),list,f.in.depth,1,"depth",rec.textures);
    (void)AerDumpViaCompute(device.Get(),list,f.in.motion,2,"mv_game",rec.textures);
    // The vectors DLSS receives this frame: our texture (known state) or the game's.
    if(corrected)(void)AerDumpDirect(device.Get(),list,corrected,D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE,"mv_dlss","NON_PIXEL_SHADER_RESOURCE",rec.textures);
    rec.json=AerFrameJson(f);
    d.records.push_back(std::move(rec));
    return reinterpret_cast<void*>(static_cast<std::uintptr_t>(d.records.size()));
}
void AerDumpDlssAfter(void* token,ID3D12GraphicsCommandList* list,const AerFrameInfo& f,Result result) noexcept {
    std::lock_guard<std::mutex> lock(g_dump.mutex);
    auto& d=g_dump;const auto index=reinterpret_cast<std::uintptr_t>(token);
    if(!index||index>d.records.size())return;
    auto& rec=d.records[index-1];
    ID3D12Device* raw=nullptr;
    if(SUCCEEDED(list->GetDevice(IID_PPV_ARGS(&raw)))&&raw){
        ComPtr<ID3D12Device> device;device.Attach(raw);
        // NGX requires and leaves the output in UNORDERED_ACCESS.
        if(Success(result))(void)AerDumpDirect(device.Get(),list,f.in.output,D3D12_RESOURCE_STATE_UNORDERED_ACCESS,"dlss_output","UNORDERED_ACCESS",rec.textures);
    }
    J(rec.json,"ngxResult","\"0x%08X\"",static_cast<unsigned>(result));
    if(++d.dlssCaptured>=d.framesWanted){d.dlssDone=true;d.dlssDoneTick=GetTickCount64();}
}

// ---------------------------------------------------------------- files
bool AerModuleDirectory(std::wstring& out) noexcept {
    HMODULE self=nullptr;
    if(!GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS|GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                           reinterpret_cast<LPCWSTR>(&AerModuleDirectory),&self)||!self)return false;
    wchar_t path[MAX_PATH*2]{};const DWORD n=GetModuleFileNameW(self,path,static_cast<DWORD>(std::size(path)));
    if(!n||n>=std::size(path))return false;
    std::wstring p(path,n);const auto slash=p.find_last_of(L"\\/");if(slash==std::wstring::npos)return false;
    out=p.substr(0,slash+1);return true;
}
bool AerWriteDds(const std::wstring& path,const AerDumpTexture& t,std::uint64_t& bytes) noexcept {
    void* mapped=nullptr;const D3D12_RANGE readRange{0,static_cast<SIZE_T>(t.total)};
    if(!t.readback||FAILED(t.readback->Map(0,&readRange,&mapped))||!mapped)return false;
    FILE* file=nullptr;
    if(_wfopen_s(&file,path.c_str(),L"wb")||!file){const D3D12_RANGE none{0,0};t.readback->Unmap(0,&none);return false;}
    #pragma pack(push,1)
    struct Header {std::uint32_t magic,size,flags,height,width,pitch,depth,mips,reserved1[11];
        std::uint32_t pfSize,pfFlags,fourCC,rgbBits,r,g,b,a,caps,caps2,caps3,caps4,reserved2;
        std::uint32_t dxgi,dimension,misc,arraySize,misc2;} h{};
    #pragma pack(pop)
    h.magic=0x20534444;h.size=124;h.flags=0x1|0x2|0x4|0x8|0x1000;h.height=t.height;h.width=t.width;
    h.pitch=static_cast<std::uint32_t>(t.rowBytes);h.mips=1;h.pfSize=32;h.pfFlags=0x4;h.fourCC=0x30315844;h.caps=0x1000;
    h.dxgi=static_cast<std::uint32_t>(AerViewFormat(t.format));h.dimension=3;h.arraySize=1;
    bool ok=std::fwrite(&h,sizeof(h),1,file)==1;
    const auto* base=static_cast<const unsigned char*>(mapped)+t.footprint.Offset;
    for(UINT row=0;ok&&row<t.rows;++row)
        ok=std::fwrite(base+static_cast<std::size_t>(row)*t.footprint.Footprint.RowPitch,1,static_cast<std::size_t>(t.rowBytes),file)==t.rowBytes;
    ok=std::fclose(file)==0&&ok;
    const D3D12_RANGE none{0,0};t.readback->Unmap(0,&none);
    if(ok)bytes+=sizeof(h)+t.rowBytes*t.rows;
    return ok;
}
std::wstring AerWide(const std::string& s){return std::wstring(s.begin(),s.end());}
void AerWriterBody(std::vector<AerDumpRecord>& records,const std::string& global,ComPtr<ID3D12Fence> fence,std::uint64_t fenceValue,
                   unsigned framesWanted,std::uint64_t requestTick);
void AerWriterThread(std::vector<AerDumpRecord> records,std::string global,ComPtr<ID3D12Fence> fence,std::uint64_t fenceValue,
                     unsigned framesWanted,std::uint64_t requestTick) noexcept {
    try{AerWriterBody(records,global,std::move(fence),fenceValue,framesWanted,requestTick);}
    catch(...){
        diagnostic_trace::WriteStatus("R149 AER_DUMP aborted reason=WRITER_EXCEPTION");
        g_dump.phase.store(kAerDumpIdle,std::memory_order_release);
    }
}
void AerWriterBody(std::vector<AerDumpRecord>& records,const std::string& global,ComPtr<ID3D12Fence> fence,std::uint64_t fenceValue,
                   unsigned framesWanted,std::uint64_t requestTick) {
    bool gpuDone=false;
    if(fence){
        HANDLE event=CreateEventW(nullptr,FALSE,FALSE,nullptr);
        if(event){
            if(fence->GetCompletedValue()>=fenceValue)gpuDone=true;
            else if(SUCCEEDED(fence->SetEventOnCompletion(fenceValue,event)))gpuDone=WaitForSingleObject(event,15000)==WAIT_OBJECT_0;
            CloseHandle(event);
        }
    }
    if(!gpuDone){
        diagnostic_trace::WriteStatus("R149 AER_DUMP aborted reason=GPU_FENCE_TIMEOUT readbacks=RETAINED");
        static std::vector<AerDumpRecord> retained;retained.insert(retained.end(),std::make_move_iterator(records.begin()),std::make_move_iterator(records.end()));
        g_dump.phase.store(kAerDumpIdle,std::memory_order_release);MessageBeep(MB_ICONHAND);return;
    }
    std::wstring dir;
    if(!AerModuleDirectory(dir)){diagnostic_trace::WriteStatus("R149 AER_DUMP aborted reason=MODULE_PATH");g_dump.phase.store(kAerDumpIdle);return;}
    SYSTEMTIME t{};GetLocalTime(&t);
    wchar_t stamp[64]{};std::swprintf(stamp,64,L"%04u%02u%02u_%02u%02u%02u",t.wYear,t.wMonth,t.wDay,t.wHour,t.wMinute,t.wSecond);
    const std::wstring root=dir+L"KCD2VR_AER_DUMP",folder=root+L"\\"+stamp;
    CreateDirectoryW(root.c_str(),nullptr);CreateDirectoryW(folder.c_str(),nullptr);
    std::uint64_t bytes=0;unsigned files=0,failures=0;
    std::string dlss,xr;
    for(auto& rec:records){
        std::string textures;
        for(auto& tex:rec.textures){
            char file[160]{};
            if(rec.kind=="dlss")std::snprintf(file,sizeof(file),"dlss%u_%s_%s.dds",rec.index,rec.eye?"R":"L",tex.name.c_str());
            else std::snprintf(file,sizeof(file),"xr%u_%s.dds",rec.index,tex.name.c_str());
            const bool ok=AerWriteDds(folder+L"\\"+AerWide(file),tex,bytes);
            ok?++files:++failures;
            std::string one;one+='{';
            JStr(one,"name",tex.name.c_str());JStr(one,"file",file);J(one,"written","%s",ok?"true":"false");
            J(one,"width","%u",tex.width);J(one,"height","%u",tex.height);
            J(one,"format","%u",unsigned(AerViewFormat(tex.format)));JStr(one,"formatName",AerFormatName(AerViewFormat(tex.format)));
            J(one,"sourceFormat","%u",unsigned(tex.sourceFormat));JStr(one,"sourceFormatName",AerFormatName(tex.sourceFormat));
            JStr(one,"captureState",tex.state.c_str());J(one,"rowBytes","%llu",(unsigned long long)tex.rowBytes);
            one+='}';
            if(!textures.empty())textures+=',';textures+=one;
            tex.readback.Reset();
        }
        std::string entry="{"+rec.json+",\"index\":"+std::to_string(rec.index)+",\"textures\":["+textures+"]}";
        std::string& list=rec.kind=="dlss"?dlss:xr;
        if(!list.empty())list+=",\n";list+=entry;
    }
    std::string json="{\n\"dump\":\"KCD2VR alternate-eye DLSS frame dump\",\n";
    json+="\"build\":\"" KCD2VR_BUILD_MARKER "\",\n";
    char header[256]{};std::snprintf(header,sizeof(header),"\"created\":\"%04u-%02u-%02u %02u:%02u:%02u\",\"framesRequested\":%u,\"requestTickMs\":%llu,\n",
        t.wYear,t.wMonth,t.wDay,t.wHour,t.wMinute,t.wSecond,framesWanted,(unsigned long long)requestTick);
    json+=header;
    json+="\"global\":{"+global+"},\n\"dlss\":[\n"+dlss+"\n],\n\"xr\":[\n"+xr+"\n]\n}\n";
    FILE* out=nullptr;
    if(!_wfopen_s(&out,(folder+L"\\dump.json").c_str(),L"wb")&&out){std::fwrite(json.data(),1,json.size(),out);std::fclose(out);++files;}
    static const char readme[]=
        "KCD2VR alternate-eye (AER) DLSS frame dump\r\n"
        "==========================================\r\n"
        "dump.json      all metadata: per live eye frame (dlss[]) and per XR composition (xr[]).\r\n"
        "dlssN_E_*.dds  live eye frame N (E = L/R) at the NGX boundary:\r\n"
        "  color_in     DLSS colour input (jittered, render resolution, HDR)\r\n"
        "  depth        DLSS depth input (raw values, see global.model.depthModeName)\r\n"
        "  mv_game      the game's motion vectors (frame N -> N-1, i.e. towards the OTHER eye)\r\n"
        "  mv_dlss      the vectors DLSS received (same eye, N -> N-2); absent when DLSS was reset\r\n"
        "  dlss_output  DLSS output (post-DLSS, pre-UI/post-processing)\r\n"
        "xrN_*.dds      XR composition of each blit after the capture started:\r\n"
        "  present_source  final game image cached for the rendered eye (new frames only)\r\n"
        "  xr_eye0/1       the two submitted eye images (fresh eye + warped stale eye)\r\n"
        "Cameras: cameraCurrent/cameraPrevious are the CRenderView CURRENT/PREVIOUS banks\r\n"
        "(Matrix34 rows, columns right/forward/up; rawHex is the full 0x310-byte CCamera),\r\n"
        "cameraHistory is the same eye's previous live camera used for N -> N-2.\r\n"
        "Analyse with: python tools\\analyze_aer_dump_r149.py <this folder>\r\n";
    if(!_wfopen_s(&out,(folder+L"\\README.txt").c_str(),L"wb")&&out){std::fwrite(readme,1,sizeof(readme)-1,out);std::fclose(out);++files;}
    char line[600]{};
    std::snprintf(line,sizeof(line),"R149 AER_DUMP written folder=%ls files=%u failedTextures=%u textureBytes=%llu records=%zu",
        folder.c_str(),files,failures,(unsigned long long)bytes,records.size());
    diagnostic_trace::WriteStatus(line);
    records.clear();
    g_dump.phase.store(kAerDumpIdle,std::memory_order_release);
    MessageBeep(failures?MB_ICONHAND:MB_OK);
}

bool AerDumpRequestInternal(unsigned frames,unsigned delayMs) noexcept {
    std::lock_guard<std::mutex> lock(g_dump.mutex);
    auto& d=g_dump;
    if(d.phase.load()!=kAerDumpIdle)return false;
    d.records.clear();d.global.clear();d.dlssCaptured=d.xrBlits=d.xrNewFrames=d.presentTicksAfterDone=0;d.dlssDone=false;
    d.framesWanted=std::clamp(frames?frames:4u,2u,8u);d.xrMaxBlits=d.framesWanted*2;
    d.requestTick=GetTickCount64();d.startTick=d.requestTick+std::min(delayMs,10000u);d.captureTick=d.dlssDoneTick=0;
    ++d.session;d.xrWanted.store(false,std::memory_order_release);
    d.phase.store(kAerDumpArmed,std::memory_order_release);
    char line[200]{};std::snprintf(line,sizeof(line),"R149 AER_DUMP armed frames=%u delayMs=%u session=%u",d.framesWanted,delayMs,d.session);
    diagnostic_trace::WriteStatus(line);
    return true;
}
void AerDumpRenderTickInternal(ID3D12CommandQueue* queue,bool presentSubmitted) noexcept {
    auto& d=g_dump;
    const int phase=d.phase.load(std::memory_order_acquire);
    if(phase!=kAerDumpArmed&&phase!=kAerDumpCapturing)return;
    std::lock_guard<std::mutex> lock(d.mutex);
    const auto now=static_cast<std::uint64_t>(GetTickCount64());
    if(d.phase.load()==kAerDumpArmed){
        if(now>=d.startTick+1500){
            // No alternate-eye DLSS frame reached NGX. Still capture the XR
            // composition plus the AER/identity state, so the dump shows why.
            // Counters are read without the NGX registry mutex on purpose: this
            // is the queue path, which must never wait on an SDK call.
            d.phase.store(kAerDumpCapturing,std::memory_order_release);d.captureTick=now;
            d.global=AerGlobalJson();
            d.global+=",\"dlssCapture\":\"none: no alternate-eye DLSS evaluation reached NGX within 1.5 s of the trigger\"";
            d.dlssDone=true;d.dlssDoneTick=now;d.xrWanted.store(true,std::memory_order_release);
            diagnostic_trace::WriteStatus("R150 AER_DUMP no alternate-eye DLSS frame within 1.5 s: capturing XR composition and AER state only");
        }
        return;
    }
    if(!d.dlssDone){
        if(d.captureTick&&now>d.captureTick+10000){d.dlssDone=true;d.dlssDoneTick=now;} // write what was captured
        else return;
    }
    const bool xrDone=!d.xrWanted.load()||now>d.dlssDoneTick+3000;
    if(!xrDone||!presentSubmitted||!queue)return;
    // Two PRESENT batches after the last capture: every list that carried a
    // capture copy has been submitted to this queue before the fence signal.
    if(++d.presentTicksAfterDone<2)return;
    d.xrWanted.store(false,std::memory_order_release);
    ID3D12Device* raw=nullptr;
    if(FAILED(queue->GetDevice(IID_PPV_ARGS(&raw)))||!raw)return;
    ComPtr<ID3D12Device> device;device.Attach(raw);
    if(!d.fence&&FAILED(device->CreateFence(0,D3D12_FENCE_FLAG_NONE,IID_PPV_ARGS(&d.fence))))return;
    const auto value=++d.fenceValue;
    if(FAILED(queue->Signal(d.fence.Get(),value)))return;
    d.phase.store(kAerDumpFlushing,std::memory_order_release);
    std::vector<AerDumpRecord> records;records.swap(d.records);
    try{
        std::thread(AerWriterThread,std::move(records),d.global,d.fence,value,d.framesWanted,d.requestTick).detach();
    }catch(...){
        d.phase.store(kAerDumpIdle,std::memory_order_release);
        diagnostic_trace::WriteStatus("R149 AER_DUMP aborted reason=WRITER_THREAD");
    }
}
