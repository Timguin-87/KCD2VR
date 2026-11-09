"""Canonical current-source members for the combined game-content package.

The C63 archive is a verified donor. Both preparation and validation use this
mapping so a new source edit cannot be silently omitted from a release.
"""
SOURCE_PAIRS = {
    'companion/source/Scripts/Startup/zzkcd2vr_chat_target_d78.lua': 'Scripts/Startup/zzkcd2vr_chat_target_d78.lua',
    'companion/source/zzkcd2vr_corpse_d72.lua': 'Scripts/Startup/zzkcd2vr_corpse_d72.lua',
    'companion/source/Scripts/Startup/zzkcd2vr_life_d73.lua': 'Scripts/Startup/zzkcd2vr_life_d73.lua',
    'companion/source/Scripts/Startup/zzkcd2vr_alchemy_sound_d80.lua': 'Scripts/Startup/zzkcd2vr_alchemy_sound_d80.lua',
    'companion/source/Scripts/Startup/zzkcd2vr_swing_whoosh_r116.lua': 'Scripts/Startup/zzkcd2vr_swing_whoosh_r116.lua', # R116 swing whoosh
    'companion/source/Scripts/Startup/zzkcd2vr_quest_diag_r158.lua': 'Scripts/Startup/zzkcd2vr_quest_diag_r158.lua', # R158 Shift+F5 quest recorder (read-only)
    'companion/source/AI/npc/basic/switch/switch.xml': 'AI/npc/basic/switch/switch.xml',
    'companion/source/AI/npc/basic/switch/kcd2vrlifelookd73.xml': 'AI/npc/basic/switch/kcd2vrlifelookd73.xml',


    'companion/source/Libs/Tables/combat/combat_action_attack.xml': 'Libs/Tables/combat/combat_action_attack.xml', # D72 dedicated native SPECIAL kick rows
    'companion/source/Libs/Tables/combat/combat_attack_config__kcdvr.xml': 'Libs/Tables/combat/combat_attack_config__kcdvr.xml',
    'companion/source/Animations/Mannequin/ADB/kcd_male_combat_generated.adb': 'Animations/Mannequin/ADB/kcd_male_combat_generated.adb',
    'companion/source/Animations/Mannequin/ADB/kcd_male_combat_database.adb': 'Animations/Mannequin/ADB/kcd_male_combat_database.adb',
    'companion/source/Libs/UI/Menu.gfx': 'Libs/UI/Menu.gfx', # D68 glyph-only presentation
    'companion/source/Libs/UI/ItemSelection.gfx': 'Libs/UI/ItemSelection.gfx', # D68 glyph-only presentation
    'companion/source/Libs/UI/ItemTransfer.gfx': 'Libs/UI/ItemTransfer.gfx', # D68 glyph-only presentation
    'companion/source/Libs/UI/ApseMap.gfx': 'Libs/UI/ApseMap.gfx', # D68 glyph-only presentation
    'companion/source/Libs/UI/ApseModalDialog.gfx': 'Libs/UI/ApseModalDialog.gfx', # D68 glyph-only presentation
    'companion/source/Libs/UI/ApsePlayerInfo.gfx': 'Libs/UI/ApsePlayerInfo.gfx', # D68 glyph-only presentation
    'companion/source/Libs/UI/GameOver.gfx': 'Libs/UI/GameOver.gfx', # D68 glyph-only presentation
    'companion/source/Libs/UI/buttons.gfx': 'Libs/UI/buttons.gfx', # D68 glyph-only presentation
    'companion/source/Libs/UI/UIElements/HUD.xml': 'Libs/UI/UIElements/HUD.xml', # D68 glyph-only presentation
    'companion/source/Libs/UI/UIElements/Menu.xml': 'Libs/UI/UIElements/Menu.xml', # D68 glyph-only presentation
    'companion/source/Libs/UI/UIElements/ItemSelection.xml': 'Libs/UI/UIElements/ItemSelection.xml', # D68 glyph-only presentation
    'companion/source/Libs/UI/UIElements/ItemTransfer.xml': 'Libs/UI/UIElements/ItemTransfer.xml', # D68 glyph-only presentation
    'companion/source/Libs/UI/UIElements/ApseMap.xml': 'Libs/UI/UIElements/ApseMap.xml', # D68 glyph-only presentation
    'companion/source/Libs/UI/UIElements/ApseModalDialog.xml': 'Libs/UI/UIElements/ApseModalDialog.xml', # D68 glyph-only presentation
    'companion/source/Libs/UI/UIElements/ApsePlayerInfo.xml': 'Libs/UI/UIElements/ApsePlayerInfo.xml', # D68 glyph-only presentation
    'companion/source/Libs/UI/UIElements/GameOver.xml': 'Libs/UI/UIElements/GameOver.xml', # D68 glyph-only presentation

    'companion/source/Animations/humans/male/male.animevents': 'Animations/humans/male/male.animevents', # D66 exact one-event typo correction
    'companion/source/Libs/UI/hud.gfx': 'Libs/UI/hud.gfx', # D63 authored half-size notification containers
    'companion/source/Scripts/Startup/zzkcd2vr_petting_d59.lua': 'Scripts/Startup/zzkcd2vr_petting_d59.lua',
    'companion/source/Scripts/Startup/zzkcd2vr_wash_d59.lua': 'Scripts/Startup/zzkcd2vr_wash_d59.lua',
    'mods/KCD2VRMap/KCD2VRMapT2.gfx': 'Libs/UI/KCD2VRMapT2.gfx',
    'mods/KCD2VRMap/KCD2VRMapT2.xml': 'Libs/UI/UIElements/KCD2VRMapT2.xml',
    'companion/source/Scripts/Startup/zzkcd2vr_prompt_d56.lua': 'Scripts/Startup/zzkcd2vr_prompt_d56.lua',
    'companion/source/Scripts/Startup/zzkcd2vr_alchemy_name_d72.lua': 'Scripts/Startup/zzkcd2vr_alchemy_name_d72.lua',
    'companion/source/Scripts/Entities/Items/PickableItem.lua': 'Scripts/Entities/Items/PickableItem.lua',
    'mods/kcd2vr_tests/zzkcd2vr_phys.lua': 'Scripts/Startup/zzkcd2vr_phys.lua',
    'mods/KCD2VRStow/zzkcd2vr_grip_stow_cb1.lua': 'Scripts/Startup/zzkcd2vr_grip_stow_cb1.lua',
    'src/subsystems/throw_drop/resources/native_throw_reservation_t20.lua': 'Scripts/KCD2VR/native_throw_reservation_t20.lua',
    'companion/source/Libs/Config/defaultProfile.xml': 'Libs/Config/defaultProfile.xml',
    'companion/source/Scripts/Entities/AI/Shared/BasicAIActions.lua': 'Scripts/Entities/AI/Shared/BasicAIActions.lua',
    'src/subsystems/throw_drop/resources/startup_supply_t22.lua': 'Scripts/Startup/zzkcd2vr_supply_t15.lua',
    'src/subsystems/throw_drop/resources/perk__kcd2vr_throwing_assets1.xml': 'Libs/Tables/rpg/perk__kcd2vr_throwing_assets1.xml',
    'src/subsystems/throw_drop/resources/perk_script__kcd2vr_throwing_assets1.xml': 'Libs/Tables/rpg/perk_script__kcd2vr_throwing_assets1.xml',
    'mods/KCD2VRMap/map_catalog_static_m41.lua': 'Scripts/KCD2VR/map_catalog_static_m41.lua',
    'mods/KCD2VRMap/map_runtime.lua': 'Scripts/Startup/zzkcd2vr_map.lua',
    'mods/KCD2VRDoors/door_runtime.lua': 'Scripts/Startup/zzkcd2vr_doors.lua',
    'mods/KCD2VRDoors/hand_only_h40.lua': 'Scripts/Startup/zzkcd2vr_hand_only_doors_h40.lua',
    'mods/KCD2VRMap/map_navigation_t2.lua': 'Scripts/KCD2VR/map_navigation_t2.lua',
    'mods/KCD2VRMap/map_native_read_h25.lua': 'Scripts/KCD2VR/map_native_read_h25.lua',
    'mods/KCD2VRMap/map_presentation_m31.lua': 'Scripts/KCD2VR/map_presentation_m31.lua',
    'mods/KCD2VRMap/map_letter_d44.lua': 'Scripts/KCD2VR/map_letter_d44.lua',
    'src/subsystems/throw_drop/resources/startup_reservation_t22.lua': 'Scripts/KCD2VR/native_supply_t2.lua',
    'src/subsystems/throw_drop/resources/inventory_delivery_t26.lua': 'Scripts/Startup/zzkcd2vr_throwing_assets1.lua',
    'companion/Mods/kcd2vr_lab/Scripts/Startup/zzkcd2vr_lab.lua': 'Scripts/Startup/zzkcd2vr_lab.lua',
    'companion/source/AI/npc/basic/switch/runDialog.xml': 'AI/npc/basic/switch/runDialog.xml',
    'companion/source/Scripts/Startup/zzkcd2vr_chest_sheath_d33.lua': 'Scripts/Startup/zzkcd2vr_chest_sheath_d33.lua',
}
# Other mandatory members must already exist in the verified donor; a typo is
# not permission to create a second script under a similar path.
ALLOWED_ADDITIONS = frozenset({
    'Scripts/Startup/zzkcd2vr_chat_target_d78.lua',
    'Scripts/Startup/zzkcd2vr_corpse_d72.lua',
    'Scripts/Startup/zzkcd2vr_life_d73.lua',
    'Scripts/Startup/zzkcd2vr_alchemy_sound_d80.lua',
    'Scripts/Startup/zzkcd2vr_swing_whoosh_r116.lua',
    'Scripts/Startup/zzkcd2vr_quest_diag_r158.lua',
    'AI/npc/basic/switch/switch.xml',
    'AI/npc/basic/switch/kcd2vrlifelookd73.xml',

    'Animations/Mannequin/ADB/kcd_male_combat_database.adb',
    'Libs/UI/Menu.gfx',
    'Libs/UI/ItemSelection.gfx',
    'Libs/UI/ItemTransfer.gfx',
    'Libs/UI/ApseMap.gfx',
    'Libs/UI/ApseModalDialog.gfx',
    'Libs/UI/ApsePlayerInfo.gfx',
    'Libs/UI/GameOver.gfx',
    'Libs/UI/buttons.gfx',
    'Libs/UI/UIElements/HUD.xml',
    'Libs/UI/UIElements/Menu.xml',
    'Libs/UI/UIElements/ItemSelection.xml',
    'Libs/UI/UIElements/ItemTransfer.xml',
    'Libs/UI/UIElements/ApseMap.xml',
    'Libs/UI/UIElements/ApseModalDialog.xml',
    'Libs/UI/UIElements/ApsePlayerInfo.xml',
    'Libs/UI/UIElements/GameOver.xml',

    'Animations/humans/male/male.animevents',
    'Libs/UI/hud.gfx',
    'Scripts/Startup/zzkcd2vr_petting_d59.lua',
    'Scripts/Startup/zzkcd2vr_wash_d59.lua',
    'Scripts/KCD2VR/map_catalog_static_m41.lua',
    'Scripts/Startup/zzkcd2vr_prompt_d56.lua',
    'Scripts/Startup/zzkcd2vr_alchemy_name_d72.lua',
    'Scripts/KCD2VR/map_letter_d44.lua',
    'Scripts/Entities/Items/PickableItem.lua',
    'AI/npc/basic/switch/runDialog.xml',
    'Scripts/Startup/zzkcd2vr_chest_sheath_d33.lua',
})

# D74 optional adult mod: never inserted into the main companion.
GORE_SOURCE_PAIRS = {
    'mods/KCD2VRGore/r93r3/zzkcd2vr_gore.lua': 'Scripts/Startup/zzkcd2vr_gore.lua',
}
