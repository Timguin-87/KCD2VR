// Actual coherent_pose caller -> two-reference-frame melee kernel. A wrist
// socket is deliberately non-identity; identity mocks hid the live mismatch.
for(double yaw32c1:{0.,.7,-1.4}){
 Init();auto t32c1=Tracking();Check(Run(t32c1,3),"R32C1 initialize real pose caller");
 const Q4 frame32c1{0,0,float(std::sin(yaw32c1*.5)),float(std::cos(yaw32c1*.5))};
 g_cal_frame_qz=frame32c1.z;g_cal_frame_qw=frame32c1.w;
 for(int side=0;side<2;++side){g_pose6689.anatomyHand[side]=true;
     g_pose6689.boneToAnatomy[side]=QNormalize({.21f,side?-.31f:.31f,.13f,.91f});}
 g_weapon_control_mode=7;mockPhysical=true;retail_build112::mockReady175=true;
 mockProfiles[0].key=777;mockProfiles[0].generation=1;mockProfiles[0].primary=mockProfiles[0].geometry=true;
 mockProfiles[0].classId=4;mockProfiles[0].tip={0,.6,0};mockProfiles[0].axis={0,1,0};
 ++t32c1.sequence;t32c1.tick+=10;
 Check(Run(t32c1,3,true),"R32C1 actual pose publishes weapon input");
 const auto captured32c1=mockPhysicalInput175;
 for(int side=0;side<2;++side){
     const auto localAxis=pc::unit(pc::Vec3{.2,.9,.3}).value();
     const auto modelAxis=pc::rotate(captured32c1.hands[side].q,localAxis);
     const auto trackedAxis=pc::rotate(captured32c1.intentHands[side].q,localAxis);
     Check(pc::length(modelAxis-pc::rotate(PQ(frame32c1),trackedAxis))<1e-5,
         "same native wrist-local socket has the same blade axis in both motion frames");
 }
 // Feed both orientations from the actual caller, including the calibrated
 // frame, through the production kernel. No native hit or damage is emulated.
 ph::Input input32c1{};ph::State state32c1{};std::array<ph::Profile,2> profiles32c1{};
 input32c1.epoch=input32c1.bodyEpoch=1;input32c1.allowed=input32c1.tracked[0]=input32c1.intentTracked[0]=true;
 input32c1.coherentMelee119=input32c1.controllerAnchor119=input32c1.requirePrimaryGrip114=input32c1.heldMotionAttacks=true;
 input32c1.dynamics=false;input32c1.grip[0]=1;input32c1.reach[0]=1.3;input32c1.shoulders[0]={0,0,1.3};
 input32c1.hands[0]={captured32c1.hands[0].q,{-.25,.2,1.2}};
 input32c1.intentHands[0]={captured32c1.intentHands[0].q,pc::rotate(PQ(QConj(frame32c1)),pc::Vec3{-.25,.2,-.4})};
 auto& profile32c1=profiles32c1[0];profile32c1.key=profile32c1.generation=1;
 profile32c1.classId=4;profile32c1.primary=profile32c1.geometry=true;profile32c1.axis={0,1,0};profile32c1.tip={0,.6,0};
 profile32c1.grip[0].q=captured32c1.hands[0].q;
 int thrusts32c1=0,cuts32c1=0,prepares32c1=0;
 for(int step32c1=0;step32c1<26;++step32c1){
     ++input32c1.sample;input32c1.tick=1000+10*input32c1.sample;input32c1.timeNs=input32c1.tick*1000000;
     if(step32c1>=14){input32c1.hands[0].p=input32c1.hands[0].p+pc::Vec3{0,.02,0};
         input32c1.intentHands[0].p=input32c1.intentHands[0].p+pc::rotate(PQ(QConj(frame32c1)),pc::Vec3{0,.02,0});}
     const auto output32c1=ph::step(state32c1,input32c1,profiles32c1,[](const ph::Profile&,ph::Pose,ph::Pose,bool){return ph::Contact103{};});
     for(unsigned j=0;j<output32c1.commandCount;++j){const auto kind=output32c1.commands[j].kind;
         thrusts32c1+=kind==ph::CommandKind::Thrust;cuts32c1+=kind==ph::CommandKind::Slash;prepares32c1+=kind==ph::CommandKind::Prepare;
         if(kind==ph::CommandKind::Thrust)Check(output32c1.commands[j].zone117==5,"deliberate axial stab retains native thrust zone");}
 }
 Check(prepares32c1==1&&thrusts32c1==1&&cuts32c1==0,"actual caller wrist orientation admits exactly one deliberate thrust at every calibrated yaw");
}
