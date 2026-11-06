// Actual queue/drain-booking refusal sites with diagnostic capture enabled.
{
 namespace rr=kcd2vr::recovery65h;auto& r=rr::stats;
 diagnostics::capture168::gate.select(diagnostics::capture168::Domain::Combat,true,4000);
 Clear();r.reset();now59c6=4000;contact176::recoveryEpoch55c1=10;
 auto p=a;p.tick=4000;p.gesture59c3=800;
 rr::edge(true);ck(r.edges==1&&r.retired==0,"P6 recovery edge is observation only, no speculative terminal drop");
 ck(Submit(p),"new-epoch contact admitted after edge");
 unsigned nativeStarted=0;
 DrainBooking59c6([&](Pending& x,auto&){ck(x.recovery55c1==10,"new epoch source is current");++nativeStarted;});
 ck(nativeStarted==1&&r.retired==0,"reproduced C61a new-epoch case now completes without false recovery drop");
 // Edge during a native continuation cannot retract or terminalize that call.
 Clear();r.reset();p.tick=now59c6=4100;p.gesture59c3++;
 ck(Submit(p),"in-flight fixture admitted");
 DrainBooking59c6([&](Pending&,auto&){++nativeStarted;contact176::recoveryEpoch55c1=11;rr::edge(true);
  auto fresh=p;fresh.gesture59c3++;fresh.tick=4100;ck(Submit(fresh),"new-epoch producer allowed while old native call runs");});
 ck(r.edges==1&&r.retired==0&&g_has&&g_pending.recovery55c1==11,"in-flight and queued new-epoch contacts are not classified as cancelled");
 DrainBooking59c6([&](Pending&,auto&){++nativeStarted;});ck(!g_has&&r.retired==0,"later real application has no first-terminal diagnostic suppression");
 // Actual entry guard retires one old contact; destructor must not count again.
 Clear();r.reset();p.tick=now59c6=4200;p.gesture59c3++;ck(Submit(p),"entry guard fixture admitted");contact176::recoveryEpoch55c1=12;
 DrainBooking59c6([&](Pending& p,auto&){
#include "generated_hybrid_entry65h.inc"
  ++nativeStarted;
 });
 ck(r.retired==1&&r.recoveryOnly==1&&r.sites[unsigned(rr::Site::DrainEntry)]==1&&!g_has,"actual entry refusal counted once, no destructor double count");
 // Waiting contact and backlog retirement use their actual removal sites.
 Clear();r.reset();p.tick=now59c6=4300;p.gesture59c3++;ck(Submit(p),"waiting fixture admitted");
 DrainBooking59c6([&](Pending& x,auto& lease){lease.wait(x);contact176::recoveryEpoch55c1=13;});
 ck(r.retired==1&&r.sites[unsigned(rr::Site::Waiting)]==1&&!g_has,"waiting lease retirement counts only actual discarded old-epoch contact");
 Clear();r.reset();p.tick=now59c6=4400;p.gesture59c3++;ck(Submit(p),"backlog fixture head");auto next=p;next.gesture59c3++;ck(Submit(next),"backlog fixture tail");
 DrainBooking59c6([&](Pending&,auto&){contact176::recoveryEpoch55c1=14;});
 ck(r.retired==1&&r.sites[unsigned(rr::Site::DrainPrune)]==1&&!g_has,"retired backlog counted, completed in-flight head not counted");
 Clear();r.reset();p.tick=now59c6=4500;p.gesture59c3+=2;ck(Submit(p),"submit-prune old head");contact176::recoveryEpoch55c1=15;
 next=p;next.gesture59c3++;ck(Submit(next),"submit-prune new epoch contact");
 ck(r.retired==1&&r.recoveryOnly==1&&r.sites[0]==1&&g_has&&g_pending.recovery55c1==15,"submit pruning removes only old epoch and retains new current contact");
 Clear();r.reset();p.tick=now59c6=4600;p.gesture59c3+=3;ck(Submit(p),"expiry-only fixture");now59c6=4761;next=p;next.tick=4761;next.gesture59c3++;ck(Submit(next),"expiry replacement");
 ck(r.retired==1&&r.expiredOnly==1&&r.recoveryOnly==0&&r.both==0,"ordinary160ms expiry is distinguished from recovery");
 Clear();r.reset();p.tick=now59c6=4800;p.gesture59c3+=4;ck(Submit(p),"combined-expiry fixture");now59c6=4961;contact176::recoveryEpoch55c1=16;next=p;next.tick=4961;next.gesture59c3++;ck(Submit(next),"combined replacement");
 ck(r.retired==1&&r.both==1&&r.recoveryOnly==0,"both causes remain explicit rather than blaming recovery alone");
 diagnostics::capture168::gate.select(diagnostics::capture168::Domain::Combat,false,5000);
 Clear();r.reset();p.tick=now59c6=5000;p.gesture59c3+=5;ck(Submit(p),"quiet fixture");contact176::recoveryEpoch55c1=17;next=p;next.gesture59c3++;ck(Submit(next),"quiet replacement");
 rr::edge(false);ck(r.edges==0&&r.retired==0,"F3 off adds no recovery diagnostic accounting");
 Clear();g_hybrid_wait65h=true;
}
