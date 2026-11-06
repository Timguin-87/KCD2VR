#pragma once

namespace kcd2vr::gesture_recovery_h30 {
// Recovery may retain the tracked gesture, never an unchecked contact or
// defensive pose. Other hands retain their own independently checked output.
template<class Output> void MaskRecoveredContacts(Output& output) noexcept {
    if(!output.handlingOnly55c1[0]&&!output.handlingOnly55c1[1])return;
    output.guard117=false;
    for(int side=0;side<2;++side)if(output.handlingOnly55c1[side]){
        output.guardBySide30c1[side]=false;output.impactRecord178r30[side]={};
        output.impactSpeed[side]=0;output.impactEventTick178r24[side]=0;
        // D44: retain the copied query outcome for the existing F2 motor
        // diagnostic. Actor/plane/contact permission is still retired here;
        // a reason or limited-prefix count is never a collision receipt.
        const auto coverageD44=output.contact119[side].coverage169r2;
        const bool limitedD44=output.contact119[side].limited115;
        output.contact119[side]={};output.contact119[side].known=false;
        output.contact119[side].coverage169r2=coverageD44;
        output.contact119[side].limited115=limitedD44;
    }
}
// This is a semantic retirement decision, not contact permission. A checked
// occurrence keeps its original age/body/part checks at drain. A display-only
// fallback with a fresh explicit tracked-intent owner does not replace that
// occurrence or cancel the native action that already owns it.
template<class Frame,class Authority>
bool Retires(const Frame& before,const Frame& current,Authority&& authority) noexcept {
    for(int side=0;side<2;++side)if(
        (current.output.solverOwnedT4[side]&&!before.output.solverOwnedT4[side])||
        (current.output.ownShieldBlocked60[side]&&!before.output.ownShieldBlocked60[side]))return true;
    // The armed native-input producer owns side0. Profile.primary describes
    // a measured grip socket, not which hand attacks: a left torch/shield can
    // legitimately have primary=true as well.
    const auto semantic=[](const Frame& f,int side) noexcept {
        return quarantined_intent_h8(f.input,f.output,f.profiles[side],side)?
            f.output.intentQuarantineH8[side].owner:f.output.normalIntentH30[side];};
    const auto owns=[&](const Frame& f,int side) noexcept {
        return authority(f.input,f.output,f.profiles[side],side)||quarantined_intent_h8(f.input,f.output,f.profiles[side],side);};
    const auto& oldRight=before.profiles[0];const auto& right=current.profiles[0];
    const auto oldIntent=semantic(before,0),intent=semantic(current,0);
    const bool retainedRight=oldIntent.current&&intent.current&&oldRight.side==0&&right.side==0&&
        oldRight.key==right.key&&oldRight.generation==right.generation&&oldRight.classId==right.classId&&
        oldIntent.key==oldRight.key&&oldIntent.generation==oldRight.generation&&
        oldIntent.bodyEpoch==before.input.bodyEpoch&&oldIntent.request==before.input.epoch&&
        oldIntent.key==intent.key&&oldIntent.generation==intent.generation&&
        oldIntent.bodyEpoch==intent.bodyEpoch&&oldIntent.request==intent.request&&
        owns(before,0)&&owns(current,0);
    const auto offhandSource=[](const Frame& f) noexcept {
        const auto& p=f.profiles[1];const auto& in=f.input;
        return p.side==1&&p.key&&p.generation&&(p.torch40()||p.shield176())&&
            !f.output.normalIntentH30[1].current&&in.tracked[1]&&in.intentTracked[1]&&
            in.modelIntentSourceH30[1]&&model_intent_selected_h30(in,1)&&model_intent_current_h30(in,1)&&
            finite(in.hands[1])&&finite(in.intentHands[1]);
    };
    const bool sameOffhand=before.profiles[1].key==current.profiles[1].key&&
        before.profiles[1].generation==current.profiles[1].generation&&
        before.profiles[1].classId==current.profiles[1].classId&&
        before.profiles[1].side==current.profiles[1].side;
    const bool priorOffhand=offhandSource(before),currentOffhand=offhandSource(current);
    // A hard source/item loss AFTER the retained soft edge must still retire
    // once. It cannot hide behind an already-active display recovery flag.
    if(retainedRight&&(before.output.handlingOnly55c1[1]||current.output.handlingOnly55c1[1])&&
       priorOffhand&&(!sameOffhand||!currentOffhand))return true;
    for(int side=0;side<2;++side){
        const auto prior=semantic(before,side),next=semantic(current,side);
        const bool live=next.current&&owns(current,side);
        if(prior.current&&(!live||prior.key!=next.key||prior.generation!=next.generation||
            prior.bodyEpoch!=next.bodyEpoch||prior.request!=next.request))return true;
        // Only an independent, same-item left torch/shield display edge may
        // keep the existing right attack ancestry. It grants no contact or new
        // cost permission; both hands still need fresh raw tracking sources.
        const bool independentOffhand=side==1&&retainedRight&&sameOffhand&&priorOffhand&&currentOffhand;
        // Legacy/no-proof and melee-offhand fallbacks remain fail-closed.
        if(current.output.handlingOnly55c1[side]&&!before.output.handlingOnly55c1[side]&&!live&&!independentOffhand)return true;
    }
    return false;
}
}
