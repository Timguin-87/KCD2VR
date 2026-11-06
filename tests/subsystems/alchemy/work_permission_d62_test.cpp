// Actual production gesture controller; native identity/contact observations
// remain explicit fixture inputs. No engine effect or particle is synthesized.
#define KCD2VR_GESTURE_WORK67_FIXTURE_ONLY_D62
#include "gesture_work67_test.cpp"

static ga::Output working(Rig& r,unsigned kind){
 r.step();
 if(kind==ga::Resource)r.tilt(.9f);
 else if(kind==ga::Mortar)r.frame.hand[0].position[0]=.01f;
 else r.frame.hand[1].position[1]=-.02f;
 auto out=r.next();check(out.totalSeconds>0&&!out.clockStop,"fixture earns current physical work");return out;
}
static void sameSampleGeometry(){
 for(unsigned kind:{ga::Resource,ga::Mortar,ga::Bellows}){
  Rig r(kind);const auto amount=working(r,kind).totalSeconds;
  if(kind==ga::Resource)r.in.pot.valid=false;
  else if(kind==ga::Mortar)r.in.mortar.valid=false;
  else r.in.tableContact.geometryValid=false;
  auto out=r.step();check(out.clockStop&&out.totalSeconds==amount&&!out.use,"lost native geometry stops even a repeated XR sample");
  if(kind==ga::Resource)r.in.pot.valid=true;
  else if(kind==ga::Mortar)r.in.mortar.valid=true;
  else r.in.tableContact.geometryValid=true;
  out=r.step();check(out.clockStop&&out.totalSeconds==amount,"restoring geometry cannot revive the already stopped sample");
  if(kind==ga::Resource){out=r.next();check(!out.clockStop&&out.totalSeconds>amount,"new sustained tilt can resume on fresh source sample");}
  else{
   if(kind==ga::Mortar)r.frame.hand[0].position[0]=.03f;else r.frame.hand[1].position[1]=-.04f;
   out=r.next();check(out.clockStop&&out.totalSeconds==amount,"returning contact establishes baseline without gap travel");
   if(kind==ga::Mortar)r.frame.hand[0].position[0]=.04f;else r.frame.hand[1].position[1]=-.05f;
   out=r.next();check(!out.clockStop&&out.totalSeconds>amount,"new contact-qualified movement resumes without another Use");
  }
 }
 // Current contact may disappear while its parent/tool identities stay valid.
 for(int fault=0;fault<5;++fault){
  Rig r(ga::Mortar);r.in.mortarContact={true,true,100,200,7,1000,10,20,500,501,.005,.02};
  r.step();r.frame.hand[0].position[0]=.01f;r.advance();r.in.mortarContact.tick=r.frame.tick;
  const auto amount=r.step().totalSeconds;check(amount>0,"joined mortar contact earns initial movement");
  switch(fault){case 0:r.in.mortarContact.valid=false;break;case 1:r.in.mortarContact.parent++;break;
   case 2:r.in.mortarContact.tick=r.frame.tick-81;break;case 3:r.in.mortarContact.gap=.036;break;
   case 4:r.in.mortarContact.pestle++;break;}
  const auto out=r.step();check(out.clockStop&&out.totalSeconds==amount,"same-sample native contact loss cannot keep grinding permission");
 }
 {Rig r;const auto amount=working(r,ga::Resource).totalSeconds;r.in.pot.point[0]=1;
  const auto out=r.step();check(out.clockStop&&out.totalSeconds==amount&&out.status==ga::Status::MoveToPot,"moved pot revokes old sample's pour permission");}
}
static void childBoundary(){
 for(unsigned kind:{ga::Resource,ga::Mortar,ga::Bellows}){
  Rig r(kind);working(r,kind);++r.in.work.child;++r.in.work.token;
  const auto out=r.step();check(out.clockStop&&out.totalSeconds==0&&!out.use,"new child cannot inherit previous child's motion permission on one source sample");
 }
 {Rig r(ga::Mortar);r.in.work.working=false;r.in.work.ready=true;r.step();r.frame.hand[0].position[0]=.02f;r.next();
  check(r.controller.strokeTravel>.019f,"ready stroke has unspent partial physical travel");
  ++r.in.work.child;++r.in.work.token;auto out=r.step();
  check(r.controller.strokeTravel==0&&out.clockStop&&!out.use,"new child discards previous ready-stroke fraction");
  r.frame.hand[0].position[0]=.04f;r.next();r.frame.hand[0].position[0]=.05f;out=r.next();
  check(!out.use,"partial old plus partial new motion cannot request another native cycle");}
 {Rig r(ga::Mortar);r.in.work.working=false;r.in.work.ready=true;r.step();r.frame.hand[0].position[0]=.03f;
  auto out=r.next();check(out.use,"qualified ready gesture emits one native input pulse");
  r.in.mortar.valid=false;out=r.step();check(!out.use&&out.clockStop,"lost contact retires that pulse immediately");
  r.in.mortar.valid=true;out=r.step();check(!out.use,"restored contact does not replay retired native input pulse");}
}
static void observedHoldIsNotDropout(){
 for(bool released:{false,true}){
  Rig r;const auto amount=working(r,ga::Resource).totalSeconds;
  if(released)r.frame.hand[1].grip=.1f;else{r.in.work.phase=2;r.in.work.working=false;}
  for(unsigned i=0;i<50;++i){const auto out=r.next();
   check(out.enabled&&out.clockStop&&out.totalSeconds==amount,"continuously observed held/released state keeps its consumed ledger beyond 80 ms");
   check(released?out.releaseRequested:out.completedHeld,"observed state does not masquerade as tracking loss");}
  r.in.nativeReturnAuthorized=true;r.frame.hand[1].grip=.1f;
  check(r.next().status==ga::Status::NativeReturn,"genuine native return still retires the same resource");
 }
}
int main(){sameSampleGeometry();childBoundary();observedHoldIsNotDropout();
 std::printf("D62 alchemy current-work permission: %u checks PASS; native observations are fixture leaves\n",checks);}
