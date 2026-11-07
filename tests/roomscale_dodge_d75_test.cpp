// D75 actual production recognizer: no copied algorithm or simulated native acceptance.
#include "vrik/roomscale58.h"
#include <cstdio>
#include <cmath>
using namespace kcd2vr::roomscale58;
static int checks=0,failed=0;
static void ck(bool v,const char* n){++checks;if(!v){++failed;std::printf("FAIL %s\n",n);}}
static Drive observeDrive(State& s,std::uint64_t t,double x,double y,double height=0,bool combat=true,bool known=true,const Config& c=Config{}){
    observeHmd(s,t,x,y,0,1,height,known);return drive(s,t,x,y,false,combat,true,c);
}
static State travel(double x,double y,int duration,double height=0,bool known=true){
    State s{};for(int t=0;t<=duration;t+=20)observeDrive(s,1000+t,x*t/duration,y*t/duration,height,true,known);return s;
}
int main(){
    const Config c{};
    ck(c.dodgeCooldownMs==2000&&c.dodgeHoldMs==120,"existing 2s cooldown and 120ms native input hold unchanged");
    for(double x:{-.15,0.,.15})for(double y:{-.15,0.,.15}){
        auto s=travel(x,y,1000);ck(s.dodges==0,"ordinary slow side/back/diagonal step never replaces native attack with Dodge");
    }
    for(double height:{0.,-.099})for(double x:{-.14,0.,.14})for(double y:{-.14,0.,.14}){
        auto s=travel(x,y,200,height);const bool expected=(y<=0&&(x!=0||y!=0))||(y>0&&std::abs(x)>y);
        ck((s.dodges==1)==expected,"fast upright side/back travel qualifies once and dominant forward travel never qualifies");
    }
    for(double x:{-.25,0.,.25})for(double y:{-.25,0.,.25}){
        State s{};for(int t=0;t<=200;t+=20)observeDrive(s,1000+t,x*t/200.,y*t/200.,-.1001);
        ck(s.dodges==0,"known height loss over10cm cannot generate dodge during lean");
        for(int t=20;t<=400;t+=20)observeDrive(s,1200+t,x,y,0);
        ck(s.dodges==0,"standing upright at leaned position never replays retired lean translation");
        for(int t=20;t<=200;t+=20)observeDrive(s,1600+t,x-.14*t/200.,y,0);
        ck(s.dodges==1,"a fresh fast side step after recovery remains available");
    }
    { // Turning height policy OFF preserves the user's explicit translation-only choice.
        State s{};Config disabled{};disabled.forwardLeanHeightD70=0;
        for(int t=0;t<=200;t+=20)observeDrive(s,2000+t,.14*t/200.,0,-.2,true,true,disabled);
        ck(s.dodges==1,"zero height setting deliberately retains translation-only fast dodge");
    }
    ck(travel(.14,0,200,-.2,false).dodges==1,"unknown height does not invent a lean or block a fresh fast side gesture");
    { // Input sends no action in an ordinary movement-only context.
        State s{};for(int t=0;t<=200;t+=20)observeDrive(s,3000+t,.2*t/200.,0,0,false);
        ck(s.dodges==0,"no opponent context cannot leak native jump from head motion");
    }
    { // Cooldown and quiet recovery apply even when the user ends at a new origin.
        State s{};for(int t=0;t<=200;t+=20)observeDrive(s,4000+t,.14*t/200.,0);
        ck(s.dodges==1,"first distinct fast gesture produces one request");
        const auto first=s.lastDodge;
        for(int t=20;t<=600;t+=20)observeDrive(s,4200+t,.14,0);
        ck(!s.returning54&&s.dodges==1,"200ms quiet new position rearms without replaying held displacement");
        for(int t=20;t<=200;t+=20)observeDrive(s,4800+t,.14-.14*t/200.,0);
        ck(s.dodges==1&&s.lastDodge==first,"fast return inside cooldown cannot repeatedly replace an attack");
        for(int t=20;t<=1400;t+=20)observeDrive(s,5000+t,0,0);
        for(int t=20;t<=200;t+=20)observeDrive(s,6400+t,-.14*t/200.,0);
        ck(s.dodges==2,"next distinct fast gesture after cooldown remains available");
    }
    { // Lean immediately after a sent request releases held input, but does not erase the cooldown or rearm.
        State s{};for(int t=0;t<=200;t+=20)observeDrive(s,8000+t,.14*t/200.,0);
        const auto first=s.lastDodge;
        auto d=observeDrive(s,8220,.15,0,-.11);ck(!d.dodge&&s.lastDodge==first&&s.returning54,"lean releases pending button hold while preserving existing cooldown and quiet rearm");
        observeDrive(s,8240,.15,0,0);
        for(int t=20;t<=200;t+=20)observeDrive(s,8240+t,.15+.14*t/200.,0);
        ck(s.dodges==1,"lean/stand motion cannot bypass cooldown and quiet recovery");
    }
    { // A sparse recording is not a valid continuous tracked gesture.
        State s{};observeDrive(s,9000,0,0);observeDrive(s,9200,.2,0);ck(s.dodges==0,"tracking gap retains existing fail-closed reset");
    }
    { // Correcting gesture admission never changes roomscale follow/anchor bookkeeping.
        State s{};s.anchorX=.4;s.anchorY=-.2;
        for(int t=0;t<=200;t+=20)observeDrive(s,10000+t,.2*t/200.,0,-.11);
        ck(s.anchorX==.4&&s.anchorY==-.2,"lean/dodge classification does not alter HMD or controller world origin");
    }
    std::printf("D75 roomscale dodge actual header: %d/%d passed\n",checks-failed,checks);return failed?1:0;
}
