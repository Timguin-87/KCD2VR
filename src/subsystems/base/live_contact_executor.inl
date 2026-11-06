            // R59.16C10R17 (review): C5235C sets the hit flag and the post reaches phase 16 INSIDE this call; the combo slot observer
            // (91115C) runs synchronously there, so the VR combo window must already see this hit. Restored if the hit is refused.
#include "../masterstrike_combos/live_lineage_owner.inl" // M2_OWNED_SOURCE
            {kcd2vr::combo_lineage19::OfferScope offer(kcd2vr::combo_lineage19::valid(comboOwner19)?&comboOwner19:nullptr);
                kcd2vr::geometry40c1::Scope injection(henryActor,details,target);
                injection.reactionZone=p.weaponItem?-1:kcd2vr::bodycombo59c7g::reactionZoneForMaterial(p.bodyMaterial,p.kind==1); // NR1
                serialEntered59=serialReservationC2.tryEnter();
                if(serialEntered59){hybridReservation54.nativeCallStarted();
                    survived=CallLiveHandler35(action,details);}}
            if(leased17>0.f){const float after17=F(hs,kAttackStrength);
                if(after17==leased17)(void)vrstate596::SetFloat(hs,kAttackStrength,prior17);else g_strength_consumed59c17.fetch_add(1,std::memory_order_relaxed);}

            if(!serialEntered59){g_drop_serial_claimed56.fetch_add(1,std::memory_order_relaxed);
                if(Allow(4,4))diagnostic_trace::Write("C59 GEOM_SKIP site=LIVE_HANDLER reason=RELEASE_REVOKED_BEFORE_NATIVE_ENTRY nativeCall=0 attempt=ROLLBACK_ON_EXIT");return;}
            if(!survived){g_live_fault178r35.store(true);g_stats[4].fetch_add(1,std::memory_order_relaxed);
                diagnostic_trace::Write("V66.178R35 GEOM_LIVE_FAULT (exception inside C5235C; live action route latched off; no direct fallback)");return;}
#include "../hybrid/live_delivery_counter.inl" // M2_OWNED_SOURCE
            // (R59.16C10R17: the combo window tick for this live hit was stored before the handler, see above.)
            g_stats[1].fetch_add(1,std::memory_order_relaxed);const auto& r=g_inject_result178r35;if(r.seen&&r.ok)g_stats[2].fetch_add(1,std::memory_order_relaxed);
            const auto completedNowC2=std::uint64_t(GetTickCount64());
            const auto receipt=PublishContact59c7g(p,r,completedNowC2,te,::kcd2vr::zone_d85::executedHenryZone(I(hs,kDefenseZone)),false,r.process==1,r.process==1,-1,I(ts,kActionPhaseProp39),hs); // D85 zone
#include "../masterstrike_combos/live_lineage_completed.inl" // M2_OWNED_SOURCE
