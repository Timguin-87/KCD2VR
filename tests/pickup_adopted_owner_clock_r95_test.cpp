// Real owner with one scheduling hook immediately before copying original XR input.
void TestFinishingInputReadR95() noexcept;
#include "pickup_finish_owner_r92r4_test.cpp"
static bool interleaveR95=false;
void TestFinishingInputReadR95() noexcept {
 if(!interleaveR95)return;interleaveR95=false;
 using namespace kcd2vr::action_time_owner59c11;
 ng13::AcquisitionInput55 input{};g_acquisition_input55.read(input);
 ++hostTickCount64;input.tick=hostTickCount64;++input.sample;PublishAcquisitionInput55(input);
}
int main(){using namespace kcd2vr::action_time_owner59c11;
 for(int side=0;side<2;++side){BeginFinishR94(side);CompleteNativeR94();F94(AdoptNativeCleanup19c4());
  ng13::FinishingPresentationR94 before{},after{};F94(ReadAdoptedAttachmentPresentationR95(before));
  interleaveR95=true;F94(ReadAdoptedAttachmentPresentationR95(after));
  F94(after.sourceTick==hostTickCount64&&after.sourceTick>before.sourceTick&&after.requestedAt==before.requestedAt);
  F94(AdoptedAttachmentPresentationCurrentR95(before));
  hostTickCount64+=81;F94(!ReadAdoptedAttachmentPresentationR95(after));
 }
 std::printf("PASS real adopted owner interleaved input clock: %u checks\n",checksFinishR94);return 0;
}
