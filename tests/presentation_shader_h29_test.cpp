#include <d3dcompiler.h>
#include <fstream>
#include <iterator>
#include <string>
#include <cstdio>
// Compile the actual complete production shader, not a copied formula.
int main(int argc,char** argv){
    if(argc!=2)return 1;
    std::ifstream file(argv[1],std::ios::binary);
    const std::string source{std::istreambuf_iterator<char>(file),{}};
    const std::string marker="const char* shader = R\"(";
    const auto begin=source.find(marker);
    if(begin==std::string::npos)return 2;
    const auto first=begin+marker.size(),end=source.find(")\";",first);
    if(end==std::string::npos)return 3;
    const auto shader=source.substr(first,end-first);
    const char* entries[]{"VSMain","PSMain"};
    const char* profiles[]{"vs_5_0","ps_5_0"};
    for(unsigned i=0;i<2;++i){
        ID3DBlob *code=nullptr,*errors=nullptr;
        const auto hr=D3DCompile(shader.data(),shader.size(),"production_present",nullptr,nullptr,entries[i],profiles[i],D3DCOMPILE_OPTIMIZATION_LEVEL3,0,&code,&errors);
        if(errors){std::fwrite(errors->GetBufferPointer(),1,errors->GetBufferSize(),stderr);errors->Release();}
        const bool ok=SUCCEEDED(hr)&&code&&code->GetBufferSize();
        if(code)code->Release();
        if(!ok)return 4;
    }
    std::puts("PASS production presentation VS/PS compile, including gated angular and unchanged legacy vignette; no GPU execution claimed");
    return 0;
}
