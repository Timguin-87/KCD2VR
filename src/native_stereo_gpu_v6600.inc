// V66.00 D3D12 view-instancing creation census. Included in device_hook.cpp.
// Read-only: does not change PSO streams, shaders, masks, targets or GPU work.
// PSO creation is NOT evidence that the PSO is bound/drawn. Pipeline-library
// loads, masks and vendor-extension stereo are outside this observer's coverage.
namespace {
template<class T> struct alignas(void*) V6600StreamPart { D3D12_PIPELINE_STATE_SUBOBJECT_TYPE type; T value; };
struct V6600StreamInfo { bool complete=false,found=false;unsigned viewCount=0,flags=0,unknown=0xffffffffu;std::size_t consumed=0; };
static V6600StreamInfo V6600ParseStream(const D3D12_PIPELINE_STATE_STREAM_DESC* desc) noexcept {
 V6600StreamInfo r{};
 __try {
  if(!desc || !desc->pPipelineStateSubobjectStream || !desc->SizeInBytes || desc->SizeInBytes>1024*1024)return r;
  const auto* b=static_cast<const unsigned char*>(desc->pPipelineStateSubobjectStream);std::size_t o=0;
  for(unsigned steps=0;o<desc->SizeInBytes && steps<256;++steps){
   if(desc->SizeInBytes-o<sizeof(D3D12_PIPELINE_STATE_SUBOBJECT_TYPE))return r;
   const auto type=*reinterpret_cast<const D3D12_PIPELINE_STATE_SUBOBJECT_TYPE*>(b+o);std::size_t size=0;
#define V6600_PART(E,T) case D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_##E: size=sizeof(V6600StreamPart<T>);break
   switch(type){
    V6600_PART(ROOT_SIGNATURE,ID3D12RootSignature*);
    V6600_PART(VS,D3D12_SHADER_BYTECODE);V6600_PART(PS,D3D12_SHADER_BYTECODE);V6600_PART(DS,D3D12_SHADER_BYTECODE);V6600_PART(HS,D3D12_SHADER_BYTECODE);V6600_PART(GS,D3D12_SHADER_BYTECODE);V6600_PART(CS,D3D12_SHADER_BYTECODE);
    V6600_PART(STREAM_OUTPUT,D3D12_STREAM_OUTPUT_DESC);V6600_PART(BLEND,D3D12_BLEND_DESC);V6600_PART(SAMPLE_MASK,UINT);V6600_PART(RASTERIZER,D3D12_RASTERIZER_DESC);V6600_PART(DEPTH_STENCIL,D3D12_DEPTH_STENCIL_DESC);
    V6600_PART(INPUT_LAYOUT,D3D12_INPUT_LAYOUT_DESC);V6600_PART(IB_STRIP_CUT_VALUE,D3D12_INDEX_BUFFER_STRIP_CUT_VALUE);V6600_PART(PRIMITIVE_TOPOLOGY,D3D12_PRIMITIVE_TOPOLOGY_TYPE);
    V6600_PART(RENDER_TARGET_FORMATS,D3D12_RT_FORMAT_ARRAY);V6600_PART(DEPTH_STENCIL_FORMAT,DXGI_FORMAT);V6600_PART(SAMPLE_DESC,DXGI_SAMPLE_DESC);V6600_PART(NODE_MASK,UINT);V6600_PART(CACHED_PSO,D3D12_CACHED_PIPELINE_STATE);V6600_PART(FLAGS,D3D12_PIPELINE_STATE_FLAGS);V6600_PART(DEPTH_STENCIL1,D3D12_DEPTH_STENCIL_DESC1);
    case D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_VIEW_INSTANCING:size=sizeof(V6600StreamPart<D3D12_VIEW_INSTANCING_DESC>);break;
    // AS/MS have the same shader-bytecode payload. Numeric tags retain compatibility
    // with SDKs predating the named enumerators. Newer unknown tags stop parsing.
    case static_cast<D3D12_PIPELINE_STATE_SUBOBJECT_TYPE>(24):
    case static_cast<D3D12_PIPELINE_STATE_SUBOBJECT_TYPE>(25):size=sizeof(V6600StreamPart<D3D12_SHADER_BYTECODE>);break;
    default:r.unknown=static_cast<unsigned>(type);r.consumed=o;return r;
   }
#undef V6600_PART
   if(!size || size>desc->SizeInBytes-o)return r;
   if(type==D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_VIEW_INSTANCING){
    if(r.found){r.unknown=static_cast<unsigned>(type);return r;}
    const auto& v=reinterpret_cast<const V6600StreamPart<D3D12_VIEW_INSTANCING_DESC>*>(b+o)->value;
    r.found=true;r.viewCount=v.ViewInstanceCount;r.flags=static_cast<unsigned>(v.Flags);
   }
   o+=size;r.consumed=o;
  }
  r.complete=(o==desc->SizeInBytes);
 }__except(EXCEPTION_EXECUTE_HANDLER){r.complete=false;}
 return r;
}
using V6600StreamCreateFn=HRESULT(__stdcall*)(ID3D12Device2*,const D3D12_PIPELINE_STATE_STREAM_DESC*,REFIID,void**);
std::optional<DetourModKit::hook::Hook> g_v6600_stream_hook;
V6600StreamCreateFn g_v6600_stream_original=nullptr;
std::atomic<std::uint64_t> g_v6600_legacy_psos{0},g_v6600_stream_psos{0},g_v6600_multi_psos{0},g_v6600_unknown_streams{0};
std::atomic<bool> g_v6600_gpu_enabled{true};
HRESULT __stdcall V6600CreateStreamDetour(ID3D12Device2*device,const D3D12_PIPELINE_STATE_STREAM_DESC*desc,REFIID iid,void**out) {
 const auto info=V6600ParseStream(desc);
 const HRESULT hr=g_v6600_stream_original?g_v6600_stream_original(device,desc,iid,out):E_FAIL;
 if(SUCCEEDED(hr) && out && *out){
  const auto n=g_v6600_stream_psos.fetch_add(1,std::memory_order_relaxed)+1;
  if(!info.complete)g_v6600_unknown_streams.fetch_add(1,std::memory_order_relaxed);
  if(info.found && info.viewCount>1)g_v6600_multi_psos.fetch_add(1,std::memory_order_relaxed);
  if(n<=32 || n%1024==0 || (info.found && info.viewCount>1 && g_v6600_multi_psos.load(std::memory_order_relaxed)<=128)){
   char m[512]{};std::snprintf(m,sizeof(m),"V66.00-GPU STREAM-CREATE n=%llu pso=%p parseComplete=%d foundViewInstancing=%d declaredViews=%u flags=0x%X unknownTag=%u consumed=%llu NOTE=creation-not-bound-draw",
    (unsigned long long)n,*out,info.complete?1:0,info.found?1:0,info.viewCount,info.flags,info.unknown,(unsigned long long)info.consumed);diagnostic_trace::Write(m);
  }
 }
 return hr;
}
void V6600LegacyPsoCreated(void*pso) noexcept {
 if(!g_v6600_gpu_enabled.load(std::memory_order_relaxed))return;
 const auto n=g_v6600_legacy_psos.fetch_add(1,std::memory_order_relaxed)+1;
 if(n<=16 || n%1024==0){char m[320]{};std::snprintf(m,sizeof(m),"V66.00-GPU LEGACY-GRAPHICS-CREATE n=%llu pso=%p D3D12-view-instancing-declaration=absent NOTE=does-not-rule-out-shader-instanced-or-vendor-stereo",(unsigned long long)n,pso);diagnostic_trace::Write(m);}
}
void V6600GpuInstall(ID3D12Device*device) noexcept {
 if(!device)return;
 g_v6600_gpu_enabled.store(true,std::memory_order_release); // V66.01 single-run package: always observe; no environment setup
 D3D12_FEATURE_DATA_D3D12_OPTIONS3 options{};const HRESULT h=device->CheckFeatureSupport(D3D12_FEATURE_D3D12_OPTIONS3,&options,sizeof(options));
 D3D12_FEATURE_DATA_SHADER_MODEL sm{D3D_SHADER_MODEL_6_1};const HRESULT hs=device->CheckFeatureSupport(D3D12_FEATURE_SHADER_MODEL,&sm,sizeof(sm));
 char m[320]{};std::snprintf(m,sizeof(m),"V66.00-GPU CAPS options3Hr=0x%08X viewInstancingTier=%u shaderModelHr=0x%08X shaderModelUpTo61=0x%X NOTE=capability-not-active-rendering",unsigned(h),SUCCEEDED(h)?unsigned(options.ViewInstancingTier):~0u,unsigned(hs),SUCCEEDED(hs)?unsigned(sm.HighestShaderModel):0u);diagnostic_trace::Write(m);
 if(g_v6600_stream_original)return;
 ID3D12Device2*d2=nullptr;const HRESULT qi=device->QueryInterface(IID_PPV_ARGS(&d2));
 if(FAILED(qi)||!d2){diagnostic_trace::Write("V66.00-GPU no Device2 interface: stream creation unobserved");return;}
 void**v=*reinterpret_cast<void***>(d2);
 // IUnknown 3 + ID3D12Object 4 + Device 37 + Device1 3 => Device2 method 47.
 hook::InlineRequest req{"v6600_create_pipeline_stream",hook::Target{DetourModKit::Address{reinterpret_cast<std::uintptr_t>(v[47])}},hook::Options{}};
 auto result=hook::inline_at(std::move(req),&V6600CreateStreamDetour);
 if(result){g_v6600_stream_hook=std::move(*result);g_v6600_stream_original=g_v6600_stream_hook->original<V6600StreamCreateFn>();if(!g_v6600_stream_original || !g_v6600_stream_hook->enable()){g_v6600_stream_original=nullptr;g_v6600_stream_hook.reset();}}
 d2->Release();diagnostic_trace::Write(g_v6600_stream_original?"V66.00-GPU stream observer installed; library loads/masks/draw binding NOT covered":"V66.00-GPU stream hook failed; multiview absence cannot be inferred");
}
void V6600GpuUninstall() noexcept {if(g_v6600_stream_hook){(void)g_v6600_stream_hook->disable();g_v6600_stream_hook.reset();}g_v6600_stream_original=nullptr;}
}
