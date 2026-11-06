#include "dialogue_pose47c1.h"
#include "dialogue_presentation19c2.h"
#include "vrik/visual_selection59c7f.h"
#include <cassert>
#include <cstdio>
using namespace kcd2vr;
namespace fixture {
bool vrik_in_dialogue_=true,vrik_probe_installed=false,safe_ui_fit=false,matrix_active=false,mis_menu_after_frame=false;
bool calibrated=false,reference=false,activity=false;int census=0,skin=0,dialogue_mode_now=2;
unsigned bodyReads=0,cameraRequests=0;std::uint64_t now=100;
ui_menu_router::Decision ui89_policy{};
dialogue_presentation19c2::Request request;
std::uint64_t GetTickCount64(){return now;}
namespace frik_pose_probe {
bool CalibrationReady(){++bodyReads;return calibrated;}
int CombatCensusMode(){++bodyReads;return census;}
int FinalSkinTestMode(){++bodyReads;return skin;}
}
namespace diagnostics::lab {bool NativeReference(){return reference;}}
namespace activity_input163 {struct Witness {bool Active(){return activity;}} witness175;}
namespace activity_lens59c10r1 {
void RequestDialogueFirstPerson19c2(bool wanted,std::uint64_t tick){++cameraRequests;request={tick,wanted};}
}
bool produce(){
#include "../src/dialogue_policy28.inl"
return dialogue_pose47c1::Allow(dialoguePolicy28);
}
void reset(){vrik_in_dialogue_=true;vrik_probe_installed=false;safe_ui_fit=matrix_active=mis_menu_after_frame=false;
 calibrated=reference=activity=false;census=skin=0;dialogue_mode_now=2;bodyReads=cameraRequests=0;
 now=100;ui89_policy={};request={};}
}
int main(){using namespace fixture;
 reset();assert(!produce()&&request.wanted&&request.tick==100&&bodyReads==0&&cameraRequests==1);
 // The optional dialogue body setting cannot cancel the explicit camera mode.
 reset();vrik_in_dialogue_=false;assert(!produce()&&request.wanted);
 reset();vrik_probe_installed=true;assert(!produce()&&request.wanted&&bodyReads==3);
 reset();vrik_probe_installed=true;calibrated=true;census=4;assert(produce()&&request.wanted);
 vrik_in_dialogue_=false;assert(!produce()&&request.wanted);
 calibrated=false;assert(!produce()&&request.wanted);
 for(int mode:{0,1}){reset();dialogue_mode_now=mode;assert(!produce()&&!request.wanted);}
 for(auto flag:{&safe_ui_fit,&matrix_active,&mis_menu_after_frame,&reference,&activity}){
  reset();*flag=true;assert(!produce()&&!request.wanted);
 }
 for(auto owner:{ui_menu_router::Owner::InventoryMap,ui_menu_router::Owner::Shop,
    ui_menu_router::Owner::Pause,ui_menu_router::Owner::NativeContext,
    ui_menu_router::Owner::GameOver,ui_menu_router::Owner::LoadingScreen}){
  reset();ui89_policy.owner=owner;assert(!produce()&&!request.wanted);
 }
 reset();ui89_policy.uncertain=1;assert(!produce()&&!request.wanted);
 reset();produce();dialogue_state59c10r2::State state{};
 // Intent alone never grants a camera: actual selector uses fresh native owner.
 assert(!dialogue_presentation19c2::chooseFirstPerson(request,state,now));
 state.tick=now;state.entity=1;state.known=state.active=true;state.camera=dialogue_state59c10r2::Camera::FirstPerson;
 assert(dialogue_presentation19c2::chooseFirstPerson(request,state,now));
 state.active=false;assert(!dialogue_presentation19c2::chooseFirstPerson(request,state,now));
 state.active=true;assert(!dialogue_presentation19c2::chooseFirstPerson(request,state,181));
 std::puts("dialogue_camera_policy28: actual Mod producer camera/body independence and native admission PASS");
}
