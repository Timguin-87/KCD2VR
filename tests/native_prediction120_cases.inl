hostRetailVerified112=true;g_prediction_contract120=base;g_prediction_fault120=false;
g_prediction_original120=&predictionModel120;g_interpolate_original120=&interpolateModel120;
// Diagnostic total assertions below explicitly own a live F3 observation lease.
g_contact_until114=GetTickCount64()+diagnostics::capture168::Duration;
diagnostics::lab::Set(diagnostics::lab::Key::F3,0);g_weapon_control_mode=7;
put(motionAction.data(),player.data());put(motionAction.data()+0xAC,nativeCurrent);
QuatT28 identity120{{0,0,0,1},{0,0,0}};
put(motionAction.data()+0x20,identity120);put(motionAction.data()+0x3C,identity120);
struct Wrapper147 {void* action; unsigned char reserved[8]{}; unsigned char admitted=1;} wrapper120{motionAction.data()};
put(motionAction.data()+0x411,std::uint8_t(1));put(motionAction.data()+0x2D0,std::uint8_t(0));
const auto savedAction120=motionAction;
auto runPrediction120=[&](){motionFrame(true);return PredictionRun120(&wrapper120,.011f,base+0xA7CDEE);};
ck(runPrediction120()==reinterpret_cast<void*>(0x1234),"prediction first correlation preserves original result");
ck(runPrediction120()==reinterpret_cast<void*>(0x1234),"prediction second fresh correlation remains native");
// An NPC prediction between Henry's samples must not starve acquisition.
std::array<unsigned char,0x420> npcAction120{};put(npcAction120.data(),entity.data());void* npcWrapper120=npcAction120.data();
ck(PredictionRun120(&npcWrapper120,.011f,base+0xA7CDEE)==reinterpret_cast<void*>(0x1234),"NPC prediction returns original result without claiming Henry scope");
ck(runPrediction120()==reinterpret_cast<void*>(0x5678),"F12=7 alone admits tracked prediction after three fresh correlations");
ck(g_prediction_scope120==nullptr,"prediction stack input lifetime ends on native return");
ck(std::abs(sweepCurrent120.t[0]-sweepPrevious120.t[0]-.02f)<1.e-5,"pure translation reaches native sweep without authored rotation");
ck(driverAngle120>.009&&driverAngle120<.011,"straight thrust drives the original loop above its zero-angle cutoff");
ck(!std::memcmp(&setupPrevious120,&sweepPrevious120,sizeof(QuatT28)),"geometry setup and sweep use identical previous pose");
ck(motionAction==savedAction120,"real action data remains byte-for-byte unchanged");
#include "attack_stage176_cases.inl"
PredictionRun120(&wrapper120,.011f,base+0xA7CDEE);
ck(!std::memcmp(&sweepPrevious120,&sweepCurrent120,sizeof(QuatT28)),"duplicate callback cannot replay tracked displacement");
frame.output.weapons[0].q=pm::from_rotation_vector({0,0,.4});runPrediction120();
ck(pm::angle(PhysPose6698(sweepCurrent120).q,frame.output.weapons[0].q)<1.e-6,"roll-only tracked rotation survives native direction-only predictor");
ck(std::abs(driverAngle120-.4)<1.e-5,"roll-only motion requests one native subdivision");
PredictionScope120 active120{};active120.wrapper=&wrapper120;active120.action=motionAction.data();active120.redirected=true;
active120.input.previous=identity120;active120.input.target={{0,0,0,1},{1,0,0}};
{
 PredictionScopeGuard120 guard120(&active120);
 QuatT28 out120{};float axis120[]{1,0,0};
 PredictionInterpolate120(base+0xA7C96D,&out120,&active120.input.previous,&active120.input.target,axis120,.1f,.5f);
 ck(std::abs(out120.t[0]-.5)<1.e-6,"interpolation honors native subdivision fraction");
 PredictionInterpolate120(base+0xA7C96E,&out120,&active120.input.previous,&active120.input.target,axis120,.1f,.5f);
 ck(out120.t[0]==99,"unrelated interpolation caller remains native");
 PredictionInterpolate120(base+0xA7C96D,&out120,&active120.input.previous,&active120.input.target,axis120,.1f,2.f);
 ck(g_prediction_fault120.load()&&out120.t[0]==0,"impossible interpolation fraction freezes motion and latches experiment off");g_prediction_fault120=false;
 DetourModKit::hook::MidContext wrong120{};active120.redirected=false;
 DetourModKit::hook::gpr(wrong120,DetourModKit::hook::Gpr::Rdi)=reinterpret_cast<std::uintptr_t>(active120.action);
 PredictionMid120(wrong120);ck(!active120.redirected,"wrong wrapper register cannot acquire shadow inputs");
 PredictionScopeGuard120 nested120(nullptr);ck(g_prediction_scope120==nullptr,"nested unrelated prediction masks Henry scope");
}
ck(g_prediction_scope120==nullptr,"nested scope restores outer then original TLS owner");
g_weapon_control_mode=6;ck(runPrediction120()==reinterpret_cast<void*>(0x1234),"F12=6 preserves original prediction for comparison");g_weapon_control_mode=7;
runPrediction120();runPrediction120();runPrediction120();
g_right_valid=false;ck(runPrediction120()==reinterpret_cast<void*>(0x1234),"tracking loss revokes prediction immediately");g_right_valid=true;
ck(runPrediction120()==reinterpret_cast<void*>(0x1234),"tracking recovery must reacquire pivot proof");runPrediction120();runPrediction120();
put(motionAction.data()+0xAC,QuatT28{{0,0,0,1},{.1f,1,0}});
ck(runPrediction120()==reinterpret_cast<void*>(0x1234),"wrong authored bone pivot fails closed");put(motionAction.data()+0xAC,nativeCurrent);
runPrediction120();runPrediction120();runPrediction120();
put(motionAction.data()+0x3C,QuatT28{{0,0,0,1},{.1f,0,0}});
ck(runPrediction120()==reinterpret_cast<void*>(0x1234),"geometry setup inverse mismatch fails closed");put(motionAction.data()+0x3C,identity120);
runPrediction120();runPrediction120();runPrediction120();
put(motionAction.data(),entity.data());ck(runPrediction120()==reinterpret_cast<void*>(0x1234),"foreign action cannot borrow Henry tracked motion");put(motionAction.data(),player.data());
runPrediction120();runPrediction120();runPrediction120();
put(motionAction.data()+0x308,std::uint8_t(1));ck(runPrediction120()==reinterpret_cast<void*>(0x1234),"already-hit action is not rearmed");put(motionAction.data()+0x308,std::uint8_t(0));
runPrediction120();runPrediction120();runPrediction120();
diagnostics::lab::Set(diagnostics::lab::Key::F3,1);ck(runPrediction120()==reinterpret_cast<void*>(0x1234),"reference mode vetoes F12 prediction");diagnostics::lab::Set(diagnostics::lab::Key::F3,0);
runPrediction120();runPrediction120();runPrediction120();
g_ctx_cutscene=true;ck(runPrediction120()==reinterpret_cast<void*>(0x1234),"cutscene retains native prediction");g_ctx_cutscene=false;
animation_filter::nativeBodyOwned112=true;ck(runPrediction120()==reinterpret_cast<void*>(0x1234),"hit reaction and paired body retain native prediction");animation_filter::nativeBodyOwned112=false;
// V125 protected animation is read-only evidence, never a warmup/write lease.
animation_filter::nativeBodyOwned112=true;motionFrame(true);PredictionScope120 protected125{};
ck(!PredictionPrepare120(&wrapper120,.011f,base+0xA7CDEE,protected125)&&protected125.nativeBodyRequired125&&
    protected125.pivotDistance>=0&&!std::strcmp(protected125.context121,"NATIVE_BODY"),
    "V125 protected body exposes measured pivot correlation without granting motion ownership");
