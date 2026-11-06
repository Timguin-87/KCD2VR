// D72 additional native-row composition. The original H41 fixture executes
// retail hydration/filter/selection/queue instructions. This adds the exact
// free-family probe and handling-type getter. Engine services stay explicit
// host leaves; this does not claim a live game, CAF playback or native damage.
#define KICK_PRODUCTION_COMPOSED
#include "native_kick_selector_native_test.cpp"
#ifndef _WIN32
#include <signal.h>
#include <ucontext.h>
#include <cstdlib>
#endif

namespace kick_fixture {
static bool ownedD72=false,validD72=true,freeReadyD72=true;
static unsigned selectorCallsD72=0;
static std::array<unsigned char,0x80> handlingD72{};
using TypeFnD72=int(__fastcall*)(void*);
static TypeFnD72 originalTypeD72=nullptr;
static void* __fastcall handling(void*,int){return handlingD72.data();}
static bool __fastcall freeReady(void*){return freeReadyD72;}
static int __fastcall requestedType(void* self){
 ++selectorCallsD72;
 const int original=originalTypeD72(self);
 return ownedD72?(validD72?4:0x7fffffff):original;
}
static void context(Native& n,bool free,int right,int left,int maximum=0){
 n.rclass=right;n.lclass=left;n.ordinaryAllowed=!free;
 put(n.state.data(),0x68,free?23:0); // explicit current native FreeGuard ACK, after original modifier input
 put(n.state.data(),0xCF8,free?nullptr:n.enemy.data());
 const int primary=right==12?5:(right>=1&&right<=7)||right==16?1:-1;
 const int secondary=right==12?4:(right>=1&&right<=7)||right==16?0:-1;
 put(handlingD72.data(),0x34,primary);put(handlingD72.data(),0x38,secondary);
 put(n.state.data(),0x630,n.image+0x3B0A968);put(n.state.data(),0x64D,std::uint8_t(maximum));
 n.vf(0x3B0A968,0x10,n.image+0x1A7DDF0);
}
static int runD72(const char* dir){
 Native n(dir);
 // Preserve the getter's exact 14-byte prologue in a host test trampoline.
 // The copied prologue has no RIP-relative instruction. Its tail uses an
 // indirect absolute jump so it does not clobber the native live RAX.
 std::memcpy(n.image+0x5001000,n.image+0x12EE05C,14);
 const unsigned char jump[]{0xff,0x25,0,0,0,0};
 std::memcpy(n.image+0x500100E,jump,sizeof jump);
 put(n.image+0x5001014,0,reinterpret_cast<std::uintptr_t>(n.image+0x12EE06A));
 originalTypeD72=n.fn<TypeFnD72>(0x5001000);
 n.patch(0x12EE05C,reinterpret_cast<void*>(&requestedType));
 n.patch(0x9D3574,reinterpret_cast<void*>(&handling));
 n.patch(0x12EE0B4,reinterpret_cast<void*>(&freeReady));
 n.patch(0x4A8CE0,reinterpret_cast<void*>(&Native::yes));
 n.patch(0x9D162C,reinterpret_cast<void*>(&Native::no));
 n.vf(0x3AA4B98,0x758,reinterpret_cast<void*>(&Native::no));
 put(n.player.data(),0,n.image+0x3AA4B98);
 put(n.image,0x5320D18,23); // exact native FreeGuard enum used by 12EDF78
 auto probe=n.fn<bool(__fastcall*)(void*,int)>(0x12EDF78);
 auto type=originalTypeD72;
 context(n,true,12,12,0);
 check(type(n.factory.data())==5,"exact retail primary unarmed handling is PUNCH5");
 context(n,true,12,12,1);
 check(type(n.factory.data())==4,"exact retail secondary unarmed handling is KICK4");
 context(n,true,4,4,1);
 check(type(n.factory.data())==0,"exact retail secondary armed handling is STAB0 without owned R3 policy");
 ownedD72=false;
 check(!probe(n.factory.data(),2),"new SPECIAL free-kick row cannot be selected by original armed STAB0 query");
 ownedD72=true;
 check(probe(n.factory.data(),2),"owned R3 KICK4 query passes untouched native free equipment filters");
 validD72=false;
 check(!probe(n.factory.data(),2),"revoked R3 request has no eligible native row instead of falling back to a weapon attack");
 validD72=true;
 // D84: MAX changes in the block-press call before the current native guard
 // type necessarily becomes FreeGuard. The old fixture pre-filled that ACK,
 // so it could not expose production's too-early prepare/release ordering.
 for(int guard:{0,1}){
  context(n,true,4,4,1);put(n.state.data(),0x68,guard);ownedD72=true;n.runHandler(2);
  check(!n.slots[0],"MAX1 without current FreeGuard cannot select an authored free kick");
 }
 context(n,true,4,4,1);ownedD72=true;n.runHandler(2);
 check(n.selectedIndex<nativeRowCountFixture&&rowMetaFixture[n.selectedIndex].atk==4,
  "same native request selects its kick after current FreeGuard acknowledgement");
 struct Gear {int right,left;const char* name;};
 const Gear gears[]{{1,12,"sword"},{2,11,"sabre torch"},{3,8,"axe shield"},{4,4,"longsword"},
  {5,12,"mace"},{6,12,"flail class"},{7,7,"polearm"},{16,12,"hunting sword"},
  {12,0,"left dagger"},{12,8,"shield only"},{12,17,"broken shield"},{3,17,"axe broken shield"},
  {12,9,"bow"},{12,10,"crossbow light"},{12,14,"crossbow medium"},{12,15,"crossbow heavy"},
  {13,13,"rifle"},{10,10,"two-hand crossbow property"}};
 for(const auto& gear:gears){
  context(n,true,gear.right,gear.left,0);ownedD72=true;
  const bool eligible=probe(n.factory.data(),2);
  check(eligible,gear.name);
  for(const auto& e:n.eligible){
   const auto index=unsigned((static_cast<unsigned char*>(e.row)-n.rows[0].data.data())/sizeof(Row));
   check(index<nativeRowCountFixture&&rowMetaFixture[index].atk==4&&rowMetaFixture[index].inp==2,
    "native free probe contains only genuine SPECIAL KICK rows");
  }
  n.runHandler(2);
  check(n.selectedIndex<nativeRowCountFixture&&rowMetaFixture[n.selectedIndex].atk==4&&
   rowMetaFixture[n.selectedIndex].inp==2,"exact native free handler selects a KICK carrier and reaches original action-manager admission");
 }
 const Gear ranged[]{{12,9,"bow"},{12,10,"crossbow light"},{12,14,"crossbow medium"},{12,15,"crossbow heavy"},{13,13,"rifle"}};
 for(const auto& gear:ranged){
  context(n,true,gear.right,gear.left,0);ownedD72=false;
  check(!probe(n.factory.data(),1),"ordinary ranged HEAVY fire has no new FreeAttack kick capability");
  context(n,false,gear.right,gear.left,0);ownedD72=true;
  n.runHandler(2);
  check(n.selectedIndex<nativeRowCountFixture&&rowMetaFixture[n.selectedIndex].atk==4&&
   rowMetaFixture[n.selectedIndex].inp==2,"ranged combat SPECIAL selects authored native foot-kick row");
  n.nativeRpgAllowed=false;n.runHandler(2);
  check(!n.slots[0]&&n.rpgCalls,"native RPG refusal remains effective for ranged kick");n.nativeRpgAllowed=true;
  n.nativeQueueAllowed=false;n.runHandler(2);
  check(!n.slots[0]&&n.queueCalls,"native action-manager refusal remains effective for ranged kick");n.nativeQueueAllowed=true;
 }
 context(n,true,4,4,0);ownedD72=true;freeReadyD72=false;n.runHandler(2);
 check(!n.slots[0],"original free-context readiness refusal prevents a kick carrier");freeReadyD72=true;
 n.noRows=true;n.runHandler(2);check(!n.slots[0],"missing companion row never admits a kick");n.noRows=false;
 context(n,true,99,99,0);n.runHandler(2);check(!n.slots[0],"unsupported native equipment remains refused");
 std::printf("D72_NATIVE_KICKS %u/%u PASS; native selector instructions with explicit engine leaves, not live gameplay\n",checks-failures,checks);
 return failures?1:0;
}
}

#ifndef _WIN32
static void fault(int, siginfo_t*,void* raw){
 auto* c=static_cast<ucontext_t*>(raw);
 std::fprintf(stderr,"D72 native fixture fault RVA=%llx RAX=%llx RCX=%llx RDX=%llx\n",
  static_cast<unsigned long long>(c->uc_mcontext.gregs[REG_RIP]-reinterpret_cast<std::uintptr_t>(kick_fixture::active->image)),
  static_cast<unsigned long long>(c->uc_mcontext.gregs[REG_RAX]),static_cast<unsigned long long>(c->uc_mcontext.gregs[REG_RCX]),
  static_cast<unsigned long long>(c->uc_mcontext.gregs[REG_RDX]));std::_Exit(99);
}
#endif
int main(int argc,char** argv){
#ifndef _WIN32
 struct sigaction sa{};sa.sa_sigaction=&fault;sa.sa_flags=SA_SIGINFO;sigaction(SIGSEGV,&sa,nullptr);
#endif
 return kick_fixture::runD72(argc>1?argv[1]:".");
}
