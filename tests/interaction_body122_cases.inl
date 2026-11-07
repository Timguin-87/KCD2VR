{
    cc::Event drain{};while(cc::pop(drain)){}
    std::array<unsigned char,0x40> target{},bodyObject{},foreign{};
    put(target.data(),base+0x4072C78);put(target.data()+0xC,std::uint32_t(987));put(bodyObject.data(),base+0x123456);
    diagnostics::lab::Set(diagnostics::lab::Key::F5,1);cc::select(true,hostTickCount64);
    hostRetailVerified112=true;g_interaction_body_contract122=base;
    WatchInteractionBody122(target.data(),987,42,nullptr);
    ck(cc::pop(drain)&&drain.kind==cc::Kind::InteractionBody&&drain.flags==0,"F5 records a target watch even before any native body getter call");
    ObserveInteractionBody122(target.data(),bodyObject.data(),base+0x37CFD28);
    ck(cc::pop(drain)&&drain.flags==1&&drain.sequence==42&&drain.key[0]==reinterpret_cast<std::uintptr_t>(bodyObject.data())&&drain.key[1]==0x123456,
        "native body return keeps exact request, target and returned identity");
    hostTickCount64+=11;ObserveInteractionBody122(foreign.data(),bodyObject.data(),base+0x37CFD28);
    ck(!cc::pop(drain),"unrelated entity physics returns are not polled or recorded");
    put(target.data()+0xC,std::uint32_t(988));ObserveInteractionBody122(target.data(),bodyObject.data(),base+0x37CFD28);
    ck(!cc::pop(drain),"reused target address with a different entity ID cannot inherit watch");put(target.data()+0xC,std::uint32_t(987));
    ObserveInteractionBody122(target.data(),nullptr,base+0x37CFD28);
    ck(cc::pop(drain)&&drain.flags==1&&!drain.key[0],"null native physics return is evidence, not a fabricated body");
    hostTickCount64+=3001;ObserveInteractionBody122(target.data(),bodyObject.data(),base+0x37CFD28);
    ck(!cc::pop(drain),"target watch expires without background entity polling");
    WatchInteractionBody122(target.data(),987,43,nullptr);cc::pop(drain);
    diagnostics::lab::Set(diagnostics::lab::Key::F5,0);ObserveInteractionBody122(target.data(),bodyObject.data(),base+0x37CFD28);
    ck(!cc::pop(drain),"leaving F5 reference revokes the passive body watch");
    static int getterCalls122=0;static void* getterValue122=nullptr;getterValue122=bodyObject.data();
    g_interaction_body_original122=+[](void*)->void*{++getterCalls122;return getterValue122;};
    ck(InteractionBodyDetour122(target.data())==bodyObject.data()&&getterCalls122==1,"body detour preserves exact return and calls original once even outside capture");
    getterValue122=nullptr;
    ck(InteractionBodyDetour122(target.data())==nullptr&&getterCalls122==2,"null original result is forwarded unchanged");// V147 targets the getter at the selected native action rather than waiting
    // for an unrelated native physics access. It never persists the body pointer.
    diagnostics::lab::Set(diagnostics::lab::Key::F5,1);hostTickCount64+=11;
    WatchInteractionBody122(target.data(),987,44,nullptr);cc::pop(drain);
    getterValue122=bodyObject.data();ProbeInteractionBody147(target.data(),987,44);
    ck(getterCalls122==3&&cc::pop(drain)&&drain.flags==2&&drain.sequence==44,
       "one selected target getter produces explicitly labelled read-only evidence");
    ProbeInteractionBody147(target.data(),987,44);
    ck(getterCalls122==3&&!cc::pop(drain),"same request cannot repeat the getter");
    WatchInteractionBody122(target.data(),987,45,nullptr);cc::pop(drain);
    put(target.data()+0xC,std::uint32_t(999));ProbeInteractionBody147(target.data(),987,45);
    ck(getterCalls122==3&&!cc::pop(drain),"replacement entity cannot be probed using an old request");
    put(target.data()+0xC,std::uint32_t(987));diagnostics::lab::Set(diagnostics::lab::Key::F5,0);
    ProbeInteractionBody147(target.data(),987,45);ck(getterCalls122==3,"F5 exit revokes selected getter permission");
    g_interaction_body_original122=nullptr;
    g_interaction_body_contract122=0;g_interaction_watch122.publish({});hostRetailVerified112=false;cc::select(false,hostTickCount64);
}
