#include "held_map_m1.h"
#include "held_map_pose_m1.h"
#include "ui_hud_experiment36c5.h"
#include <cassert>
#include <limits>
#include <cstdio>
using namespace kcd2vr;
int main(){
 using namespace held_map_m1;
 assert(GetPrintedRegion()==PrintedRegion::Trosky);
 const auto serial=activation.load();assert(SetPrintedRegion(PrintedRegion::Kuttenberg));
 assert(GetPrintedRegion()==PrintedRegion::Kuttenberg&&activation.load()==serial+1);
 assert(SetPrintedRegion(PrintedRegion::Kuttenberg)&&activation.load()==serial+1);
 assert(!SetPrintedRegion(static_cast<PrintedRegion>(2))&&GetPrintedRegion()==PrintedRegion::Kuttenberg);
 assert(SetPrintedRegion(PrintedRegion::Trosky));
 Select(false);assert(!Requested()&&!Selected());PublishAllowed(false,100);assert(!Allowed(100));
 ui_hud_experiment36c5::Ladder startup{2};assert(startup.Press()==3&&startup.Press()==0); // C26: rung 3 = palm HUD + VR map
 ui_hud_experiment36c5::Ladder forbidden{3};assert(forbidden.Rung()==0); // never auto-open an experimental scene object
 Select(true);PublishAllowed(true,100);assert(Requested()&&Selected()&&Allowed(100)&&Allowed(250)&&!Allowed(251));
 BackEdge back;assert(!back.Filter(false,true));assert(back.Filter(true,true)&&!Requested());
 assert(back.Filter(true,false));assert(!back.Filter(false,false));
 Toggle();assert(Requested());Select(false);Toggle();assert(!Requested());
 Enable(true);SetRequested31(true,1000);assert(Present31(1000));
 BackEdge closing;assert(closing.Filter(true,true,1100)&&!Requested());
 assert(Present31(1100)&&Present31(7099)&&!Present31(7100));
 SetRequested31(true,3000);assert(closingUntil31.load()==0&&Present31(3000));
 Enable(false);assert(!Present31(3001));
 Enable(true);SetRequested31(true,3100);const auto foldSerial39=activation.load();
 SetPrintedRegion(PrintedRegion::Kuttenberg);
 assert(!CloseIfCurrentM39(foldSerial39,3101)&&Requested());
 assert(CloseIfCurrentM39(activation.load(),3102)&&!Requested());
 assert(!CloseIfCurrentM39(activation.load(),3103));
 Enable(false);
 // A held B at activation is not a new action, and never gets an invented release.
 BackEdge carried;assert(!carried.Filter(true,false));Select(true);assert(!carried.Filter(true,true)&&Requested());
 assert(!carried.Filter(false,true));assert(carried.Filter(true,true)&&!Requested());
 PublishAllowed(false,300);assert(!Allowed(300));
 float hand[8]{0,0,0,1,10,20,30,1},out[8]{};
 assert(held_map_pose_m1::Place(hand,out));
 // Native paper grip must land on the actual socket, including world turn.
 constexpr float grip39[]{-.00367412553f,-.002206292f,.1560563594f};
 float point39[3]{};held_map_pose_m1::Point(out,grip39,point39);
 for(int k=0;k<3;++k)assert(std::fabs(point39[k]-hand[k+4])<1e-5);
 float turned[8]{0,0,1,0,10,20,30,1};assert(held_map_pose_m1::Place(turned,out));
 held_map_pose_m1::Point(out,grip39,point39);
 for(int k=0;k<3;++k)assert(std::fabs(point39[k]-turned[k+4])<1e-5);
 hand[3]=1.02f;assert(held_map_pose_m1::Place(hand,out));
 double norm=0;for(int i=0;i<4;++i)norm+=out[i]*out[i];assert(std::fabs(norm-1)<1e-5);
 hand[0]=std::numeric_limits<float>::quiet_NaN();assert(!held_map_pose_m1::Place(hand,out));
 hand[0]=0;hand[3]=0;assert(!held_map_pose_m1::Place(hand,out));
 assert(!held_map_pose_m1::Place(nullptr,out));
 std::puts("PASS held-map controls, context expiry, balanced B, fixed physical pose and invalid-input rejection");
}
