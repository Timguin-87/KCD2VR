// Production completed-selection adapter -> production State::step.
// Values below are from the 2026-10-30 capture, not a fabricated native verb.
// The capture's hint rows occur inside selection; this test supplies a stable
// completed-frame publication of the same identity. It does not replay native
// candidate enumeration or claim a headset/native action result.
#define KCD2VR_INPUT67_NO_MAIN
#include "input67_test.cpp"

namespace native63=kcd2vr::alchemy_input19c4;
static void productionAdapter63(Fixture& q){
 namespace alchemy_input19c4=kcd2vr::alchemy_input19c4;
 auto& in=q.in;const auto now=in.now;
 struct {std::uintptr_t alchemy;std::uint64_t lifetime;} c{in.context.instance,in.context.lifetime};
#include D59R1_SELECTION_ADAPTER
}
struct Recorded63 {
 const char* label;unsigned line,entity,type,actionType;int side;
 double gap;float grip;std::uint64_t tick;
};
static constexpr Recorded63 recorded63[]{
 // Water: paired C71 input line729843 and native hint line729897 share tick.
 {"water after book",729897,952230,13,19,1,.060194,.804f,65835578},
 // Idle mortar/pestle and post-resource herb/plate use native hint identities.
 // Their close geometry and deliberate grip are controlled test inputs.
 {"idle mortar",628054,952205,2,21,1,.089269,1.f,65799781},
 {"idle pestle",646817,952214,3,21,0,.05,1.f,65805640},
 {"herb after resource",778266,952204,8,13,0,.06,1.f,65852968},
 {"plate after resource",798369,952226,0,13,0,.100249,1.f,65860406},
};
static Fixture fixture63(const Recorded63& r){
 Fixture q;q.in.proximityCraft1=q.in.handsOnlyD53=true;
 q.in.context.instance=0x1F1F444A5A0ull;q.in.context.lifetime=3;
 auto& table=q.in.nativeFrame.table;table.instance=q.in.context.instance;table.lifetime=3;
 table.actionKnown=table.handTypesKnown68=true;table.handType68[0]=table.handType68[1]=21;
 q.in.action.instance=table.instance;q.in.action.lifetime=3;
 for(auto& offer:q.in.nativeFrame.offers)offer={};
 table.refs[r.type]={int(r.type),r.entity};
 auto& offer=q.in.nativeFrame.offers[r.type];
 offer={{int(r.type),r.entity},{r.gap+.01,0,0},q.in.now,true};offer.boundsValid=true;
 for(int j=0;j<3;++j){offer.bounds.local[j]=-.01f;offer.bounds.local[j+3]=.01f;offer.bounds.world[j*4+j]=1;}
 offer.bounds.world[3]=float(r.gap+.01);
 q.in.palm[r.side].point={0,0,0};q.in.palm[1-r.side].point={2,0,0};
 q.in.world[r.side].point[0]=0;q.in.world[1-r.side].point[0]=2;
 q.advance(unsigned(r.tick-q.in.now-10));
 native63::clear();q.step();q.advance();q.grip(r.side,r.grip);
 q.step(); // A squeeze latches a nomination; it cannot invent selection.
 q.advance();
 return q;
}
static void publish63(Fixture& q,const Recorded63& r){
 native63::observed[r.type].publish({q.in.context.instance,3,q.in.now,r.entity,r.type,false});
 native63::selected.publish({q.in.context.instance,3,q.in.now,r.entity,r.actionType});
 productionAdapter63(q);
}
static unsigned failures63=0;
static void expect63(bool value,const char* why){
 if(!value){++failures63;std::fprintf(stderr,"D63 acquisition FAIL: %s\n",why);}
}
int main(){
 for(const auto& r:recorded63){
  auto q=fixture63(r);publish63(q,r);auto out=q.step();
  expect63(q.in.nativeSelectedKnown&&q.in.nativeSelected.entity==r.entity&&q.in.nativeSelected.type==r.type,
           "current entity hint must survive idle/previous action input type");
  expect63(out.admitted&&out.use&&q.state.admittedRef.entity==r.entity&&q.state.admittedRef.slot==int(r.type),
           "coherent nearby selected entity + Grip must emit native Use");
  std::printf("D63 record line%u %s entity=%u hintType=%u actionInputType=%u known=%d admitted=%d use=%d\n",
   r.line,r.label,r.entity,r.type,r.actionType,q.in.nativeSelectedKnown,out.admitted,out.use);
  q.advance(81);publish63(q,r);expect63(!q.step().use,"release-hint Use pulse must finish without repeating");
 }
 // Book identity from the captured table, with controlled idle completion.
 Recorded63 book{"book boundary",0,524208,19,21,0,.101639,1.f,65805328};
 {auto q=fixture63(book);publish63(q,book);const auto out=q.step();
  expect63(out.admitted&&out.use,"book must remain selectable without an active action type");}
 // Selection/Grip remain required. Removing the false action-type gate must
 // not create input from a candidate, stale hint, or unpressed open palm.
 {auto q=fixture63(recorded63[0]);publish63(q,recorded63[0]);q.grip(1,0);
  expect63(!q.step().use,"open palm must not emit native Use");}
 {auto q=fixture63(recorded63[0]);publish63(q,recorded63[0]);native63::selected.publish({});productionAdapter63(q);
  expect63(!q.step().use,"missing completed native selection must not admit");}
 {auto q=fixture63(recorded63[0]);publish63(q,recorded63[0]);q.advance(81);productionAdapter63(q);
  expect63(!q.in.nativeSelectedKnown&&!q.step().use,"stale native hint/selection must not admit");}
 {auto q=fixture63(recorded63[0]);publish63(q,recorded63[0]);q.in.nativeA=true;const auto out=q.step();
  expect63(out.passenger&&!out.use&&out.semiControl.manualUse,"physical A must keep native input ownership");}
 {auto q=fixture63(recorded63[0]);publish63(q,recorded63[0]);auto& t=q.in.nativeFrame.table;
  t.nativeRetortD56=true;t.action=q.in.action.parent=900;const auto out=q.step();
  expect63(out.passenger&&!out.use&&!out.control.trackedPresentation71&&out.semiControl.manualUse,
           "native Hold-A distillation must keep authored fixture and native input");}
 native63::clear();
 std::printf("D63 acquisition adapter: %s; native engine/headset NOT_RUN\n",failures63?"FAIL":"PASS");
 return failures63?1:0;
}
