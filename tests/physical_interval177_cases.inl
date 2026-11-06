// Actual publisher helper + actual native predictor, native leaf calls mocked.
const auto publicModeH31=kcd2vr::user175::configuration(kcd2vr::user175::Mode::CombatSynchronized);
const int priorCensusH31=g_census_mode.load(),priorWeaponH31=g_weapon_control_mode.load();
const bool priorSyncH31=g_synchronized59c10r9.load(),priorIntegratedH31=g_combat_integrated178r22c2.load();
g_census_mode=publicModeH31.pose;g_weapon_control_mode=publicModeH31.weapon;
g_synchronized59c10r9=g_combat_integrated178r22c2=true;g_intent59c16=g_intent_pi59c16=false;
g_combat_experiment175=g_physical_combat177=true;frame.input.physicalCombat177=true;
IntegratedFistSelection178r22c2();ck(!FistSelected101(),"public F4 HYBRID equipped weapon cannot be stolen by fist mode selector");
g_prediction_lease120={};put(motionAction.data()+0xAC,nativeCurrent);
put(motionAction.data()+0x40C,0);
motionFrame(true);PhysFrame6698 previous177=frame;
motionFrame(true);PhysInterval177(previous177,frame,previous177.input.sample);
ck(frame.previousSample177==previous177.input.sample,"completed same-owner consecutive frames form interval before first native callback");
const auto complete177=frame;
auto publish177=[&](){
    g_phys_frame6698.publish(frame);g_phys_ready_sample6698=frame.input.sample;
    TrackingSnapshot t{};t.allowed=t.head_valid=t.left_valid=t.right_valid=true;
    t.tick=t.head_good_tick=t.left_good_tick=t.right_good_tick=GetTickCount64();
    g_tracking_frames6695.publish(t);
};
publish177();PredictionScope120 first177{};
ck(PredictionPrepare120(&wrapper120,.011f,base+0xA7CDEE,first177)&&first177.correlated122==2,
    "F4 rung2 uses two completed poses immediately without three native warmups");
ck(std::abs(first177.input.target.t[0]-first177.input.previous.t[0]-.02f)<1.e-5,
    "first admitted interval contains only latest tracked displacement");
PredictionScope120 duplicate177{};
ck(PredictionPrepare120(&wrapper120,.011f,base+0xA7CDEE,duplicate177)&&
    !std::memcmp(&duplicate177.input.previous,&duplicate177.input.target,sizeof(QuatT28)),
    "repeated native callback cannot replay interval");
{
 auto subTickBefore=previous177;frame=complete177;frame.input.timeNs=frame.input.tick*1000000ull;
 subTickBefore.input.tick=frame.input.tick;subTickBefore.correlation126[0].stamp.tick=frame.input.tick;
 subTickBefore.input.timeNs=frame.input.timeNs-4000000ull;
 PhysInterval177(subTickBefore,frame,subTickBefore.input.sample);g_prediction_lease120={};publish177();PredictionScope120 subTick{};
 ck(frame.previousNs59c6==subTickBefore.input.timeNs&&PredictionPrepare120(&wrapper120,.004f,base+0xA7CDEE,subTick),
    "same coarse tick but newer nanosecond pose reaches actual predictor without native fallback");
 frame=complete177;publish177();
}
for(int kind=0;kind<7;++kind){
    auto bad=previous177;frame=complete177;
    switch(kind){case 0:bad.profiles[0].generation++;break;case 1:bad.input.allowed=false;break;
    case 2:bad.output.contactKnown[0]=false;break;case 3:bad.correlation126[0].socket.relative.t[0]+=.01f;break;
    case 4:bad.input.tick=frame.input.tick-51;break;case 5:bad.player=nullptr;break;case 6:bad.input.physicalCombat177=false;break;}
    PhysInterval177(bad,frame,bad.input.sample);
    ck(!frame.previousSample177,"replacement/invalid/unknown/topology/stale/non-rung2 predecessor cannot form interval");
}
frame=complete177;frame.previousSample177=0;publish177();PredictionScope120 missing177{};
ck(!PredictionPrepare120(&wrapper120,.011f,base+0xA7CDEE,missing177),"no previous complete pose means no interval replacement");
frame=complete177;publish177();g_phys_ready_sample6698=frame.input.sample-1;
ck(!PredictionPrepare120(&wrapper120,.011f,base+0xA7CDEE,missing177),"new unfinished publication is rejected instead of borrowing older pose");
publish177();wrapper120.admitted=0;
ck(!PredictionPrepare120(&wrapper120,.011f,base+0xA7CDEE,missing177),"completed pair never opens a native attack window");wrapper120.admitted=1;
frame=complete177;publish177();hostRetailVerified112=false;
ck(!PredictionPrepare120(&wrapper120,.011f,base+0xA7CDEE,missing177),"rung2 cannot bypass exact retail fingerprint");hostRetailVerified112=true;
g_combat_experiment175=g_physical_combat177=false;frame.input.physicalCombat177=false;
g_census_mode=priorCensusH31;g_weapon_control_mode=priorWeaponH31;g_synchronized59c10r9=priorSyncH31;g_combat_integrated178r22c2=priorIntegratedH31;
frame.previousSample177=frame.previousTick177=0;g_prediction_lease120={};put(motionAction.data()+0x40C,7);
