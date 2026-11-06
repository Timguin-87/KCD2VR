#include <cstdio>
struct Hand{float trigger=0;bool active=true;float grip=0;};
static bool consumes=false, minigame=false, fistConsumes=false;
namespace action_map_probe { inline bool NativeMinigame(){return minigame;} }
namespace frik_pose_probe {
static bool PhysicalWeaponConsumesRightTrigger139(){return consumes;}
static float FistTransportTrigger101(int,float v,bool active,bool menu,float){return fistConsumes&&active&&!menu?0.f:v;}
}
struct MockUiPolicy{bool blocks=false; bool BlocksWorldInteraction()const{return blocks;}};
static float Route(bool mis_menu_after_frame,bool game_flat_menu_context,bool immersive_cursor_menu,float raw){Hand routeR{raw},routeL{raw};MockUiPolicy ui89_policy{game_flat_menu_context};
struct {bool nativeControls;} activity163{minigame};
#include "generated_physical_input.inc"
return routeR.trigger;}
int main(){unsigned n=0,f=0;auto check=[&](bool b,const char*s){++n;if(!b)++f;std::printf("%s %s\n",b?"PASS":"FAIL",s);};
 consumes=false;check(Route(false,false,false,.8f)==.8f,"ordinary/melee/native trigger transport unchanged when physical RT is not owned");
 consumes=true;check(Route(false,false,false,.8f)==0,"physical bow trigger ownership masks duplicate game transport");
 check(Route(true,false,false,.8f)==.8f,"VR menu keeps trigger");
 check(Route(false,true,false,.8f)==.8f,"flat game menu keeps trigger");
 check(Route(false,false,true,.8f)==.8f,"immersive contextual menu keeps trigger independently of theater");
 minigame=true;check(Route(false,false,false,.8f)==.8f,"authored minigame keeps native RT even if physical bow reports ownership");
 minigame=false;consumes=false;fistConsumes=true;
 check(Route(false,false,false,.8f)==0,"fresh fist or physical melee owner filters RT before native XI publication");
 check(Route(false,true,false,.8f)==.8f,"UI retains RT despite selected fist mode");
 std::printf("physical_input=%u/%u full_mod_translation_unit=NOT_COMPILED\n",n-f,n);return f?1:0;
}