ck(g_prediction_lease120.correlated==0&&!protected125.redirected,"V125 read-only protected correlation cannot seed a later write lease");
animation_filter::nativeBodyOwned112=false;
put(state.data()+0x218,5);ck(runPrediction120()==reinterpret_cast<void*>(0x1234),"special attack is not rewritten as ordinary melee");put(state.data()+0x218,1);
hostRetailVerified112=false;ck(runPrediction120()==reinterpret_cast<void*>(0x1234),"fingerprint mismatch vetoes prediction");hostRetailVerified112=true;
runPrediction120();runPrediction120();runPrediction120();
frame.output.weapons[0].p.x+=1;ck(runPrediction120()==reinterpret_cast<void*>(0x1234),"teleport cannot produce a meter-long damage sweep");
runPrediction120();runPrediction120();runPrediction120();
ck(PredictionRun120(&wrapper120,.1f,base+0xA7CDEE)==reinterpret_cast<void*>(0x1234),"large native dt remains native");
ck(PredictionRun120(&wrapper120,.011f,base+0xA7CDEF)==reinterpret_cast<void*>(0x1234),"unknown prediction caller stays native");
ck(predictionCalls120>0&&interpolationCalls120>0,"test executes production hooks with explicit native replacements");
// V121 reason labels must distinguish failures without changing admission.
PredictionScope120 reason121{};NativeOwner104 owner121{};PhysFrame6698 checked121{};
g_right_valid=false;
ck(!PredictionContext120(checked121,owner121,reason121)&&!std::strcmp(reason121.context121,"TRACKING"),"prediction reports tracking gate separately");
g_right_valid=true;motionFrame(true);frame.output.contactKnown[0]=false;frame.output.sweepDeferred115[0]=true;g_phys_frame6698.publish(frame);
ck(!PredictionContext120(checked121,owner121,reason121)&&!std::strcmp(reason121.context121,"CONTACT_DEFERRED"),"prediction distinguishes rotation deferral from a native attack gate");
frame.output.contactPrefixKnown126[0]=true;g_phys_frame6698.publish(frame);
ck(PredictionContext120(checked121,owner121,reason121),"F12=7 may use the already tested applied prefix while the remainder stays deferred");
frame.output.contactPrefixKnown126[0]=false;
frame.output.contactKnown[0]=true;frame.output.sweepDeferred115[0]=false;g_phys_frame6698.publish(frame);put(state.data()+0x2D8,std::uint8_t(0));
ck(PredictionContext120(checked121,owner121,reason121),"pre-strike callback may collect read-only context");
g_prediction_lease120={};const auto segmentsBefore122=g_prediction_segments120.load();
ck(runPrediction120()==reinterpret_cast<void*>(0x1234),"cleared request-candidate still requires first correlation");
ck(runPrediction120()==reinterpret_cast<void*>(0x1234),"cleared request-candidate still requires second correlation");
ck(runPrediction120()==reinterpret_cast<void*>(0x5678),"native admitted execution redirects with request-candidate cleared");
ck(g_prediction_lease120.correlated==3&&g_prediction_segments120.load()>segmentsBefore122,
    "three fresh correlations plus native admission grant the scope");
