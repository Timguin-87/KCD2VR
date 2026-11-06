        if(c.kind==ph::CommandKind::DropT1){
            // The original command stays queued through exact collider
            // retirement; the existing80/100ms history/command limits still
            // apply. Never obtain a fresh release from a late stale velocity.
            static heldT4::Owner retiringOwnerH21{};static std::uint64_t releaseSampleH21=0,releaseEpochH21=0;
            const auto testEpochH21=kcd2vr::collab_h21::throwEpoch.load(std::memory_order_acquire);
            if(!c.throwEpochH21||c.throwEpochH21!=testEpochH21||c.physicsEpochH21!=kcd2vr::collab_h21::epoch.load(std::memory_order_acquire)||!kcd2vr::weapon_release_t1::TestEnabledH21()){
                g_phys_commands6698.pop(c);g_weapon_release_pending_t1[c.side].store(0,std::memory_order_release);continue;
            }
            if(c.side==0&&kcd2vr::weapon_release_t1::TestEnabledH21()&&NativeHeldBusyT4(0)){
                heldT4::Owner ownerH21{};
                if(heldT4::ReadRetainedOwner(ownerH21)&&ownerH21.key==c.key&&ownerH21.generation==c.generation&&
                   ownerH21.request==c.epoch&&ownerH21.epoch==c.bodyEpoch&&ownerH21.side==0){
                    if(releaseSampleH21!=c.sample||releaseEpochH21!=testEpochH21){
                        retiringOwnerH21=ownerH21;releaseSampleH21=c.sample;releaseEpochH21=testEpochH21;NativeHeldInvalidateT4();
                    }
                    break;
                }
            }
            bool retiredReleaseH21=false;
            if(c.side==0&&releaseSampleH21==c.sample&&releaseEpochH21==testEpochH21){
                heldT4::Achieved retiredH21{};
                retiredReleaseH21=heldT4::Read(retiredH21)&&retiredH21.owner==retiringOwnerH21&&retiredH21.colliderRetired;
            }
            g_phys_commands6698.pop(c);PhysCancelNative6698();
            if(c.side==0&&(NativeHeldBusyT4(0)||(g_native_held_release_inhibit_t4.load(std::memory_order_acquire)&&!retiredReleaseH21))){
                g_weapon_release_pending_t1[0].store(0,std::memory_order_release);continue;}
            namespace w=kcd2vr::weapon_release_t1;w::Request release{};w::Sample sample{};
            const int index=e.heldIndex[c.side];
            if(w::TestEnabledH21()&&w::Mode()==1&&index>=0&&index<e.count&&e.items[index].dropUnitT1&&e.handKey[c.side]==c.key&&
               e.items[index].profile.generation==c.generation&&frame.input.tracked[c.side]&&
               frame.input.intentTracked[c.side]&&std::isfinite(frame.input.grip[c.side])&&frame.input.grip[c.side]<.25f&&
               g_weapon_release_sample_t1[c.side].read(sample)){
                release.manager=reinterpret_cast<std::uintptr_t>(e.manager);release.player=reinterpret_cast<std::uintptr_t>(e.player);
                release.character=frame.correlation126[c.side].stamp.actor;release.key=c.key;release.generation=c.generation;
                release.bodyEpoch=c.bodyEpoch;release.requestEpoch=c.epoch;release.sample=c.sample;release.tick=c.tick;
                release.throwEpochH21=c.throwEpochH21;
                release.physicsEpochH21=c.physicsEpochH21;
                release.side=c.side;release.classId=e.items[index].profile.classId;release.entity=e.items[index].profile.entity;release.attachment=sample.attachment;
                if(w::matches(sample,release,now)){
                    sample.motion.velocity(now,true,release.linear,release.angular);release.motion=sample.motion.pair;
                    release.lastAllowed=sample.motion.newer.world;release.motionTick=sample.motion.newer.tick;
                    (void)kcd2vr::action_time_owner59c11::DropWeaponT1(release);
                }else if(PhysHolsterCaptureD48(now))diagnostic_trace::Write("T1 WEAPON_RELEASE REFUSED reason=STALE_OR_CHANGED_ALLOWED_OBJECT_HISTORY sheathFallback=NONE");
            }else if(PhysHolsterCaptureD48(now))diagnostic_trace::Write("T1 WEAPON_RELEASE REFUSED reason=CURRENT_PRIMARY_RELEASE_OR_HISTORY_UNAVAILABLE sheathFallback=NONE");
            g_weapon_release_pending_t1[c.side].store(0,std::memory_order_release);break;
        }
        if(c.kind==ph::CommandKind::Draw||c.kind==ph::CommandKind::Holster){
            g_phys_commands6698.pop(c);PhysCancelNative6698();bool enteredD48=false;
            const bool returned=PhysEquipmentCommand6698(e,c,&enteredD48);
            // D63: an entered native DRAW from a positively non-held item owns
            // extraction motion. A failed/unentered request and an already-held
            // weapon cannot disarm combat. The producer additionally requires a
            // current same-key held profile and a newer XR source before this
            // ticket is admitted; native return alone is not that ACK.
            if(c.kind==ph::CommandKind::Draw&&enteredD48&&e.handKnown[c.side]&&e.handKey[c.side]==0)
                kcd2vr::draw_signal_d63::publish({0,c.key,c.generation,c.bodyEpoch,c.epoch,c.sample,c.tick,c.side,c.drawSourceD63});
            if(!returned&&!enteredD48&&PhysCancelUnenteredEquipmentH36(e,c))PhysRejectUnenteredEquipmentD48(c);
            // Refresh only the native facts changed by this draw. The native
            // return is not a held-item ACK and never grants a physical pose.
            if(returned&&c.kind==ph::CommandKind::Draw){
                const auto publishDrawFacts=[&](){
                    const auto currentOwner=[&](){return g_weapon_control_mode.load()>=4&&
                        !animation_filter::nativeBodyOwned112.load()&&!NativePassengerCurrentA34(GetTickCount64())&&
                        !g_phys_fault6698.load()&&g_gameplay_allowed.load()&&!diagnostics::lab::NativeReference()&&
                        e.manager==g_henry_item_attach_manager.load()&&e.request==g_weapon_request_epoch6694.load()&&
                        e.epoch==g_pose_epoch6689.load()&&TrackingHeadUsable111(GetTickCount64());};
                    if(!currentOwner())return;
                    const auto membership=g_left_native_membership_h36.load(std::memory_order_acquire);
                    const auto refreshedAt=std::uint64_t(GetTickCount64());PhysEquipment6698 refreshed{};
                    if(!PhysRefreshEquipment103(refreshed,refreshedAt)||!refreshed.valid||
                       refreshed.manager!=e.manager||refreshed.player!=e.player||
                       refreshed.request!=e.request||refreshed.epoch!=e.epoch||!currentOwner()||
                       membership!=g_left_native_membership_h36.load(std::memory_order_acquire))return;
                    refreshed.leftNativeMembershipH36=membership;
                    g_phys_equipment6698.publish(refreshed);
                    for(int i=0;i<8;++i)g_phys_relevant_id101[i].store(i<refreshed.count?refreshed.items[i].profile.entity:0,std::memory_order_relaxed);
                };
                publishDrawFacts();
            }
            if(PhysHolsterCaptureD48(now)){char msg[256]{};std::snprintf(msg,sizeof(msg),"V66.121 EQUIPMENT_REQUEST kind=%s side=%d set=%d key=%016llX returned=%d acknowledged=NOT_YET",
                c.kind==ph::CommandKind::Draw?"DRAW":"HOLSTER",c.side,c.set,(unsigned long long)c.key,returned);diagnostic_trace::Write(msg);}break;}
