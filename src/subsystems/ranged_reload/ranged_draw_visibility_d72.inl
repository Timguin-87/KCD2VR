// D76 review: preserve the D75 exact visibility owner across every physical
// weapon family and retain D76's stricter current melee binding graph.
// Native attach/equip/animation still runs. Only an accepted current placement
// readback or a deliberate native ownership handoff can reveal an owned hide.
// Legacy RangedDraw names are retained for existing crossbow/physical callers.
namespace rdrawD72=kcd2vr::ranged_draw_d72;
namespace wdrawD75=kcd2vr::weapon_draw_d75;
struct RangedDrawPendingD72 {
    rdrawD72::Identity binding{};bool owned=false;
    std::uint64_t tracking=0,since=0,seen=0;
    void* item=nullptr;void* manager=nullptr;bool cleanup=false;
    std::uint64_t reloadLease=0;
};
struct WeaponDrawAcceptedD75 {rdrawD72::Identity binding{};std::uint64_t tracking=0;};
static RangedDrawPendingD72 g_ranged_draw_d72[2];
// Unknown old lifetimes must not be overwritten when a different item is drawn.
// Bounded cleanup slots retain only our exact original-visible write receipts.
static std::array<RangedDrawPendingD72,8> g_weapon_draw_cleanup_d75{};
static WeaponDrawAcceptedD75 g_weapon_draw_accepted_d75[2];
static std::uint64_t g_weapon_draw_attach_serial_d75[2]{};
static std::atomic<unsigned> g_ranged_draw_mask_d73{0};
static rdrawD72::Identity RangedDrawIdentityD72(const PhysEquipment6698& e,const PhysEquipmentItem6698& i,
        void* self,void* attachment) noexcept {
    return {reinterpret_cast<std::uintptr_t>(i.entity),reinterpret_cast<std::uintptr_t>(self),reinterpret_cast<std::uintptr_t>(attachment),
        i.profile.key,i.profile.generation,e.epoch,e.request,i.profile.entity,i.profile.side};
}
static bool WeaponDrawTransportD75(const rdrawD72::Identity& a,const rdrawD72::Identity& b) noexcept {
    // A provisional callback has no equipment generation yet. This relation
    // transfers only an existing visibility debt, never placement readiness.
    return a.entity&&a.self&&a.attachment&&a.key&&a.id&&a.side>=0&&a.side<2&&
        a.entity==b.entity&&a.self==b.self&&a.attachment==b.attachment&&a.key==b.key&&
        a.body==b.body&&a.request==b.request&&a.id==b.id&&a.side==b.side;
}
static void WeaponDrawMaskD75() noexcept {
    unsigned mask=0;for(int s=0;s<2;++s)if(g_ranged_draw_d72[s].owned)mask|=1u<<s;
    for(const auto& old:g_weapon_draw_cleanup_d75)if(old.owned){mask|=4u;break;}
    g_ranged_draw_mask_d73.store(mask,std::memory_order_release);
}
static void RangedDrawForgetD73(int side) noexcept {g_ranged_draw_d72[side]={};WeaponDrawMaskD75();}
static bool WeaponDrawRestoreD75(RangedDrawPendingD72& debt,bool cleanup) noexcept {
    if(!debt.owned)return true;
    if(cleanup)debt.cleanup=true;
    const auto old=debt.binding;void* entity=EntityById6694(old.id);
    if(!entity)return false; // unavailable lookup does not prove destruction
    if(reinterpret_cast<std::uintptr_t>(entity)!=old.entity){debt={};WeaponDrawMaskD75();return true;}
    if(!PhysEntityIdentity135(entity,old.id))return false;
    bool hidden=false;if(!EntityHiddenFlag59c14(entity,hidden))return false;
    if(hidden){
        if(!EntityHide59c14(entity,false))return false;
        void* after=EntityById6694(old.id);if(!after)return false;
        if(after!=entity){debt={};WeaponDrawMaskD75();return true;}
        if(!PhysEntityIdentity135(entity,old.id)||!EntityHiddenFlag59c14(entity,hidden)||hidden)return false;
    }
    debt={};WeaponDrawMaskD75();return true;
}
static bool RangedDrawReleaseD72(int side) noexcept {
    return side>=0&&side<2&&WeaponDrawRestoreD75(g_ranged_draw_d72[side],true);
}
static bool WeaponDrawRetainCleanupD75(int side) noexcept {
    auto& old=g_ranged_draw_d72[side];if(!old.owned||RangedDrawReleaseD72(side))return true;
    for(auto& slot:g_weapon_draw_cleanup_d75)if(!slot.owned){
        slot=old;slot.cleanup=true;old={};WeaponDrawMaskD75();return true;}
    return false; // no receipt may be discarded to make room for another hide
}
static bool WeaponDrawNativeOwnerD75(std::uint64_t now,bool ownedReload=false) noexcept {
    // Tracking/equipment read gaps are deliberately absent: they are not a
    // native takeover. The ordinary calibrated VRIK feature has no F2/trial gate.
    return g_weapon_control_mode.load()<4||!StowedBowImmersive54()||g_phys_fault6698.load()||
        (!ownedReload&&(!g_gameplay_allowed.load()||animation_filter::nativeBodyOwned112.load()))||
        NativePassengerCurrentA34(now);
}
static bool RangedDrawContextD73(int side,std::uint64_t now,bool ownedReload=false) noexcept {
    return side>=0&&side<2&&!WeaponDrawNativeOwnerD75(now,ownedReload)&&
        TrackingHeadUsable111(now)&&TrackingHandUsable111(side,now);
}
static bool RangedDrawReloadOwnsD73(const RangedDrawPendingD72& pending,std::uint64_t now) noexcept {
    namespace xr=kcd2vr::crossbow_native_xr34;xr::Status status{};xr::Source current{};
    if(!xr::ReadStatus(status)||!status.clockOwned||!xr::handDrivenD81(status.owner.source)||
       !xr::ReadSource(current)||!xr::fresh(current,now)||!xr::sameSource(current,status.owner.source))return false;
    const auto& b=pending.binding;
    return current.primary==b.side&&current.key==b.key&&current.generation==b.generation&&
        current.body==b.body&&current.request==b.request&&current.tracking==pending.tracking;
}
static bool WeaponDrawReloadRetainsD75(RangedDrawPendingD72& pending) noexcept {
    // Native lease ownership and pose freshness are different proofs. Losing
    // the latter must not expose an in-progress native reload animation.
    namespace xr=kcd2vr::crossbow_native_xr34;xr::Status status{};
    if(xr::ReadStatus(status)){
        const auto& s=status.owner.source;const auto& b=pending.binding;
        const bool same=status.clockOwned&&xr::handDrivenD81(s)&&s.primary==b.side&&s.key==b.key&&
            s.generation==b.generation&&s.body==b.body&&s.request==b.request&&s.tracking==pending.tracking;
        pending.reloadLease=same?status.owner.serial:0;return same;
    }
    return pending.reloadLease&&xr::activeNativeLease.load(std::memory_order_acquire)==pending.reloadLease&&
        xr::revokedNativeLease.load(std::memory_order_acquire)!=pending.reloadLease;
}
static bool WeaponDrawReloadAttachmentD75(void* self,void* attachment,std::uint64_t now) noexcept {
    if(WeaponDrawNativeOwnerD75(now,true))return false;
    for(auto& pending:g_ranged_draw_d72)if(pending.owned&&pending.binding.self==reinterpret_cast<std::uintptr_t>(self)&&
        pending.binding.attachment==reinterpret_cast<std::uintptr_t>(attachment)&&WeaponDrawReloadRetainsD75(pending))return true;
    return false;
}
static bool RangedDrawEquipmentD73(PhysEquipment6698& e,std::uint64_t now) noexcept {
    return g_phys_equipment6698.read(e)&&e.valid&&e.tick&&now>=e.tick&&now-e.tick<=100&&
        e.epoch==g_pose_epoch6689.load(std::memory_order_acquire)&&
        e.request==g_weapon_request_epoch6694.load(std::memory_order_acquire);
}
static const PhysEquipmentItem6698* RangedDrawItemD73(const PhysEquipment6698& e,const rdrawD72::Identity& b) noexcept {
    for(int i=0;i<e.count&&i<int(e.items.size());++i){const auto& item=e.items[i];
        if(item.profile.side==b.side&&rdrawD72::same(b,RangedDrawIdentityD72(e,item,
            reinterpret_cast<void*>(b.self),reinterpret_cast<void*>(b.attachment))))return &item;}
    return nullptr;
}
static bool RangedDrawHeldD73(const PhysEquipment6698& e,const PhysEquipmentItem6698* item) noexcept {
    return item&&item->held&&item->profile.side>=0&&item->profile.side<2&&
        e.handKnown[item->profile.side]&&e.handKey[item->profile.side]==item->profile.key;
}
static bool WeaponDrawMayStowD75(const PhysEquipment6698& e,const PhysEquipmentItem6698& item) noexcept {
    // The outer-frame snapshot can still say "stowed" inside a newly accepted
    // native attach callback. Only current positive non-membership may end the
    // first-placement hide; an unreadable hand must retain the pending debt.
    const int side=item.profile.side;std::uint64_t live=0;
    return side>=0&&side<2&&e.manager&&e.manager==g_henry_item_attach_manager.load(std::memory_order_acquire)&&
        NativeHandKey6697(e.manager,side,live)&&live!=item.profile.key;
}
static bool WeaponDrawBindingD75(const rdrawD72::Identity& b) noexcept {
    void* self=reinterpret_cast<void*>(b.self);void* attachment=reinterpret_cast<void*>(b.attachment);
    void* object=nullptr;void* manager=nullptr;std::uint32_t id=0;
    return PhysVft6698(self,0x3A68F68)&&PhysVft6698(attachment,0x3A657C0)&&
        Read(static_cast<unsigned char*>(self)+8,id)&&id==b.id&&
        Read(static_cast<unsigned char*>(attachment)+0x20,object)&&object==self&&
        Read(static_cast<unsigned char*>(attachment)+0x28,manager)&&manager&&
        manager==g_henry_char_attachment_manager.load(std::memory_order_acquire);
}
static bool WeaponDrawAcceptedForD75(const rdrawD72::Identity& b) noexcept {
    if(b.side<0||b.side>1)return false;
    const auto& a=g_weapon_draw_accepted_d75[b.side];
    return a.tracking==tracking59c6::epoch.load(std::memory_order_acquire)&&rdrawD72::same(a.binding,b);
}
static bool WeaponDrawHeldNowD75(const rdrawD72::Identity& b,void* nativeItem,void* manager) noexcept {
    // PRS/readback calls native code. A still-fresh equipment mailbox from
    // before those calls cannot certify the current hand or item world link.
    if(!nativeItem||!manager||manager!=g_henry_item_attach_manager.load(std::memory_order_acquire)||
       b.body!=g_pose_epoch6689.load(std::memory_order_acquire)||
       b.request!=g_weapon_request_epoch6694.load(std::memory_order_acquire))return false;
    std::uint64_t live=0;PhysEquipmentItem6698 current{};
    if(!NativeHandKey6697(manager,b.side,live)||live!=b.key||!PhysDescribeItem54(b.key,nativeItem,current)||
       current.profile.entity!=b.id||!wdrawD75::weaponClass(current.profile.classId)||
       EntityById6694(b.id)!=reinterpret_cast<void*>(b.entity)||!PhysEntityIdentity135(reinterpret_cast<void*>(b.entity),b.id)||
       !WeaponDrawBindingD75(b))return false;
    current.entity=reinterpret_cast<void*>(b.entity);current.profile.side=b.side;
    if(!current.profile.bow()&&!kcd2vr::ranged_kind_xr32::crossbow(current.profile.classId)&&
       !PhysicalDrawBindingGraphD76(current,reinterpret_cast<void*>(b.self),reinterpret_cast<void*>(b.attachment),b.body,b.request))return false;
    void* link=nullptr;std::uint32_t id=0;
    return NativeHandKey6697(manager,b.side,live)&&live==b.key&&
        PhysReadItemWorld54(nativeItem,link,id)&&id==b.id&&link==current.worldLink54&&
        manager==g_henry_item_attach_manager.load(std::memory_order_acquire)&&
        b.body==g_pose_epoch6689.load(std::memory_order_acquire)&&
        b.request==g_weapon_request_epoch6694.load(std::memory_order_acquire)&&WeaponDrawBindingD75(b);
}
static void WeaponDrawArmD75(const rdrawD72::Identity& binding,void* item,void* manager,std::uint64_t now) noexcept {
    if(binding.side<0||binding.side>1||!WeaponDrawTransportD75(binding,binding)||
       !RangedDrawContextD73(binding.side,now)||!WeaponDrawBindingD75(binding)||WeaponDrawAcceptedForD75(binding))return;
    auto& debt=g_ranged_draw_d72[binding.side];
    // Rebind an owned hide only after this caller has proved the current CItem
    // and exact entity. No intermediate show occurs while metadata catches up.
    const bool sameEntity=debt.owned&&debt.binding.entity==binding.entity&&debt.binding.id==binding.id&&
        debt.binding.key==binding.key&&debt.item==item&&debt.manager==manager;
    if(debt.owned&&!sameEntity&&!WeaponDrawRetainCleanupD75(binding.side))return;
    void* entity=EntityById6694(binding.id);bool hidden=true;
    if(reinterpret_cast<std::uintptr_t>(entity)!=binding.entity||!PhysEntityIdentity135(entity,binding.id)||
       !EntityHiddenFlag59c14(entity,hidden))return;
    if(sameEntity){
        if(!rdrawD72::same(debt.binding,binding))debt.reloadLease=0;
        debt.binding=binding;debt.tracking=tracking59c6::epoch.load(std::memory_order_acquire);
        debt.seen=now;debt.cleanup=false;if(hidden)return;
    }else if(hidden)return; // native/station/stowed pre-hides are never adopted
    const auto since=sameEntity?debt.since:now;
    bool invoked=false;(void)EntityHide59c14(entity,true,&invoked);if(!invoked)return;
    debt={binding,true,tracking59c6::epoch.load(std::memory_order_acquire),since,now,item,manager,false};
    WeaponDrawMaskD75();
    void* after=EntityById6694(binding.id);
    if(after&&after!=entity){RangedDrawForgetD73(binding.side);return;}
    if(after==entity&&PhysEntityIdentity135(entity,binding.id)&&EntityHiddenFlag59c14(entity,hidden)&&!hidden)
        RangedDrawForgetD73(binding.side); // positively visible means no hide debt
}
// Retire our original-visible hide BEFORE the stowed owner can acquire a hide.
static bool RangedDrawStowedD73(const PhysEquipmentItem6698& item) noexcept {
    const auto side=item.profile.side;if(side<0||side>1)return true;
    g_weapon_draw_accepted_d75[side]={};
    const auto& debt=g_ranged_draw_d72[side];
    if(debt.owned&&debt.binding.entity==reinterpret_cast<std::uintptr_t>(item.entity)&&
       debt.binding.id==item.profile.entity&&!RangedDrawReleaseD72(side))return false;
    return true;
}
static void RangedDrawReleaseAllD72() noexcept {
    for(int side=0;side<2;++side){g_weapon_draw_accepted_d75[side]={};(void)RangedDrawReleaseD72(side);}
    for(auto& old:g_weapon_draw_cleanup_d75)(void)WeaponDrawRestoreD75(old,true);
}
static void RangedDrawTickD72(std::uint64_t now) noexcept {
    // Accepted identities also need a positive stow observation so a same-item
    // redraw cannot inherit readiness even when its attachment pointer is reused.
    PhysTryLock6698 lock;if(!lock.owned)return;
    PhysEquipment6698 equipment{};const bool have=RangedDrawEquipmentD73(equipment,now);
    for(auto& old:g_weapon_draw_cleanup_d75)(void)WeaponDrawRestoreD75(old,true);
    for(int side=0;side<2;++side){auto& debt=g_ranged_draw_d72[side];auto& accepted=g_weapon_draw_accepted_d75[side];
        const bool reload=debt.owned&&WeaponDrawReloadRetainsD75(debt);
        if(WeaponDrawNativeOwnerD75(now,reload)){
            accepted={};(void)RangedDrawReleaseD72(side);continue;
        }
        if(have&&accepted.binding.key&&equipment.handKnown[side]&&equipment.handKey[side]!=accepted.binding.key)accepted={};
        if(!debt.owned)continue;
        if(debt.cleanup){(void)RangedDrawReleaseD72(side);continue;}
        void* entity=EntityById6694(debt.binding.id);
        if(entity&&reinterpret_cast<std::uintptr_t>(entity)!=debt.binding.entity){RangedDrawForgetD73(side);continue;}
        if(reload)continue;
        std::uint64_t live=0;
        // A current explicit empty/different native hand is a lifecycle end.
        // Missing lookup, stale equipment, tracking loss and elapsed time are not.
        const bool liveKnown=debt.manager&&debt.manager==g_henry_item_attach_manager.load(std::memory_order_acquire)&&
            NativeHandKey6697(debt.manager,side,live);
        if(liveKnown&&live!=debt.binding.key){
            accepted={};
            const auto* item=have?RangedDrawItemD73(equipment,debt.binding):nullptr;
            if(item&&!item->held){if(RangedDrawStowedD73(*item)&&item->profile.bow())StowedBowHide59c14(item->entity,now);}
            else (void)RangedDrawReleaseD72(side);
        }
    }
}
static bool RangedDrawPendingForD72(const rdrawD72::Identity& binding) noexcept {
    return binding.side>=0&&binding.side<2&&g_ranged_draw_d72[binding.side].owned&&
        rdrawD72::same(g_ranged_draw_d72[binding.side].binding,binding);
}
static void RangedDrawBeginD72(const rdrawD72::Identity& binding) noexcept {
    if(binding.side<0||binding.side>1||!rdrawD72::same(binding,binding))return;
    const auto now=std::uint64_t(GetTickCount64());PhysEquipment6698 equipment{};
    if(!RangedDrawEquipmentD73(equipment,now))return;
    const auto* item=RangedDrawItemD73(equipment,binding);
    if(!RangedDrawHeldD73(equipment,item)||!wdrawD75::weaponClass(item->profile.classId))return;
    std::uint64_t live=0;
    if(equipment.manager!=g_henry_item_attach_manager.load(std::memory_order_acquire)||
       !NativeHandKey6697(equipment.manager,binding.side,live)||live!=binding.key)return;
    if(!item->profile.bow()&&!kcd2vr::ranged_kind_xr32::crossbow(item->profile.classId)&&
       !PhysicalDrawHandBindingD75(equipment,*item,reinterpret_cast<void*>(binding.self),reinterpret_cast<void*>(binding.attachment)))return;
    WeaponDrawArmD75(binding,item->item,equipment.manager,now);
}
static void RangedDrawPlacedD72(const rdrawD72::Identity& binding,bool read,bool ok) noexcept {
    if(!rdrawD72::mayReveal(read,ok)||!rdrawD72::same(binding,binding)||!WeaponDrawBindingD75(binding))return;
    const auto now=std::uint64_t(GetTickCount64());auto& debt=g_ranged_draw_d72[binding.side];
    const bool reload=debt.owned&&RangedDrawReloadOwnsD73(debt,now);PhysEquipment6698 equipment{};
    if(!RangedDrawContextD73(binding.side,now,reload))return;
    const bool have=RangedDrawEquipmentD73(equipment,now);
    const auto* item=have?RangedDrawItemD73(equipment,binding):nullptr;
    if(!reload&&!RangedDrawHeldD73(equipment,item))return;
    if(!WeaponDrawHeldNowD75(binding,item?item->item:debt.item,item?equipment.manager:debt.manager))return;
    if(debt.owned){
        // A pending first native callback may predate metadata generation.
        // Join only the current exact CItem/binding; no timer promotes it.
        if(!debt.binding.generation&&item&&item->item==debt.item&&equipment.manager==debt.manager&&
           WeaponDrawTransportD75(debt.binding,binding))debt.binding=binding;
        if(!RangedDrawPendingForD72(binding)||debt.tracking!=tracking59c6::epoch.load(std::memory_order_acquire))return;
        if(!WeaponDrawRestoreD75(debt,false))return; // retry only on another current placement, not next tick
    }
    g_weapon_draw_accepted_d75[binding.side]={binding,tracking59c6::epoch.load(std::memory_order_acquire)};
}
static void RangedDrawRetireAttachmentD72(void* self,void* attachment) noexcept {
    for(int side=0;side<2;++side){const auto& b=g_ranged_draw_d72[side].binding;
        if(b.self==reinterpret_cast<std::uintptr_t>(self)&&b.attachment==reinterpret_cast<std::uintptr_t>(attachment))
            (void)RangedDrawReleaseD72(side);
        const auto& a=g_weapon_draw_accepted_d75[side].binding;
        if(a.self==reinterpret_cast<std::uintptr_t>(self)&&a.attachment==reinterpret_cast<std::uintptr_t>(attachment))g_weapon_draw_accepted_d75[side]={};}
}
static void WeaponDrawBeforeAttachmentD75(void* self,void* attachment) noexcept {
    const auto now=std::uint64_t(GetTickCount64());
    if(!self||!attachment||WeaponDrawNativeOwnerD75(now))return;
    std::uint32_t id=0;if(!Read(static_cast<unsigned char*>(self)+8,id)||!id)return;
    auto* scope=wdrawD75::currentAttach;void* manager=g_henry_item_attach_manager.load(std::memory_order_acquire);
    if(!manager)return;
    void* candidate[2]{};
    for(int side=0;side<2;++side){
        if(g_native_side_owner[side].load(std::memory_order_acquire)==manager&&g_native_side_item_state[side].load(std::memory_order_acquire)==1){
            std::uint32_t possible=0;if(NativeCandidateItemId6699(side,possible)&&possible==id)
                candidate[side]=g_native_side_item[side].load(std::memory_order_acquire);
        }
    }
    // Scope is a candidate only. Even a successful original return must still
    // pass the independent live hand-key proof below before visibility changes.
    if(scope&&scope->owner==manager&&scope->item){
        void* link=nullptr;std::uint32_t possible=0;
        if(PhysReadItemWorld54(scope->item,link,possible)&&possible==id){
            scope->remember(self,attachment);
            for(int side=0;side<2;++side)if(scope->side==2||scope->side==side)candidate[side]=scope->item;
        }
    }
    if(!candidate[0]&&!candidate[1]){
        void* attachmentManager=nullptr;
        if(!Read(static_cast<unsigned char*>(attachment)+0x28,attachmentManager)||!attachmentManager||
           attachmentManager!=g_henry_char_attachment_manager.load(std::memory_order_acquire))return;
    }
    PhysTryLock6698 lock;if(!lock.owned)return;
    ReadScope101R2 pages;
    PhysEquipment6698 equipment{};const bool have=RangedDrawEquipmentD73(equipment,now);
    // Async binding callbacks need not retain the synchronous attach scope.
    // The current equipment packet can nominate the item too. It grants no
    // write authority: live hand key and the full binding are checked below.
    if(have&&equipment.manager==manager){
        for(int i=0;i<equipment.count&&i<int(equipment.items.size());++i){
            const auto& item=equipment.items[i];const int side=item.profile.side;
            if(side>=0&&side<2&&!candidate[side]&&item.profile.entity==id&&
               RangedDrawHeldD73(equipment,&item)&&wdrawD75::weaponClass(item.profile.classId))
                candidate[side]=item.item;
        }
    }
    if(!candidate[0]&&!candidate[1])return;
    for(int side=0;side<2;++side){if(!candidate[side]||!RangedDrawContextD73(side,now))continue;
        std::uint64_t live=0;if(!NativeHandKey6697(manager,side,live)||!live)continue;
        const bool newNativeAttach=scope&&scope->item==candidate[side]&&scope->serial!=g_weapon_draw_attach_serial_d75[side];
        bool accepted=false;
        if(have&&!newNativeAttach&&!g_ranged_draw_d72[side].owned&&equipment.manager==manager)
            for(int n=0;n<equipment.count&&n<int(equipment.items.size());++n){const auto& item=equipment.items[n];
                if(item.item==candidate[side]&&item.profile.entity==id&&item.profile.key==live&&
                   item.profile.side==side&&RangedDrawHeldD73(equipment,&item)){
                    const auto b=RangedDrawIdentityD72(equipment,item,self,attachment);
                    accepted=WeaponDrawAcceptedForD75(b)&&WeaponDrawBindingD75(b);break;}}
        if(accepted)continue; // ordinary held frames need no new descriptor/registry/hide transaction
        PhysEquipmentItem6698 current{};
        if(!PhysDescribeItem54(live,candidate[side],current)||current.profile.entity!=id||
           !wdrawD75::weaponClass(current.profile.classId))continue;
        void* entity=EntityById6694(id);if(!entity||!PhysEntityIdentity135(entity,id))continue;
        const auto epoch=g_pose_epoch6689.load(std::memory_order_acquire),request=g_weapon_request_epoch6694.load(std::memory_order_acquire);
        rdrawD72::Identity binding{reinterpret_cast<std::uintptr_t>(entity),reinterpret_cast<std::uintptr_t>(self),
            reinterpret_cast<std::uintptr_t>(attachment),live,0,epoch,request,id,side};
        if(!WeaponDrawBindingD75(binding))continue;
        current.entity=entity;current.profile.side=side;
        if(!current.profile.bow()&&!kcd2vr::ranged_kind_xr32::crossbow(current.profile.classId)&&
           !PhysicalDrawBindingGraphD76(current,self,attachment,epoch,request))continue;
        if(have&&equipment.manager==manager)for(int i=0;i<equipment.count&&i<int(equipment.items.size());++i){const auto& item=equipment.items[i];
            if(item.item==candidate[side]&&item.entity==entity&&item.profile.entity==id&&item.profile.key==live&&
               item.profile.side==side)binding.generation=item.profile.generation;}
        std::uint64_t again=0;void* link=nullptr;std::uint32_t after=0;
        if(manager!=g_henry_item_attach_manager.load(std::memory_order_acquire)||
           !NativeHandKey6697(manager,side,again)||again!=live||
           !PhysReadItemWorld54(candidate[side],link,after)||after!=id||link!=current.worldLink54||
           epoch!=g_pose_epoch6689.load(std::memory_order_acquire)||request!=g_weapon_request_epoch6694.load(std::memory_order_acquire)||
           !WeaponDrawBindingD75(binding))continue;
        if(scope&&scope->item==candidate[side]&&scope->serial!=g_weapon_draw_attach_serial_d75[side]){
            g_weapon_draw_attach_serial_d75[side]=scope->serial;g_weapon_draw_accepted_d75[side]={};}
        WeaponDrawArmD75(binding,candidate[side],manager,now);
    }
}
// ---- R120 draw guard (draw_guard_r120.h) ---------------------------------
namespace dgR120=kcd2vr::draw_guard_r120;
// lock held. Show only our own hidden write; release the guard slot.
static void DrawGuardShowR120(dgR120::Guard& g) noexcept {
    const auto id=g.id.load(std::memory_order_acquire);
    if(id&&g.ours.load(std::memory_order_acquire)){
        void* e=EntityById6694(id);bool hidden=false;
        if(e&&PhysEntityIdentity135(e,id)&&EntityHiddenFlag59c14(e,hidden)&&hidden)(void)EntityHide59c14(e,false);
    }
    g.ours.store(false);g.placed.store(false);g.first.store(0);g.armed.store(0);g.frames.store(0);g.key.store(0);g.id.store(0,std::memory_order_release);
}
// R121: our grip for this drawn item is learned (placement can be written on the next callback).
static bool DrawGuardGripReadyR121(const dgR120::Guard& g,int side) noexcept {
    const auto key=g.key.load(std::memory_order_acquire);if(!key||side<0||side>1)return false;
    PhysGripBank6698 bank{};if(!g_phys_grips6698.read(bank))return false;
    for(const auto& x:bank.grip)if(x.key==key)return x.ready[side];
    return false;
}
// lock held. Hide when visible; only a write we made is ours to show later.
static void DrawGuardHideR120(dgR120::Guard& g) noexcept {
    const auto id=g.id.load(std::memory_order_acquire);if(!id)return;
    void* e=EntityById6694(id);bool hidden=false;
    if(!e||!PhysEntityIdentity135(e,id)||!EntityHiddenFlag59c14(e,hidden)||hidden)return;
    bool invoked=false;(void)EntityHide59c14(e,true,&invoked);
    if(invoked)g.ours.store(true,std::memory_order_release);
}
static bool DrawGuardVisibleR120(std::uint32_t id) noexcept {
    void* e=EntityById6694(id);bool hidden=true;
    return e&&PhysEntityIdentity135(e,id)&&EntityHiddenFlag59c14(e,hidden)&&!hidden;
}
// Native item attach into one of Henry's hands succeeded: arm for that weapon entity.
static void DrawGuardArmR120(void* owner,void* item) noexcept {
    const auto now=std::uint64_t(GetTickCount64());
    if(!owner||!item||owner!=g_henry_item_attach_manager.load(std::memory_order_acquire)||WeaponDrawNativeOwnerD75(now))return;
    std::uint64_t key=0;if(!Read(static_cast<unsigned char*>(item)+0x30,key)||!key)return;
    int side=-1;for(int s=0;s<2;++s){std::uint64_t live=0;if(NativeHandKey6697(owner,s,live)&&live==key){side=s;break;}}
    if(side<0)return; // not in a hand after the op: not a draw
    PhysEquipmentItem6698 d{};if(!PhysDescribeItem54(key,item,d)||!wdrawD75::weaponClass(d.profile.classId))return;
    void* link=nullptr;std::uint32_t id=0;if(!PhysReadItemWorld54(item,link,id)||!id)return;
    std::unique_lock<std::mutex> l(dgR120::lock,std::try_to_lock);if(!l.owns_lock())return;
    auto& g=dgR120::guards[side];
    const auto old=g.id.load(std::memory_order_acquire);
    if(old&&old!=id)DrawGuardShowR120(g);
    if(g.id.load(std::memory_order_acquire)!=id){g.ours.store(false);g.id.store(id,std::memory_order_release);}
    g.key.store(key);g.frames.store(0);
    g.armed.store(now);g.first.store(0);g.placed.store(false,std::memory_order_release);
    DrawGuardHideR120(g);
}
static void DrawGuardTickR120(std::uint64_t now) noexcept {
    bool any=false;for(auto& g:dgR120::guards)any|=g.id.load(std::memory_order_acquire)!=0;
    if(!any)return;
    std::unique_lock<std::mutex> l(dgR120::lock,std::try_to_lock);if(!l.owns_lock())return;
    const bool native=WeaponDrawNativeOwnerD75(now);
    for(int side=0;side<2;++side){auto& g=dgR120::guards[side];
        if(!g.id.load(std::memory_order_acquire))continue;
        if(g.first.load())g.frames.fetch_add(1); // R121: one native frame passed since the first hand callback
        if(native||dgR120::decide(g.placed.load(),g.armed.load(),g.first.load(),now,false,g.frames.load(),DrawGuardGripReadyR121(g,side))==dgR120::Step::Show)DrawGuardShowR120(g);
    }
}
static void DrawGuardAfterCallbackR120(void* self,void* attachment) noexcept {
    std::uint32_t id=0;
    if(!self||!attachment||!Read(static_cast<unsigned char*>(self)+8,id)||!dgR120::armedFor(id))return; // cheap common exit
    void* manager=nullptr;
    if(!PhysVft6698(self,0x3A68F68)||!Read(static_cast<unsigned char*>(attachment)+0x28,manager)||
       !manager||manager!=g_henry_char_attachment_manager.load(std::memory_order_acquire))return;
    const auto now=std::uint64_t(GetTickCount64());
    std::unique_lock<std::mutex> l(dgR120::lock,std::try_to_lock);if(!l.owns_lock())return; // next callback decides again
    for(int side=0;side<2;++side){auto& g=dgR120::guards[side];
        if(g.id.load(std::memory_order_acquire)!=id)continue;
        std::uint64_t expected=0;(void)g.first.compare_exchange_strong(expected,now);
        const auto step=WeaponDrawNativeOwnerD75(now)?dgR120::Step::Show:
            dgR120::decide(g.placed.load(),g.armed.load(),g.first.load(),now,DrawGuardVisibleR120(id),g.frames.load(),DrawGuardGripReadyR121(g,side));
        if(step==dgR120::Step::Show)DrawGuardShowR120(g);
        else if(step==dgR120::Step::Hide)DrawGuardHideR120(g);
    }
}
static void WeaponDrawAfterNativeAttachD75(const wdrawD75::NativeAttachScope& scope) noexcept {
    // The first callback can precede the native hand-key publication. Recheck
    // its exact observed binding after the original returned successfully.
    InvalidateReadCache101R2();
    for(const auto& c:scope.observed)if(c.self)WeaponDrawBeforeAttachmentD75(c.self,c.attachment);
    InvalidateReadCache101R2();
    DrawGuardArmR120(scope.owner,scope.item); // R120
}
