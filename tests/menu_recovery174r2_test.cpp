#include <windows.h>
#include <cassert>
#include <vector>
#include <cstdio>
static std::vector<INPUT> sent;
static UINT InputSpy(UINT n,INPUT* p,int){sent.insert(sent.end(),p,p+n);return n;}
#define SendInput InputSpy
#include "../src/touch_bindings.cpp"
namespace kcd2vr::diagnostic_trace { void Write(const char*) noexcept {} } // V66.178R21: R3_EDGE trace
#undef SendInput
static unsigned inventoryDown(){unsigned n=0;for(auto& e:sent)if(e.type==INPUT_KEYBOARD&&e.ki.wScan=='I'&&!(e.ki.dwFlags&KEYEVENTF_KEYUP))++n;return n;}
int main(){
 using namespace kcd2vr;
 for(bool xi:{false,true}){
  TouchBindings b;ControllerState l{},r{};l.active=r.active=true;
  b.SetNativeGamepadActions(xi);b.Update(l,r,false);sent.clear();
  // Native inventory owns down; native close/Reset happens while still held.
  l.menu_down=true;b.ConsumeMenuUntilRelease174r2();b.Reset();
  for(int i=0;i<8;++i)assert(!b.Update(l,r,false).toggle_inventory_comfort);
  l.menu_down=false;assert(!b.Update(l,r,false).toggle_inventory_comfort);
  assert(inventoryDown()==0);
  l.menu_down=true;b.Update(l,r,false);l.menu_down=false;
  assert(b.Update(l,r,false).toggle_inventory_comfort&&inventoryDown()==1);
  assert(!b.Update(l,r,false).toggle_inventory_comfort&&inventoryDown()==1);
 }
 std::puts("PASS one inventory action per new click across native menu close and Reset; OS input replaced");
}
