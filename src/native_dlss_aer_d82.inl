// R149 alternate-eye (AFR) DLSS with same-eye motion vectors.
// Included inside namespace kcd2vr::native_dlss_stereo_d82 { namespace { after
// native_dlss_stereo_d82_runtime.inl. Host routing fixtures stub
// EvaluateAlternateEye and never compile this file.
//
// Each AFR game frame renders ONE eye. The paired NGX handles keep two histories:
// the game handle only ever sees LEFT live frames, the twin only RIGHT live frames.
// The game's motion vectors map frame N to N-1, which was the OTHER eye (IPD
// baseline). DLSS history of this eye is frame N-2. Earlier drafts composed
// N->N-1 with the other eye's N-1->N-2 field; wherever a surface was hidden in
// the other eye that lookup read the occluder's vector, which is exactly the
// shimmer on "the occluded parts of the other eye". This path instead rebuilds
// N->N-2 per pixel from depth and the exact CRenderView camera banks:
//   out = gameMV(N->N-1) + [project(X, cam N-2 same eye) - project(X, cam N-1)]
//         + gated extrapolation of the object motion the game encoded.
// The stale eye never enters NGX; XR composition warps its post-DLSS image.

using Microsoft::WRL::ComPtr;

// ---------------------------------------------------------------- native layout
// Exact retail WHGame (timestamp 0x6A350E20, verified by Install). Offsets are
// the same ones the native stereo modules read (stereo_preflight_model_v6602.h).
constexpr std::uintptr_t kAerPipelineVtableRva=0x3DFEBD0; // CStandardGraphicsPipeline
constexpr std::uintptr_t kAerRenderViewVtableRva=0x3DF25C0; // CRenderView
constexpr std::uintptr_t kAerPipelineRenderView=0x1978;
constexpr std::uintptr_t kAerCurrentBank=0x8EE8;  // CURRENT camera, physical eye 0
constexpr std::uintptr_t kAerPreviousBank=0x9508; // PREVIOUS camera, physical eye 0
constexpr std::size_t kAerCameraBytes=0x310;
// g_wh_base (runtime.inl) is the verified WHGame module recorded by Install().

struct AerCamera {
    double rot[9]{};   // Matrix34 rotation rows; columns are right/forward/up
    double pos[3]{};
    float fov{},nearZ{},farZ{},tanX{},tanY{},projRatio{},pixelAspect{};
    float nlt[3]{},plt[3]{},flt[3]{};
    std::int32_t width{},height{};
    std::uint32_t eyeTag{};
    bool valid{};
    const char* failure{"unread"};
    unsigned char raw[kAerCameraBytes]{};
};

bool AerSafeCopy(void* dst,std::uintptr_t src,std::size_t bytes) noexcept {
    if(!dst||!src)return false;
    __try {std::memcpy(dst,reinterpret_cast<const void*>(src),bytes);return true;}
    __except(EXCEPTION_EXECUTE_HANDLER) {return false;}
}

// CCamera layout as written by CryEngine SetFrustum (KCD2Tools' coupled fields
// +0x50/+0x58/+0x60/+0x68/+0x70 are exactly nlt.x/nlt.z/plt.y/flt.x/flt.z):
// +0x00 Matrix34, +0x30 fov, +0x34 width, +0x38 height, +0x3C projection ratio,
// +0x40 pixel aspect, +0x50 near-left-top edge, +0x5C projection-plane edge,
// +0x68 far-left-top edge. The frustum tangents are read from the edges and must
// agree with the fov, otherwise the layout assumption is rejected.
bool DecodeAerCamera(AerCamera& c) noexcept {
    c.valid=false;
    float f[12]{};std::memcpy(f,c.raw,sizeof(f));
    for(float v:f)if(!std::isfinite(v)){c.failure="matrix-nonfinite";return false;}
    for(int r=0;r<3;++r){for(int k=0;k<3;++k)c.rot[r*3+k]=f[r*4+k];c.pos[r]=f[r*4+3];}
    for(int k=0;k<3;++k){
        const double n=c.rot[k]*c.rot[k]+c.rot[3+k]*c.rot[3+k]+c.rot[6+k]*c.rot[6+k];
        if(!(n>0.96&&n<1.04)){c.failure="matrix-scale";return false;}
    }
    for(int a=0;a<3;++a)for(int b=a+1;b<3;++b){
        const double d=c.rot[a]*c.rot[b]+c.rot[3+a]*c.rot[3+b]+c.rot[6+a]*c.rot[6+b];
        if(!(std::fabs(d)<0.03)){c.failure="matrix-shear";return false;}
    }
    std::memcpy(&c.fov,c.raw+0x30,4);std::memcpy(&c.width,c.raw+0x34,4);std::memcpy(&c.height,c.raw+0x38,4);
    std::memcpy(&c.projRatio,c.raw+0x3C,4);std::memcpy(&c.pixelAspect,c.raw+0x40,4);
    std::memcpy(c.nlt,c.raw+0x50,12);std::memcpy(c.plt,c.raw+0x5C,12);std::memcpy(c.flt,c.raw+0x68,12);
    std::memcpy(&c.eyeTag,c.raw+0x20C,4);
    if(!(c.fov>0.35f&&c.fov<2.95f)){c.failure="fov";return false;}
    c.nearZ=c.nlt[1];c.farZ=c.flt[1];
    if(!(c.nearZ>1.0e-4f&&c.nearZ<10.0f)){c.failure="near-plane";return false;}
    if(!(c.farZ>c.nearZ*4.0f&&c.farZ<1.0e7f)){c.failure="far-plane";return false;}
    c.tanY=c.nlt[2]/c.nearZ;c.tanX=std::fabs(c.nlt[0])/c.nearZ;
    const float expected=std::tan(c.fov*0.5f);
    if(!(std::fabs(c.tanY-expected)<=0.02f*expected+1.0e-4f)){c.failure="frustum-layout";return false;}
    if(!(std::fabs(c.flt[2]/c.farZ-c.tanY)<=0.02f*c.tanY+1.0e-4f)){c.failure="far-frustum";return false;}
    if(!(c.tanX>0.05f&&c.tanX<20.0f&&c.tanX/c.tanY>0.2f&&c.tanX/c.tanY<5.0f)){c.failure="aspect";return false;}
    c.valid=true;c.failure="ok";return true;
}

bool ReadAerCameras(std::uintptr_t pipeline,AerCamera& cur,AerCamera& prev,std::uintptr_t& crvOut,
                    std::uint32_t& renderFrame,const char*& failure) noexcept {
    crvOut=0;renderFrame=0;
    if(!g_wh_base||!pipeline){failure="no-module";return false;}
    std::uintptr_t vt=0,crv=0,cvt=0;std::uint32_t viewType=~0u,selector=~0u;
    if(!AerSafeCopy(&vt,pipeline,sizeof(vt))||vt!=g_wh_base+kAerPipelineVtableRva){failure="pipeline-vtable";return false;}
    if(!AerSafeCopy(&crv,pipeline+kAerPipelineRenderView,sizeof(crv))||!crv||
       !AerSafeCopy(&cvt,crv,sizeof(cvt))||cvt!=g_wh_base+kAerRenderViewVtableRva){failure="renderview-vtable";return false;}
    if(!AerSafeCopy(&viewType,crv+0x14,4)||viewType!=0){failure="renderview-type";return false;}
    if(!AerSafeCopy(&selector,crv+0x8EE0,4)||selector!=0){failure="selector";return false;}
    (void)AerSafeCopy(&renderFrame,crv+0x2C,4);
    if(!AerSafeCopy(cur.raw,crv+kAerCurrentBank,kAerCameraBytes)){failure="current-bank";return false;}
    if(!AerSafeCopy(prev.raw,crv+kAerPreviousBank,kAerCameraBytes)){failure="previous-bank";return false;}
    crvOut=crv;
    if(!DecodeAerCamera(cur)){failure=cur.failure;return false;}
    if(!DecodeAerCamera(prev)){failure=prev.failure;return false;}
    failure="ok";return true;
}

