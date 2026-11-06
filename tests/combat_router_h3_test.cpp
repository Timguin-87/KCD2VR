#include "subsystems/direct/combat_router_h3.h"
#include <cassert>
#include <map>
#include <string>
#include <limits>
namespace h=kcd2vr::routerH3;
struct Console {
    std::map<std::string,float> values{{"wh_cs_ComboSlotDuration",.5f},{"wh_cs_RiposteSlotDuration",.5f},
        {"wh_cs_StandardSlotDuration",1.f},{"wh_cs_AISlotDurationMod",.5f},
        {"wh_cs_PLSlotDurationMod",.5f},{"wh_cs_TimeAlignment_BlockSpeedMod",.8f}};
    int reads=0,writes=0;bool failRead=false,failWrite=false,writeThenFail=false;
    bool Read(const char* name,float& value){++reads;if(failRead)return false;value=values.at(name);return true;}
    bool Write(const char* name,float value){++writes;if(!failWrite||writeThenFail)values.at(name)=value;return !failWrite;}
};
static auto quiet=[](const char*,const char*,float,float){};
int main(){
    h::mode=0;assert(h::comboWindow(1500)==1500);
    h::mode=1;assert(h::comboWindow(1500)==1500);
    h::mode=2;assert(h::comboWindow(1500)==3000&&h::comboWindow(0)==0);
    {Console io;h::Windows w;h::mode=0;w.service(true,1000,io,quiet);
     h::mode=1;w.service(true,1300,io,quiet);assert(io.reads==0&&io.writes==0&&!w.owns());}
    {Console io;h::Windows w;h::mode=2;w.service(true,1000,io,quiet);
     assert(w.owns()&&io.values["wh_cs_ComboSlotDuration"]==3.f&&io.values["wh_cs_AISlotDurationMod"]==.2f);
     const int reads=io.reads,writes=io.writes;for(int n=0;n<1000;++n)w.service(true,1300+n,io,quiet);
     assert(io.reads==reads&&io.writes==writes); // zero steady native polling
     h::mode=1;w.service(true,2300,io,quiet);assert(!w.owns()&&io.values["wh_cs_ComboSlotDuration"]==.5f);
     assert(io.values["wh_cs_AISlotDurationMod"]==.5f);}
    {Console io;h::Windows w;h::mode=2;w.service(true,1000,io,quiet);io.failRead=true;
     h::mode=0;w.service(false,1001,io,quiet);assert(w.owns());
     io.failRead=false;w.service(false,1300,io,quiet);assert(!w.owns());}
    {Console io;h::Windows w;h::mode=2;w.service(true,1000,io,quiet);io.failWrite=true;
     h::mode=0;w.service(false,1001,io,quiet);assert(w.owns());
     io.failWrite=false;w.service(false,1300,io,quiet);assert(!w.owns());}
    {Console io;h::Windows w;h::mode=2;io.failWrite=io.writeThenFail=true;
     w.service(true,1000,io,quiet);assert(w.owns());
     h::mode=0;io.failWrite=false;w.service(false,1001,io,quiet);assert(!w.owns()&&io.values["wh_cs_ComboSlotDuration"]==.5f);}
    {Console io;h::Windows w;h::mode=2;w.service(true,1000,io,quiet);
     io.values["wh_cs_ComboSlotDuration"]=2.25f;h::mode=0;w.service(false,1001,io,quiet);
     assert(io.values["wh_cs_ComboSlotDuration"]==2.25f&&!w.owns());}
    {Console io;h::Windows w;h::mode=2;io.failRead=true;
     for(int n=0;n<100;++n)w.service(true,1000+n*250,io,quiet);
     assert(io.reads==48&&io.writes==0); // max eight reads per unavailable entry
     io.failRead=false;h::mode=1;w.service(false,27000,io,quiet);h::mode=2;w.service(true,27001,io,quiet);assert(w.owns());
     h::mode=0;w.service(false,27002,io,quiet);assert(!w.owns());}
    // H18: OFF stays a real no-op even during live combat/context changes.
    {Console io;h::Windows w;h::mode=0;
     for(int i=0;i<100;++i)w.service(i%2==0,30000+i*300,io,quiet);
     assert(!w.owns()&&io.reads==0&&io.writes==0&&h::comboWindow(1500)==1500);
     h::mode=2;w.service(true,61000,io,quiet);assert(w.owns());
     h::mode=0;w.service(true,61001,io,quiet);
     assert(!w.owns()&&io.values["wh_cs_PLSlotDurationMod"]==.5f&&io.values["wh_cs_TimeAlignment_BlockSpeedMod"]==.8f);}
    {h::Observer observer;h::Observation o{};o.key={1,2,3,4,5,6,7};
     assert(!observer.take(o));o.mode=1;assert(observer.take(o)&&!observer.take(o));
     ++o.key.body;assert(observer.take(o));++o.key.request;assert(observer.take(o));
     ++o.key.recovery;assert(observer.take(o));++o.key.player;assert(observer.take(o));
     o.key.player=0;assert(!observer.take(o));}
    assert(h::setting(std::numeric_limits<float>::quiet_NaN(),0,1,.2f)==.2f);
    assert(h::setting(10,0,1,.2f)==1);
}
