#pragma once
#include <windows.h>
namespace kcd2vr::block_indicator_h24 {
inline constexpr const char* iniKey="VrShowBlockIndicator";
inline constexpr bool defaultEnabled=false;
inline bool visible(bool selected,bool nativeBlock,bool menu,bool calibration) noexcept {
 return selected&&nativeBlock&&!menu&&!calibration;
}
// Existing menu bitmap surface, text-only diagnostic. No input or renderer ownership.
// R120: shield=true (the shield box owns the pressed block) says SHIELD BLOCK in a wider label.
inline void draw(HDC dc,HFONT font,int width,int height,bool shield=false) noexcept {
 RECT panel{0,0,width,height};FillRect(dc,&panel,static_cast<HBRUSH>(GetStockObject(BLACK_BRUSH)));
 const auto old=SelectObject(dc,font);SetBkMode(dc,TRANSPARENT);SetTextColor(dc,RGB(250,230,145));
 RECT label{shield?400L:450L,560,shield?700L:650L,605};DrawTextW(dc,shield?L"SHIELD BLOCK":L"BLOCK",-1,&label,DT_CENTER|DT_VCENTER|DT_SINGLELINE);
 SelectObject(dc,old);
}
inline unsigned char keyedAlpha(unsigned char b,unsigned char g,unsigned char r) noexcept {return (b||g||r)?255u:0u;}
}
