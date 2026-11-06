// Actual restore-only production lease and F9 policy; native CVar IO is mocked.
#include "ragdoll_lease59.h"
#include "vrik/lab58.h"
#include <cstdio>
#include <limits>
#include <string>
#include <unordered_map>
using Lease=kcd2vr::ragdoll59::RetiredGlobalLeaseD51;
using Result=kcd2vr::ragdoll59::RetireResultD51;
static unsigned checks=0,failed=0;
static void ck(bool b,const char* why){++checks;if(!b){++failed;std::printf("FAIL %s\n",why);}}
struct IO {
    std::unordered_map<std::string,float> values;
    unsigned reads=0,writes=0;bool readable=true,writable=true,vanishAfterWrite=false,foreignAfterWrite=false;
    bool Read(const char* name,float& out) noexcept {
        ++reads;if(!readable)return false;
        const auto i=values.find(name);if(i==values.end())return false;out=i->second;return true;
    }
    bool Write(const char* name,float value) noexcept {
        ++writes;if(!writable)return false;values[name]=foreignAfterWrite?7.f:value;
        if(vanishAfterWrite)readable=false;return true;
    }
};
int main(){
    namespace lab=kcd2vr::lab58;
    ck(!lab::livingReactionsAvailableD51(),"unproven living animation/physics actuation is unavailable");
    for(int rung=0;rung<4;++rung)for(int switches=0;switches<16;++switches){
        lab::rung=rung;lab::livingHitImpact=bool(switches&1);lab::livingTouch=bool(switches&2);
        lab::corpseDrag=bool(switches&4);lab::propWake=bool(switches&8);
        ck(!lab::active(lab::Sub::LivingHitImpact)&&!lab::active(lab::Sub::LivingTouch),"old saved living selections cannot activate at any F9 rung");
        ck(lab::active(lab::Sub::CorpseDrag)==bool(rung==3&&(switches&4)),"corpse selection and rung policy preserved");
        ck(lab::active(lab::Sub::PropWake)==bool(rung==3&&(switches&8)),"prop selection and rung policy preserved");
        ck(!lab::fallAndPlayRequestedH17(true,true,true,rung),"no global fall threshold override even inside F9 lab");
    }
    for(const char* name:{"ca_physicsProcessImpact","ca_UseRecoil","g_fallAndPlayThreshold"}){
        IO io;io.values[name]=50.f;Lease lease{name};
        for(int frame=0;frame<200;++frame)ck(lease.Retire(io)==Result::Inactive,"unowned restoration is an atomic/local idle path");
        ck(io.reads==0&&io.writes==0&&io.values[name]==50.f,"normal D51 frame does no global CVar IO");
    }
    {IO io;io.values["x"]=1.f;Lease s{"x",true,0.f,1.f};
     ck(s.Retire(io)==Result::Restored&&!s.owned&&io.values["x"]==0.f,"restore only exact owned value and verify");
     const auto reads=io.reads,writes=io.writes;for(int i=0;i<100;++i)(void)s.Retire(io);
     ck(io.reads==reads&&io.writes==writes,"successful retirement never reasserts or polls");}
    {IO io;io.values["x"]=7.f;Lease s{"x",true,0.f,1.f};
     ck(s.Retire(io)==Result::ForeignValue&&!s.owned&&io.writes==0,"another writer keeps its value");
     io.values["x"]=1.f;(void)s.Retire(io);ck(io.writes==0&&io.values["x"]==1.f,"retired ownership is never reacquired");}
    {IO io;io.values["x"]=5.005f;Lease s{"x",true,0.f,5.f};
     ck(s.Retire(io)==Result::ForeignValue&&!s.owned&&io.writes==0&&io.values["x"]==5.005f,"nearby foreign float is not mistaken for our exact write");}
    {IO io;io.values["x"]=.005f;Lease s{"x",true,0.f,1.f};
     ck(s.Retire(io)==Result::ForeignValue&&!s.owned&&io.writes==0,"nearby original float is not falsely reported already restored");}
    {IO io;io.values["x"]=0.f;Lease s{"x",true,0.f,1.f};
     ck(s.Retire(io)==Result::AlreadyOriginal&&!s.owned&&io.writes==0,"already restored value needs no write");}
    {IO io;io.readable=false;Lease s{"x",true,0.f,1.f};
     for(int i=0;i<3;++i)ck(s.Retire(io)==Result::ReadUnavailable&&s.owned,"uncertain read retains restore debt");
     for(int i=0;i<100;++i)(void)s.Retire(io);
     ck(io.reads==3&&io.writes==0&&s.Retire(io)==Result::RetryLimit,"unavailable native IO has finite retry budget");}
    {IO io;io.values["x"]=1.f;io.writable=false;Lease s{"x",true,0.f,1.f};
     ck(s.Retire(io)==Result::WriteRefused&&s.owned,"refused restore is not reported as success");
     io.writable=true;ck(s.Retire(io)==Result::Restored&&!s.owned&&io.writes==2,"later successful restore closes exact debt");}
    {IO io;io.values["x"]=1.f;io.vanishAfterWrite=true;Lease s{"x",true,0.f,1.f};
     ck(s.Retire(io)==Result::VerifyUnavailable&&s.owned,"lost write readback remains uncertain");
     io.readable=true;ck(s.Retire(io)==Result::AlreadyOriginal&&io.writes==1&&!s.owned,"completed uncertain restore is recognized without another write");}
    {IO io;io.values["x"]=1.f;io.foreignAfterWrite=true;Lease s{"x",true,0.f,1.f};
     ck(s.Retire(io)==Result::ForeignValue&&!s.owned&&io.values["x"]==7.f,"a foreign writer during readback wins");}
    {IO io;io.values["x"]=1.f;Lease s{"x",true,std::numeric_limits<float>::quiet_NaN(),1.f};
     ck(s.Retire(io)==Result::InvalidSavedValue&&!s.owned&&io.reads==0&&io.writes==0,"invalid saved originals cannot become native writes");}
    std::printf("D51 living policy/global retirement %u/%u; native IO mocked\n",checks-failed,checks);return failed?1:0;
}
