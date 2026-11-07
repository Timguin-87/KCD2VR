// C58 PROP WAKE: pure logic of src/vrik/lab_prop58.h - lua chunk text (exact), contact admission matrix, rate limiter,
// receipt cap, fault latch, impulse k.
#include "../src/vrik/lab_prop58.h"
#include <cassert>
#include <cmath>
#include <cstdio>
#include <cstring>
#include <limits>
using namespace kcd2vr::lab_prop58;
static unsigned n=0;static void ck(bool b){++n;if(!b){std::printf("check %u failed\n",n);std::fflush(stdout);}assert(b);}
static bool eq(const char* a,const char* b){return a&&b&&std::strcmp(a,b)==0;}
static Contact good(){Contact c{};c.entity=4711;c.body=Rigid;c.speed=3;c.distKnown=true;c.dist=.1;c.active=true;c.pakPresent=true;c.luaRoute=true;return c;}
int main(){
    // ---- lua chunk: exact text, protocol check, pcall, unit direction, no quotes from input
    {WakeEvent e{};e.id=4711;e.d[0]=2;e.d[1]=0;e.d[2]=0;e.p[0]=1.5;e.p[1]=-2.25;e.p[2]=0.125;e.speed=3.456;e.hand=0;
     char buf[600]{};const int len=buildWakeLua(e,buf,sizeof(buf));ck(len>0&&len==int(std::strlen(buf)));
     ck(eq(buf,"if KCD2VR_Lab and KCD2VR_Lab.protocol==1 and KCD2VR_Lab.Wake then local ok,err=pcall(KCD2VR_Lab.Wake,{id=4711,"
        "d={x=1.000,y=0.000,z=0.000},p={x=1.500,y=-2.250,z=0.125},speed=3.46,hand=0}) "
        "if not ok then System.LogAlways('KCD2VR_LAB error id=4711 '..tostring(err)) end "
        "else System.LogAlways('KCD2VR_LAB pak script not loaded (protocol 1)') end"));}
    {WakeEvent e{};e.id=7;e.d[0]=0;e.d[1]=0;e.d[2]=-1;e.speed=.6;e.hand=2;char buf[600]{};ck(buildWakeLua(e,buf,sizeof(buf))>0);
     ck(std::strstr(buf,"{id=7,d={x=0.000,y=0.000,z=-1.000},p={x=0.000,y=0.000,z=0.000},speed=0.60,hand=2}")!=nullptr);
     ck(std::strchr(buf,'"')==nullptr);}
    // bad input -> 0, nothing cut off
    {WakeEvent e{};e.id=0;e.d[2]=1;e.speed=1;char b[600]{};ck(buildWakeLua(e,b,sizeof(b))==0);}                        // no id
    {WakeEvent e{};e.id=1;e.speed=1;char b[600]{};ck(buildWakeLua(e,b,sizeof(b))==0);}                                 // zero direction
    {WakeEvent e{};e.id=1;e.d[2]=1;e.speed=0;char b[600]{};ck(buildWakeLua(e,b,sizeof(b))==0);}                        // no speed
    {WakeEvent e{};e.id=1;e.d[2]=std::numeric_limits<double>::quiet_NaN();e.speed=1;char b[600]{};ck(buildWakeLua(e,b,sizeof(b))==0);}
    {WakeEvent e{};e.id=1;e.d[2]=1;e.p[0]=std::numeric_limits<double>::infinity();e.speed=1;char b[600]{};ck(buildWakeLua(e,b,sizeof(b))==0);}
    {WakeEvent e{};e.id=1;e.d[2]=1;e.speed=1;e.hand=3;char b[600]{};ck(buildWakeLua(e,b,sizeof(b))==0);}
    {WakeEvent e{};e.id=1;e.d[2]=1;e.speed=1;char b[100]{};ck(buildWakeLua(e,b,sizeof(b))==0&&b[0]==0);ck(buildWakeLua(e,nullptr,600)==0);} // too small
    // ---- impulse k = clamp(0.3*speed, 0.2, 2.0)
    ck(std::fabs(impulseK(3)-.9)<1e-12&&impulseK(.1)==kMinK&&impulseK(50)==kMaxK&&impulseK(std::numeric_limits<double>::quiet_NaN())==kMinK);
    // ---- admission matrix
    ck(admit(good())&&refuse(good())==nullptr);
    {auto c=good();c.active=false;ck(eq(refuse(c),"NOT_ACTIVE"));}
    {auto c=good();c.pakPresent=false;ck(eq(refuse(c),"NO_LAB_PAK"));}
    {auto c=good();c.luaRoute=false;ck(eq(refuse(c),"NO_LUA_ROUTE"));}
    {auto c=good();c.latched=true;ck(eq(refuse(c),"FAULT_LATCHED_OFF"));}
    {auto c=good();c.entity=0;ck(eq(refuse(c),"NO_ENTITY"));}
    {auto c=good();c.henry=true;ck(eq(refuse(c),"HENRY"));}
    {auto c=good();c.held=4711;ck(eq(refuse(c),"HELD_ITEM"));c.held=99;ck(admit(c));}     // another item in the hand is fine
    {auto c=good();c.heldAny=true;ck(eq(refuse(c),"HELD_OR_PRESENTED"));}                 // S6: held visual / prop servo presentation on either hand
    {auto c=good();c.body=Living;ck(eq(refuse(c),"LIVING"));c.body=Articulated;ck(eq(refuse(c),"ARTICULATED"));
     c.body=Physical;ck(eq(refuse(c),"STATIC_BODY"));c.body=77;ck(eq(refuse(c),"BODY_KIND"));c.body=Unknown;ck(eq(refuse(c),"UNKNOWN_BODY"));c.body=Rigid;ck(admit(c));} // S5: only a proven Rigid body
    {auto c=good();c.speed=.59;ck(eq(refuse(c),"TOO_SLOW"));c.speed=.6;ck(admit(c));c.speed=12;ck(admit(c));c.speed=12.01;ck(eq(refuse(c),"TOO_FAST"));
     c.speed=std::numeric_limits<double>::quiet_NaN();ck(eq(refuse(c),"TOO_SLOW"));}
    {auto c=good();c.distKnown=false;ck(eq(refuse(c),"NO_ENTITY_POS"));c.distKnown=true;c.dist=.31;ck(eq(refuse(c),"TOO_FAR"));c.dist=.3;ck(admit(c));
     c.dist=std::numeric_limits<double>::infinity();ck(eq(refuse(c),"NO_ENTITY_POS"));}
    // ---- rate limiter: 8 per second, per-entity 500 ms
    {Budget b{};std::uint64_t t=10000;
     for(unsigned i=0;i<8;++i)ck(b.admit(100+i,t+i));
     ck(eq(b.refuse(200,t+8),"GLOBAL_BUDGET")&&!b.admit(200,t+8));
     ck(b.admit(200,t+1000));                                           // window passed
     ck(eq(b.refuse(200,t+1400),"ENTITY_COOLDOWN")&&!b.admit(200,t+1400));
     ck(b.refuse(201,t+1400)==nullptr);                                  // other entity fine (budget has room again)
     ck(b.admit(200,t+1500));                                            // cooldown over
     ck(eq(b.refuse(0,t),"NO_ENTITY"));}
    {Budget b{};for(unsigned i=0;i<40;++i)ck(b.admit(1000+i,20000+i*130));} // spread out: never refused, slots recycle
    // ---- receipt cap 4/s
    {ReceiptCap c{};ck(c.allow(1)&&c.allow(2)&&c.allow(3)&&c.allow(4)&&!c.allow(5)&&!c.allow(999)&&c.allow(1001));}
    // ---- fault latch after 3
    faults.store(0);latched.store(false);ck(!noteFault()&&!latched.load());ck(!noteFault());ck(noteFault()&&latched.load());ck(faults.load()==3);
    std::printf("lab_prop58_test ok (%u checks)\n",n);return 0;
}
