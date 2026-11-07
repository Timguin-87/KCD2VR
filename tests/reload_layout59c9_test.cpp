#include <array>
#include <cassert>
#include <cstdint>
#include <cstring>
#include <cstdio>
static std::array<unsigned char,0x100> equipment{},weapon{},ammo{},component{},visual{};
static int player;
static bool haveVisual=true;
struct ManualItems596 {void* equipment=nullptr;void* weapon=nullptr;void* ammo=nullptr;void* component=nullptr;
    std::uint64_t weaponKey=0,ammoKey=0;bool loaded=false;std::uint32_t nock=0;
    void* visual=nullptr;std::uint64_t nockedKey=0;};
static bool ManualPlayer596(void* p){return p==&player;}
static void* ManualEquipmentPtr59c9(void*){return equipment.data();}
static void* ManualComponent59c9(void* p){assert(p==weapon.data());return component.data();}
static void* ManualVisual59c9(void* p){assert(p==component.data());return haveVisual?visual.data():nullptr;}
static bool PhysVft6698(void* p,std::uintptr_t){return p==weapon.data()||p==ammo.data();}
static bool PhysEmpty6698(std::uint64_t key){return !key;}
template<class T> static bool Read(const void* p,T& out){std::memcpy(&out,p,sizeof(out));return true;}
template<class T> static void put(void* p,T v){std::memcpy(p,&v,sizeof(v));}
#include "reload_layout59c9.inc"
int main(){
    put(equipment.data()+0x40,static_cast<void*>(weapon.data()));
    put(equipment.data()+0x48,static_cast<void*>(ammo.data()));
    put(weapon.data()+0x30,std::uint64_t(20));put(ammo.data()+0x30,std::uint64_t(30));
    put(component.data()+0xE0,std::uint64_t(30));
    // Two distinct native receivers. A plausible value at the OLD wrong offset
    // must never be accepted as the nocked entity.
    put(component.data()+0xDC,std::uint32_t(999));put(visual.data()+0xDC,std::uint32_t(123));
    ManualItems596 i{};assert(ManualItemsRead596(&player,i)&&i.loaded&&i.nock==123);
    put(visual.data()+0xDC,std::uint32_t(0));assert(ManualItemsRead596(&player,i)&&i.loaded&&i.nock==0);
    haveVisual=false;assert(ManualItemsRead596(&player,i)&&i.loaded&&i.nock==0);
    put(component.data()+0xE0,std::uint64_t(0));assert(ManualItemsRead596(&player,i)&&!i.loaded);
    assert(!ManualItemsRead596(nullptr,i));
    std::puts("PASS production nock layout: logical ammo and visual entity remain distinct");
}
