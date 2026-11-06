// R120 pickup Use fallback (see vrik/pickup_use_fallback_r120.h). Native game rhythm (same thread as static_grab_r89n::Tick).
namespace pickup_use_fallback_r120n {
namespace pf=kcd2vr::pickup_use_fallback_r120;
static void Tick(std::uint64_t now) noexcept {
    const auto p=pf::read();
    const auto handled=pf::handled.load(std::memory_order_acquire);
    if(!p.serial||p.serial==handled)return;
    bool took=action_time_owner59c11::NativePickupOutstandingD72()||!EntityById6694(p.target);
    {PropLock596 lock;if(!lock.owned)return;took=took||g_prop596.id!=0;}
    // R124: a hand that already holds something (scenery grab / instant claim) is not free for the item: the instant
    // route would refuse after the game chose "immediate" and the item would go to the inventory. Leave it alone.
    took=took||(p.side>=0&&p.side<2&&kcd2vr::mutual_admission_t25::pickupClaimsD72[p.side].load(std::memory_order_acquire)!=0);
    TrackingSnapshot t{};
    const bool grip=ReadTrackingSnapshot(t)&&t.allowed&&(p.side?t.left_grip:t.right_grip)>.6f;
    const bool context=g_gameplay_allowed.load(std::memory_order_acquire)&&!g_ctx_menu.load()&&!g_ctx_dialogue.load()&&
        !g_ctx_cutscene.load()&&!g_ctx_reading.load()&&!g_ctx_scripted.load();
    const auto step=pf::decide(p,handled,now,kcd2vr::instant_d72::Interactions(),context,took,grip);
    if(step==pf::Step::Wait)return;
    auto expected=handled;if(!pf::handled.compare_exchange_strong(expected,p.serial))return; // one decision per squeeze
    if(step!=pf::Step::Act)return;
    auto exec=gore32n::g_lua_exec43.load(std::memory_order_acquire);
    char lua[520]{};std::snprintf(lua,sizeof(lua),
        "local e=System.GetEntity(%u) local u=rawget(_G,'g_localActor') "
        "if e and u and e.class=='PickableItem' and e.item and not e.npcOnly and e.item:CanUse(u.id)==true and "
        "e.item:CanSteal(u.id)==false and e.item:BelongsToDeadBody()==false then e.user=u e:OnUsed(u) end",p.target);
    const bool ran=exec&&exec(lua);
    pf::fallbacks.fetch_add(1,std::memory_order_relaxed);
    char m[220]{};std::snprintf(m,sizeof(m),"R120 PICKUP_USE_FALLBACK target=%u side=%d ageMs=%llu reason=GAME_INTERACTOR_DID_NOT_USE_ITEM lua=%s",
        p.target,p.side,(unsigned long long)(now-p.tick),!exec?"NO_LUA_EXEC":ran?"RAN":"FAILED");
    diagnostic_trace::Write(m);
}
}
