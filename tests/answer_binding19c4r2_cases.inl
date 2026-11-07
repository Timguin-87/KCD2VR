// Actual frik answer snapshot reader, real admitted binding and memory adapters.
{
 namespace ans=kcd2vr::answer59c4r2;
 g_combat_integrated178r22c2=true;g_synchronized59c10r9=true;
 put22(state.data()+0x38,8);put22(state.data()+0x218,0);put22(state.data()+0xCF8,other.data());
 ans::Lineage proof{};auto answer=[&](){return GeometryNativeAnswer19c4r2(actor.data(),other.data(),0,proof);};
 assert(answer()&&proof.action==reinterpret_cast<std::uintptr_t>(action.data()));
 const auto originalAnswer=proof;
 // A new release is pending while A remains the admitted native action. Read
 // the bounded A history, never blindly copy latest released B as its lineage.
 auto newer=proof.release;++newer.serial;newer.gesture59c3+=100;newer.sample+=1;
 kcd2vr::contact59c3::releases.publish(newer);kcd2vr::contact176::released.publish(newer);
 assert(answer()&&ans::same(proof.release,originalAnswer.release));
 ac50::Binding bound{};assert(g_motion_clock_binding50.read(bound));
 for(unsigned field=0;field<8;++field){auto wrong=bound;
  switch(field){case 0:++wrong.intent.player;break;case 1:++wrong.intent.key;break;case 2:++wrong.intent.generation;break;
   case 3:++wrong.intent.epoch;break;case 4:++wrong.intent.request;break;case 5:++wrong.intent.sample;break;
   case 6:++wrong.intent.tick;break;case 7:++wrong.intent.gesture59c3;break;}
  g_motion_clock_binding50.publish(wrong);assert(!answer()&&!proof.release.serial);g_motion_clock_binding50.publish(bound);
 }
 slots[0]=reinterpret_cast<std::uintptr_t>(other.data());assert(!answer());slots[0]=reinterpret_cast<std::uintptr_t>(action.data());
 put22(helper.data()+0x20,other.data());assert(!answer());put22(helper.data()+0x20,action.data()+0x88);
 put22(state.data()+0x38,4);assert(!answer());put22(state.data()+0x38,8);
 put22(state.data()+0x218,1);assert(!answer());put22(state.data()+0x218,0);
 put22(type.data()+8,1);assert(!answer());put22(type.data()+8,0);
 put22(state.data()+0xCF8,static_cast<void*>(nullptr));assert(!answer());put22(state.data()+0xCF8,other.data());
 ++kcd2vr::contact176::recoveryEpoch55c1;assert(!answer());--kcd2vr::contact176::recoveryEpoch55c1;
 hostTickCount64+=1501;assert(!answer());hostTickCount64-=1501;
 f.output.handlingOnly55c1[0]=true;g_phys_frame6698.publish(f);assert(!answer());f.output.handlingOnly55c1[0]=false;g_phys_frame6698.publish(f);
 g_synchronized59c10r9=false;assert(!answer());g_synchronized59c10r9=true;
 kcd2vr::motion59c4::selected=true;assert(!answer());kcd2vr::motion59c4::selected=false;
 assert(answer());g_synchronized59c10r9=false;g_combat_integrated178r22c2=false;
 put22(state.data()+0x38,4);put22(state.data()+0x218,1);
 std::printf("answer_binding19c4r2=PASS actual_current_action_history_reader=YES native_execution=NONE\n");
}
