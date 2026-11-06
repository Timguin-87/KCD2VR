// Retain legacy backend helpers/configuration; verify the new public cycle separately.
#include "vrik/user_modes175.h"
#include <cstdio>
#include <cstring>
using namespace kcd2vr::user175;
static int fails=0;static void ck(bool b,const char* m){std::printf("%s %s\n",b?"PASS":"FAIL",m);fails+=!b;}
int main(){
    // OFF -> rung 2 -> rung 3 SYNCHRONIZED -> rung 4 VR DIRECT -> OFF (F4Rung178r10: integrated 1, geometry 2, contact lab 3)
    ck(hybridPlain59c10r9(false,false,false,0)==Mode::CombatGeometry,"OFF -> rung 2 (hybrid geometry)");
    ck(hybridPlain59c10r9(true,false,false,2)==Mode::CombatSynchronized,"rung 2 -> rung 3 SYNCHRONIZED");
    ck(hybridPlain59c10r9(true,true,true,1)==Mode::CombatContactLab,"rung 3 SYNCHRONIZED -> rung 4 VR direct");
    ck(hybridPlain59c10r9(true,false,false,3)==Mode::Off,"rung 4 -> OFF");
    ck(hybridPlain59c10r9(true,true,false,1)==Mode::CombatGeometry,"Shift+F4 rung 1 + plain F4 -> rung 2 (as before)");
    ck(hybridShift59c7e(false,false)==Mode::CombatIntegrated&&hybridShift59c7e(true,true)==Mode::Off,"Shift+F4 toggles rung 1 unchanged");
    const auto a=configuration(Mode::CombatSynchronized),b=configuration(Mode::CombatIntegrated);
    ck(a.combat==b.combat&&a.interaction==b.interaction&&a.pose==b.pose&&a.weapon==b.weapon&&a.experiment==b.experiment&&
       a.physical177==b.physical177&&a.full178r10==b.full178r10&&a.rung178r15==b.rung178r15,"SYNCHRONIZED uses exactly the rung-1 configuration");
    ck(std::strstr(name(Mode::CombatSynchronized),"SYNCHRONIZED")!=nullptr&&std::strstr(name(Mode::CombatContactLab),"PRESS2")!=nullptr,"retained DIRECT backend uses the current public press number");
    // R59.16C10R16: OFF -> 1 VR INTENT -> 2 GEOMETRY -> 3 SYNCHRONIZED -> 4 CONTACT LAB -> OFF (intent carries the sync flag too)
    ck(hybridPlain59c16(false,false,false,false,0)==Mode::CombatIntent,"R16 OFF -> press 1 VR INTENT");
    ck(hybridPlain59c16(true,true,true,true,1)==Mode::CombatGeometry,"R16 VR INTENT -> press 2 geometry");
    ck(hybridPlain59c16(true,false,false,false,2)==Mode::CombatSynchronized,"R16 geometry -> press 3 SYNCHRONIZED");
    ck(hybridPlain59c16(true,true,true,false,1)==Mode::CombatContactLab,"R16 SYNCHRONIZED -> press 4 contact lab");
    ck(hybridPlain59c16(true,false,false,false,3)==Mode::Off,"R16 contact lab -> OFF");
    ck(hybridPlain59c16(true,true,false,false,1)==Mode::CombatIntent,"R16 Shift+F4 integrated + plain F4 -> press 1");
    const auto c=configuration(Mode::CombatIntent);
    ck(c.combat==a.combat&&c.pose==a.pose&&c.weapon==a.weapon&&c.experiment&&c.rung178r15==a.rung178r15,"VR INTENT uses the SYNCHRONIZED configuration");
    ck(press59c16(Mode::CombatIntent)==1&&press59c16(Mode::CombatGeometry)==2&&press59c16(Mode::CombatSynchronized)==3&&
       press59c16(Mode::CombatContactLab)==4&&press59c16(Mode::CombatIntegrated)==5&&press59c16(Mode::Off)==0,"beeps = press number");
    ck(std::strstr(name(Mode::CombatIntent),"PRESS1")!=nullptr&&std::strstr(name(Mode::CombatGeometry),"PRESS2")!=nullptr&&
       std::strstr(name(Mode::CombatSynchronized),"PRESS1")!=nullptr,"retained SYNC backend uses the current public press number");
    ck(hybridCurrent19c4r2(false,false,false)==Mode::CombatSynchronized&&
       hybridCurrent19c4r2(true,true,false)==Mode::CombatContactLab&&
       hybridCurrent19c4r2(true,false,true)==Mode::Weapons,"current public cycle is SYNC -> DIRECT -> NORMAL_WEAPONS");
    ck(pressCurrent19c4r2(Mode::CombatSynchronized)==1&&pressCurrent19c4r2(Mode::CombatContactLab)==2&&
       pressCurrent19c4r2(Mode::Off)==0&&pressCurrent19c4r2(Mode::CombatIntent)==0,
       "current public beeps exclude the retired intent comparison");
    std::printf("%d failures\n",fails);return fails;}
