// Production return-value adapter with explicit synthetic owner records.
{
    const auto savedFrame121=frame;void* savedSoul121=nullptr;std::uint64_t savedLeft121=0;
    Read(player.data()+0x668,savedSoul121);Read(hands.data()+0x268,savedLeft121);
    std::array<unsigned char,8> rpg121{},soul121{};put(rpg121.data(),base+0x3AE4410);
    put(player.data()+0x668,soul121.data());put(hands.data()+0x268,std::uint64_t(99));
    g_bow_zoom_contract121=base;hostRetailVerified112=true;g_weapon_control_mode=6;
    diagnostics::lab::Set(diagnostics::lab::Key::F3,0);g_gameplay_allowed=g_head_valid=g_left_valid=g_right_valid=true;
    g_phys_armed6698=true;g_phys_fault6698=false;
    frame.input.allowed=frame.input.bowActions=frame.input.tracked[0]=frame.input.tracked[1]=true;
    frame.request=g_weapon_request_epoch6694.load();frame.input.bodyEpoch=g_pose_epoch6689.load();
    frame.input.tick=hostTickCount64;frame.input.sample++;
    frame.profiles[1].key=99;frame.profiles[1].side=1;frame.profiles[1].primary=true;frame.profiles[1].classId=9;
    frame.output.active[1]=frame.output.owned[1]=true;g_phys_frame6698.publish(frame);
    auto zoom121=[&](std::uintptr_t caller=0){return BowZoomResult121(caller?caller:base+0x10610B9,rpg121.data(),soul121.data(),99,.4f);};
    ck(zoom121()==0,"only Henry physical bow camera consumes zero aim zoom");
    frame.profiles[1].classId=10;g_phys_frame6698.publish(frame);
    kcd2vr::ranged_options_xr32::crossbowNoZoom=false;
    ck(zoom121()==.4f,"crossbow optional native zoom remains available");
    kcd2vr::ranged_options_xr32::crossbowNoZoom=true;
    ck(zoom121()==0,"crossbow exact Henry held key consumes zero camera zoom");
    for(int kind32:{14,15}){frame.profiles[1].classId=kind32;g_phys_frame6698.publish(frame);ck(zoom121()==0,"medium/heavy crossbows share qualified camera policy");}
    ck(zoom121(base+0x10610BA)==.4f,"crossbow never changes a noncamera RPG consumer");
    frame.profiles[1].classId=13;g_phys_frame6698.publish(frame);
    ck(zoom121()==.4f,"crossbow preference does not broaden to rifle");
    frame.profiles[1].classId=9;g_phys_frame6698.publish(frame);

    frame.output.active[1]=frame.output.owned[1]=false;g_phys_frame6698.publish(frame);
    ck(zoom121()==0,"release-to-holster transition does not reintroduce zoom while native bow is still held");
    frame.output.active[1]=frame.output.owned[1]=true;g_phys_frame6698.publish(frame);
    ck(zoom121(base+0x10610BA)==.4f,"other RPG consumers preserve calculated zoom");
    ck(BowZoomResult121(base+0x10610B9,rpg121.data(),entity.data(),99,.4f)==.4f,"NPC soul cannot acquire camera override");
    ck(BowZoomResult121(base+0x10610B9,rpg121.data(),soul121.data(),77,.4f)==.4f,"wrong item key cannot acquire zoom override");
    g_weapon_control_mode=0;ck(zoom121()==.4f,"native F12 baseline retains native camera zoom");g_weapon_control_mode=7;
    ck(zoom121()==0,"F12 prediction comparison shares physical bow zoom policy");
    for(auto* blocker:{&g_ctx_dialogue,&g_ctx_cutscene,&g_ctx_menu,&g_ctx_scripted}){
        *blocker=true;ck(zoom121()==.4f,"native scene/UI ownership preserves FOV");*blocker=false;
    }
    g_gameplay_allowed=false;ck(zoom121()==.4f,"Theater gameplay veto preserves FOV");g_gameplay_allowed=true;
    diagnostics::lab::Set(diagnostics::lab::Key::F3,1);ck(zoom121()==.4f,"native reference veto preserves FOV");diagnostics::lab::Set(diagnostics::lab::Key::F3,0);
    hostRetailVerified112=false;ck(zoom121()==.4f,"unverified SHA cannot modify native zoom");hostRetailVerified112=true;
    g_left_valid=false;ck(zoom121()==0,"current HMD camera remains unzoomed while a bow hand is temporarily unavailable");g_left_valid=true;
    frame.input.bowActions=false;g_phys_frame6698.publish(frame);ck(zoom121()==0,"manual reload draw gate cannot toggle the HMD camera zoom");
    hostTickCount64+=81;ck(zoom121()==.4f,"stale physical ownership cannot modify zoom");hostTickCount64-=81;
    put(hands.data()+0x268,std::uint64_t(0));ck(zoom121()==.4f,"live unequip defeats still-fresh frame");
    put(hands.data()+0x268,savedLeft121);put(player.data()+0x668,savedSoul121);frame=savedFrame121;g_phys_frame6698.publish(frame);
    g_bow_zoom_contract121=0;hostRetailVerified112=false;g_weapon_control_mode=6;
}
