// C43: VR grab window (the hand stays on the controller from the VR pickup Use until the native Attach).
// Pure header logic: note/read/fresh, the fail-closed veto decision and the edge tracker used by the F5 receipts.
#include "../src/vrik/grab_reach43.h"
#include <cassert>
#include <cstdio>
#include <string>
using namespace kcd2vr::grab43;
static unsigned checks=0;
static void ck(bool b,const char* what){++checks;if(!b){std::printf("FAIL %s\n",what);assert(b);}}
int main(){
    Use u{};
    ck(!read(0,u),"no use yet");
    ck(!note(-1,5,1000),"side -1 refused");ck(!note(0,0,1000),"entity 0 refused");ck(!note(0,5,0),"tick 0 refused");
    ck(!read(0,u),"refused notes publish nothing");
    Use n{};ck(note(0,787046,10000,&n)&&n.serial==1&&n.entity==787046&&n.tick==10000,"first note");
    ck(read(0,u)&&u.serial==1&&u.entity==787046&&u.tick==10000,"read back");
    ck(!read(1,u),"other side untouched");
    ck(read(0,u)&&fresh(u,10000)&&fresh(u,10000+kWindowMs)&&!fresh(u,10000+kWindowMs+1)&&!fresh(u,9999),"4 s window, no reversed clock");
    ck(note(0,787046,12000)&&read(0,u)&&u.serial==2,"same entity again = new serial");
    // fail-closed decision: all four conditions
    ck(veto(true,true,true,false),"veto when setting+capability+fresh and not yet wrist-owned");
    ck(!veto(false,true,true,false),"setting off");ck(!veto(true,false,true,false),"native grasp capability unavailable");
    ck(!veto(true,true,false,false),"stale window");ck(!veto(true,true,true,true),"wrist already owned (R12B) needs no veto");
    // tracker edges
    Tracker t{};read(0,u);
    ck(t.reach(u,true,.25f)&&!t.reach(u,true,.30f)&&t.maxPull==.30f&&t.vetoed&&t.nativeSeen,"one REACH edge per serial, max pull kept");
    float henry[3]{3194.75f,500.65f,42.1f};float moved=0;const char* end="";
    ck(t.frame(u,true,false,henry,12000,moved,end)==1&&t.open&&t.openedAt==12000,"window opens on the first fresh committed frame");
    ck(t.frame(u,true,false,henry,12100,moved,end)==0,"stays open");
    henry[0]+=0.3f;henry[1]-=0.4f;
    ck(t.frame(u,true,true,henry,12900,moved,end)==2&&!t.open&&std::string(end)=="ATTACH_ADOPTED"&&moved>.499f&&moved<.501f,"closes when the grasp owner adopts the wrist, entity delta measured");
    ck(t.frame(u,true,true,henry,12950,moved,end)==0,"closed window stays closed for the same serial");
    Use u2{};note(0,55,20000,&u2);Tracker t2{};t2.frame(u2,true,false,henry,20000,moved,end);
    ck(t2.frame(u2,false,false,henry,24100,moved,end)==2&&std::string(end)=="EXPIRED"&&moved<.0001f,"closes EXPIRED without adoption");
    Use u3{};note(1,66,30000,&u3);Tracker t3{};
    ck(t3.frame(u3,true,true,henry,30000,moved,end)==0&&!t3.open,"a wrist already owned never opens a window");
    std::printf("grab_reach43: %u checks OK\n",checks);return 0;
}
