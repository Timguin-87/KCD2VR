// Include after initial_draw_callback_h36r3.inl's pending-map reader. No native
// writes, skipped originals, renderer calls, visibility edits or camera changes.
namespace dn77=kcd2vr::diagnostics::draw_native_d77;
static std::atomic<bool> g_draw_native_ready_d77{false};
static std::uintptr_t g_draw_native_base_d77=0;
static std::array<std::optional<DetourModKit::hook::Hook>,16> g_draw_native_hooks_d77;
static dn77::Recorder::Stamp DrawStampD77() noexcept {LARGE_INTEGER q{};QueryPerformanceCounter(&q);return {std::uint64_t(q.QuadPart),GetCurrentThreadId()};}
static bool DrawNativeActiveD77() noexcept {
 if(!dn77::recorder.active()||!g_draw_native_ready_d77.load(std::memory_order_acquire))return false;
 const auto now=GetTickCount64();using namespace kcd2vr::diagnostics;
 return capture168::active(capture168::Domain::Bug,now)&&dn77::recorder.session.load()==capture168::gate.session[unsigned(capture168::Domain::Bug)].load()&&retail_build112::Ready(WhBase());
}
static bool DrawNativeWindowD77() noexcept {return dn77::recorder.active()&&dn77::recorder.hasWindow(GetTickCount64())&&DrawNativeActiveD77();}
static bool DrawEntityIdD77(std::uintptr_t e,std::uint32_t& id) noexcept {return e&&PhysVft6698(reinterpret_cast<void*>(e),kCEntityVftRva)&&Read(reinterpret_cast<void*>(e+0xC),id)&&id;}
static bool DrawLinkedEntityD77(const dn77::Draw& d,std::uintptr_t entity,std::uint32_t id) noexcept {
 std::uintptr_t link=0,back=0,actual=0;std::uint32_t actualId=0;
 return d.item&&Read(reinterpret_cast<void*>(d.item+0x88),link)&&link&&PhysVft6698(reinterpret_cast<void*>(link),0x3EC85E0)&&Read(reinterpret_cast<void*>(link+0x58),back)&&back==d.item&&Read(reinterpret_cast<void*>(link+0x38),actual)&&actual==entity&&Read(reinterpret_cast<void*>(link+0x30),actualId)&&actualId==id;
}
static void DrawObserveExistingD77(std::uint64_t occurrence) noexcept {
 if(!DrawNativeWindowD77())return;const auto now=GetTickCount64();const auto d=dn77::recorder.lookup(occurrence,now);
 std::uintptr_t link=0,entity=0;std::uint32_t id=0;std::uint64_t key=0;
 if(!d.occurrence||!PhysVft6698(reinterpret_cast<void*>(d.item),0x3A2DA88)||!Read(reinterpret_cast<void*>(d.item+0x30),key)||key!=d.key||!Read(reinterpret_cast<void*>(d.item+0x88),link)||!link||!PhysVft6698(reinterpret_cast<void*>(link),0x3EC85E0)||!Read(reinterpret_cast<void*>(link+0x38),entity)||!DrawEntityIdD77(entity,id)||!DrawLinkedEntityD77(d,entity,id))return;
 dn77::recorder.entity(occurrence,entity,id,now,true);
}
static void DrawTokenSubmittedD77(DetourModKit::hook::MidContext& c) noexcept {
 if(!DrawNativeWindowD77())return;using R=DetourModKit::hook::Gpr;
 dn77::recorder.token(dn77::invoking,DetourModKit::hook::gpr(c,R::Rcx),DetourModKit::hook::gpr(c,R::Rdx),DetourModKit::hook::gpr(c,R::Rbx),GetTickCount64());
}
static dn77::Scope DrawBeforeWorldCompletionD77(void* self) noexcept {
 if(!DrawNativeWindowD77())return dn77::begin(0);
 std::uintptr_t vft=0,world=0,item=0;std::uint64_t token=0;
 if(!self||!Read(static_cast<unsigned char*>(self)-8,vft)||vft!=WhBase()+0x3EBC600||!Read(self,world)||!Read(static_cast<unsigned char*>(self)+8,token)||ReadInitialPendingH36r3(world,token,item)!=InitialPendingH36r3::Present)return dn77::begin(0);
 return dn77::begin(dn77::recorder.completion(world,token,item,GetTickCount64()));
}
static void DrawAfterWorldCompletionD77(dn77::Scope scope) noexcept {
 if(DrawNativeActiveD77())dn77::recorder.mark(scope.current,dn77::Kind::WorldEnd,GetTickCount64());dn77::end(scope);
}
using D77Void=void(__fastcall*)(void*);using D77Alloc=void*(__fastcall*)(void*);
using D77Ctor=void*(__fastcall*)(void*,void*,void*,unsigned char);
static D77Alloc g_d77_alloc=nullptr,g_d77_clone=nullptr;
static D77Void g_d77_invoke=nullptr;static D77Ctor g_d77_ctor=nullptr;
static thread_local std::uintptr_t g_d77_spawn_params=0,g_d77_spawn_draw=0;
#if defined(_MSC_VER)
#pragma push_macro("__try")
#undef __try
#endif
static void DrawInvokeWorldOriginalD77(void* self,void(__fastcall* original)(void*)) {
 const auto oldParams=g_d77_spawn_params,oldDraw=g_d77_spawn_draw;g_d77_spawn_params=0;g_d77_spawn_draw=0;
 const auto scope=DrawBeforeWorldCompletionD77(self);
 __try {original(self);} __finally {g_d77_spawn_params=oldParams;g_d77_spawn_draw=oldDraw;DrawAfterWorldCompletionD77(scope);}
}
static void* __fastcall DrawAllocateD77(void* payload) {
 const auto draw=DrawNativeWindowD77()?dn77::invoking:0;
 void* result=g_d77_alloc(payload);
 if(draw&&DrawNativeActiveD77())dn77::recorder.create(draw,reinterpret_cast<std::uintptr_t>(result),GetTickCount64());return result;
}
static void* __fastcall DrawCloneD77(void* source) {
 const auto from=DrawNativeWindowD77()?dn77::recorder.findCommand(reinterpret_cast<std::uintptr_t>(source),GetTickCount64()):dn77::Receipt{};
 void* result=g_d77_clone(source);
 if(from.ptr&&DrawNativeActiveD77())dn77::recorder.clone(from,reinterpret_cast<std::uintptr_t>(result),GetTickCount64());return result;
}
static void __fastcall DrawCommandInvokeD77(void* payload) {
 const auto receipt=DrawNativeWindowD77()?dn77::recorder.findCommand(reinterpret_cast<std::uintptr_t>(payload)-8,GetTickCount64()):dn77::Receipt{};
 const auto scoped=dn77::begin(receipt.draw?receipt.draw:(DrawNativeWindowD77()?dn77::invoking:0));
 if(scoped.current&&DrawNativeActiveD77())dn77::recorder.mark(scoped.current,dn77::Kind::CommandInvoke,GetTickCount64(),receipt.ptr,reinterpret_cast<std::uintptr_t>(payload),receipt.generation);
 __try {g_d77_invoke(payload);} __finally {dn77::end(scoped);}
}
static void* __fastcall DrawBindingCtorD77(void* storage,void* entity,void* parent,unsigned char flags) {
 const auto draw=DrawNativeWindowD77()?dn77::recorder.lookup(dn77::invoking,GetTickCount64()):dn77::Draw{};
 std::uint32_t id=0;const auto ep=reinterpret_cast<std::uintptr_t>(entity);
 const bool exact=draw.occurrence&&DrawEntityIdD77(ep,id)&&DrawLinkedEntityD77(draw,ep,id);
 // This makes constructor Hide/Show attributable before constructor returns.
 if(exact)dn77::recorder.entity(draw.occurrence,ep,id,GetTickCount64(),true);
 void* result=g_d77_ctor(storage,entity,parent,flags);
 if(exact&&DrawNativeActiveD77()&&result==storage)dn77::recorder.binding(draw.occurrence,reinterpret_cast<std::uintptr_t>(result),ep,id,GetTickCount64());return result;
}
#if defined(_MSC_VER)
#pragma pop_macro("__try")
#endif

