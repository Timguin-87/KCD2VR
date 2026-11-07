static void PhysPublishImpactEffort19(const PhysFrame6698& f,const PhysEquipment6698& equipment) noexcept {
    namespace mo=kcd2vr::motion59c4;
    const bool direct=mo::selected.load(std::memory_order_acquire)&&f.input.motionOwned59c5;
    const bool normal=!mo::selected.load(std::memory_order_acquire)&&CombatSynchronized59c10r9()&&
        ph::normal_intent_enabled_h30(f.input)&&!f.input.motionOwned59c5&&f.output.normalIntentH30[0].current;
    // A soft-recovery commit has no achieved-pose effort receipt. The original
    // checked impact may supply it later, under the SAME gesture/native cost
    // key. Never substitute controller speed or create another native debit.
    if((!direct&&!normal)||!f.input.physicalCombat177||
       !equipment.valid||!equipment.player||f.player!=equipment.player||f.request!=equipment.request||
       f.input.epoch!=equipment.request||f.input.bodyEpoch!=equipment.epoch||!f.input.tick||!f.input.sample||
       !equipment.tick||(f.input.tick>=equipment.tick?f.input.tick-equipment.tick:equipment.tick-f.input.tick)>100||
       equipment.count<0||equipment.count>int(equipment.items.size())||
       (f.input.haveWorld&&(f.publishWorldDeltaM122>.002||f.publishWorldDeltaRad122>.002||
           !std::isfinite(f.publishWorldDeltaM122)||!std::isfinite(f.publishWorldDeltaRad122)||
           !std::isfinite(f.world[7])||!std::isfinite(f.input.worldScale)||std::abs(double(f.world[7])-f.input.worldScale)>1e-5)))return;
    for(int side=0;side<2;++side){const auto& r=f.output.impactRecord178r30[side];const auto& p=f.profiles[side];
        const int item=equipment.heldIndex[side];
        if(!p.melee()||p.side!=side||!equipment.handKnown[side]||equipment.handKey[side]!=p.key||item<0||item>=equipment.count||
           !equipment.items[item].held||equipment.items[item].profile.key!=p.key||equipment.items[item].profile.generation!=p.generation)continue;
        if(!r.valid||!r.effortKnown19||!r.gesture59c3||!r.entityId||!r.collider||r.part<0||
           !std::isfinite(r.speed)||r.speed<.8||r.tick!=f.input.tick||r.sample!=f.input.sample||
           r.key!=p.key||r.generation!=p.generation||r.epoch!=f.input.bodyEpoch||r.request!=f.request||
           !f.output.contactKnown[side]||f.output.sweepDeferred115[side]||!ph::attack_authority55c1(f.input,f.output,side))continue;
        const mo::Key key{r.gesture59c3,r.key,r.generation,r.epoch,r.request,
            kcd2vr::contact176::recoveryEpoch55c1.load(std::memory_order_acquire),reinterpret_cast<std::uintptr_t>(f.player)};
        (void)kcd2vr::swing_effort59c14::ledger.publish(key,r.effortScale19,r.effortMass19,r.twoHands59c7g,r.tick);
    }
}
