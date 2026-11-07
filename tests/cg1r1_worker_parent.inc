// BEFORE the ordinary pulse/held policies see LEFT Grip. This filters only the
// physical gesture input; the real controller A button is not modified.
static kcd2vr::corpse_grip_cg1::Gate corpseGripGateCg1;
{
 const auto nowCg1=std::uint64_t(GetTickCount64());
 kcd2vr::corpse_grip_cg1::Candidate candidateCg1{};bool inputCurrentCg1=false;
 const bool qualifiedCg1=frik_pose_probe::ReadCorpseGrabCandidateCg1(candidateCg1,nowCg1,inputCurrentCg1);
 tracking59c6::Stamp squeezeStampCg1{};
 const bool squeezeCurrentCg1=xr_manager_&&xr_manager_->LeftController().grip_valid&&
  xr_manager_->CopyInputSampleStamp59c6(squeezeStampCg1)&&
  kcd2vr::corpse_grip_cg1::fresh(squeezeStampCg1.tick,nowCg1)&&
  squeezeStampCg1.epoch==tracking59c6::epoch.load(std::memory_order_acquire);
 const auto outCg1=corpseGripGateCg1.update(nowCg1,inputCurrentCg1&&squeezeCurrentCg1,
  qualifiedCg1&&pi.enabled&&!pi.menu&&pi.hand[1].tracked&&pi.hand[1].empty,
  xr_manager_?xr_manager_->LeftController().grip:0.f,candidateCg1);
 (void)kcd2vr::corpse_grip_cg1::request.publish(outCg1.native);
 if(outCg1.suppressLeftUse)pi.hand[1].tracked=false;
}
