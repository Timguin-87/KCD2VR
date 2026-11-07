// Reuse the retained explicit memory leaves, real alchemy membership reader and
// production attachment include. Its historical test main is not substituted
// for this new end-to-end native-passenger dataflow fixture.
#define main RetainedToolAttachmentFixtureMain
#include "tool_attachment59c10_test.cpp"
#undef main

static unsigned nativeUpdates46=0;
static void AttachmentPass46(void* self,void* attachment){
    const auto native=[](){++nativeUpdates46;};
#include "../src/vrik/native_activity_passthrough46.inl"
    if(!ToolAttachment59c10(self,attachment))native();
}
static int NativeActivityScenario46(){
    namespace ai=kcd2vr::activity_input163;
    namespace ws=kcd2vr::workstation178r20c1;
    namespace mh=kcd2vr::minigame_hands45;
    namespace cr=kcd2vr::combat_rules178r10;
    cr::workstationEnabled=true;cr::f5Rung=0;ws::setExperiment36(false);
    ws::setSmithAutoHold36(false);mh::mode=mh::SemiPassenger;mh::publish(mh::Alchemy,now);
    ai::witness175.ready=true;ai::witness175.Begin(ai::Kind175::Alchemy,reinterpret_cast<std::uintptr_t>(alchemy.data()));
    ws::heldToolActivity59c10=int(ws::Kind::Alchemy);ws::heldToolActivityTick59c10=now;
    g_gameplay_allowed=false;g_dialogue_body178r9=true;g_right_grip=g_left_grip=0;
    g_henry_char_attachment_manager=manager.data();g_pose_epoch6689=7;
    Put(self,0,std::uintptr_t(0x3A68F68));Put(self,8,std::uint32_t(42));
    Put(attachment,0,std::uintptr_t(0x3A657C0));Put(attachment,0x28,static_cast<void*>(manager.data()));
    Put(attachment,0x20,reinterpret_cast<std::uintptr_t>(self.data()));
    Put(manager,0x70,static_cast<void*>(character.data()));Put(character,0,std::uintptr_t(0x40602F8));
    Put(attachment,0x10,std::uintptr_t("entity_right"));
    Put(alchemy,0,std::uintptr_t(0x3F60B08));Put(alchemy,0x2C,std::uint8_t(1));
    Put(alchemy,0x18,reinterpret_cast<std::uintptr_t>(alchemyPlayer.data()));
    Put(alchemyPlayer,0,std::uintptr_t(0x3AA4B98));
    Put(alchemy,0x2A0,std::uint32_t(3));Put(alchemy,0x150+4*3,std::uint32_t(42));
    alchemyActive=true;(void)ToolContext19c4r2(true);
    const auto ch=reinterpret_cast<std::uintptr_t>(character.data());
    assert(!NativeAlchemyFingers18(ch,7,0,now));
    AttachmentPass46(self.data(),attachment.data());
    assert(nativeUpdates46==1&&writes==0&&NativeAlchemyFingers18(ch,7,0,now));
    ToolSocket59c10 empty{};assert(!g_tool_socket59c10[0].read(empty)||!empty.self);
    // No F5, Grip, placement proof or committed-pose bootstrap was required.
    assert(!NativeAlchemyFingers18(ch,8,0,now)&&!NativeAlchemyFingers18(ch,7,1,now));
    assert(!NativeAlchemyFingers18(ch+8,7,0,now));
    Put(alchemy,0x2A0,std::uint32_t(21));assert(!NativeAlchemyFingers18(ch,7,0,now));
    Put(alchemy,0x2A0,std::uint32_t(3));
    ai::witness175.lifetime175.fetch_add(1);assert(!NativeAlchemyFingers18(ch,7,0,now));
    assert(ObserveNativeActivityHand46(self.data(),attachment.data()));
    assert(NativeAlchemyFingers18(ch,7,0,now));
    (void)ToolContext19c4r2(true,true);assert(!ObserveNativeActivityHand46(self.data(),attachment.data()));
    assert(!NativeAlchemyFingers18(ch,7,0,now));(void)ToolContext19c4r2(true);
    now+=81;assert(!ObserveNativeActivityHand46(self.data(),attachment.data()));now-=81;
    mh::mode=mh::Native;assert(!ObserveNativeActivityHand46(self.data(),attachment.data()));mh::mode=mh::SemiPassenger;
    g_henry_char_attachment_manager=nullptr;assert(!ObserveNativeActivityHand46(self.data(),attachment.data()));g_henry_char_attachment_manager=manager.data();
    // C55: only the real native attachment + a successfully committed body
    // packet publishes the guided grasp point. F10 placement is not a witness.
    cr::f5Rung=3;ws::setExperiment36(true);mh::mode=mh::Native;
    bool guidedBody55=false;(void)ToolContext19c4r2(true,false,false,false,false,false,false,false,&guidedBody55);
    assert(guidedBody55&&ws::semiPassengerOwns45(now)&&ObserveNativeActivityHand46(self.data(),attachment.data()));
    CommittedFrame6695 guidedFrame{};guidedFrame.stamp={7,33,now,ch};guidedFrame.valid[0]=true;
    guidedFrame.propRequestedValid5916c1[0]=true;guidedFrame.native[0].t[0]=.2f;
    guidedFrame.propRequested5916c1[0].t[0]=.25f;
    PublishGuidedTools55(guidedFrame,9);
    kcd2vr::guided_crafting55::Dock gd{};
    assert(kcd2vr::guided_crafting55::dock[0].read(gd)&&gd.entity==42&&gd.kind==1&&gd.type==3&&gd.side==0);
    assert(gd.instance==reinterpret_cast<std::uintptr_t>(alchemy.data())&&gd.object==reinterpret_cast<std::uintptr_t>(self.data()));
    assert(std::fabs(gd.gap-.05f)<.001f&&gd.poseEpoch==7&&gd.sourceEpoch==9&&gd.sequence==33);
    Put(alchemy,0x2A0,std::uint32_t(21));PublishGuidedTools55(guidedFrame,9);
    assert(kcd2vr::guided_crafting55::dock[0].read(gd)&&!gd.object);Put(alchemy,0x2A0,std::uint32_t(3));
    cr::f5Rung=0;ws::setExperiment36(false);mh::mode=mh::SemiPassenger;
    // Wrong current IAttachmentObject never earns visual ownership.
    Put(attachment,0x20,std::uintptr_t(0));assert(!ObserveNativeActivityHand46(self.data(),attachment.data()));
    Put(attachment,0x20,reinterpret_cast<std::uintptr_t>(self.data()));
    ai::witness175.End(reinterpret_cast<std::uintptr_t>(alchemy.data()));alchemyActive=false;
    ai::witness175.Begin(ai::Kind175::Smithing,999);mh::publish(mh::Smithing,now);
    ws::heldToolActivity59c10=int(ws::Kind::Anvil);(void)ToolContext19c4r2(true);
    smithBinding37.instance=999;smithBinding37.player=888;smithBinding37.character=ch;
    smithBinding37.attachment=reinterpret_cast<std::uintptr_t>(attachment.data());
    smithBinding37.object=reinterpret_cast<std::uintptr_t>(self.data());smithBinding37.holder=777;
    smithBinding37.entity=42;smithBinding37.side=0;smithBinding37.tool=0;smithBinding37.phase=11;smithBinding37.lifetime=88;
    smithActive37=true;cr::f5Rung=2;ws::setExperiment36(true);ws::setSmithAutoHold36(true);
    AttachmentPass46(self.data(),attachment.data());
    assert(nativeUpdates46==2&&writes==0&&NativeSmithFingers36(ch,7,0,now));
    ++smithBinding37.phase;assert(!NativeSmithFingers36(ch,7,0,now));
    assert(ObserveNativeActivityHand46(self.data(),attachment.data()));
    assert(NativeSmithFingers36(ch,7,0,now));
    ai::witness175.End(999);assert(!ObserveNativeActivityHand46(self.data(),attachment.data()));
    assert(writes==0&&receipts.empty());
    std::puts("native_activity_hands46: actual attachment->native-update->finger permission without F5/Grip/placement, lifecycle/UI/identity revocation PASS");
    return 0;
}

#ifndef KCD2VR_NATIVE_ACTIVITY_NO_MAIN
int main(){return NativeActivityScenario46();}
#endif
