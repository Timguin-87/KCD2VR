// Invisible inventory holsters. No attachment creation, bone lookup or loadout-slot writes.
static void PhysPublishSupplyT2(const PhysFrame6698& frame,const PhysEquipment6698& e,bool allowed,std::uint64_t now) noexcept {
    namespace s=kcd2vr::supply_t2;s::Input i{};
    if(!kcd2vr::action_time_owner59c11::ThrowingSupplyT2()&&!s::nativeLease.load(std::memory_order_acquire)){
        kcd2vr::action_time_owner59c11::PublishThrowingSupplyT2(i);return;
    }
    i.manager=reinterpret_cast<std::uintptr_t>(e.manager);i.player=reinterpret_cast<std::uintptr_t>(e.player);
    i.body=frame.input.bodyEpoch;i.request=frame.request;i.sample=frame.input.sample;i.tick=frame.input.tick;
    i.grip=float(frame.input.grip[0]);i.tracked=frame.input.tracked[0]&&frame.input.intentTracked[0];i.allowed=allowed;
    i.hand[0]=e.handKey[0];i.hand[1]=e.handKey[1];i.beltKey=e.handKey[1]; // wire field16 now excludes the actual occupied left hand
    const auto a=ph::body_axes(frame.input);
    const pm::Vec3 mount[2]{frame.input.shoulders[0]-a.up*.50+a.right*.03+a.forward*.13,
        frame.input.shoulders[0]-a.up*.25-a.forward*.25+a.right*.06};
    for(int k=0;k<2;++k){const auto offset=frame.input.hands[0].p-mount[k];
        i.offset[k][0]=float(offset.x);i.offset[k][1]=float(offset.y);i.offset[k][2]=float(offset.z);}
    const auto& grant=frame.output.hipGrantT10;
    const bool same=grant.body==frame.input.bodyEpoch&&grant.request==frame.request&&grant.sample==frame.input.sample;
    i.pouchNear[0]=same&&grant.owner==kcd2vr::hip_t10::Owner::Dagger;
    i.pouchNear[1]=same&&grant.owner==kcd2vr::hip_t10::Owner::Axe;
    void* ci=allowed?HenryCharacter59c15():nullptr;void* entity=allowed?g_last_player_entity.load(std::memory_order_acquire):nullptr;
    std::uint32_t id=0;
    if(ci&&entity&&allowed&&PlayerIdentityAccepts37(ci,now)&&PhysVft6698(entity,kCEntityVftRva)&&Read(static_cast<unsigned char*>(entity)+0xC,id)&&id&&
       static_cast<unsigned char*>(ci)+8==g_henry_char_attachment_manager.load(std::memory_order_acquire)){
        i.character=reinterpret_cast<std::uintptr_t>(ci);i.entity=id;
    }
    for(int side=0;side<2;++side){
        i.grip18[side]=float(frame.input.grip[side]);
        float pos[3]{},q[4]{};std::uint64_t body=0,tracking=0,tick=0;std::uintptr_t actor=0;
        const bool have=allowed&&frame.input.tracked[side]&&frame.input.intentTracked[side]&&
            ReadGrabHandWorld60(side,pos,q,body,tracking,actor,tick)&&body==i.body&&
            actor==reinterpret_cast<std::uintptr_t>(g_last_player_entity.load())&&tick&&now>=tick&&now-tick<=80;
        i.pose18[side]=have;i.poseTick18[side]=have?tick:0;
        if(have){std::copy_n(q,4,i.wrist18[side]);std::copy_n(pos,3,i.wrist18[side]+4);}
        for(int k=0;k<2;++k){
            const auto offset=frame.input.hands[side].p-mount[k];
            i.offset18[side][k][0]=float(offset.x);i.offset18[side][k][1]=float(offset.y);i.offset18[side][k][2]=float(offset.z);
            i.near18[side][k]=side==0?i.pouchNear[k]:pm::length(offset)<.18;
        }
    }
    (void)kcd2vr::action_time_owner59c11::PublishThrowingSupplyT2(i);
}
