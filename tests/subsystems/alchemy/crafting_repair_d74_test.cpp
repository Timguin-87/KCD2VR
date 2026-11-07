// Actual production host policy; no WHGame calls or claimed runtime outcomes.
#define KCD2VR_INPUT67_NO_MAIN
#include "input67_test.cpp"
static void bounds74(n::Offer& o,double x=0,double y=0,double z=0){
 o.boundsValid=true;for(int i=0;i<3;++i){o.bounds.local[i]=-.01f;o.bounds.local[i+3]=.01f;o.bounds.world[4*i+i]=1;}
 o.bounds.world[3]=float(x);o.bounds.world[7]=float(y);o.bounds.world[11]=float(z);
}
static void owned74(Fixture& q){
 q.in.proximityCraft1=q.in.handsOnlyD53=true;q.in.nativeFrame.offers[6].valid=false;
 q.admit(7,0);bounds74(q.in.nativeFrame.offers[7]);q.advance(90);
 auto& t=q.in.nativeFrame.table;t.handTypesKnown68=true;t.handType68[0]=7;t.actionKnown=true;t.action=0;
 check(q.step().logicalIngredientHeldD47);q.advance();
}
int main(){
 Fixture q;owned74(q);q.grip(0,0);q.in.nativeSelectedKnown=false;
 q.in.nativeSelectedTypeOnly=7;q.in.nativeTypeOnlyTick=q.in.now;auto o=q.step();
 check(q.state.ordinaryReleasePending&&!q.state.ordinaryReturning&&!o.use);
 check(o.nomination.decision.ref.slot==7&&o.nomination.decision.selfReturnRecipientD74);
 for(int i=0;i<20;++i){q.advance(20);o=q.step();check(!o.use&&o.nomination.decision.ref.slot==7);}
 q.advance();q.selected(8);o=q.step();check(!o.use&&o.nomination.decision.ref.slot==7);
 q.advance();q.selected(7);o=q.step();check(o.use&&o.ordinaryReturnRequested&&q.state.ordinaryReturning);
 q.advance();o=q.step();check(o.use&&o.nomination.decision.ref.slot==7);
 q.advance(90);o=q.step();check(!o.use); // once-only pulse, no repeated native effects
 Fixture regrip;owned74(regrip);regrip.grip(0,0);regrip.in.nativeSelectedKnown=false;regrip.step();
 regrip.advance();regrip.grip(0,1);regrip.selected(7);o=regrip.step();
 check(!o.use&&!regrip.state.ordinaryReleasePending&&!o.nomination.decision.selfReturnRecipientD74);
 Fixture moved;owned74(moved);moved.grip(0,0);moved.in.nativeSelectedKnown=false;moved.step();
 moved.advance();moved.in.palm[0].point={1,0,0};moved.selected(7);o=moved.step();check(!o.use&&!moved.state.ordinaryReleasePending);
 // Head hover/name fallback only when no hand target exists. Closing an empty
 // Grip retains empty ownership and never admits the distant native hover.
 Fixture idle;idle.in.proximityCraft1=idle.in.handsOnlyD53=true;
 for(auto& x:idle.in.nativeFrame.offers)x.valid=false;
 idle.selected(19);o=idle.step();check(o.nomination.decision.source==n::Source::Gaze&&!o.use&&!o.admitted);
 idle.grip(0,1);idle.advance();o=idle.step();check(o.nomination.decision.source==n::Source::Right&&!o.use&&!o.admitted);
 // Inverted vessel/contents use gravity rather than the hand's pointing ray.
 n::Offer pot{{6,106},{},1000,true};bounds74(pot,0,0,0);n::Palm palm{{0,0,.15},1000,4,5,6,0,0,true};
 n::AimH21 aim{{0,0,1},1000,4,5,6,0,true};
 check(n::pourRecipientD72(pot,palm));check(!n::palmCorridorD70(pot,palm,aim,1000));
 palm.point.x=.08;check(!n::pourRecipientD72(pot,palm));palm.point={0,0,.23};check(!n::pourRecipientD72(pot,palm));
 palm.point={0,0,-.03};check(!n::pourRecipientD72(pot,palm));
 Fixture vessel;owned74(vessel);vessel.in.gestureTransferD59=false;
 auto& dst=vessel.in.nativeFrame.offers[6];dst={{6,106},{.03,0,0},vessel.in.now,true};bounds74(dst,.03);
 vessel.selected(8);vessel.grip(0,0);o=vessel.step();check(!o.use&&o.nomination.decision.ref.slot==6&&o.transferStateD59==3);
 vessel.advance();vessel.selected(6);o=vessel.step();check(o.use&&o.ordinaryReturnRequested);
 std::printf("D74 crafting host regression: %u checks PASS; native effects/pixels not tested\n",checks);
}
