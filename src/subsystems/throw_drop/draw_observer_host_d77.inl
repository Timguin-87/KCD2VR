// D77 exact actual invocation only. This observer cannot admit/reject a draw.
// All input identities are copied from the already-validated equipment command.
template<class Invoke> static bool PhysObserveDrawInvokeD77(const PhysEquipment6698& e,const ph::Command& c,Invoke& original) {
    namespace d=kcd2vr::diagnostics::draw_native_d77;
    if(!DrawNativeActiveD77())return original();
    const PhysEquipmentItem6698* item=nullptr;
    for(int i=0;i<e.count&&i<int(e.items.size());++i)
        if(e.items[i].profile.key==c.key&&e.items[i].profile.generation==c.generation){item=&e.items[i];break;}
    if(!item||!item->item)return original();
    const auto cls=item->profile.classId;
    if(!(cls==1||cls==2||cls==4||cls==16||item->profile.bow()||item->profile.crossbowD72()))return original();
    d::Draw draw{};draw.session=d::recorder.session.load(std::memory_order_acquire);
    draw.tick=GetTickCount64();draw.sourceTick=c.tick;draw.key=c.key;draw.body=c.bodyEpoch;draw.request=c.epoch;
    draw.sample=c.sample;draw.frame=ReadCompletedPublicationT23();draw.generation=c.generation;
    draw.trackingEpoch=tracking59c6::epoch.load(std::memory_order_acquire);draw.side=c.side;draw.classId=cls;
    draw.player=reinterpret_cast<std::uintptr_t>(e.player);draw.manager=reinterpret_cast<std::uintptr_t>(e.manager);
    draw.character=g_player_char37.load(std::memory_order_acquire);draw.item=reinterpret_cast<std::uintptr_t>(item->item);
    const auto occurrence=d::recorder.arm(draw);
    if(!occurrence)return original();
    DrawObserveExistingD77(occurrence); // exact pre-existing entity witness, not a spawn/render claim
    return d::invokeScoped(occurrence,[](void* context)->bool{return (*static_cast<Invoke*>(context))();},&original);
}
