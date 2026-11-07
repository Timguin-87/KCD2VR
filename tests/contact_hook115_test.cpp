// Real installer/dispatcher with a patching hook spy. Tests hook ownership,
// SHA/entry rejection and unchanged single native forwarding, not WHGame code.
#include <cstdint>
static void* serviceCaller54=nullptr;
#if defined(_MSC_VER)
#ifdef _ReturnAddress
#undef _ReturnAddress
#endif
#define _ReturnAddress() serviceCaller54
#else
#define __builtin_return_address(n) serviceCaller54
#endif
#define KCD2VR_TEST_HOOK_SPY115
#include "../src/combat_diag_probe.cpp"
#include <vector>
#include <cstdlib>
namespace kcd2vr::diagnostic_trace {void Write(const char*) noexcept {}}
namespace kcd2vr::retail_build112 {bool verified=true;bool Verify(std::uintptr_t b) noexcept {return b&&verified;}bool Ready(std::uintptr_t b) noexcept {return b&&verified;}}
namespace kcd2vr::frik_pose_probe {
// This passive native probe fixture has no physical fist owner.
int FistContactAdmissionH12(const void*,const float*,int,const void*) noexcept {return -1;}

// Diagnostic-only native namespace reader is absent in this passive combat fixture.
void ObserveHitPartNamespace62(const kcd2vr::part_namespace62::Ticket&) noexcept {}

bool contactMode59c3=false,integrated59c3=false;
bool CombatExperiment175Enabled() noexcept {return contactMode59c3;}
bool PhysicalCombat177Enabled() noexcept {return contactMode59c3;}
bool CombatIntegrated178r22c2Enabled() noexcept {return integrated59c3;}
int F4ContactGateMode178r26() noexcept {return 1;}
bool GeometryAutoLock178r29() noexcept {return false;} // R29 stub
bool GeometryOwnerValid178r30(std::uint64_t,std::uint64_t,std::uint64_t,std::uint64_t) noexcept {return true;} // R30 stub
bool GeometryAdmittedRelease51(const kcd2vr::contact176::Ticket&,const void*) noexcept {return false;} // passive hook fixture owns no admitted action
bool GeometryNativeAnswer19c4r2(const void*,const void*,int,kcd2vr::answer59c4r2::Lineage&) noexcept {return false;}
bool GuardOwnedByUs59c7e(const void*) noexcept {return false;}
int GeometryOwnerStage49() noexcept {return 0;} // R49 stub
bool ReadF4ContactLive176(kcd2vr::contact176::Live&) noexcept {return false;}
bool PhysicalWeaponInputOwned6698() noexcept { return false; } // No gameplay owner in this passive contact fixture.
void NotifyNativeWeaponImpact136R2(float,float,float,bool,bool,bool,std::uint32_t) noexcept {} // Haptic leaf; notification behavior has its own full detour fixture.
void* target122=nullptr;
int mode164=0;
int f11Mode167=0;
int FocusedTestMode167() noexcept {return f11Mode167;}
int WeaponControlMode() noexcept {return mode164;}
void* LastPlayerEntity() noexcept {return target122;}
void* WeaponControlEntity(int) noexcept {return nullptr;}
void ObserveCollisionAnimationCharacter(void*,std::uint32_t,std::uint64_t) noexcept {}
}
namespace kcd2vr::animation_filter {
void MarkContactForPlayback(std::uint64_t) noexcept {}
PlaybackStamp DiagnosticPlaybackStamp() noexcept {return {};}
}
using namespace kcd2vr::combat_diag_probe;
static int checks=0,forwarded=0,observed=0;static const void* observedRaw=nullptr;
static void ck(bool b,const char* message){++checks;if(!b){std::fprintf(stderr,"FAIL %s\n",message);std::exit(1);}}
template<class T>void put(unsigned char*p,T value){std::memcpy(p,&value,sizeof(value));}
static void native(void*,const void*,const void*,const void* raw){++forwarded;ck(raw==observedRaw,"original receives untouched raw event pointer");}
static bool enrich(void*,void*,const void*,const void*,const void*,const void*){return true;}
static void* characterGetter63(void*,int){return nullptr;}
static void observe(void*,const void*,const void*,const void* raw) noexcept {++observed;observedRaw=raw;}

