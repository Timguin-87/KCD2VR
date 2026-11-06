#include "vrik/hand_pair_contact_d39.h"
#include "data/finger_pair_i10_r29_d39.h"
#include <cstdio>
#include <limits>
namespace m=kcd2vr::pose_core;
namespace hp=kcd2vr::hand_pair_contact_d39;
namespace hs=kcd2vr::hand_shape59c7f;
static int checks=0,failures=0;
static void ck(bool ok,const char* label){++checks;if(!ok){++failures;std::printf("FAIL %s\n",label);}}
static bool equal(m::Vec3 a,m::Vec3 b){return m::length(a-b)<1e-10;}
static hp::Input input(unsigned sample){
 hp::Input i{};i.key={0x2432B186720,0x1234,7,5,20,4,9223372036854775816ull,2,13};
 i.sample=sample?76358:76354;i.tick=sample?1096862250:1096862234;
 i.timeNs=sample?1096862272508700:1096862261499000;i.now=i.tick;i.eligible=true;return i;
}
static hp::Staged run(const hp::State& old,hp::Input in,const pair_i10_d39::Sample& f){
 return hp::stage(old,in,f.hand[0],f.hand[1],f.requested[0],f.requested[1]);
}
static pair_i10_d39::Sample transformed(pair_i10_d39::Sample f,m::Quat q,m::Vec3 t){
 for(unsigned side=0;side<2;++side){f.requested[side]=m::rotate(q,f.requested[side])+t;
  for(unsigned p=0;p<f.hand[side].count;++p){f.hand[side].part[p].a=m::rotate(q,f.hand[side].part[p].a);f.hand[side].part[p].b=m::rotate(q,f.hand[side].part[p].b);}}
 return f;
}
int main(){
 const auto f=pair_i10_d39::samples();hs::Result old[2]{};
 const m::Vec3 logged[2][2]{{{.07068,.15124,1.31765},{-.11198,.17320,1.29355}},{{.06962,.17055,1.29739},{-.11039,.15443,1.31336}}};
 for(int n=0;n<2;++n){old[n]=hs::separate(f[n].hand[0],f[n].hand[1],f[n].requested[0],f[n].requested[1]);
  ck(old[n].valid&&old[n].contact,"unchanged stateless production kernel reproduces contact");
  for(int side=0;side<2;++side)ck(m::length(f[n].requested[side]+old[n].correction[side]-logged[n][side])<.0002,"full finger replay matches rounded logged wrist within 0.2 mm");}
 const auto a=run({},input(0),f[0]);const auto b=run(a.state,input(1),f[1]);
 ck(a.contact.valid&&b.contact.valid&&a.state.touching&&b.state.touching&&b.continued,"qualified current contact continues");
 ck(equal(a.contact.correction[0],old[0].correction[0]),"new episode preserves first contact solution");
 for(int side=0;side<2;++side){
  const double raw=m::length(f[1].requested[side]-f[0].requested[side]);
  const double before=m::length(f[1].requested[side]+old[1].correction[side]-f[0].requested[side]-old[0].correction[side]);
  const double after=m::length(f[1].requested[side]+b.contact.correction[side]-f[0].requested[side]-a.contact.correction[side]);
  std::printf("I10 full-finger side=%d controller=%.6f mm stateless=%.6f mm continuity=%.6f mm\n",side,raw*1000,before*1000,after*1000);
  ck(before>.025&&after<.005,"recorded 28 mm normal reversal eliminated in same real geometry");
 }
 ck(equal(b.contact.correction[0],b.contact.correction[1]*-1.),"pair response remains symmetric");
 ck(m::length(b.contact.correction[0])<=.06,"original 6 cm total correction bound retained");
 const auto duplicate=run(b.state,input(1),f[1]);
 ck(duplicate.duplicate&&equal(duplicate.contact.correction[0],b.contact.correction[0]),"duplicate sample reuses beginning-of-sample normal and is idempotent");
 hp::State committed=a.state;hp::commit(committed,b.state,false,true);
 ck(committed.sample==a.state.sample&&equal(committed.normal,a.state.normal),"failed paired publication cannot commit staged history");
 hp::commit(committed,b.state,true,false);ck(committed.sample==a.state.sample,"late owner refusal discards staged history");
 hp::commit(committed,b.state,true,true);ck(committed.sample==b.state.sample,"exact successful publication commits staged history");
 for(int key=0;key<9;++key){auto i=input(1);
  switch(key){case 0:++i.key.character;break;case 1:++i.key.skeleton;break;case 2:++i.key.poseEpoch;break;case 3:++i.key.trackingEpoch;break;
   case 4:i.key.policy+=2;break;case 5:++i.key.contactControl;break;case 6:++i.key.frameControl;break;case 7:++i.key.nativeAdmission;break;case 8:++i.key.geometry;break;}
  const auto r=run(a.state,i,f[1]);ck(!r.continued&&equal(r.contact.correction[0],old[1].correction[0]),"owner/policy/geometry change starts fresh contact");}
 for(int bad=0;bad<7;++bad){auto i=input(1);switch(bad){case 0:i.eligible=false;break;case 1:i.now=i.tick-1;break;case 2:i.now=i.tick+81;break;
  case 3:i.sample=input(0).sample-1;break;case 4:i.tick=input(0).tick-1;i.now=i.tick;break;case 5:i.timeNs=input(0).timeNs-1;break;
  case 6:i.sample=input(0).sample;break;}
  const auto r=run(a.state,i,f[1]);ck(!r.state.touching&&!r.contact.valid,"ineligible/stale/future/backwards/inconsistent stamp retires contact");}
 auto gap=input(1);gap.tick=input(0).tick+81;gap.now=gap.tick;
 ck(!run(a.state,gap,f[1]).continued,"tracking gap over 80 ms cannot borrow old separating side");
 auto nsGap=input(1);nsGap.timeNs=input(0).timeNs+81000000ull;
 ck(!run(a.state,nsGap,f[1]).continued,"original nanosecond source gap also retires continuity");
 auto changing=input(1);++changing.key.policy;
 ck(!run(a.state,changing,f[1]).contact.valid,"odd policy generation marks an in-progress owner change, not contact authority");
 auto separated=f[1];separated.requested[0].x+=1.;const auto cleared=run(a.state,input(1),separated);
 ck(cleared.contact.valid&&!cleared.contact.contact&&!cleared.state.touching,"clear separation retires episode");
 ck(!run(cleared.state,input(1),f[1]).continued,"new contact after separation is not latched to stale side");
 auto large=f[1];large.requested[0].z+=.06;
 ck(!run(a.state,input(1),large).continued,"large relative controller discontinuity rejects old normal");
 auto invalid=f[1];invalid.hand[0].part[0].radius=std::numeric_limits<double>::quiet_NaN();
 ck(!run(a.state,input(1),invalid).contact.valid,"nonfinite shape is refused");
 invalid=f[1];invalid.hand[0].count=27;ck(!run(a.state,input(1),invalid).contact.valid,"invalid shape count refused before indexing");
 const auto q=m::from_rotation_vector({.3,-.4,1.2});const m::Vec3 translation{7,-4,2};
 auto i0=input(0),i1=input(1);i0.basis=q;i1.basis=q;
 const auto qa=run({},i0,transformed(f[0],q,translation));const auto qb=run(qa.state,i1,transformed(f[1],q,translation));
 ck(equal(qb.contact.correction[0],m::rotate(q,b.contact.correction[0])),"rigid model rotation/translation covariance");
 // The frame may turn between samples. Transport the committed normal into the
 // current calibrated basis; do not retain a stale model/world-space direction.
 const auto moved=run(a.state,i1,transformed(f[1],q,translation));
 ck(moved.continued&&equal(moved.contact.correction[0],m::rotate(q,b.contact.correction[0])),"inter-sample basis rotation transports contact witness");
 auto malformed=input(1);malformed.basis={0,0,0,0};ck(!run(a.state,malformed,f[1]).contact.valid,"invalid transform has no authority");
 std::printf("hand_pair_contact_d39: %d checks, %d failures\n",checks,failures);return failures?1:0;
}
