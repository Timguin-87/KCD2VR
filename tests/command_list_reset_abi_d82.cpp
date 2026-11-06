// Optional Windows SDK ABI check, deliberately separate from production TUs.
// Run: cl /nologo /std:c++20 /c tests\command_list_reset_abi_d82.cpp
#define CINTERFACE
#include <d3d12.h>
#include <cstddef>
#include <type_traits>
static_assert(offsetof(ID3D12GraphicsCommandListVtbl,Reset)==10*sizeof(void*));
static_assert(offsetof(ID3D12GraphicsCommandListVtbl,ClearState)==11*sizeof(void*));
static_assert(offsetof(ID3D12GraphicsCommandListVtbl,DrawInstanced)==12*sizeof(void*));
static_assert(offsetof(ID3D12GraphicsCommandListVtbl,DrawIndexedInstanced)==13*sizeof(void*));
using ResetSignature=HRESULT(STDMETHODCALLTYPE*)(ID3D12GraphicsCommandList*,ID3D12CommandAllocator*,ID3D12PipelineState*);
static_assert(std::is_same_v<decltype(ID3D12GraphicsCommandListVtbl::Reset),ResetSignature>);
