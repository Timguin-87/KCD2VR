// Actual outer world-create completion: std::_Func_impl_no_alloc<lambda93d548,
// void()>, vft3EBC600+10 ->1F10AF0(add RCX,8) ->1F0D230. One this
// argument, VOID return. The original tailcalls the original inner draw callback.
using InitialDrawCallbackH36r3=void(__fastcall*)(void*);
static InitialDrawCallbackH36r3 g_initial_draw_original_h36r3=nullptr;
static std::optional<DetourModKit::hook::Hook> g_initial_draw_hook_h36r3,g_initial_draw_mid_hook_h36r3;
static std::uintptr_t g_initial_draw_base_h36r3=0;
static bool InitialWorldManagerH36r3(std::uintptr_t expected) noexcept {
 void* env=nullptr;void* world=nullptr;void* manager=nullptr;
 return expected&&retail_build112::Ready(WhBase())&&Read(reinterpret_cast<void*>(WhBase()+0x549D388),env)&&env&&
  Read(static_cast<unsigned char*>(env)+0xE0,world)&&world&&Read(static_cast<unsigned char*>(world)+0xC8,manager)&&reinterpret_cast<std::uintptr_t>(manager)==expected;
}
static bool InitialDrawContextH36r3(const kcd2vr::initial_draw_h36r3::Watch&w) noexcept {
 void* player=nullptr;void* manager=nullptr;std::uint64_t key=0,held=0;
 const auto membership=g_left_native_membership_h36.load(std::memory_order_acquire);
 return w.serial&&w.membership!=UINT64_MAX&&(membership==w.membership||membership==w.membership+1)&&
  w.body==g_pose_epoch6689.load(std::memory_order_acquire)&&w.request==g_weapon_request_epoch6694.load(std::memory_order_acquire)&&
  w.character==g_player_char37.load(std::memory_order_acquire)&&w.manager==reinterpret_cast<std::uintptr_t>(g_henry_item_attach_manager.load(std::memory_order_acquire))&&
  PhysVft6698(reinterpret_cast<void*>(w.manager),0x3EC9958)&&PhysVft6698(reinterpret_cast<void*>(w.player),0x3AA4B98)&&
  Read(reinterpret_cast<void*>(w.manager+0x38),player)&&reinterpret_cast<std::uintptr_t>(player)==w.player&&
  Read(reinterpret_cast<void*>(w.player+0x9D0),manager)&&reinterpret_cast<std::uintptr_t>(manager)==w.manager&&
  NativeHandKey6697(manager,1,held)&&held==w.key&&PhysVft6698(reinterpret_cast<void*>(w.item),0x3A2DA88)&&
  Read(reinterpret_cast<void*>(w.item+0x30),key)&&key==w.key;
}
// Existing original DRAW scope, no fresh source. Registers are read through
// DMK's supported API only; no register, flags, IP or stack write is performed.
static void InitialDrawSubmittedMidH36r3(DetourModKit::hook::MidContext&c) noexcept {
 DrawTokenSubmittedD77(c); // F2 both-hand observation; no F9 dependency
 namespace initial=kcd2vr::initial_draw_h36r3;using namespace DetourModKit::hook;
 if(!initial::callbackReady.load(std::memory_order_acquire))return;
 const auto original=initial::invoking;
 const auto world=gpr(c,Gpr::Rcx),item=gpr(c,Gpr::Rdx),token=gpr(c,Gpr::Rbx);
 if(!token||item!=original.item||!InitialWorldManagerH36r3(world)||!InitialDrawContextH36r3(original))return;
 initial::submitted(original,world,token); // actual allocated token, never counter+1 inference
}
// Exact map layout from8D098C/1F0D230. A bounded read is either valid absence,
// exact token->original item, or UNKNOWN; null/fault is never absence.
enum class InitialPendingH36r3 {Unknown,Absent,Present};
static InitialPendingH36r3 ReadInitialPendingH36r3(std::uintptr_t world,std::uint64_t token,std::uintptr_t&item) noexcept {
 item=0;void* head=nullptr;void* node=nullptr;unsigned char nil=0;
 if(!InitialWorldManagerH36r3(world)||!Read(reinterpret_cast<void*>(world+0x5B9030),head)||!head||
    !Read(static_cast<unsigned char*>(head)+0x19,nil)||nil!=1||!Read(static_cast<unsigned char*>(head)+8,node)||!node)return InitialPendingH36r3::Unknown;
 const auto root=node;
 for(unsigned n=0;n<64;++n){
  if(node==head){void* after=nullptr;void* rootAfter=nullptr;
   return Read(reinterpret_cast<void*>(world+0x5B9030),after)&&after==head&&Read(static_cast<unsigned char*>(head)+8,rootAfter)&&rootAfter==root?InitialPendingH36r3::Absent:InitialPendingH36r3::Unknown;}
  std::uint64_t key=0;void* value=nullptr;
  if(!Read(static_cast<unsigned char*>(node)+0x19,nil)||nil||!Read(static_cast<unsigned char*>(node)+0x20,key))return InitialPendingH36r3::Unknown;
  if(key==token){void* after=nullptr;std::uint64_t keyAfter=0;void* valueAfter=nullptr;
   if(!Read(static_cast<unsigned char*>(node)+0x28,value)||!value||!Read(reinterpret_cast<void*>(world+0x5B9030),after)||after!=head||
      !Read(static_cast<unsigned char*>(node)+0x20,keyAfter)||keyAfter!=key||!Read(static_cast<unsigned char*>(node)+0x28,valueAfter)||valueAfter!=value)return InitialPendingH36r3::Unknown;
   item=reinterpret_cast<std::uintptr_t>(value);return InitialPendingH36r3::Present;}
  void* next=nullptr;const auto offset=static_cast<std::int64_t>(token)<static_cast<std::int64_t>(key)?0:0x10;
  if(!Read(static_cast<unsigned char*>(node)+offset,next)||!next||next==node)return InitialPendingH36r3::Unknown;node=next;
 }
 return InitialPendingH36r3::Unknown;
}
struct InitialDrawCallH36r3 {kcd2vr::initial_draw_h36r3::Submitted submitted{};bool valid=false;};
static InitialDrawCallH36r3 CaptureInitialDrawCallH36r3(void*self) noexcept {
 namespace initial=kcd2vr::initial_draw_h36r3;InitialDrawCallH36r3 out{};
 if(!initial::callbackReady.load(std::memory_order_acquire)||!self||!retail_build112::Ready(WhBase())||g_initial_draw_base_h36r3!=WhBase())return out;
 std::uintptr_t vft=0,world=0;std::uint64_t token=0;
 if(!Read(static_cast<unsigned char*>(self)-8,vft)||vft!=WhBase()+0x3EBC600||!Read(self,world)||
    !Read(static_cast<unsigned char*>(self)+8,token)||!token)return out;
 std::uintptr_t item=0;if(ReadInitialPendingH36r3(world,token,item)!=InitialPendingH36r3::Present)return out;
 bool found=false;
 for(auto&s:initial::slots){const auto serial=s.active.load(std::memory_order_acquire);initial::Submitted submitted{};void*link=nullptr;
  if(!serial||serial==UINT64_MAX||s.submission.load(std::memory_order_acquire)!=2||!s.submitted.read(submitted)||
     submitted.original.serial!=serial||submitted.world!=world||submitted.token!=token||submitted.original.item!=item||
     !InitialDrawContextH36r3(submitted.original)||!Read(reinterpret_cast<void*>(item+0x88),link)||link||s.active.load(std::memory_order_acquire)!=serial)continue;
  if(found)return {};found=true;out={submitted,true};}
 return out;
}
static void __fastcall InitialDrawCallbackDetourH36r3(void*self) {
 InvalidateReadCache101R2();const auto original=CaptureInitialDrawCallH36r3(self);
 // Preserve the complete actual original ABI. No self borrowed beyond its call.
 if(g_initial_draw_original_h36r3)DrawInvokeWorldOriginalD77(self,g_initial_draw_original_h36r3);else return;
 InvalidateReadCache101R2();if(!original.valid)return;
 namespace initial=kcd2vr::initial_draw_h36r3;const auto&sub=original.submitted;std::uintptr_t pending=0;void*link=nullptr;void*back=nullptr;void*entity=nullptr;std::uint32_t id=0,entityId=0;
 if(!InitialDrawContextH36r3(sub.original)||ReadInitialPendingH36r3(sub.world,sub.token,pending)!=InitialPendingH36r3::Absent||
    !Read(reinterpret_cast<void*>(sub.original.item+0x88),link)||!PhysVft6698(link,0x3EC85E0)||
    !Read(static_cast<unsigned char*>(link)+0x58,back)||reinterpret_cast<std::uintptr_t>(back)!=sub.original.item||
    !Read(static_cast<unsigned char*>(link)+0x30,id)||!id||!Read(static_cast<unsigned char*>(link)+0x38,entity)||
    !PhysVft6698(entity,kCEntityVftRva)||!Read(static_cast<unsigned char*>(entity)+0x0C,entityId)||entityId!=id)return;
 // Original job RETURNED. The real same-item attachment is separately checked
 // later; a pending-map erasure or born world link alone grants nothing. The
 // metadata writer lease prevents an old callback from overwriting slot reuse.
 (void)initial::publishReturned(sub,{sub.original,reinterpret_cast<std::uintptr_t>(link),reinterpret_cast<std::uintptr_t>(entity),id,sub.world,sub.token});
}
static void StopInitialDrawCallbackH36r3() noexcept {
 kcd2vr::initial_draw_h36r3::callbackReady.store(false,std::memory_order_release);
 if(g_initial_draw_mid_hook_h36r3&&g_initial_draw_mid_hook_h36r3->is_enabled())(void)g_initial_draw_mid_hook_h36r3->disable();
 if(g_initial_draw_hook_h36r3&&g_initial_draw_hook_h36r3->is_enabled())(void)g_initial_draw_hook_h36r3->disable();
 // Both hook backends/trampoline survive process lifetime, including uncertain disable.
}
static bool InstallInitialDrawCallbackH36r3(std::uintptr_t base) noexcept {
 namespace initial=kcd2vr::initial_draw_h36r3;using namespace DetourModKit::hook;initial::callbackReady.store(false,std::memory_order_release);
 if(!base||!retail_build112::Ready(base)||(g_initial_draw_base_h36r3&&base!=g_initial_draw_base_h36r3))return false;
 if(!g_initial_draw_hook_h36r3){
  constexpr unsigned char entry[]{0x40,0x57,0x48,0x83,0xEC,0x20,0x4C,0x8B,0x19,0x48,0x8B,0xF9,0x4D,0x8B,0x93,0x30,0x90,0x5B,0x00};
  constexpr unsigned char thunk[]{0x48,0x83,0xC1,0x08,0xE9,0x37,0xC7,0xFF,0xFF};
  unsigned char actual[sizeof(entry)]{},tail[sizeof(thunk)]{};std::uintptr_t target=0;
  if(!Read(reinterpret_cast<void*>(base+0x1F0D230),actual)||std::memcmp(actual,entry,sizeof(entry))||
     !Read(reinterpret_cast<void*>(base+0x1F10AF0),tail)||std::memcmp(tail,thunk,sizeof(thunk))||
     !Read(reinterpret_cast<void*>(base+0x3EBC600+0x10),target)||target!=base+0x1F10AF0)return false;
  auto hook=inline_at(InlineRequest{"kcd2vr_h36r3_original_left_draw_completion",Target{DetourModKit::Address{base+0x1F0D230}},Options{}},&InitialDrawCallbackDetourH36r3);if(!hook)return false;
  g_initial_draw_original_h36r3=hook->original<InitialDrawCallbackH36r3>();g_initial_draw_base_h36r3=base;g_initial_draw_hook_h36r3.emplace(std::move(*hook));
 }
 if(!g_initial_draw_original_h36r3){StopInitialDrawCallbackH36r3();return false;}
 if(!g_initial_draw_mid_hook_h36r3){
  constexpr unsigned char before[]{0x48,0xFF,0x81,0x40,0x90,0x5B,0x00,0x49,0x8B,0xF8,0x48,0x8B,0x99,0x40,0x90,0x5B,0x00};
  constexpr unsigned char at[]{0x4C,0x8D,0x45,0x9F,0x4C,0x8B,0xF1,0x48,0x89,0x55,0xA7,0x48,0x81,0xC1,0x30,0x90,0x5B,0x00,0x48,0x89,0x5D,0x9F};
  unsigned char a[sizeof(before)]{},b[sizeof(at)]{};
  if(!Read(reinterpret_cast<void*>(base+0x8D08F0),a)||std::memcmp(a,before,sizeof(before))||
     !Read(reinterpret_cast<void*>(base+0x8D0901),b)||std::memcmp(b,at,sizeof(at))){StopInitialDrawCallbackH36r3();return false;}
  auto hook=mid_at(MidRequest{"kcd2vr_h36r3_original_draw_token",Target{DetourModKit::Address{base+0x8D0901}},Options{}},&InitialDrawSubmittedMidH36r3);
  if(!hook){StopInitialDrawCallbackH36r3();return false;}g_initial_draw_mid_hook_h36r3.emplace(std::move(*hook));
 }
 if((!g_initial_draw_hook_h36r3->is_enabled()&&!g_initial_draw_hook_h36r3->enable())||
    (!g_initial_draw_mid_hook_h36r3->is_enabled()&&!g_initial_draw_mid_hook_h36r3->enable())||
    !g_initial_draw_hook_h36r3->is_enabled()||!g_initial_draw_mid_hook_h36r3->is_enabled()){StopInitialDrawCallbackH36r3();return false;}
 initial::callbackReady.store(true,std::memory_order_release);return true;
}