put(state.data()+0x2D8,std::uint8_t(1));
ck(runPrediction120()==reinterpret_cast<void*>(0x5678),"request-candidate does not replace original caller admission");
ck(std::abs(sweepCurrent120.t[0]-sweepPrevious120.t[0]-.02f)<1.e-5,"state change does not replay the backswing");
wrapper120.admitted=0;
ck(runPrediction120()==reinterpret_cast<void*>(0x1234)&&g_prediction_lease120.correlated==0,
    "revoking native wrapper admission immediately stops replacement and destroys the lease");
wrapper120.admitted=1;
g_right_valid=false;runPrediction120();g_right_valid=true;
ck(g_prediction_lease120.correlated==0,"tracking loss destroys pre-strike evidence as well as active write ownership");
put(state.data()+0x2D8,std::uint8_t(1));
ck(runPrediction120()==reinterpret_cast<void*>(0x1234),"active attack after tracking recovery cannot reuse lost correlation");
runPrediction120();runPrediction120();runPrediction120();
++captured.nativeStamp.sample;g_phys_grips6698.publish(grips);
ck(PredictionRun120(&wrapper120,.011f,base+0xA7CDEE)==reinterpret_cast<void*>(0x5678),
    "newer independent grip snapshot cannot invalidate committed correlation provenance");
socket.relative.t[0]=.02f;g_phys_sockets101.publish(sockets);PredictionScope120 changed126{};
ck(!PredictionPrepare120(&wrapper120,.011f,base+0xA7CDEE,changed126)&&changed126.reason==5,
    "changed live socket topology from the same callback still revokes captured correlation");
socket.relative.t[0]=0;socket.callback=77;g_phys_sockets101.publish(sockets);
motionFrame(true);runPrediction120();motionFrame(true);runPrediction120();motionFrame(true);runPrediction120();
// Keep the committed capture at callback 77; advance only the async mailbox.
socket.relative.t[0]=.02f;socket.callback=78;g_phys_sockets101.publish(sockets);PredictionScope120 newer126{};
ck(PredictionPrepare120(&wrapper120,.011f,base+0xA7CDEE,newer126),
    "newer attachment callback topology does not invalidate committed captured correlation");
socket.callback=77;socket.relative.t[0]=0;g_phys_sockets101.publish(sockets);motionFrame(true);
++frame.correlation126[0].stamp.sample;g_phys_frame6698.publish(frame);PredictionScope120 mixed126{};
ck(!PredictionPrepare120(&wrapper120,.011f,base+0xA7CDEE,mixed126)&&mixed126.reason==5,
    "mixed committed frame and captured source samples fail closed");
