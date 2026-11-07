// Runs the real production72C97E callback, synthetic owned memory only.
 {
 std::array<unsigned char,0x300> otherOwner32{};
 std::array<unsigned char,0x100> otherEntity32{};
 put168(otherOwner32.data()+0x278,enemy.data());put168(otherOwner32.data()+0x38,otherEntity32.data());
 put168(enemy.data()+0x1F8,otherOwner32.data());
 put168(otherEntity32.data(),base+0x4072C78);put168(otherEntity32.data()+0xC,std::uint32_t(456));
 Matrix34V6550 tm32{};tm32.m[0]=tm32.m[5]=tm32.m[10]=1;
 put168(entity.data()+0x58,tm32);tm32.m[7]=.8f;put168(otherEntity32.data()+0x58,tm32);
 g_combat_experiment175=true;g_combat_integrated178r22c2=true;
 g_census_mode=3;g_weapon_control_mode=7;g_fist_fault101=false;
 frame.player=player.data();frame.request=5;frame.input.sample=99;frame.input.tick=10000;frame.input.bodyEpoch=2;
 frame.input.allowed=frame.input.coherentMelee119=frame.input.tracked[0]=frame.input.tracked[1]=true;
 frame.profiles[0].classId=1;frame.output.active[0]=frame.output.owned[0]=true;
 g_phys_ready_sample6698=99;g_phys_frame6698.publish(frame);SetClinchDistanceMeters32(.55f);
 put168(state.data()+0x218,-1);put168(state.data()+0xCF8,enemy.data());put168(enemyState.data()+0xCF8,actor.data());
 put168(actor.data()+0x3F0,factory.data());put168(enemy.data()+0x3F0,factory.data());
 put168(factory.data()+8,actor.data());put168(image.data()+0x53204E4,35);
 put168(state.data()+8,std::uint8_t(1));put168(enemyState.data()+8,std::uint8_t(1));
 auto radius32=[&](bool reject,bool nativeResult=true){
   gpr(ctx,Gpr::Rbx)=reinterpret_cast<std::uintptr_t>(factory.data());
   gpr(ctx,Gpr::Rax)=nativeResult?0x1122334455667701ull:0x1122334455667700ull;auto before32=ctx;
   CloseCombatAutoGate175(ctx);
   check168(gpr(ctx,Gpr::Rax)==(reject?0x1122334455667700ull:gpr(before32,Gpr::Rax)));
   for(unsigned i=0;i<10;++i)if(i!=unsigned(Gpr::Rax))check168(ctx.registers[i]==before32.registers[i]);
   check168(ctx.stack==before32.stack&&factory[0x1A]==0x5A);
 };
 radius32(true); // sword0.8m: decline this candidate only
 put168(factory.data()+8,enemy.data());radius32(true); // NPC initiator, same Henry pair
 tm32.m[7]=.4f;put168(otherEntity32.data()+0x58,tm32);radius32(false); // inside preserves native
 radius32(false,false); // never widen native false into success
 tm32.m[7]=.8f;put168(otherEntity32.data()+0x58,tm32);radius32(true); // current, not cached, positions
 SetClinchDistanceMeters32(0);radius32(false);SetClinchDistanceMeters32(.55f);
 SetClinchDistanceMeters32(std::numeric_limits<float>::quiet_NaN());radius32(false);SetClinchDistanceMeters32(.55f);
 frame.input.tick=9919;g_phys_frame6698.publish(frame);radius32(false);frame.input.tick=10000;
 frame.input.bodyEpoch=1;g_phys_frame6698.publish(frame);radius32(false);frame.input.bodyEpoch=2;
 frame.request=4;g_phys_frame6698.publish(frame);radius32(false);frame.request=5;
 frame.input.sample=98;g_phys_frame6698.publish(frame);radius32(false);frame.input.sample=99;
 frame.input.tracked[1]=false;g_phys_frame6698.publish(frame);radius32(false);frame.input.tracked[1]=true;
 frame.input.allowed=false;g_phys_frame6698.publish(frame);radius32(false);frame.input.allowed=true;
 frame.profiles[0].classId=8;g_phys_frame6698.publish(frame);radius32(false);frame.profiles[0].classId=1;
 frame.output.owned[0]=false;g_phys_frame6698.publish(frame);radius32(false);frame.output.owned[0]=true;
 g_phys_frame6698.publish(frame);
 g_ctx_dialogue=true;radius32(false);g_ctx_dialogue=false;
 g_ctx_menu=true;radius32(false);g_ctx_menu=false;
 g_ctx_cutscene=true;radius32(false);g_ctx_cutscene=false;
 g_ctx_scripted=true;radius32(false);g_ctx_scripted=false;
 g_combat_experiment175=false;radius32(false);g_combat_experiment175=true;
 kcd2vr::animation_filter::nativeBodyOwned112=true;radius32(false);kcd2vr::animation_filter::nativeBodyOwned112=false;
 kcd2vr::animation_filter::nativeKickActive175=true;radius32(false);kcd2vr::animation_filter::nativeKickActive175=false;
 kcd2vr::retail_build112::hostRetailVerified112=false;radius32(false);kcd2vr::retail_build112::hostRetailVerified112=true;
 put168(otherEntity32.data(),base+0x4072C70);radius32(false);put168(otherEntity32.data(),base+0x4072C78);
 put168(otherEntity32.data()+0xC,std::uint32_t(123));radius32(false);put168(otherEntity32.data()+0xC,std::uint32_t(456));
 put168(otherOwner32.data()+0x278,actor.data());radius32(false);put168(otherOwner32.data()+0x278,enemy.data());
 put168(otherOwner32.data()+0x38,entity.data());radius32(false);put168(otherOwner32.data()+0x38,otherEntity32.data());
 put168(enemyState.data()+0xCF8,static_cast<void*>(nullptr));radius32(false);put168(enemyState.data()+0xCF8,actor.data());
 tm32.m[7]=std::numeric_limits<float>::quiet_NaN();put168(otherEntity32.data()+0x58,tm32);radius32(false);
 tm32.m[7]=.8f;put168(otherEntity32.data()+0x58,tm32);radius32(true);
 // Open fists outside radius use newF4 pair rule; preexisting closed-fist
 // F10 no-clinch behavior remains independently covered above.
 std::array<unsigned char,0x10> character32{};g_committed_character6689=character32.data();
 boxing.sample.actor=reinterpret_cast<std::uintptr_t>(character32.data()); // distinct CCharInstance domain
 g_census_mode=5;g_weapon_control_mode=0;boxing.sample.grip[0]=boxing.sample.trigger[0]=0;
 boxing.sample.tick=10000;boxing.request=9;boxing.sample.epoch=2;g_fist_frame101.publish(boxing);
 radius32(true);g_committed_character6689=entity.data();radius32(false);g_committed_character6689=character32.data();
 tm32.m[7]=.4f;put168(otherEntity32.data()+0x58,tm32);radius32(false);
 }