// rows = [R_K^T R_N | R_K^T (C_N - C_K)]: current-camera local point -> camera K local.
void AerRelativeRows(const double* rotN,const double* posN,const double* rotK,const double* posK,float rows[12]) noexcept {
    const double d[3]{posN[0]-posK[0],posN[1]-posK[1],posN[2]-posK[2]};
    for(int i=0;i<3;++i){
        for(int j=0;j<3;++j){
            double a=0.0;for(int k=0;k<3;++k)a+=rotK[k*3+i]*rotN[k*3+j];
            rows[i*4+j]=static_cast<float>(a);
        }
        double b=0.0;for(int k=0;k<3;++k)b+=rotK[k*3+i]*d[k];
        rows[i*4+3]=static_cast<float>(b);
    }
}
double AerAngleDeg(const double* a,const double* b) noexcept {
    double trace=0.0;for(int k=0;k<3;++k)for(int r=0;r<3;++r)trace+=a[r*3+k]*b[r*3+k];
    return std::acos(std::clamp((trace-1.0)*0.5,-1.0,1.0))*57.29577951308232;
}

// ---------------------------------------------------------------- inputs
struct AerInputs {
    ID3D12Resource* color{};ID3D12Resource* depth{};ID3D12Resource* motion{};ID3D12Resource* output{};
    D3D12_RESOURCE_DESC colorDesc{},depthDesc{},motionDesc{},outputDesc{};
    float jitterX{},jitterY{},mvScaleX{1.0f},mvScaleY{1.0f},preExposure{-1.0f},exposureScale{-1.0f},sharpness{-1.0f};
    unsigned subW{},subH{},colorX{},colorY{},depthX{},depthY{},mvX{},mvY{},outX{},outY{};
    unsigned width{},height{},outWidth{},outHeight{};
    unsigned renderW{},renderH{},mvW{},mvH{};
    int reset{};
    bool ok{};
};
unsigned AerGetU(void* parameters,const char* key,unsigned fallback) noexcept {
    unsigned value=0;
    if(g_get_ui&&Success(g_get_ui(parameters,key,&value)))return value;
    int signedValue=0;
    if(g_get_i&&Success(g_get_i(parameters,key,&signedValue))&&signedValue>=0)return static_cast<unsigned>(signedValue);
    return fallback;
}
float AerGetF(void* parameters,const char* key,float fallback) noexcept {
    float value=0.0f;
    return g_get_f&&Success(g_get_f(parameters,key,&value))&&std::isfinite(value)?value:fallback;
}
bool AerReadInputs(void* parameters,int createFlags,AerInputs& in) noexcept {
    in={};
    void* r[4]{};const char* keys[]{"Color","Depth","MotionVectors","Output"};
    for(unsigned i=0;i<4;++i)if(!Success(g_get_resource(parameters,keys[i],&r[i]))||!r[i])return false;
    in.color=static_cast<ID3D12Resource*>(r[0]);in.depth=static_cast<ID3D12Resource*>(r[1]);
    in.motion=static_cast<ID3D12Resource*>(r[2]);in.output=static_cast<ID3D12Resource*>(r[3]);
    in.colorDesc=in.color->GetDesc();in.depthDesc=in.depth->GetDesc();
    in.motionDesc=in.motion->GetDesc();in.outputDesc=in.output->GetDesc();
    in.jitterX=AerGetF(parameters,"Jitter.Offset.X",0.0f);in.jitterY=AerGetF(parameters,"Jitter.Offset.Y",0.0f);
    in.mvScaleX=AerGetF(parameters,"MV.Scale.X",1.0f);in.mvScaleY=AerGetF(parameters,"MV.Scale.Y",1.0f);
    in.preExposure=AerGetF(parameters,"DLSS.Pre.Exposure",-1.0f);in.exposureScale=AerGetF(parameters,"DLSS.Exposure.Scale",-1.0f);
    in.sharpness=AerGetF(parameters,"Sharpness",-1.0f);
    in.subW=AerGetU(parameters,"DLSS.Render.Subrect.Dimensions.Width",0);
    in.subH=AerGetU(parameters,"DLSS.Render.Subrect.Dimensions.Height",0);
    in.colorX=AerGetU(parameters,"DLSS.Input.Color.Subrect.Base.X",0);in.colorY=AerGetU(parameters,"DLSS.Input.Color.Subrect.Base.Y",0);
    in.depthX=AerGetU(parameters,"DLSS.Input.Depth.Subrect.Base.X",0);in.depthY=AerGetU(parameters,"DLSS.Input.Depth.Subrect.Base.Y",0);
    in.mvX=AerGetU(parameters,"DLSS.Input.MV.Subrect.Base.X",0);in.mvY=AerGetU(parameters,"DLSS.Input.MV.Subrect.Base.Y",0);
    in.outX=AerGetU(parameters,"DLSS.Output.Subrect.Base.X",0);in.outY=AerGetU(parameters,"DLSS.Output.Subrect.Base.Y",0);
    in.width=AerGetU(parameters,"Width",0);in.height=AerGetU(parameters,"Height",0);
    in.outWidth=AerGetU(parameters,"OutWidth",0);in.outHeight=AerGetU(parameters,"OutHeight",0);
    // DLSS reads the full colour texture unless a render subrect is supplied.
    in.renderW=in.subW?in.subW:static_cast<unsigned>(in.colorDesc.Width);
    in.renderH=in.subH?in.subH:in.colorDesc.Height;
    // MVLowRes (bit 1): vectors at render resolution; otherwise at output size.
    const bool lowRes=createFlags<0||(createFlags&(1<<1))!=0;
    in.mvW=lowRes?in.renderW:(in.outWidth?in.outWidth:static_cast<unsigned>(in.motionDesc.Width));
    in.mvH=lowRes?in.renderH:(in.outHeight?in.outHeight:in.motionDesc.Height);
    if(in.mvX+in.mvW>in.motionDesc.Width)in.mvW=static_cast<unsigned>(in.motionDesc.Width)-std::min<unsigned>(in.mvX,static_cast<unsigned>(in.motionDesc.Width));
    if(in.mvY+in.mvH>in.motionDesc.Height)in.mvH=in.motionDesc.Height-std::min<unsigned>(in.mvY,in.motionDesc.Height);
    in.ok=in.renderW>=16&&in.renderH>=16&&in.mvW>=16&&in.mvH>=16&&
        std::fabs(in.mvScaleX)>1.0e-6f&&std::fabs(in.mvScaleY)>1.0e-6f&&
        in.motionDesc.Dimension==D3D12_RESOURCE_DIMENSION_TEXTURE2D&&in.depthDesc.Dimension==D3D12_RESOURCE_DIMENSION_TEXTURE2D&&
        in.motionDesc.SampleDesc.Count==1&&in.depthDesc.SampleDesc.Count==1;
    return in.ok;
}

