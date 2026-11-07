// Executes actual completed-source producer with candidate activation=true.
// The runner changes ONLY integrated=false to true in a copied public header.
#define main retained_legacy_producer_main
#include "pending_reach_t23_test.cpp"
#undef main
int main(){
 kcd2vr::weapon_release_t1::SetMode(0); // RIGHT auto-holster remains independent
 setup();
 const auto a=ph::body_axes(frame.input);
 const auto chest=(frame.input.shoulders[0]+frame.input.shoulders[1])*.5-a.up*.18+a.forward*.12;
 frame.input.hands[1].p=chest;equipment.handKey[0]=1234;
 frame.input.grip[0]=frame.input.trigger[0]=1;
 publish();CHECK(lastInput.chestKnivesT26&&lastInput.allowed);
 CHECK(lastInput.near18[1][0]&&!lastInput.near18[0][0]&&!lastInput.near18[0][1]&&!lastInput.near18[1][1]);
 CHECK(lastInput.hand[0]==1234&&!lastInput.hand[1]);
 CHECK(!lastInput.pouchNear[0]&&!lastInput.pouchNear[1]);
 auto source=lastInput;source.chestKnivesT26=false;
 CHECK(!t23::SameFrame(source,lastInput));
 frame.input.grip[1]=frame.input.trigger[1]=1;advance();publish();CHECK(!lastInput.near18[1][0]);
 frame.input.trigger[1]=.60;advance();publish();CHECK(lastInput.near18[1][0]);
 equipment.handKey[1]=4321;advance();publish();CHECK(!lastInput.near18[1][0]);
 equipment.handKey[1]=0;frame.input.trigger[1]=0;
 frame.input.hands[1].p=chest+a.forward*.17;advance();publish();CHECK(!lastInput.near18[1][0]);
 frame.input.hands[1].p=chest;advance();publish();CHECK(lastInput.near18[1][0]);
 // An active RIGHT physics owner is not cancelled by LEFT stock admission.
 auto p=request(900,1);CHECK(t23::PublishPending(p));busy=true;retireCalls=0;physicsControl=1;
 publish();CHECK(t23::CurrentGrant(p,lastInput,now)&&!retireCalls&&!(physicsControl&8));CHECK(t23::ClearPending(p));
 packetRead=false;advance();publish();CHECK(!lastInput.allowed&&!lastInput.near18[1][0]);
 std::printf("{\"checks\":%u,\"actual_completed_producer\":true,\"activation_header_only\":true,\"native_execution\":false}\n",checks);
}
