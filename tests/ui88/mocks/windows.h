#pragma once
// TEST DOUBLE ONLY. This is not the Windows SDK and does not validate Win32 ABI/SEH.
#include <cstdint>
#include <cstddef>
#include <cstring>
#include <strings.h>
#define WINAPI
#define __fastcall
#define __try try
#define __except(x) catch(...)
#define EXCEPTION_EXECUTE_HANDLER 1
#define _stricmp strcasecmp
using BYTE=std::uint8_t;using WORD=std::uint16_t;using DWORD=std::uint32_t;using SHORT=std::int16_t;using LONG=std::int32_t;using UINT=unsigned;using BOOL=int;using HWND=void*;using HMODULE=void*;using FARPROC=void(*)();
struct POINT{LONG x,y;};struct RECT{LONG left,top,right,bottom;};
struct KEYBDINPUT{WORD wVk{},wScan{};DWORD dwFlags{},time{};std::uintptr_t dwExtraInfo{};};
struct MOUSEINPUT{LONG dx{},dy{};DWORD mouseData{},dwFlags{},time{};std::uintptr_t dwExtraInfo{};};
struct INPUT{DWORD type{};MOUSEINPUT mi{};KEYBDINPUT ki{};};
struct IMAGE_DOS_HEADER{WORD e_magic{};LONG e_lfanew{};};
struct IMAGE_FILE_HEADER{DWORD TimeDateStamp{};};struct IMAGE_NT_HEADERS64{DWORD Signature{};IMAGE_FILE_HEADER FileHeader{};};
constexpr DWORD IMAGE_DOS_SIGNATURE=0x5A4D,IMAGE_NT_SIGNATURE=0x4550;
constexpr DWORD ERROR_SUCCESS=0,ERROR_BAD_ARGUMENTS=160,ERROR_DEVICE_NOT_CONNECTED=1167;
constexpr DWORD PAGE_READWRITE=4,INPUT_KEYBOARD=1,INPUT_MOUSE=0,KEYEVENTF_KEYUP=2;
constexpr DWORD MOUSEEVENTF_LEFTDOWN=2,MOUSEEVENTF_LEFTUP=4,MOUSEEVENTF_WHEEL=0x800;
constexpr int WHEEL_DELTA=120,VK_UP=0x26,VK_DOWN=0x28,VK_LEFT=0x25,VK_RIGHT=0x27,VK_RETURN=13,VK_ESCAPE=27,VK_SHIFT=16,VK_TAB=9;
BOOL VirtualProtect(void*,std::size_t,DWORD,DWORD*);
void* InterlockedCompareExchangePointer(void* volatile*,void*,void*);
HMODULE GetModuleHandleW(const wchar_t*);FARPROC GetProcAddress(HMODULE,const char*);
UINT SendInput(UINT,INPUT*,int);HWND GetForegroundWindow();std::uint64_t GetTickCount64();
BOOL GetClientRect(HWND,RECT*);BOOL ClientToScreen(HWND,POINT*);BOOL SetCursorPos(int,int);

#ifndef VK_SPACE
#define VK_SPACE 0x20
#endif

BOOL GetCursorPos(POINT*);
