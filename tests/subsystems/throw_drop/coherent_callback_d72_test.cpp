// Actual completed source publisher and admission API, with the existing
// explicit storage/native leaves. Prior legacy-route fixture is not executed.
#define main PreviousPendingReachMainD72
#include "pending_reach_t23_test.cpp"
#undef main
int main(){
 setup();frame.input.hands[1].p=ph::knife_pouch_d66(frame.input,false);publish();
 CHECK(lastInput.chestKnivesT26&&lastInput.near18[1][0]&&!lastInput.near18[0][0]);
 t23::Source captured{};CHECK(t23::CaptureSourceD72(captured,now));const auto original=lastInput;
 advance();completedPublication+=2;frame.publicationT23=completedPublication;packet.allowed[1].t[0]=6;publish();
 CHECK(!t23::CurrentSource(original,now));
 {t23::SourceScopeD72 callback(captured);
  CHECK(t23::CurrentSource(original,now));
  auto forged=original;forged.wrist18[1][4]=6;CHECK(!t23::CurrentSource(forged,now));
  t23::Pending pending{original,900,1,0};CHECK(t23::PublishPending(pending,now));CHECK(t23::ClearPending(pending));
  equipment.handKey[1]=123;publish();CHECK(!t23::PublishPending({original,901,1,0},now));
  equipment.handKey[1]=0;publish();CHECK(t23::CurrentSource(original,now));
  CHECK(!t23::CurrentSource(original,original.tick+81));
  CHECK(t23::PublishLive({}));CHECK(!t23::CurrentSource(original,now));
 }
 CHECK(!t23::CurrentSource(original,now));
 std::printf("D72 coherent callback LEFT chest source %u checks PASS; native leaves mocked\n",checks);
}
