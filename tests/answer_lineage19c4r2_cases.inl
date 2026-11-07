// Included inside the complete production SubmitHitDetour fixture's main.
{
 namespace a=kcd2vr::answer59c4r2;namespace g=n::geom27;
 hostTickCount64+=2000;const auto now=std::uint64_t(hostTickCount64);
 gameplay175=u::configuration(u::Mode::CombatSynchronized);g::g_sync59c10r9=true;
 std::array<unsigned char,0xD00> targetAnswer{},targetStateAnswer{};std::array<unsigned char,0x80> targetEntityAnswer{};
 put(targetAnswer,0,n::g_base+n::kCombatActorVft);put(targetAnswer,0x1F8,targetEntityAnswer.data());put(targetAnswer,0x210,targetStateAnswer.data());
 put(targetStateAnswer,0xCE0,targetAnswer.data());put(targetEntityAnswer,0,n::g_base+n::kEntityVft);put(targetEntityAnswer,0xC,std::uint32_t(200));
 put(hit,0,std::uint32_t(100));put(hit,0x28,std::uint32_t(200));put(request,0x10,targetAnswer.data());
 details.fill(0);details[1]=1;nativeResult168=true;inputOwned175=false;
 testCaller168=reinterpret_cast<void*>(n::g_base+0xD536BC);
 kcd2vr::contact176::Ticket ta{};ta.serial=100;ta.policy=7;ta.tick=now-20;ta.sample=20;ta.epoch=3;ta.request=4;
 ta.key=77;ta.generation=5;ta.gesture59c3=1000;ta.recovery59c3=9;
 ta.player=reinterpret_cast<std::uintptr_t>(player.data());ta.entity=reinterpret_cast<std::uintptr_t>(entity.data());
 auto tb=ta;tb.serial=101;tb.tick=now-10;tb.sample=21;tb.gesture59c3=1001;
 answerKnown19c4r2=true;answerLineage19c4r2={ta,0x1000};
 const auto aa=reinterpret_cast<std::uintptr_t>(actor.data()),tt=reinterpret_cast<std::uintptr_t>(targetAnswer.data());
 auto submit=[&](){const auto before=originalCalls168;const bool r=n::SubmitHitDetour(rpg.data(),hit.data(),&strength,&post,request.data(),true,1,details.data());
  check175(originalCalls168==before+1&&r==nativeResult168,"answer snapshot preserves original exactly once and returned result");};
 // Actual Submit callback and actual Drain consumption helper: the native
 // candidate may predict a parry/HP delta, but has not applied this contact.
 g::Pending evaluationPending{};evaluationPending.tick=now;evaluationPending.release51=ta;evaluationPending.lease47c2={ta.policy,ta.serial};
 for(auto caller:{0x173BF58ull,0xC6248Full,0xC61543ull}){
  testCaller168=reinterpret_cast<void*>(n::g_base+caller);details[0]=1;details[3]=1;details[7]=1;
  const auto calls=originalCalls168,pulses=notifications168,snapshots=answerSnapshots19c4r2;
  check175(n::SubmitHitDetour(rpg.data(),hit.data(),&strength,&post,request.data(),false,1,details.data())&&originalCalls168==calls+1,
   "native A/B/C evaluation original forwarded exactly once, unchanged result");
  check175(answerSnapshots19c4r2==snapshots&&notifications168==pulses&&g::g_answers19c4r2.query(aa,tt,ta,now).match==a::Match::None,
   "computed damage/block/master evaluation neither snapshots an application nor publishes accepted outcome");
  const auto claimed=kcd2vr::contact176::claimedSerial.load();
  check175(!g::ConsumeNativeAnswer19c4r2(evaluationPending,actor.data(),targetAnswer.data(),now)&&kcd2vr::contact176::claimedSerial==claimed,
   "predicted native parry cannot prematurely consume the real contact");
 }
 details.fill(0);details[1]=1;testCaller168=reinterpret_cast<void*>(n::g_base+0xD536BC);
 replaceAnswerDuringNative19c4r2=true;submit();replaceAnswerDuringNative19c4r2=false;
 check175(g::g_answers19c4r2.query(aa,tt,ta,now).match==a::Match::Same,"pre-submit A survives native callback replacement of live binding");
 check175(g::g_answers19c4r2.query(aa,tt,tb,now).match==a::Match::Different,"old admitted A answer cannot consume newer pending B");
 g::Pending p{};p.serial=200;p.entityId=200;p.tick=now;p.energy=.7f;p.release51=tb;p.lease47c2={tb.policy,tb.serial};
 kcd2vr::contact176::claimedSerial=99;
 check175(!g::ConsumeNativeAnswer19c4r2(p,actor.data(),targetAnswer.data(),now)&&kcd2vr::contact176::claimedSerial==99,
  "known different preserves all remaining contact gates and does not claim newer release");
 answerLineage19c4r2={tb,0x2000};submit();answerLineage19c4r2={ta,0x1000};submit();
 check175(g::g_answers19c4r2.query(aa,tt,tb,now).match==a::Match::Same,"older A callback returning after B cannot shadow exact B answer");
 const auto pulses=notifications168,calls=originalCalls168;
 check175(g::ConsumeNativeAnswer19c4r2(p,actor.data(),targetAnswer.data(),now)&&kcd2vr::contact176::claimedSerial==tb.serial,
  "same-release native answer claims tracked delivery once");
 check175(g::ConsumeNativeAnswer19c4r2(p,actor.data(),targetAnswer.data(),now)&&notifications168==pulses+1&&originalCalls168==calls,
  "duplicate contact makes no second native call or clash effect");
 hostTickCount64+=1000;tb.tick=hostTickCount64-10;tb.serial=102;tb.sample=22;tb.gesture59c3=1002;
 answerKnown19c4r2=false;submit();answerKnown19c4r2=true;answerLineage19c4r2={ta,0x1000};submit();
 p.tick=hostTickCount64;p.release51=tb;p.lease47c2={tb.policy,tb.serial};const auto unknownPulses=notifications168;
 check175(g::g_answers19c4r2.query(aa,tt,tb,hostTickCount64).match==a::Match::Unknown,"older known answer cannot shadow unknown B-era answer");
 check175(g::ConsumeNativeAnswer19c4r2(p,actor.data(),targetAnswer.data(),hostTickCount64)&&notifications168==unknownPulses,
  "unknown keeps conservative no-redelivery without inventing block feedback");
 hostTickCount64+=1000;nativeResult168=false;tb.tick=hostTickCount64-10;answerLineage19c4r2={tb,0x2000};submit();nativeResult168=true;
 check175(g::g_answers19c4r2.query(aa,tt,tb,hostTickCount64).match==a::Match::None,"native refusal publishes no accepted answer");
 const auto snapshots=answerSnapshots19c4r2;g::g_sync59c10r9=false;submit();
 check175(answerSnapshots19c4r2==snapshots,"non-SYNC submit adds no admitted-binding read");
 g::g_sync59c10r9=true;put(rpg,8,static_cast<void*>(nullptr));submit();put(rpg,8,actor.data());
 check175(answerSnapshots19c4r2==snapshots,"non-Henry submit adds no admitted-binding read");
 // Pure production ledger: every release component participates; no temporal
 // window can turn a genuinely different key into an accepted same-release hit.
 for(int field=0;field<13;++field){a::Ledger ledger;auto changed=ta;
  switch(field){case 0:++changed.serial;break;case 1:++changed.policy;break;case 2:++changed.player;break;case 3:++changed.entity;break;
   case 4:++changed.tick;break;case 5:++changed.sample;break;case 6:++changed.epoch;break;case 7:++changed.request;break;
   case 8:++changed.key;break;case 9:++changed.generation;break;case 10:++changed.gesture59c3;break;case 11:++changed.recovery59c3;break;case 12:changed.serial=0;break;}
  ledger.note({aa,tt,now,1,{ta,0x1000},a::Effect::Application});
  const auto expected=(field==0||field==1||field==4||field==5||field==12)?a::Match::Unknown:a::Match::Different;
  check175(ledger.query(aa,tt,changed,now).match==expected,"distinct motion is different; same-motion transport alias stays conservative");
 }
 {a::Ledger ledger;ledger.note({aa,tt,now,1,{ta,0x1000},a::Effect::Application});
  check175(ledger.query(aa+1,tt,ta,now).match==a::Match::None&&ledger.query(aa,tt+1,ta,now).match==a::Match::None,"answers remain actor and target specific");
  check175(ledger.query(aa,tt,ta,now+701).match==a::Match::None,"original answer window expires without read renewal");
  auto later=ta;later.tick=now+1;check175(ledger.query(aa,tt,later,now+1).match==a::Match::None,"answer before release cannot resolve later release");
 }
 {a::Ledger ledger;for(unsigned i=0;i<17;++i)ledger.note({aa,tt+i,now+i,1,{ta,0x1000},a::Effect::Application});
  check175(ledger.query(aa,tt,ta,now+16).match==a::Match::Unknown,"bounded ledger overflow preserves no-redelivery instead of losing old answer");
  check175(ledger.query(aa,tt,ta,now+717).match==a::Match::None,"overflow conservatism expires and is never renewed by reading");
 }
 {a::Ledger ledger;
  for(unsigned i=0;i<100;++i){ledger.note({aa,tt,now+i,1,{ta,0x1000},a::Effect::Evaluation});ledger.note({aa,tt,now+i,1,{ta,0x1000}});}
  check175(ledger.query(aa,tt,ta,now+100).match==a::Match::None,"evaluation or unknown-effect flood cannot poison applied ledger or overflow into UNKNOWN");
  ledger.note({aa,tt,now,5,{ta,0x1000},a::Effect::Application});
  check175(ledger.query(aa,tt,ta,now).match==a::Match::Same,"normal applied hit, including zero armor damage, is still a completed consequence");
 }
 hostTickCount64+=2000;tb.tick=hostTickCount64-10;tb.serial=103;tb.gesture59c3=1003;answerLineage19c4r2={tb,0x3000};answerKnown19c4r2=true;
 details.fill(0);submit();p.tick=hostTickCount64;p.release51=tb;p.lease47c2={tb.policy,tb.serial};
 const auto normalPulses=notifications168,normalCalls=originalCalls168;
 check175(g::ConsumeNativeAnswer19c4r2(p,actor.data(),targetAnswer.data(),hostTickCount64)&&notifications168==normalPulses,
  "normal applied native hit consumes contact without inventing block haptics");
 check175(g::ConsumeNativeAnswer19c4r2(p,actor.data(),targetAnswer.data(),hostTickCount64)&&originalCalls168==normalCalls,
  "repeated contact cannot repeat the normal applied native hit");
 hostTickCount64+=1000;tb.tick=hostTickCount64-10;tb.serial=104;tb.gesture59c3=1004;answerLineage19c4r2={tb,0x4000};
 put(hit,0x28,std::uint32_t(999));submit();put(hit,0x28,std::uint32_t(200));
 check175(g::g_answers19c4r2.query(aa,tt,tb,hostTickCount64).match==a::Match::Unknown,
  "applied result target mismatch stays conservative, never an exact accepted defense");
 hostTickCount64+=1000;auto tc=tb;tc.tick=hostTickCount64-10;tc.serial=105;tc.gesture59c3=1005;
 auto td=tc;td.serial=106;td.gesture59c3=1006;answerLineage19c4r2={tc,0x5000};details[1]=1;
 const auto reentrantCalls=originalCalls168;
 nestedSubmit19c4r3=[&](){answerLineage19c4r2={td,0x6000};
  (void)n::SubmitHitDetour(rpg.data(),hit.data(),&strength,&post,request.data(),false,1,details.data());
  (void)n::SubmitHitDetour(rpg.data(),hit.data(),&strength,&post,request.data(),true,1,details.data());};
 (void)n::SubmitHitDetour(rpg.data(),hit.data(),&strength,&post,request.data(),true,1,details.data());
 check175(originalCalls168==reentrantCalls+3&&g::g_answers19c4r2.query(aa,tt,tc,hostTickCount64).match==a::Match::Same&&
  g::g_answers19c4r2.query(aa,tt,td,hostTickCount64).match==a::Match::Same,
  "nested evaluation and B application cannot overwrite A pre-call lineage or call any native original twice");
 check175(g::g_answers19c4r2.query(aa,tt+1,td,hostTickCount64).match==a::Match::None,
  "a later contact target swap cannot consume another target's application");
 check175(g::g_answers19c4r2.query(aa,tt,tc,hostTickCount64+701).match==a::Match::None,
  "actual callback application expires at its original timestamp without renewal");
 g::g_sync59c10r9=false;answerKnown19c4r2=false;inputOwned175=true;kcd2vr::contact176::claimedSerial=0;
}
