struct GestureFrame59c5 {bool valid=false;XrPose head{};ControllerState hands[2]{};}; // hands[0]=right, [1]=left
static GestureFrame59c5 g_gesture_frame59c5{};
static kcd2vr::gesture_talk59c5::Output PrepareGestureTalkH4(const native_vr_pad_premap::Context& padCtx,bool trackedGameplay) noexcept
{
                namespace gt=kcd2vr::gesture_talk59c5;static gt::State gesture59c5{};
                const auto gNow=std::uint64_t(GetTickCount64());
                const bool chatOpen=padCtx.quick_talk_open54;
                const bool gameplay=!padCtx.vr_menu&&!padCtx.theater&&!padCtx.system_menu&&!padCtx.dialogue_navigation&&
                    !padCtx.semantic_minigame&&!padCtx.apse_ui&&(!padCtx.back_context||chatOpen)&&trackedGameplay;
                gt::Config gestureConfig47{};gestureConfig47.holdPoint=true;gestureConfig47.manualPoint54=true;gestureConfig47.requireIndexOffTrigger=true;gestureConfig47.pointHoldMs=0;gestureConfig47.pointMinUp=-.90;gestureConfig47.automaticVoice=false; // R93R1: optional immediate index-point chooser; native choices only, original cooldown retained
                kcd2vr::grasp13::PendingUse55 pendingPickupD78{};
                const bool hasPendingPickupD78=kcd2vr::action_time_owner59c11::ReadPendingUse55(pendingPickupD78)&&
                    pendingPickupD78.serial&&pendingPickupD78.character&&pendingPickupD78.epoch&&
                    pendingPickupD78.requestedAt&&gNow>=pendingPickupD78.requestedAt&&gNow-pendingPickupD78.requestedAt<=1500;
                kcd2vr::hand_only_door_h40::Admission doorTalkAdmissionH40[2];
                gt::Hand hands[2]{};
                if(trackedGameplay&&g_gesture_talk59c5.load(std::memory_order_relaxed)&&g_gesture_frame59c5.valid){const auto& head_pose=g_gesture_frame59c5.head;
                    // OpenXR look direction = head quaternion applied to (0,0,-1); keep its horizontal part (Y is up).
                    const float qx=head_pose.qx,qy=head_pose.qy,qz=head_pose.qz,qw=head_pose.qw;
                    float fx=-2.f*(qw*qy+qx*qz),fz=-1.f+2.f*(qx*qx+qy*qy);
                    const float fl=std::sqrt(fx*fx+fz*fz);if(fl>1e-4f){fx/=fl;fz/=fl;}else{fx=0.f;fz=-1.f;} // horizontal look direction
                    for(int side=0;side<2;++side){const auto& c=g_gesture_frame59c5.hands[side];
                        auto& h=hands[side];h.valid=c.active&&c.aim_active&&
                            !(kcd2vr::corpse_grip_cg1::claims.reserved()&(1u<<side))&&
                            !(hasPendingPickupD78&&pendingPickupD78.side==side)&&doorTalkAdmissionH40[side].take(1u<<side);
                        // DS-2026-10-23 (Miki: near someone the GRAB owns the hand, not the quick talk).
                        // Only a HELD grip on a living NPC suppresses the talk - a mere brush keeps it
                        // (the Codex T27 finding 6: no incidental-contact invalidation).
                        if(h.valid&&c.grip>=.60f&&kcd2vr::grab_over_talk_ds::LivingNear(side,gNow,250))h.valid=false;
                        // A support hand has no native held-item WUID. It is
                        // still occupied by the committed two-handed solve.
                        h.empty=frik_pose_probe::ExactHandItemState(side)==0&&
                            frik_pose_probe::ReadPhysicalHandHeld55(side,gNow)==0;
                        // An already committed holster grasp keeps this Grip
                        // while withdrawing; it is not a new QuickTalk point.
                        if(c.grip>.55f&&frik_pose_probe::ReadHolsterGripOwnerR15(side,gNow))h.valid=false;
                        h.grip=c.grip;h.trigger=c.trigger;h.triggerTouch=c.trigger_touch;
                        const float rx=c.aim_pose.pos_x-head_pose.pos_x,ry=c.aim_pose.pos_y-head_pose.pos_y,rz=c.aim_pose.pos_z-head_pose.pos_z;
                        const auto& aq=c.aim_pose;
                        const double ax=-2.0*(aq.qw*aq.qy+aq.qx*aq.qz),ay=2.0*(aq.qw*aq.qx-aq.qy*aq.qz),az=-1.0+2.0*(aq.qx*aq.qx+aq.qy*aq.qy);
                        h.outwardR26=gt::OutwardPointR26(rx,ry,rz,ax,ay,az,fx,fz);
                        h.forward=double(rx*fx+rz*fz);h.up=double(ry);}
                }
                // Observe/query an NPC only while a current free hand is in
                // the existing pointing envelope. Idle gameplay adds no script
                // calls; pending pickup/door/holster owners cannot request one.
                const bool pointRequestedD78=g_gesture_talk59c5.load(std::memory_order_acquire)&&gameplay&&!chatOpen&&
                    (gt::retainedPointH4(hands[0],gestureConfig47)||gt::retainedPointH4(hands[1],gestureConfig47));
                const bool pointTargetD78=kcd2vr::quick_talk_target_d78::Prepare(
                    pointRequestedD78&&kcd2vr::misc_gestures_d59::Ensure(gNow),gNow);
                // The native-open chooser keeps its original owner. A new
                // point needs this NPC's enabled descriptor; raised battle cry
                // retains its existing native route and does not need a target.
                for(auto& hand:hands)hand.outwardR26=hand.outwardR26&&(chatOpen||pointTargetD78);
                const bool inCombat=combat_diag_probe::HenryInCombat178r31(gNow)==1;
                static unsigned previousChoices54=0;
                const unsigned choices54=(g_gesture_frame59c5.hands[0].a_down?1u:0u)|(g_gesture_frame59c5.hands[0].b_down?2u:0u)|
                    (g_gesture_frame59c5.hands[1].x_down?4u:0u)|(g_gesture_frame59c5.hands[1].y_down?8u:0u);
                const bool selected54=chatOpen&&(choices54&~previousChoices54)!=0;previousChoices54=choices54;
                static std::uint64_t selfPatSeen54=0,selfPatTick54=0;
                kcd2vr::self_contact54::Event selfPat54{};
                bool patFired54=false;
                if(gestureConfig47.automaticVoice&&frik_pose_probe::ReadSelfPat54(selfPat54,gNow)&&
                   (selfPat54.serial!=selfPatSeen54||selfPat54.tick!=selfPatTick54)){
                    selfPatSeen54=selfPat54.serial;selfPatTick54=selfPat54.tick; // context refusal cannot defer a gesture into a later menu/fight
                    patFired54=gt::battleCryContact54(gesture59c5,gNow,true,
                        g_gesture_talk59c5.load(std::memory_order_relaxed)&&gameplay&&!gt::combatChord(hands),inCombat,gestureConfig47);
                }
                auto g=gt::update(gesture59c5,gNow,hands,g_gesture_talk59c5.load(std::memory_order_relaxed)&&gameplay,inCombat,chatOpen,gestureConfig47,selected54);
                if(patFired54)g.fired=gt::Kind::BattleCry;
                frik_pose_probe::PublishQuickTalkOwnerH4(g.claimMaskH4,gNow);
                if(g.fired!=gt::Kind::None||g.chatTimeout){char m[200]{};std::snprintf(m,sizeof(m),"R59.16C5 GESTURE_TALK %s inCombat=%d chatOpen=%d route=NATIVE_QUICK_TALK option=%s",
                    g.chatTimeout?"CHAT_DID_NOT_OPEN":g.fired==gt::Kind::Talk?"POINT_TALK":g.fired==gt::Kind::Taunt?"POINT_TAUNT":patFired54?"CONTACT_BATTLE_CRY":"RAISE_BATTLE_CRY",inCombat?1:0,chatOpen?1:0,
                    g.chatTimeout?"NONE":g.fired==gt::Kind::Taunt?"3_X":g.fired==gt::Kind::BattleCry?"2_B":"MENU_ONLY");diagnostic_trace::Write(m);}
                return g;
            }

