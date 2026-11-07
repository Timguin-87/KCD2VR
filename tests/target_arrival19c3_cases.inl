// These cases cross the actual Submit/DrainLease, ActorSeen/map and target gate.
// Native calls/damage are not emulated: `ready` counts permission to continue.
{
 using namespace target_fixture19c3;
 std::array<unsigned char,0x100> he{},ne{},oe{},head{},node{},proxy{};
 std::array<unsigned char,0xD20> hs{},ns{},os{};
 regions={{he.data(),he.size()},{ne.data(),ne.size()},{oe.data(),oe.size()},
  {head.data(),head.size()},{node.data(),node.size()},{proxy.data(),proxy.size()},
  {hs.data(),hs.size()},{ns.data(),ns.size()},{os.data(),os.size()}};
 const auto put=[](auto& bytes,std::size_t off,auto value){std::memcpy(bytes.data()+off,&value,sizeof(value));};
 const auto ptr=[](const auto& bytes){return reinterpret_cast<std::uintptr_t>(bytes.data());};
 const auto reset=[&](){
  Clear();g_actors={};ownerValid=true;key=20;generation=1;body=2;request=3;now59c6=2000;
  contact176::recoveryEpoch55c1=20;
  he={};ne={};oe={};head={};node={};proxy={};hs={};ns={};os={};
  put(he,0,g_base+kEntityVft);put(he,0xc,std::uint32_t(1));
  put(ne,0,g_base+kEntityVft);put(ne,0xc,std::uint32_t(50));
  put(oe,0,g_base+kEntityVft);put(oe,0xc,std::uint32_t(51));
  put(ne,0xa8,ptr(head));put(head,8,ptr(node));put(node,0x19,std::uint8_t(0));
  put(node,0x20,std::uint32_t(1));put(node,0x28,ptr(proxy));
  put(proxy,0,g_base+impulse596::kPhysicalProxyVft5916c3);put(proxy,0x30,ptr(ne));put(proxy,0x38,std::uintptr_t(0x9876));
  henry={he.data(),hs.data(),true};npc={ne.data(),ns.data(),true};other={oe.data(),os.data(),true};
  a={};a.gesture59c3=101;a.key=20;a.generation=1;a.epoch=2;a.request=3;a.sample=555;a.tick=2000;a.entityId=50;
  a.metadata59c7g.collider=0x9876;
 };
 unsigned ready=0;
 const auto drain=[&](){DrainBooking59c6([&](Pending& p,auto& lease){
   const void *target=nullptr,*entity=nullptr,*state=nullptr;
   if(AwaitDirectTarget19c3(p,lease,&henry,hs.data(),now59c6,target,entity,state)){
    ck(target==&npc&&entity==ne.data()&&state==ns.data(),"resolved contact preserves exact struck actor/entity/state");++ready;}
  });};
 reset();ready=0;ck(Submit(a),"map arrival: original contact admitted");const auto firstSerial=g_pending.serial;
 drain();ck(g_has&&g_pending.targetWait19c3&&ready==0,"missing map waits without continuation or claim");
 now59c6=2010;drain();ck(g_has&&ready==0&&g_pending.tick==2000&&g_pending.queued==2000&&g_pending.sample==555&&g_pending.serial==firstSerial,
  "retry preserves original impact age, admission time, sample and serial");
 auto dupe=a;dupe.sample++;dupe.tick=2010;ck(!Submit(dupe)&&g_pending.tick==2000,"duplicate cannot refresh unresolved contact");
 now59c6=2014;ActorSeen27(&npc);drain();drain();ck(!g_has&&ready==1,"actor Update arrival delivers original contact once");
 reset();ready=0;ck(Submit(a),"deadline fixture admitted");drain();now59c6=2160;ActorSeen27(&npc);drain();
 ck(ready==1&&!g_has,"existing original 160ms endpoint is unchanged");
 reset();ready=0;ck(Submit(a),"expiry fixture admitted");drain();now59c6=2161;ActorSeen27(&npc);drain();
 ck(ready==0&&!g_has,"actor arriving after original 160ms lease cannot deliver");
 for(unsigned bad=0;bad<16;++bad){
  reset();ready=0;ck(Submit(a),"retained identity negative fixture admitted");drain();now59c6=2014;ActorSeen27(&npc);
  switch(bad){
   case 0:ownerValid=false;break; // tracking/menu/protected context loss from real owner validator
   case 1:++key;break;
   case 2:++generation;break;
   case 3:++body;break;
   case 4:++request;break;
   case 5:++contact176::recoveryEpoch55c1;break;
   case 6:put(ne,0xc,std::uint32_t(51));break; // same cached pointer, mutated entity id
   case 7:npc.coherent=false;break; // native actor/entity/state backlink failure
   case 8:g_actors[0].actor=&henry;break; // cache falsely points at Henry
   case 9:put(proxy,0x38,std::uintptr_t(0x5678));break; // original collider replaced
   case 10:put(proxy,0x30,ptr(oe));break; // another entity owns this proxy
   case 11:put(proxy,0,g_base+0x1234);break;
   case 12:put(ne,0xa8,std::uintptr_t(0));break;
   case 13:put(ne,0,g_base+0x1234);break;
   case 14:g_pending.metadata59c7g.collider=0;break;
   case 15:now59c6=1999;break; // clock rollback, original impact is in the future
  }
  drain();ck(ready==0&&!g_has,"retained target refuses changed ownership, body, identity, context or clock");
 }
 reset();ready=0;ck(Submit(a),"wrong cached actor fixture admitted");
 g_actors[0]={&other,50,2000,2000};put(hs,0xCF8,&npc);drain();
 ck(ready==0&&!g_has,"known wrong map candidate is terminal even with exact opponent available");
 reset();ready=0;ck(Submit(a),"unrelated opponent fixture admitted");put(hs,0xCF8,&other);drain();
 ck(ready==0&&g_has,"unrelated opponent never substitutes for missing struck actor");
 now59c6=2015;ActorSeen27(&npc);drain();ck(ready==1&&!g_has,"exact later actor qualifies despite unrelated opponent");
 reset();ready=0;ck(Submit(a),"exact opponent fallback fixture admitted");put(hs,0xCF8,&npc);drain();
 ck(ready==1&&!g_has,"existing exact-entity opponent fallback remains immediate");
 reset();ready=0;ck(Submit(a),"map refresh fixture admitted");drain();
 // A stale map entry cannot be treated as fresh merely because the retry exists.
 now59c6=2010;g_actors[0]={&npc,50,1490,1490};drain();ck(ready==0&&g_has,"stale actor observation remains unresolved");
 now59c6=2016;ActorSeen27(&npc);drain();ck(ready==1&&!g_has,"only a real fresh actor Update resolves stale map observation");
 reset();ready=0;npc.coherent=false;ActorSeen27(&npc);
 ck(g_actors[0].entityId==0&&g_actors[0].resolved==2000,"failed native actor identity is explicitly negative cached");
 ck(Submit(a),"negative-cache original contact admitted");drain();
 const auto negativeSerial=g_pending.serial;npc.coherent=true;now59c6=2014;const auto beforeRetry=identityReads;
 ActorSeen27(&npc);ck(identityReads==beforeRetry&&g_actors[0].entityId==0,"negative cache is not polled every actor frame");
 drain();ck(g_has&&ready==0,"same unresolved contact waits within its original lease");
 now59c6=2050;ActorSeen27(&npc);
 ck(identityReads==beforeRetry+1&&g_actors[0].entityId==50,"next real Update after 50ms retries failed identity once");
 ck(g_pending.serial==negativeSerial&&g_pending.tick==2000&&g_pending.queued==2000,"identity retry does not refresh contact age");
 drain();drain();ck(ready==1&&!g_has,"negative-cache arrival resumes original contact exactly once");
 now59c6=2100;const auto validReads=identityReads;ActorSeen27(&npc);
 ck(identityReads==validReads,"valid actor identity keeps existing 1000ms resolve interval");
 Clear();regions.clear();
}
