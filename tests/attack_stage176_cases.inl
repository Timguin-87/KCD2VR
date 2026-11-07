// Invoked from the complete production predictor fixture, with a qualified
// native action, committed socket/pose and the replacement native loop leaf.
{
 const bool savedF4176=g_combat_experiment175.load();
 const auto savedPolicy176=kcd2vr::contact176::policy.load();
 int phase176=0;std::memcpy(&phase176,state.data()+0x38,sizeof(phase176));
 g_combat_experiment175=true;
 diagnostics::capture168::gate.select(diagnostics::capture168::Domain::Combat,false,GetTickCount64());
 g_contact_until114=GetTickCount64()+diagnostics::capture168::Duration;
 predictionTraceLines176.clear();
 const auto quietCalls176=predictionCalls120;
 const auto quietSegments176=g_prediction_segments120.load(),quietSweeps176=g_prediction_sweeps120.load();
 ck(runPrediction120()==reinterpret_cast<void*>(0x5678)&&predictionCalls120==quietCalls176+1,
    "F3 OFF actual predictor still redirects and calls the native loop exactly once");
 ck(predictionTraceLines176.empty()&&g_prediction_segments120.load()==quietSegments176&&g_prediction_sweeps120.load()==quietSweeps176,
    "F3 OFF stale contact deadline cannot format stage/witness or increment diagnostic totals");
 const auto quietReject176=g_prediction_reject120[1].load();
 PredictionRun120(&wrapper120,.011f,base+0xA7CDEF);
 ck(g_prediction_reject120[1].load()==quietReject176&&predictionTraceLines176.empty(),
    "F3 OFF rejection still fails closed without diagnostic counters or formatting");
 runPrediction120();runPrediction120();runPrediction120(); // reacquire the real three-sample trajectory contract
 diagnostics::capture168::gate.select(diagnostics::capture168::Domain::Combat,true,GetTickCount64());
 g_contact_until114=GetTickCount64()+diagnostics::capture168::Duration;
 g_prediction_stage176={};g_prediction_stage_next176=0;
 ph::Command release176{};release176.kind=ph::CommandKind::Slash;release176.side=0;
 release176.tick=GetTickCount64();release176.sample=frame.input.sample;release176.bodyEpoch=frame.input.bodyEpoch;
 release176.epoch=frame.request;release176.key=frame.profiles[0].key;release176.generation=frame.profiles[0].generation;
 PublishPhysicalStrike176(release176,frame);
 const auto captureCalls176=predictionCalls120;
 ck(runPrediction120()==reinterpret_cast<void*>(0x5678)&&predictionCalls120==captureCalls176+1,
    "F3 ON observation does not add a native predictor call or alter the tracked result");
 auto stages176=[&](){unsigned n=0;for(const auto& line:predictionTraceLines176)n+=line.find("F4_ATTACK_STAGE")!=std::string::npos;return n;};
 ck(stages176()==1,"actual F4 predictor emits one bounded stage when capture begins");
 std::string stage176;for(const auto& line:predictionTraceLines176)if(line.find("F4_ATTACK_STAGE")!=std::string::npos)stage176=line;
 ck(stage176.find("releaseTick="+std::to_string(release176.tick))!=std::string::npos&&
    stage176.find("releaseSample="+std::to_string(release176.sample))!=std::string::npos&&
    stage176.find("releaseAgeMs=11 releaseIdentity=1 releasePolicy=1 releaseWithin120ms=1")!=std::string::npos,
    "stage retains exact original release tick/sample and labels current matching identity separately");
 ck(stage176.find("ready=1 redirected=1")!=std::string::npos&&stage176.find("vrDeltaM=0.02000")!=std::string::npos&&
    stage176.find("nativeActionWindow=RETAINED enqueueSuppressionRequested=0 independentAttackState=UNPROVEN damage=UNCLAIMED")!=std::string::npos,
    "stage reports committed tracked interval and retained native action without claiming independent damage");
 const auto duplicateStages176=stages176();
 PredictionRun120(&wrapper120,.011f,base+0xA7CDEE);
 ck(stages176()==duplicateStages176,"repeated same stage/sample cannot flood capture");
 // Exercise rapidly alternating native stage observations through the actual
 // predictor; the global atomic cap applies even when every key changes.
 const auto boundedBefore176=stages176(),boundedCalls176=predictionCalls120;
 for(unsigned i=0;i<20;++i){put(state.data()+0x38,i%2?4:8);runPrediction120();}
 ck(predictionCalls120==boundedCalls176+20&&stages176()-boundedBefore176<=9,
    "twenty real predictor calls retain all native calls while stage output is capped to 25ms slots");
 predictionTraceLines176.clear();
 g_prediction_original120=+[](void* wrapper,float dt)->void* {
    auto* result=predictionModel120(wrapper,dt);
    diagnostics::capture168::gate.select(diagnostics::capture168::Domain::Combat,false,GetTickCount64());return result;
 };
 const auto stoppedCalls176=predictionCalls120;
 ck(runPrediction120()==reinterpret_cast<void*>(0x5678)&&predictionCalls120==stoppedCalls176+1&&predictionTraceLines176.empty(),
    "recorder disabled during original call prevents post-return stage/witness without changing native result");
 g_prediction_original120=&predictionModel120;
 diagnostics::capture168::gate.select(diagnostics::capture168::Domain::Combat,true,GetTickCount64());
 g_contact_until114=GetTickCount64()+1;
 const auto expiryCalls176=predictionCalls120;
 ck(runPrediction120()==reinterpret_cast<void*>(0x5678)&&predictionCalls120==expiryCalls176+1&&predictionTraceLines176.empty(),
    "expired observation deadline leaves actual tracked predictor working and formatting quiet");
 g_combat_experiment175=false;g_contact_until114=GetTickCount64()+diagnostics::capture168::Duration;
 predictionTraceLines176.clear();runPrediction120();
 ck(stages176()==0,"F12 retains its predictor and cannot be mislabeled as the F4 attack-stage experiment");
 put(state.data()+0x38,phase176);g_combat_experiment175=savedF4176;
 kcd2vr::contact176::policy=savedPolicy176+1; // no test release may survive into later fixture cases
 predictionTraceLines176.clear();
}
