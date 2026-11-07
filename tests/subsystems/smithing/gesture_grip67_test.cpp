// Actual tool attachment and packet builder; only existing fixture native
// leaves are substituted. Native phase still qualifies every live binding.
#define main OriginalSmithToolG1
#include "../../smith_tool36_test.cpp"
#undef main
namespace ai=kcd2vr::activity_input163;
namespace cr=kcd2vr::combat_rules178r10;
namespace ws=kcd2vr::workstation178r20c1;
static void setupGesture67(){
    namespace mh=kcd2vr::minigame_hands45;
    Image();frame={};now=7000;writes=0;tracking=placement=identity=true;
    ai::witness175.Clear();ai::witness175.ready=true;ai::witness175.transitionDepth175=0;
    ai::witness175.Begin(ai::Kind175::Smithing,P(smith.data()));Phase(11);
    cr::workstationEnabled=true;cr::f5Rung=0;ws::setExperiment36(false);ws::setSmithAutoHold36(true);
    mh::setMode(mh::Smithing,mh::Gesture);mh::publish(mh::Smithing,now);
    hands59c7f::rung=0;hands59c7f::admitted=false;
    g_henry_char_attachment_manager=manager.data();g_pose_epoch6689=7;
    g_tool_socket59c10[0].publish({});g_tool_socket59c10[1].publish({});
    ws::heldToolActivity59c10=int(ws::Kind::Anvil);
    frame.stamp.actor=P(character.data());frame.stamp.epoch=7;frame.stamp.sample=1;
    frame.valid[0]=frame.valid[1]=true;
    frame.native[0].t[0]=1;frame.allowed[0].t[0]=2;original[0].t[0]=1.1f;
    bool body=false;(void)ToolContext19c4r2(true,false,false,false,false,false,false,false,&body);
    ck(body,"Gesture mode admits the actual workstation body without F5/F10");g_dialogue_body178r9=body;
    Fresh();
    kcd2vr::subsystems::smithing::bridge67::tools[0].publish({});
    ck(!ObserveNativeActivityHand46(hammerObject.data(),attachment.data()),"actual Gesture observer yields the native transform owner");
    kcd2vr::subsystems::smithing::bridge67::NativeTool observed{};
    ck(kcd2vr::subsystems::smithing::bridge67::tools[0].read(observed)&&
       kcd2vr::subsystems::smithing::bridge67::qualified(observed,now,0,P(smith.data()),ai::witness175.lifetime175.load(),7,mh::modeEpoch67.load()),
       "actual native attachment observer publishes the bound hammer input proof");
    ck(!ToolAttachment59c10(hammerObject.data(),attachment.data()),"first capture publishes only native socket");
    Correlate();ck(ToolAttachment59c10(hammerObject.data(),attachment.data()),"canonical gesture hammer acquired");
    Correlate();ck(frame.tools59c10[0].socket.gripReady67,"actual packet stores the first native grip");
}
int main(){
    namespace mh=kcd2vr::minigame_hands45;
    setupGesture67();const auto initial=frame.tools59c10[0].socket.grip67;
    kcd2vr::subsystems::smithing::contact68::HammerFrame recorded68{};
    ck(!g_smith_placed68.read(recorded68),"recorder-off tool placement publishes no diagnostic hammer");
    cr::f5Rung=1;frame.doorTrackingEpoch66=5;
    diagnostics::capture168::gate.select(diagnostics::capture168::Domain::Interaction,true,now);
    ck(ToolAttachment59c10(hammerObject.data(),attachment.data()),"recorded successful actual tool placement");
    ck(g_smith_placed68.read(recorded68)&&recorded68.placed&&recorded68.sourceEpoch==5&&
       recorded68.poseEpoch==frame.stamp.epoch&&recorded68.sample==frame.stamp.sample&&
       recorded68.binding.key==frame.tools59c10[0].socket.smith36.key&&recorded68.binding.instance==P(smith.data())&&
       std::memcmp(recorded68.world,lastWanted,sizeof(recorded68.world))==0,
       "diagnostic hammer is the placed native item and exact world transform, not a requested empty wrist");
    cr::f5Rung=0;diagnostics::capture168::gate.select(diagnostics::capture168::Domain::Interaction,false,now);
    const float wanted=lastWanted[4];
    auto previous=frame.tools59c10[0].socket.smith36;
    for(int phase:{14,16,11}){
        Phase(phase);++frame.stamp.sample;Fresh();
        original[0].t[0]+=.13f;frame.native[0].t[0]-=.04f;
        QuatT28 changed{};changed.t[0]=.05f*float(phase);Put(attachment,0x9C,changed);
        ck(ToolAttachment59c10(hammerObject.data(),attachment.data()),"phase change keeps continuous canonical grip placement");
        Correlate();const auto& p=frame.tools59c10[0];
        ck(p.valid&&p.socket.smith36.phase==phase&&p.socket.gripReady67,"new phase independently revalidates live native binding");
        ck(std::memcmp(&p.socket.grip67,&initial,sizeof(initial))==0,"11/14/16 authored offsets cannot recapture canonical grip");
        ck(std::fabs(lastWanted[4]-wanted)<.00001f,"native raise/fall does not move the controller-held tool away");
        ck(!st::same(previous,p.socket.smith36)&&st::sameGripIdentity67(previous,p.socket.smith36),"phase-free grip identity never weakens phase-aware admission");
        previous=p.socket.smith36;
    }
    // An unknown phase still refuses, even while an old canonical grip exists.
    Phase(-1);int before=writes;ck(!ToolAttachment59c10(hammerObject.data(),attachment.data())&&writes==before,"unknown native phase cannot reuse canonical grip");
    setupGesture67();
    const auto oldEpoch=mh::modeEpoch67.load();mh::setMode(mh::Smithing,mh::SemiPassenger);mh::setMode(mh::Smithing,mh::Gesture);
    ck(mh::modeEpoch67.load()!=oldEpoch,"actual mode change advances owner epoch");
    ck(!ToolAttachment59c10(hammerObject.data(),attachment.data()),"old gesture socket cannot resume after a mode round trip");
    Correlate();ck(frame.tools59c10[0].valid&&frame.tools59c10[0].socket.modeEpoch67==mh::modeEpoch67.load(),"new mode gets a new grip transaction");
    ck(ToolAttachment59c10(hammerObject.data(),attachment.data()),"new mode recaptures from a fresh native packet");
    setupGesture67();std::array<unsigned char,0x40> replacementHolder=hammer;
    Put(model,st::kHammerSlot,P(replacementHolder.data()));original[0].t[0]+=.2f;
    ck(!ToolAttachment59c10(hammerObject.data(),attachment.data()),"different actual holder cannot carry the canonical grip");
    Correlate();ck(frame.tools59c10[0].valid&&frame.tools59c10[0].socket.smith36.holder==P(replacementHolder.data()),"replacement holder requires a new native grip transaction");
    setupGesture67();const auto first=frame.tools59c10[0].socket.smith36;
    auto replacement=first;replacement.holder+=8;ck(!st::sameGripIdentity67(first,replacement),"changed holder invalidates canonical grip identity");
    replacement=first;++replacement.lifetime;ck(!st::sameGripIdentity67(first,replacement),"changed minigame lifetime invalidates canonical grip identity");
    replacement=first;++replacement.key;ck(!st::sameGripIdentity67(first,replacement),"changed native item invalidates canonical grip identity");
    replacement=first;replacement.attachment+=8;ck(!st::sameGripIdentity67(first,replacement),"changed native attachment invalidates canonical grip identity");
    ai::witness175.End(P(smith.data()));ai::witness175.Begin(ai::Kind175::Smithing,P(smith.data()));Phase(11);Fresh();
    ck(!ToolAttachment59c10(hammerObject.data(),attachment.data()),"same pointer with new lifetime cannot retain old grip");
    // H21: the native fixture owns the iron/tongs pose and its supporting
    // wrist; only the separate hammer remains controller-positioned.
    for(bool iron:{false,true}){
        setupGesture67();void* object=iron?static_cast<void*>(workpieceObject.data()):static_cast<void*>(tongsObject.data());
        void* att=iron?static_cast<void*>(attachmentE.data()):static_cast<void*>(attachmentL.data());
        const auto entity=iron?44u:43u;const auto beforeSupport=writes;
        ck(ObserveNativeActivityHand46(object,att),"actual Gesture support attachment retains native Update");
        ck(NativeActivityHandOwned46(false,P(character.data()),7,1,now),"fixture support earns same native wrist/finger owner");
        ck(!NativeActivityHandOwned46(false,P(character.data()),7,0,now),"hammer never borrows fixture wrist ownership");
        ck(!ToolAttachment59c10(object,att)&&writes==beforeSupport,"direct tool path also refuses support controller placement");
        st::Binding current{};ck(ReadSmithTool36(att,entity,P(character.data()),current),"current native support binding read");
        ToolSocket59c10 old{};old.smith36=current;old.attachment=att;old.character=character.data();old.id=entity;old.side=1;old.joint=0;
        old.epoch=7;old.tick=now;old.request=ws::smithHoldEpoch36.load();old.modeEpoch67=mh::modeEpoch67.load();
        g_tool_socket59c10[1].publish(old);frame.tools59c10[1].valid=true;
        frame.allowed[1].t[0]=20.f;BuildToolPoses59c10(frame,original.data(),original.size());
        ck(!frame.tools59c10[1].valid&&writes==beforeSupport,"old controller support socket retired even with far tracked hand");
        for(int phase:{14,16,17,11}){Phase(phase);Fresh();
            ck(ObserveNativeActivityHand46(object,att)&&NativeActivityHandOwned46(false,P(character.data()),7,1,now),"native flip/work phase keeps same fixture support owner");
            ck(!ToolAttachment59c10(object,att)&&writes==beforeSupport,"native support animation cannot compete with controller placement");}
        if(iron)Put(workpiece,st::kAttached,std::uint8_t(0));else Put(tongs,st::kAttached,std::uint8_t(0));
        ck(!ObserveNativeActivityHand46(object,att)&&!NativeActivityHandOwned46(false,P(character.data()),7,1,now),"native detach revokes fixture wrist and placement permission");
    }
    std::printf("PASS G1 canonical smith grip: %u checks; actual placement/build path, mocked native leaves\n",checks);
}
