// The only H40 motion producer: one completed/restored pose transaction.
// Request is captured at BeginCoherent6689, never sampled after the solve.
static void PublishHandOnlyDoorH40(const CoherentTransaction6689& tx,const CommittedFrame6695& packet) noexcept {
    namespace h=kcd2vr::hand_only_door_h40;h::Frame f{};
    const auto now=std::uint64_t(GetTickCount64());PhysEquipment6698 e{};
    f.tick=tx.tracking.tick;f.body=tx.epoch;f.request=tx.doorRequestH40;f.sample=tx.tracking.sequence;
    f.tracking=tx.tracking.source_epoch;f.policy=tx.policyGeneration57;f.actor=packet.doorPlayer38;f.character=packet.stamp.actor;
    if(!f.actor||!Read(reinterpret_cast<void*>(f.actor+0x0C),f.player)||!f.player||
       !g_phys_equipment6698.read(e)||!e.valid||!h::Fresh(e.tick,now,100)||e.epoch!=f.body||e.request!=f.request||
       e.manager!=g_henry_item_attach_manager.load()||!e.player){h::RevokeCompleted();return;}
    f.manager=reinterpret_cast<std::uintptr_t>(e.manager);f.nativePlayer=reinterpret_cast<std::uintptr_t>(e.player);
    f.permitted=tx.committed&&tx.restored&&CoherentOwnerCurrent57(tx)&&h::Fresh(f.tick,now)&&
       f.body==g_pose_epoch6689.load()&&f.request==g_weapon_request_epoch6694.load()&&
       !(f.policy&1)&&f.policy==g_pose_policy_generation57.load()&&f.tracking==kcd2vr::tracking59c6::epoch.load()&&
       tx.tracking.allowed&&tx.tracking.head_valid&&!tx.tracking.head_held&&g_gameplay_allowed.load()&&
       !tx.reloadWorkingHandXR34&& // CL-D: no door_use46 switch
       !g_ctx_menu.load()&&!g_ctx_dialogue.load()&&!g_ctx_cutscene.load()&&!g_ctx_reading.load()&&!g_ctx_scripted.load()&&
       !animation_filter::nativeBodyOwned112.load()&&!animation_filter::NativeKickActive175();
    for(int side=0;side<2;++side){
        f.grip[side]=side?tx.tracking.left_grip:tx.tracking.right_grip;
        const float trigger=side?tx.tracking.left_trigger:tx.tracking.right_trigger;
        QuatT28 palm=packet.allowed[side];
        if(packet.propRequestedValid5916c1[side]){
            // Drive from this completed frame's pre-contact controller request.
            // Keep the same anatomical palm offset; a panel's achieved contact
            // correction must not become the input that tries to move it.
            const auto offset=packet.palmValid59c9[side]?
                QRotateV(QConj(QFromQuatT(palm)),Sub(packet.palm59c9[side],FromT(palm))):V3{};
            palm=packet.propRequested5916c1[side];
            ToT(palm,Add(FromT(palm),QRotateV(QFromQuatT(palm),offset)));
        }else if(packet.palmValid59c9[side])ToT(palm,packet.palm59c9[side]);
        const auto world=FromT(HandWorld6694(palm,packet.world));f.point[side][0]=world.x;f.point[side][1]=world.y;f.point[side][2]=world.z;
        f.available[side]=packet.valid[side]&&(side?!tx.tracking.left_held:!tx.tracking.right_held)&&
           e.handKnown[side]&&!e.handKey[side]&&ExactHandItemState(side)==0&&ReadPhysicalHandHeld55(side,now)==0&&
           trigger<.10f&&!tx.physicalOutput.support[side]&&!tx.physicalOutput.solverOwnedT4[side]&&
           (tx.doorInputOwnerMaskH40[side]&~1u)==0;
    }
    // A failed/overlapping publication never leaves an earlier hand current.
    if(!h::Valid(f,now)||f.request!=g_weapon_request_epoch6694.load()||f.body!=g_pose_epoch6689.load())h::RevokeCompleted();
    else h::PublishCompleted(f);
}
