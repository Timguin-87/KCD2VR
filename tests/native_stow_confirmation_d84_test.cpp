// Complete production native-grasp owner; only native/OS leaves are inherited
// from the established fixture. Native transfer semantics are byte-pinned in
// the separate retail qualification, not inferred from these simulated leaves.
#ifndef _MSC_VER
#define __fastcall
#define __finally catch(...) {} if(true)
#define __declspec(x)
#endif
#define KCD2VR_RESOURCE_D60_NO_MAIN
#include "alchemy_resource_receipt_d60_test.cpp"

namespace kcd2vr::action_time_owner59c11 {namespace {
namespace hp84=kcd2vr::haptics101;
static unsigned nativeStowChecksD84=0;
static void nativeStowCheckD84(bool value,const char* why){
    ++nativeStowChecksD84;if(!value){std::fprintf(stderr,"D84 native stow: %s\n",why);std::abort();}
}
static hp84::Pulse nativePulseD84(int side){hp84::Pulse p{};hp84::queue[side].read(p);return p;}
static void nativeFrameD84(int side,std::uint64_t tick,float grip=1,float distance=.6f){
    hostTickCount64=tick;ng13::AcquisitionInput55 input{};
    input.character=reinterpret_cast<std::uintptr_t>(character.data());input.epoch=9;input.trackingEpoch=19;
    input.sample=tick;input.tick=tick;input.headTracked=true;input.tracked[side]=true;input.grip[side]=grip;
    PublishAcquisitionInput55(input);
    ng13::HandFrame f{};f.character=input.character;f.epoch=9;f.sample=tick;f.tick=tick;
    f.tracked[side]=true;f.grip[side]=grip;f.chestDistance[side]=distance;PublishHands(f);
}
static void nativeStowSetupD84(int side,bool capture=false){
    reset();ResetOwner();g_consumed_use13=0;put(logic.data(),0x30,side);
    put(image.data(),0x40AC2B0,static_cast<const char*>("hand_left"));
    nativeFrameD84(side,1000);NoteUseSent(side,1000);QueueDetour(controller.data(),action.data(),0);TickOwner(controller.data(),1000);
    nativeStowCheckD84(g_owner.state==Playing&&g_owner.side==side,"actual Use/queue owns requested hand");
    hp84::select(true);hp84::allowed=true;for(auto& queue:hp84::queue)queue.publish({});
    namespace cap=kcd2vr::diagnostics::capture168;
    cap::gate.select(cap::Domain::Interaction,false,1000);
    if(capture)cap::gate.select(cap::Domain::Interaction,true,1000);
    traceEnabled=true;lines.clear();attach();
    nativeStowCheckD84(g_pick13.bind.side==side&&g_pick13.attached,"real native attachment owned on acquiring hand");
}
static void nativeStowFinishD84(int side){
    nativeFrameD84(side,1020,1,.15f);TickOwner(controller.data(),1020);
    nativeFrameD84(side,1201,1,.15f);TickOwner(controller.data(),1201);
}
static unsigned nativeStowReceiptsD84(){unsigned n=0;for(const auto& line:lines)
    if(line.find("D84 F5_STOW_CONFIRM source=NATIVE_GRASP ")==0)++n;return n;}
static int nativeStowCasesD84(){
    for(int side=0;side<2;++side)for(bool capture:{false,true}){
        nativeStowSetupD84(side,capture);nativeStowFinishD84(side);
        auto cue=nativePulseD84(side);
        nativeStowCheckD84(transfers==1&&g_owner.state==Idle&&Container13(item.data())==&inventory,"native transfer completed once");
        nativeStowCheckD84(cue.key==key&&cue.amplitude==.65f&&cue.durationMs==65&&hp84::eligible(cue,1201),"confirmed storage cues acquiring hand");
        nativeStowCheckD84(!nativePulseD84(1-side).key,"other hand has no storage cue");
        nativeStowCheckD84(nativeStowReceiptsD84()==unsigned(capture),"F5 controls receipt only");
        PickupEndDetour19c4(logic.data(),0,0);TickOwner(controller.data(),1202);
        nativeStowCheckD84(transfers==1&&nativePulseD84(side).sequence==cue.sequence,"retired native callbacks cannot replay cue");
    }
    for(int side=0;side<2;++side){
        nativeStowSetupD84(side);transferAck=false;nativeStowFinishD84(side);
        nativeStowCheckD84(transfers==1&&!nativePulseD84(side).key,"failed transfer has no success cue");
        nativeStowSetupD84(side);hp84::enabled=false;nativeStowFinishD84(side);
        nativeStowCheckD84(transfers==1&&!nativePulseD84(side).key,"haptics preference cannot gate native storage");
        nativeStowSetupD84(side);hp84::allowed=false;nativeStowFinishD84(side);
        nativeStowCheckD84(transfers==1&&!nativePulseD84(side).key,"XR haptic admission remains authoritative");
        nativeStowSetupD84(side);nativeFrameD84(side,1020,0,.6f);TickOwner(controller.data(),1020);
        nativeStowCheckD84(drops==1&&transfers==0&&!nativePulseD84(side).key,"loose native drop is not inventory confirmation");
        nativeStowSetupD84(side);SetContextAvailable(false);TickOwner(controller.data(),1001);
        nativeStowCheckD84(transfers==0&&!nativePulseD84(side).key,"context cancellation cannot confirm storage");
        nativeStowSetupD84(side);mergeOnStow=true;nativeStowFinishD84(side);
        nativeStowCheckD84(transfers==1&&itemRetired&&nativePulseD84(side).key==key,"native stack merge success confirms original occurrence without retired item read");
    }
    std::printf("D84 complete native stow confirmation: %u checks PASS; native leaves substituted; headset NOT_RUN\n",nativeStowChecksD84);
    return 0;
}
}}
int main(){return kcd2vr::action_time_owner59c11::nativeStowCasesD84();}
