// Exercise the production input adapter and its production source selector.
// Native selection is explicit fixture evidence, never an inferred Use result.
#define KCD2VR_INPUT67_NO_MAIN
#include "input67_test.cpp"

static void targets72(Fixture& q,int activeSide){
 for(auto& offer:q.in.nativeFrame.offers)offer={};
 q.in.nativeFrame.offers[4]={{4,104},{.01,0,0},q.in.now,true};
 q.in.nativeFrame.offers[5]={{5,105},{1.06,0,0},q.in.now,true};
 q.in.palm[activeSide].point={1,0,0};q.in.palm[1-activeSide].point={0,0,0};
}
static void bounds72(n::Offer& offer,float x){
 offer.bounds={};offer.boundsValid=true;
 for(int j=0;j<3;++j){offer.bounds.local[j]=-.01f;offer.bounds.local[j+3]=.01f;offer.bounds.world[j*4+j]=1;}
 offer.bounds.world[3]=x;offer.point={99,99,99}; // only copied bounds qualify proximity
}
static void carry72(Fixture& q,int side){
 q.admit(5,side);auto o=q.step();check(!o.ordinaryHeld&&!o.control.held[side]);
 q.tool(side,5);o=q.step();check(o.ordinaryHeld&&o.control.held[side]); // real native tool join required
 q.advance(81);q.step();q.advance();
 q.in.palm[side].point={1,0,0};q.in.palm[1-side].point={2,0,0};
 q.in.nativeFrame.offers[6]={{6,106},{},q.in.now,true};bounds72(q.in.nativeFrame.offers[6],1.07f);
 q.in.nativeFrame.offers[4]={{4,104},{},q.in.now,true};bounds72(q.in.nativeFrame.offers[4],2.02f);
}
int main(){
 for(int side=0;side<2;++side){
  const auto source=side?n::Source::Left:n::Source::Right;
  // The closer idle hand cannot steal the initial deliberate Grip.
  Fixture first;targets72(first,side);first.grip(side,1);auto o=first.step();
  check(o.nomination.decision.source==source&&o.nomination.decision.ref.slot==5&&o.nomination.decision.gripping);
  check(!o.admitted&&!o.use);first.advance();first.selected(5);o=first.step();
  check(o.admitted&&o.use&&!o.control.held[side]); // actual native attach still required
  // A retained, unlatched hover cannot indefinitely block that same Grip.
  Fixture prior;targets72(prior,side);check(prior.step().nomination.decision.ref.slot==4);
  prior.advance();prior.grip(side,1);o=prior.step();
  check(o.nomination.decision.source==source&&o.nomination.decision.ref.slot==5&&o.nomination.decision.gripping&&!o.admitted);
  prior.advance();prior.selected(4);o=prior.step();check(!o.admitted&&!o.use); // native mismatch stays refused
  prior.advance();prior.selected(5);check(prior.step().admitted);
  // Once latched, an even closer new deliberate Grip cannot redirect it.
  Fixture holder;targets72(holder,side);holder.grip(side,1);holder.step();holder.advance();holder.grip(1-side,1);
  o=holder.step();check(o.nomination.decision.source==source&&o.nomination.decision.ref.slot==5&&o.nomination.decision.gripping);
  // The native A reference mode keeps its own passenger lifetime.
  holder.in.nativeA=true;o=holder.step();check(o.passenger&&o.semiControl.manualUse&&!o.admitted&&!o.use);
  // Invalid/stale/quarantined input cannot claim deliberate priority.
  for(int fault=0;fault<4;++fault){Fixture q;targets72(q,side);q.grip(side,1);
   if(fault==0)q.in.palm[side].valid=false;
   else if(fault==1)q.in.palm[side].tick=q.in.now-81;
   else if(fault==2)++q.in.palm[side].generation;
   else q.state.selection.quarantined[side]=true;
   o=q.step();check(o.nomination.decision.ref.slot==4&&!o.nomination.decision.gripping&&!o.admitted&&!o.use);
  }
  Fixture distant;targets72(distant,side);distant.in.nativeFrame.offers[5].point={1.100001,0,0};
  distant.grip(side,1);o=distant.step();check(o.nomination.decision.ref.slot==4&&!o.nomination.decision.gripping);
  // A real carried tool chooses its own recipient. The idle other palm is
  // closer to a different item, but cannot choose where this tool is returned.
  Fixture carried;carry72(carried,side);carried.selected(6);o=carried.step();
  check(o.ordinaryHeld&&o.control.held[side]&&o.nomination.decision.source==source&&o.nomination.decision.ref.slot==6);
  check(!o.nomination.decision.gripping); // source Grip remains private, no second acquisition
  carried.advance();carried.grip(side,0);o=carried.step();
  check(o.use&&o.passenger&&o.ordinaryReturnRequested&&!o.control.held[side]);
  // Recipient nomination never replaces the final native selection or actual
  // carrying-palm bounds proof. Nor can a missing native tool become held.
  for(int fault=0;fault<4;++fault){Fixture q;carry72(q,side);q.selected(6);q.step();q.advance();
   if(fault==0)q.selected(5);
   else if(fault==1)q.in.nativeFrame.offers[6].bounds.world[0]=0;
   else if(fault==2)q.in.nativeFrame.offers[6].bounds.world[3]=1.121f;
   else q.in.tool[side].joined=false;
   q.grip(side,0);o=q.step();check(!o.use&&!o.ordinaryReturnRequested&&!o.control.held[side]);
  }
 }
 // Simultaneous fresh Grip ties retain the original deterministic hand/slot order.
 Fixture tie;targets72(tie,0);tie.in.palm[0].point=tie.in.palm[1].point={0,0,0};
 tie.grip(0,1);tie.grip(1,1);auto o=tie.step();
 check(o.nomination.decision.source==n::Source::Right&&o.nomination.decision.ref.slot==4&&o.nomination.decision.gripping);
 // Idle hysteresis and an existing deliberate hover remain stable within10cm.
 Fixture idle;targets72(idle,1);idle.step();idle.advance();idle.in.nativeFrame.offers[5].point={1.001,0,0};
 o=idle.step();check(o.nomination.decision.ref.slot==4&&!o.nomination.decision.gripping);
 idle.advance();idle.grip(0,1);idle.in.nativeFrame.offers[5].point={.001,0,0};o=idle.step();
 check(o.nomination.decision.ref.slot==4&&o.nomination.decision.gripping);
 std::printf("selection_intent72: %u production input/selector checks PASS\n",checks);
}
