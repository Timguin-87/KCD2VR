// Actual complete native weapon client, with the canonical native phase/leaf
// fixture. Controlled failure returns do not stand in for retail execution.
#define main native_weapon_existing_main_r96
#include "native_weapon_body_t4_test.cpp"
#undef main

int main(int argc,char** argv){
 using namespace fixture;
 const std::string mode=argc>1?argv[1]:"recovery";
 assert(w::SetSourcePredicate(&Source));assert(w::Install());w::SetEnabled(true);
 auto in=Input();postEffectFault=true;Fresh(in);Tick();pending=false;Advance(in);Advance(in);
 assert(w::record.quarantined&&w::fault.load()&&w::record.lease.Held()&&w::record.geometry&&collision);
 const auto retainedGeometry=w::record.geometry;const auto retainedOwner=w::record.input.owner;
 const auto originalGeneration=w::record.generation;
 const auto originalCookie=w::record.foreignCookieH24;
 const auto created=creates,removed=removes,released=releases,unregistered=unregisters;
 w::Achieved out{};
 if(mode=="remove-failure")removeOkR96=false;
 else if(mode=="release-failure")releaseOkR96=false;
 else if(mode=="unregister-failure")unregisterOkR96=false;
 else if(mode=="constraint-debt")w::record.constraintPinned=true;
 else if(mode=="uncertain-pair")w::selfPairH5.uncertain=true;
 else if(mode=="old-generation")++generation;
 else if(mode=="unknown-factory"){
  // A fault can occur after Create's side effect but before it returns a body.
  // Its retained native state is explicitly unknown, not an empty owner.
  p::Access::Set(w::record.lease,false);w::record.geometry=nullptr;w::record.geometryManager=nullptr;
  w::record.hasGeometry=w::record.partAttempted=false;
 }
 else if(mode=="world-replaced")++generation;
 else assert(mode=="recovery");
 auto& context=p::Access::Ctx();
 p::Access::PhaseOf(mode=="world-replaced"?p::Phase::WorldReplaced:p::Phase::WorldReset);
 client.reset(context,nullptr);
 if(mode=="recovery"){
  assert(removes==removed+1&&releases==released+1&&unregisters==unregistered+1);
  assert(!w::record.lease.Held()&&!w::record.geometry&&!w::record.quarantined&&!w::fault.load());
  assert(!w::HasUnretiredCollider()&&w::Read(out)&&out.colliderRetired&&!out.poseValid);
  assert(w::Install()&&registerCalls==1&&phaseInstallCalls==1);
  // Original submitted packet was retired: no implicit replay or new Create.
  alive=false;geometry=collision=false;postEffectFault=false;++generation;
  Tick();assert(creates==created&&!w::HasUnretiredCollider());
  in.owner.request++;in.cancelGeneration++;in.colliderLease++;Active(in);
  assert(creates==created+1&&w::record.generation!=originalGeneration&&
      w::record.foreignCookieH24!=originalCookie&&w::Read(out)&&out.poseValid&&out.state==w::State::Active);
  Cleanup();
  std::puts("PASS exact same-world reset cleanup recovers actual client; old input retired; new owner/cookie creates and reaches ACTIVE");
 }else{
  assert(w::record.quarantined&&w::fault.load()&&w::HasUnretiredCollider());
  assert(w::Read(out)&&!out.colliderRetired&&!out.safeNativeRestore&&!out.poseValid);
  assert(w::record.input.owner==retainedOwner);
  if(mode!="unknown-factory")assert(w::record.geometry==retainedGeometry);
  if(mode=="remove-failure")assert(removes==removed+1&&releases==released&&unregisters==unregistered);
  if(mode=="release-failure")assert(removes==removed+1&&releases==released+1&&unregisters==unregistered);
  if(mode=="unregister-failure")assert(removes==removed+1&&releases==released+1&&unregisters==unregistered+1);
  if(mode=="constraint-debt"||mode=="uncertain-pair"||mode=="old-generation"||mode=="unknown-factory"||mode=="world-replaced")
   assert(removes==removed&&releases==released&&unregisters==unregistered);
  const auto beforeCreates=creates;
  w::SetEnabled(false);w::SetEnabled(true);postEffectFault=false;
  Fresh(in);Tick();assert(creates==beforeCreates);
  std::printf("PASS %s retains exact occurrence; no cleanup through a new world and no false retirement/replay\n",mode.c_str());
 }
}
