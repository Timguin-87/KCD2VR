// Included in the complete production action-owner fixture. Native animation
// propagation is the same explicitly doubled, separately retail-pinned leaf.
{
 namespace stage=kcd2vr::alchemy_stage19c4;namespace act=kcd2vr::activity_input163;namespace ws=kcd2vr::workstation178r20c1;
 static std::array<unsigned char,0x700> alchemy19{},resource19{};
 static std::array<std::uintptr_t,2> alchemySlots19{};
 auto pin=[&](unsigned at,std::initializer_list<unsigned char> bytes){std::copy(bytes.begin(),bytes.end(),image.begin()+at);};
 pin(0x6C6D33,{0xE8,0x70,0xC0,0xFB,0xFF,0x48,0x8D,0x05,0x09,0x0C,0x38,0x03,0x48,0x89,0x9F,0x68,0x02,0x00,0x00});
 pin(0x682DD3,{0xB8,0x00,0x00,0x80,0x3F,0x89,0x43,0x5C,0x89,0x43,0x60});
 pin(0x7012EF,{0x48,0x8D,0x7B,0x78});
 pin(0x4B3A76,{0xF3,0x0F,0x10,0x7D,0x5C,0xF3,0x0F,0x10,0x43,0x58,0x0F,0x2E,0xC7});
 pin(0x4B3F12,{0x0F,0x28,0xCF,0xF3,0x0F,0x59,0x4B,0x5C,0x48,0x8B,0xCB,0xFF,0x50,0x30});
 stage::Control input19{};stage::Proof proof19{};
 auto prepare19=[&]{reset();(void)RestoreSpeed13();ResetOwner();g_alchemy_stage19c4={};g_alchemy_retired19c4={};g_alchemy_pending19c4=false;g_alchemy_stop19c4=false;
  alchemy19.fill(0);resource19.fill(0);const auto b=g_base.load();const auto a=reinterpret_cast<std::uintptr_t>(alchemy19.data());
  put(player.data(),0x38,std::uintptr_t(123));put(alchemy19.data(),0,b+0x3F60B08);put(alchemy19.data(),0x18,reinterpret_cast<std::uintptr_t>(player.data()));put(alchemy19.data(),0x2C,std::uint8_t(1));
  put(alchemy19.data(),0x78,b+0x3BFA5B0);alchemySlots19[1]=reinterpret_cast<std::uintptr_t>(resource19.data());
  put(alchemy19.data(),0xB0,reinterpret_cast<std::uintptr_t>(alchemySlots19.data()));put(alchemy19.data(),0xB8,reinterpret_cast<std::uintptr_t>(alchemySlots19.data()+2));
  put(alchemy19.data(),0x304,unsigned(14));put(alchemy19.data(),0x150+4*14,unsigned(72));put(alchemy19.data(),0x2A0,unsigned(21));put(alchemy19.data(),0x2A4,unsigned(21));
  put(resource19.data(),0,b+0x3A5C670);put(resource19.data(),0x58,a);put(resource19.data(),0x40,a+0x78);put(resource19.data(),0x28,std::uint8_t(1));put(resource19.data(),0x3C,int(1));put(resource19.data(),0x78,reinterpret_cast<std::uintptr_t>(action.data()));
  put(action.data(),0,b+0x3A47948);put(action.data(),0x268,a);put(action.data(),kActContext,static_cast<void*>(actionContext.data()));
  put(attachment.data(),0x10,static_cast<const char*>("entity_left"));put(attachment.data(),0x20,reinterpret_cast<std::uintptr_t>(attachmentObject.data()));
  act::witness175.Clear();act::witness175.ready=true;act::witness175.Begin(act::Kind175::Alchemy,a);act::witness175.transitionDepth175=0;
  kcd2vr::combat_rules178r10::f5Rung=2;ws::setExperiment36(true);
  proof19={};proof19.child=reinterpret_cast<std::uintptr_t>(action.data());proof19.token=55;proof19.request=ws::experimentEpoch36.load();proof19.epoch=9;proof19.sample=1000;proof19.tick=1000;
  proof19.binding={a,reinterpret_cast<std::uintptr_t>(player.data()),reinterpret_cast<std::uintptr_t>(character.data()),reinterpret_cast<std::uintptr_t>(attachment.data()),72,1,14,
      reinterpret_cast<std::uintptr_t>(resource19.data()),b+0x3A5C670,reinterpret_cast<std::uintptr_t>(attachmentObject.data()),act::witness175.lifetime175.load()};
  input19={};input19.instance=a;input19.lifetime=proof19.binding.lifetime;input19.request=proof19.request;input19.enabled=true;input19.held[1]=true;input19.tick[1]=1000;
  stage::attached.publish(proof19);stage::control.publish(input19);g_update_original=UpdateActionLeaf;updateOriginalCalls=0;
 };
 prepare19();const auto originalSpeed19=Rd<float>(action.data(),kActSpeedBias,-1);
 UpdateDetour(controller.data(),.02f);assert(updateOriginalCalls==1&&Rd<float>(action.data(),kActSpeedBias,-1)==0&&Rd<float>(scope.data(),kScopeSpeedBias,-1)==0&&g_alchemy_pending19c4);
 for(unsigned n=1;n<20;++n){hostTickCount64=1000+20*n;input19.tick[1]=hostTickCount64;proof19.tick=hostTickCount64;proof19.sample=hostTickCount64;stage::control.publish(input19);stage::attached.publish(proof19);UpdateDetour(controller.data(),.02f);}
 assert(Rd<float>(action.data(),kActActiveTime,-1)==0); // native consumer actually stopped
 input19.held[1]=false;stage::control.publish(input19);UpdateDetour(controller.data(),.02f);
 assert(!g_alchemy_pending19c4&&Rd<float>(action.data(),kActSpeedBias,-1)==originalSpeed19&&Rd<float>(action.data(),kActActiveTime,-1)>.019f);
 input19.held[1]=true;stage::control.publish(input19);UpdateDetour(controller.data(),.02f);
 assert(Rd<float>(action.data(),kActSpeedBias,-1)==originalSpeed19); // same action already resumed/pouring: never freeze again
 // Native FullBody can expose this child in ROOT and non-root slots. The
 // later non-root match must not be mistaken for retirement on release.
 prepare19();
 put(controller.data(),kCtrlScopeCount,unsigned(2));
 put(scope.data()+kScopeStride,kScopeAction,static_cast<void*>(action.data()));
 UpdateDetour(controller.data(),.02f);assert(g_alchemy_pending19c4&&Rd<float>(action.data(),kActSpeedBias,-1)==0.f);
 input19.held[1]=false;stage::control.publish(input19);UpdateDetour(controller.data(),.02f);
 assert(!g_alchemy_pending19c4&&Rd<float>(action.data(),kActSpeedBias,-1)==1.f&&Rd<float>(action.data(),kActActiveTime,-1)>.019f);
 // Replaced ROOT is still a refusal, even if a non-root slot keeps old pointer.
 prepare19();UpdateDetour(controller.data(),.02f);put(controller.data(),kCtrlScopeCount,unsigned(2));
 put(scope.data()+kScopeStride,kScopeAction,static_cast<void*>(action.data()));
 put(scope.data(),kScopeAction,static_cast<void*>(nullptr));input19.held[1]=false;stage::control.publish(input19);
 UpdateDetour(controller.data(),.02f);assert(!g_alchemy_pending19c4&&Rd<float>(action.data(),kActSpeedBias,-1)==0.f);
 for(unsigned fault=0;fault<16;++fault){prepare19();
  if(fault==0)input19.tick[1]=919;if(fault==1)proof19.tick=919;if(fault==2)proof19.tick=1001;
  if(fault==3)proof19.token++;if(fault==4)put(action.data(),0x268,std::uintptr_t(1));if(fault==5)put(resource19.data(),0x78,std::uintptr_t(0));
  if(fault==6)put(action.data(),0,g_base.load()+0x3A47949);if(fault==7)put(action.data(),kActStatus,int(3));if(fault==8)put(action.data(),kActRootScope,static_cast<void*>(nullptr));
  if(fault==9)put(action.data(),kActContext,static_cast<void*>(nullptr));if(fault==10)act::witness175.lifetime175.fetch_add(1);
  if(fault==11)put(resource19.data(),0,g_base.load()+0x3F60770);if(fault==12)input19.enabled=false;if(fault==13)input19.held[1]=false;
  if(fault==14)put(attachment.data(),0x20,std::uintptr_t(0));if(fault==15)act::witness175.transitionDepth175=1;
  stage::control.publish(input19);stage::attached.publish(proof19);UpdateDetour(controller.data(),.02f);
  assert(!g_alchemy_pending19c4&&Rd<float>(action.data(),kActSpeedBias,-1)==1.f);
 }
 prepare19();UpdateDetour(controller.data(),.02f);assert(g_alchemy_pending19c4);
 g_owner_thread13=99;assert(!StopAlchemyStage19c4()&&g_alchemy_pending19c4); // worker cannot discard cleanup
 g_owner_thread13=1;UpdateDetour(controller.data(),.02f);assert(!g_alchemy_pending19c4&&Rd<float>(action.data(),kActSpeedBias,-1)==1.f);
 prepare19();stopAtAlchemyClaim19c4=true;UpdateDetour(controller.data(),.02f);stopAtAlchemyClaim19c4=false;
 assert(!g_alchemy_pending19c4&&Rd<float>(action.data(),kActSpeedBias,-1)==1.f); // Stop between claim and write cannot strand zero speed
 prepare19();UpdateDetour(controller.data(),.02f);put(action.data(),kActSpeedBias,.75f);input19.held[1]=false;stage::control.publish(input19);
 UpdateDetour(controller.data(),.02f);assert(!g_alchemy_pending19c4&&Rd<float>(action.data(),kActSpeedBias,-1)==.75f); // foreign writer preserved
 prepare19();UpdateDetour(controller.data(),.02f);refusedWrite19=action.data()+kActSpeedBias;input19.held[1]=false;stage::control.publish(input19);
 UpdateDetour(controller.data(),.02f);assert(g_alchemy_pending19c4&&!StopAlchemyStage19c4());
 refusedWrite19=nullptr;UpdateDetour(controller.data(),.02f);assert(!g_alchemy_pending19c4&&Rd<float>(action.data(),kActSpeedBias,-1)==1.f);
 prepare19();traceEnabled=true;kcd2vr::diagnostics::capture168::gate.select(kcd2vr::diagnostics::capture168::Domain::Interaction,true,1000);lines.clear();
 UpdateDetour(controller.data(),.02f);for(unsigned i=0;i<10;++i)UpdateDetour(controller.data(),.02f);
 auto stageLines19=[&]{unsigned n=0;for(const auto& line:lines)if(line.find("F5_ALCHEMY_STAGE")!=std::string::npos)++n;return n;};
 assert(stageLines19()==1);input19.held[1]=false;stage::control.publish(input19);
 refusedWrite19=action.data()+kActSpeedBias;for(unsigned i=0;i<10;++i)UpdateDetour(controller.data(),.02f);
 assert(stageLines19()==2&&g_alchemy_pending19c4);refusedWrite19=nullptr;UpdateDetour(controller.data(),.02f);
 assert(stageLines19()==3&&!g_alchemy_pending19c4); // one pause, one pending, one resume, no per-frame formatting
 traceEnabled=false;kcd2vr::diagnostics::capture168::gate.select(kcd2vr::diagnostics::capture168::Domain::Interaction,false,1000);
 stage::attached.publish({});stage::control.publish({});act::witness175.Clear();act::witness175.ready=false;ws::setExperiment36(false);kcd2vr::combat_rules178r10::f5Rung=0;
 std::puts("PASS actual alchemy carafe child pause: native speed propagation, held stage, release resume once, 16 ownership/freshness negatives, compare-owned restoration and retained cleanup");
}