// ---------------------------------------------------------------- GPU
DXGI_FORMAT AerViewFormat(DXGI_FORMAT f) noexcept {
    switch(f){
    case DXGI_FORMAT_R32G32B32A32_TYPELESS:return DXGI_FORMAT_R32G32B32A32_FLOAT;
    case DXGI_FORMAT_R16G16B16A16_TYPELESS:return DXGI_FORMAT_R16G16B16A16_FLOAT;
    case DXGI_FORMAT_R32G32_TYPELESS:return DXGI_FORMAT_R32G32_FLOAT;
    case DXGI_FORMAT_R10G10B10A2_TYPELESS:return DXGI_FORMAT_R10G10B10A2_UNORM;
    case DXGI_FORMAT_R8G8B8A8_TYPELESS:return DXGI_FORMAT_R8G8B8A8_UNORM;
    case DXGI_FORMAT_B8G8R8A8_TYPELESS:return DXGI_FORMAT_B8G8R8A8_UNORM;
    case DXGI_FORMAT_B8G8R8X8_TYPELESS:return DXGI_FORMAT_B8G8R8X8_UNORM;
    case DXGI_FORMAT_R16G16_TYPELESS:return DXGI_FORMAT_R16G16_FLOAT;
    case DXGI_FORMAT_R32_TYPELESS:case DXGI_FORMAT_D32_FLOAT:return DXGI_FORMAT_R32_FLOAT;
    case DXGI_FORMAT_R24G8_TYPELESS:case DXGI_FORMAT_D24_UNORM_S8_UINT:return DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
    case DXGI_FORMAT_R32G8X24_TYPELESS:case DXGI_FORMAT_D32_FLOAT_S8X24_UINT:return DXGI_FORMAT_R32_FLOAT_X8X24_TYPELESS;
    case DXGI_FORMAT_R16_TYPELESS:case DXGI_FORMAT_D16_UNORM:return DXGI_FORMAT_R16_UNORM;
    default:return f;
    }
}
bool AerTwoChannelFloat(DXGI_FORMAT f) noexcept {return f==DXGI_FORMAT_R16G16_FLOAT||f==DXGI_FORMAT_R32G32_FLOAT;}

constexpr const char* kAerReprojectShader=R"(
cbuffer Params : register(b0) {
    uint4 G0;  // xy dispatch size (MV active rect), z reproject, w depth convention
    uint4 G1;  // xy depth subrect base, zw MV subrect base
    float4 F0; // xy MV scale (texel -> render px), zw current tanX/tanY
    float4 F1; // x near, y far, z object extrapolation gain, w stats enable
    float4 P0; float4 P1; float4 P2; // current camera -> PREVIOUS bank camera
    float4 S0; float4 S1; float4 S2; // current camera -> same-eye history camera
    float4 F2; // xy previous tanX/tanY, zw history tanX/tanY
    float4 F3; // xy history jitter term (render px), zw render size (px)
    float4 F4; // xy depth texels per dispatch texel, z extrapolation gate factor
};
Texture2D<float2> GameMV : register(t0);
Texture2D<float> Depth : register(t1);
RWTexture2D<float2> Output : register(u0);
RWStructuredBuffer<uint> Stats : register(u1);
float LinearZ(float d, uint mode) {
    // 0 reversed hyperbolic, 1 standard hyperbolic, 2 linear / far, 3 linear metres
    if (mode == 3) return max(d, F1.x);
    if (mode == 2) return max(d * F1.y, F1.x);
    const float dr = mode == 0 ? d : 1.0 - d;
    return F1.x * F1.y / max(F1.x + dr * (F1.y - F1.x), 1e-7);
}
float3 Xform(float4 a, float4 b, float4 c, float3 v) { return float3(dot(a.xyz, v) + a.w, dot(b.xyz, v) + b.w, dot(c.xyz, v) + c.w); }
float3 Rot(float4 a, float4 b, float4 c, float3 v) { return float3(dot(a.xyz, v), dot(b.xyz, v), dot(c.xyz, v)); }
float2 ProjectUv(float3 l, float2 t) {
    const float y = max(l.y, 1e-4);
    return float2(0.5 + 0.5 * l.x / (y * t.x), 0.5 - 0.5 * l.z / (y * t.y));
}
[numthreads(8, 8, 1)]
void main(uint3 id : SV_DispatchThreadID) {
    if (id.x >= G0.x || id.y >= G0.y) return;
    const int2 mvPixel = int2(id.xy) + int2(G1.zw);
    float2 mv = GameMV.Load(int3(mvPixel, 0));
    if (!all(isfinite(mv)) || any(abs(mv) > 60000.0)) mv = 0.0;
    float2 result = mv;
    if (G0.z != 0) {
        const float2 size = float2(G0.xy);
        const float2 uv = (float2(id.xy) + 0.5) / size;
        const float2 ndc = float2(uv.x * 2.0 - 1.0, 1.0 - uv.y * 2.0);
        const int2 depthPixel = int2(G1.xy) + int2((float2(id.xy) + 0.5) * F4.xy);
        const float d = Depth.Load(int3(depthPixel, 0));
        const float2 render = F3.zw;
        const float2 gamePx = mv * F0.xy;
        const bool sample = F1.w > 0.5 && ((id.x | id.y) & 3) == 0;
        float2 outPx = gamePx;
        [unroll] for (uint m = 0; m < 4; ++m) {
            if (m != G0.w && !sample) continue;
            const float z = LinearZ(d, m);
            const float3 l = float3(ndc.x * F0.z * z, z, ndc.y * F0.w * z);
            const float2 uvPrev = ProjectUv(Xform(P0, P1, P2, l), F2.xy);
            // What the game's vector would be if this point were static.
            const float2 objectPx = gamePx - (uvPrev - uv) * render;
            if (m == G0.w) {
                const float2 uvHist = ProjectUv(Xform(S0, S1, S2, l), F2.zw);
                const float2 uvInf = ProjectUv(Rot(P0, P1, P2, l), F2.xy);
                // Depth sensitivity of the static prediction: only motion clearly
                // larger than a depth-model error is extrapolated a second frame.
                const float gate = 1.0 + F4.z * length((uvPrev - uvInf) * render);
                const float w = saturate((length(objectPx) - gate) / gate);
                outPx = gamePx + (uvHist - uvPrev) * render + F3.xy + F1.z * w * objectPx;
            }
            if (sample) {
                const float r = length(objectPx);
                uint bin = r < 0.25 ? 0 : r < 0.5 ? 1 : r < 1.0 ? 2 : r < 2.0 ? 3 : r < 4.0 ? 4 : r < 8.0 ? 5 : r < 16.0 ? 6 : 7;
                if (!isfinite(r)) bin = 7;
                InterlockedAdd(Stats[m * 8 + bin], 1);
            }
        }
        result = outPx / F0.xy;
        if (!all(isfinite(result)) || any(abs(result) > 60000.0)) result = mv;
    }
    Output[mvPixel] = result;
}
)";

// Dump capture: copy an SRV-readable game resource into our own UAV texture
// without touching the game resource's state (it is already shader-readable for
// DLSS). CH selects the channel count of both views.
constexpr const char* kAerCopyShader=R"(
cbuffer Params : register(b0) { uint4 G0; };
#if CH == 1
Texture2D<float> Src : register(t0); RWTexture2D<float> Dst : register(u0);
#elif CH == 2
Texture2D<float2> Src : register(t0); RWTexture2D<float2> Dst : register(u0);
#else
Texture2D<float4> Src : register(t0); RWTexture2D<float4> Dst : register(u0);
#endif
[numthreads(8, 8, 1)]
void main(uint3 id : SV_DispatchThreadID) {
    if (id.x >= G0.x || id.y >= G0.y) return;
    Dst[id.xy] = Src.Load(int3(id.xy, 0));
}
)";

constexpr unsigned kAerDescriptorSlots=512; // 4 descriptors per slot
constexpr unsigned kAerStatsWords=32;
struct AerGpu {
    ComPtr<ID3D12Device> device;
    ComPtr<ID3D12RootSignature> root;
    ComPtr<ID3D12PipelineState> reproject,copy[3];
    ComPtr<ID3D12DescriptorHeap> heap;
    UINT increment{};unsigned cursor{};
    ComPtr<ID3D12Resource> output;D3D12_RESOURCE_DESC outputDesc{};bool outputUav{true};
    std::vector<std::pair<std::uint64_t,ComPtr<ID3D12Resource>>> retired;
    ComPtr<ID3D12Resource> stats,statsZero,statsReadback[4];
    const std::uint32_t* statsMapped[4]{};
    std::uint64_t statsTicket[4]{};
    std::uint64_t dispatches{};
    bool statsCopyDest{true};
    bool broken{};
    const char* brokenReason{""};
};
AerGpu g_aer_gpu{};

