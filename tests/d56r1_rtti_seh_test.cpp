// Actual-source SEH probe: only WHGame module lookup is a synthetic leaf.
// Run one case per process so a compiler SEH regression cannot hide later cases.
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>

static std::uintptr_t g_fake_base = 0;
static HMODULE TestGetModuleHandleA(const char*) noexcept {
    return reinterpret_cast<HMODULE>(g_fake_base);
}
#define GetModuleHandleA TestGetModuleHandleA
namespace actual {
#include "d56r1_rtti_actual.inc"
}
#undef GetModuleHandleA

// Inline expansion at a real callsite remains enabled inside these boundaries.
__declspec(noinline) static const char* ReadName(void* p, char* out, std::size_t n) noexcept {
    return actual::ObjectRttiName(p, out, n);
}
__declspec(noinline) static std::uintptr_t ReadVtable(void* p) noexcept {
    return actual::ObjectVtableRva(p);
}
static LONG WINAPI Unhandled(EXCEPTION_POINTERS* e) {
    std::fprintf(stderr, "UNHANDLED 0x%08lx at %p\n",
                 e->ExceptionRecord->ExceptionCode, e->ExceptionRecord->ExceptionAddress);
    std::fflush(stderr);
    // A nonzero exit preserves failure evidence and prevents Windows error UI.
    TerminateProcess(GetCurrentProcess(), e->ExceptionRecord->ExceptionCode);
    return EXCEPTION_EXECUTE_HANDLER;
}

int main(int argc, char** argv) {
    SetErrorMode(SEM_FAILCRITICALERRORS | SEM_NOGPFAULTERRORBOX | SEM_NOOPENFILEERRORBOX);
    SetUnhandledExceptionFilter(Unhandled);
    if (argc != 2) return 2;
    SYSTEM_INFO sys{}; GetSystemInfo(&sys);
    const auto page = static_cast<std::size_t>(sys.dwPageSize);
    auto* memory = static_cast<unsigned char*>(VirtualAlloc(nullptr, page * 5,
        MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE));
    if (!memory) return 3;
    DWORD prior = 0;
    if (!VirtualProtect(memory + page * 2, page, PAGE_NOACCESS, &prior)) return 4;
    g_fake_base = reinterpret_cast<std::uintptr_t>(memory);
    auto* object = reinterpret_cast<std::uintptr_t*>(memory + 0x100);
    const auto vt = g_fake_base + 0x208;
    auto* col_slot = reinterpret_cast<std::uintptr_t*>(memory + 0x200);
    auto* type_rva = reinterpret_cast<std::uint32_t*>(memory + 0x30C);
    *object = vt; *col_slot = g_fake_base + 0x300; *type_rva = 0x400;
    std::strcpy(reinterpret_cast<char*>(memory + 0x410), ".?AVExampleEntity@@");
    void* input = object;
    char out[64]; std::memset(out, 'Z', sizeof(out));
    std::size_t out_size = sizeof(out);
    const char* expected = "";
    std::uintptr_t expected_rva = 0;
    const char* name = argv[1];
    bool vtable = false, null_out = false, tiny = false;
    if (!std::strcmp(name, "rtti_valid")) expected = ".?AVExampleEntity@@";
    else if (!std::strcmp(name, "rtti_truncate")) { out_size = 8; expected = ".?AVExa"; }
    else if (!std::strcmp(name, "rtti_null")) input = nullptr;
    else if (!std::strcmp(name, "rtti_object_noaccess")) input = memory + page * 2;
    else if (!std::strcmp(name, "rtti_object_split_read")) input = memory + page * 2 - 4;
    else if (!std::strcmp(name, "rtti_null_vtable")) *object = 0;
    else if (!std::strcmp(name, "rtti_vtable_noaccess")) *object = g_fake_base + page * 2 + sizeof(void*);
    else if (!std::strcmp(name, "rtti_null_col")) *col_slot = 0;
    else if (!std::strcmp(name, "rtti_col_noaccess")) *col_slot = g_fake_base + page * 2;
    else if (!std::strcmp(name, "rtti_col_split_read")) *col_slot = g_fake_base + page * 2 - 14;
    else if (!std::strcmp(name, "rtti_zero_type")) *type_rva = 0;
    else if (!std::strcmp(name, "rtti_no_module")) g_fake_base = 0;
    else if (!std::strcmp(name, "rtti_name_noaccess")) *type_rva = static_cast<std::uint32_t>(page * 2 - 0x10);
    else if (!std::strcmp(name, "rtti_name_crosses_guard") || !std::strcmp(name, "rtti_bounded_name_at_guard")) {
        *type_rva = static_cast<std::uint32_t>(page * 2 - 0x12);
        memory[page * 2 - 2] = 'A'; memory[page * 2 - 1] = 'B';
        if (!std::strcmp(name, "rtti_bounded_name_at_guard")) { out_size = 2; expected = "A"; }
    }
    else if (!std::strcmp(name, "rtti_null_output")) null_out = true;
    else if (!std::strcmp(name, "rtti_tiny_output")) { tiny = true; out_size = 1; }
    else if (!std::strcmp(name, "vtable_valid")) { vtable = true; expected_rva = 0x208; }
    else if (!std::strcmp(name, "vtable_null")) { vtable = true; input = nullptr; }
    else if (!std::strcmp(name, "vtable_noaccess")) { vtable = true; input = memory + page * 2; }
    else if (!std::strcmp(name, "vtable_split_read")) { vtable = true; input = memory + page * 2 - 4; }
    else if (!std::strcmp(name, "vtable_outside_image")) { vtable = true; *object = g_fake_base + 0x08000000ull; }
    else if (!std::strcmp(name, "vtable_before_image")) { vtable = true; *object = g_fake_base - 1; }
    else if (!std::strcmp(name, "vtable_no_module")) { vtable = true; g_fake_base = 0; }
    else { std::fprintf(stderr, "Unknown case: %s\n", name); return 2; }
    bool pass;
    if (vtable) pass = ReadVtable(input) == expected_rva;
    else {
        const auto* result = ReadName(input, null_out ? nullptr : out, out_size);
        pass = result && !std::strcmp(result, expected);
        if (!null_out && !tiny) pass = pass && result == out;
        if (tiny) pass = pass && out[0] == 'Z';
    }
    std::printf("%s %s\n", pass ? "PASS" : "FAIL", name);
    VirtualFree(memory, 0, MEM_RELEASE);
    return pass ? 0 : 1;
}
