#define main retained_physical_contract175_h26
#include "physical_contract175_test.cpp"
#undef main
int main(){checks=failures=0;
 MeleeReplay r;r.in.windupHold57=r.in.thrustAsSwing55=r.in.physicalCombat177=true;r.in.strikeMinScale178=0;
 for(int n=0;n<30;++n)r.move({});r.commands.clear();
 r.in.trigger[0]=1;r.move({});
 ck(r.count(CommandKind::Prepare)==1&&r.state.weapon[0].cocked57.controlledH26,"H26 explicit trigger starts one controlled native held preparation");
 for(int n=0;n<80;++n)r.move({});
 ck(r.count(CommandKind::Prepare)==1&&r.count(CommandKind::Slash)==0,"H26 held trigger stays one prepare without stationary attack release");
 r.in.trigger[0]=0;r.move({});
 ck(r.count(CommandKind::Abort)==1&&r.count(CommandKind::Slash)==0&&!r.state.weapon[0].prepared,"H26 trigger-up cancels own stationary prepare, no attack");
 r.commands.clear();for(int n=0;n<40;++n)r.move({});
 ck(r.count(CommandKind::Prepare)==0,"H26 resting two-hand-independent pose never autoprepares");
 r.in.trigger[0]=1;r.move({});for(int n=0;n<330;++n)r.move({});
 ck(r.count(CommandKind::Prepare)==1&&r.count(CommandKind::Abort)==1,"H26 manualhold timeout never autoreprepares while same trigger held");
 r.in.trigger[0]=0;r.move({});r.in.trigger[0]=1;r.move({});
 ck(r.count(CommandKind::Prepare)==2,"H26 explicit release/repress rearms one new preparation");

 MeleeReplay f;f.in.windupHold57=f.in.thrustAsSwing55=f.in.physicalCombat177=true;f.in.strikeMinScale178=0;
 f.in.hands[0].p={.3,-.06,1.25};f.in.intentHands[0]=f.in.hands[0];for(int n=0;n<30;++n)f.move({});f.commands.clear();
 f.in.trigger[0]=1;f.move({});for(int n=0;n<40;++n)f.move({});
 for(int n=0;n<60;++n)f.move({-.01,0,0});for(int n=0;n<34;++n)f.move({});
 int redirects=0;for(const auto& c:f.commands)redirects+=c.kind==CommandKind::Zone57&&c.cutFeint62&&c.controlledHoldH26;
 ck(redirects==1&&f.count(CommandKind::Prepare)==1&&f.count(CommandKind::Slash)==0,"H26 deliberate cross-zone held motion emits one tagged native redirect, not a swing");
 for(int n=0;n<20;++n)f.move({0,.025,0});
 bool release=false;for(const auto& c:f.commands)release|=(c.kind==CommandKind::Thrust||c.kind==CommandKind::Slash)&&c.held57;
 ck(release&&f.count(CommandKind::Prepare)==1,"H26 real forward stroke releases existing held preparation without second input");

 MeleeReplay loss;loss.in.windupHold57=loss.in.thrustAsSwing55=loss.in.physicalCombat177=true;
 loss.in.trigger[0]=1;loss.state={};loss.commands.clear();for(int n=0;n<40;++n)loss.move({});
 ck(loss.count(CommandKind::Prepare)==0,"H26 trigger already down on acquisition does not prepare");
 loss.in.trigger[0]=0;loss.move({});loss.in.trigger[0]=1;loss.move({});
 loss.in.allowed=false;loss.move({});loss.in.allowed=true;loss.commands.clear();for(int n=0;n<40;++n)loss.move({});
 ck(loss.count(CommandKind::Prepare)==0,"H26 context reacquisition while trigger held cannot autoplay another prepare");
 std::printf("H26 manual held producer=%u/%u\n",checks-failures,checks);return failures?1:0;}
