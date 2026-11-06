        case ph::CommandKind::BowBegin:
            if(c.side==1&&frame.profiles[1].bow()&&!n.bowHeld&&
               !frame.output.handlingOnly55c1[1]&&!frame.output.displayRejoinR13[1]){
                n.key=c.key;n.bowGeneration175=c.generation;n.bowBeginSample175=c.sample;n.nativeChargeCap=0;n.bowChargeObserved114=false;
                n.physicalBow175=!frame.input.nativeBowTrigger126;
                g_phys_charge_action175.store(nullptr,std::memory_order_release);
                g_phys_charge_player175.store(n.physicalBow175?e.player:nullptr,std::memory_order_release);
                n.bowHeld=true; // includes a possible synchronous native charge callback
                called=PhysBowCall6698(e.player,0);n.bowHeld=called;n.bowBeginFrame=nativeFrame;
                if(!called){n.physicalBow175=false;g_phys_charge_player175.store(nullptr,std::memory_order_release);}
            }break;
        case ph::CommandKind::BowFire:
            // D44: a queued same-item release cannot borrow a new draw's
            // display-only grip while collision acquisition is pending.
            if(frame.output.handlingOnly55c1[1]||frame.output.displayRejoinR13[1]){PhysCancelNative6698(false);break;}
            if((n.physicalBow175||ph::bow_requires_charge_ack175(frame.input))&&!n.bowChargeObserved114){diagnostic_trace::Write("V66.116 BOW_FIRE rejected=NO_NATIVE_CHARGE_ACK");PhysCancelNative6698(false);break;}
            if(PhysBowReleaseReady175(n,c,frame,now,g_right_valid.load(),g_left_valid.load())){
                namespace aim=kcd2vr::bow_physical_aim_d65c;
                aim::Snapshot shot{};shot.player=e.player;shot.key=c.key;shot.generation=c.generation;
                shot.body=c.bodyEpoch;shot.request=c.epoch;shot.sample=c.sample;shot.tick=now;
                shot.tracking=kcd2vr::tracking59c6::epoch.load(std::memory_order_acquire);
                void* charge=g_phys_charge_action175.load(std::memory_order_acquire);
                if(n.physicalBow175&&c.aimWorldD65C&&PhysVft6698(charge,0x47083C0)){
                    (void)Read(static_cast<unsigned char*>(charge)+0x60,shot.ownerInterface);
                    const double dir[]{c.aimDirWorldD65C.x,c.aimDirWorldD65C.y,c.aimDirWorldD65C.z};
                    const double up[]{c.aimUpWorldD65C.x,c.aimUpWorldD65C.y,c.aimUpWorldD65C.z};
                    const double pos[]{c.aimNockWorldD65C.x,c.aimNockWorldD65C.y,c.aimNockWorldD65C.z};
                    for(int i=0;i<3;++i){shot.dir[i]=dir[i];shot.up[i]=up[i];shot.nock[i]=pos[i];}
                }
                aim::ReleaseScope release(shot);
                called=PhysBowCall6698(e.player,1);const bool boundAim=release.finish(called);
                if(n.physicalBow175&&ranged_options_d65c::bowPhysicalAim.load()&&
                   (diagnostics::capture168::active(diagnostics::capture168::Domain::Combat,now)||
                    diagnostics::capture168::active(diagnostics::capture168::Domain::Reload,now))){
                    char row[192]{};std::snprintf(row,sizeof(row),"D65 BOW_RELEASE nativeCallReturned=%d exactFiringAction=%d action=%p release=%llu",
                        int(called),int(boundAim),release.value.action,(unsigned long long)release.value.serial);diagnostic_trace::Write(row);}
                n.bowHeld=false;n.physicalBow175=false;
                g_phys_charge_action175.store(nullptr,std::memory_order_release);
                g_phys_charge_player175.store(nullptr,std::memory_order_release);
            }break;
        case ph::CommandKind::BowCancel:PhysCancelNative6698(false);called=true;break;
