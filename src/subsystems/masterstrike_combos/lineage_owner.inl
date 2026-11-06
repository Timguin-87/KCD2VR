// R59.16C10R17 combos: a direct (non-C5235C) hit on Henry's CURRENT opponent sets the native hit flag (state +0x480) exactly as the
// sweep receivers do (C5235C@C5241A: 72FA34(state+0x480, target == opponent)); the combo manager (C5B404 -> AdvanceCombo) and the
// combo slot (909A90 at phase 16) read it. Once per release.
inline std::atomic<bool> g_combo_flag59c17{true};inline std::atomic<std::uint64_t> g_combo_flags59c17{0},g_combo_flag_serial59c17{0},g_combo_edges19{0},g_combo_same_value19{0},g_combo_unknown_edge19{0};
inline std::atomic<std::uint64_t> g_combo_hit_tick59c17{0}; // last VR hit on Henry's current opponent in F4 press 1/3 (combo59c17 window)
// C10R19C1: only an exact admitted native action can seed native combo grace.
static unsigned ComboMode19() noexcept {return (g_sync59c10r9.load()?1u:0u)|(g_intent59c16.load()?2u:0u)|
    (kcd2vr::motion59c4::selected.load()?4u:0u)|(g_live178r35.load()?8u:0u);}
static kcd2vr::combo_lineage19::Owner ComboOwner19(const Pending& p,const void* actor,const void* hs,const void* target,const void* action,std::uint64_t now) noexcept {
    namespace cg=kcd2vr::combo_lineage19;cg::Owner owner{};
    if(!g_enabled.load()||!g_sync59c10r9.load()||!actor||!hs||!target||!action||!p.release51.serial||
       !frik_pose_probe::GeometryAdmittedRelease51(p.release51,action))return owner;
    const void* player=Rd<const void*>(actor,0x1F8,nullptr),*manager=Rd<const void*>(actor,0x3B0,nullptr);
    const void* entity=nullptr,*targetState=nullptr;
    if(!CombatActor(target,entity,targetState,false)||!player||!manager||!entity||Rd<const void*>(manager,8,nullptr)!=actor||Rd<const void*>(hs,0xCE0,nullptr)!=actor||
       Rd<const void*>(actor,0x210,nullptr)!=hs||Rd<const void*>(hs,0xCF8,nullptr)!=target||
       reinterpret_cast<std::uintptr_t>(player)!=p.release51.player||p.release51.key!=p.key||p.release51.generation!=p.generation||
       p.release51.epoch!=p.epoch||p.release51.request!=p.request||p.release51.recovery59c3!=p.recovery55c1||
       !p.gesture59c3||p.release51.gesture59c3!=p.gesture59c3||EntityId(entity)!=p.entityId)return owner;
    owner.key={p.gesture59c3,p.key,p.generation,p.epoch,p.request,p.recovery55c1,reinterpret_cast<std::uintptr_t>(player)};
    owner.actor=reinterpret_cast<std::uintptr_t>(actor);owner.state=reinterpret_cast<std::uintptr_t>(hs);owner.manager=reinterpret_cast<std::uintptr_t>(manager);
    owner.target=reinterpret_cast<std::uintptr_t>(target);owner.targetEntity=reinterpret_cast<std::uintptr_t>(entity);owner.targetId=p.entityId;
    owner.action=reinterpret_cast<std::uintptr_t>(action);owner.release=p.release51.serial;owner.modeGeneration=cg::grace.generation();owner.mode=ComboMode19();owner.tick=now;
    return owner;
}