void AerGpuBreak(const char* reason) noexcept {
    if(!g_aer_gpu.broken){
        char line[200]{};std::snprintf(line,sizeof(line),"R149 AER_DLSS gpu=DISABLED reason=%s motion=RESET_EVERY_EYE_FRAME",reason?reason:"unknown");
        diagnostic_trace::WriteStatus(line);
    }
    g_aer_gpu.broken=true;g_aer_gpu.brokenReason=reason?reason:"unknown";
}
bool AerCompile(const char* source,const char* name,const D3D_SHADER_MACRO* macros,ComPtr<ID3DBlob>& code) noexcept {
    ComPtr<ID3DBlob> errors;
    const HRESULT hr=D3DCompile(source,std::strlen(source),name,macros,nullptr,"main","cs_5_0",
                                D3DCOMPILE_OPTIMIZATION_LEVEL3,0,&code,&errors);
    if(SUCCEEDED(hr)&&code)return true;
    char line[512]{};
    std::snprintf(line,sizeof(line),"R149 AER_DLSS shader=%s compile=FAILED hr=0x%08X %.360s",name,static_cast<unsigned>(hr),
        errors?static_cast<const char*>(errors->GetBufferPointer()):"");
    diagnostic_trace::WriteStatus(line);
    return false;
}
D3D12_HEAP_PROPERTIES AerHeap(D3D12_HEAP_TYPE type) noexcept {
    D3D12_HEAP_PROPERTIES heap{};heap.Type=type;heap.CPUPageProperty=D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
    heap.MemoryPoolPreference=D3D12_MEMORY_POOL_UNKNOWN;heap.CreationNodeMask=heap.VisibleNodeMask=1;return heap;
}
D3D12_RESOURCE_DESC AerBufferDesc(UINT64 bytes,D3D12_RESOURCE_FLAGS flags=D3D12_RESOURCE_FLAG_NONE) noexcept {
    D3D12_RESOURCE_DESC d{};d.Dimension=D3D12_RESOURCE_DIMENSION_BUFFER;d.Width=bytes;d.Height=1;d.DepthOrArraySize=1;
    d.MipLevels=1;d.Format=DXGI_FORMAT_UNKNOWN;d.SampleDesc.Count=1;d.Layout=D3D12_TEXTURE_LAYOUT_ROW_MAJOR;d.Flags=flags;return d;
}
bool AerGpuInit(ID3D12Device* device) noexcept {
    auto& g=g_aer_gpu;
    if(g.broken||!device)return false;
    if(g.device)return g.device.Get()==device;
    D3D12_DESCRIPTOR_RANGE ranges[2]{};
    ranges[0]={D3D12_DESCRIPTOR_RANGE_TYPE_SRV,2,0,0,0};
    ranges[1]={D3D12_DESCRIPTOR_RANGE_TYPE_UAV,2,0,0,0};
    D3D12_ROOT_PARAMETER params[3]{};
    params[0].ParameterType=D3D12_ROOT_PARAMETER_TYPE_32BIT_CONSTANTS;params[0].Constants={0,0,52};
    params[0].ShaderVisibility=D3D12_SHADER_VISIBILITY_ALL;
    params[1].ParameterType=D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;params[1].DescriptorTable={1,&ranges[0]};
    params[1].ShaderVisibility=D3D12_SHADER_VISIBILITY_ALL;
    params[2].ParameterType=D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;params[2].DescriptorTable={1,&ranges[1]};
    params[2].ShaderVisibility=D3D12_SHADER_VISIBILITY_ALL;
    const D3D12_ROOT_SIGNATURE_DESC rootDesc{3,params,0,nullptr,D3D12_ROOT_SIGNATURE_FLAG_NONE};
    ComPtr<ID3DBlob> rootBytes,errors,code;
    if(FAILED(D3D12SerializeRootSignature(&rootDesc,D3D_ROOT_SIGNATURE_VERSION_1,&rootBytes,&errors))||!rootBytes||
       FAILED(device->CreateRootSignature(0,rootBytes->GetBufferPointer(),rootBytes->GetBufferSize(),IID_PPV_ARGS(&g.root)))){
        AerGpuBreak("root-signature");return false;}
    D3D12_COMPUTE_PIPELINE_STATE_DESC pso{};pso.pRootSignature=g.root.Get();
    if(!AerCompile(kAerReprojectShader,"KCD2VR_R149_AER_REPROJECT",nullptr,code)){AerGpuBreak("reproject-shader");return false;}
    pso.CS={code->GetBufferPointer(),code->GetBufferSize()};
    if(FAILED(device->CreateComputePipelineState(&pso,IID_PPV_ARGS(&g.reproject)))){AerGpuBreak("reproject-pso");return false;}
    const char* channels[3]{"1","2","4"};
    for(int i=0;i<3;++i){
        const D3D_SHADER_MACRO macros[]{{"CH",channels[i]},{nullptr,nullptr}};
        ComPtr<ID3DBlob> copyCode;
        if(!AerCompile(kAerCopyShader,"KCD2VR_R149_AER_COPY",macros,copyCode)){AerGpuBreak("copy-shader");return false;}
        pso.CS={copyCode->GetBufferPointer(),copyCode->GetBufferSize()};
        if(FAILED(device->CreateComputePipelineState(&pso,IID_PPV_ARGS(&g.copy[i])))){AerGpuBreak("copy-pso");return false;}
    }
    D3D12_DESCRIPTOR_HEAP_DESC heapDesc{};heapDesc.Type=D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
    heapDesc.NumDescriptors=4*kAerDescriptorSlots;heapDesc.Flags=D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
    if(FAILED(device->CreateDescriptorHeap(&heapDesc,IID_PPV_ARGS(&g.heap)))){AerGpuBreak("descriptor-heap");return false;}
    g.increment=device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
    auto defaultHeap=AerHeap(D3D12_HEAP_TYPE_DEFAULT),uploadHeap=AerHeap(D3D12_HEAP_TYPE_UPLOAD),readHeap=AerHeap(D3D12_HEAP_TYPE_READBACK);
    auto statsDesc=AerBufferDesc(kAerStatsWords*4,D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS);
    auto plainDesc=AerBufferDesc(kAerStatsWords*4);
    if(FAILED(device->CreateCommittedResource(&defaultHeap,D3D12_HEAP_FLAG_NONE,&statsDesc,D3D12_RESOURCE_STATE_COPY_DEST,nullptr,IID_PPV_ARGS(&g.stats)))||
       FAILED(device->CreateCommittedResource(&uploadHeap,D3D12_HEAP_FLAG_NONE,&plainDesc,D3D12_RESOURCE_STATE_GENERIC_READ,nullptr,IID_PPV_ARGS(&g.statsZero)))){
        AerGpuBreak("stats-buffer");return false;}
    void* zero=nullptr;
    if(FAILED(g.statsZero->Map(0,nullptr,&zero))||!zero){AerGpuBreak("stats-zero-map");return false;}
    std::memset(zero,0,kAerStatsWords*4);g.statsZero->Unmap(0,nullptr);
    for(int i=0;i<4;++i){
        void* mapped=nullptr;const D3D12_RANGE everything{0,kAerStatsWords*4};
        if(FAILED(device->CreateCommittedResource(&readHeap,D3D12_HEAP_FLAG_NONE,&plainDesc,D3D12_RESOURCE_STATE_COPY_DEST,nullptr,IID_PPV_ARGS(&g.statsReadback[i])))||
           FAILED(g.statsReadback[i]->Map(0,&everything,&mapped))||!mapped){AerGpuBreak("stats-readback");return false;}
        g.statsMapped[i]=static_cast<const std::uint32_t*>(mapped); // readback heaps may stay mapped
    }
    g.device=device;
    return true;
}
struct AerSlot {D3D12_CPU_DESCRIPTOR_HANDLE cpu{};D3D12_GPU_DESCRIPTOR_HANDLE gpu{};};
AerSlot AerNextSlot() noexcept {
    auto& g=g_aer_gpu;const unsigned slot=g.cursor++%kAerDescriptorSlots;
    AerSlot s{g.heap->GetCPUDescriptorHandleForHeapStart(),g.heap->GetGPUDescriptorHandleForHeapStart()};
    s.cpu.ptr+=static_cast<SIZE_T>(slot)*4*g.increment;s.gpu.ptr+=static_cast<UINT64>(slot)*4*g.increment;return s;
}
D3D12_CPU_DESCRIPTOR_HANDLE AerCpu(const AerSlot& s,unsigned i) noexcept {auto h=s.cpu;h.ptr+=static_cast<SIZE_T>(i)*g_aer_gpu.increment;return h;}
D3D12_GPU_DESCRIPTOR_HANDLE AerGpuHandle(const AerSlot& s,unsigned i) noexcept {auto h=s.gpu;h.ptr+=static_cast<UINT64>(i)*g_aer_gpu.increment;return h;}
void AerSrv(ID3D12Resource* resource,DXGI_FORMAT format,D3D12_CPU_DESCRIPTOR_HANDLE at) noexcept {
    D3D12_SHADER_RESOURCE_VIEW_DESC srv{};srv.Shader4ComponentMapping=D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
    srv.Format=format;srv.ViewDimension=D3D12_SRV_DIMENSION_TEXTURE2D;srv.Texture2D.MipLevels=1;
    g_aer_gpu.device->CreateShaderResourceView(resource,&srv,at);
}
void AerTextureUav(ID3D12Resource* resource,DXGI_FORMAT format,D3D12_CPU_DESCRIPTOR_HANDLE at) noexcept {
    D3D12_UNORDERED_ACCESS_VIEW_DESC uav{};uav.Format=format;uav.ViewDimension=D3D12_UAV_DIMENSION_TEXTURE2D;
    g_aer_gpu.device->CreateUnorderedAccessView(resource,nullptr,&uav,at);
}
void AerStatsUav(ID3D12Resource* resource,D3D12_CPU_DESCRIPTOR_HANDLE at) noexcept {
    D3D12_UNORDERED_ACCESS_VIEW_DESC uav{};uav.Format=DXGI_FORMAT_UNKNOWN;uav.ViewDimension=D3D12_UAV_DIMENSION_BUFFER;
    uav.Buffer.NumElements=kAerStatsWords;uav.Buffer.StructureByteStride=4;
    g_aer_gpu.device->CreateUnorderedAccessView(resource,nullptr,&uav,at);
}
void AerBarrier(ID3D12GraphicsCommandList* list,ID3D12Resource* resource,D3D12_RESOURCE_STATES before,D3D12_RESOURCE_STATES after) noexcept {
    if(before==after)return;
    D3D12_RESOURCE_BARRIER b{};b.Type=D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;b.Transition.pResource=resource;
    b.Transition.Subresource=D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;b.Transition.StateBefore=before;b.Transition.StateAfter=after;
    list->ResourceBarrier(1,&b);
}
bool AerEnsureOutput(const D3D12_RESOURCE_DESC& motion) noexcept {
    auto& g=g_aer_gpu;
    const DXGI_FORMAT format=AerViewFormat(motion.Format);
    if(!AerTwoChannelFloat(format))return false;
    if(g.output&&g.outputDesc.Width==motion.Width&&g.outputDesc.Height==motion.Height&&g.outputDesc.Format==format)return true;
    // A previous texture may still be read by queued DLSS work: retire it late.
    if(g.output)g.retired.emplace_back(g.dispatches+16,std::move(g.output));
    D3D12_FEATURE_DATA_FORMAT_SUPPORT support{format};
    if(FAILED(g.device->CheckFeatureSupport(D3D12_FEATURE_FORMAT_SUPPORT,&support,sizeof(support)))||
       !(support.Support2&D3D12_FORMAT_SUPPORT2_UAV_TYPED_STORE))return false;
    D3D12_RESOURCE_DESC desc{};desc.Dimension=D3D12_RESOURCE_DIMENSION_TEXTURE2D;desc.Width=motion.Width;desc.Height=motion.Height;
    desc.DepthOrArraySize=1;desc.MipLevels=1;desc.Format=format;desc.SampleDesc.Count=1;
    desc.Layout=D3D12_TEXTURE_LAYOUT_UNKNOWN;desc.Flags=D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;
    auto heap=AerHeap(D3D12_HEAP_TYPE_DEFAULT);
    if(FAILED(g.device->CreateCommittedResource(&heap,D3D12_HEAP_FLAG_NONE,&desc,D3D12_RESOURCE_STATE_UNORDERED_ACCESS,nullptr,IID_PPV_ARGS(&g.output))))return false;
    g.outputDesc=desc;g.outputUav=true;return true;
}

