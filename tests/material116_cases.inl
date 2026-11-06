 // Execute actual observers with native functions replaced, never WHGame calls.
 {
 std::array<unsigned char,0xA0> impact{};std::array<unsigned char,0xE0> params{};std::array<unsigned char,0x20> service{};
 materialSelf116=service.data();materialParams116=params.data();put(service.data(),base+0x4056C88);put(image.data()+0x492D920,service.data());
 g_material_dispatch116=&materialDispatch116;g_material_lookup116=&materialLookup116;g_material_execute116=&materialExecute116;
 g_material_contract116=base;kcd2vr::retail_build112::hostRetailVerified112=true;
 diagnostics::lab::Set(diagnostics::lab::Key::F3,1);hostTickCount64+=100;cc::select(true,hostTickCount64);
 put(player.data()+0x30,std::uint32_t(123));put(entity.data()+0xC,std::uint32_t(123));put(state.data()+0xCE0,actor.data());
 ContactIdentity114 identity{};identity.id=123;identity.entity=entity.data();identity.epoch=g_pose_epoch6689.load();identity.tick=hostTickCount64;g_contact_identity114.publish(identity);
 put(impact.data(),std::uint32_t(123));put(impact.data()+0x28,std::uint32_t(0));
 put(params.data()+0x34,1.f);put(params.data()+0x38,2.f);put(params.data()+0x3c,3.f);put(params.data()+0x6c,1.f);put(params.data()+0x74,.6f);
 cc::Event material{};while(cc::pop(material)){}
 ck(!MaterialDispatchDetour116(nullptr,impact.data())&&materialCalls116==3,"material observer preserves all original calls and false execute result");
 ck(cc::pop(material)&&material.kind==cc::Kind::Material&&material.flags==1&&material.id==12&&material.layer==37&&!std::strcmp(material.name,"hit_passed_sword_slash_player"),"native row/surface/effect captured in Henry-qualified scope");
 const auto serial=material.sequence;
 ck(cc::pop(material)&&material.flags==2&&material.sequence==serial&&material.hand[0]==1&&material.hand[4]==0&&material.hand[5]==1&&material.phase==.6f&&material.value==0,"material execution captures position normal scale and actual return without damage claim");
 ck(cc::pop(material)&&material.flags==0&&material.sequence==serial&&!g_material_scope116.active,"dispatcher restores thread scope and correlates native result");
 ck(!cc::pop(material),"one dispatch produces only bounded observer records");
 put(impact.data(),std::uint32_t(456));hostTickCount64+=30;identity.tick=hostTickCount64;g_contact_identity114.publish(identity);
 const auto originalCalls=materialCalls116;MaterialDispatchDetour116(nullptr,impact.data());
 ck(materialCalls116==originalCalls+3&&!cc::pop(material),"unrelated NPC effect runs unchanged without recording");
 put(impact.data(),std::uint32_t(123));kcd2vr::retail_build112::hostRetailVerified112=false;
 MaterialDispatchDetour116(nullptr,impact.data());ck(!cc::pop(material),"unverified retail hash disables material observation");
 kcd2vr::retail_build112::hostRetailVerified112=true;put(state.data()+0xCE0,static_cast<void*>(nullptr));
 MaterialDispatchDetour116(nullptr,impact.data());ck(!cc::pop(material),"broken Henry backlink rejects material capture");put(state.data()+0xCE0,actor.data());
 ShutdownMaterial116();ck(!g_material_contract116.load()&&!g_material_dispatch116&&!g_material_lookup116&&!g_material_execute116,"material shutdown clears all observer ownership");
 }
