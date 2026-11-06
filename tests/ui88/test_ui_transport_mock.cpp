// Includes an unmodified configure-time copy of the production adapter.
// The dependencies below are test doubles, not a Windows ABI/runtime certification.
#include "ui_menu_input.cpp"
#include <vector>
#include <string>
#include <iostream>
#include <cstdlib>
#include <limits>
namespace mock {
std::uint64_t time=1000;HWND foreground=reinterpret_cast<HWND>(1);
bool physical=false,failSend=false,failMove=false,failRect=false;
std::vector<INPUT> events;unsigned moves=0,passthrough=0;int lastX=0,lastY=0;
std::vector<std::string> logs;bool suspended=false;
}
BOOL VirtualProtect(void*,std::size_t,DWORD,DWORD* p){*p=PAGE_READWRITE;return 1;}
void* InterlockedCompareExchangePointer(void* volatile* p,void* after,void* before){auto*old=*p;if(old==before)*p=after;return old;}
HMODULE GetModuleHandleW(const wchar_t*){return nullptr;}FARPROC GetProcAddress(HMODULE,const char*){return nullptr;}
UINT SendInput(UINT n,INPUT* p,int){if(mock::failSend)return 0;for(unsigned i=0;i<n;++i)mock::events.push_back(p[i]);return n;}
HWND GetForegroundWindow(){return mock::foreground;}std::uint64_t GetTickCount64(){return mock::time;}
BOOL GetClientRect(HWND,RECT* r){*r={0,0,1920,1080};return !mock::failRect;}
BOOL ClientToScreen(HWND,POINT* p){p->x+=20;p->y+=30;return 1;}
BOOL GetCursorPos(POINT*p){p->x=mock::lastX;p->y=mock::lastY;return 1;}
namespace kcd2vr::flashui_probe {bool HudExperiment36Enabled() noexcept{return false;}}
BOOL SetCursorPos(int x,int y){if(mock::failMove)return 0;++mock::moves;mock::lastX=x;mock::lastY=y;return 1;}
namespace kcd2vr::diagnostic_trace{void Write(const char* s)noexcept{mock::logs.push_back(s?s:"");}}
namespace kcd2vr::prompt_device_lock{void SetSuspended(bool b)noexcept{mock::suspended=b;}}
namespace test {
using namespace kcd2vr::ui_menu_input;
unsigned assertions=0;
#define CHECK(expr) do {++assertions;if(!(expr)){std::cerr<<"FAIL "<<__LINE__<<": " #expr "\n";std::exit(1);}}while(0)
DWORD State(DWORD,XINPUT_STATE* s){++mock::passthrough;if(!s)return ERROR_BAD_ARGUMENTS;if(mock::physical){*s={};s->dwPacketNumber=900;s->Gamepad.wButtons=0x2000;return ERROR_SUCCESS;}return ERROR_DEVICE_NOT_CONNECTED;}
DWORD Caps(DWORD,DWORD,XINPUT_CAPABILITIES* c){if(mock::physical){*c={};return ERROR_SUCCESS;}return ERROR_DEVICE_NOT_CONNECTED;}
DWORD Set(DWORD,XINPUT_VIBRATION*){return ERROR_DEVICE_NOT_CONNECTED;}
void Reset(){
    g_state=State;g_caps=Caps;g_set=Set;g_present=false;g_installed=true;g_real_pad=false;g_native=false;g_seq=0;g_word0=0;g_word1=0;g_polls=0;g_seen_seq=0;++g_session;
    g_grip_rearm={};g_world_grip_blocked=0;g_was_captured=false;
    g_packet=0;g_last={};g_have_pad=false;g_owner={};g_was_menu=false;g_mouse_down=false;g_pointer_armed=false;g_have_pixel=false;g_neutral_exit=false;g_open_ms=0;g_poll_base=0;g_wheel_ms=0;g_drain_seq=0;g_draining=false;g_hand=-1;g_last_backend=-1;g_io_errors=0;std::memset(g_key_down,0,sizeof g_key_down);
    mock::time=1000;mock::foreground=reinterpret_cast<HWND>(1);mock::physical=false;mock::failSend=false;mock::failMove=false;mock::failRect=false;mock::events.clear();mock::moves=0;mock::passthrough=0;mock::logs.clear();
}
kcd2vr::ui_menu_input::Frame Menu(){kcd2vr::ui_menu_input::Frame f{};f.window=mock::foreground;f.menu=true;f.system_menu=true;f.context=1;f.left.active=f.right.active=true;return f;}
XINPUT_STATE Poll(){XINPUT_STATE s{};CHECK(GetState(0,&s)==ERROR_SUCCESS);return s;}
void Native(kcd2vr::ui_menu_input::Frame&f){Update(f);Poll();CHECK(Update(f).native_pad);CHECK(!mock::suspended);}
unsigned MouseEvents(DWORD flags){unsigned n=0;for(auto e:mock::events)if(e.type==INPUT_MOUSE&&e.mi.dwFlags==flags)++n;return n;}
void Tests(){
    Reset();XINPUT_STATE s{};CHECK(GetState(0,&s)==ERROR_DEVICE_NOT_CONNECTED);CHECK(mock::passthrough==1);g_installed=false;CHECK(!Install());g_installed=true;
    auto f=Menu();Native(f);CHECK(mock::moves==0);CHECK(mock::events.empty());
    auto a=Poll(),b=Poll();CHECK(a.dwPacketNumber==b.dwPacketNumber);
    f.left.x_down=true;f.left.thumb_x=.7f;f.right.thumb_x=-.8f;f.right.thumb_y=.9f;Update(f);a=Poll();CHECK(a.Gamepad.wButtons==(0x4000|0x100));CHECK(a.Gamepad.sThumbLX>22000);CHECK(a.Gamepad.sThumbRY>29000);CHECK(mock::events.empty());
    // Normal grip transfer defers while a native hold is down; no fabricated X-up.
    f.left.grip=.9f;CHECK(Update(f).hand==-1);CHECK(Poll().Gamepad.wButtons&0x4000);
    f.left.thumb_x=f.right.thumb_x=f.right.thumb_y=0;f.left.x_down=false;f.ray_hand=0;f.ray_hit=true;
    CHECK(Update(f).hand==-1);CHECK(g_draining);CHECK(mock::moves==0);CHECK(Poll().Gamepad.wButtons==0);
    CHECK(Update(f).hand==0);CHECK(mock::suspended);CHECK(mock::moves==1);
    for(int i=0;i<100;++i){Update(f);}CHECK(mock::moves==1);CHECK(MouseEvents(MOUSEEVENTF_LEFTDOWN)==0);
    f.left.trigger=.8f;Update(f);CHECK(MouseEvents(MOUSEEVENTF_LEFTDOWN)==1);
    for(int i=0;i<100;++i){Update(f);}CHECK(MouseEvents(MOUSEEVENTF_LEFTDOWN)==1);
    f.ray_hit=false;Update(f);CHECK(MouseEvents(MOUSEEVENTF_LEFTUP)==0);f.left.trigger=0;Update(f);CHECK(MouseEvents(MOUSEEVENTF_LEFTUP)==1);
    // Miss + pressed trigger never uses a stale point; reacquire requires trigger release.
    f.left.trigger=.8f;Update(f);f.ray_hit=true;Update(f);CHECK(MouseEvents(MOUSEEVENTF_LEFTDOWN)==1);
    f.left.trigger=0;Update(f);f.left.trigger=.8f;Update(f);CHECK(MouseEvents(MOUSEEVENTF_LEFTDOWN)==2);
    f.left.trigger=0;Update(f);f.left.grip=0;const auto moves=mock::moves;CHECK(Update(f).hand==-1);CHECK(!mock::suspended);for(int i=0;i<100;++i){Update(f);}CHECK(mock::moves==moves);
    // Native page/context changes retain only the physically held preexisting action.
    f.left.x_down=true;Update(f);CHECK(Poll().Gamepad.wButtons&0x4000);++f.context;Update(f);CHECK(Poll().Gamepad.wButtons&0x4000);
    f.right.a_down=true;Update(f);CHECK(!(Poll().Gamepad.wButtons&0x1000));f.left.x_down=false;Update(f);CHECK(Poll().Gamepad.wButtons==0);
    f.right.a_down=false;Update(f);f.right.a_down=true;Update(f);CHECK(Poll().Gamepad.wButtons==0x1000);
    // Inactive hand cannot retain stale buttons/analog values.
    f.right.active=false;f.right.aim_active=false;Update(f);CHECK(Poll().Gamepad.wButtons==0);CHECK(PolicyFrame(f).hands[1].trigger==0);
    // Actual pad takes priority; simulated pad cannot shadow it.
    mock::physical=true;a=Poll();CHECK(a.dwPacketNumber==900&&a.Gamepad.wButtons==0x2000);CHECK(!Update(f).native_pad);
    // Fullscreen foreground loss releases once and stops injection.
    Reset();f=Menu();Native(f);f.right.grip=1;f.ray_hand=1;f.ray_hit=true;Update(f);Poll();Update(f);f.right.trigger=1;Update(f);CHECK(g_mouse_down);
    mock::foreground=nullptr;Update(f);CHECK(!g_mouse_down);const auto before=mock::events.size();for(int i=0;i<10;++i){Update(f);}CHECK(mock::events.size()==before);
    // Pointer key hold is not pressed/released every frame; IO failures get counted.
    Reset();f=Menu();Native(f);f.right.grip=1;f.ray_hand=1;f.ray_hit=true;Update(f);Poll();Update(f);
    f.right.b_down=true;Update(f);const auto kb=mock::events.size();for(int i=0;i<100;++i){Update(f);}CHECK(mock::events.size()==kb);
    f.right.b_down=false;Update(f);CHECK(mock::events.size()==kb+1);
    mock::failMove=true;f.u=.9f;CHECK(!Update(f).cursor);CHECK(g_io_errors>0);mock::failMove=false;
    mock::failSend=true;f.right.trigger=1;Update(f);CHECK(!g_mouse_down);CHECK(g_io_errors>1);
    // Honest timeout fallback logs once; A has a focus-confirm route without mouse.
    Reset();f=Menu();Update(f);mock::time+=901;Update(f);CHECK(mock::suspended);CHECK(mock::logs.back().find("KEYBOARD_FALLBACK")!=std::string::npos);
    f.right.a_down=true;Update(f);CHECK(g_key_down[VK_RETURN]);const auto count=mock::events.size();for(int i=0;i<20;++i){Update(f);}CHECK(mock::events.size()==count);CHECK(mock::moves==0);
    f.right.a_down=false;Update(f);CHECK(!g_key_down[VK_RETURN]);
    // Root close blocks gameplay until physical inputs return to neutral.
    f.right.a_down=true;Update(f);f.menu=false;CHECK(Update(f).blocks_gameplay);f.right.a_down=false;CHECK(!Update(f).blocks_gameplay);
    // No malformed pointer/NaN values can become movement or a click.
    Reset();f=Menu();Native(f);f.right.grip=1;f.ray_hand=1;f.ray_hit=true;f.u=std::numeric_limits<float>::quiet_NaN();Update(f);Poll();CHECK(!Update(f).cursor);CHECK(mock::moves==0);
    // V66.89: loading/progress has a presentation but no controller/mouse UI owner.
    Reset();f=Menu();Native(f);f.menu=false;f.block_gameplay=true;f.consume_grips=true;
    f.right.grip=1;f.ray_hand=1;f.ray_hit=true;auto passive=Update(f);
    CHECK(passive.blocks_gameplay&&!passive.native_pad&&passive.hand==-1);
    CHECK(mock::moves==0);CHECK(!g_present);CHECK(WorldGripBlockedMask()==3);
    f.menu=true;CHECK(Update(f).hand==-1); // explicit input block has priority
    f.menu=false;f.block_gameplay=false;f.consume_grips=false;
    auto exit=Update(f);CHECK(!exit.blocks_gameplay);CHECK(exit.grip_rearm_mask==2);
    CHECK(WorldGripBlockedMask()==2); // grip latch does not freeze locomotion
    f.right.active=false;f.right.aim_active=false;f.right.grip=0;CHECK(Update(f).grip_rearm_mask==2);
    f.right.active=true;CHECK(Update(f).grip_rearm_mask==0);
    // A gamepad-only interface never enables the pointer, even with both grips held.
    Reset();f=Menu();f.allow_pointer=false;Native(f);f.left.grip=f.right.grip=1;
    f.ray_hand=1;f.ray_hit=true;CHECK(Update(f).hand==-1);CHECK(mock::moves==0);
    f.right.a_down=true;Update(f);CHECK(Poll().Gamepad.wButtons==0x1000);
    // UI->world release rearm is per hand; no extra T-pose or input reset required.
    Reset();f=Menu();Native(f);f.left.grip=1;f.ray_hand=0;f.ray_hit=true;Update(f);Poll();Update(f);
    f.menu=false;exit=Update(f);CHECK(!exit.blocks_gameplay);CHECK(exit.grip_rearm_mask==1);
    CHECK(WorldGripBlockedMask()==1);const auto noMove=mock::moves;
    f.right.grip=1;Update(f);CHECK(mock::moves==noMove);CHECK(WorldGripBlockedMask()==1);
    f.left.grip=0;CHECK(Update(f).grip_rearm_mask==0);
    // No wheel event at a stale hover target after a ray miss.
    Reset();f=Menu();Native(f);f.right.grip=1;f.ray_hand=1;f.ray_hit=true;Update(f);Poll();Update(f);
    const auto wheels=MouseEvents(MOUSEEVENTF_WHEEL);f.ray_hit=false;f.right.thumb_y=1;mock::time+=200;
    Update(f);CHECK(MouseEvents(MOUSEEVENTF_WHEEL)==wheels);
    f.ray_hit=true;Update(f);CHECK(MouseEvents(MOUSEEVENTF_WHEEL)==wheels+1);
    // Exact menu content edges map to the client edges, not cropped/clamped stale UVs.
    f.right.thumb_y=0;f.u=0;f.v=0;Update(f);CHECK(mock::lastX==20&&mock::lastY==30);
    f.u=1;f.v=1;Update(f);CHECK(mock::lastX==1939&&mock::lastY==1109);
    // Existing real controller priority must not also inject a keyboard fallback.
    Reset();f=Menu();Update(f);mock::physical=true;Poll();mock::time+=1500;f.right.a_down=true;
    Update(f);CHECK(mock::events.empty());CHECK(mock::moves==0);
    // Loading whitelist: native B/Skip works, all unrelated actions/axes stay neutral.
    Reset();f=Menu();f.allow_pointer=false;f.passive_back_only=true;Native(f);
    f.left.x_down=f.left.y_down=f.right.a_down=true;f.left.trigger=f.right.trigger=1;
    f.left.thumb_x=f.right.thumb_y=1;f.left.grip=f.right.grip=1;Update(f);a=Poll();
    CHECK(a.Gamepad.wButtons==0&&a.Gamepad.bLeftTrigger==0&&a.Gamepad.bRightTrigger==0);
    CHECK(a.Gamepad.sThumbLX==0&&a.Gamepad.sThumbRY==0&&Update(f).hand==-1);
    f.right.b_down=true;Update(f);CHECK(Poll().Gamepad.wButtons==0x2000);CHECK(mock::moves==0);
    Reset();f=Menu();f.passive_back_only=true;f.allow_pointer=false;Update(f);mock::time+=901;Update(f);
    f.right.b_down=true;Update(f);CHECK(g_key_down[VK_SPACE]);CHECK(!g_key_down[VK_ESCAPE]);
    f.right.b_down=false;Update(f);CHECK(!g_key_down[VK_SPACE]);
    // API packet codec roundtrip, saturation and stable packet IDs.
    Reset();g_present=true;kcd2vr::ui_menu_policy::Pad pad{};pad.buttons=0xA301;pad.lt=255;pad.rt=17;pad.lx=-32767;pad.ly=12345;pad.rx=-42;pad.ry=32767;Publish(pad);a=Poll();CHECK(a.Gamepad.wButtons==pad.buttons&&a.Gamepad.bLeftTrigger==255&&a.Gamepad.bRightTrigger==17&&a.Gamepad.sThumbLX==-32767&&a.Gamepad.sThumbLY==12345&&a.Gamepad.sThumbRX==-42&&a.Gamepad.sThumbRY==32767);Publish(pad);CHECK(Poll().dwPacketNumber==a.dwPacketNumber);
    // Session reset cannot reuse a held TLS packet during a contended read.
    ++g_session;g_seq=1;a=Poll();CHECK(a.Gamepad.wButtons==0);g_seq=2;
    std::cout<<"PASS "<<assertions<<" mocked Win32/XInput adapter assertions; not a Windows SDK build\n";
}
}
int main(){test::Tests();}
