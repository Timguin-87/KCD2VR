// Execute the production TouchBindings::Update route. Never inject OS input.
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>
#include <cassert>
#include <cstdio>
#include <cstring>
static ULONGLONG testNow=1000;
static unsigned mapKeyEvents=0,inventoryKeyEvents=0,gestureReceipts=0;
static ULONGLONG WINAPI TestMapClock(){return testNow;}
static UINT WINAPI TestMapSend(UINT count,LPINPUT inputs,int){
 for(UINT n=0;n<count;++n)if(inputs[n].type==INPUT_KEYBOARD&&
  (inputs[n].ki.wVk=='M'||inputs[n].ki.wScan==MapVirtualKeyW('M',MAPVK_VK_TO_VSC)))++mapKeyEvents;
 for(UINT n=0;n<count;++n)if(inputs[n].type==INPUT_KEYBOARD&&inputs[n].ki.wScan==MapVirtualKeyW('I',MAPVK_VK_TO_VSC))++inventoryKeyEvents;
 return count;
}
#define GetTickCount64 TestMapClock
#define SendInput TestMapSend
#include "../src/subsystems/hud_ui_controls/runtime/touch_bindings.cpp"
#undef SendInput
#undef GetTickCount64
namespace kcd2vr::diagnostic_trace {
void Write(const char* s) noexcept {if(std::strstr(s,"H25 MAP_GESTURE"))++gestureReceipts;}
}
int main(){
 using namespace kcd2vr;using namespace held_map_m1;
 TouchBindings touch;touch.SetEnabled(true);touch.SetNativeGamepadActions(true);touch.SetNativeGamepadAxes(true,true);
 ControllerState left{},right{};left.active=right.active=true;right.quick_access_touch=true;left.thumb_y=1;
 auto step=[&](ULONGLONG t){testNow=t;return touch.Update(left,right,false,false,false,false);};
 Enable(true);PublishAllowed(true,1000);
 assert(!step(1000).toggle_inventory_comfort&&!Requested());
 PublishAllowed(true,1999);assert(!step(1999).toggle_inventory_comfort&&!Requested());
 PublishAllowed(true,2000);assert(!step(2000).toggle_inventory_comfort&&Requested()&&mapKeyEvents==0&&inventoryKeyEvents==0&&gestureReceipts==1);
 assert(!step(2100).toggle_inventory_comfort&&Requested());
 touch.Reset();step(2110);PublishAllowed(true,2120);step(3120);assert(Requested()&&inventoryKeyEvents==0);
 right.quick_access_touch=false;step(3200);right.quick_access_touch=true;step(3300);
 PublishAllowed(true,4300);assert(!step(4300).toggle_inventory_comfort&&!Requested()&&Present31(4300)&&inventoryKeyEvents==0);
 right.quick_access_touch=false;step(4400);right.quick_access_touch=true;step(4500);
 PublishAllowed(true,5500);step(5500);assert(Requested()&&inventoryKeyEvents==0&&mapKeyEvents==0);
 // A carried chord cannot repeat within one Update instance.
 step(5600);assert(Requested());
 // Authored minigames retain native controls.
 right.quick_access_touch=false;step(5700);touch.SetNativeMinigameOwnership162r1(true);
 right.quick_access_touch=true;step(5800);step(6900);assert(inventoryKeyEvents==0&&mapKeyEvents==0);
 touch.SetNativeMinigameOwnership162r1(false);right.quick_access_touch=false;step(7000);
 Enable(false);right.quick_access_touch=true;step(7100);
 assert(step(8100).toggle_map_comfort&&mapKeyEvents==2&&!Requested());
 assert(!step(9200).toggle_map_comfort&&mapKeyEvents==2);
 std::puts("PASS production direct native parchment shortcut: no Inventory keys, 1000ms hold, carried-chord/reset exclusion, minigame exclusion and disabled-option fallback");
}
