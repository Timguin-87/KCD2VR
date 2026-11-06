// M1: actual menu contributions, included at their original host scope.
// Shared bitmap rendering, state/apply/save and coordinate transforms stay host owned.
#if defined(KCD2VR_MENU_CLICK_KV149USERHOLSTERS)
if(PtIn(kV149UserHolsters,x,y)){
    s.throwing_supply_t2=!s.throwing_supply_t2;ApplyState(s);save();MarkDirty();return;
}
#elif defined(KCD2VR_MENU_DRAW_KV149USERHOLSTERS)
{
    const int stock=inventory_lifecycle_t8::daggers.load(std::memory_order_acquire);
    wchar_t label[80]{};
    if(!s.throwing_supply_t2)std::swprintf(label,80,L"KNIVES (HIP/CHEST): OFF");
    else if(stock<0)std::swprintf(label,80,L"KNIVES (HIP/CHEST): CHECKING");
    else std::swprintf(label,80,L"KNIVES (HIP/CHEST): %d",stock);
    DrawButton(dc,font_small_,kV149UserHolsters,label,s.throwing_supply_t2?kAccent:kPanel2);
}
#elif defined(KCD2VR_MENU_DRAW_STOCK_T8)
// R90 (R88 interact): D80 refill button restored. D85R5 removed it and turned off the
// starter grant, so a save without throwing knives/common dagger had 0 stock and
// the left hip never armed (D85R5 log 00:40-00:45: knives=0 in every preflight).
        DrawButton(dc,font_small_,U49(immersion_menu45::kNote),(inventory_lifecycle_t8::state.load()&1)?L"KNIFE STOCK: WAIT":(inventory_lifecycle_t8::ReplenishWanted()?L"KNIVES QUEUED":L"REFILL KNIVES TO 5"),kPanel2);
#elif defined(KCD2VR_MENU_CLICK_KWEAPONRELEASET1)
if(PtIn(U49(immersion_menu45::kNote),x,y)){inventory_lifecycle_t8::RequestReplenish();s.throwing_supply_t2=true;ApplyState(s);save();MarkDirty();return;} // R90 (R88 interact)
if(PtIn(kWeaponReleaseT1,x,y)){s.weapon_release_mode_t1=s.weapon_release_mode_t1==1?0:1;ApplyState(s);save();MarkDirty();return;}
#elif defined(KCD2VR_MENU_DRAW_KWEAPONRELEASET1)
        DrawButton(dc,font_small_,kWeaponReleaseT1,s.weapon_release_mode_t1==1?L"RELEASE: DROP / THROW (NATIVE RULES)":L"RELEASE: AUTO HOLSTER",s.weapon_release_mode_t1==1?kAccent:kPanel2);
#elif defined(KCD2VR_MENU_CLICK_KIXCHESTDAGGERT3)
if(PtIn(kIxChestDaggerT3,x,y)){s.chest_dagger54=!s.chest_dagger54;done(s.chest_dagger54?"T3 CHEST_DAGGER visual=ON":"T3 CHEST_DAGGER visual=OFF");return;}
#elif defined(KCD2VR_MENU_DRAW_KIXCHESTDAGGERT3)
        DrawButton(dc,font_small_,kIxChestDaggerT3,s.chest_dagger54?L"CHEST DAGGER: ON (VISUAL)":L"CHEST DAGGER: OFF",s.chest_dagger54?kAccent:kPanel2);
#endif
