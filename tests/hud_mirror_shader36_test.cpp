#include "ui_hud_mirror36.h"
#include <cassert>
#include <cstdio>
struct FakeList {
    unsigned states=0,binds=0,draws=0;
    void SetDescriptorHeaps(UINT,ID3D12DescriptorHeap**){++states;}
    void SetGraphicsRootSignature(ID3D12RootSignature*){++states;}
    void SetPipelineState(ID3D12PipelineState*){++states;}
    void SetGraphicsRootDescriptorTable(UINT,D3D12_GPU_DESCRIPTOR_HANDLE){++states;}
    void IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY){++states;}
    void RSSetViewports(UINT,const D3D12_VIEWPORT*){++states;}
    void RSSetScissorRects(UINT,const D3D12_RECT*){++states;}
    // Intercepted COM entry points are forbidden while the nonrecursive HUD lock is held.
    void OMSetRenderTargets(UINT,const D3D12_CPU_DESCRIPTOR_HANDLE*,BOOL,const D3D12_CPU_DESCRIPTOR_HANDLE*){assert(false);}
    void DrawInstanced(UINT,UINT,UINT,UINT){assert(false);}
};
void BindOriginal(FakeList* l,UINT n,const D3D12_CPU_DESCRIPTOR_HANDLE* target,BOOL,const D3D12_CPU_DESCRIPTOR_HANDLE*){assert(n==1&&target->ptr==42);++l->binds;}
void DrawOriginal(FakeList* l,UINT n,UINT instances,UINT,UINT){assert(n==3&&instances==1);++l->draws;}
int main(){
    FakeList list;
    using namespace kcd2vr::hud_layer_v6664;
    RecordMirror36(&list,nullptr,nullptr,nullptr,{11},{42},1920,1080,&BindOriginal,&DrawOriginal);
    assert(list.states==7&&list.binds==1&&list.draws==1);
    RecordMirror36(&list,nullptr,nullptr,nullptr,{11},{42},1920,1080,decltype(&BindOriginal)(nullptr),&DrawOriginal);
    assert(list.states==7&&list.binds==1&&list.draws==1); // unavailable trampoline skips all recording
    const auto* code=kcd2vr::hud_layer_v6664::MirrorShader36();
    const char* entries[]{"vs","ps"};const char* profiles[]{"vs_5_0","ps_5_0"};
    for(unsigned i=0;i<2;++i){ID3DBlob* compiled=nullptr;ID3DBlob* errors=nullptr;
        const auto hr=D3DCompile(code,std::strlen(code),nullptr,nullptr,nullptr,entries[i],profiles[i],D3DCOMPILE_OPTIMIZATION_LEVEL3,0,&compiled,&errors);
        if(errors){std::fwrite(errors->GetBufferPointer(),1,errors->GetBufferSize(),stderr);errors->Release();}
        assert(SUCCEEDED(hr)&&compiled&&compiled->GetBufferSize()>0);compiled->Release();
    }
    std::puts("PASS actual desktop HUD vertex/pixel shaders compile; no GPU execution claimed");
}
