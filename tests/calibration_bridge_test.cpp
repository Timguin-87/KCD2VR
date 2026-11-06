// COMPLETE production translation unit; only OS/external integration leaves mocked.
// No native fixed-address code is executed by this fixture.
#include "../src/frik_pose_probe.cpp"
#include <cstdio>
#include "temp_ini_fixture_d62.h"
namespace kcd2vr::diagnostic_trace {void Write(const char*) noexcept {}}
namespace kcd2vr::animation_filter {void SetMode(SuppressionMode) noexcept {} bool Install() noexcept {return false;} bool Ready() noexcept {return false;}}
namespace kcd2vr::native_stereo_outer_lab {bool EnsurePhysicalFramePump6698() noexcept {return false;}}
using namespace kcd2vr::frik_pose_probe;
static unsigned checks=0,failures=0;
static void Check(bool b,const char*n){++checks;failures+=!b;std::printf("%s %s\n",b?"PASS":"FAIL",n);}
static fc100::Hand H(int side,double curl){fc100::Hand h;h.count=15;for(int d=0;d<5;++d){h.bend[d]=curl;for(int k=0;k<3;++k){int n=d*3+k;h.joints[n]={100+side*100+n,k?99+side*100+n:1+side,d,unsigned(1+n+side*100),{0,0,std::sin(curl*.25),std::cos(curl*.25)},{.03,0,0}};}}return h;}
int main(){
 TempIniFixtureD62 profile;
 hostIni.clear();hostTickCount64=10000;
 ConfigureCalibrationPersistence(profile.name.c_str());
 Check(!g_saved_arm_ready66100,"missing calibration bank does not fabricate ready anthropometry");
 cp100::Arms arms{1.44,.76,.76,.22},read;
 Check(SaveProfile66100("VRIK.ArmCalibration",arms)&&LoadProfile66100("VRIK.ArmCalibration",read)&&read.span==arms.span,"actual Win32 INI adapter validates readback despite zero flush return");
 ConfigureCalibrationPersistence(profile.name.c_str());
 Check(CalibrationReady()&&g_cal_reanchor66100,"saved arm profile requests lazy session reanchor without T pose");
 TrackingSnapshot ts{};ts.tick=10000;ts.sequence=1;ts.allowed=ts.head_valid=ts.left_valid=ts.right_valid=true;ts.head={2,1.6f,-3};ts.hq={0,0,0,1};
 QuatT28 nativeHead{{0,0,0,1},{.1f,.2f,1.7f}};
 RestoreSessionCalibration66100(ts,nativeHead);
 Check(g_cal_state==2&&!g_cal_reanchor66100&&g_cal_head_x==2&&g_cal_head_z==-3,"restart anchors fresh room coordinates instead of replaying old origin");
 const auto epoch=g_pose_epoch6689.load();ts.head.x=4;ts.hq={0,.7071068f,0,.7071068f};RestoreSessionCalibration66100(ts,nativeHead);
 Check(g_cal_head_x==2&&g_pose_epoch6689==epoch,"ordinary head motion cannot continuously recalibrate controller mapping");
 RequestTrackingReanchor();RestoreSessionCalibration66100(ts,nativeHead);
 Check(g_cal_head_x==4&&!g_cal_reanchor66100&&std::abs(g_cal_frame_qz.load())>.5f,"explicit recenter reanchors once without changing saved reach");
 SetEyeHeightAboveFloor(true,1.2f);g_standing_eye_floor_m107=1.7f;
 RequestTrackingReanchor();RestoreSessionCalibration66100(ts,nativeHead);
 // C43: with VrCalibrationAutoRestore (default) a recenter taken 0.5 m below the standing eye keeps the calibrated height:
 // the body base is the headset corrected by (current - standing). With the setting off the old current-origin rule stands.
 Check(std::abs(g_cal_head_y.load()-(ts.head.y+.5f))<1e-5&&g_standing_eye_floor_m107==1.7f&&std::abs(StandingEyeShift43()+.5f)<1e-6,"C43 camera recenter re-seats the body at the calibrated eye height without erasing standing calibration");
 SetCalibrationAutoRestore43(false);RequestTrackingReanchor();RestoreSessionCalibration66100(ts,nativeHead);
 Check(std::abs(g_cal_head_y.load()-ts.head.y)<1e-6&&StandingEyeShift43()==0.f,"auto restore off: explicit camera recenter seats body at the current HMD origin (pre-C43 rule)");
 ForceCalibrationRestore43();Check(std::abs(StandingEyeShift43()+.5f)<1e-6,"menu RESTORE forces the standing correction once even with auto restore off");
 RequestTrackingReanchor();RestoreSessionCalibration66100(ts,nativeHead);Check(!g_cal_restore_force43&&std::abs(g_cal_head_y.load()-(ts.head.y+.5f))<1e-5,"forced restore consumed by the re-seat");
 SetCalibrationAutoRestore43(true);
 // C43 head anchor: a re-seat keeps the calibrated (F8/INI) anchor over Henry's live stance/knockdown head; without one the live head is latched.
 Check(std::abs(g_cal_native_head_z.load()-1.7f)<1e-6&&!g_cal_anchor_saved43,"no stored anchor yet: live native head latched (pre-C43 rule)");
 g_cal_anchor_x43=.1f;g_cal_anchor_y43=.2f;g_cal_anchor_z43=1.7f;g_cal_anchor_saved43=true;
 QuatT28 stanceHead{{0,0,0,1},{.1f,.0f,1.35f}};RequestTrackingReanchor();RestoreSessionCalibration66100(ts,stanceHead);
 Check(std::abs(g_cal_native_head_z.load()-1.7f)<1e-6&&g_cal_drift_count43==1,"re-seat during a combat stance (-35 cm head) keeps the calibrated anchor and reports the drift");
 g_cal_native_head_z=1.35f;Check(RestoreCalibration43("TEST_EXIT")&&std::abs(g_cal_native_head_z.load()-1.7f)<1e-6&&g_cal_drift_count43==2,"owner exit restore re-asserts the calibrated anchor (drift receipt)");
 Check(!RestoreCalibration43("TEST_EXIT_AGAIN"),"owner exit bursts are debounced (one restore per 500 ms)");
 hostTickCount64+=600;ts.tick=hostTickCount64;Check(RestoreCalibration43("TEST_EXIT_LATER")&&g_cal_drift_count43==2,"a later exit restores again, no drift when the anchor already matches");
 hostTickCount64=10000;ts.tick=10000;
 GuidedCalAccum pitched{};pitched.n=100;pitched.haveQ=true;
 pitched.h={0,170,0};pitched.l={-72,150,0};pitched.r={72,150,0};
 pitched.hqSum={-38.268343f,0,0,92.387953f};pitched.lqSum=pitched.rqSum={0,0,0,100};
 Check(CommitGuidedCalibrationV6562(pitched,{0,0,1.7f})&&std::abs(g_cal_hq_x.load())<1e-6&&std::abs(g_cal_frame_qx.load())<1e-6,
       "F8 looking down cannot persist pitch as the body neutral orientation");
 RequestTrackingReanchor();ts.hq={0,0,0,0};RestoreSessionCalibration66100(ts,nativeHead);Check(g_cal_reanchor66100,"invalid HMD quaternion cannot restore calibration");ts.hq={0,0,0,1};ts.tick=1;RestoreSessionCalibration66100(ts,nativeHead);Check(g_cal_reanchor66100,"stale HMD sample cannot restore calibration");ts.tick=10000;RestoreSessionCalibration66100(ts,nativeHead);
 SetCombatCensusMode(0);bool ladder=true;
 const char*expected[]{"1","2","3","4","0","1","2","3","4","0"};
 for(const char* x:expected){CycleCombatCensusMode();ladder=ladder&&std::strcmp(CombatCensusRungLabel(CombatCensusMode()),x)==0;}
 Check(ladder,"actual public F10 ladder skips recorder and reaches OFF twice without native callbacks");
 BeginFingerCapture();SetCombatCensusMode(0);Check(!FingerCaptureActive()&&CombatCensusMode()==0,"explicit OFF cancels finger capture");
 fc100::Pair pair;pair.rig=123;pair.open={H(0,0),H(1,0)};pair.closed={H(0,2),H(1,1.8)};
 BeginFingerCapture();FingerPacket66100 pkt{pair,0,g_capture_session66100.load(),hostTickCount64};g_finger_candidate66100.publish(pkt);g_finger_candidate_ready66100=true;
 CycleCombatCensusMode();ServiceCalibrationIo66100();
 fc100::Pair saved;Check(CombatCensusMode()==2&&!FingerCaptureActive()&&LoadProfile66100("VRIK.Fingers.000000000000007B",saved),"F10 previews qualified numeric candidate and worker persists both hands");
 Check(saved.rig==123&&saved.closed[1].bend[1]==1.8&&CalibrationPersistenceStatus()==2,"INI contains independent left-hand and right-hand endpoints");
 auto handBank=hostIni;
 BeginArmLengthCalibration();BeginGuidedCalibrationPose(1);
 TrackingPose hp{},lp{},rp{};hp.qw=lp.qw=rp.qw=1;hp.py=1.7f;lp.px=-.72f;rp.px=.72f;lp.py=rp.py=1.48f;
 for(int n=0;n<101;++n){hostTickCount64=11000+n*20;PublishTrackingSnapshot(hp,true,lp,true,rp,true,0,0,0,0,true);}
 Check(CompleteArmLengthCalibration()&&CalibrationReady(),"actual current two-second calibration commits tracked T-pose means");
 ServiceCalibrationIo66100();Check(LoadProfile66100("VRIK.ArmCalibration",read)&&std::abs(read.span-1.44)<1e-5,"successful current calibration saves measured span to INI");
 bool fingersSame=true;for(const auto&kv:handBank)if(kv.first.find("VRIK.Fingers.")!=std::string::npos)fingersSame=fingersSame&&hostIni[kv.first]==kv.second;
 Check(fingersSame&&LoadProfile66100("VRIK.Fingers.000000000000007B",saved),"recalibrating reach never overwrites native finger bank");
 BeginFingerCapture();Check(FingerCaptureActive()&&std::strcmp(CombatCensusRungLabel(CombatCensusMode()),"1.5")==0,"explicit capture command remains available outside ordinary F8/F10 cycling");
 CycleCombatCensusMode();Check(CombatCensusMode()==2&&!FingerCaptureActive(),"incomplete recapture can leave 1.5 and use previously saved pair");
 BeginArmLengthCalibration();BeginGuidedCalibrationPose(1);for(int n=0;n<101;++n){hostTickCount64=14000+n*20;PublishTrackingSnapshot(hp,true,lp,n!=50,rp,true,0,0,0,0,true);}
 Check(!CompleteArmLengthCalibration()&&g_saved_arm_ready66100,"single tracking loss rejects recalibration but preserves saved reach");
 BeginArmLengthCalibration();BeginGuidedCalibrationPose(1);hp.qw=0;for(int n=0;n<101;++n){hostTickCount64=17000+n*20;PublishTrackingSnapshot(hp,true,lp,true,rp,true,0,0,0,0,true);}
 Check(!CompleteArmLengthCalibration(),"invalid orientation cannot be normalized into a successful calibration");hp.qw=1;
 BeginArmLengthCalibration();CancelArmLengthCalibration();Check(g_saved_arm_ready66100&&!g_arm_capture_active66100,"cancelled countdown leaves prior persistent reach intact");
 ResetCalibration();Check(CalibrationReady()&&g_cal_reanchor66100,"generic runtime reset requests reanchor rather than repeat personal calibration");
 hostIniFailWrites=true;cp100::Arms next=arms;next.span=1.55;Check(!SaveProfile66100("VRIK.ArmCalibration",next)&&LoadProfile66100("VRIK.ArmCalibration",read)&&std::abs(read.span-1.44)<1e-5,"actual failed INI write does not erase previous calibration");hostIniFailWrites=false;
 BeginFingerCapture();pkt.session=g_capture_session66100;pkt.tick=hostTickCount64-200;g_finger_candidate66100.publish(pkt);g_finger_candidate_ready66100=true;CycleCombatCensusMode();Check(CombatCensusMode()==2&&FingerCaptureStatus()==0,"stale preview candidate is not promoted and cannot trap ladder");

 // R2: recover old raw-float-hash sections by structural and fixed-geometry checks.
 hostIni.clear();pair.rig=0xDEADBEEF;Check(SaveProfile66100("VRIK.Fingers.00000000DEADBEEF",pair),"legacy bank fixture saved through actual transport");
 FingerRigRequest101R2 descriptor{};descriptor.rig=0x12345678;descriptor.hand=pair.open;
 for(auto& hand:descriptor.hand)for(int j=0;j<hand.count;++j){hand.joints[j].q={0,0,0,-1};hand.joints[j].t.x+=1e-7;}
 const auto originalIni=hostIni;fc100::Pair recovered{};
 Check(RecoverFingerPair101R2("VRIK.Fingers.0000000012345678",descriptor,recovered)==1&&EquivalentFingerPair101R2(pair,recovered),"old hash recovers exact endpoint rotations despite equivalent rest quaternion sign and floating noise");
 Check(hostIni==originalIni,"legacy recovery does not rewrite or delete accepted finger banks");
 auto wrong=descriptor;wrong.hand[0].joints[0].crc++;
 Check(RecoverFingerPair101R2("none",wrong,recovered)==0,"matching geometry cannot override a different joint identity");
 wrong=descriptor;wrong.hand[1].joints[2].t.x+=.02;
 Check(RecoverFingerPair101R2("none",wrong,recovered)==0,"incompatible finger length rejected rather than stretched");
 auto other=pair;other.rig=0xABC;other.open[0].joints[0].q=pc::from_rotation_vector({.02,0,0});
 Check(SaveProfile66100("VRIK.Fingers.0000000000000ABC",other)&&RecoverFingerPair101R2("none",descriptor,recovered)==-1,"conflicting compatible saved profiles are reported ambiguous, not silently selected");
 hostIni=originalIni;g_finger_rig_descriptor101R2.publish(descriptor);g_finger_rig_request66100=descriptor.rig;ServiceCalibrationIo66100();
 FingerPacket66100 migrated{};g_finger_saved66100.read(migrated);
 Check(g_finger_profile_status101R2==1&&migrated.pair.rig==descriptor.rig&&EquivalentFingerPair101R2(migrated.pair,pair),"actual worker publishes compatible legacy profile for the stable new rig key");
 std::array<unsigned char,64> bytes{};std::uint32_t value=7;std::memcpy(bytes.data(),&value,4);std::uint32_t readValue=0;
 ResetPageMemo169(); // isolate this measurement from earlier stack-page reads
 const auto beforeQ=g_read_queries101R2.load(),beforeReuse=g_read_hits101R2.load();bool readOK=true;
 {ReadScope101R2 scope;for(int i=0;i<100;++i)readOK=Read(bytes.data(),readValue)&&readValue==7&&readOK;
  value=9;std::memcpy(bytes.data(),&value,4);readOK=Read(bytes.data(),readValue)&&readValue==9&&readOK;}
 Check(readOK&&g_read_queries101R2-beforeQ==1&&g_read_hits101R2-beforeReuse==100,"101 guarded reads share one page query but always reread actual data");
 const auto nextQ=g_read_queries101R2.load();value=11;std::memcpy(bytes.data(),&value,4);
 {ReadScope101R2 scope;Read(bytes.data(),readValue);}
 Check(g_read_queries101R2==nextQ&&readValue==11,"V169 short page memo saves the query but always rereads object contents");
 hostTickCount64+=151;
 {ReadScope101R2 scope;Read(bytes.data(),readValue);}
 Check(g_read_queries101R2-nextQ==1,"expired 150ms page memo revalidates the page on the next callback");
 Check(!Safe(bytes.data(),SIZE_MAX),"overflowing memory span rejected");
 std::printf("calibration_bridge=%u/%u native fixed-address calls=NOT_EXECUTED\n",checks-failures,checks);return failures?1:0;
}
