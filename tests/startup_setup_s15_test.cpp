#include "../src/subsystems/base/startup_setup_s15.inl"
#include <cassert>
#include <iostream>
using namespace kcd2vr::startup_s15;
static std::string bytes(const fs::path& p){std::ifstream f(p,std::ios::binary);return {std::istreambuf_iterator<char>(f),{}};}
static void put(const fs::path& p,const std::string& s){std::ofstream f(p,std::ios::binary);f<<s;}
int main(){
 wchar_t dir[MAX_PATH]{};GetTempPathW(MAX_PATH,dir);auto root=fs::path(dir)/(L"KCD2VR_S15_TEST_"+std::to_wstring(GetCurrentProcessId()));fs::create_directories(root);
 struct Clean{fs::path p;~Clean(){fs::remove_all(p);}} clean{root};
 State s{};s.root=root;s.ini=root/L"KCD2VR.ini";
 const std::string ini="; keep comment\r\n[Settings]\r\nVrStereoExperiment=0\r\nfirst_start=1\r\nVrRenderScale=0.9\r\nVrSourceWidth=1800\r\nVrSourceHeight=1700\r\nVrGameFov=101\r\nVrPresentationZoom=1.07\r\nCustomUnrelated=hello\r\n[Calibration]\r\nHeight=1.83\r\n";
 put(s.ini,ini);for(const auto& f:fields)s.initial.push_back(Read(s.ini,f));s.values=s.initial;
 double v=0;assert(Parse(L"0.6",fields[0],v)&&Parse(L"1.3",fields[0],v));
 for(const auto* bad:{L"",L"nan",L"inf",L"1.31",L"0.59",L"1evil"})assert(!Parse(bad,fields[0],v));
 assert(!Parse(L"1800.1",fields[1],v)&&!Parse(L"4097",fields[1],v));
 s.values[0]=L"1.1";s.values[3]=L"100";s.values[4]=L"140";assert(SaveIni(s,true));
 assert(bytes(s.ini)==ini); // running game's loaded configuration is untouched
 assert(Read(PendingPath(s.ini),fields[0])==L"1.1");
 assert(Read(PendingPath(s.ini),fields[3])==L"95"&&Read(PendingPath(s.ini),fields[4])==L"95");
 assert(GetPrivateProfileIntW(L"Settings",L"VrCorrectedCenterScaleFovDegrees",0,PendingPath(s.ini).c_str())==95);
 // Same native profile writer used by SaveMenuSettings may still save OLD live
 // settings after the GUI. Pending values survive; fresh unrelated calibration
 // is retained by the merge BEFORE the next LoadConfig.
 assert(WritePrivateProfileStringW(L"Settings",L"VrRenderScale",L"0.9",s.ini.c_str()));
 assert(WritePrivateProfileStringW(L"Calibration",L"NewFineTune",L"0.123",s.ini.c_str()));
 const auto beforeApply=bytes(s.ini);assert(ApplyPending(s.ini));
 assert(GetPrivateProfileIntW(L"Settings",L"first_start",9,s.ini.c_str())==0);
 assert(Read(s.ini,fields[0])==L"1.1"&&Read(s.ini,fields[3])==L"95");
 auto out=bytes(s.ini);assert(out.find("keep comment")!=std::string::npos&&out.find("CustomUnrelated=hello")!=std::string::npos&&out.find("Height=1.83")!=std::string::npos&&out.find("NewFineTune=0.123")!=std::string::npos);
 assert(bytes(fs::path(s.ini.wstring()+L".pre-setup.bak"))==beforeApply);
 assert(!fs::exists(PendingPath(s.ini)));
 assert(SaveIni(s,true));assert(WritePrivateProfileStringW(L"Settings",L"VrSourceVerticalFovDegrees",nullptr,PendingPath(s.ini).c_str()));
 assert(WritePrivateProfileStringW(L"Settings",L"VrHorizontalFov",L"110",PendingPath(s.ini).c_str()));
 assert(ApplyPending(s.ini)&&Read(s.ini,fields[3])==Read(s.ini,fields[4]));
 const auto after=out;s.values[0]=L"1.2";assert(SaveIni(s,false));assert(Read(s.ini,fields[0])==L"1.1");assert(ApplyPending(s.ini));assert(Read(s.ini,fields[0])==L"1.1");
 // Cleanup failure/replayed file never reapplies old graphics after an intervening
 // deliberate in-game setting change.
 s.values[0]=L"1.15";assert(SaveIni(s,true));const auto pendingReplay=bytes(PendingPath(s.ini));assert(ApplyPending(s.ini));
 assert(WritePrivateProfileStringW(L"Settings",L"VrRenderScale",L"1.2",s.ini.c_str()));put(PendingPath(s.ini),pendingReplay);
 assert(ApplyPending(s.ini)&&Read(s.ini,fields[0])==L"1.2");
 const std::string cfg="; keep width comment\r\nr_Width = 1000\r\nr_width=1200\r\nr_Height=900\r\nr_Fullscreen=0\r\ncl_fov=110\r\nCL_FOV=112\r\ncl_hfov=120\r\ncustom_key=7\r\n";put(root/L"user.cfg",cfg);
 s.values[1]=L"1800";s.values[2]=L"1700";assert(SaveSource(s));out=bytes(root/L"user.cfg");assert(out.find("r_Fullscreen=0")!=std::string::npos&&out.find("custom_key=7")!=std::string::npos&&out.find("r_Width = 1800")!=std::string::npos&&out.find("r_Height = 1700")!=std::string::npos&&out.find("r_width=1200")==std::string::npos);assert(bytes(root/L"user.cfg.pre-setup.bak")==cfg);
 assert(out.find("cl_fov = 95")!=std::string::npos&&out.find("cl_fov=110")==std::string::npos&&out.find("CL_FOV=112")==std::string::npos&&out.find("cl_hfov=120")==std::string::npos);
 // Validated scientific/hex integer dimensions must persist their numeric
 // value consistently through BOTH source cfg and next-start INI.
 for(const auto* text:{L"1e3",L"0x400"}){s.values[1]=text;s.values[2]=L"1700";double parsed=0;assert(Parse(text,fields[1],parsed));
  assert(SaveSource(s));assert(bytes(root/L"user.cfg").find("r_Width = "+std::to_string(int(parsed)))!=std::string::npos);
  assert(SaveIni(s,true));assert(Read(PendingPath(s.ini),fields[1])==std::to_wstring(int(parsed)));assert(ApplyPending(s.ini));}
 auto invalid=s;invalid.root.clear();assert(!SaveSource(invalid));invalid.ini=root/L"missing"/L"bad.ini";assert(!SaveIni(invalid,true));
 // Actual Win32 controls and production command handler, on a hidden OWNED
 // test window only. No launcher export, Steam, game, runtime or installed INI.
 WNDCLASSW wc{};wc.lpfnWndProc=Proc;wc.hInstance=GetModuleHandleW(nullptr);wc.hbrBackground=reinterpret_cast<HBRUSH>(COLOR_WINDOW+1);wc.lpszClassName=L"KCD2VR_S15_HIDDEN_TEST";assert(RegisterClassW(&wc));
 s.font=CreateFontW(-16,0,0,0,FW_NORMAL,FALSE,FALSE,FALSE,DEFAULT_CHARSET,OUT_DEFAULT_PRECIS,CLIP_DEFAULT_PRECIS,DEFAULT_QUALITY,DEFAULT_PITCH,L"Segoe UI");
 s.launcher=true;s.done=s.start=false;s.initial.clear();for(const auto& f:fields)s.initial.push_back(Read(s.ini,f));
 auto w=CreateWindowW(wc.lpszClassName,L"hidden test",WS_OVERLAPPED,0,0,600,520,nullptr,nullptr,wc.hInstance,&s);assert(w&&GetDlgItem(w,100)&&GetDlgItem(w,106));

 // Actual dependent dropdown notifications for every named headset / scale.
 assert(GetDlgItem(w,headsetControl)&&GetDlgItem(w,resolutionControl));
 assert(SendDlgItemMessageW(w,headsetControl,CB_GETCOUNT,0,0)==std::size(headsets));
 auto choose=[&](int id,int row){SendDlgItemMessageW(w,id,CB_SETCURSEL,row,0);SendMessageW(w,WM_COMMAND,MAKEWPARAM(id,CBN_SELCHANGE),0);};
 auto number=[&](int id){wchar_t value[128]{};GetDlgItemTextW(w,id,value,128);return std::wstring(value);};
 assert(!IsWindowEnabled(GetDlgItem(w,103))&&!IsWindowEnabled(GetDlgItem(w,104)));
 SetDlgItemTextW(w,103,L"95");assert(number(104)==L"95");
 const auto runtimeScale=number(100);assert(HeadsetIndex(L"unknown-device")==0);
 for(int h=1;h<int(std::size(headsets));++h){choose(headsetControl,h);
  assert(s.headset==h&&SendDlgItemMessageW(w,resolutionControl,CB_GETCOUNT,0,0)==9);
  for(int row=1;row<=8;++row){choose(resolutionControl,row);int width=0,height=0;assert(PresetSize(h,row,width,height));
   assert(number(101)==std::to_wstring(width)&&number(102)==std::to_wstring(height));
   wchar_t label[128]{};SendDlgItemMessageW(w,resolutionControl,CB_GETLBTEXT,row,reinterpret_cast<LPARAM>(label));
   assert(std::wstring(label)==PresetLabel(h,row));assert((std::wstring(label).find(L"native")!=std::wstring::npos)==(row==5));
   assert(number(100)==runtimeScale); // no accidental double scaling
  }
 }
 choose(headsetControl,3);choose(resolutionControl,5);
 assert(number(101)==L"2064"&&number(102)==L"2208");
 choose(headsetControl,6);assert(number(101)==L"2000"&&number(102)==L"2040");
 choose(resolutionControl,1);choose(headsetControl,7);
 assert(number(101)==L"1296"&&number(102)==L"1296"); // preserve 0.6x while changing headset
 SetDlgItemTextW(w,101,L"1919");assert(SendDlgItemMessageW(w,resolutionControl,CB_GETCURSEL,0,0)==0);
 choose(headsetControl,0);assert(SendDlgItemMessageW(w,resolutionControl,CB_GETCOUNT,0,0)==1&&number(101)==L"1919");
 assert(PresetLabel(0,5)==L"Custom (use width / height below)");
 int ignoredWidth=0,ignoredHeight=0;assert(!PresetSize(-1,5,ignoredWidth,ignoredHeight)&&!PresetSize(1,0,ignoredWidth,ignoredHeight)&&!PresetSize(1,9,ignoredWidth,ignoredHeight));
 put(root/L"user.cfg","custom_key=7\r\ncl_fov=110\r\n");
 SendMessageW(w,WM_COMMAND,201,0);assert(s.done&&s.start&&!IsWindow(w));
 assert(bytes(root/L"user.cfg").find("cl_fov = "+std::string(s.initial[3].begin(),s.initial[3].end()))!=std::string::npos);
 assert(bytes(root/L"user.cfg").find("cl_fov=110")==std::string::npos);
 s.done=s.start=false;w=CreateWindowW(wc.lpszClassName,L"hidden test",WS_OVERLAPPED,0,0,600,520,nullptr,nullptr,wc.hInstance,&s);assert(w);SetDlgItemTextW(w,103,L"102");SetDlgItemTextW(w,101,L"1e3");SetDlgItemTextW(w,102,L"0x400");SendMessageW(w,WM_COMMAND,IDOK,0);assert(s.done&&s.start&&Read(PendingPath(s.ini),fields[1])==L"1000"&&Read(PendingPath(s.ini),fields[2])==L"1024"&&Read(PendingPath(s.ini),fields[3])==L"95"&&ApplyPending(s.ini)&&Read(s.ini,fields[3])==L"95");
 // Save selected headset metadata and concrete pixels through the real pending
 // profile merger. Reopening detects the matching native preset without edits.
 s.done=s.start=false;w=CreateWindowW(wc.lpszClassName,L"hidden test",WS_OVERLAPPED,0,0,600,616,nullptr,nullptr,wc.hInstance,&s);assert(w);
 choose(headsetControl,3);choose(resolutionControl,5);SendMessageW(w,WM_COMMAND,IDOK,0);
 assert(s.done&&s.start&&ReadHeadset(PendingPath(s.ini))==3&&ApplyPending(s.ini)&&ReadHeadset(s.ini)==3);
 assert(Read(s.ini,fields[1])==L"2064"&&Read(s.ini,fields[2])==L"2208");
 s.initial.clear();for(const auto& f:fields)s.initial.push_back(Read(s.ini,f));s.headset=ReadHeadset(s.ini);
 s.done=s.start=false;w=CreateWindowW(wc.lpszClassName,L"hidden test",WS_OVERLAPPED,0,0,600,616,nullptr,nullptr,wc.hInstance,&s);assert(w);
 assert(SendDlgItemMessageW(w,headsetControl,CB_GETCURSEL,0,0)==3&&SendDlgItemMessageW(w,resolutionControl,CB_GETCURSEL,0,0)==5);
 RECT lastEdit{},balancedRect{},buttons{};GetWindowRect(GetDlgItem(w,106),&lastEdit);GetWindowRect(GetDlgItem(w,200),&balancedRect);GetWindowRect(GetDlgItem(w,IDOK),&buttons);
 MapWindowPoints(nullptr,w,reinterpret_cast<POINT*>(&lastEdit),2);MapWindowPoints(nullptr,w,reinterpret_cast<POINT*>(&balancedRect),2);MapWindowPoints(nullptr,w,reinterpret_cast<POINT*>(&buttons),2);
 assert(lastEdit.bottom<balancedRect.top&&balancedRect.bottom<buttons.top&&buttons.bottom<=616);

 SendMessageW(w,WM_COMMAND,IDCANCEL,0);assert(s.done&&!s.start);
 // Real viewport/child movement in representative short logical work areas
 // (720p / 768p at125%): screen fitting includes frame and reserved work area.
 for(const RECT work:{RECT{100,40,1124,616},RECT{-900,60,193,674}}){
  const RECT frame=FitWindow(work);assert(frame.left>=work.left+8&&frame.top>=work.top+8);
  assert(frame.right<=work.right-8&&frame.bottom<=work.bottom-8);
  s.done=s.start=false;s.scrollY=s.wheelDelta=0;
  w=CreateWindowW(wc.lpszClassName,L"short viewport",setupStyle,frame.left,frame.top,
   frame.right-frame.left,frame.bottom-frame.top,nullptr,nullptr,wc.hInstance,&s);assert(w);
  auto visible=[&](int id){RECT child{},client{};GetWindowRect(GetDlgItem(w,id),&child);MapWindowPoints(nullptr,w,reinterpret_cast<POINT*>(&child),2);GetClientRect(w,&client);return child.top>=0&&child.bottom<=client.bottom;};
  assert(s.scrollY==0&&!visible(IDOK));
  SendMessageW(w,WM_VSCROLL,SB_BOTTOM,0);assert(s.scrollY>0&&visible(IDOK)&&visible(IDCANCEL));
  SendMessageW(w,WM_VSCROLL,SB_TOP,0);assert(s.scrollY==0&&visible(headsetControl));
  RevealControl(w,s,GetDlgItem(w,IDOK));assert(s.scrollY>0&&visible(IDOK)); // production Tab-focus reveal
  SendMessageW(w,WM_VSCROLL,SB_TOP,0);
  SendMessageW(w,WM_MOUSEWHEEL,MAKEWPARAM(0,WORD(-WHEEL_DELTA)),0);assert(s.scrollY>0);
  // Actual growth re-clamps the scroll offset and restores original child coords.
  SetWindowPos(w,nullptr,0,0,620,730,SWP_NOMOVE|SWP_NOZORDER);assert(s.scrollY==0&&visible(IDOK));
  RECT button{};GetWindowRect(GetDlgItem(w,IDOK),&button);MapWindowPoints(nullptr,w,reinterpret_cast<POINT*>(&button),2);assert(button.top==558);
  SendMessageW(w,WM_COMMAND,IDCANCEL,0);assert(s.done&&!s.start);
 }
 DeleteObject(s.font);UnregisterClassW(wc.lpszClassName,wc.hInstance);
 std::cout<<"H21 startup pending-INI ownership, canonical dimensions, replay/backup/calibration, keep/cancel, actual dropdowns and short-work-area scroll/focus/resize PASS; no game/Steam launched\n";
}