// ---------------------------------------------------------------- model check
// Every dispatch also measures, on a 1/16 pixel subsample, how far the game's
// own N->N-1 vector is from the static-camera prediction of our model, for all
// four depth conventions. Static geometry must agree to sub-pixel precision for
// the right convention; the wrong ones are many pixels off (the IPD baseline
// between N and N-1 always exposes them). The best convention is selected and
// the reprojected vectors are used only while the model holds.
struct AerModel {
    double counts[4][8]{};
    float score[4]{};     // share of samples within 2 px of the static prediction (validity)
    float precise[4]{};   // share within 0.5 px (separates far-biased wrong conventions)
    int depthMode{-1};
    bool valid{};
    int goodRuns{},badRuns{};
    std::uint64_t readbacks{},switches{},validations{},invalidations{};
};
AerModel g_aer_model{};
const char* AerDepthName(int mode) noexcept {
    switch(mode){case 0:return "REVERSED_Z";case 1:return "STANDARD_Z";case 2:return "LINEAR_OVER_FAR";case 3:return "LINEAR_METRES";default:return "UNKNOWN";}
}
void AerModelConsume(const std::uint32_t* bins,int initialMode) noexcept {
    auto& m=g_aer_model;
    if(m.depthMode<0)m.depthMode=initialMode;
    double total[4]{},good[4]{},tight[4]{};
    for(int k=0;k<4;++k)for(int b=0;b<8;++b){
        m.counts[k][b]=m.counts[k][b]*0.85+static_cast<double>(bins[k*8+b]);
        total[k]+=m.counts[k][b];if(b<4)good[k]+=m.counts[k][b];if(b<2)tight[k]+=m.counts[k][b];
    }
    ++m.readbacks;
    for(int k=0;k<4;++k){
        m.score[k]=total[k]>0.0?static_cast<float>(good[k]/total[k]):0.0f;
        m.precise[k]=total[k]>0.0?static_cast<float>(tight[k]/total[k]):0.0f;
    }
    if(total[0]<400.0)return; // not enough evidence in the decayed window yet
    // A convention that treats everything as far away can stay within 2 px in a
    // distant scene; the exact one also lands within 0.5 px. Rank by both.
    auto rank=[&](int k){return m.score[k]+m.precise[k];};
    int best=0;for(int k=1;k<4;++k)if(rank(k)>rank(best))best=k;
    if(best!=m.depthMode&&m.score[best]>=0.30f&&rank(best)>rank(m.depthMode)+0.10f){
        char line[256]{};std::snprintf(line,sizeof(line),"R149 AER_DLSS depth=%s->%s scores=%.3f/%.3f/%.3f/%.3f",
            AerDepthName(m.depthMode),AerDepthName(best),m.score[0],m.score[1],m.score[2],m.score[3]);
        diagnostic_trace::WriteStatus(line);m.depthMode=best;++m.switches;m.goodRuns=m.badRuns=0;
    }
    const float s=m.score[m.depthMode];
    if(s>=0.30f){++m.goodRuns;m.badRuns=0;}else if(s<0.20f){++m.badRuns;m.goodRuns=0;}
    if(!m.valid&&m.goodRuns>=3){
        m.valid=true;++m.validations;
        char line[256]{};std::snprintf(line,sizeof(line),"R149 AER_DLSS model=VALID depth=%s staticAgreement=%.3f motion=SAME_EYE_REPROJECTION",
            AerDepthName(m.depthMode),s);
        diagnostic_trace::WriteStatus(line);
    }
    if(m.valid&&m.badRuns>=12){
        m.valid=false;++m.invalidations;
        char line[256]{};std::snprintf(line,sizeof(line),"R149 AER_DLSS model=INVALID depth=%s staticAgreement=%.3f motion=RESET_UNTIL_VALID",
            AerDepthName(m.depthMode),s);
        diagnostic_trace::WriteStatus(line);
    }
}