static int serviceForwards=0;
static std::array<unsigned char,0x30> effectResult{};
static void* serviceEffect(void* context,void* transaction,std::uint64_t wuid,float delta,unsigned char flag){
 ++serviceForwards;ck(context==reinterpret_cast<void*>(0x55)&&transaction==reinterpret_cast<void*>(0x66)&&wuid==0x77&&delta==-8.0f&&flag==2,"effect ABI forwards each argument unchanged");return effectResult.data();
}
static void serviceCommit(void* rpg){
 ++serviceForwards;auto actor=nhp134::Rd<unsigned char*>(rpg,8,nullptr);auto state=nhp134::Rd<unsigned char*>(actor,0x210,nullptr);
 ck(nhp134::services59c5::Effect(reinterpret_cast<void*>(0x55),reinterpret_cast<void*>(0x66),0x77,-8.0f,2)==effectResult.data(),"effect result unchanged");
 put(state+0x1E8,.42f);
}
static void serviceObservationTest(std::uintptr_t b){
 namespace svc=nhp134::services59c5;namespace data=kcd2vr::diagnostics::services59c5;
 std::array<unsigned char,0x700> actor{},player{};std::array<unsigned char,0xD10> state{};
 std::array<unsigned char,0x80> rpg{},entity{};
 put(actor.data(),b+nhp134::kCombatPlayerVft);put(actor.data()+0x1F8,player.data());put(actor.data()+0x210,state.data());put(actor.data()+0x388,rpg.data());
 put(player.data(),b+nhp134::kPlayerVft);put(player.data()+0x278,actor.data());put(player.data()+0x38,entity.data());
 put(entity.data(),b+nhp134::kEntityVft);put<std::uint32_t>(entity.data()+0xC,42);put(state.data()+0xCE0,actor.data());put(rpg.data()+8,actor.data());
 kcd2vr::frik_pose_probe::target122=entity.data();g_enabled=true;kcd2vr::diagnostics::lab::state=1;
 auto& gate=kcd2vr::diagnostics::capture168::gate;const auto domain=kcd2vr::diagnostics::capture168::Domain::Combat;gate.select(domain,true,hostTickCount64);
 svc::commit=&serviceCommit;svc::effect=&serviceEffect;
 const auto before=actor;svc::Commit(rpg.data());ck(serviceForwards==2&&actor==before,"service observation forwards once without actor writes");
 data::Record a{},e{},z{};ck(data::journal.pop(a)&&data::journal.pop(e)&&data::journal.pop(z),"bounded service records exist");
 ck(a.kind==data::Kind::CommitBegin&&e.kind==data::Kind::EffectBuilt&&z.kind==data::Kind::CommitEnd&&a.span==e.span&&a.span==z.span,"same native stack joins effort construction to wrapper completion");
 ck(e.delta==-8.0f&&e.actorWuid==0x77&&z.strength==.42f,"effect request and returned strength observed separately");
 gate.select(domain,false,hostTickCount64);svc::Commit(rpg.data());ck(serviceForwards==4&&!data::journal.pop(a),"capture off preserves both native calls with no records");
 gate.select(domain,true,hostTickCount64);put<std::uintptr_t>(player.data()+0x278,0);svc::Commit(rpg.data());ck(serviceForwards==6&&!data::journal.pop(a),"invalid owner still forwards but cannot publish a Henry span");
 gate.select(domain,false,hostTickCount64);g_enabled=false;svc::commit=nullptr;svc::effect=nullptr;kcd2vr::frik_pose_probe::target122=nullptr;
}
static float nativeResource54=80;
static float __fastcall nativeResourceRead54(const void*,int state){ck(state==1,"native cost reads stamina resource1");return nativeResource54;}
static void* nativeEffectReturn54(void*,void*,std::uint64_t wuid,float delta,unsigned char flag){
 ck(wuid==0x77&&(flag==1?delta==-3.f:delta==-7.f),"native cost observer preserves original cost delta");return effectResult.data();
}
static void nativeRaise54(void*,bool up){if(!up)return;const auto old=serviceCaller54;
 serviceCaller54=reinterpret_cast<void*>(nhp134::g_base+0xC5BBEB);nhp134::services59c5::Effect(nullptr,nullptr,0x77,-3.f,1);
 nativeResource54-=3;serviceCaller54=old;
}
static void nativeCommit54(void*){const auto old=serviceCaller54;
 serviceCaller54=reinterpret_cast<void*>(nhp134::g_base+0xC64DCA);nhp134::services59c5::Effect(nullptr,nullptr,0x77,-7.f,2);
 nativeResource54-=7;serviceCaller54=old;
}
static kcd2vr::swing_effort59c14::NativeCost54 dispatchModel54{};static bool dispatchAccepted54=true;
static bool nativeActionDispatch54(const void* actor,void* holder){namespace se=kcd2vr::swing_effort59c14;
 auto c=dispatchModel54;ck(reinterpret_cast<std::uintptr_t>(actor)==c.actor&&nhp134::Rd<std::uintptr_t>(holder,0,0)==c.action,"native dispatcher receives original actor and action holder");
 ck(se::actionRegistry54.start(c.action,hostTickCount64,c.causal54),"actual dispatch nested MainStart finds exact constructor-bound action");
 ck(kcd2vr::motion59c4::same(c.key,c.causal54.key),"native MainStart gets immutable dispatch key rather than latest intent");
 se::NativeCostScope54 scope(c);auto* rpg=nhp134::Rd<void*>(actor,0x388,nullptr);nhp134::services59c5::Raised(rpg,true);
 const auto old=serviceCaller54;serviceCaller54=reinterpret_cast<void*>(nhp134::g_base+0x93921C);nhp134::services59c5::Commit(rpg);serviceCaller54=old;
 ck(!scope.finish(true,hostTickCount64)&&!se::ledger.paidFor54(c.key,hostTickCount64),"completed native submissions wait for real dispatcher return");return dispatchAccepted54;
}
static void nativeCostObservation54(){
 namespace svc=nhp134::services59c5;namespace se=kcd2vr::swing_effort59c14;
 const auto oldBase=nhp134::g_base;const auto oldTarget=kcd2vr::frik_pose_probe::target122;
 // Only the already known resource leaf is synthetic. No WHGame code executes.
 const auto base=reinterpret_cast<std::uintptr_t>(&nativeResourceRead54)-0x642B30;
 nhp134::g_base=base;
 std::array<unsigned char,0x700> actor{},player{};std::array<unsigned char,0xD10> state{};
 std::array<unsigned char,0x80> rpg{},entity{},soul{};
 put(actor.data(),base+nhp134::kCombatPlayerVft);put(actor.data()+0x1F8,player.data());put(actor.data()+0x210,state.data());put(actor.data()+0x388,rpg.data());
 put(player.data(),base+nhp134::kPlayerVft);put(player.data()+0x278,actor.data());put(player.data()+0x38,entity.data());put(player.data()+0x668,soul.data());
 put(entity.data(),base+nhp134::kEntityVft);put(rpg.data()+8,actor.data());put(state.data()+0xCE0,actor.data());
 put(state.data()+nhp134::kAttackType+8,1);put(state.data()+nhp134::kAttackingHand+8,0);
 put(state.data()+0x8C0,base+0x3B0A5E8);put<std::uint64_t>(state.data()+0x8C8,0x300);
 put(soul.data(),base+0x3F44F28);put<std::uint64_t>(soul.data()+0x30,0x77);
 kcd2vr::frik_pose_probe::target122=entity.data();
 se::NativeCost54 cost{};cost.key={91,0x300,3,4,5,6,reinterpret_cast<std::uintptr_t>(player.data())};
 cost.actor=reinterpret_cast<std::uintptr_t>(actor.data());cost.state=reinterpret_cast<std::uintptr_t>(state.data());cost.action=2;cost.helper=3;cost.row=4;cost.type=1;cost.hand=0;
 const auto bindCost54=[&](){se::actionRegistry54.construct(cost.action,cost.actor,true,hostTickCount64);
  se::Input54 input{cost.key,cost.actor,cost.state,hostTickCount64};se::Action54 binding{};kcd2vr::motion59c4::Key credit{};
  ck(se::actionRegistry54.bind(input,cost.action,cost.helper,cost.row,hostTickCount64,binding),"actual native dispatch binds immutable physical key");
  ck(!se::actionRegistry54.dispatchFinish(binding,true,hostTickCount64,credit),"dispatch alone does not credit cost");
  ck(se::actionRegistry54.start(cost.action,hostTickCount64,cost.causal54),"later MainStart uses exact dispatched action allocation");};
 svc::raised=nativeRaise54;svc::commit=nativeCommit54;svc::effect=nativeEffectReturn54;
 se::ledger.clear();ck(se::ledger.publish(cost.key,.8,1.6,false,hostTickCount64),"native cost exact pending energy");
 bindCost54();
 {se::NativeCostScope54 scope(cost);svc::Raised(rpg.data(),true);
  serviceCaller54=reinterpret_cast<void*>(base+0x93921C);svc::Commit(rpg.data());serviceCaller54=nullptr;
  ck(scope.value.raised==1&&scope.value.struck==1&&!scope.value.invalid,"actual wrappers join own exact native effects with recorder off");
  ck(scope.finish(true,hostTickCount64),"both completed wrappers credit already native-paid gesture");}
 ck(nativeResource54==70&&se::ledger.paidFor54(cost.key,hostTickCount64),"one original30/70 cost, no duplicate native debit");
 // Real preparation precedes MainAttack's assigned type/hand. Preserve its
 // actual native values; do not reinterpret native type-1 as a cut request.
 nativeResource54=80;++cost.key.gesture;ck(se::ledger.publish(cost.key,.8,1.6,false,hostTickCount64),"split native ancestry has own immutable gesture");bindCost54();
 auto prep=cost;prep.preparationOnly54=true;prep.nativeFields54=true;prep.type=-1;prep.hand=1;prep.nativeWeapon54=0;
 put(state.data()+nhp134::kAttackType+8,-1);put(state.data()+nhp134::kAttackingHand+8,1);
 {se::NativeCostScope54 scope(prep);svc::Raised(rpg.data(),true);ck(scope.finish(true,hostTickCount64),"actual input-scope native preparation is retained without attack credit");}
 ck(!se::ledger.paidFor54(cost.key,hostTickCount64),"preparation alone never grants full effort");
 put(state.data()+nhp134::kAttackType+8,1);put(state.data()+nhp134::kAttackingHand+8,0);
 {se::NativeCostScope54 scope(cost);serviceCaller54=reinterpret_cast<void*>(base+0x93921C);svc::Commit(rpg.data());serviceCaller54=nullptr;
  ck(scope.finish(true,hostTickCount64),"actual native MainAttack strike joins ONLY same input preparation key");}
 ck(nativeResource54==70&&se::ledger.paidFor54(cost.key,hostTickCount64),"split preparation/strike original cost preserved without extra debit");
 // Execute the real 90C850 detour, including its exact caller/class/actor/
 // helper/row checks, with a native-dispatch leaf that starts synchronously.
 std::array<unsigned char,0xB0> action{};std::array<unsigned char,0x50> helper{};std::array<unsigned char,0x230> row{};std::array<unsigned char,0x20> type{};
 put(action.data(),base+0x3A61C58);put(action.data()+0x70,actor.data());put(action.data()+0x58,row.data());put(action.data()+0xA0,helper.data());put(action.data()+0x88,base+0x3A61440);
 put(helper.data(),base+0x3A61040);put(helper.data()+8,actor.data());put(helper.data()+0x20,action.data()+0x88);put(row.data()+0x148,type.data());put<int>(type.data()+8,1);put<int>(row.data()+0x224,0);
 cost.action=reinterpret_cast<std::uintptr_t>(action.data());cost.helper=reinterpret_cast<std::uintptr_t>(helper.data());cost.row=reinterpret_cast<std::uintptr_t>(row.data());
 se::actionContractBase54=base;nhp134::g_reaction_dispatch_original50=nativeActionDispatch54;
 for(bool accepted:{true,false}){nativeResource54=80;++cost.key.gesture;dispatchModel54=cost;dispatchAccepted54=accepted;
  se::Input54 input{cost.key,cost.actor,cost.state,hostTickCount64};auto* held=action.data();
  ck(se::actionRegistry54.construct(cost.action,cost.actor,true,hostTickCount64)!=0,"actual factory receipt supplies a new allocation generation");
  ck(se::ledger.publish(cost.key,.8,1.6,false,hostTickCount64),"actual dispatch has own pending cost key");
  serviceCaller54=reinterpret_cast<void*>(base+0xD4B0D2);{se::ReleaseScope54 scope(&input);
   ck(nhp134::ReactionDispatchDetour23(actor.data(),&held)==accepted,"original dispatch acceptance is unchanged");}
  serviceCaller54=nullptr;ck(nativeResource54==70,"actual dispatch observes costs without duplicate debit");
  ck(se::ledger.paidFor54(cost.key,hostTickCount64)==accepted,"only accepted exact action plus original submissions completes cost proof");
  if(!accepted){se::Record attempted{};ck(se::ledger.reserve(cost.key,hostTickCount64,attempted)==se::Claim::Indeterminate,"rejected dispatch with native submissions cannot cause a second DIRECT debit");}}
 nhp134::g_reaction_dispatch_original50=nullptr;se::actionContractBase54=0;
 svc::raised=nullptr;svc::commit=nullptr;svc::effect=nullptr;nhp134::g_base=oldBase;kcd2vr::frik_pose_probe::target122=oldTarget;
}
int main(){
 // Match DMK's release contract: an armed release retains its patch; a
 // disabled release retains the original/backend without re-arming it.
 for(bool disableFirst:{false,true}){
  unsigned char patch=0x90;const auto address=reinterpret_cast<std::uintptr_t>(&patch);
  const auto original=reinterpret_cast<std::uintptr_t>(&native);
  std::optional<DetourModKit::hook::Hook> owner{DetourModKit::hook::Hook{address,original}};
  ck(owner->enable()&&patch==0xCC,"release fixture owns its patch");
  if(disableFirst)ck(owner->disable()&&patch==0x90,"disabled retained hook is already unpatched");
  const auto before=DetourModKit::hook::hostRetainedHooks115.size();owner->release();owner.reset();
  const auto& retained=DetourModKit::hook::hostRetainedHooks115.back();
  ck(DetourModKit::hook::hostRetainedHooks115.size()==before+1&&retained.address==address&&retained.fn==original,
     "detached handle retains backend and original record");
  ck(retained.live==!disableFirst&&patch==(disableFirst?0x90:0xCC),"release never changes armed or disabled patch state");
 }
 std::vector<unsigned char> image(0x4200000);hostModuleHandleW=image.data();const auto b=reinterpret_cast<std::uintptr_t>(image.data());
 auto*d=reinterpret_cast<IMAGE_DOS_HEADER*>(image.data());d->e_magic=IMAGE_DOS_SIGNATURE;d->e_lfanew=0x100;
 auto*n=reinterpret_cast<IMAGE_NT_HEADERS64*>(image.data()+0x100);n->Signature=IMAGE_NT_SIGNATURE;n->FileHeader.TimeDateStamp=kTimestamp;
 std::memcpy(image.data()+kOnCollisionRva,kEntry,sizeof(kEntry));put(image.data()+0x3B912F0,b+kOnCollisionRva);
 const unsigned char enrichment[]{0x48,0x8B,0xC4,0x48,0x89,0x58,0x10,0x48,0x89,0x70,0x18,0x48,0x89,0x78,0x20,0x48};
 std::memcpy(image.data()+0xD51B38,enrichment,sizeof(enrichment));
 std::memcpy(image.data()+kcd2vr::character_entry63::kEntry,kcd2vr::character_entry63::kPristine,sizeof(kcd2vr::character_entry63::kPristine));
 DetourModKit::hook::hostOriginal115=+[](const DetourModKit::hook::InlineRequest&r)->std::uintptr_t {
  if(r.name=="kcd2vr_v6689_combat_collision")return reinterpret_cast<std::uintptr_t>(&native);
  if(r.name=="kcd2vr_v6689_collision_enrichment")return reinterpret_cast<std::uintptr_t>(&enrich);
  if(r.name=="kcd2vr_v6690_passive_contact_character")return reinterpret_cast<std::uintptr_t>(&characterGetter63);
  return 0;};
 kcd2vr::retail_build112::verified=false;ck(!RegisterContactObserver115(&observe),"unverified SHA cannot install observer");kcd2vr::retail_build112::verified=true;
 image[kOnCollisionRva]^=1;ck(!Install(),"foreign entry fails closed");image[kOnCollisionRva]^=1;
 ck(!NativeCharacterEntryQualified63(b),"no passive character receipt before install");
 ck(Install(),"legacy observer installs first");
 ck(NativeCharacterEntryQualified63(b)&&image[kcd2vr::character_entry63::kEntry]==0xCC,"actual passive observer publishes its own exact post-enable entry");
 image[kcd2vr::character_entry63::kEntry+9]^=1;
 ck(!NativeCharacterEntryQualified63(b),"foreign byte change after owned hook publication refuses");image[kcd2vr::character_entry63::kEntry+9]^=1;
 ck(!NativeCharacterEntryQualified63(b+1),"other module base cannot borrow installed receipt");
 serviceObservationTest(b);
 nativeCostObservation54();
 ck(RegisterContactObserver115(&observe)&&RegisterContactObserver115(&observe),"new observer shares already patched entry, idempotently");
 ck(DetourModKit::hook::hostCollisionInstalls115==1,"one native collision hook owns both observations");
 std::array<unsigned char,0x88> raw{};const auto before=raw;g_enabled=true;kcd2vr::diagnostics::lab::state=1;
 // Since V168 the finite capture gate, not lab state alone, enables recording.
 kcd2vr::diagnostics::capture168::gate.select(kcd2vr::diagnostics::capture168::Domain::Combat,true,hostTickCount64);
 Detour(nullptr,nullptr,nullptr,raw.data());ck(observed==1&&forwarded==1&&g_calls==1&&raw==before,"both diagnostics and one original call survive in F3 reference");
 kcd2vr::diagnostics::capture168::gate.select(kcd2vr::diagnostics::capture168::Domain::Combat,false,hostTickCount64);
 g_enabled=false;Detour(nullptr,nullptr,nullptr,raw.data());ck(observed==2&&forwarded==2&&g_calls==1,"observer and native call survive with finite recorder off, without logging");
 image[kOnCollisionRva+2]^=1;ck(!RegisterContactObserver115(&observe),"foreign modification to our patch is rejected");image[kOnCollisionRva+2]^=1;
 ClearContactObserver115(&observe);Detour(nullptr,nullptr,nullptr,raw.data());ck(observed==2&&forwarded==3,"unregister preserves original native forwarding");
 Uninstall();ck(image[kOnCollisionRva]==kEntry[0],"uninstall restores owned native entry");
 ck(!NativeCharacterEntryQualified63(b)&&image[kcd2vr::character_entry63::kEntry]==kcd2vr::character_entry63::kPristine[0],"passive observer uninstall retires receipt before restoring native entry");
 ck(RegisterContactObserver115(&observe)&&Install(),"reverse install order also shares one owner");
 ck(DetourModKit::hook::hostCollisionInstalls115==2,"one installation per lifetime, never a stacked patch");Uninstall();
 // Same-callback stage proof uses fixture registers; never executes native code.
 std::array<unsigned char,0x60> entity{},collision{},stack{};
 std::array<unsigned char,0x300> actor{},player{};
 put(entity.data(),b+0x4072C78);put(entity.data()+0xC,std::uint32_t(42));
 put(collision.data(),b+0x3B91300);put(collision.data()+0x10,b+0x3B912E8);
 put(collision.data()+8,actor.data());put(actor.data(),b+0x3A80610);put(actor.data()+0x218,collision.data());
 put(actor.data()+0x1F8,player.data());put(player.data(),b+0x3AA4B98);put(player.data()+0x278,actor.data());put(player.data()+0x38,entity.data());
 kcd2vr::frik_pose_probe::target122=entity.data();put(raw.data()+0x24,std::uint32_t(100));
 g_path_ready122=true;kcd2vr::diagnostics::lab::state=1;cp122::select(true,hostTickCount64);
 kcd2vr::diagnostics::capture168::gate.select(kcd2vr::diagnostics::capture168::Domain::Combat,true,hostTickCount64);
 DetourModKit::hook::MidContext ctx{};using DetourModKit::hook::Gpr;using DetourModKit::hook::gpr;
 gpr(ctx,Gpr::Rbx)=reinterpret_cast<std::uintptr_t>(collision.data()+0x10);ctx.stack=reinterpret_cast<std::uintptr_t>(stack.data());
 {ContactPathScope122 scope(collision.data()+0x10,raw.data(),90);ck(scope.active,"exact Henry outgoing callback acquires passive path");
  gpr(ctx,Gpr::Rax)=1;ContactPathMid122(ctx,0);gpr(ctx,Gpr::Rax)=0;
  for(unsigned i=1;i<5;++i)ContactPathMid122(ctx,i);
  const auto saved=ctx;
  {ContactPathScope122 foreign(nullptr,raw.data(),91);ContactPathMid122(ctx,0);ck(!foreign.active,"nested unrelated callback masks outer path");}
  ck(scope.e.flags==31&&scope.e.layer==0,"native stages and candidate-copy are distinguished from damage");
  ck(!std::memcmp(&saved,&ctx,sizeof(ctx)),"diagnostic never edits captured native registers");}
 cp122::Event event{};ck(cp122::pop(event)&&event.sequence==90&&event.flags==31&&event.layer==0,"one causal path record is queued on native return");
 hostTickCount64+=21;
 {ContactPathScope122 scope(collision.data()+0x10,raw.data(),92);gpr(ctx,Gpr::Rax)=0;ContactPathMid122(ctx,0);}
 ck(cp122::pop(event)&&event.flags==1&&event.layer==1,"failed enrichment is recorded as a stop, never candidate acceptance");
 hostTickCount64+=21;kcd2vr::diagnostics::lab::state=0;
 kcd2vr::diagnostics::capture168::gate.select(kcd2vr::diagnostics::capture168::Domain::Combat,false,hostTickCount64);
 {ContactPathScope122 scope(collision.data()+0x10,raw.data(),93);ck(!scope.active,"passive path is off when the explicit Combat recorder is off");}
 hostTickCount64+=21;kcd2vr::frik_pose_probe::f11Mode167=1;kcd2vr::diagnostics::lab::state=2;
 cp122::selectSources(cp122::F11Hands167,hostTickCount64,true);
 {ContactPathScope122 scope(collision.data()+0x10,raw.data(),96);ck(!scope.active,"retired F11 source flag cannot arm passive observation");}
 kcd2vr::diagnostics::capture168::gate.select(kcd2vr::diagnostics::capture168::Domain::Combat,true,hostTickCount64);
 {ContactPathScope122 scope(collision.data()+0x10,raw.data(),96);ck(scope.active&&!scope.force164,"explicit recorder opens passive exact-Henry contact observation without weapon mutation");}
 ck(cp122::pop(event)&&cp122::f11Rung167(event.source)==1,"boxing contact keeps its capture identity");
 hostTickCount64+=180001;
 {ContactPathScope122 scope(collision.data()+0x10,raw.data(),97);ck(!scope.active,"F11 detailed contact window expires without extending itself");}
 kcd2vr::frik_pose_probe::f11Mode167=0;kcd2vr::diagnostics::lab::state=0;
 hostTickCount64+=21;cp122::select(false,hostTickCount64);kcd2vr::frik_pose_probe::mode164=7;
 kcd2vr::diagnostics::capture168::gate.select(kcd2vr::diagnostics::capture168::Domain::Combat,true,hostTickCount64);
 kcd2vr::diagnostics::trace164::until=hostTickCount64+1000;
 kcd2vr::diagnostics::trace164::Context parent{};parent.collisions=reinterpret_cast<std::uintptr_t>(collision.data());parent.prediction=601;parent.sweep=602;parent.sample=99;parent.redirected=true;
 {kcd2vr::diagnostics::trace164::Scope prediction(parent);
  ContactPathScope122 scope(collision.data()+0x10,raw.data(),94);ck(scope.active&&scope.force164,"weapon mode records a contact path under an independent Combat recorder");
  ck(kcd2vr::diagnostics::trace164::current.contact==94,"contact has exact callback identity inside predictor");}
 ck(cp122::pop(event)&&event.parent164[0]==601&&event.parent164[1]==602&&event.mode==1,"same-stack predictor and sweep provenance reaches queued contact");
 hostTickCount64+=21;
 {ContactPathScope122 scope(collision.data()+0x10,raw.data(),95);ck(scope.active,"async contact still records native filters");}
 ck(cp122::pop(event)&&!event.parent164[0]&&!event.parent164[1],"async contact never borrows a recent unrelated tracked sweep");
 kcd2vr::frik_pose_probe::mode164=0;g_path_ready122=false;cp122::select(true,hostTickCount64);kcd2vr::diagnostics::lab::state=1;
 nhp134::g_base=b;
 std::array<unsigned char,0xD20> state{};std::array<unsigned char,0xA0> query{};std::array<unsigned char,0x20> owner{};
 put(actor.data()+0x210,state.data());put(state.data()+0xCE0,actor.data());put(query.data()+8,actor.data());put(owner.data()+8,actor.data());
 put(query.data()+0x4C,2);put(query.data()+0x3C,9);put(query.data()+0x98,2u);
 gpr(ctx,Gpr::Rbx)=reinterpret_cast<std::uintptr_t>(query.data());gpr(ctx,Gpr::Rdi)=reinterpret_cast<std::uintptr_t>(owner.data());
 const auto savedQuery=query;const auto savedContext=ctx;
 CloseQuery164(ctx);
 ck(cp122::pop(event)&&event.kind==cp122::Kind::CloseQuery&&event.id==2&&event.mode==9&&event.key[0]==42,"completed exact Henry close query records input and native criteria");
 ck(query==savedQuery&&!std::memcmp(&savedContext,&ctx,sizeof(ctx)),"close query observer never edits native criteria or registers");
 cp122::select(false,hostTickCount64);
 kcd2vr::diagnostics::capture168::gate.select(kcd2vr::diagnostics::capture168::Domain::Combat,false,hostTickCount64);
 { // An admitted old hit must not survive a brief display-only recovery.
   namespace geo=nhp134::geom27;
   geo::Clear();geo::g_last_submit=0;
   geo::Pending pending{};pending.tick=hostTickCount64;pending.entityId=42;pending.gesture59c3=100;
   ck(geo::Submit(pending)&&geo::g_has,"contact queued before recovery");
   const auto first=geo::g_pending;
   ++pending.gesture59c3;++pending.entityId;
   ck(geo::Submit(pending)&&geo::g_pending.serial==first.serial&&geo::g_pending.entityId==first.entityId&&geo::g_backlog59c6.count==1,
      "newer overlapping contact waits separately without replacing the first gesture or target");
   const auto rejected=geo::g_stats[5].load();
   ++kcd2vr::contact176::recoveryEpoch55c1;
   geo::Drain(nullptr); // must retire before any ctor, actor read or native call
   ck(!geo::g_has&&geo::g_stats[5].load()==rejected+1,"recovery permanently retires queued contact");
   ck(geo::Submit(pending),"a new recovered contact can acquire the released slot");
   hostTickCount64+=161;pending.tick=hostTickCount64;
   ck(geo::Submit(pending)&&geo::g_pending.tick==hostTickCount64,"expired contact is replaced without renewing its old identity");
   geo::g_draining59c3=true;geo::Clear();
   ck(geo::Submit(pending)&&!geo::g_has&&geo::g_backlog59c6.count==1,"in-flight native consumer reserves active slot across clear; new request waits in backlog");
   geo::g_draining59c3=false;geo::Clear();
   pending.gesture59c3=0;ck(!geo::Submit(pending),"passive object pressure cannot acquire a contact gesture");
   std::array<unsigned char,0x20> rpg{};put(rpg.data()+8,actor.data());
   kcd2vr::frik_pose_probe::contactMode59c3=true;geo::g_live178r35=true;
   for(bool integrated:{false,true}){
    kcd2vr::frik_pose_probe::integrated59c3=integrated;
    for(int kind:{0,1}){put(state.data()+nhp134::kAttackType+8,kind);
     ck(geo::NativeSweepRejected178r40(0xC52534,rpg.data(),nullptr),"both F4 rungs deny authored ordinary hit without a contact ticket");}
    put(state.data()+nhp134::kAttackType+8,7);
    ck(!geo::NativeSweepRejected178r40(0xC52534,rpg.data(),nullptr),"paired special remains outside ordinary-hit denial");
   }
   kcd2vr::frik_pose_probe::contactMode59c3=false;kcd2vr::frik_pose_probe::integrated59c3=false;
 }
 std::printf("PASS %d shared native contact hook checks\n",checks);
}
