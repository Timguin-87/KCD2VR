// Included inside the existing complete action-owner TU fixture. Only native
// storage/consumer leaves are simulated; admission, clock ledger and cleanup
// execute the actual production functions. No game or recipe result is claimed.
static std::array<unsigned char,0x168*20> resourceScopesD69{};
static std::array<std::array<unsigned char,0x200>,3> resourceFifosD69{};
static unsigned checksResourceD69=0;
static void ckResourceD69(bool ok,const char* why){++checksResourceD69;if(!ok){std::fprintf(stderr,"FAIL D69 resource: %s\n",why);std::exit(1);}}
static unsigned char* resourceScopeD69(unsigned n){return resourceScopesD69.data()+n*kScopeStride;}
static void resourceRoot3SetupD69(){
 ResourceFixture46();g_owner={};g_pickup_clock_h23={};g_pickup_clock_live_h23=false;g_alchemy_stage19c4={};g_semi_resource_pending19={};
 const auto b=g_base.load();resourceScopesD69.fill(0);for(auto& f:resourceFifosD69)f.fill(0);
 // Exact byte sequences independently checked against SHA bdf8...4479f7 in
 // review/crafting/INDEPENDENT_NATIVE_PIN_AUDIT.json, not a native substitute.
 const unsigned char layer[]{0x8B,0x53,0x30,0x0F,0x28,0xD7,0x48,0x8B,0x06,0x41,0x03,0xD6,0x48,0x8B,0xCE,0xFF,0x90,0xB8,0,0,0,0x41,0xFF,0xC6,0x44,0x3B,0x73,0x34,0x72,0xE2};
 const unsigned char clock[]{0xF3,0x0F,0x59,0x73,0x58,0x0F,0x28,0xC6,0xF3,0x0F,0x58,0x43,0x60,0xF3,0x0F,0x58,0x43,0x64,0xF3,0x0F,0x58,0x83,0x1C,1,0,0};
 const unsigned char arg[]{0x0F,0x28,0xCE,0x48,0x8B,0xCB},tail[]{0xE9,0x28,6,0,0};
 std::memcpy(image.data()+0x4B3B43,layer,sizeof(layer));std::memcpy(image.data()+0x4B3AA2,clock,sizeof(clock));
 std::memcpy(image.data()+0x4B3AC4,arg,sizeof(arg));std::memcpy(image.data()+0x4B3AF3,tail,sizeof(tail));
 put(image.data(),0x3A47948+0x30,b+0x1A7E0E0);
 put(controller.data(),kCtrlScopeCount,16u);put(controller.data(),kCtrlScopes,static_cast<void*>(resourceScopesD69.data()));
 put(action.data(),kActRootScope,static_cast<void*>(resourceScopesD69.data()));
 for(unsigned i=0;i<16;++i){auto* s=resourceScopeD69(i);put(s,0,b+0x3AEA100);put(s,0x20,static_cast<void*>(contexts.data()));
  put(s,kScopeLayer,i==0?0u:15u);put(s,kScopeNumLayers,i==0?3u:i==14?1u:0u);put(s,kScopeSpeedBias,1.f);
  if(i==0||i==14||i==15)put(s,kScopeAction,static_cast<void*>(action.data()));
 }
 for(unsigned i=0;i<3;++i){auto* e=resourceFifosD69[i].data()+8;put(resourceFifosD69[i].data(),4,i==0?1u:0u);
  put(testSkel,0x48+i*0x40,reinterpret_cast<std::uintptr_t>(e));put(testSkel,0x50+i*0x40,1.f);
  if(!i){put(e,0,b+0x3A70A60);put(e,0x10,42u);put(e,0x4C,0x4104u);}
 }
}
static void resourceAttachD69(){SemiResourceScanDetour46(testSkel,false,.14f,.16f,&eventInfo46,nullptr,0);}
static void resourceTickD69(float dt){OwnerTransaction19c4 tx;AlchemyUpdateScopeR27 a(controller.data());TickSemiAlchemy46(controller.data(),dt,hostTickCount64);}
static bool resourceRestoreD69(){OwnerTransaction19c4 tx;AlchemyUpdateScopeR27 a(controller.data());return SemiAlchemyRestore46("D69_FIXTURE_RELEASE");}
static void resourceNativeAdvanceD69(float dt){
 // Explicit native consumer for verified child-time/scope-layer propagation.
 const float rate=Rd<float>(action.data(),kActSpeedBias,-1.f);
 put(action.data(),kActActiveTime,Rd<float>(action.data(),kActActiveTime,0)+dt*rate);
 for(unsigned i:{0u,14u,15u})put(resourceScopeD69(i),kScopeSpeedBias,rate);
 for(unsigned i=0;i<3;++i)put(testSkel,0x50+i*0x40,rate);
}
static void runResourceRoot3D69(){
 resourceRoot3SetupD69();SemiResourceState46 probe{};
 ckResourceD69(SemiAlchemyParent46(controller.data(),probe.owner),"real current resource parent for image-lease test");
 probe.skel=reinterpret_cast<std::uintptr_t>(testSkel);probe.animationId=42;
 testRetailReadyD69=false;ckResourceD69(!SemiResourceRoot3D67R1(probe,controller.data()),"pending image verification denies first qualification");
 testRetailReadyD69=true;ckResourceD69(bool(SemiResourceRoot3D67R1(probe,controller.data())),"ready image retries without cached pending failure");
 testRetailReadyD69=false;ckResourceD69(!SemiResourceRoot3D67R1(probe,controller.data()),"revoked image denied even after cached immutable pins");
 testRetailReadyD69=true;
 resourceRoot3SetupD69();resourceAttachD69();
 ckResourceD69(nativeAttach46==1&&nativeUse46==0,"actual Attach dispatches once before admission");
 ckResourceD69(g_semi_alchemy46.written&&g_semi_pending46&&Rd<float>(action.data(),kActSpeedBias,-1)==0,"root3 actual Acquire pauses owned child");
 ckResourceD69(g_semi_alchemy46.resourceRoot3D67R1.ownedScopes==0xC001,"actual owner stores root+LookPose+Looking proof");
 const float start=Rd<float>(action.data(),kActActiveTime,-1);
 Sample46(1010,.01);resourceTickD69(.01f);
 ckResourceD69(std::fabs(Rd<float>(action.data(),kActSpeedBias,-1)-1.f)<1e-6,"fresh work credit reaches actual child clock");
 resourceNativeAdvanceD69(.01f);resourceTickD69(.01f);
 ckResourceD69(std::fabs(Rd<float>(action.data(),kActActiveTime,-1)-start-.01f)<1e-6,"one credit advances native consumer once");
 ckResourceD69(Rd<float>(action.data(),kActSpeedBias,-1)==0&&g_semi_alchemy46.clock.pending==0,"same sample has no duplicate remaining credit");
 const float done=Rd<float>(action.data(),kActActiveTime,-1);resourceNativeAdvanceD69(.01f);resourceTickD69(.01f);
 ckResourceD69(Rd<float>(action.data(),kActActiveTime,-1)==done&&g_semi_alchemy46.consumed==input46.sequence,"repeat callback does not invent work");
 SemiResourceScanDetour46(testSkel,false,.40f,.43f,&eventInfo46,nullptr,0);
 ckResourceD69(nativeUse46==1&&!g_semi_pending46&&!g_semi_alchemy46.written&&Rd<float>(action.data(),kActSpeedBias,-1)==1,"actual Use once restores native rate and clears lease");
 resourceTickD69(.01f);ckResourceD69(nativeAttach46==1&&nativeUse46==1&&!g_semi_pending46,"later callback cannot reacquire completed resource");
 for(unsigned extra:{1u,2u}){resourceRoot3SetupD69();put(resourceFifosD69[extra].data(),4,1u);resourceAttachD69();
  ckResourceD69(nativeAttach46==1&&!g_semi_alchemy46.written&&Rd<float>(action.data(),kActSpeedBias,-1)==1,"nonempty extra CAF keeps native Attach and refuses rate lease");}
 resourceRoot3SetupD69();put(resourceScopeD69(1),kScopeLayer,1u);put(resourceScopeD69(1),kScopeNumLayers,1u);resourceAttachD69();
 ckResourceD69(nativeAttach46==1&&!g_semi_alchemy46.written&&Rd<float>(action.data(),kActSpeedBias,-1)==1,"foreign overlapping scope refuses acquisition without event loss");
 resourceRoot3SetupD69();resourceAttachD69();put(resourceFifosD69[1].data(),4,1u);resourceTickD69(.01f);
 ckResourceD69(!g_semi_alchemy46.written&&Rd<float>(action.data(),kActSpeedBias,-1)==1,"extra CAF after admission restores owned rate");
 resourceRoot3SetupD69();resourceAttachD69();put(action.data(),kActSpeedBias,.5f);resourceTickD69(.01f);
 ckResourceD69(!g_semi_alchemy46.written&&Rd<float>(action.data(),kActSpeedBias,-1)==.5f,"foreign child rate survives active proof failure cleanup");
 resourceRoot3SetupD69();resourceAttachD69();refusedWrite19=action.data()+kActSpeedBias;
 ckResourceD69(!resourceRestoreD69()&&g_semi_alchemy46.written&&g_semi_pending46,"failed owned restoration retains exact cleanup debt");
 refusedWrite19=nullptr;ckResourceD69(resourceRestoreD69()&&!g_semi_alchemy46.written&&Rd<float>(action.data(),kActSpeedBias,-1)==1,"cleanup retries and restores owned saved rate only");
 resourceRoot3SetupD69();resourceAttachD69();forbidRetiredRead19c4=true;
 ckResourceD69(!resourceRestoreD69()&&g_semi_alchemy46.written,"unreadable live child is not guessed retired");
 forbidRetiredRead19c4=false;ckResourceD69(resourceRestoreD69()&&!g_semi_alchemy46.written,"same exact owner cleanup resumes when readable");
 resourceRoot3SetupD69();resourceAttachD69();put(action.data(),kActToken,56u);put(action.data(),kActSpeedBias,.75f);
 ckResourceD69(resourceRestoreD69()&&!g_semi_alchemy46.written&&Rd<float>(action.data(),kActSpeedBias,-1)==.75f,"reused pointer/new native token is never restored through old lease");
 std::printf("D69 actual root3 resource owner %u checks PASS; native leaves explicit, game NOT_RUN\n",checksResourceD69);
}