// V126 native source identity is checked in the original synchronous update,
// independent of the earlier committed animation pose. These are production
// observers over synthetic memory, not a retail scheduling claim.
motionFrame(true);g_prediction_lease120={};
std::array<std::uintptr_t,12> poseVft126{};
poseVft126[2]=base+kPoseGetterBuffer0Rva;poseVft126[10]=base+kPoseGetterBuffer1Rva;
put(character.data(),base+kCCharInstanceVftRva);
put(character.data()+kSkeletonAnimOffset,base+kCSkeletonAnimVftRva);
auto* sourcePose126=character.data()+kPoseInCharInstanceOffset;put(sourcePose126,poseVft126.data());
QuatT28 laterBone126=nativeCurrent;laterBone126.t[0]=.25f;
put(motionAction.data()+0xAC,laterBone126);
auto sourceFrame126=[&](){
 motionFrame(true);PredictionSourceBegin126(producer.data(),sourcePose126);
 PredictionSourceCapture126(motionAction.data(),sourcePose126,&laterBone126);
 return PredictionRun120(&wrapper120,.011f,base+0xA7CDEE);
};
ck(sourceFrame126()==reinterpret_cast<void*>(0x1234),"same-update source proof still requires first fresh warmup");
ck(sourceFrame126()==reinterpret_cast<void*>(0x1234),"same-update source proof still requires second fresh warmup");
ck(sourceFrame126()==reinterpret_cast<void*>(0x5678),"same-update exact collision bone admits motion despite older committed authored pose");
PredictionScope120 observedSource126{};
ck(PredictionPrepare120(&wrapper120,.011f,base+0xA7CDEE,observedSource126)&&observedSource126.sourceCorrelated126&&
 observedSource126.framePivotM126>.24&&observedSource126.pivotDistance<.002,
 "source proof reports animation-time mismatch separately from strict same-update equality");
const auto capturedSource126=g_prediction_source126;
g_prediction_source126.bone=18;
ck(!PredictionPrepare120(&wrapper120,.011f,base+0xA7CDEE,observedSource126),"source for a different collision bone cannot authorize writes");
g_prediction_source126=capturedSource126;g_prediction_source126.character=entity.data();
ck(!PredictionPrepare120(&wrapper120,.011f,base+0xA7CDEE,observedSource126),"another character source cannot authorize Henry trajectory");
g_prediction_source126=capturedSource126;g_prediction_source126.joint.p.x+=.003;
ck(!PredictionPrepare120(&wrapper120,.011f,base+0xA7CDEE,observedSource126),"source proof rejects even 3mm disagreement inside the same native update");
g_prediction_source126=capturedSource126;g_prediction_source126.world.p.x+=.001;
ck(!PredictionPrepare120(&wrapper120,.011f,base+0xA7CDEE,observedSource126),"changed native world transform revokes source proof");
g_prediction_source126=capturedSource126;++g_prediction_source126.epoch;
ck(!PredictionPrepare120(&wrapper120,.011f,base+0xA7CDEE,observedSource126),"recenter epoch cannot borrow earlier source proof");
g_prediction_source126=capturedSource126;put(state.data()+0x2D8,std::uint8_t(0));
sourceFrame126();sourceFrame126(); // reacquire after deliberate epoch mismatch above
ck(sourceFrame126()==reinterpret_cast<void*>(0x5678),"native caller admits execution after request-candidate clears");
wrapper120.admitted=0;ck(sourceFrame126()==reinterpret_cast<void*>(0x1234),"a call without the native wrapper admission remains native");wrapper120.admitted=1;
put(state.data()+0x2D8,std::uint8_t(1));
DetourModKit::hook::MidContext endSource126{};PredictionSourceEndMid126(endSource126);
ck(!g_prediction_source126.action&&!PredictionPrepare120(&wrapper120,.011f,base+0xA7CDEE,observedSource126),
 "update exit clears source evidence and mismatched older pose remains rejected");
poseVft126[10]=base+kPoseGetterBuffer1Rva+1;PredictionSourceBegin126(producer.data(),sourcePose126);
ck(!g_prediction_source126.action,"wrong exact retail pose getter fails closed before source capture");poseVft126[10]=base+kPoseGetterBuffer1Rva;
hostRetailVerified112=false;PredictionSourceBegin126(producer.data(),sourcePose126);
ck(!g_prediction_source126.action,"source observer remains inert on fingerprint mismatch");hostRetailVerified112=true;
put(motionAction.data()+0xAC,nativeCurrent);
#include "physical_interval177_cases.inl"
g_prediction_contract120=0;hostRetailVerified112=false;g_weapon_control_mode=6;
