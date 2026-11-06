#include "subsystems/throw_drop/inventory_lifecycle_t8.h"
#include <cassert>
#include <cstdio>
#include <cwchar>
#include <string>
using namespace kcd2vr;
struct State {bool throwing_supply_t2=false;int weapon_release_mode_t1=0;};
struct Rect {int id;};
namespace immersion_menu45 {constexpr Rect kNote{1};}
constexpr Rect kV149UserHolsters{2},kWeaponReleaseT1{3};
Rect U49(Rect r){return r;}
bool PtIn(Rect r,int x,int){return r.id==x;}
State actual{};unsigned saves=0,dirty=0;std::string trace;
void ApplyState(State s){if(s.throwing_supply_t2&&!actual.throwing_supply_t2)inventory_lifecycle_t8::Recheck();actual=s;}
void save(){++saves;}void MarkDirty(){++dirty;}
namespace diagnostic_trace {void Write(const char* s){trace=s;}}
void Click(int x){int y=0;auto s=actual;
#define KCD2VR_MENU_CLICK_KV149USERHOLSTERS
#include "subsystems/throw_drop/menu.inl"
#undef KCD2VR_MENU_CLICK_KV149USERHOLSTERS
#define KCD2VR_MENU_CLICK_KWEAPONRELEASET1
#include "subsystems/throw_drop/menu.inl"
#undef KCD2VR_MENU_CLICK_KWEAPONRELEASET1
}
std::wstring labels[4];int dc=0,font_small_=0,kAccent=1,kPanel2=0;
void DrawButton(int,int,Rect r,const wchar_t* text,int){labels[r.id]=text;}
void Draw(){auto s=actual;
#define KCD2VR_MENU_DRAW_KV149USERHOLSTERS
#include "subsystems/throw_drop/menu.inl"
#undef KCD2VR_MENU_DRAW_KV149USERHOLSTERS
#define KCD2VR_MENU_DRAW_STOCK_T8
#include "subsystems/throw_drop/menu.inl"
#undef KCD2VR_MENU_DRAW_STOCK_T8
}
int main(){namespace inv=inventory_lifecycle_t8;
inv::state=4;Draw();assert(labels[2]==L"KNIVES (HIP/CHEST): OFF");
Click(2);assert(actual.throwing_supply_t2);assert(inv::state==6);assert(!inv::ReplenishWanted());
inv::daggers=3;Draw();assert(labels[2]==L"KNIVES (HIP/CHEST): 3");
Click(1);assert(!inv::ReplenishWanted());assert(inv::replenishRequest34==0); // no old refill hotspot
Click(2);assert(!actual.throwing_supply_t2);Click(3);assert(actual.weapon_release_mode_t1==1);
assert(saves==dirty);std::puts("PASS actual left-hip menu handlers: gate, stock, no grant/refill, release mode unchanged");}
