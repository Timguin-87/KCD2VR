// No native AI decision is emulated here. Verify publication ordering and that
// readiness cannot become phase8, damage, effort or an indefinitely renewed lease.
vs::Forget();mo::attempts.clear();hostInts[{hs,kActionPhaseProp39}]=1;hostNow=30000;
vs::g_telegraph_strike59c10=true;next.gesture=300;
const auto effortBefore=hostEffortCalls;
event(next,mo::Event::Prepare,1,2,0,hostNow);
assert(vs::g_stage==4&&vs::g_telegraph_offer.pending&&hostEffortCalls==effortBefore);
assert(mo::attempts.claim(next,hostNow,hostNow,55,claimed)==mo::Claim::Wait);
// A repeated same-time tick cannot collapse both notifications into one edge.
vs::Tick(hs,hostNow);assert(vs::g_stage==4);
hostNow+=10;vs::Tick(hs,hostNow);
assert(vs::g_stage==4&&!vs::g_telegraph_offer.open&&!vs::g_telegraph_offer.pending&&hostEffortCalls==effortBefore&&vs::g_deadline==30600);
assert(mo::attempts.claim(next,hostNow,hostNow,55,claimed)==mo::Claim::Wait);
// The early cut-to-stab refinement remains the SAME prepared gesture. No
// second attack, longer deadline, extra stamina charge or assumed contact.
event(next,mo::Event::Feint,3,5,1,30020);
assert(vs::g_stage==4&&vs::g_technique==1&&vs::g_zone==5&&vs::g_deadline==30600&&hostEffortCalls==effortBefore);
event(next,mo::Event::Commit,2,5,1,30030);
assert(vs::g_stage==8&&!vs::g_telegraph_offer.open&&vs::g_deadline==30600&&hostEffortCalls==effortBefore+1);
assert(mo::attempts.claim(next,30031,30031,55,claimed)==mo::Claim::Ready);
assert(mo::attempts.claim(next,30031,30031,55,claimed)==mo::Claim::Duplicate);
assert(vs::Publish(hs,0,-1,0));
// Option off has the same authority: phase8 waits for deliberate commit.
vs::g_telegraph_strike59c10=false;++next.gesture;event(next,mo::Event::Prepare,1,2,0,31000);
hostNow=31010;vs::Tick(hs,hostNow);assert(vs::g_stage==4&&!vs::g_telegraph_offer.pending);
event(next,mo::Event::Commit,2,2,0,31020);assert(vs::g_stage==8);assert(vs::Publish(hs,0,-1,0));
vs::g_telegraph_strike59c10=true;
// Cancellation before the deferred edge never advertises a cancelled attack.
++next.gesture;event(next,mo::Event::Prepare,1,2,0,32000);
event(next,mo::Event::Cancel,4,-1,0,32010);assert(vs::g_stage==0&&!vs::g_telegraph_offer.pending);
// Stale pending event, changed live identity and native phase takeover all win.
for(int fail=0;fail<3;++fail){
 ++next.gesture;hostInts[{hs,kActionPhaseProp39}]=1;
 event(next,mo::Event::Prepare,1,2,0,33000+fail*1000);
 hostNow+=fail==0?151:10;
 if(fail==1)hostOwner=false;
 if(fail==2)hostInts[{hs,kActionPhaseProp39}]=32;
 vs::Tick(hs,hostNow);assert(vs::g_stage==0);
 if(fail==2)assert(I(hs,kActionPhaseProp39)==32);
 hostOwner=true;
}
// A native update resetting phase4 to2 is NOT assumed safe to overwrite. F3
// gets one precise retirement, including explicit unreadable fields; off reads none.
for(int capture=0;capture<2;++capture){
 vs::Forget();hostInts[{hs,kActionPhaseProp39}]=1;++next.gesture;
 event(next,mo::Event::Prepare,1,2,0,36000+capture*100);
 const auto readCount=hostOfferReads,published=hostPhasePublications;
 hostCombatCapture=capture!=0;hostOfferReadMask=capture?1:7;diagnostic_trace::offerRetired.clear();
 hostInts[{hs,kActionPhaseProp39}]=2;hostNow+=10;vs::Tick(hs,hostNow);
 assert(vs::g_stage==0&&I(hs,kActionPhaseProp39)==2&&hostPhasePublications==published);
 assert(hostOfferReads==readCount+unsigned(capture));
 if(capture){assert(diagnostic_trace::offerRetired.find("livePhase=2 writtenPhase=4")!=std::string::npos);
  assert(diagnostic_trace::offerRetired.find("live150=UNKNOWN")!=std::string::npos);
  assert(diagnostic_trace::offerRetired.find("live450=UNKNOWN")!=std::string::npos);}
 else assert(diagnostic_trace::offerRetired.empty());
 vs::Tick(hs,hostNow+10);assert(hostOfferReads==readCount+unsigned(capture));
}
hostCombatCapture=false;hostOfferReadMask=7;
// A borrowed delayed A contact must not clear B's pending early notification.
vs::Forget();hostInts[{hs,kActionPhaseProp39}]=1;++next.gesture;
event(next,mo::Event::Prepare,1,2,0,37000);
borrowed.key=a;borrowed.commit=37000;borrowed.origin=5;borrowed.technique=1;hostNow=37001;
{vs::ContactScope5912 contact;assert(contact.begin(hs,borrowed));assert(!vs::g_telegraph_offer.pending);}
assert(vs::g_telegraph_offer.pending&&mo::same(vs::g_telegraph_offer.key,next)&&vs::g_deadline==37600);
hostNow=37010;vs::Tick(hs,hostNow);assert(vs::g_stage==4&&!vs::g_telegraph_offer.open&&!vs::g_telegraph_offer.pending);
assert(vs::Publish(hs,0,-1,0));
std::puts("H21 phase4 retained until real Commit/contact; unchanged effort, refinement, cancellation and borrowed offer verified.");