static void DrawSpawnRequestD77(DetourModKit::hook::MidContext& c) noexcept {
 if(!DrawNativeWindowD77())return;using R=DetourModKit::hook::Gpr;
 const auto d=dn77::recorder.lookup(dn77::invoking,GetTickCount64());
 if(d.occurrence&&d.item==DetourModKit::hook::gpr(c,R::R14)){g_d77_spawn_params=DetourModKit::hook::gpr(c,R::Rdx);g_d77_spawn_draw=d.occurrence;}
}
static void DrawSpawnPublishD77(DetourModKit::hook::MidContext& c) noexcept {
 if(!DrawNativeWindowD77()||!g_d77_spawn_params||g_d77_spawn_draw!=dn77::invoking)return;using R=DetourModKit::hook::Gpr;
 if(DetourModKit::hook::gpr(c,R::Rsi)!=g_d77_spawn_params)return;
 const auto entity=DetourModKit::hook::gpr(c,R::Rbx);std::uint32_t id=0;
 if(DrawEntityIdD77(entity,id))dn77::recorder.entity(dn77::invoking,entity,id,GetTickCount64());
}
static void DrawSpawnReturnedD77(DetourModKit::hook::MidContext&) noexcept {g_d77_spawn_params=0;g_d77_spawn_draw=0;}
static void DrawCommandRetireD77(DetourModKit::hook::MidContext& c) noexcept {
 if(!DrawNativeWindowD77())return;using R=DetourModKit::hook::Gpr;const auto p=DetourModKit::hook::gpr(c,R::Rcx);std::uintptr_t v=0;
 if(Read(reinterpret_cast<void*>(p),v)&&v==WhBase()+0x3E74FE8)dn77::recorder.retireCommand(p,GetTickCount64());
}
static void DrawBindingRetireD77(DetourModKit::hook::MidContext& c) noexcept {
 if(!DrawNativeWindowD77())return;using R=DetourModKit::hook::Gpr;const auto p=DetourModKit::hook::gpr(c,R::Rcx);
 if(PhysVft6698(reinterpret_cast<void*>(p),0x3A68F68))dn77::recorder.retireBinding(p,GetTickCount64());
}
static void DrawBindingStoredD77(DetourModKit::hook::MidContext& c) noexcept {
 if(!DrawNativeWindowD77())return;using R=DetourModKit::hook::Gpr;
 // At9DFA3A, the actual native store at9DFA36 has completed.
 dn77::recorder.stored(DetourModKit::hook::gpr(c,R::Rbx),DetourModKit::hook::gpr(c,R::Rdi),GetTickCount64());
}
static void DrawVisibilityD77(DetourModKit::hook::MidContext& c) noexcept {
 if(!DrawNativeWindowD77())return;using R=DetourModKit::hook::Gpr;const auto entity=DetourModKit::hook::gpr(c,R::Rcx);std::uint32_t id=0;
 if(!DrawEntityIdD77(entity,id))return;const auto d=dn77::recorder.byEntity(entity,id,GetTickCount64());if(!d.occurrence)return;
 dn77::Event e{};e.draw=d;e.kind=dn77::Kind::Hide;e.tick=GetTickCount64();e.rva=0x87963C;e.value=int(DetourModKit::hook::gpr(c,R::Rdx)&255);e.object=entity;
 (void)Read(reinterpret_cast<void*>(DetourModKit::hook::stack_pointer(c)),e.caller);dn77::recorder.detail(e);
}
static void DrawProxyRegistrationD77(DetourModKit::hook::MidContext& c) noexcept {
 if(!DrawNativeWindowD77())return;using R=DetourModKit::hook::Gpr;const auto payload=DetourModKit::hook::gpr(c,R::Rcx);std::uintptr_t proxy=0,entity=0;unsigned char request=0;std::uint32_t id=0;
 if(!Read(reinterpret_cast<void*>(payload),request)||!Read(reinterpret_cast<void*>(payload+8),proxy)||!PhysVft6698(reinterpret_cast<void*>(proxy),0x3A49FD8)||!Read(reinterpret_cast<void*>(proxy+0x80),entity)||!DrawEntityIdD77(entity,id))return;
 const auto d=dn77::recorder.byEntity(entity,id,GetTickCount64());if(!d.occurrence)return;
 // The native queued callable is invoked only while its cancellation key is
 // alive; identity comes from the exact SpawnEntity occurrence, not proxy alone.
 dn77::recorder.proxy(d.occurrence,proxy,entity,id,GetTickCount64());
 dn77::Event e{};e.draw=d;e.kind=dn77::Kind::ProxyRegister;e.tick=GetTickCount64();e.object=proxy;e.other=payload;e.value=request;e.rva=0x2101D50;
 // Registration entry may unregister/no-op. It is not submission or presentation.
 // Only the root placement observer supplies already-proven copied transforms.
 e.transformValid=false; // No unqualified raw transform offset; root copies proven placement matrices.
 dn77::recorder.detail(e);
}
static void DrawProxyRetireD77(DetourModKit::hook::MidContext& c) noexcept {if(!DrawNativeWindowD77())return;using R=DetourModKit::hook::Gpr;dn77::recorder.retireProxy(DetourModKit::hook::gpr(c,R::Rcx),GetTickCount64());}
#ifndef D77_NATIVE_ADAPTER_TEST
struct DrawPinD77 {std::uint32_t rva;std::array<unsigned char,16> bytes;};
#include "draw_native_pins_d77.inl"
static bool InstallDrawNativeD77() noexcept {
 using namespace DetourModKit::hook;const auto base=WhBase();
 if(!retail_build112::Ready(base)||(g_draw_native_base_d77&&g_draw_native_base_d77!=base))return false;
 g_draw_native_ready_d77.store(false,std::memory_order_release);
 for(auto& h:g_draw_native_hooks_d77)if(h&&h->is_enabled()&&!h->disable())return false;
 {
  for(const auto&p:kDrawPinsD77){unsigned char actual[16]{};if(!Read(reinterpret_cast<void*>(base+p.rva),actual)||std::memcmp(actual,p.bytes.data(),16))return false;}
  g_draw_native_base_d77=base;
 }
 auto mid=[&](unsigned i,const char*name,auto fn){if(g_draw_native_hooks_d77[i])return true;auto h=mid_at(MidRequest{name,Target{DetourModKit::Address{base+kDrawPinsD77[i].rva}},Options{}},fn);if(!h)return false;g_draw_native_hooks_d77[i].emplace(std::move(*h));return true;};
 auto inl=[&](unsigned i,const char*name,auto fn,auto&original){if(g_draw_native_hooks_d77[i])return true;auto h=inline_at(InlineRequest{name,Target{DetourModKit::Address{base+kDrawPinsD77[i].rva}},Options{}},fn);if(!h)return false;original=h->template original<std::remove_reference_t<decltype(original)>>();g_draw_native_hooks_d77[i].emplace(std::move(*h));return original!=nullptr;};
 const bool ok=inl(0,"d77_draw_command_allocate",DrawAllocateD77,g_d77_alloc)&&inl(1,"d77_draw_command_clone",DrawCloneD77,g_d77_clone)&&inl(2,"d77_draw_command_invoke",DrawCommandInvokeD77,g_d77_invoke)&&inl(3,"d77_draw_binding_construct",DrawBindingCtorD77,g_d77_ctor)&&mid(4,"d77_draw_command_retire",DrawCommandRetireD77)&&mid(5,"d77_draw_binding_retire",DrawBindingRetireD77)&&mid(6,"d77_draw_binding_stored",DrawBindingStoredD77)&&mid(7,"d77_draw_entity_visibility",DrawVisibilityD77)&&mid(8,"d77_draw_spawn_request",DrawSpawnRequestD77)&&mid(9,"d77_draw_spawn_publish",DrawSpawnPublishD77)&&mid(10,"d77_draw_spawn_return",DrawSpawnReturnedD77)&&mid(11,"d77_draw_registration_only",DrawProxyRegistrationD77)&&mid(12,"d77_draw_proxy_retire",DrawProxyRetireD77);
 if(!ok)return false;for(unsigned i=0;i<13;++i)if(!g_draw_native_hooks_d77[i]->is_enabled()&&!g_draw_native_hooks_d77[i]->enable())return false;
 dn77::recorder.clock=DrawStampD77;g_draw_native_ready_d77.store(true,std::memory_order_release);return true;
}
static void StopDrawNativeD77() noexcept {g_draw_native_ready_d77.store(false,std::memory_order_release);dn77::recorder.enable(0);for(auto&h:g_draw_native_hooks_d77)if(h&&h->is_enabled())(void)h->disable();}
static void DrawNativeServiceD77() noexcept {
 using namespace kcd2vr::diagnostics;const auto now=GetTickCount64();const bool on=capture168::active(capture168::Domain::Bug,now);const auto session=capture168::gate.session[unsigned(capture168::Domain::Bug)].load();static std::uint64_t attempted=0;
 if(!on){if(dn77::recorder.active())StopDrawNativeD77();}
 else if(dn77::recorder.session.load()!=session&&attempted!=session){attempted=session;if(InstallDrawNativeD77()){dn77::recorder.enable(session);diagnostic_trace::WriteFor(capture168::Domain::Bug,"D77_F2 DRAW_NATIVE_COVERAGE hooks=READY sites=13 windowMs=1500 budgetPerDraw=96 attachmentCopies=12 firstActualRender=UNOBSERVED observerWrites=NONE");}else {StopDrawNativeD77();diagnostic_trace::WriteFor(capture168::Domain::Bug,"D77_F2 DRAW_NATIVE_COVERAGE hooks=REFUSED exactByteOrBackendUnavailable=1 actualRender=UNOBSERVED writes=NONE");}}
 dn77::Event e{};for(unsigned i=0;i<64&&dn77::recorder.pop(e);++i){if(!capture168::active(capture168::Domain::Bug,now)||e.draw.session!=session)continue;
 char line[2048]{};std::snprintf(line,sizeof(line),"D77_F2 DRAW_NATIVE stage=%s session=%llu draw=%llu seq=%llu qpc=%llu ms=%llu thread=%u side=%d key=%016llX class=%d item=%p request=%llu body=%llu generation=%llu sourceTick=%llu sample=%llu publication=%llu tracking=%llu token=%llu entity=%p entityId=%u object=%p other=%p lifetime=%llu rva=%X caller=%p value=%d matrixValid=%d matrix=(%.7g,%.7g,%.7g,%.7g;%.7g,%.7g,%.7g,%.7g;%.7g,%.7g,%.7g,%.7g) observedSample=%llu observedPublication=%llu observedTracking=%llu natural=(%.7g,%.7g,%.7g,%.7g,%.7g,%.7g,%.7g,%.7g) wanted=(%.7g,%.7g,%.7g,%.7g,%.7g,%.7g,%.7g,%.7g) firstActualRender=UNOBSERVED writes=NONE loss=%llu",
 dn77::name(e.kind),(unsigned long long)e.draw.session,(unsigned long long)e.draw.occurrence,(unsigned long long)e.sequence,(unsigned long long)e.qpc,(unsigned long long)e.tick,e.thread,e.draw.side,(unsigned long long)e.draw.key,e.draw.classId,(void*)e.draw.item,(unsigned long long)e.draw.request,(unsigned long long)e.draw.body,(unsigned long long)e.draw.generation,(unsigned long long)e.draw.sourceTick,(unsigned long long)e.draw.sample,(unsigned long long)e.draw.frame,(unsigned long long)e.draw.trackingEpoch,(unsigned long long)e.token,(void*)e.entity,e.entityId,(void*)e.object,(void*)e.other,(unsigned long long)e.generation,e.rva,(void*)e.caller,e.value,e.transformValid,
 double(e.transform[0]),double(e.transform[1]),double(e.transform[2]),double(e.transform[3]),double(e.transform[4]),double(e.transform[5]),double(e.transform[6]),double(e.transform[7]),double(e.transform[8]),double(e.transform[9]),double(e.transform[10]),double(e.transform[11]),(unsigned long long)e.observedSample,(unsigned long long)e.observedPublication,(unsigned long long)e.observedTracking,double(e.natural[0]),double(e.natural[1]),double(e.natural[2]),double(e.natural[3]),double(e.natural[4]),double(e.natural[5]),double(e.natural[6]),double(e.natural[7]),double(e.wanted[0]),double(e.wanted[1]),double(e.wanted[2]),double(e.wanted[3]),double(e.wanted[4]),double(e.wanted[5]),double(e.wanted[6]),double(e.wanted[7]),(unsigned long long)dn77::recorder.lost.load());diagnostic_trace::WriteFor(capture168::Domain::Bug,line);}
}

#endif // D77_NATIVE_ADAPTER_TEST
