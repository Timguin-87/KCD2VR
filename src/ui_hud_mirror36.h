#pragma once
#include <d3d12.h>
#include <d3dcompiler.h>
#include <cstring>

namespace kcd2vr::hud_layer_v6664 {
template<class List,class Bind,class DrawOriginal>
void RecordMirror36(List* list,ID3D12DescriptorHeap* srv,ID3D12RootSignature* root,ID3D12PipelineState* pso,
                    D3D12_GPU_DESCRIPTOR_HANDLE source,D3D12_CPU_DESCRIPTOR_HANDLE target,float width,float height,
                    Bind originalBind,DrawOriginal originalDraw) noexcept {
    if(!originalBind||!originalDraw)return;
    ID3D12DescriptorHeap* heaps[]{srv};list->SetDescriptorHeaps(1,heaps);list->SetGraphicsRootSignature(root);
    list->SetPipelineState(pso);list->SetGraphicsRootDescriptorTable(0,source);
    list->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    D3D12_VIEWPORT viewport{0,0,width,height,0,1};D3D12_RECT scissor{0,0,LONG(width),LONG(height)};
    list->RSSetViewports(1,&viewport);list->RSSetScissorRects(1,&scissor);
    // Called with the late HUD scope and mutex active: bypass both intercepted entries.
    originalBind(list,1,&target,FALSE,nullptr);originalDraw(list,3,1,0,0);
}
inline const char* MirrorShader36() noexcept {
    return "Texture2D t:register(t0);SamplerState s:register(s0);struct V{float4 p:SV_Position;float2 uv:TEXCOORD0;};V vs(uint i:SV_VertexID){V o;float2 u=float2((i<<1)&2,i&2);o.uv=u;o.p=float4(u.x*2-1,1-u.y*2,0,1);return o;}float4 ps(V i):SV_Target{return t.Sample(s,i.uv);}";
}
// A single alpha composite AFTER both eyes and the XR HUD cache copy. No Flash
// replay, readback, extra queue submission or wait. Resources belong to the HUD
// texture generation and follow its retained-until-shutdown GPU lifetime.
struct Mirror36 {
    ID3D12DescriptorHeap* srv=nullptr;
    ID3D12RootSignature* root=nullptr;
    ID3D12PipelineState* pso=nullptr;
    bool ready=false;
    bool Initialize(ID3D12Device* device,ID3D12Resource* texture,DXGI_FORMAT format) noexcept {
        D3D12_DESCRIPTOR_HEAP_DESC heap{};heap.Type=D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;heap.NumDescriptors=1;heap.Flags=D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
        if(FAILED(device->CreateDescriptorHeap(&heap,IID_PPV_ARGS(&srv))))return false;
        D3D12_SHADER_RESOURCE_VIEW_DESC view{};view.Format=format;view.ViewDimension=D3D12_SRV_DIMENSION_TEXTURE2D;view.Shader4ComponentMapping=D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;view.Texture2D.MipLevels=1;
        device->CreateShaderResourceView(texture,&view,srv->GetCPUDescriptorHandleForHeapStart());
        D3D12_DESCRIPTOR_RANGE range{};range.RangeType=D3D12_DESCRIPTOR_RANGE_TYPE_SRV;range.NumDescriptors=1;
        D3D12_ROOT_PARAMETER parameter{};parameter.ParameterType=D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;parameter.DescriptorTable={1,&range};parameter.ShaderVisibility=D3D12_SHADER_VISIBILITY_PIXEL;
        D3D12_STATIC_SAMPLER_DESC sampler{};sampler.Filter=D3D12_FILTER_MIN_MAG_MIP_LINEAR;sampler.AddressU=sampler.AddressV=sampler.AddressW=D3D12_TEXTURE_ADDRESS_MODE_CLAMP;sampler.MaxLOD=D3D12_FLOAT32_MAX;sampler.MaxAnisotropy=1;sampler.ComparisonFunc=D3D12_COMPARISON_FUNC_ALWAYS;sampler.ShaderVisibility=D3D12_SHADER_VISIBILITY_PIXEL;
        D3D12_ROOT_SIGNATURE_DESC signature{1,&parameter,1,&sampler,D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT};
        ID3DBlob* serialized=nullptr;ID3DBlob* error=nullptr;
        const auto hr=D3D12SerializeRootSignature(&signature,D3D_ROOT_SIGNATURE_VERSION_1,&serialized,&error);
        if(error)error->Release();if(FAILED(hr))return false;
        const auto rootHr=device->CreateRootSignature(0,serialized->GetBufferPointer(),serialized->GetBufferSize(),IID_PPV_ARGS(&root));serialized->Release();if(FAILED(rootHr))return false;
        const char* code=MirrorShader36();
        ID3DBlob* vs=nullptr;ID3DBlob* ps=nullptr;
        auto compile=[&](const char* entry,const char* model,ID3DBlob** blob){ID3DBlob* errors=nullptr;const auto h=D3DCompile(code,std::strlen(code),nullptr,nullptr,nullptr,entry,model,D3DCOMPILE_OPTIMIZATION_LEVEL3,0,blob,&errors);if(errors)errors->Release();return SUCCEEDED(h);};
        if(!compile("vs","vs_5_0",&vs)||!compile("ps","ps_5_0",&ps)){if(vs)vs->Release();if(ps)ps->Release();return false;}
        D3D12_GRAPHICS_PIPELINE_STATE_DESC pipeline{};pipeline.pRootSignature=root;pipeline.VS={vs->GetBufferPointer(),vs->GetBufferSize()};pipeline.PS={ps->GetBufferPointer(),ps->GetBufferSize()};
        pipeline.SampleMask=UINT_MAX;pipeline.RasterizerState.FillMode=D3D12_FILL_MODE_SOLID;pipeline.RasterizerState.CullMode=D3D12_CULL_MODE_NONE;pipeline.RasterizerState.DepthClipEnable=TRUE;
        pipeline.DepthStencilState.DepthEnable=FALSE;pipeline.DepthStencilState.StencilEnable=FALSE;pipeline.DepthStencilState.DepthFunc=D3D12_COMPARISON_FUNC_ALWAYS;
        auto& blend=pipeline.BlendState.RenderTarget[0];blend.BlendEnable=TRUE;blend.SrcBlend=D3D12_BLEND_ONE;blend.DestBlend=D3D12_BLEND_INV_SRC_ALPHA;blend.BlendOp=D3D12_BLEND_OP_ADD;blend.SrcBlendAlpha=D3D12_BLEND_ONE;blend.DestBlendAlpha=D3D12_BLEND_INV_SRC_ALPHA;blend.BlendOpAlpha=D3D12_BLEND_OP_ADD;blend.RenderTargetWriteMask=D3D12_COLOR_WRITE_ENABLE_ALL;
        pipeline.PrimitiveTopologyType=D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;pipeline.NumRenderTargets=1;pipeline.RTVFormats[0]=format;pipeline.SampleDesc.Count=1;
        ready=SUCCEEDED(device->CreateGraphicsPipelineState(&pipeline,IID_PPV_ARGS(&pso)));vs->Release();ps->Release();return ready;
    }
    template<class Bind,class DrawOriginal>
    void Draw(ID3D12GraphicsCommandList* list,D3D12_CPU_DESCRIPTOR_HANDLE target,float width,float height,Bind originalBind,DrawOriginal originalDraw) noexcept {
        RecordMirror36(list,srv,root,pso,srv->GetGPUDescriptorHandleForHeapStart(),target,width,height,originalBind,originalDraw);
    }
};
}
