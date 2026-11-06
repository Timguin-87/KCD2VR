#include "vr_menu.h"
#include "combat_menu49.h"
#include <cstdio>
#include <vector>
namespace bi=kcd2vr::block_indicator_h24;
static unsigned checks=0,failures=0;
static void ck(bool v,const char* what){++checks;failures+=!v;std::printf("%s %s\n",v?"PASS":"FAIL",what);}
int main(){
 kcd2vr::VrMenuState s{};ck(!s.show_block_indicator_h24&&!bi::defaultEnabled,"actual menu state defaults OFF");
 ck(!bi::visible(false,true,false,false)&&!bi::visible(true,false,false,false),"disabled or native clear is hidden");
 ck(bi::visible(true,true,false,false),"fresh native block selected in gameplay is shown");
 ck(!bi::visible(true,true,true,false)&&!bi::visible(true,true,false,true),"menu and calibration own presentation");
 ck(kcd2vr::combat_menu49::clean(),"first Experiments row is inside page and nonoverlapping");
 char dir[MAX_PATH]{},ini[MAX_PATH]{};GetTempPathA(MAX_PATH,dir);GetTempFileNameA(dir,"h24",0,ini);DeleteFileA(ini);
 ck(GetPrivateProfileIntA("Settings",bi::iniKey,bi::defaultEnabled?1:0,ini)==0,"missing persisted key defaults OFF");
 ck(WritePrivateProfileStringA("Settings",bi::iniKey,"1",ini)&&GetPrivateProfileIntA("Settings",bi::iniKey,0,ini)==1,"actual production INI key roundtrips ON");
 ck(WritePrivateProfileStringA("Settings",bi::iniKey,"0",ini)&&GetPrivateProfileIntA("Settings",bi::iniKey,1,ini)==0,"actual production INI key roundtrips OFF");DeleteFileA(ini);
 constexpr int width=1100,height=700;HDC dc=CreateCompatibleDC(nullptr);BITMAPINFO info{};info.bmiHeader.biSize=sizeof(BITMAPINFOHEADER);info.bmiHeader.biWidth=width;info.bmiHeader.biHeight=-height;info.bmiHeader.biPlanes=1;info.bmiHeader.biBitCount=32;info.bmiHeader.biCompression=BI_RGB;void* bits=nullptr;
 HBITMAP bmp=CreateDIBSection(dc,&info,DIB_RGB_COLORS,&bits,nullptr,0);auto oldBmp=SelectObject(dc,bmp);
 HFONT font=CreateFontW(-20,0,0,0,FW_BOLD,FALSE,FALSE,FALSE,DEFAULT_CHARSET,OUT_DEFAULT_PRECIS,CLIP_DEFAULT_PRECIS,ANTIALIASED_QUALITY,DEFAULT_PITCH|FF_DONTCARE,L"Segoe UI");
 bi::draw(dc,font,width,height);GdiFlush();auto* pixels=static_cast<unsigned char*>(bits);unsigned ink=0,outside=0;
 for(int y=0;y<height;++y)for(int x=0;x<width;++x){auto* p=pixels+4*(y*width+x);p[3]=bi::keyedAlpha(p[0],p[1],p[2]);if(p[3]){++ink;if(x<450||x>=650||y<560||y>=605)++outside;}}
 ck(ink>100&&ink<3000&&outside==0,"actual GDI draw contains only compact BLOCK label, no background or pointer");
 ck(bi::keyedAlpha(0,0,0)==0&&bi::keyedAlpha(1,0,0)==255,"production alpha makes black background fully transparent");
 FILE* f=nullptr;fopen_s(&f,"BLOCK_H24_BGRA.raw","wb");if(f){fwrite(bits,4,width*height,f);fclose(f);}else ck(false,"render receipt written");
 SelectObject(dc,oldBmp);DeleteObject(font);DeleteObject(bmp);DeleteDC(dc);
 std::printf("block_indicator_h24=%u/%u headset_execution=NONE\n",checks-failures,checks);return failures?1:0;
}