// ---------------------------------------------------------------- per-frame record
struct AerFrameInfo {
    unsigned eye{};std::uint32_t frame{};std::uint64_t epoch{};std::uintptr_t pipeline{};
    int nativeReset{},createFlags{-1},motionMode{1},depthMode{-1};
    AerRenderIdentity identity{};bool identityKnown{};
    AerInputs in{};
    AerCamera cur{},prev{};bool camerasOk{};const char* cameraFailure{"unread"};std::uintptr_t crv{};std::uint32_t renderFrame{};
    AerEye history{};
    bool continuity{},duplicate{},sameEyeHistory{},cut{};double jumpM{},jumpDeg{};
    bool dispatched{},useCorrected{},substituted{},effectiveReset{},modelValid{};
    float prevRows[12]{},histRows[12]{};float jitterFix[2]{};float objectGain{},gateFactor{};
    const char* decision{""};
};

// Dump hooks (native_dlss_aer_dump_d82.inl).
bool AerDumpArmedForDlss() noexcept;
void* AerDumpDlssBefore(ID3D12GraphicsCommandList* list,const AerFrameInfo& info,ID3D12Resource* corrected) noexcept;
void AerDumpDlssAfter(void* token,ID3D12GraphicsCommandList* list,const AerFrameInfo& info,Result result) noexcept;

struct AerStats {
    std::uint64_t evaluations{},reprojected{},resets{},nativeResets{},cuts{},duplicates{},passThrough{},nativeFailures{},
        restoreFailures{},cameraFailures{},noHistory{},modelPending{},lastLogTick{};
    float jitter[2][64][2]{};unsigned jitterCount[2]{};
    const char* lastPassThrough{""};
    const char* lastCameraFailure{""};
};
AerStats g_aer_stats{};
std::atomic<int> g_aer_motion_mode{1};

