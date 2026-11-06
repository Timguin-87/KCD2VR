#include "ui_menu_camera59c2.h"
#include "retail_build112.h"
#include "diagnostic_trace.h"
#include <DetourModKit/hook.hpp>
#include <windows.h>
#include <intrin.h>
#include <optional>
#include <cstring>

namespace kcd2vr::ui_menu_camera59c2 { namespace {
using ui_menu_transition59c2::Camera;
using Fn=void(__fastcall*)(void*,void*);
constexpr std::uintptr_t Resolver=0x9E5314,ApseOutput=0x14E40A0,ShopOutput=0x1630B60;
constexpr std::uintptr_t OutputReturn=0x9E53BF,ApseVtable=0x3B8F418,ShopVtable=0x3B8F490;
std::uintptr_t base=0;
Fn resolver=nullptr,apse=nullptr,shop=nullptr;
std::optional<DetourModKit::hook::Hook> hooks[3];
std::atomic<bool> installed{false};
ui_menu_transition59c2::Mailbox mailbox;
thread_local unsigned depth=0;
thread_local Camera selected=Camera::Other;
bool Is(void* p,std::uintptr_t vt) noexcept {
    __try {return p&&*static_cast<std::uintptr_t*>(p)==base+vt;}
    __except(EXCEPTION_EXECUTE_HANDLER){return false;}
}
void __fastcall Resolve(void* self,void* output) {
    if(!Is(self,0x3A5A280)){resolver(self,output);return;}
    struct Scope {
        Scope(){if(depth++==0)selected=Camera::Other;}
        ~Scope(){if(--depth==0)mailbox.Publish(selected);}
    } scope;
    resolver(self,output); // Native eligibility and output execute once, unchanged.
}
void __fastcall Apse(void* self,void* output) {
    const bool consumed=depth==1&&reinterpret_cast<std::uintptr_t>(_ReturnAddress())==base+OutputReturn&&Is(self,ApseVtable);
    apse(self,output);
    if(consumed)selected=Camera::Apse;
}
void __fastcall Shop(void* self,void* output) {
    const bool consumed=depth==1&&reinterpret_cast<std::uintptr_t>(_ReturnAddress())==base+OutputReturn&&Is(self,ShopVtable);
    shop(self,output);
    if(consumed)selected=Camera::Shop;
}
bool Pins() noexcept {
    __try {
        const auto* dos=reinterpret_cast<IMAGE_DOS_HEADER*>(base);
        if(dos->e_magic!=IMAGE_DOS_SIGNATURE)return false;
        const auto* nt=reinterpret_cast<IMAGE_NT_HEADERS64*>(base+dos->e_lfanew);
        if(nt->Signature!=IMAGE_NT_SIGNATURE||nt->FileHeader.TimeDateStamp!=0x6A350E20)return false;
        constexpr unsigned char r[]={0x48,0x89,0x5c,0x24,0x08,0x48,0x89,0x6c,0x24,0x10,0x48,0x89,0x74,0x24,0x18};
        constexpr unsigned char a[]={0xf3,0x0f,0x10,0x81,0x58,0x01,0x00,0x00};
        constexpr unsigned char s[]={0xf3,0x0f,0x10,0x81,0xdc,0x00,0x00,0x00};
        constexpr unsigned char call[]={0xff,0x50,0x08,0x8b,0x43,0x30};
        const auto* av=reinterpret_cast<std::uintptr_t*>(base+ApseVtable);
        const auto* sv=reinterpret_cast<std::uintptr_t*>(base+ShopVtable);
        return std::memcmp(reinterpret_cast<void*>(base+Resolver),r,sizeof(r))==0&&
            std::memcmp(reinterpret_cast<void*>(base+ApseOutput),a,sizeof(a))==0&&
            std::memcmp(reinterpret_cast<void*>(base+ShopOutput),s,sizeof(s))==0&&
            std::memcmp(reinterpret_cast<void*>(base+OutputReturn-3),call,sizeof(call))==0&&
            av[1]==base+ApseOutput&&av[2]==base+0x9E5BE8&&sv[1]==base+ShopOutput&&sv[2]==base+0x15A1020&&
            reinterpret_cast<std::uintptr_t*>(base+0x3A5A280)[1]==base+Resolver;
    } __except(EXCEPTION_EXECUTE_HANDLER){return false;}
}
bool Hook(unsigned i,std::uintptr_t rva,Fn detour,Fn& original,const char* name) {
    DetourModKit::hook::InlineRequest request{name,DetourModKit::hook::Target{DetourModKit::Address{base+rva}},DetourModKit::hook::Options{}};
    auto h=DetourModKit::hook::inline_at(std::move(request),detour);
    if(!h)return false;
    hooks[i]=std::move(*h);original=hooks[i]->original<Fn>();
    return original&&hooks[i]->enable();
}
}
bool Install() noexcept {
    if(installed.load(std::memory_order_acquire))return true;
    base=reinterpret_cast<std::uintptr_t>(GetModuleHandleW(L"WHGame.dll"));
    // Called on the existing startup/control path; SHA verification never occurs
    // inside the camera callback. All three hooks must install together.
    if(!base||!retail_build112::Verify(base)||!Pins())return false;
    if(!Hook(0,ApseOutput,&Apse,apse,"r59c2_apse_camera_output")||
       !Hook(1,ShopOutput,&Shop,shop,"r59c2_shop_camera_output")||
       !Hook(2,Resolver,&Resolve,resolver,"r59c2_ui_camera_resolver")){Uninstall();return false;}
    installed.store(true,std::memory_order_release);
    diagnostic_trace::Write("R59C2 MENU_CAMERA installed SHA256+timestamp+pins observer=SELECTED_NATIVE_OUTPUT writes=NONE");
    return true;
}
void Uninstall() noexcept {
    installed.store(false,std::memory_order_release);
    for(int i=2;i>=0;--i)if(hooks[i]){(void)hooks[i]->disable();hooks[i].reset();}
}
ui_menu_transition59c2::Observation Read() noexcept {return mailbox.Read(installed.load(std::memory_order_acquire));}
}
