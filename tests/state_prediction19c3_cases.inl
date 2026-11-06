// Actual extracted driver and actual attempt ledger. Native leaves only are
// substituted; no fixture emulates an NPC decision or a successful block.
{
 vs::g_fault=false;vs::g_setter_exception=false;vs::Forget();vs::Reset();mo::attempts.clear();
 vs::g_prediction_history19c3={};hostNativeAction=nullptr;hostGuardOwned=false;
 hostFailProp=hostExceptionProp=hostMismatchProp=~0u;hostNow=90040;
 hostInts[{hs,kActionPhaseProp39}]=1;hostFloats[{hs,0x450}]=-1.f;
 vs::Tick(hs,hostNow);vs::g_telegraph_strike59c10=false;
 vs::g_eta_prepare59c10=.25f;vs::g_npc_block_speed18=1.f;
 mo::Key p=a;p.gesture=900;mo::Key q=p;q.gesture=901;
 assert(mo::attempts.event(p,mo::Event::Prepare,90000,90000,2,0));
 assert(mo::attempts.beginForKey(p,90040)==90000);
 for(unsigned field=0;field<7;++field){auto wrong=p;
  switch(field){case 0:++wrong.gesture;break;case 1:++wrong.weapon;break;case 2:++wrong.generation;break;
   case 3:++wrong.body;break;case 4:++wrong.request;break;case 5:++wrong.recovery;break;case 6:++wrong.player;break;}
  assert(mo::attempts.beginForKey(wrong,90040)==0);
 }
 assert(mo::attempts.beginForKey(p,89999)==0);
 assert(vs::Event(p,90000,1,2,0));vs::Tick(hs,hostNow);
 assert(vs::g_prediction19c3.onset==90000&&vs::g_prediction19c3.deadline==90250);
 assert(F(hs,0x450)>.209f&&F(hs,0x450)<.211f); // queue delay is not returned
 assert(vs::g_deadline==90600&&I(hs,kActionPhaseProp39)==4);
 event(p,mo::Event::Feint,3,5,1,90060);
 assert(vs::g_prediction19c3.deadline==90250&&F(hs,0x450)>.189f&&F(hs,0x450)<.191f);
 event(p,mo::Event::Commit,2,5,1,90080);
 assert(vs::g_deadline==90600&&vs::g_prediction19c3.deadline==90250&&F(hs,0x450)>.169f&&F(hs,0x450)<.171f);
 vs::g_eta_prepare59c10=.60f;vs::g_eta_strike59c10=.60f;vs::g_npc_block_speed18=3.f;
 hostNow=90090;assert(vs::Publish(hs,8,2,0));
 assert(vs::g_prediction19c3.deadline==90250&&F(hs,0x450)>.159f&&F(hs,0x450)<.161f);
 hostNow=90249;vs::Tick(hs,hostNow);assert(F(hs,0x450)>.0009f&&F(hs,0x450)<.0011f);
 hostNow=90250;vs::Tick(hs,hostNow);assert(F(hs,0x450)==0.f&&vs::g_stage==8);
 hostNow=90255;vs::Tick(hs,hostNow);assert(F(hs,0x450)==0.f); // no 50 ms floor
 // Newer B gets its own onset; a delayed A contact never changes either one.
 vs::g_eta_prepare59c10=.25f;event(q,mo::Event::Prepare,1,2,0,90260);
 assert(vs::g_prediction19c3.deadline==90510&&vs::g_deadline==90860);
 hostNow=90300;mo::Attempt originalA;assert(mo::attempts.claim(p,90300,90300,55,originalA)==mo::Claim::Ready);
 vs::g_eta_prepare59c10=.60f; // changing settings during A does not re-seed B
 {vs::ContactScope5912 scope;assert(scope.begin(hs,originalA));
  assert(mo::same(vs::g_prediction19c3.key,p)&&vs::g_prediction19c3.deadline==90250&&F(hs,0x450)==0.f);
  hostNow=90320;assert(scope.current());}
 assert(mo::same(vs::g_prediction19c3.key,q)&&vs::g_prediction19c3.deadline==90510&&vs::g_deadline==90860);
 assert(F(hs,0x450)>.189f&&F(hs,0x450)<.191f);
 assert(mo::attempts.claim(p,90300,90320,55,originalA)==mo::Claim::Duplicate);
 // Reclaim of our exact guard phase changes no clock anchor or deadline.
 hostGuardOwned=true;hostInts[{hs,kActionPhaseProp39}]=2;hostNow=90330;vs::Tick(hs,hostNow);
 assert(vs::g_stage==4&&vs::g_prediction19c3.deadline==90510&&F(hs,0x450)>.179f&&F(hs,0x450)<.181f);
 hostGuardOwned=false;
 // Context publication lost to native idle can be refreshed for this checked
 // contact, but the original prediction stays expired (begin != commit).
 assert(mo::attempts.contact(q,90340,90340,55,0,originalA)==mo::Claim::Ready);
 assert(originalA.begin==90260&&originalA.commit==90340);
 hostInts[{hs,kActionPhaseProp39}]=2;hostNow=90520;
 {vs::ContactScope5912 scope;assert(scope.begin(hs,originalA));
  assert(vs::g_prediction19c3.deadline==90510&&F(hs,0x450)==0.f);}
 assert(vs::Publish(hs,0,-1,0)&&F(hs,0x450)==-1.f);
 // A first CONTACT publication (Prepare never reached this game-thread
 // driver) uses ledger begin, not the contact-generated commit timestamp.
 mo::Key early=p;early.gesture=902;hostNow=91000;
 assert(mo::attempts.event(early,mo::Event::Prepare,hostNow,hostNow,2,0));
 hostNow=91120;assert(mo::attempts.contact(early,91120,91120,55,1,originalA)==mo::Claim::Ready);
 vs::g_eta_prepare59c10=.25f;
 {vs::ContactScope5912 scope;assert(scope.begin(hs,originalA));
  assert(vs::g_prediction19c3.onset==91000&&vs::g_prediction19c3.deadline==91250);
  assert(F(hs,0x450)>.129f&&F(hs,0x450)<.131f);}
 assert(vs::Publish(hs,0,-1,0));
 // Original collision and attempt limits are unchanged by the prediction.
 mo::Key expired=p;expired.gesture=903;
 assert(mo::attempts.event(expired,mo::Event::Prepare,92000,92000,2,0));
 assert(mo::attempts.event(expired,mo::Event::Commit,92010,92010,2,0));
 assert(mo::attempts.claim(expired,92010,92171,55,originalA)==mo::Claim::Expired);
 assert(mo::attempts.beginForKey(expired,92611)==0);
 // A new admitted native action or a native timing rewrite owns its clock.
 mo::Key native=p;native.gesture=904;event(native,mo::Event::Prepare,1,2,0,93000);
 hostFloats[{hs,0x450}]=.321f;hostNow=93010;vs::Tick(hs,hostNow);
 assert(vs::g_stage==0&&F(hs,0x450)==.321f);
 assert(mo::attempts.beginForKey(native,hostNow)==0); // timing takeover cannot refresh the same attempt
 hostInts[{hs,kActionPhaseProp39}]=1;++native.gesture;event(native,mo::Event::Prepare,1,2,0,94000);
 hostNativeAction=actor;hostGuardOwned=true;hostInts[{hs,kActionPhaseProp39}]=2;
 const auto beforeReclaim=hostPhasePublications;hostNow=94010;vs::Tick(hs,hostNow);
 assert(vs::g_stage==0&&F(hs,0x450)==.321f&&I(hs,kActionPhaseProp39)==2&&hostPhasePublications==beforeReclaim);
 hostGuardOwned=false; // admitted action prevents even an earlier own-guard reclaim
 hostInts[{hs,kActionPhaseProp39}]=1;vs::g_key5912=native;
 const auto beforeNative=hostPhasePublications;
 assert(!vs::Publish(hs,4,2,0,94600)&&hostPhasePublications==beforeNative&&F(hs,0x450)==.321f);
 hostNativeAction=nullptr;
 // The same queued event cannot start this service in native comparison.
 vs::Forget();vs::Reset();mo::selected=false;hostInts[{hs,kActionPhaseProp39}]=1;
 assert(vs::Event(native,hostNow,1,2,0));vs::Tick(hs,hostNow);
 assert(vs::g_stage==0&&F(hs,0x450)==.321f&&hostPhasePublications==beforeNative);
 mo::selected=true;
 // Full-key history and installed .14 s setting, independent of live context.
 auto keySeed=p;keySeed.gesture=1000;hostNow=95020;vs::g_key5912=keySeed;vs::g_eta_prepare59c10=.14f;
 assert(vs::SelectPrediction19c3(hostNow,95000)&&vs::g_prediction19c3.deadline==95140);
 vs::g_eta_prepare59c10=.60f;
 for(unsigned field=0;field<7;++field){auto changed=keySeed;
  switch(field){case 0:++changed.gesture;break;case 1:++changed.weapon;break;case 2:++changed.generation;break;
   case 3:++changed.body;break;case 4:++changed.request;break;case 5:++changed.recovery;break;case 6:++changed.player;break;}
  vs::g_key5912=changed;assert(vs::SelectPrediction19c3(hostNow,95005)&&vs::g_prediction19c3.deadline==95605);
 }
 vs::g_key5912=keySeed;assert(vs::SelectPrediction19c3(hostNow,95010)&&vs::g_prediction19c3.deadline==95140);
 for(unsigned bound=0;bound<3;++bound){auto bounded=keySeed;bounded.gesture=1010+bound;vs::g_key5912=bounded;
  vs::g_eta_prepare59c10=bound==0?.001f:bound==1?100.f:std::numeric_limits<float>::quiet_NaN();
  assert(vs::SelectPrediction19c3(hostNow,95000));
  assert(vs::g_prediction19c3.deadline==95000+(bound==0?100:bound==1?600:250));}
 auto missing=keySeed;missing.gesture=1020;vs::g_key5912=missing;
 assert(vs::SelectPrediction19c3(hostNow)&&vs::g_prediction19c3.deadline==0&&vs::PredictionRemaining19c3(vs::g_prediction19c3,hostNow)==0.f);
 ++missing.gesture;vs::g_key5912=missing;
 assert(!vs::SelectPrediction19c3(~std::uint64_t(0)-10,~std::uint64_t(0)-10)); // overflow does not create a deadline
 // Actual timing setter readback: readable native override retires only this
 // attempt; an unreadable result is an ambiguous fault, never acceptance.
 vs::Forget();hostInts[{hs,kActionPhaseProp39}]=1;hostFloats[{hs,0x450}]=-1.f;
 vs::Tick(hs,hostNow);mo::attempts.clear();auto mismatch=keySeed;mismatch.gesture=1100;hostNow=96000;vs::g_eta_prepare59c10=.25f;
 hostMismatchProp=0x450;hostMismatchFloat=.4f;const auto beforeMismatch=hostPhasePublications;
 event(mismatch,mo::Event::Prepare,1,2,0,hostNow);
 assert(!vs::g_fault&&vs::g_stage==0&&hostPhasePublications==beforeMismatch&&F(hs,0x450)==.4f);
 assert(mo::attempts.beginForKey(mismatch,hostNow)==0);
 hostMismatchProp=~0u;hostFailProp=0x450;++mismatch.gesture;event(mismatch,mo::Event::Prepare,1,2,0,97000);
 assert(vs::g_fault&&hostPhasePublications==beforeMismatch);hostFailProp=~0u;vs::g_fault=false;vs::Forget();
 // A newly allocated native state with the same row value is not the old
 // lease. Refusal before ordinary-action ownership must never write into it.
 hostInts[{hs,kActionPhaseProp39}]=1;hostInts[{hs,kDefenseZone}]=7;++mismatch.gesture;
 event(mismatch,mo::Event::Prepare,1,2,0,98000);assert(vs::g_defense_owned&&vs::g_written_defense==1);
 int replacement=0;const void* newState=&replacement;
 hostInts[{newState,kActionPhaseProp39}]=4;hostInts[{newState,kDefenseZone}]=1;hostFloats[{newState,0x450}]=.456f;
 hostNativeAction=actor;hostNow=98010;
 assert(!vs::Publish(newState,4,2,0,98600));
 assert(I(newState,kDefenseZone)==1&&I(newState,kActionPhaseProp39)==4&&F(newState,0x450)==.456f);
 hostNativeAction=nullptr;
 vs::g_eta_prepare59c10=.25f;vs::g_eta_strike59c10=.12f;vs::g_npc_block_speed18=1.f;
 std::puts("C19C3 actual-driver prediction: onset/queue age, full key, feint/commit/config, zero expiry, A/B, reclaim, first contact, native takeover and comparison isolation passed.");
}