bool AerDispatch(ID3D12GraphicsCommandList* list,AerFrameInfo& f,ID3D12Resource** corrected) noexcept {
    *corrected=nullptr;
    ID3D12Device* rawDevice=nullptr;
    if(FAILED(list->GetDevice(IID_PPV_ARGS(&rawDevice)))||!rawDevice)return false;
    ComPtr<ID3D12Device> device;device.Attach(rawDevice);
    if(!AerGpuInit(device.Get())||!AerEnsureOutput(f.in.motionDesc))return false;
    auto& g=g_aer_gpu;
    const DXGI_FORMAT depthView=AerViewFormat(f.in.depthDesc.Format),motionView=AerViewFormat(f.in.motionDesc.Format);
    if(!AerTwoChannelFloat(motionView))return false;
    while(!g.retired.empty()&&g.retired.front().first<=g.dispatches)g.retired.erase(g.retired.begin());
    const auto ticket=++g.dispatches;
    // Consume the statistics written four dispatches ago (long since executed).
    const unsigned statsSlot=static_cast<unsigned>(ticket%4);
    if(g.statsTicket[statsSlot]){
        std::uint32_t bins[kAerStatsWords]{};std::memcpy(bins,g.statsMapped[statsSlot],sizeof(bins));
        const int initial=f.createFlags>=0&&(f.createFlags&(1<<3))==0?1:0; // DepthInverted bit 3
        AerModelConsume(bins,initial);
        g.statsTicket[statsSlot]=0;
    }
    if(g_aer_model.depthMode<0)g_aer_model.depthMode=f.createFlags>=0&&(f.createFlags&(1<<3))==0?1:0;
    f.depthMode=g_aer_model.depthMode;

    // Same-eye history camera; without one (first frame) the history rows are
    // the previous camera so the statistics still run, but nothing is used.
    const AerEye& h=f.history;
    const bool haveHistory=h.valid;
    AerRelativeRows(f.cur.rot,f.cur.pos,f.prev.rot,f.prev.pos,f.prevRows);
    if(haveHistory)AerRelativeRows(f.cur.rot,f.cur.pos,h.rot,h.pos,f.histRows);
    else std::memcpy(f.histRows,f.prevRows,sizeof(f.histRows));
    // MVJittered (bit 2): the game's vector also carries the jitter difference
    // to N-1; the history is N-2. Unknown flags are treated as unjittered.
    const bool jittered=f.createFlags>=0&&(f.createFlags&(1<<2))!=0;
    f.jitterFix[0]=jittered&&haveHistory?h.jitterX-g_aer_stats.jitter[f.eye^1u][(g_aer_stats.jitterCount[f.eye^1u]+63)%64][0]:0.0f;
    f.jitterFix[1]=jittered&&haveHistory?h.jitterY-g_aer_stats.jitter[f.eye^1u][(g_aer_stats.jitterCount[f.eye^1u]+63)%64][1]:0.0f;
    f.objectGain=1.0f;f.gateFactor=0.15f;
    const float histTanX=haveHistory?h.tanX:f.prev.tanX,histTanY=haveHistory?h.tanY:f.prev.tanY;
    const bool stats=f.nativeReset==0;

    std::uint32_t constants[52]{};
    auto putF=[&](unsigned i,float v){constants[i]=std::bit_cast<std::uint32_t>(v);};
    constants[0]=f.in.mvW;constants[1]=f.in.mvH;constants[2]=1u;constants[3]=static_cast<std::uint32_t>(f.depthMode);
    constants[4]=f.in.depthX;constants[5]=f.in.depthY;constants[6]=f.in.mvX;constants[7]=f.in.mvY;
    putF(8,f.in.mvScaleX);putF(9,f.in.mvScaleY);putF(10,f.cur.tanX);putF(11,f.cur.tanY);
    putF(12,f.cur.nearZ);putF(13,f.cur.farZ);putF(14,f.objectGain);putF(15,stats?1.0f:0.0f);
    for(unsigned i=0;i<12;++i)putF(16+i,f.prevRows[i]);
    for(unsigned i=0;i<12;++i)putF(28+i,f.histRows[i]);
    putF(40,f.prev.tanX);putF(41,f.prev.tanY);putF(42,histTanX);putF(43,histTanY);
    putF(44,f.jitterFix[0]);putF(45,f.jitterFix[1]);putF(46,static_cast<float>(f.in.renderW));putF(47,static_cast<float>(f.in.renderH));
    putF(48,static_cast<float>(f.in.renderW)/static_cast<float>(f.in.mvW));putF(49,static_cast<float>(f.in.renderH)/static_cast<float>(f.in.mvH));
    putF(50,f.gateFactor);putF(51,0.0f);

    const AerSlot slot=AerNextSlot();
    AerSrv(f.in.motion,motionView,AerCpu(slot,0));
    AerSrv(f.in.depth,depthView,AerCpu(slot,1));
    AerTextureUav(g.output.Get(),g.outputDesc.Format,AerCpu(slot,2));
    AerStatsUav(g.stats.Get(),AerCpu(slot,3));

    if(g.statsCopyDest)list->CopyBufferRegion(g.stats.Get(),0,g.statsZero.Get(),0,kAerStatsWords*4);
    D3D12_RESOURCE_BARRIER before[2]{};UINT beforeCount=0;
    auto push=[&](D3D12_RESOURCE_BARRIER* set,UINT& count,ID3D12Resource* r,D3D12_RESOURCE_STATES a,D3D12_RESOURCE_STATES b){
        auto& x=set[count++];x.Type=D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;x.Transition.pResource=r;
        x.Transition.Subresource=D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;x.Transition.StateBefore=a;x.Transition.StateAfter=b;};
    push(before,beforeCount,g.stats.Get(),D3D12_RESOURCE_STATE_COPY_DEST,D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
    if(!g.outputUav)push(before,beforeCount,g.output.Get(),D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE,D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
    list->ResourceBarrier(beforeCount,before);
    ID3D12DescriptorHeap* heaps[]{g.heap.Get()};list->SetDescriptorHeaps(1,heaps);
    list->SetComputeRootSignature(g.root.Get());list->SetPipelineState(g.reproject.Get());
    list->SetComputeRoot32BitConstants(0,52,constants,0);
    list->SetComputeRootDescriptorTable(1,AerGpuHandle(slot,0));
    list->SetComputeRootDescriptorTable(2,AerGpuHandle(slot,2));
    list->Dispatch((f.in.mvW+7)/8,(f.in.mvH+7)/8,1);
    D3D12_RESOURCE_BARRIER after[2]{};UINT afterCount=0;
    push(after,afterCount,g.output.Get(),D3D12_RESOURCE_STATE_UNORDERED_ACCESS,D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE);
    push(after,afterCount,g.stats.Get(),D3D12_RESOURCE_STATE_UNORDERED_ACCESS,D3D12_RESOURCE_STATE_COPY_SOURCE);
    list->ResourceBarrier(afterCount,after);
    list->CopyBufferRegion(g.statsReadback[statsSlot].Get(),0,g.stats.Get(),0,kAerStatsWords*4);
    AerBarrier(list,g.stats.Get(),D3D12_RESOURCE_STATE_COPY_SOURCE,D3D12_RESOURCE_STATE_COPY_DEST);
    g.statsCopyDest=true;g.outputUav=false;
    g.statsTicket[statsSlot]=stats?ticket:0;
    *corrected=g.output.Get();
    return true;
}

bool AerCameraContinuous(const AerEye& h,const AerCamera& c,double& jumpM,double& jumpDeg) noexcept {
    const double dx=c.pos[0]-h.pos[0],dy=c.pos[1]-h.pos[1],dz=c.pos[2]-h.pos[2];
    jumpM=std::sqrt(dx*dx+dy*dy+dz*dz);jumpDeg=AerAngleDeg(h.rot,c.rot);
    // Two AFR frames apart: a larger change is a cut the game did not flag.
    return jumpM<1.5&&jumpDeg<40.0;
}

void AerLogFirstFrame(const AerFrameInfo& f) noexcept {
    static bool logged=false;
    if(logged)return;
    logged=true;
    char line[768]{};
    std::snprintf(line,sizeof(line),
        "R149 AER_DLSS first-frame createFlags=0x%X color=%u:%llux%u depth=%u:%llux%u motion=%u:%llux%u output=%u:%llux%u render=%ux%u mvActive=%ux%u "
        "mvScale=%.4f/%.4f jitter=%.4f/%.4f cameras=%s(%s) fov=%.4f near=%.4f far=%.1f tan=%.4f/%.4f identity=%d eye=%u motionMode=%d",
        static_cast<unsigned>(f.createFlags),unsigned(f.in.colorDesc.Format),(unsigned long long)f.in.colorDesc.Width,f.in.colorDesc.Height,
        unsigned(f.in.depthDesc.Format),(unsigned long long)f.in.depthDesc.Width,f.in.depthDesc.Height,
        unsigned(f.in.motionDesc.Format),(unsigned long long)f.in.motionDesc.Width,f.in.motionDesc.Height,
        unsigned(f.in.outputDesc.Format),(unsigned long long)f.in.outputDesc.Width,f.in.outputDesc.Height,
        f.in.renderW,f.in.renderH,f.in.mvW,f.in.mvH,double(f.in.mvScaleX),double(f.in.mvScaleY),double(f.in.jitterX),double(f.in.jitterY),
        f.camerasOk?"ok":"FAILED",f.cameraFailure,double(std::isfinite(f.cur.fov)?f.cur.fov:0.0f),double(std::isfinite(f.cur.nearZ)?f.cur.nearZ:0.0f),
        double(std::isfinite(f.cur.farZ)?f.cur.farZ:0.0f),double(std::isfinite(f.cur.tanX)?f.cur.tanX:0.0f),double(std::isfinite(f.cur.tanY)?f.cur.tanY:0.0f),
        f.identity.source,f.eye,f.motionMode);
    diagnostic_trace::WriteStatus(line);
}
void AerLogPeriodic(std::uint64_t now) noexcept {
    auto& s=g_aer_stats;
    static std::uint64_t lastStatus=0;
    const bool status=!lastStatus||now-lastStatus>=60000; // always-on summary, once a minute
    if(!status&&s.lastLogTick&&now-s.lastLogTick<10000)return;
    s.lastLogTick=now;if(status)lastStatus=now;
    std::uint64_t id[6]{};FrameSync::AerIdentityStats(id);
    const auto& m=g_aer_model;
    char line[768]{};
    std::snprintf(line,sizeof(line),
        "R149 AER_DLSS status eval=%llu reprojected=%llu resets=%llu nativeResets=%llu cuts=%llu noHistory=%llu modelPending=%llu duplicates=%llu passThrough=%llu(%s) cameraFail=%llu(%s) nativeFail=%llu "
        "model={valid:%d depth:%s scores:%.2f/%.2f/%.2f/%.2f switches:%llu} identity={exact:%llu parity:%llu main:%llu mainDisagree:%llu ambiguous:%llu published:%llu} motionMode=%d gpu=%s",
        (unsigned long long)s.evaluations,(unsigned long long)s.reprojected,(unsigned long long)s.resets,(unsigned long long)s.nativeResets,
        (unsigned long long)s.cuts,(unsigned long long)s.noHistory,(unsigned long long)s.modelPending,(unsigned long long)s.duplicates,
        (unsigned long long)s.passThrough,s.lastPassThrough,(unsigned long long)s.cameraFailures,s.lastCameraFailure,(unsigned long long)s.nativeFailures,
        m.valid?1:0,AerDepthName(m.depthMode),m.score[0],m.score[1],m.score[2],m.score[3],(unsigned long long)m.switches,
        (unsigned long long)id[0],(unsigned long long)id[1],(unsigned long long)id[2],(unsigned long long)id[3],(unsigned long long)id[4],(unsigned long long)id[5],
        g_aer_motion_mode.load(std::memory_order_relaxed),g_aer_gpu.broken?g_aer_gpu.brokenReason:"ok");
    if(status)diagnostic_trace::WriteStatus(line);else diagnostic_trace::Write(line);
}

Result EvaluateAlternateEye(EvaluateFn fn,void* command,const void* handle,Pair* p,void* parameters,
    void* callback,const native_stereo_preflight::UpscalerView* view) {
    auto& st=g_aer_stats;
    auto passThrough=[&](const char* why)->Result {
        // Keep the game's own evaluation running; only our history management stops.
        ++st.passThrough;st.lastPassThrough=why;
        if(p)Invalidate(*p);
        AerLogPeriodic(GetTickCount64());
        return fn(command,handle,parameters,callback);
    };
    if(!view||view->eye>1)return passThrough("no-eye-identity");
    if(!p)return passThrough("no-captured-create");
    if(!p->right||p->right==p->native)return passThrough("no-twin");
    int nativeReset=0;
    if(!ReadEvaluation(parameters,nativeReset))return passThrough("unreadable-inputs");
    const auto generation=g_generation.load(std::memory_order_acquire);
    if(p->generation!=generation||p->epoch!=view->epoch||p->pipeline!=view->pipeline){
        Invalidate(*p);p->generation=generation;p->epoch=view->epoch;p->pipeline=view->pipeline;
    }
    auto* list=static_cast<ID3D12GraphicsCommandList*>(command);
    const unsigned eye=view->eye;
    p->nativeReset[eye]=nativeReset;
    AerFrameInfo f{};
    f.eye=eye;f.frame=view->frame;f.epoch=view->epoch;f.pipeline=view->pipeline;f.nativeReset=nativeReset;
    f.createFlags=p->createFlags;f.motionMode=g_aer_motion_mode.load(std::memory_order_acquire);
    f.identityKnown=FrameSync::CurrentAerRenderIdentity(f.identity);
    const bool inputsOk=AerReadInputs(parameters,p->createFlags,f.in);
    f.camerasOk=ReadAerCameras(view->pipeline,f.cur,f.prev,f.crv,f.renderFrame,f.cameraFailure);
    if(!f.camerasOk){++st.cameraFailures;st.lastCameraFailure=f.cameraFailure;}
    f.duplicate=p->have[eye]&&p->frame[eye]==view->frame;
    if(f.duplicate){++st.duplicates;p->reset[eye]=true;}
    f.history=p->aer[eye];
    f.continuity=!f.duplicate&&p->have[eye]&&!p->reset[eye]&&std::uint32_t(view->frame-p->frame[eye])==2u;
    f.sameEyeHistory=f.continuity&&f.history.valid&&f.camerasOk;
    if(f.sameEyeHistory&&!AerCameraContinuous(f.history,f.cur,f.jumpM,f.jumpDeg)){f.sameEyeHistory=false;f.cut=true;++st.cuts;}

    ID3D12Resource* corrected=nullptr;
    if(f.motionMode==1&&inputsOk&&f.camerasOk&&!g_aer_gpu.broken)
        f.dispatched=AerDispatch(list,f,&corrected);
    f.modelValid=g_aer_model.valid;
    f.useCorrected=f.motionMode==1&&f.dispatched&&corrected&&f.sameEyeHistory&&nativeReset==0&&f.modelValid;
    if(f.useCorrected&&g_set_resource){
        g_set_resource(parameters,"MotionVectors",corrected);void* observed=nullptr;
        f.substituted=Success(g_get_resource(parameters,"MotionVectors",&observed))&&observed==corrected;
        if(!f.substituted)g_set_resource(parameters,"MotionVectors",f.in.motion);
    }
    if(f.motionMode==2)f.effectiveReset=nativeReset!=0||!f.continuity; // diagnostic: cross-eye vectors as-is
    else f.effectiveReset=nativeReset!=0||!f.substituted;
    f.decision=nativeReset?"native-reset":f.motionMode==0?"mode-reset":f.motionMode==2?(f.effectiveReset?"raw-reset":"raw-vectors"):
        f.substituted?"reprojected":!inputsOk?"inputs":!f.camerasOk?"cameras":g_aer_gpu.broken?"gpu":
        f.duplicate?"duplicate":f.cut?"cut":!f.continuity?"no-history":!f.history.valid?"no-history-camera":
        !f.dispatched?"dispatch":!f.modelValid?"model-pending":"substitution";

    // R154 stale-eye warp inputs: this eye's depth for its own next warp, and the
    // game's N -> N-1 vectors (N-1 is the other eye's image) as a motion field on
    // the other eye's grid. Uses the ORIGINAL vectors, never the substituted ones.
    if(aer_warp_r154::Mode()>=aer_warp_r154::kSixDof && inputsOk && f.camerasOk &&
       f.identityKnown && f.identity.source==1 && f.identity.poseFrame){
        aer_warp_r154::LiveEyeInputs w{};
        w.eye=eye;w.renderFrame=view->frame;w.poseFrame=f.identity.poseFrame;
        w.eyeOffsetWorld=f.identity.eyeOffsetWorld;w.eyeOffsetKnown=f.identity.eyeOffsetKnown;
        w.depth=f.in.depth;w.depthView=AerViewFormat(f.in.depthDesc.Format);w.depthX=f.in.depthX;w.depthY=f.in.depthY;
        w.motion=f.in.motion;w.motionView=AerViewFormat(f.in.motionDesc.Format);
        w.mvX=f.in.mvX;w.mvY=f.in.mvY;w.mvW=f.in.mvW;w.mvH=f.in.mvH;w.mvScaleX=f.in.mvScaleX;w.mvScaleY=f.in.mvScaleY;
        w.renderW=f.in.renderW;w.renderH=f.in.renderH;
        w.depthMode=g_aer_model.depthMode;w.modelValid=g_aer_model.valid;
        w.camerasOk=AerTwoChannelFloat(w.motionView);
        auto copyCamera=[](const AerCamera& a,aer_warp_r154::Camera& b){
            std::memcpy(b.rot,a.rot,sizeof(b.rot));std::memcpy(b.pos,a.pos,sizeof(b.pos));
            b.tanX=a.tanX;b.tanY=a.tanY;b.nearZ=a.nearZ;b.farZ=a.farZ;};
        copyCamera(f.cur,w.cur);copyCamera(f.prev,w.prev);
        aer_warp_r154::OnLiveEyeEvaluate(list,w);
    }

    p->reset[eye]=f.effectiveReset;
    const bool overrideReset=p->reset[eye] && nativeReset==0;
    if(overrideReset){
        g_set_i(parameters,"Reset",1);int observed=0;
        if(!Success(g_get_i(parameters,"Reset",&observed))||observed!=1){
            g_set_i(parameters,"Reset",nativeReset);
            if(f.substituted)g_set_resource(parameters,"MotionVectors",f.in.motion);
            return passThrough("reset-write-not-observed");
        }
    }
    void* dumpToken=AerDumpArmedForDlss()?AerDumpDlssBefore(list,f,f.substituted?corrected:nullptr):nullptr;
    const void* selected=eye?p->right:p->native;
    const Result result=fn(command,selected,parameters,callback);
    const auto nativeError=SaveNativeError();
    if(dumpToken)AerDumpDlssAfter(dumpToken,list,f,result);
    bool restored=true;
    if(overrideReset){
        g_set_i(parameters,"Reset",nativeReset);int observed=-1;
        restored=Success(g_get_i(parameters,"Reset",&observed))&&observed==nativeReset;
    }
    if(f.substituted){
        g_set_resource(parameters,"MotionVectors",f.in.motion);void* observed=nullptr;
        restored=restored&&Success(g_get_resource(parameters,"MotionVectors",&observed))&&observed==f.in.motion;
    }
    RestoreNativeError(nativeError);
    ++st.evaluations;
    if(!restored){++st.restoreFailures;Invalidate(*p);return result;}
    if(!Success(result)){++st.nativeFailures;Invalidate(*p);return result;}
    if(generation!=g_generation.load(std::memory_order_acquire)){Invalidate(*p);return result;}
    p->have[eye]=true;p->frame[eye]=view->frame;p->reset[eye]=false;p->effectiveReset[eye]=f.effectiveReset;
    AerEye saved{};
    if(f.camerasOk){
        std::memcpy(saved.rot,f.cur.rot,sizeof(saved.rot));std::memcpy(saved.pos,f.cur.pos,sizeof(saved.pos));
        saved.tanX=f.cur.tanX;saved.tanY=f.cur.tanY;saved.nearZ=f.cur.nearZ;saved.farZ=f.cur.farZ;
        saved.jitterX=f.in.jitterX;saved.jitterY=f.in.jitterY;saved.frame=view->frame;saved.valid=true;
    }
    p->aer[eye]=saved;
    auto& j=st.jitter[eye][st.jitterCount[eye]%64];j[0]=f.in.jitterX;j[1]=f.in.jitterY;++st.jitterCount[eye];
    if(f.substituted)++st.reprojected;else ++st.resets;
    if(nativeReset)++st.nativeResets;
    if(f.motionMode==1&&!f.substituted){if(!f.continuity||!f.history.valid)++st.noHistory;else if(!f.modelValid)++st.modelPending;}
    if(eye)++g_stats.rightEvaluations;else ++g_stats.leftEvaluations;
    if(f.effectiveReset)++g_stats.resets;
    ReportEvaluationInputs(eye,view->epoch,view->frame,parameters,nativeReset,f.effectiveReset);
    AerLogFirstFrame(f);
    AerLogPeriodic(GetTickCount64());
    return result;
}
