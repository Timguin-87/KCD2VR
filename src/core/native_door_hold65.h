#pragma once
#include "native_door_script19c4r4.h"
#include "door_hold65.h"

namespace kcd2vr::door_script19c4r4 {
// Hold-to-play native animation. No learned movement axis, time scrubbing,
// hinge model or controller distance is converted into authored progress.
struct HoldOwner65 {
    Request request{};door_hold65::Hand source{};
    std::uint64_t seen=0,press=0;std::uint32_t sequence=0;
    bool pending=false,active=false,paused=true;float wanted=0,reported=0;unsigned reports=0;
    bool commandPhase(int p) const noexcept {return p==0||p==1||p==3||p==4;}
    template<class IO> bool command(IO& io,int phase) noexcept {
        if(!commandPhase(phase)||sequence>8388602u)return false;
        sequence+=2;return io.publish(Command{sequence,std::uint32_t(request.serial),request.id,phase,wanted});
    }
    void clearOperation() noexcept {(void)door_hold65::operation.publish({});}
    template<class IO> void release(IO& io,const char* reason) noexcept {
        if(active||pending){(void)command(io,0);io.receipt(request,reason,wanted,false);}
        active=pending=false;paused=true;press=0;clearOperation();
    }
    template<class IO> void tick(IO& io,const Request& incoming,std::uint64_t now) noexcept {
        if(incoming.serial&&incoming.serial!=seen){
            release(io,"REPLACED");seen=incoming.serial;request=incoming;
            source={};pending=true;paused=true;press=0;wanted=reported=0;reports=0;
        }
        if(!pending&&!active)return;
        door_hold65::Hand hand{};
        if(!request.serial||request.serial>8388607ull||!request.id||request.id>16777215u||
           !request.tick||now<request.tick||(pending&&now-request.tick>250)||
           !io.control65(request,hand)||!door_hold65::current(hand,now)){
            release(io,"CONTEXT_OR_CURRENT_SOURCE_LOST");return;
        }
        if(!source.player){
            if(hand.grip<.30f||!hand.press||(request.inputPress65&&hand.press!=request.inputPress65)){
                release(io,"INITIAL_GRIP_RELEASED_OR_REPLACED");return;}
            source=hand;press=hand.press;
        }else if(!door_hold65::sameSource(source,hand)){
            release(io,"SOURCE_FAMILY_CHANGED");return;
        }
        Ack ack{};const bool current=io.ack(ack)&&ack.request==request.serial&&ack.source&&
            ack.source<=sequence&&door_hold65::fresh(ack.tick,now)&&Owner::fraction(ack.fraction);
        // ACK2 is generated only after the original Lua OnUpdate completes its
        // own operation. It retires the same lease, never fabricates completion.
        if(active&&current&&ack.state==2){
            wanted=ack.fraction;io.receipt(request,"NATIVE_OPERATION_COMPLETED",wanted,true);
            active=pending=false;press=0;paused=true;clearOperation();return;
        }
        if(pending){
            if(current&&ack.state==1){
                wanted=reported=ack.fraction;pending=false;active=true;paused=false;
                io.receipt(request,"NATIVE_PAUSE_CONFIRMED",wanted,true);
            }else{if(!command(io,1))release(io,"CLAIM_SIGNAL_REFUSED");return;}
        }
        if(!current||ack.state!=1){release(io,"LUA_ACK_STALE_OR_RELEASED");return;}
        wanted=ack.fraction;
        if(reports<16&&std::fabs(wanted-reported)>=.1f){
            reported=wanted;++reports;io.receipt(request,"LUA_PROGRESS_READBACK",wanted,true);
        }
        const bool held=hand.grip>=.30f&&hand.press;
        const bool wasPaused=paused;
        if(!held){paused=true;press=0;}
        else if(!paused&&hand.press!=press){paused=true;press=0;}
        door_hold65::Operation op{request,hand,now,paused};
        if(paused&&held){
            door_hold65::Resume resume{};
            if(door_hold65::resume.read(resume)&&door_hold65::resumeMatches(resume,op,hand,now)){
                paused=false;press=hand.press;op.paused=false;
            }
        }
        if(!door_hold65::operation.publish(op)){release(io,"SESSION_PUBLICATION_REFUSED");return;}
        if(wasPaused!=paused)io.receipt(request,paused?"GRIP_RELEASE_PAUSE_REQUEST":"SAME_DOOR_GRIP_RESUME_REQUEST",wanted,true);
        if(!command(io,paused?4:3))release(io,"PLAY_PAUSE_SIGNAL_REFUSED");
    }
};
}
