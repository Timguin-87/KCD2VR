#pragma once
#include <array>
#include <atomic>
#include <cstdint>
namespace kcd2vr::diagnostics::draw_native_d77 {
// Observation only. No native pointers are dereferenced here; every identity is
// created by a qualified invocation/allocation event, never matching payloads.
enum class Kind : unsigned { Invoke,Token,WorldBegin,WorldEnd,Spawn,CommandCreate,
 CommandClone,CommandInvoke,CommandRetire,BindingCreate,BindingStore,BindingRetire,
 Hide,ProxyRegister,ProxyRetire,Expired,Lost,Attachment,Placement,ExistingEntity,ProxyIdentity };
inline const char* name(Kind k) noexcept {static constexpr const char* n[]={"INVOKE","TOKEN","WORLD_BEGIN","WORLD_SCOPE_EXIT","SPAWN","COMMAND_CREATE","COMMAND_CLONE","COMMAND_INVOKE","COMMAND_RETIRE","BINDING_CREATE","BINDING_STORE","BINDING_RETIRE","ENTITY_HIDE_CALL_ENTRY","RENDER_REGISTRATION_CALLBACK_ENTRY","PROXY_RETIRE","EXPIRED","LOST","ATTACHMENT","PLACEMENT","PREEXISTING_ENTITY","PROXY_OCCURRENCE_BOUND"};return n[unsigned(k)];}
struct Draw {std::uint64_t session=0,occurrence=0,key=0,body=0,request=0,sample=0,frame=0,tick=0,generation=0,trackingEpoch=0,sourceTick=0;
 std::uintptr_t manager=0,player=0,character=0,item=0;int side=-1,classId=0;};
struct Event {Draw draw{};Kind kind=Kind::Lost;std::uint64_t tick=0,token=0,generation=0,qpc=0,sequence=0,observedSample=0,observedPublication=0,observedTracking=0;
 std::uintptr_t object=0,other=0,entity=0,caller=0;std::uint32_t entityId=0,rva=0,thread=0;
 int value=0;bool transformValid=false;float transform[12]{},wanted[8]{},natural[8]{};};
struct Scope {std::uint64_t previous=0,current=0;};
struct Receipt {std::uintptr_t ptr=0;std::uint64_t draw=0,generation=0;std::uintptr_t entity=0;std::uint32_t id=0;};
class Recorder {
 struct Slot {Draw d{};std::uintptr_t world=0,entity=0;std::uint64_t token=0;std::uint32_t id=0,events=0,attachmentEvents=0;};
 std::atomic_flag lock_=ATOMIC_FLAG_INIT;std::array<Slot,24> slots_{};
 std::array<Receipt,96> commands_{},bindings_{},proxies_{};std::array<Event,512> queue_{};
 std::size_t read_=0,write_=0,count_=0;std::uint64_t serial_=0,generation_=0,sequence_=0;
 Slot* slot(std::uint64_t id) noexcept {for(auto& s:slots_)if(s.d.occurrence==id&&s.d.session==session.load())return &s;return nullptr;}
 bool enter() noexcept {if(lock_.test_and_set(std::memory_order_acquire)){++lost;return false;}return true;}
 void leave() noexcept {lock_.clear(std::memory_order_release);}
 void emit(Slot& s,Kind kind,std::uint64_t now,std::uintptr_t object=0,std::uintptr_t other=0,std::uint64_t generation=0) noexcept {
  if(s.events++>=96||count_==queue_.size()){++lost;return;}Event e{};e.draw=s.d;e.kind=kind;e.tick=now;e.token=s.token;e.object=object;e.other=other;e.generation=generation;e.entity=s.entity;e.entityId=s.id;e.sequence=++sequence_;if(const auto stamp=clock.load(std::memory_order_acquire)){const auto t=stamp();e.qpc=t.qpc;e.thread=t.thread;}queue_[write_]=e;write_=(write_+1)%queue_.size();++count_;
 }
 bool live(Slot* s,std::uint64_t now) noexcept {return s&&s->d.occurrence&&s->d.session&&s->d.session==session.load()&&now>=s->d.tick&&now-s->d.tick<=1500;}
 Receipt add(std::array<Receipt,96>& table,Slot& s,std::uintptr_t ptr,std::uintptr_t entity,std::uint32_t id,Kind k,std::uint64_t now) noexcept {
  if(!ptr)return {};for(auto& r:table)if(r.ptr==ptr){++lost;r={};}
  for(auto& r:table)if(!r.ptr||!slot(r.draw)){r={ptr,s.d.occurrence,++generation_,entity,id};emit(s,k,now,ptr,0,r.generation);return r;}++lost;return {};
 }
 public:
 struct Stamp {std::uint64_t qpc;std::uint32_t thread;};
 std::atomic<Stamp(*)() noexcept> clock{nullptr};
 std::atomic<std::uint64_t> session{0},lost{0},deadline{0};
 void enable(std::uint64_t id) noexcept {if(id!=session.load())deadline.store(0,std::memory_order_release);session.store(id,std::memory_order_release);}
 bool hasWindow(std::uint64_t now) const noexcept {const auto until=deadline.load(std::memory_order_acquire);return active()&&now&&now<=until&&until-now<=1500;}
 bool active() const noexcept{return session.load(std::memory_order_acquire)!=0;}
 std::uint64_t arm(Draw d) noexcept {if(!d.session||d.session!=session.load()||!d.item||!d.key||d.side<0||d.side>1||!enter())return 0;
  Slot* target=nullptr;for(auto& s:slots_)if(!s.d.occurrence||s.d.session!=d.session||d.tick>=s.d.tick+1500){target=&s;break;}
  if(!target){++lost;leave();return 0;}d.occurrence=++serial_;*target={d};deadline.store(d.tick+1500,std::memory_order_release);emit(*target,Kind::Invoke,d.tick);leave();return d.occurrence;}
 Draw lookup(std::uint64_t id,std::uint64_t now) noexcept {if(!active()||!enter())return {};auto* s=slot(id);Draw d=live(s,now)?s->d:Draw{};leave();return d;}
 void token(std::uint64_t id,std::uintptr_t world,std::uintptr_t item,std::uint64_t token,std::uint64_t now) noexcept {if(!active()||!enter())return;auto*s=slot(id);if(live(s,now)&&s->d.item==item&&world&&token){s->world=world;s->token=token;emit(*s,Kind::Token,now,world);}leave();}
 std::uint64_t completion(std::uintptr_t world,std::uint64_t token,std::uintptr_t item,std::uint64_t now) noexcept {if(!active()||!enter())return 0;Slot* found=nullptr;for(auto& s:slots_)if(live(&s,now)&&s.world==world&&s.token==token&&s.d.item==item){if(found){++lost;leave();return 0;}found=&s;}auto id=found?found->d.occurrence:0;if(found)emit(*found,Kind::WorldBegin,now,world);leave();return id;}
 void mark(std::uint64_t id,Kind k,std::uint64_t now,std::uintptr_t obj=0,std::uintptr_t other=0,std::uint64_t generation=0) noexcept {if(!active()||!enter())return;auto*s=slot(id);if(live(s,now))emit(*s,k,now,obj,other,generation);leave();}
 void entity(std::uint64_t id,std::uintptr_t entity,std::uint32_t eid,std::uint64_t now,bool existing=false) noexcept {if(!active()||!entity||!eid||!enter())return;auto*s=slot(id);if(live(s,now)){if(s->entity&&(s->entity!=entity||s->id!=eid)){++lost;}else{s->entity=entity;s->id=eid;emit(*s,existing?Kind::ExistingEntity:Kind::Spawn,now,entity);}}leave();}
 Draw byEntity(std::uintptr_t entity,std::uint32_t id,std::uint64_t now) noexcept {if(!active()||!enter())return {};Draw out{};for(auto&s:slots_)if(live(&s,now)&&s.entity==entity&&s.id==id){if(out.occurrence){out={};++lost;break;}out=s.d;}leave();return out;}
 Receipt create(std::uint64_t id,std::uintptr_t ptr,std::uint64_t now) noexcept {if(!active()||!enter())return {};auto*s=slot(id);Receipt r{};if(live(s,now))r=add(commands_,*s,ptr,0,0,Kind::CommandCreate,now);leave();return r;}
 Receipt findCommand(std::uintptr_t ptr,std::uint64_t now) noexcept {if(!active()||!enter())return {};Receipt out{};for(auto&r:commands_)if(r.ptr==ptr&&live(slot(r.draw),now)){out=r;break;}leave();return out;}
 void clone(Receipt from,std::uintptr_t ptr,std::uint64_t now) noexcept {if(!active()||!from.ptr||!enter())return;bool valid=false;for(auto&r:commands_)if(r.ptr==from.ptr&&r.generation==from.generation&&r.draw==from.draw)valid=true;auto*s=slot(from.draw);if(valid&&live(s,now))add(commands_,*s,ptr,0,0,Kind::CommandClone,now);else ++lost;leave();}
 void retireCommand(std::uintptr_t ptr,std::uint64_t now) noexcept {if(!active()||!enter())return;for(auto&r:commands_)if(r.ptr==ptr){if(auto*s=slot(r.draw);live(s,now))emit(*s,Kind::CommandRetire,now,ptr,0,r.generation);r={};}leave();}
 void binding(std::uint64_t id,std::uintptr_t ptr,std::uintptr_t entity,std::uint32_t eid,std::uint64_t now) noexcept {if(!active()||!enter())return;auto*s=slot(id);if(live(s,now)&&s->entity==entity&&s->id==eid)add(bindings_,*s,ptr,entity,eid,Kind::BindingCreate,now);leave();}
 void stored(std::uintptr_t binding,std::uintptr_t socket,std::uint64_t now) noexcept {if(!active()||!enter())return;for(auto&r:bindings_)if(r.ptr==binding){if(auto*s=slot(r.draw);live(s,now))emit(*s,Kind::BindingStore,now,binding,socket,r.generation);}leave();}
 void retireBinding(std::uintptr_t ptr,std::uint64_t now) noexcept {if(!active()||!enter())return;for(auto&r:bindings_)if(r.ptr==ptr){if(auto*s=slot(r.draw);live(s,now))emit(*s,Kind::BindingRetire,now,ptr,0,r.generation);r={};}leave();}
 void proxy(std::uint64_t id,std::uintptr_t ptr,std::uintptr_t entity,std::uint32_t eid,std::uint64_t now) noexcept {if(!active()||!enter())return;auto*s=slot(id);if(live(s,now)&&s->entity==entity&&s->id==eid){bool known=false;for(auto&r:proxies_)if(r.ptr==ptr&&r.draw==id)known=true;if(!known)add(proxies_,*s,ptr,entity,eid,Kind::ProxyIdentity,now);}leave();}
 Receipt findProxy(std::uintptr_t ptr,std::uint64_t now) noexcept {if(!active()||!enter())return {};Receipt out{};for(auto&r:proxies_)if(r.ptr==ptr&&live(slot(r.draw),now)){out=r;break;}leave();return out;}
 void retireProxy(std::uintptr_t ptr,std::uint64_t now) noexcept {if(!active()||!enter())return;for(auto&r:proxies_)if(r.ptr==ptr){if(auto*s=slot(r.draw);live(s,now))emit(*s,Kind::ProxyRetire,now,ptr,0,r.generation);r={};}leave();}
 void detail(Event e) noexcept {if(!active()||!enter())return;auto*s=slot(e.draw.occurrence);if(live(s,e.tick)&&s->d.session==e.draw.session){if(s->events++>=96||count_==queue_.size())++lost;else{e.draw=s->d;e.entity=s->entity;e.entityId=s->id;e.token=s->token;e.sequence=++sequence_;if(const auto stamp=clock.load(std::memory_order_acquire)){const auto t=stamp();e.qpc=t.qpc;e.thread=t.thread;}queue_[write_]=e;write_=(write_+1)%queue_.size();++count_;}}leave();}
 void attachment(Event e) noexcept {if(!active()||!enter())return;Slot* found=nullptr;
  for(auto& s:slots_)if(live(&s,e.tick)&&s.d.key==e.draw.key&&s.d.body==e.draw.body&&s.d.request==e.draw.request&&s.d.side==e.draw.side&&s.entity==e.entity&&s.id==e.entityId){if(found){++lost;leave();return;}found=&s;}
  if(found&&found->attachmentEvents++<12&&found->events++<96&&count_<queue_.size()){e.draw=found->d;e.token=found->token;e.sequence=++sequence_;if(const auto stamp=clock.load(std::memory_order_acquire)){const auto t=stamp();e.qpc=t.qpc;e.thread=t.thread;}queue_[write_]=e;write_=(write_+1)%queue_.size();++count_;}else if(found){++lost;}leave();}
 bool pop(Event& e) noexcept {if(!enter())return false;if(!count_){leave();return false;}e=queue_[read_];read_=(read_+1)%queue_.size();--count_;leave();return true;}
};
inline Recorder recorder;
inline thread_local std::uint64_t invoking=0;
inline Scope begin(std::uint64_t id) noexcept {Scope s{invoking,id};invoking=id;return s;}
inline void end(Scope s) noexcept {invoking=s.previous;}
#if defined(_MSC_VER) && defined(_WIN32)
#pragma push_macro("__try")
#pragma push_macro("__finally")
#undef __try
#undef __finally
#endif
// Scope objects are POD so wrappers can restore with MSVC __finally on SEH.
inline bool invokeScoped(std::uint64_t id,bool(*fn)(void*),void* context) {
 const auto s=begin(id);bool result=false;
#if defined(_MSC_VER) && defined(_WIN32)
 __try {result=fn(context);} __finally {end(s);}
#else
 try{result=fn(context);}catch(...){end(s);throw;}end(s);
#endif
 return result;
}
#if defined(_MSC_VER) && defined(_WIN32)
#pragma pop_macro("__finally")
#pragma pop_macro("__try")
#endif
} // namespace
