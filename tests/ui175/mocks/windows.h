#pragma once
// Test-only supplement to the repository's existing host Win32 shim.
#include "../../syntax_shims/windows.h"
using HWND=void*;using LONG=std::int32_t;using BOOL=int;
struct POINT{LONG x,y;};struct RECT{LONG left,top,right,bottom;};
inline HWND ui175Foreground=reinterpret_cast<HWND>(1);
inline unsigned ui175Moves=0;
inline LONG ui175X=0,ui175Y=0;
inline LONG ui175OriginX=20,ui175OriginY=30,ui175Width=1920,ui175Height=1080;
inline HWND GetForegroundWindow(){return ui175Foreground;}
inline BOOL GetClientRect(HWND,RECT*r){*r={0,0,ui175Width,ui175Height};return 1;}
inline BOOL ClientToScreen(HWND,POINT*p){p->x+=ui175OriginX;p->y+=ui175OriginY;return 1;}
inline BOOL SetCursorPos(int x,int y){++ui175Moves;ui175X=x;ui175Y=y;return 1;}
inline BOOL GetCursorPos(POINT*p){p->x=ui175X;p->y=ui175Y;return 1;}
constexpr unsigned VK_DELETE=0x2e,VK_INSERT=0x2d,VK_HOME=0x24,VK_END=0x23,
    VK_PRIOR=0x21,VK_NEXT=0x22,VK_F13=0x7c,VK_F14=0x7d,VK_F15=0x7e;
struct LARGE_INTEGER{std::int64_t QuadPart=0;};
inline int QueryPerformanceCounter(LARGE_INTEGER*out){out->QuadPart=hostTickCount64*1000;return 1;}
inline int QueryPerformanceFrequency(LARGE_INTEGER*out){out->QuadPart=1000000;return 1;}
