#include "companion_loader59c4.h"
#include "retail_build112.h"
#include <DetourModKit/hook.hpp>
#include <DetourModKit/logger.hpp>
#include <windows.h>
#include <atomic>
#include <algorithm>
#include <iterator>
#include <array>
#include <optional>
#include <cstdio>
#include <cstring>
#include <cwchar>

namespace kcd2vr::companion_loader59c4 { namespace {
using LoadFn=bool(__fastcall*)(void*,void*,bool);
using TwoFn=bool(__fastcall*)(void*,void*);
LoadFn load=nullptr; TwoFn dispatch=nullptr,xml=nullptr;
std::uintptr_t base=0;
std::optional<DetourModKit::hook::Hook> hooks[7];
std::atomic<unsigned> lines{0},failures{0},criticalLines{0};
std::atomic<bool> ready{false};
struct Frame { void* descriptor=nullptr; char table[128]{},path[256]{};
    int version=-1; unsigned conversions=0,mismatches=0; bool xmlSeen=false,watched=false; };
thread_local Frame* current=nullptr;
thread_local unsigned depth=0;
template<class T> T Read(std::uintptr_t p,T fallback={}) noexcept {
    __try {return p?*reinterpret_cast<const T*>(p):fallback;}
    __except(EXCEPTION_EXECUTE_HANDLER){return fallback;}
}
template<std::size_t N> void Text(char (&out)[N],std::uintptr_t p) noexcept {
    out[0]=0;if(!p)return;
    __try {for(std::size_t i=0;i<N-1;++i){const char c=*reinterpret_cast<const char*>(p+i);
        out[i]=(c=='\r'||c=='\n'||c=='\t')?' ':c;if(!c)return;}out[N-1]=0;}
    __except(EXCEPTION_EXECUTE_HANDLER){std::snprintf(out,N,"<unreadable>");}
}
void Write(const char* event,const Frame* f,bool result,int detail=0) noexcept {
    const bool critical=!result&&std::strcmp(event,"XML_RESULT_NOT_FATAL_BY_ITSELF")!=0;
    if(critical){if(criticalLines.fetch_add(1)>=64)return;}
    else if(lines.fetch_add(1)>=2048)return;
    // Native callback: stay inside the logger inline record bound and never wait.
    if(!DetourModKit::log().is_async_mode_enabled())return;
    char line[DetourModKit::LOG_INLINE_MESSAGE_SIZE]{};
    const int n=std::snprintf(line,sizeof(line),
        "R59C4 LOADER tick=%llu thread=%lu depth=%u event=%s result=%d table=%s descriptor=%p serializerPath=%s xmlVersion=%d conversions=%u rowTagMismatches=%u detail=%d writes=NONE\r\n",
        (unsigned long long)GetTickCount64(),GetCurrentThreadId(),depth,event,result,
        f?f->table:"<none>",f?f->descriptor:nullptr,f?f->path:"<none>",f?f->version:-1,
        f?f->conversions:0,f?f->mismatches:0,detail);
    if(n>0)(void)DetourModKit::log().log_noexcept(critical?DetourModKit::LogLevel::Error:DetourModKit::LogLevel::Info,
        std::string_view(line,(std::min)(std::size_t(n),sizeof(line)-1)));
    // Admission is best-effort; a fatal immediately afterward may lose queued text.
}
struct Scope { Frame frame{};Frame* previous=current;
    explicit Scope(void* descriptor) {frame.descriptor=descriptor;
        Text(frame.table,Read<std::uintptr_t>(reinterpret_cast<std::uintptr_t>(descriptor)+0x28));
        frame.watched=std::strncmp(frame.table,"combat_",7)==0||std::strncmp(frame.table,"hit_reaction",12)==0;
        ++depth;current=&frame;}
    ~Scope(){current=previous;--depth;}
};
bool __fastcall Load(void* self,void* descriptor,bool force) {
    Scope scope(descriptor);Write("TABLE_BEGIN",current,true,force);
    const bool result=load(self,descriptor,force);
    if(!result)++failures;
    Write("TABLE_END",current,result);return result;
}
bool __fastcall Dispatch(void* self,void* descriptor) {
    // D183A8's false reaches fatal D18507; record BEFORE its caller can abort.
    const bool result=dispatch(self,descriptor);
    if(!result) {Scope scope(descriptor);Write("FIRST_STAGE_NO_SERIALIZER_ACCEPTED",current,false,int(++failures));}
    return result;
}
bool __fastcall Xml(void* self,void* descriptor) {
    Scope scope(descriptor);current->xmlSeen=true;
    const bool result=xml(self,descriptor);
    // Missing optional XML is a normal serializer miss, not a database-failure claim.
    if(current->watched&&current->path[0])Write("XML_RESULT_NOT_FATAL_BY_ITSELF",current,result);
    return result;
}
using namespace DetourModKit::hook;
void Path(MidContext& c) noexcept {
    if(current)Text(current->path,Read<std::uintptr_t>(stack_pointer(c)+0x58));
}
void Rows(MidContext& c) noexcept {
    if(!current)return;
    current->version=Read<int>(stack_pointer(c)+0xA0,-1);
    ++current->conversions;
    if(!(gpr(c,Gpr::Rax)&0xff))Write("ROW_CONVERSION_FALSE",current,false);
}
void RowTag(MidContext& c) noexcept {
    if(!current||!current->watched)return;
    char actual[128]{},expected[128]{};
    Text(actual,gpr(c,Gpr::Rax));Text(expected,Read<std::uintptr_t>(gpr(c,Gpr::Rbp)-0x11));
    if(std::strcmp(actual,expected)==0)return;
    ++current->mismatches;
    if(current->mismatches>2)return;
    char event[300]{};std::snprintf(event,sizeof(event),"ROW_TAG_MISMATCH(expected=%s,actual=%s)",expected,actual);
    Write(event,current,false,int(Read<unsigned>(gpr(c,Gpr::Rbp)+0x6F)));
}
void Merge(MidContext& c) noexcept {
    // Before 2749940: RCX destination table, RDX patch. Names only, no guessed return ABI.
    if(!current)return;
    Write("PATCH_MERGE_ENTER",current,true);
    (void)c;
}
// Exact retail SHA-256 bdf8f9e4a11257a72b64c84700e284c29e4c4ccaf5b8d4bfa7d0b2a7294479f7
struct Pin {std::uintptr_t rva;std::array<unsigned char,16> bytes;};
constexpr Pin pins[]{
    {0xD18490,{0x48,0x89,0x5C,0x24,0x08,0x48,0x89,0x74,0x24,0x10,0x55,0x57,0x41,0x56,0x48,0x8B}},
    {0xD183A8,{0x48,0x89,0x5C,0x24,0x08,0x48,0x89,0x74,0x24,0x10,0x48,0x89,0x7C,0x24,0x18,0x41}},
    {0xD1A11C,{0x48,0x89,0x5C,0x24,0x08,0x48,0x89,0x74,0x24,0x10,0x57,0x41,0x56,0x41,0x57,0x48}},
    {0xD1A142,{0x48,0x8B,0x3D,0x07,0x37,0xC1,0x03,0x4C,0x8D,0x05,0x48,0xA0,0xD3,0x02,0x48,0x8B}},
    {0xD1A35F,{0x84,0xC0,0x74,0x4B,0x49,0x8B,0x4F,0x18,0x4C,0x8D,0x84,0x24,0xA8,0x00,0x00,0x00}},
    {0x71A48E,{0x48,0x8B,0x4D,0xEF,0x48,0x2B,0xC1,0x44,0x0F,0xB6,0x01,0x0F,0xB6,0x14,0x01,0x44}},
    {0x2749940,{0x4C,0x89,0x4C,0x24,0x20,0x4C,0x89,0x44,0x24,0x18,0x48,0x89,0x54,0x24,0x10,0x53}},
};
bool Pins() noexcept {
    __try {
        const auto* dos=reinterpret_cast<IMAGE_DOS_HEADER*>(base);
        if(dos->e_magic!=IMAGE_DOS_SIGNATURE)return false;
        const auto* nt=reinterpret_cast<IMAGE_NT_HEADERS64*>(base+dos->e_lfanew);
        if(nt->Signature!=IMAGE_NT_SIGNATURE||nt->FileHeader.TimeDateStamp!=0x6A350E20)return false;
        for(const auto& pin:pins)if(std::memcmp(reinterpret_cast<void*>(base+pin.rva),pin.bytes.data(),pin.bytes.size()))return false;
        return true;
    }__except(EXCEPTION_EXECUTE_HANDLER){return false;}
}
template<class Fn> bool Inline(unsigned i,std::uintptr_t rva,Fn replacement,Fn& original,const char* name) {
    Options options{};options.fail_if_already_hooked=true;
    auto h=inline_at(InlineRequest{name,Target{DetourModKit::Address{base+rva}},options},replacement);
    if(!h)return false;hooks[i]=std::move(*h);original=hooks[i]->original<Fn>();
    return original&&hooks[i]->enable();
}
bool Mid(unsigned i,std::uintptr_t rva,void(*callback)(MidContext&) noexcept,const char* name) {
    Options options{};options.fail_if_already_hooked=true;
    auto h=mid_at(MidRequest{name,Target{DetourModKit::Address{base+rva}},options},callback);
    if(!h)return false;hooks[i]=std::move(*h);return bool(hooks[i]->enable());
}
}
bool Install() noexcept {
    if(ready.load())return true;
    Write("WORKER_START",nullptr,true);
    base=reinterpret_cast<std::uintptr_t>(GetModuleHandleW(L"WHGame.dll"));
    if(!base||!retail_build112::Verify(base)||!Pins()) {Write("UNAVAILABLE_MODULE_OR_RETAIL_PINS",nullptr,false);return false;}
    // No waiting for a device, OpenXR, a save or the first render. If ASI loading
    // itself occurred after database loading, the trace says installed but has
    // no TABLE_BEGIN records; it must not be described as a successful load test.
    if(!Inline(0,0xD18490,&Load,load,"r59c4_table_load")||
       !Inline(1,0xD183A8,&Dispatch,dispatch,"r59c4_serializer_dispatch")||
       !Inline(2,0xD1A11C,&Xml,xml,"r59c4_xml_table")||
       !Mid(3,0xD1A142,Path,"r59c4_xml_path")||
       !Mid(4,0xD1A35F,Rows,"r59c4_xml_rows_return")||
       !Mid(5,0x71A48E,RowTag,"r59c4_row_tag")||
       !Mid(6,0x2749940,Merge,"r59c4_patch_merge")) {
        Write("INSTALL_FAILED",nullptr,false);Uninstall();return false;}
    ready.store(true);Write("INSTALLED_SHA_TIMESTAMP_PINS",nullptr,true);return true;
}
void Uninstall() noexcept {
    ready.store(false);
    for(int i=6;i>=0;--i)if(hooks[i]){(void)hooks[i]->disable();hooks[i].reset();}
    Write("SHUTDOWN",nullptr,true,int(failures.load())); // shared sink remains process-owned
}
}
