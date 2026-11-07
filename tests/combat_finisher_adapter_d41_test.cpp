// Literal production OpponentHeightCB1; memory and measured blade path are leaves.
#include "subsystems/masterstrike_combos/physical_finisher_cb1.h"
#include <unordered_map>
#include <cstring>
#include <cstdio>
using A=std::uintptr_t;
static std::atomic<A> g_combo_base6691{0x10000000};
static std::unordered_map<A,unsigned char> memory;
template<class T>static void put(A p,const T& value){const auto* bytes=reinterpret_cast<const unsigned char*>(&value);for(unsigned i=0;i<sizeof(T);++i)memory[p+i]=bytes[i];}
static bool ComboRead6691(A p,void* to,std::size_t size){auto* bytes=static_cast<unsigned char*>(to);for(std::size_t i=0;i<size;++i){auto it=memory.find(p+i);if(it==memory.end())return false;bytes[i]=it->second;}return true;}
static float tipD41[3]{},wasD41[3]{};static bool pathKnownD41=true;
namespace frik_pose_probe {static bool WeaponPathWorldCB4(bool,float* tip,float* was){std::memcpy(tip,tipD41,sizeof(tipD41));std::memcpy(was,wasD41,sizeof(wasD41));return pathKnownD41;}}
#include "finisher_adapter_d41_production.inc"
static unsigned checks=0,failed=0;
static void ck(bool value,const char* label){++checks;if(!value)++failed;std::printf("%s %s\n",value?"PASS":"FAIL",label);}
static void path(float x,float y,float z,float wx,float wy,float wz){tipD41[0]=x;tipD41[1]=y;tipD41[2]=z;wasD41[0]=wx;wasD41[1]=wy;wasD41[2]=wz;}
int main(){
 const A base=g_combo_base6691.load(),actor=0x1000,state=0x2000,opp=0x4000,owner=0x5000,entity=0x6000;
 put(actor+0x210,state);put(state+0xCF8,opp);put(opp,base+0x3E3FAC8);put(opp+0x1F8,owner);put(owner+0x38,entity);put(entity,base+0x4072C78);
 float tm[12]{1,0,0,0,0,1,0,0,0,0,1,0};put(entity+0x58,tm);double h=0,hc=0;
 path(.7f,0,.95f,.8f,0,1.05f);ck(OpponentHeightCB1(actor,false,h,hc)&&hc>.54&&hc<.56,"native adapter retains measured approach to approximate knee-height contact");
 path(.7f,.4f,.95f,.8f,.4f,1.05f);ck(!OpponentHeightCB1(actor,false,h,hc),"native adapter does not claim region for line missing opponent cylinder");
 path(2.5f,0,.5f,2.6f,0,1.5f);ck(!OpponentHeightCB1(actor,false,h,hc),"native adapter refuses body beyond bounded physical approach");
 path(1,0,.5f,1,0,.6f);ck(!OpponentHeightCB1(actor,false,h,hc),"native adapter refuses vertical stroke beside opponent");
 path(.7f,0,.5f,.6f,0,.4f);ck(!OpponentHeightCB1(actor,false,h,hc),"native adapter refuses measured motion away from opponent");
 path(.2f,0,.5f,.3f,0,.6f);ck(OpponentHeightCB1(actor,false,h,hc)&&hc==.5,"current point already inside approximate body retains current height");
 pathKnownD41=false;ck(!OpponentHeightCB1(actor,false,h,hc),"missing measured physical weapon path never grants region");
 std::printf("combat_finisher_adapter_d41 %u/%u\n",checks-failed,checks);return failed?1:0;
}
