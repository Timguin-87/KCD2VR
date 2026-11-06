// Executes the current production two-consumer leaf and pickup callback result.
// Native memory and calls are explicit doubles; no game process is involved.
#include <windows.h>
#include <array>
#include <cmath>
#include <cstdint>
#include <cstdio>
#include <cstring>

using EntitySetPosRotScaleFn=void(__fastcall*)(void*,const float*,const float*,const float*,unsigned);
static std::array<unsigned char,0x200> entity{},entityVtable{},self{},child{};
static std::array<unsigned char,0x200> childVtable{};
static int prsCalls=0,childCalls=0,nativeCalls=0,failures=0,successReceipts=0;
static bool haveChild=false,throwChild=false;
static float entityX=0,childX=0;
template<class T> static bool Read(const void* p,T& out) noexcept {
    if(!p)return false;std::memcpy(&out,p,sizeof(out));return true;
}
template<class T> static void put(unsigned char* p,const T& value){std::memcpy(p,&value,sizeof(value));}
static void __fastcall SetEntity(void*,const float* p,const float*,const float*,unsigned){++prsCalls;entityX=p[0];}
static void __fastcall SetChild(void*,const float* q){++childCalls;if(throwChild)RaiseException(0xE1234567,0,0,nullptr);childX=q[4];}
static std::uintptr_t WhBase() noexcept {return reinterpret_cast<std::uintptr_t>(&SetChild)-0x423C30ull;}
static void* CharacterForEntity6689(void*) noexcept {return haveChild?child.data():nullptr;}
#include "pickup_place_actual.inc"

static bool QualifiedCallback(bool nativeGrasp,float destination){
    struct {void* entity;} socket{entity.data()};
    float wanted[8]{0,0,0,1,destination,0,0,1};
    const auto refuse=[](const char*){++failures;return false;};
    void* const self=::self.data();
#include "pickup_placement_result_actual.inc"
    ++successReceipts;return true;
}
static void Dispatch(bool ordinaryPickup,float destination){
    if(!QualifiedCallback(ordinaryPickup,destination)){++nativeCalls;entityX=-5;childX=-5;}
}
static void reset(){
    entity={};entityVtable={};self={};child={};childVtable={};
    prsCalls=childCalls=nativeCalls=failures=successReceipts=0;haveChild=throwChild=false;entityX=childX=0;
    put(entity.data(),static_cast<void*>(entityVtable.data()));put(entityVtable.data()+0x168,EntitySetPosRotScaleFn(&SetEntity));
    for(int i=0;i<3;++i)put(self.data()+0x0C+4*i,1.f);
    put(child.data(),static_cast<void*>(childVtable.data()));put(childVtable.data()+0x190,reinterpret_cast<void*>(&SetChild));
}
static int checks=0;
#define CHECK(x) do{++checks;if(!(x)){std::printf("FAIL line %d: %s\n",__LINE__,#x);return 1;}}while(0)
int main(){
    reset();Dispatch(true,2);CHECK(entityX==2&&prsCalls==1&&nativeCalls==0&&successReceipts==1);
    reset();haveChild=true;Dispatch(true,3);CHECK(entityX==3&&childX==3&&childCalls==1&&successReceipts==1);
    // Actual production leaf writes the entity before rejecting a child ABI.
    reset();haveChild=true;put(childVtable.data()+0x190,static_cast<void*>(nullptr));Dispatch(true,4);
    CHECK(entityX==4&&prsCalls==1&&childCalls==0&&failures==1);
    CHECK(nativeCalls==0&&successReceipts==0);
    // Revalidation/retry must use the new destination, not freeze the previous pose.
    put(childVtable.data()+0x190,reinterpret_cast<void*>(&SetChild));Dispatch(true,6);
    CHECK(entityX==6&&childX==6&&successReceipts==1&&nativeCalls==0);
    // A native exception after the entity write has the same disposition.
    reset();haveChild=true;throwChild=true;Dispatch(true,7);
    CHECK(entityX==7&&childCalls==1&&failures==1&&successReceipts==0&&nativeCalls==0);
    // Even an early failure grants no placement receipt or secondary writer.
    reset();put(entityVtable.data()+0x168,EntitySetPosRotScaleFn(nullptr));Dispatch(true,9);
    CHECK(prsCalls==0&&failures==1&&successReceipts==0&&nativeCalls==0);
    // This repair intentionally leaves station tool fallback behaviour alone.
    reset();haveChild=true;put(childVtable.data()+0x190,static_cast<void*>(nullptr));Dispatch(false,4);
    CHECK(prsCalls==1&&nativeCalls==1&&entityX==-5&&successReceipts==0);
    std::printf("R91R3 actual pickup placement callback: %d PASS\n",checks);return 0;
}
