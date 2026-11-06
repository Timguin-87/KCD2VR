cmake_minimum_required(VERSION 3.20)
project(KCD2VR_V66104_Contracts LANGUAGES CXX)
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/../src")
add_compile_definitions(NOMINMAX)
if(MSVC)
 add_compile_options("/FI${CMAKE_CURRENT_SOURCE_DIR}/host_test_runtime.h")
endif()
set(CMAKE_CXX_STANDARD 23) # Match current production headers; historical C++17 fixtures include C++20/23 APIs.
set(CMAKE_CXX_STANDARD_REQUIRED ON)
if(MSVC)
  add_compile_options(/UNDEBUG)
  option(KCD2VR_HOST_ASAN "AddressSanitizer for host fixtures (RelWithDebInfo)" OFF)
  if(KCD2VR_HOST_ASAN)
    add_compile_options(/fsanitize=address /Zi)
    add_link_options(/INCREMENTAL:NO /DEBUG)
  endif()
endif()

option(KCD2VR_D87_DLSS_ONLY "Configure temporal bootstrap and actual NGX/view regressions" OFF)
if(KCD2VR_D87_DLSS_ONLY)
 enable_testing()
 include(d87_dlss_tests.cmake)
 return()
endif()

# D84 selected consumers avoid obsolete historical whole-suite generators.
option(KCD2VR_D84_FOCUSED_ONLY "Configure D84 current regression contracts" OFF)
if(KCD2VR_D84_FOCUSED_ONLY)
 enable_testing()
 find_package(Threads REQUIRED)
 include(d84_tests.cmake)
 return()
endif()

# Current small repair fixtures can run without configuring historical extractors.
option(KCD2VR_D80_FOCUSED_ONLY "Configure current D80 repair policies and adapter" OFF)
if(KCD2VR_D80_FOCUSED_ONLY)
  enable_testing()
  find_package(Threads REQUIRED)
  include(d80_tests.cmake)
  return()
endif()

# Current menu defaults, dialogue mask lifetime, and instant-outcome reporting.
option(KCD2VR_D79_FOCUSED_ONLY "Configure current D79 portable policies" OFF)
if(KCD2VR_D79_FOCUSED_ONLY)
 enable_testing()
 foreach(suite live_settings_d39 dialogue_ratio_d79)
  add_executable(${suite}_test ${suite}_test.cpp)
  target_compile_features(${suite}_test PRIVATE cxx_std_20)
  add_test(NAME ${suite} COMMAND ${suite}_test)
 endforeach()
 add_executable(instant_outcome_d79_test subsystems/alchemy/instant_outcome_d79_test.cpp)
 target_compile_features(instant_outcome_d79_test PRIVATE cxx_std_20)
 add_test(NAME instant_outcome_d79 COMMAND instant_outcome_d79_test)
 return()
endif()

# D67 verified repairs plus retained input/gesture/held-weapon contracts.
option(KCD2VR_D67_FOCUSED_ONLY "Configure D67 integration and retained guards" OFF)
option(KCD2VR_D67R1_FOCUSED_ONLY "Configure D67R1 repairs and retained D67 guards" OFF)
if(KCD2VR_D67_FOCUSED_ONLY OR KCD2VR_D67R1_FOCUSED_ONLY)
 enable_testing()
 include(d65_repair_tests.cmake)
 foreach(suite motion_trace_d68 tracking_trace_d68 thrust_tuning_d67 motion_trials_d67 held_motion_d67 contact_fairness_d67 alchemy_stale_binding_d67 smith_recovery_d67 smith_station_policy_d67 early_frame_pacing_d45 paper_grip_d64 live_settings_d39 performance_metrics_d61 weapon_dynamics177 two_hand_axis59c16 knife_pouch_d66 alchemy_transfer_runtime_d65)
  if(suite STREQUAL "alchemy_transfer_runtime_d65" AND NOT MSVC)
   # This includes the complete native owner TU and Windows SEH leaves. It is
   # not a portable policy fixture; do not replace __finally with a fake catch.
   message(STATUS "alchemy_transfer_runtime_d65 requires Windows/MSVC; not run on this host")
   continue()
  endif()
  add_executable(${suite}_test ${suite}_test.cpp)
  target_compile_features(${suite}_test PRIVATE cxx_std_20)
  target_include_directories(${suite}_test PRIVATE ../src ../src/vrik syntax_shims "${CMAKE_CURRENT_BINARY_DIR}/d70_roomscale")
  if(MSVC)
   target_compile_options(${suite}_test PRIVATE /bigobj)
   target_link_options(${suite}_test PRIVATE /STACK:16777216)
  endif()
  if(suite STREQUAL "motion_trials_d67")
   add_test(NAME ${suite} COMMAND ${suite}_test "${CMAKE_CURRENT_SOURCE_DIR}/data/d67_elbow_counterexample.txt")
  else()
   add_test(NAME ${suite} COMMAND ${suite}_test)
  endif()
 endforeach()
 # D69 exact readable-prompt owner, no duplicated implementation in the fixture.
 execute_process(COMMAND "${Python3_EXECUTABLE}" "${CMAKE_CURRENT_SOURCE_DIR}/../tools/extract_item_prompt_owner_d69.py"
  RESULT_VARIABLE D69_PROMPT_EXTRACT_RESULT)
 if(NOT D69_PROMPT_EXTRACT_RESULT EQUAL 0)
  message(FATAL_ERROR "Could not extract current readable-prompt owner")
 endif()
 add_executable(hud_item_prompt_d69_test hud_item_prompt_d69_test.cpp)
 target_compile_features(hud_item_prompt_d69_test PRIVATE cxx_std_20)
 target_include_directories(hud_item_prompt_d69_test PRIVATE ../src .)
 add_test(NAME hud_item_prompt_d69 COMMAND hud_item_prompt_d69_test)
 # Execute the actual current adapter in the rigid-contact counterexample.
 # Do not silently test a generated copy left over from the D67 baseline.
 execute_process(COMMAND "${Python3_EXECUTABLE}" "${CMAKE_CURRENT_SOURCE_DIR}/../tools/extract_world_adapter_test.py"
  RESULT_VARIABLE D67R1_ADAPTER_EXTRACT_RESULT)
 if(NOT D67R1_ADAPTER_EXTRACT_RESULT EQUAL 0)
  message(FATAL_ERROR "Could not extract the current D67R1 contact adapter")
 endif()
 execute_process(COMMAND "${Python3_EXECUTABLE}" "${CMAKE_CURRENT_SOURCE_DIR}/../tools/extract_roomscale_d53.py" --out "${CMAKE_CURRENT_BINARY_DIR}/d70_roomscale"
  RESULT_VARIABLE D70_ROOMSCALE_EXTRACT_RESULT)
 if(NOT D70_ROOMSCALE_EXTRACT_RESULT EQUAL 0)
  message(FATAL_ERROR "Could not extract current roomscale owner")
 endif()
 find_package(Threads REQUIRED)
 foreach(suite contact_continuity_d71 item_handover_d71 crafting_d70 smith_lift_d70 selector_palm_d70 plate_left_d70 draw_handover_d70 dodge_window_d70 roomscale_lean_d70 claude_bundle_d70 committed_stroke_h8 master_gate47 hud_transition170 hand_prediction_d69 elbow_hysteresis_d69 weapon_trials_d69 weapon_same_frame_d69 crossbow_start_leaf_d69 publication_wait_d67r1 native_palette_d67r1 native_palette_view_d67r1 weapon_sweep_d67r1 rigid_contact_d67r1 smith_recovery_lifetime_d67r1 alchemy_resource_scope_d67r1 contact_locality_d64 contact_continuity_d63 sweep169r2 initial_acquire_sweep_d44 root_turn_sweep_d44)
  add_executable(${suite}_test ${suite}_test.cpp)
  target_compile_features(${suite}_test PRIVATE cxx_std_20)
  if(suite STREQUAL "publication_wait_d67r1")
   # Pure std::future/thread fixture must use MSVC's real intrinsics, not the
   # legacy native-adapter intrin.h shim that omits _ReturnAddress for PPL.
   target_include_directories(${suite}_test PRIVATE ../src)
  else()
  target_include_directories(${suite}_test PRIVATE ../src ../src/vrik syntax_shims "${CMAKE_CURRENT_BINARY_DIR}/d70_roomscale")
  endif()
  target_link_libraries(${suite}_test PRIVATE Threads::Threads)
  if(suite STREQUAL "initial_acquire_sweep_d44")
   target_compile_definitions(${suite}_test PRIVATE D44_INITIAL_SWEEP)
  endif()
  if(MSVC)
   target_compile_options(${suite}_test PRIVATE /bigobj)
   target_link_options(${suite}_test PRIVATE /STACK:16777216)
  endif()
  if(suite STREQUAL "native_palette_d67r1" OR suite STREQUAL "native_palette_view_d67r1")
   add_test(NAME ${suite} COMMAND ${suite}_test "${CMAKE_CURRENT_SOURCE_DIR}/fixtures/${suite}_abi.txt")
  else()
   add_test(NAME ${suite} COMMAND ${suite}_test)
  endif()
 endforeach()
 include(d73_tests.cmake)
 include(d74_hud_tests.cmake)
 include(d75_tests.cmake)
 include(d76_tests.cmake)
include(d77_tests.cmake)
 return()
endif()

# D65 corrected donor ownership and current-menu roundtrip.
option(KCD2VR_D65_FOCUSED_ONLY "Configure D65 corrected ownership and menu fixtures" OFF)
if(KCD2VR_D65_FOCUSED_ONLY)
  enable_testing()
  include(d65_repair_tests.cmake)
  add_executable(knife_pouch_d66_test knife_pouch_d66_test.cpp)
  target_compile_features(knife_pouch_d66_test PRIVATE cxx_std_20)
  add_test(NAME knife_pouch_d66 COMMAND knife_pouch_d66_test)
  add_executable(live_settings_d39_test live_settings_d39_test.cpp)
  add_test(NAME live_settings_d39 COMMAND live_settings_d39_test)
  return()
endif()

# D64 focused production helpers; retained suites are run independently.
option(KCD2VR_D64_FOCUSED_ONLY "Configure D64 measured-regression helpers" OFF)
if(KCD2VR_D64_FOCUSED_ONLY)
  enable_testing()
  foreach(suite elbow_target_d64 early_frame_pacing_d45 fist_phase_d64 fist_motion fist_guard175 fist_rearm_d53 fist_stroke_d49 weapon_grip_continuity_d64 pickup_grip_d64 paper_grip_d64)
    add_executable(${suite}_test ${suite}_test.cpp)
    target_include_directories(${suite}_test PRIVATE ../src/vrik)
    target_compile_features(${suite}_test PRIVATE cxx_std_20)
    add_test(NAME ${suite} COMMAND ${suite}_test)
  endforeach()
  return()
endif()

# D62 current-source integration suite, independent of retired extractors.
option(KCD2VR_D62_FOCUSED_ONLY "Configure the D62 integration and retained guards" OFF)
option(KCD2VR_D63_FOCUSED_ONLY "Configure D63 repairs and retained D62 guards" OFF)
if(KCD2VR_D62_FOCUSED_ONLY OR KCD2VR_D63_FOCUSED_ONLY)
  enable_testing()
  find_package(Python3 COMPONENTS Interpreter REQUIRED)
  if(NOT MSVC)
    option(KCD2VR_D62_SANITIZERS "Enable ASAN/UBSAN for D62 host tests" ON)
    if(KCD2VR_D62_SANITIZERS)
      add_compile_options(-fsanitize=address,undefined -fno-omit-frame-pointer -fno-pie)
      add_link_options(-fsanitize=address,undefined -no-pie)
    endif()
  endif()
  include("${CMAKE_CURRENT_SOURCE_DIR}/d62_crafting_tests.cmake")
  include("${CMAKE_CURRENT_SOURCE_DIR}/d62_config_tests.cmake")
  foreach(retained hud_phase_cost_p1 performance_metrics_d61)
    add_executable(${retained}_test ${retained}_test.cpp)
    target_compile_features(${retained}_test PRIVATE cxx_std_20)
    add_test(NAME ${retained} COMMAND ${retained}_test)
  endforeach()
  target_include_directories(hud_phase_cost_p1_test PRIVATE ui175/mocks syntax_shims openxr_sdk)
  if(NOT MSVC)
    target_compile_features(hud_phase_cost_p1_test PRIVATE cxx_std_23)
    target_compile_options(hud_phase_cost_p1_test PRIVATE -fms-extensions -ffunction-sections -fdata-sections -Wno-subobject-linkage)
    target_compile_definitions(hud_phase_cost_p1_test PRIVATE _ReturnAddress=HostReturnAddress __fastcall=)
    target_link_options(hud_phase_cost_p1_test PRIVATE -Wl,--gc-sections)
    target_link_libraries(hud_phase_cost_p1_test PRIVATE Threads::Threads)
  endif()
  add_test(NAME hand_present_d62_production COMMAND "${Python3_EXECUTABLE}"
    "${CMAKE_CURRENT_SOURCE_DIR}/../tools/run_hand_present_d62.py"
    --out "${CMAKE_CURRENT_BINARY_DIR}/d62_hand_present")
  # The extraction runners below currently use GCC's host ABI/shims. Keep that
  # fact explicit; the Windows DLL is built and linked separately at repo root.
  if(NOT WIN32)
    add_test(NAME combat_d62_production COMMAND "${Python3_EXECUTABLE}"
      "${CMAKE_CURRENT_SOURCE_DIR}/../tools/run_combat_integration_d62.py"
      --out "${CMAKE_CURRENT_BINARY_DIR}/d62_combat")
    add_test(NAME interaction_d62_production COMMAND "${Python3_EXECUTABLE}"
      "${CMAKE_CURRENT_SOURCE_DIR}/../tools/run_interaction_integration_d62.py"
      --out "${CMAKE_CURRENT_BINARY_DIR}/d62_interaction")
    add_test(NAME interaction_retained_production COMMAND "${Python3_EXECUTABLE}"
      "${CMAKE_CURRENT_SOURCE_DIR}/../tools/run_interactions_d56.py"
      --out "${CMAKE_CURRENT_BINARY_DIR}/d62_retained_interaction"
      --only contact_corner_d56,contact_reseed_d53,contact_occupancy_d56,hand_pair_contact_d39,acquisition_foundation_d56)
    set_tests_properties(combat_d62_production interaction_d62_production interaction_retained_production PROPERTIES RUN_SERIAL TRUE)
  endif()
  set_tests_properties(hand_present_d62_production PROPERTIES RUN_SERIAL TRUE)
  if(KCD2VR_D63_FOCUSED_ONLY)
    include("${CMAKE_CURRENT_SOURCE_DIR}/d63_regression_tests.cmake")
  endif()
  get_property(d62_tests DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}" PROPERTY TESTS)
  set_tests_properties(${d62_tests} PROPERTIES ENVIRONMENT "ASAN_OPTIONS=detect_leaks=0;UBSAN_OPTIONS=halt_on_error=1")
  return()
endif()

# D56 crafting production-policy regressions can run independently of retired extractors.
option(KCD2VR_D56_FOCUSED_ONLY "Configure the D56 integrated regression set" OFF)
if(KCD2VR_D56_FOCUSED_ONLY)
  enable_testing()
  find_package(Python3 COMPONENTS Interpreter REQUIRED)
  include("${CMAKE_CURRENT_SOURCE_DIR}/d56_crafting_tests.cmake")
  include("${CMAKE_CURRENT_SOURCE_DIR}/d56_combat_tests.cmake")
  add_test(NAME d56_interactions_production COMMAND "${Python3_EXECUTABLE}"
    "${CMAKE_CURRENT_SOURCE_DIR}/../tools/run_interactions_d56.py" --out "${CMAKE_CURRENT_BINARY_DIR}/interaction_production")
  add_test(NAME d56_prompt_refresh_production COMMAND "${Python3_EXECUTABLE}"
    "${CMAKE_CURRENT_SOURCE_DIR}/../tools/test_prompt_refresh_d56.py" --out "${CMAKE_CURRENT_BINARY_DIR}/prompt_production")
  add_test(NAME d56_live_preferences_production COMMAND "${Python3_EXECUTABLE}"
    "${CMAKE_CURRENT_SOURCE_DIR}/../tools/run_live_runtime_d40.py" --out "${CMAKE_CURRENT_BINARY_DIR}/live_production")
  return()
endif()

option(KCD2VR_D56_CRAFTING_ONLY "Configure the D56 crafting regressions only" OFF)
if(KCD2VR_D56_CRAFTING_ONLY)
  enable_testing()
  find_package(Python3 COMPONENTS Interpreter REQUIRED)
  include("${CMAKE_CURRENT_SOURCE_DIR}/d56_crafting_tests.cmake")
  return()
endif()

# D55 focused adapters are independent of the retired legacy extractor chain.
option(KCD2VR_D55_FOCUSED_ONLY "Configure the D55 wrist/camera/station adapters only" OFF)
if(KCD2VR_D55_FOCUSED_ONLY)
  enable_testing()
  find_package(Python3 COMPONENTS Interpreter REQUIRED)
  include("${CMAKE_CURRENT_SOURCE_DIR}/d55_tests.cmake")
  return()
endif()

add_executable(gesture_recovery_h30_test gesture_recovery_h30_test.cpp)
add_test(NAME gesture_recovery_h30 COMMAND gesture_recovery_h30_test)
add_executable(gesture_owner_h30_test gesture_owner_h30_test.cpp)
add_test(NAME gesture_owner_h30 COMMAND gesture_owner_h30_test)
add_executable(gesture_cost_h30_test gesture_cost_h30_test.cpp)
add_test(NAME gesture_cost_h30 COMMAND gesture_cost_h30_test)

add_executable(ranged_request_h21_test ranged_request_h21_test.cpp)
add_test(NAME ranged_request_h21 COMMAND ranged_request_h21_test)
add_executable(fist_responsive_h27_test fist_responsive_h27_test.cpp)
add_test(NAME fist_responsive_h27 COMMAND fist_responsive_h27_test)
find_package(Threads REQUIRED)
add_executable(grip_binding59c10r3_test grip_binding59c10r3_test.cpp)
add_test(NAME grip_binding59c10r3 COMMAND grip_binding59c10r3_test)
add_executable(hud_transition170_test hud_transition170_test.cpp)
add_test(NAME hud_transition170 COMMAND hud_transition170_test)
add_executable(hud171_test hud171_test.cpp)
add_test(NAME hud171 COMMAND hud171_test)
if(MSVC)
 target_compile_options(hud171_test PRIVATE /UNDEBUG)
else()
 target_compile_options(hud171_test PRIVATE -UNDEBUG)
endif()
enable_testing()
option(KCD2VR_D53_FOCUSED_ONLY "Configure D53 fixtures without retired legacy extractors" OFF)
foreach(d53 forge_handle_d53 fist_rearm_d53 hand_reference_dedup_d53 capture_routes_d53)
 add_executable(${d53}_test ${d53}_test.cpp)
 target_compile_features(${d53}_test PRIVATE cxx_std_20)
 add_test(NAME ${d53} COMMAND ${d53}_test)
endforeach()
add_executable(mortar_destination_d53_test subsystems/alchemy/mortar_destination_d53_test.cpp)
target_compile_features(mortar_destination_d53_test PRIVATE cxx_std_20)
add_test(NAME mortar_destination_d53 COMMAND mortar_destination_d53_test)
find_package(Python3 COMPONENTS Interpreter REQUIRED)
foreach(extractor prompt_consumer_d53 roomscale_d53)
 execute_process(COMMAND "${Python3_EXECUTABLE}" "${CMAKE_CURRENT_SOURCE_DIR}/../tools/extract_${extractor}.py"
  --out "${CMAKE_CURRENT_BINARY_DIR}/generated_d53" COMMAND_ERROR_IS_FATAL ANY)
endforeach()
foreach(target prompt_consumer_d53_test roomscale_heading_d53_test)
 add_executable(${target} ${target}.cpp)
 target_compile_features(${target} PRIVATE cxx_std_20)
 target_include_directories(${target} PRIVATE "${CMAKE_CURRENT_BINARY_DIR}/generated_d53")
 add_test(NAME ${target} COMMAND ${target})
endforeach()
if(KCD2VR_D53_FOCUSED_ONLY)
 return()
endif()
if(WIN32)
  add_executable(pickup_windows_headers_d40r1_test pickup_windows_headers_d40r1_test.cpp)
  add_test(NAME pickup_windows_headers_d40r1 COMMAND pickup_windows_headers_d40r1_test)
endif()
add_executable(hip_selection_t10_test hip_selection_t10_test.cpp)
add_test(NAME hip_selection_t10 COMMAND hip_selection_t10_test)
add_executable(combat_h5_test combat_h5_test.cpp)
add_test(NAME combat_h5 COMMAND combat_h5_test)
add_executable(combat_contact_h26_test combat_contact_h26_test.cpp)
target_include_directories(combat_contact_h26_test PRIVATE ../src)
add_test(NAME combat_contact_h26 COMMAND combat_contact_h26_test)
add_executable(attack_zone_h28_test attack_zone_h28_test.cpp)
target_include_directories(attack_zone_h28_test PRIVATE ../src)
add_test(NAME attack_zone_h28 COMMAND attack_zone_h28_test)
add_executable(c54_cut_admission_h27_test c54_cut_admission_h27_test.cpp)
target_include_directories(c54_cut_admission_h27_test PRIVATE ../src)
add_test(NAME c54_cut_admission_h27 COMMAND c54_cut_admission_h27_test)
if(MSVC)
 target_link_options(combat_h5_test PRIVATE /STACK:8388608)
endif()
add_executable(combat_router_h3_test combat_router_h3_test.cpp)
add_test(NAME combat_router_h3 COMMAND combat_router_h3_test)
foreach(c2 contact_reservation_c2 finisher_observation_c2)
 add_executable(${c2}_test ${c2}_test.cpp)
 add_test(NAME ${c2} COMMAND ${c2}_test)
endforeach()


# H1 recognition only: actual weapon producer and both-frame stroke evidence.
add_executable(heavy_recognition_h1_test heavy_recognition_h1_test.cpp)
add_test(NAME heavy_recognition_h1 COMMAND heavy_recognition_h1_test)
add_executable(physical_target_clock_test physical_target_clock_test.cpp)
add_test(NAME physical_target_clock COMMAND physical_target_clock_test)
add_executable(activity_eye59c19_test activity_eye59c19_test.cpp)
add_test(NAME activity_eye59c19 COMMAND activity_eye59c19_test)
add_executable(ui_pause_page59c19_test ui_pause_page59c19_test.cpp)
add_test(NAME ui_pause_page59c19 COMMAND ui_pause_page59c19_test)
foreach(c7g body_combo59c7g part_anchor59c7g immersive_fov59c7g carry_snapshot59c7g)
 add_executable(${c7g}_test ${c7g}_test.cpp)
 add_test(NAME ${c7g} COMMAND ${c7g}_test)
endforeach()
add_executable(staged_hold596c2_test staged_hold596c2_test.cpp)
add_test(NAME staged_hold596c2 COMMAND staged_hold596c2_test)
add_executable(contact_receipt58c1_test contact_receipt58c1_test.cpp)
add_test(NAME contact_receipt58c1 COMMAND contact_receipt58c1_test)
add_executable(r44_intent_test r44_intent_test.cpp)
add_test(NAME r44_intent COMMAND r44_intent_test)
add_executable(kick_style26_test kick_style26_test.cpp) # C26 KICKS page
add_executable(vr_exposure26_test vr_exposure26_test.cpp) # C26d head-steady eye adaptation
add_test(NAME vr_exposure26 COMMAND vr_exposure26_test)
add_test(NAME kick_style26 COMMAND kick_style26_test)
add_executable(kick_tx37_test kick_tx37_test.cpp) # C37 VR leg kick transaction (one click, one hit)
add_test(NAME kick_tx37 COMMAND kick_tx37_test)
add_executable(defense_timing39_test defense_timing39_test.cpp) # C39 VR DIRECT skill-scaled announced contact ETA (waiting opponent only)
add_test(NAME defense_timing39 COMMAND defense_timing39_test)
if(MSVC)
  add_executable(npc_difficulty_test npc_difficulty_test.cpp)
  target_include_directories(npc_difficulty_test PRIVATE syntax_shims ../src)
  add_test(NAME npc_difficulty COMMAND npc_difficulty_test)
  add_executable(npc_unarmed_spacing_test npc_unarmed_spacing_test.cpp)
  target_include_directories(npc_unarmed_spacing_test PRIVATE syntax_shims ../src)
  add_test(NAME npc_unarmed_spacing COMMAND npc_unarmed_spacing_test)
  add_executable(npc_settings_test npc_settings_test.cpp)
  target_include_directories(npc_settings_test PRIVATE ../src)
  add_test(NAME npc_settings COMMAND npc_settings_test)
  add_executable(npc_defense_connected_test npc_defense_connected_test.cpp)
  target_include_directories(npc_defense_connected_test PRIVATE syntax_shims ../src)
  add_test(NAME npc_defense_connected COMMAND npc_defense_connected_test)
endif()
add_executable(combat_menu49_test combat_menu49_test.cpp) # C49 first EXPERIMENTS page + GRAPHICS shadow row layout / slider mapping; C45 kill-move row + IMMERSION grid
add_test(NAME combat_menu49 COMMAND combat_menu49_test)
add_executable(minigame_hands45_test minigame_hands45_test.cpp) # C45 MINIGAME HANDS pure gate + ownsActivityWrist45
target_include_directories(minigame_hands45_test PRIVATE ../src)
add_test(NAME minigame_hands45 COMMAND minigame_hands45_test)
add_executable(dirblock39_test dirblock39_test.cpp) # C39 DIRBLOCK directional NPC block: approach sector, combat_zone.xml mapping, mailbox, bounded updates
add_test(NAME dirblock39 COMMAND dirblock39_test)
add_executable(gore32_test gore32_test.cpp) # C32 impale hang + decapitation policy
add_executable(impale40_test impale40_test.cpp) # C40 impale by energy + native zone armour: eligibility, zone, depth model, living STUCK release
add_test(NAME impale40 COMMAND impale40_test)
add_executable(impale40_weapon_test impale40_weapon_test.cpp) # C40 weapon side: non-lethal thrust latch only with impale40, STUCK display depth cap + wound line, pin-lost release
add_test(NAME impale40_weapon COMMAND impale40_weapon_test)
add_executable(impale_hang44_test impale_hang44_test.cpp) # C44 weapon side: the hanging dead body's weight sags the weapon target (VrImpaleHangMassKg), ramps out on release, never for STUCK
add_test(NAME impale_hang44 COMMAND impale_hang44_test)
add_executable(impale47_test impale47_test.cpp) # C47 late lethal receipt -> blade-line stuck point, bounded death-ragdoll wait, decap zone + part->joint mapping
add_test(NAME impale47 COMMAND impale47_test)
add_executable(blood49_test blood49_test.cpp) # C49 BLOODY FIGHTS: hit qualification, once-per-hit gate, rows/zones, kill FIFO, gush scheduler
add_test(NAME blood49 COMMAND blood49_test)
add_test(NAME gore32 COMMAND gore32_test)
add_executable(gorepak50_test gorepak50_test.cpp) # C50 separate optional gore pak: limb cut, skin filter, Lua event
add_test(NAME gorepak50 COMMAND gorepak50_test)
add_executable(early_warning33_test early_warning33_test.cpp) # C33 DIRECT backswing early warning
add_test(NAME early_warning33 COMMAND early_warning33_test)
add_executable(master_handoff23_test master_handoff23_test.cpp)
add_test(NAME master_handoff23 COMMAND master_handoff23_test)
add_executable(outcome_chain23_test outcome_chain23_test.cpp) # C23b OUTCOME_CHAIN receipt builder
add_test(NAME outcome_chain23 COMMAND outcome_chain23_test)
add_executable(rung1_recovery30c1_test rung1_recovery30c1_test.cpp)
add_test(NAME rung1_recovery30c1 COMMAND rung1_recovery30c1_test)
add_executable(align_veto_receipts24_test align_veto_receipts24_test.cpp) # C25 veto receipts ledger
add_test(NAME align_veto_receipts24 COMMAND align_veto_receipts24_test)
add_executable(attack_timing178r22c2_test attack_timing178r22c2_test.cpp)
add_test(NAME attack_timing178r22c2 COMMAND attack_timing178r22c2_test)
add_executable(weapon_dynamics177_test weapon_dynamics177_test.cpp)
add_test(NAME weapon_dynamics177 COMMAND weapon_dynamics177_test)
add_executable(regression112_test regression112_test.cpp)
add_test(NAME regression112 COMMAND regression112_test)
add_executable(regression116_test regression116_test.cpp)
add_test(NAME regression116 COMMAND regression116_test)
add_executable(regression117_test regression117_test.cpp)
add_test(NAME regression117 COMMAND regression117_test)
add_executable(melee118_test melee118_test.cpp)
add_test(NAME melee118 COMMAND melee118_test)
add_executable(melee119_test melee119_test.cpp)
add_test(NAME melee119 COMMAND melee119_test)
add_executable(regression121_test regression121_test.cpp)
add_test(NAME regression121 COMMAND regression121_test)
add_executable(regression124_test regression124_test.cpp)
add_test(NAME regression124 COMMAND regression124_test)
add_executable(tracking_origin125_test tracking_origin125_test.cpp)
add_test(NAME tracking_origin125 COMMAND tracking_origin125_test)
add_executable(bow126_test bow126_test.cpp)
add_test(NAME bow126 COMMAND bow126_test)
add_executable(constraint_drive129_test constraint_drive129_test.cpp)
add_test(NAME constraint_drive129 COMMAND constraint_drive129_test)
add_executable(support_constraint130_test support_constraint130_test.cpp)
add_test(NAME support_constraint130 COMMAND support_constraint130_test)
add_executable(holster131_test holster131_test.cpp)
add_test(NAME holster131 COMMAND holster131_test)
add_executable(physical_handoff132_test physical_handoff132_test.cpp)
add_test(NAME physical_handoff132 COMMAND physical_handoff132_test)
add_executable(holster136_test holster136_test.cpp)
add_test(NAME holster136 COMMAND holster136_test)
add_executable(holster_location_h42_test holster_location_h42_test.cpp)
add_test(NAME holster_location_h42 COMMAND holster_location_h42_test)
add_executable(holster_user_layout152_test holster_user_layout152_test.cpp)
add_test(NAME holster_user_layout152 COMMAND holster_user_layout152_test)
add_executable(impact_haptics133_test impact_haptics133_test.cpp)
add_test(NAME impact_haptics133 COMMAND impact_haptics133_test)
add_executable(haptics136r1_test haptics136r1_test.cpp)
add_test(NAME haptics136r1 COMMAND haptics136r1_test)
add_executable(deferred_impact136r2_test deferred_impact136r2_test.cpp)
add_test(NAME deferred_impact136r2 COMMAND deferred_impact136r2_test)
if(MSVC)
 add_executable(readable_pages178r22c1_test readable_pages178r22c1_test.cpp)
 target_link_libraries(readable_pages178r22c1_test PRIVATE psapi)
 add_test(NAME readable_pages178r22c1 COMMAND readable_pages178r22c1_test)
 add_executable(resident_pages126_test resident_pages126_test.cpp)
 target_link_libraries(resident_pages126_test PRIVATE psapi)
 add_test(NAME resident_pages126 COMMAND resident_pages126_test)
endif()
add_executable(hook_cost178r22c1_test hook_cost178r22c1_test.cpp)
add_test(NAME hook_cost178r22c1 COMMAND hook_cost178r22c1_test)
add_executable(environment_fx178r22c1_test environment_fx178r22c1_test.cpp)
add_test(NAME environment_fx178r22c1 COMMAND environment_fx178r22c1_test)
find_package(Python3 COMPONENTS Interpreter REQUIRED)
# D52: exact changed production boundaries, generated caller fragments stay in build output.
foreach(d52 contact_occupancy_d52 gore_skin_path_d52 gore_lifecycle_d52 dialogue_participants_d52 smith_contact_dispatch_repair smith_tool_contact_repair capture_routes_d52)
 add_executable(${d52}_test ${d52}_test.cpp)
 target_compile_features(${d52}_test PRIVATE cxx_std_20)
 add_test(NAME ${d52} COMMAND ${d52}_test)
endforeach()
execute_process(COMMAND "${Python3_EXECUTABLE}" "${CMAKE_CURRENT_SOURCE_DIR}/../tools/extract_smith_wrist_d52.py"
 --out "${CMAKE_CURRENT_BINARY_DIR}/smith_d52" COMMAND_ERROR_IS_FATAL ANY)
add_executable(smith_final_wrist_d52_test smith_final_wrist_d52_test.cpp)
target_compile_features(smith_final_wrist_d52_test PRIVATE cxx_std_20)
target_include_directories(smith_final_wrist_d52_test PRIVATE "${CMAKE_CURRENT_BINARY_DIR}/smith_d52" "${CMAKE_CURRENT_SOURCE_DIR}")
add_test(NAME smith_final_wrist_d52 COMMAND smith_final_wrist_d52_test)
# T24 standalone consumers use the exact private claim helper, with explicit native identity leaves.
set_property(DIRECTORY APPEND PROPERTY CMAKE_CONFIGURE_DEPENDS
 ../src/frik_pose_probe.cpp ../src/subsystems/throw_drop/hand_claim_t24.h
 ../tools/extract_hand_claim_fixture_t24.py)
execute_process(COMMAND "${Python3_EXECUTABLE}" "${CMAKE_CURRENT_SOURCE_DIR}/../tools/extract_hand_claim_fixture_t24.py" COMMAND_ERROR_IS_FATAL ANY)
execute_process(COMMAND "${Python3_EXECUTABLE}" "${CMAKE_CURRENT_SOURCE_DIR}/../tools/extract_intent_frame_h8.py"
 "${CMAKE_CURRENT_BINARY_DIR}/intent_frame_h8" COMMAND_ERROR_IS_FATAL ANY)
add_executable(intent_frame_h8_test intent_frame_h8_test.cpp)
target_include_directories(intent_frame_h8_test PRIVATE "${CMAKE_CURRENT_BINARY_DIR}/intent_frame_h8")
target_compile_definitions(intent_frame_h8_test PRIVATE KCD2VR_HAS_INTENT_FRAME_H8=1)
add_test(NAME intent_frame_h8 COMMAND intent_frame_h8_test)
# H8 ordinary public-Hybrid source identity and exact current native actor lookup.
# The stroke fixture includes the full production kernel. The actor fixture
# compiles exact production wrappers/callbacks regenerated into build output.
add_executable(committed_stroke_h8_test committed_stroke_h8_test.cpp)
add_test(NAME committed_stroke_h8 COMMAND committed_stroke_h8_test)
execute_process(COMMAND "${Python3_EXECUTABLE}" "${CMAKE_CURRENT_SOURCE_DIR}/../tools/extract_actor_lookup_h8.py"
 "${CMAKE_CURRENT_BINARY_DIR}/actor_lookup_h8" COMMAND_ERROR_IS_FATAL ANY)
set_property(DIRECTORY APPEND PROPERTY CMAKE_CONFIGURE_DEPENDS
 ../src/diagnostics/geometry_hit178r27.inl ../src/diagnostics/native_hit_probe134.inl
 ../src/diagnostics/actor_lookup_h7.h ../src/subsystems/direct/actor_update_observer.inl
 ../tools/extract_actor_lookup_h8.py ../tools/source_text.py)
add_executable(actor_lookup_h8_test actor_lookup_h8_test.cpp)
target_include_directories(actor_lookup_h8_test PRIVATE "${CMAKE_CURRENT_BINARY_DIR}/actor_lookup_h8")
add_test(NAME actor_lookup_h8 COMMAND actor_lookup_h8_test)
execute_process(COMMAND "${Python3_EXECUTABLE}" "${CMAKE_CURRENT_SOURCE_DIR}/../tools/extract_phys_bodies19c1.py"
  "${CMAKE_CURRENT_BINARY_DIR}/phys_bodies_adapter19c1.inc" RESULT_VARIABLE PHYS_BODIES19C1_EXTRACT)
if(NOT PHYS_BODIES19C1_EXTRACT EQUAL 0)
 message(FATAL_ERROR "Could not extract current physics proxy producer/cleanup")
endif()
set_property(DIRECTORY APPEND PROPERTY CMAKE_CONFIGURE_DEPENDS
 ../src/vrik/phys_bodies59c10r8.inl ../src/vrik/physical_native.inl ../src/frik_pose_probe.cpp
 ../tools/extract_phys_bodies19c1.py)
add_executable(phys_bodies_adapter19c1_test phys_bodies_adapter19c1_test.cpp)
target_include_directories(phys_bodies_adapter19c1_test PRIVATE "${CMAKE_CURRENT_BINARY_DIR}")
add_test(NAME phys_bodies_adapter19c1 COMMAND phys_bodies_adapter19c1_test)
add_executable(palm_reveal_lifetime_test palm_reveal_lifetime_test.cpp)
add_test(NAME palm_reveal_lifetime COMMAND palm_reveal_lifetime_test)
add_executable(swing_effort_key19_test swing_effort_key19_test.cpp)
add_test(NAME swing_effort_key19 COMMAND swing_effort_key19_test)
add_executable(physical_effort_packet19_test physical_effort_packet19_test.cpp)
add_test(NAME physical_effort_packet19 COMMAND physical_effort_packet19_test)
execute_process(COMMAND "${Python3_EXECUTABLE}" "${CMAKE_CURRENT_SOURCE_DIR}/../tools/extract_combo_lineage19.py"
 "${CMAKE_CURRENT_BINARY_DIR}/combo_admitted_direct_r17.inc" RESULT_VARIABLE COMBO_LINEAGE19_EXTRACT)
if(NOT COMBO_LINEAGE19_EXTRACT EQUAL 0)
 message(FATAL_ERROR "Could not extract current combo grace consumer")
endif()
set_property(DIRECTORY APPEND PROPERTY CMAKE_CONFIGURE_DEPENDS ../src/subsystems/masterstrike_combos/native_combo_grace.inl ../src/frik_pose_probe.cpp)
set_property(DIRECTORY APPEND PROPERTY CMAKE_CONFIGURE_DEPENDS ../src/subsystems/hybrid/drain_live_delivery.inl ../tools/extract_combo_lineage19.py)
add_executable(combo_lineage19_test combo_lineage19_test.cpp)
add_test(NAME combo_lineage19 COMMAND combo_lineage19_test)
add_executable(combo_step_window_cb2_test combo_step_window_cb2_test.cpp)
add_test(NAME combo_step_window_cb2 COMMAND combo_step_window_cb2_test)
add_executable(combo_public_rearm_h8_test combo_public_rearm_h8_test.cpp)
add_test(NAME combo_public_rearm_h8 COMMAND combo_public_rearm_h8_test)
add_executable(combo_publication_h8_test combo_publication_h8_test.cpp)
add_test(NAME combo_publication_h8 COMMAND combo_publication_h8_test)
add_executable(defense_publication19_test defense_publication19_test.cpp)
add_test(NAME defense_publication19 COMMAND defense_publication19_test)
add_executable(dialogue_presentation19c2_test dialogue_presentation19c2_test.cpp)
add_test(NAME dialogue_presentation19c2 COMMAND dialogue_presentation19c2_test)
execute_process(COMMAND "${Python3_EXECUTABLE}" "${CMAKE_CURRENT_SOURCE_DIR}/../tools/extract_adapter_types.py" RESULT_VARIABLE TYPES_EXTRACT)
if(NOT TYPES_EXTRACT EQUAL 0)
 message(FATAL_ERROR "Could not extract current native adapter types")
endif()
execute_process(COMMAND "${Python3_EXECUTABLE}" "${CMAKE_CURRENT_SOURCE_DIR}/../tools/extract_world_adapter_test.py" RESULT_VARIABLE WORLD_EXTRACT)
if(NOT WORLD_EXTRACT EQUAL 0)
  message(FATAL_ERROR "Could not extract current production world adapter")
endif()
execute_process(COMMAND "${Python3_EXECUTABLE}" "${CMAKE_CURRENT_SOURCE_DIR}/../tools/extract_controller_test.py" RESULT_VARIABLE CONTROLLER_EXTRACT)
if(NOT CONTROLLER_EXTRACT EQUAL 0)
 message(FATAL_ERROR "Could not extract current controller adapter")
endif()
execute_process(COMMAND "${Python3_EXECUTABLE}" "${CMAKE_CURRENT_SOURCE_DIR}/../tools/extract_physical_frame_test.py" RESULT_VARIABLE FRAME_EXTRACT)
if(NOT FRAME_EXTRACT EQUAL 0)
 message(FATAL_ERROR "Could not extract current physical frame/input adapters")
endif()
execute_process(COMMAND "${Python3_EXECUTABLE}" "${CMAKE_CURRENT_SOURCE_DIR}/../tools/extract_regression_test.py" RESULT_VARIABLE REGRESSION_EXTRACT)
if(NOT REGRESSION_EXTRACT EQUAL 0)
 message(FATAL_ERROR "Could not extract production regression paths")
endif()
foreach(name comparison114 theater_exit113 capture113 pose_math pose_adapter c54_foundation_restore_h26 controller_adapter state_queue playback_lab combo_snapshot world_adapter snapshot_mailbox elbow_continuity trigger_ownership physical_weapon physical_frame physical_input frame_gate finger_capture calibration_profile arm_recovery fist_motion quantized_fists physical_feel_v66102 merge103 integration104 physical_interaction106 v66107 combat_ownership111 interaction_ownership111 tracking_grace111 async_save25 inventory_early_exit25)
  add_executable(${name}_test ${name}_test.cpp)
  target_link_libraries(${name}_test PRIVATE Threads::Threads)
  add_test(NAME ${name} COMMAND ${name}_test)
endforeach()
# These tests execute the active portable kernel and the production pose/F12 adapters
# with in-memory native API replacements. They do not load WHGame or emulate Windows.

add_executable(xr_haptics_test xr_haptics_test.cpp)
target_include_directories(xr_haptics_test PRIVATE syntax_shims openxr_sdk)
target_compile_definitions(xr_haptics_test PRIVATE __fastcall=)
target_link_libraries(xr_haptics_test PRIVATE Threads::Threads)
add_test(NAME xr_haptics COMMAND xr_haptics_test)

add_executable(default_migration111_test default_migration111_test.cpp)
target_include_directories(default_migration111_test PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../src)
add_test(NAME default_migration111 COMMAND default_migration111_test)

# Run the existing complete-production-TU fixtures on Windows as well as the
# Linux script. Native leaves remain explicit host substitutes in these tests.
if(MSVC)
 add_executable(contact_hook115_test contact_hook115_test.cpp)
 target_include_directories(contact_hook115_test PRIVATE syntax_shims openxr_sdk)
 target_compile_definitions(contact_hook115_test PRIVATE _ReturnAddress=HostReturnAddress)
 add_test(NAME contact_hook115 COMMAND contact_hook115_test)
 add_executable(touch_ownership114_test touch_ownership114_test.cpp)
 target_include_directories(touch_ownership114_test PRIVATE syntax_shims openxr_sdk)
 add_test(NAME touch_ownership114 COMMAND touch_ownership114_test)
 add_executable(retail_hash112_test retail_hash112_test.cpp)
 target_link_libraries(retail_hash112_test PRIVATE bcrypt)
 add_test(NAME retail_hash112 COMMAND retail_hash112_test)
 add_executable(animation_ownership112_bridge animation_ownership112_bridge.cpp)
 target_include_directories(animation_ownership112_bridge PRIVATE syntax_shims openxr_sdk)
 add_test(NAME animation_ownership112_bridge COMMAND animation_ownership112_bridge)
 foreach(name fist calibration physical native_contract native104 lifecycle bridges168 attack_timing178r22c2)
  add_executable(${name}_bridge ${name}_bridge_test.cpp bridge_external_stubs112.cpp ../src/subsystems/doors/module.cpp)
  if(name STREQUAL "fist" OR name STREQUAL "calibration")
   target_compile_definitions(${name}_bridge PRIVATE BRIDGE_HAS_MODE_STUBS)
  endif()
  if(name STREQUAL "bridges168")
   target_compile_definitions(${name}_bridge PRIVATE BRIDGE_HAS_MODE_STUBS BRIDGE_VERIFIED_ACTIVATION_H24)
  endif()
  if(name STREQUAL "fist")
   target_compile_definitions(${name}_bridge PRIVATE BRIDGE_HAS_TARGET_STUB)
  endif()
  target_include_directories(${name}_bridge PRIVATE syntax_shims openxr_sdk)
  target_compile_definitions(${name}_bridge PRIVATE _ReturnAddress=HostReturnAddress)
  target_compile_options(${name}_bridge PRIVATE /Gy /Gw /bigobj)
  target_link_options(${name}_bridge PRIVATE /OPT:REF /INCREMENTAL:NO)
  add_test(NAME ${name}_bridge COMMAND ${name}_bridge)
 endforeach()
 add_test(NAME gesture_native_h30 COMMAND native104_bridge --gesture-recovery-only-h30)
 add_test(NAME xr32_camera_native COMMAND native104_bridge --xr32-camera-only)
 target_compile_definitions(native104_bridge PRIVATE KCD2VR_R17_ROUTE_EXTRACT="${CMAKE_CURRENT_BINARY_DIR}/combo_admitted_direct_r17.inc")
 file(WRITE "${CMAKE_CURRENT_BINARY_DIR}/native104_seh.h"
  "#include \"${CMAKE_CURRENT_SOURCE_DIR}/syntax_shims/windows.h\"\n#undef __try\n#undef __except\n")
 target_compile_options(native104_bridge PRIVATE /FI"${CMAKE_CURRENT_BINARY_DIR}/native104_seh.h")
 add_test(NAME admitted_direct_lineage_r17 COMMAND native104_bridge --admitted-direct-lineage-only-r17)
endif()


foreach(name bow146 support146 admission147)
 add_executable(${name}_test ${name}_test.cpp)
 add_test(NAME ${name} COMMAND ${name}_test)
endforeach()

add_executable(holster_layout152_test holster_layout152_test.cpp)
add_test(NAME holster_layout152 COMMAND holster_layout152_test)

add_executable(bow160_test bow160_test.cpp)
add_test(NAME bow160 COMMAND bow160_test)

add_executable(contextual_hud162r1_test contextual_hud162r1_test.cpp)
add_test(NAME contextual_hud162r1 COMMAND contextual_hud162r1_test)
add_executable(minigame_pad162r1_test minigame_pad162r1_test.cpp ../src/native_vr_pad_premap.cpp)
target_include_directories(minigame_pad162r1_test PRIVATE openxr_sdk syntax_shims)
add_test(NAME minigame_pad162r1 COMMAND minigame_pad162r1_test)
add_executable(regress163_test regress163_test.cpp)
add_test(NAME regress163 COMMAND regress163_test)

add_executable(contact164_test contact164_test.cpp)
add_test(NAME contact164 COMMAND contact164_test)

add_executable(lifecycle165_test lifecycle165_test.cpp)
target_link_libraries(lifecycle165_test PRIVATE Threads::Threads)
add_test(NAME lifecycle165 COMMAND lifecycle165_test)

add_executable(hud_draw166_test hud_draw166_test.cpp)
add_test(NAME hud_draw166 COMMAND hud_draw166_test)
add_executable(f11_tests167_test f11_tests167_test.cpp)
add_test(NAME f11_tests167 COMMAND f11_tests167_test)
add_executable(shortcut_cycle167r1_test shortcut_cycle167r1_test.cpp)
add_test(NAME shortcut_cycle167r1 COMMAND shortcut_cycle167r1_test)

# V168 new portable policy test (production headers, no native execution).
add_executable(bridge_policy168_test bridge_policy168_test.cpp)
target_link_libraries(bridge_policy168_test PRIVATE Threads::Threads)
add_test(NAME bridge_policy168 COMMAND bridge_policy168_test)

if(NOT MSVC)
 add_executable(touch_ownership114_test touch_ownership114_test.cpp)
 target_include_directories(touch_ownership114_test PRIVATE syntax_shims openxr_sdk)
 add_test(NAME touch_ownership114 COMMAND touch_ownership114_test)
else()
 target_compile_options(touch_ownership114_test PRIVATE /Gy /Gw /bigobj)
 target_link_options(touch_ownership114_test PRIVATE /OPT:REF /INCREMENTAL:NO)
endif()

add_executable(capture_runtime168_test capture_runtime168_test.cpp)
target_include_directories(capture_runtime168_test PRIVATE syntax_shims ../src)
add_test(NAME capture_runtime168 COMMAND capture_runtime168_test)

# Real native-hit detour with synthetic identity memory and original-call spy.
# Return-address intrinsic substitution and absent service leaves are host-only.
if(NOT MSVC)
 add_executable(quiet_hit_feedback168_test quiet_hit_feedback168_test.cpp)
 target_include_directories(quiet_hit_feedback168_test PRIVATE syntax_shims ../src)
 target_compile_options(quiet_hit_feedback168_test PRIVATE -fms-extensions -ffunction-sections -fdata-sections)
 target_link_options(quiet_hit_feedback168_test PRIVATE -Wl,--gc-sections)
 target_link_libraries(quiet_hit_feedback168_test PRIVATE Threads::Threads)
 add_test(NAME quiet_hit_feedback168 COMMAND quiet_hit_feedback168_test)
else()
 add_executable(quiet_hit_feedback168_test quiet_hit_feedback168_msvc_test.cpp)
 target_include_directories(quiet_hit_feedback168_test PRIVATE syntax_shims ../src)
 target_compile_options(quiet_hit_feedback168_test PRIVATE /Gy /Gw /bigobj)
 target_link_options(quiet_hit_feedback168_test PRIVATE /OPT:REF /INCREMENTAL:NO)
 target_link_libraries(quiet_hit_feedback168_test PRIVATE Threads::Threads)
 add_test(NAME quiet_hit_feedback168 COMMAND quiet_hit_feedback168_test)
endif()

add_executable(capture_keys168r1_test capture_keys168r1_test.cpp)
target_compile_features(capture_keys168r1_test PRIVATE cxx_std_17)
add_test(NAME capture_keys168r1 COMMAND capture_keys168r1_test)

# Actual passive contact policy/transport; no game/Windows execution.
add_executable(passive169r1_test passive169r1_test.cpp)
target_link_libraries(passive169r1_test PRIVATE Threads::Threads)
add_test(NAME passive169r1 COMMAND passive169r1_test)

add_executable(experiments169_test experiments169_test.cpp)
add_test(NAME experiments169 COMMAND experiments169_test)

# Focused V169R2 policies plus the production coherent-pose fixture below.
foreach(name archer169r2 hilt169r2 sweep169r2)
 add_executable(${name}_test ${name}_test.cpp)
 target_compile_features(${name}_test PRIVATE cxx_std_17)
 target_link_libraries(${name}_test PRIVATE Threads::Threads)
 add_test(NAME ${name} COMMAND ${name}_test)
endforeach()

add_executable(npc_contact172_test npc_contact172_test.cpp)
add_test(NAME npc_contact172 COMMAND npc_contact172_test)
add_executable(contact174_test contact174_test.cpp)
add_test(NAME contact174 COMMAND contact174_test)
add_executable(combat_rules178r10_test combat_rules178r10_test.cpp)
target_include_directories(combat_rules178r10_test PRIVATE ../src)
add_test(NAME combat_rules178r10 COMMAND combat_rules178r10_test)
add_executable(calibration_restore43_test calibration_restore43_test.cpp)
target_include_directories(calibration_restore43_test PRIVATE ../src)
add_test(NAME calibration_restore43 COMMAND calibration_restore43_test)
add_executable(workstation178r20c1_test workstation178r20c1_test.cpp)
target_include_directories(workstation178r20c1_test PRIVATE ../src)
add_test(NAME workstation178r20c1 COMMAND workstation178r20c1_test)
add_executable(menu_native174_test menu_native174_test.cpp ../src/native_vr_pad_premap.cpp)
target_include_directories(menu_native174_test PRIVATE openxr_sdk syntax_shims)
add_test(NAME menu_native174 COMMAND menu_native174_test)
# R118: controller layout table, X hold = torch / inventory in the real pad premap, Experiments GEAR page 2, default INI writer
add_executable(controller_layout_r118_test controller_layout_r118_test.cpp ../src/native_vr_pad_premap.cpp)
target_include_directories(controller_layout_r118_test PRIVATE ../src openxr_sdk syntax_shims)
if(NOT MSVC)
  target_compile_definitions(controller_layout_r118_test PRIVATE __fastcall= __cdecl= __stdcall= __thiscall=)
endif()
add_test(NAME controller_layout_r118 COMMAND controller_layout_r118_test)
add_executable(menu_recovery174r2_test menu_recovery174r2_test.cpp)
target_include_directories(menu_recovery174r2_test PRIVATE openxr_sdk syntax_shims)
add_test(NAME menu_recovery174r2 COMMAND menu_recovery174r2_test)

execute_process(COMMAND "${Python3_EXECUTABLE}" "${CMAKE_CURRENT_SOURCE_DIR}/../tools/extract_user_controls175.py" RESULT_VARIABLE USER_CONTROLS_EXTRACT175)
if(NOT USER_CONTROLS_EXTRACT175 EQUAL 0)
 message(FATAL_ERROR "Could not extract current production user control clauses")
endif()
execute_process(COMMAND "${Python3_EXECUTABLE}" "${CMAKE_CURRENT_SOURCE_DIR}/../tools/extract_fist_transport175.py" RESULT_VARIABLE FIST_TRANSPORT_EXTRACT175)
if(NOT FIST_TRANSPORT_EXTRACT175 EQUAL 0)
 message(FATAL_ERROR "Could not extract current production fist transport and caller")
endif()
add_executable(user_controls175_test user_controls175_test.cpp)
add_test(NAME user_controls175 COMMAND user_controls175_test)
add_executable(shift_f12_h29_test shift_f12_h29_test.cpp)
add_test(NAME shift_f12_h29 COMMAND shift_f12_h29_test)

# V175 current user contract. Production adapters and whole-TU fixtures use
# native leaf substitutes; these tests do not execute the retail game.
foreach(name physical_contract175 solver_command_h28 trigger_hold_h26 support_contract175 recorder_contract175 fist_guard175 hud_repair175)
 add_executable(${name}_test ${name}_test.cpp)
 target_link_libraries(${name}_test PRIVATE Threads::Threads)
 add_test(NAME ${name} COMMAND ${name}_test)
endforeach()
if(MSVC)
 target_link_options(physical_contract175_test PRIVATE /STACK:8388608)
endif()
foreach(name input_repair175 hud_writer175_bridge activity_native175_bridge fist_transport175)
 add_executable(${name}_test ${name}_test.cpp)
 target_include_directories(${name}_test PRIVATE ui175/mocks syntax_shims openxr_sdk)
 target_link_libraries(${name}_test PRIVATE Threads::Threads)
 if(MSVC)
  target_compile_options(${name}_test PRIVATE /Gy /Gw /bigobj)
  target_link_options(${name}_test PRIVATE /OPT:REF /INCREMENTAL:NO)
 else()
  target_compile_options(${name}_test PRIVATE -fms-extensions -ffunction-sections -fdata-sections)
  target_link_options(${name}_test PRIVATE -Wl,--gc-sections)
 endif()
 add_test(NAME ${name} COMMAND ${name}_test)
endforeach()
target_compile_features(fist_transport175_test PRIVATE cxx_std_20)
foreach(name bow_native_contract175 diagnostic_modes175)
add_executable(${name}_bridge ${name}_bridge_test.cpp bridge_external_stubs112.cpp ../src/subsystems/doors/module.cpp)
target_include_directories(${name}_bridge PRIVATE syntax_shims openxr_sdk)
target_compile_definitions(${name}_bridge PRIVATE _ReturnAddress=HostReturnAddress)
target_link_libraries(${name}_bridge PRIVATE Threads::Threads)
if(MSVC)
 target_compile_options(${name}_bridge PRIVATE /Gy /Gw /bigobj)
 target_link_options(${name}_bridge PRIVATE /OPT:REF /INCREMENTAL:NO)
else()
 target_compile_options(${name}_bridge PRIVATE -fms-extensions -ffunction-sections -fdata-sections)
 target_link_options(${name}_bridge PRIVATE -Wl,--gc-sections)
endif()
add_test(NAME ${name}_bridge COMMAND ${name}_bridge)
endforeach()

# V176 F4 additions. Native execution remains an explicit fixture leaf.
execute_process(COMMAND "${Python3_EXECUTABLE}" "${CMAKE_CURRENT_SOURCE_DIR}/../tools/extract_r3_route176.py" RESULT_VARIABLE R3_EXTRACT176)
if(NOT R3_EXTRACT176 EQUAL 0)
 message(FATAL_ERROR "Could not extract current production R3 dispatcher")
endif()
foreach(name guard_parry176 kick176 r3_route176)
 add_executable(${name}_test ${name}_test.cpp)
 target_link_libraries(${name}_test PRIVATE Threads::Threads)
 add_test(NAME ${name} COMMAND ${name}_test)
endforeach()
foreach(name contact_authority176 kick176_ingress)
 add_executable(${name}_bridge ${name}_bridge_test.cpp bridge_external_stubs112.cpp ../src/subsystems/doors/module.cpp)
 target_include_directories(${name}_bridge PRIVATE syntax_shims openxr_sdk)
 target_compile_definitions(${name}_bridge PRIVATE _ReturnAddress=HostReturnAddress)
 target_link_libraries(${name}_bridge PRIVATE Threads::Threads)
 if(MSVC)
  target_compile_options(${name}_bridge PRIVATE /Gy /Gw /bigobj)
  target_link_options(${name}_bridge PRIVATE /OPT:REF /INCREMENTAL:NO)
 else()
  target_compile_options(${name}_bridge PRIVATE -fms-extensions -ffunction-sections -fdata-sections)
  target_link_options(${name}_bridge PRIVATE -Wl,--gc-sections)
 endif()
 add_test(NAME ${name}_bridge COMMAND ${name}_bridge)
endforeach()
# R55C2: this actual incoming-Submit observer now supports the MSVC caller shim.
add_executable(parry_outcome176_test parry_outcome176_test.cpp)
target_include_directories(parry_outcome176_test PRIVATE syntax_shims openxr_sdk)
target_compile_definitions(parry_outcome176_test PRIVATE _ReturnAddress=HostReturnAddress)
target_link_libraries(parry_outcome176_test PRIVATE Threads::Threads)
if(MSVC)
 target_compile_options(parry_outcome176_test PRIVATE /Gy /Gw /bigobj)
 target_link_options(parry_outcome176_test PRIVATE /OPT:REF /INCREMENTAL:NO)
else()
 target_compile_options(parry_outcome176_test PRIVATE -fms-extensions -ffunction-sections -fdata-sections)
 target_link_options(parry_outcome176_test PRIVATE -Wl,--gc-sections)
endif()
add_test(NAME parry_outcome176 COMMAND parry_outcome176_test)
# These fixtures replace the GNU caller-address intrinsic or include the
# existing portable pose fixture; they are deliberately not an MSVC claim.
if(NOT MSVC)
 foreach(name contact_gate176 f4_contact_install176 kick176_bridge)
  add_executable(${name}_test ${name}_test.cpp)
  target_include_directories(${name}_test PRIVATE syntax_shims openxr_sdk)
  target_compile_definitions(${name}_test PRIVATE _ReturnAddress=HostReturnAddress)
  target_compile_options(${name}_test PRIVATE -fms-extensions -ffunction-sections -fdata-sections)
  target_link_options(${name}_test PRIVATE -Wl,--gc-sections)
  target_link_libraries(${name}_test PRIVATE Threads::Threads)
  add_test(NAME ${name} COMMAND ${name}_test)
 endforeach()
endif()

# H14: exercise the actual native combo formatter and recorder domain on Windows
# too. Unused native services are explicit fail-closed fixture leaves.
add_executable(combo_timing176_test combo_timing176_test.cpp)
target_include_directories(combo_timing176_test PRIVATE syntax_shims openxr_sdk)
target_link_libraries(combo_timing176_test PRIVATE Threads::Threads)
if(MSVC)
 target_compile_options(combo_timing176_test PRIVATE /Gy /Gw /bigobj)
 target_link_options(combo_timing176_test PRIVATE /OPT:REF)
else()
 target_compile_definitions(combo_timing176_test PRIVATE _ReturnAddress=HostReturnAddress)
 target_compile_options(combo_timing176_test PRIVATE -fms-extensions -ffunction-sections -fdata-sections)
 target_link_options(combo_timing176_test PRIVATE -Wl,--gc-sections)
endif()
add_test(NAME combo_timing176 COMMAND combo_timing176_test)

add_executable(reload_lifecycle178r36c2_test reload_lifecycle178r36c2_test.cpp)
add_test(NAME reload_lifecycle178r36c2 COMMAND reload_lifecycle178r36c2_test)

add_executable(bow_grip178r36c1_test bow_grip178r36c1_test.cpp)
add_test(NAME bow_grip178r36c1 COMMAND bow_grip178r36c1_test)

add_executable(hud_experiment36c5_test hud_experiment36c5_test.cpp)
add_test(NAME hud_experiment36c5 COMMAND hud_experiment36c5_test)

if(MSVC)
 add_executable(hud_mirror_shader36_test hud_mirror_shader36_test.cpp)
 target_link_libraries(hud_mirror_shader36_test PRIVATE d3dcompiler d3d12)
 add_test(NAME hud_mirror_shader36 COMMAND hud_mirror_shader36_test)
endif()


add_executable(bow_coupling39c1_test bow_coupling39c1_test.cpp)
add_test(NAME bow_coupling39c1 COMMAND bow_coupling39c1_test)

add_executable(geometry_scope40c1_test geometry_scope40c1_test.cpp)
target_link_libraries(geometry_scope40c1_test PRIVATE Threads::Threads)
add_test(NAME geometry_scope40c1 COMMAND geometry_scope40c1_test)

add_executable(readable_hud44c2_test readable_hud44c2_test.cpp)
target_include_directories(readable_hud44c2_test PRIVATE ../src)
add_test(NAME readable_hud44c2 COMMAND readable_hud44c2_test)
add_executable(dialogue_pose47c1_test dialogue_pose47c1_test.cpp)
target_include_directories(dialogue_pose47c1_test PRIVATE ../src)
add_test(NAME dialogue_pose47c1 COMMAND dialogue_pose47c1_test)

add_executable(bow_grip47c2_test bow_grip47c2_test.cpp)
add_test(NAME bow_grip47c2 COMMAND bow_grip47c2_test)
add_executable(contact_lease47c2_test contact_lease47c2_test.cpp)
add_test(NAME contact_lease47c2 COMMAND contact_lease47c2_test)
add_executable(contact_receiver47c2_test contact_receiver47c2_test.cpp)
add_test(NAME contact_receiver47c2 COMMAND contact_receiver47c2_test)

add_executable(hud_guard49c1_test hud_guard49c1_test.cpp)
add_test(NAME hud_guard49c1 COMMAND hud_guard49c1_test)
add_executable(native_hit_receipt50c1_test native_hit_receipt50c1_test.cpp)
add_test(NAME native_hit_receipt50c1 COMMAND native_hit_receipt50c1_test)

add_executable(attack_clock50c2_test attack_clock50c2_test.cpp)
add_test(NAME attack_clock50c2 COMMAND attack_clock50c2_test)

add_executable(native_sweep_denial51c3_test native_sweep_denial51c3_test.cpp)
add_test(NAME native_sweep_denial51c3 COMMAND native_sweep_denial51c3_test)
add_executable(roomscale58_test roomscale58_test.cpp)
add_test(NAME roomscale58 COMMAND roomscale58_test)
add_executable(gesture_talk59c5_test gesture_talk59c5_test.cpp) # R59.16C5
add_test(NAME gesture_talk59c5 COMMAND gesture_talk59c5_test)
add_executable(ragdoll_lease59_test ragdoll_lease59_test.cpp)
add_test(NAME ragdoll_lease59 COMMAND ragdoll_lease59_test)
add_executable(fault_pause59_test fault_pause59_test.cpp)
add_test(NAME fault_pause59 COMMAND fault_pause59_test)

add_executable(recovery55c1_test recovery55c1_test.cpp)
target_include_directories(recovery55c1_test PRIVATE ../src)
add_test(NAME recovery55c1 COMMAND recovery55c1_test)

add_executable(interaction_cone57c1_test interaction_cone57c1_test.cpp)
target_include_directories(interaction_cone57c1_test PRIVATE ../src)
add_test(NAME interaction_cone57c1 COMMAND interaction_cone57c1_test)

add_executable(adaptive_hud57c1_test adaptive_hud57c1_test.cpp)
target_include_directories(adaptive_hud57c1_test PRIVATE ../src)
add_test(NAME adaptive_hud57c1 COMMAND adaptive_hud57c1_test)

add_executable(hud_r112_test hud_r112_test.cpp) # R112 combat crosshair size + quick-slot fade
target_include_directories(hud_r112_test PRIVATE ../src)
add_test(NAME hud_r112 COMMAND hud_r112_test)

add_executable(swing_whoosh_r116_test swing_whoosh_r116_test.cpp) # R116 weapon whoosh at the physical release
target_compile_features(swing_whoosh_r116_test PRIVATE cxx_std_20)
target_include_directories(swing_whoosh_r116_test PRIVATE ../src)
add_test(NAME swing_whoosh_r116 COMMAND swing_whoosh_r116_test)
add_executable(prop_impact_sound_r124_test prop_impact_sound_r124_test.cpp) # R124 impact sound of our loose props
target_compile_features(prop_impact_sound_r124_test PRIVATE cxx_std_20)
target_include_directories(prop_impact_sound_r124_test PRIVATE ../src)
target_compile_definitions(prop_impact_sound_r124_test PRIVATE KCD2VR_SOURCE_ROOT="${CMAKE_CURRENT_SOURCE_DIR}/..")
add_test(NAME prop_impact_sound_r124 COMMAND prop_impact_sound_r124_test)

add_executable(focus_zoom58c2_test focus_zoom58c2_test.cpp)
add_test(NAME focus_zoom58c2 COMMAND focus_zoom58c2_test)

add_executable(pause_chord58c2_test pause_chord58c2_test.cpp)
add_test(NAME pause_chord58c2 COMMAND pause_chord58c2_test)

add_executable(overlay59c1_test overlay59c1_test.cpp)
add_test(NAME overlay59c1 COMMAND overlay59c1_test)

execute_process(COMMAND "${Python3_EXECUTABLE}" "${CMAKE_CURRENT_SOURCE_DIR}/../tools/extract_held_map_m1.py" RESULT_VARIABLE MAP_M1_EXTRACT)
if(NOT MAP_M1_EXTRACT EQUAL 0)
 message(FATAL_ERROR "Could not extract complete held-map left pose getter")
endif()
add_executable(held_map_m1_test held_map_m1_test.cpp)
add_test(NAME held_map_m1 COMMAND held_map_m1_test)
add_executable(native_map_open_h25_test native_map_open_h25_test.cpp)
add_test(NAME native_map_open_h25 COMMAND native_map_open_h25_test)
add_executable(native_map_live_h28_test native_map_live_h28_test.cpp)
add_test(NAME native_map_live_h28 COMMAND native_map_live_h28_test)
execute_process(COMMAND "${Python3_EXECUTABLE}" "${CMAKE_CURRENT_SOURCE_DIR}/../tools/extract_native_map_live_encoder_h28.py" "${CMAKE_CURRENT_BINARY_DIR}/h28_native_encode.inc" "${CMAKE_CURRENT_BINARY_DIR}/h28_native_combined_service.inc" RESULT_VARIABLE MAP_H28_EXTRACT)
if(NOT MAP_H28_EXTRACT EQUAL 0)
 message(FATAL_ERROR "Could not extract current native map service/encoder")
endif()
add_executable(native_map_live_service_h28_test native_map_live_service_h28_test.cpp)
target_include_directories(native_map_live_service_h28_test PRIVATE "${CMAKE_CURRENT_BINARY_DIR}")
add_test(NAME native_map_live_service_h28 COMMAND native_map_live_service_h28_test)
if(WIN32)
 add_executable(held_map_touch_h24_test held_map_touch_h24_test.cpp)
 target_include_directories(held_map_touch_h24_test PRIVATE openxr_sdk)
 if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/../deps/DetourModKit/include")
  target_include_directories(held_map_touch_h24_test PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/../deps/DetourModKit/include")
  set_property(TARGET held_map_touch_h24_test PROPERTY CXX_STANDARD 23)
 endif()
 target_link_libraries(held_map_touch_h24_test PRIVATE user32)
 add_test(NAME held_map_touch_h24 COMMAND held_map_touch_h24_test)
endif()
add_executable(held_map_getter_m1_test held_map_getter_m1_test.cpp)
add_test(NAME held_map_getter_m1 COMMAND held_map_getter_m1_test)

# ASan is a host-test option only. Make its runtime available even when CTest
# is launched outside a Visual Studio developer prompt (no global PATH edits).
add_executable(menu_transition59c2_test menu_transition59c2_test.cpp)
add_test(NAME menu_transition59c2 COMMAND menu_transition59c2_test)

add_executable(contact_handoff59c3_test contact_handoff59c3_test.cpp)
add_test(NAME contact_handoff59c3 COMMAND contact_handoff59c3_test)



add_executable(motion_owned59c4_test motion_owned59c4_test.cpp)
target_link_libraries(motion_owned59c4_test PRIVATE Threads::Threads)
add_test(NAME motion_owned59c4 COMMAND motion_owned59c4_test)

add_executable(motion_contact59c5_test motion_contact59c5_test.cpp)
add_test(NAME motion_contact59c5 COMMAND motion_contact59c5_test)
add_executable(combat_services59c5_test combat_services59c5_test.cpp)
target_link_libraries(combat_services59c5_test PRIVATE Threads::Threads)
add_test(NAME combat_services59c5 COMMAND combat_services59c5_test)



add_executable(game_hit_event5910c1_test game_hit_event5910c1_test.cpp)
add_test(NAME game_hit_event5910c1 COMMAND game_hit_event5910c1_test)

# These fixtures exercise generated production bodies, never an archived donor copy.
foreach(adapter state5912 prop5916c1 effort5916c1)
 execute_process(COMMAND "${Python3_EXECUTABLE}" "${CMAKE_CURRENT_SOURCE_DIR}/../tools/extract_${adapter}.py" RESULT_VARIABLE ADAPTER_EXTRACT)
 if(NOT ADAPTER_EXTRACT EQUAL 0)
  message(FATAL_ERROR "Could not extract current ${adapter} production code")
 endif()
endforeach()
set_property(DIRECTORY APPEND PROPERTY CMAKE_CONFIGURE_DEPENDS
 ../src/diagnostics/geometry_hit178r27.inl ../src/vrik/physical_native.inl
 ../tools/extract_effort5916c1.py)
add_executable(state_context5912_test state_context5912_test.cpp)
target_include_directories(state_context5912_test PRIVATE ../src)
add_test(NAME state_context5912_test COMMAND state_context5912_test)

add_executable(live_effort5916c1_test live_effort5916c1_test.cpp)
target_include_directories(live_effort5916c1_test PRIVATE ../src)
add_test(NAME live_effort5916c1_test COMMAND live_effort5916c1_test)

add_executable(prop_drive5916c1_test prop_drive5916c1_test.cpp)
add_test(NAME prop_drive5916c1 COMMAND prop_drive5916c1_test)
add_executable(prop_adapter5916c1_test prop_adapter5916c1_test.cpp)
add_test(NAME prop_adapter5916c1 COMMAND prop_adapter5916c1_test)

add_executable(effort_transaction5916c1_test effort_transaction5916c1_test.cpp)
add_test(NAME effort_transaction5916c1 COMMAND effort_transaction5916c1_test)

execute_process(COMMAND "${Python3_EXECUTABLE}" "${CMAKE_CURRENT_SOURCE_DIR}/../tools/extract_repair5916c3.py" RESULT_VARIABLE REPAIR_EXTRACT)
if(NOT REPAIR_EXTRACT EQUAL 0)
 message(FATAL_ERROR "C3 production extraction failed")
endif()
add_executable(repair5916c3_test repair5916c3_test.cpp)
add_test(NAME repair5916c3 COMMAND repair5916c3_test)
add_executable(interactor5916c3_test interactor5916c3_test.cpp)
target_include_directories(interactor5916c3_test PRIVATE syntax_shims ../src)
if(NOT MSVC)
 target_compile_options(interactor5916c3_test PRIVATE -fms-extensions -D__fastcall= -D_ReturnAddress=HostReturnAddress)
else()
 target_compile_definitions(interactor5916c3_test PRIVATE _ReturnAddress=HostReturnAddress) # R59.16C7A (Codex review): MSVC test-only mapping
endif()
add_test(NAME interactor5916c3 COMMAND interactor5916c3_test)

execute_process(COMMAND "${Python3_EXECUTABLE}" "${CMAKE_CURRENT_SOURCE_DIR}/../tools/extract_reload5916c3.py" RESULT_VARIABLE RELOAD_C3_EXTRACT)
if(NOT RELOAD_C3_EXTRACT EQUAL 0)
 message(FATAL_ERROR "Reload policy extraction failed")
endif()
add_executable(reload5916c3_test reload5916c3_test.cpp)
add_test(NAME reload5916c3 COMMAND reload5916c3_test)
add_executable(victim_watch5916c3_test victim_watch5916c3_test.cpp)
add_test(NAME victim_watch5916c3 COMMAND victim_watch5916c3_test)
execute_process(COMMAND "${Python3_EXECUTABLE}" "${CMAKE_CURRENT_SOURCE_DIR}/../tools/extract_impulse5916c3.py" RESULT_VARIABLE IMPULSE_C3_EXTRACT)
if(NOT IMPULSE_C3_EXTRACT EQUAL 0)
 message(FATAL_ERROR "Impulse policy extraction failed")
endif()
add_executable(impulse5916c3_test impulse5916c3_test.cpp)
add_test(NAME impulse5916c3 COMMAND impulse5916c3_test)

# R59.16C6: actual current-time XR sampler plus validity/session policy.
execute_process(COMMAND "${Python3_EXECUTABLE}" "${CMAKE_CURRENT_SOURCE_DIR}/../tools/extract_tracking59c6.py" RESULT_VARIABLE TRACKING_EXTRACT)
if(NOT TRACKING_EXTRACT EQUAL 0)
 message(FATAL_ERROR "tracking59c6 extraction failed")
endif()
add_executable(tracking59c6_test tracking59c6_test.cpp)
target_include_directories(tracking59c6_test PRIVATE syntax_shims)
add_test(NAME tracking59c6 COMMAND tracking59c6_test)
add_executable(xr_pose_family59c6_test xr_pose_family59c6_test.cpp)
target_include_directories(xr_pose_family59c6_test PRIVATE syntax_shims openxr_sdk)
add_test(NAME xr_pose_family59c6 COMMAND xr_pose_family59c6_test)

execute_process(COMMAND "${Python3_EXECUTABLE}" "${CMAKE_CURRENT_SOURCE_DIR}/../tools/extract_contact_queue59c6.py" RESULT_VARIABLE QUEUE59C6_EXTRACT)
if(NOT QUEUE59C6_EXTRACT EQUAL 0)
 message(FATAL_ERROR "Could not extract contact inbox production code")
endif()
add_executable(contact_queue59c6_test contact_queue59c6_test.cpp)
target_link_libraries(contact_queue59c6_test PRIVATE Threads::Threads)
add_test(NAME contact_queue59c6 COMMAND contact_queue59c6_test)
add_executable(articulated_target_owner_h8_test articulated_target_owner_h8_test.cpp)
target_link_libraries(articulated_target_owner_h8_test PRIVATE Threads::Threads)
add_test(NAME articulated_target_owner_h8 COMMAND articulated_target_owner_h8_test)
add_executable(feature_probe59c6_test feature_probe59c6_test.cpp)
add_test(NAME feature_probe59c6 COMMAND feature_probe59c6_test)

add_executable(feature_ray59c6_test feature_ray59c6_test.cpp)
add_test(NAME feature_ray59c6 COMMAND feature_ray59c6_test)

execute_process(COMMAND "${Python3_EXECUTABLE}" "${CMAKE_CURRENT_SOURCE_DIR}/../tools/extract_render_pacing59c6.py" RESULT_VARIABLE PACING59C6_EXTRACT)
if(NOT PACING59C6_EXTRACT EQUAL 0)
 message(FATAL_ERROR "Could not extract actual render handoff")
endif()
add_executable(render_pacing59c6_test render_pacing59c6_test.cpp)
target_link_libraries(render_pacing59c6_test PRIVATE Threads::Threads)
add_test(NAME render_pacing59c6 COMMAND render_pacing59c6_test)

# C7 complete production HUD functions; explicit native/XR/GPU leaves in fixtures.
execute_process(COMMAND "${Python3_EXECUTABLE}" "${CMAKE_CURRENT_SOURCE_DIR}/../tools/extract_hud59c7.py" RESULT_VARIABLE HUD59C7_EXTRACT)
if(NOT HUD59C7_EXTRACT EQUAL 0)
 message(FATAL_ERROR "Could not extract current HUD production functions")
endif()
foreach(hud_case copy trace policy elements)
 add_executable(hud_${hud_case}59c7_test hud_${hud_case}59c7_test.cpp)
 target_link_libraries(hud_${hud_case}59c7_test PRIVATE Threads::Threads)
 add_test(NAME hud_${hud_case}59c7 COMMAND hud_${hud_case}59c7_test)
endforeach()

add_executable(exhaustion59c7b_test exhaustion59c7b_test.cpp)
add_test(NAME exhaustion59c7b COMMAND exhaustion59c7b_test)

add_executable(hands59c7f_test hands59c7f_test.cpp)
add_test(NAME hands59c7f COMMAND hands59c7f_test)

execute_process(COMMAND "${Python3_EXECUTABLE}" "${CMAKE_CURRENT_SOURCE_DIR}/../tools/extract_reload_layout59c9.py" RESULT_VARIABLE RELOAD_LAYOUT_EXTRACT)
if(NOT RELOAD_LAYOUT_EXTRACT EQUAL 0)
 message(FATAL_ERROR "Reload layout extraction failed")
endif()
add_executable(reload_layout59c9_test reload_layout59c9_test.cpp)
add_test(NAME reload_layout59c9 COMMAND reload_layout59c9_test)
add_executable(live_attributes59c9_test live_attributes59c9_test.cpp)
add_test(NAME live_attributes59c9 COMMAND live_attributes59c9_test)

add_executable(palm_selection59c9_test palm_selection59c9_test.cpp)
add_test(NAME palm_selection59c9 COMMAND palm_selection59c9_test)
add_executable(contact_location59c10_test contact_location59c10_test.cpp)
add_test(NAME contact_location59c10 COMMAND contact_location59c10_test)
add_executable(tool_attachment59c10_test tool_attachment59c10_test.cpp)
add_test(NAME tool_attachment59c10 COMMAND tool_attachment59c10_test)
add_executable(smith_tool36_test smith_tool36_test.cpp) # C37 smithing tool reader + auto-hold
add_test(NAME smith_tool36 COMMAND smith_tool36_test)
add_executable(activity_lens59c10r1_test activity_lens59c10r1_test.cpp)
add_test(NAME activity_lens59c10r1 COMMAND activity_lens59c10r1_test)
add_executable(dialogue_state59c10r2_test dialogue_state59c10r2_test.cpp)
add_test(NAME dialogue_state59c10r2 COMMAND dialogue_state59c10r2_test)
add_executable(bow_geometry59c10r2_test bow_geometry59c10r2_test.cpp)
add_test(NAME bow_geometry59c10r2 COMMAND bow_geometry59c10r2_test)

add_executable(interaction_receipt59c10_test interaction_receipt59c10_test.cpp)
add_test(NAME interaction_receipt59c10 COMMAND interaction_receipt59c10_test)
add_executable(strike_power59c10r6_test strike_power59c10r6_test.cpp) # R59.16C10R6
add_test(NAME strike_power59c10r6 COMMAND strike_power59c10r6_test)

# C10R8: regression against the real extracted native-state controller.
add_executable(followup_edge19_test followup_edge19_test.cpp)
target_include_directories(followup_edge19_test PRIVATE ../src)
target_link_libraries(followup_edge19_test PRIVATE Threads::Threads)
add_test(NAME followup_edge19_test COMMAND followup_edge19_test)
add_executable(state_eta59r8_test state_eta59r8_test.cpp)
target_include_directories(state_eta59r8_test PRIVATE ../src)
add_test(NAME state_eta59r8_test COMMAND state_eta59r8_test)

add_executable(phys_bodies59c10r8_test phys_bodies59c10r8_test.cpp) # R59.16C10R8 (Claude)
add_test(NAME phys_bodies59c10r8 COMMAND phys_bodies59c10r8_test)

add_executable(two_hand_axis59c16_test two_hand_axis59c16_test.cpp) # R59.16C10R16 (Claude): two-handed axis grip
add_test(NAME two_hand_axis59c16 COMMAND two_hand_axis59c16_test)
add_executable(intent59c16_test intent59c16_test.cpp) # R59.16C10R16 (Claude): F4 press 1 VR INTENT producer
add_test(NAME intent59c16 COMMAND intent59c16_test)
add_executable(sync_mode59c10r9_test sync_mode59c10r9_test.cpp) # R59.16C10R9 (Claude)
target_include_directories(sync_mode59c10r9_test PRIVATE ../src)
add_test(NAME sync_mode59c10r9 COMMAND sync_mode59c10r9_test)

# C10R13 native grasp transaction: exact production owner with explicit native leaves.
add_executable(native_grasp13_test native_grasp13_test.cpp)
add_test(NAME native_grasp13 COMMAND native_grasp13_test)
add_executable(native_grasp_owner13_test native_grasp_owner13_test.cpp)
target_include_directories(native_grasp_owner13_test PRIVATE syntax_shims ../src)
target_link_libraries(native_grasp_owner13_test PRIVATE Threads::Threads)
if(MSVC)
 target_compile_options(native_grasp_owner13_test PRIVATE /Gy /Gw /bigobj)
 target_link_options(native_grasp_owner13_test PRIVATE /OPT:REF)
else()
 target_compile_options(native_grasp_owner13_test PRIVATE -fms-extensions -ffunction-sections -fdata-sections)
 target_link_options(native_grasp_owner13_test PRIVATE -Wl,--gc-sections)
endif()
add_test(NAME native_grasp_owner13 COMMAND native_grasp_owner13_test)

# D40R1: actual Grip producer through native Picking/Attach target qualification.
add_executable(pickup_target_d40r1_test pickup_target_d40r1_test.cpp pickup_target_d40r1_stubs.cpp)
target_include_directories(pickup_target_d40r1_test PRIVATE syntax_shims ../src)
target_link_libraries(pickup_target_d40r1_test PRIVATE Threads::Threads)
target_compile_features(pickup_target_d40r1_test PRIVATE cxx_std_20)
if(MSVC)
 target_compile_options(pickup_target_d40r1_test PRIVATE /Gy /Gw /bigobj)
 target_link_options(pickup_target_d40r1_test PRIVATE /OPT:REF /STACK:8388608)
else()
 target_compile_options(pickup_target_d40r1_test PRIVATE -fms-extensions -ffunction-sections -fdata-sections)
 target_link_options(pickup_target_d40r1_test PRIVATE -Wl,--gc-sections)
endif()
add_test(NAME pickup_target_d40r1 COMMAND pickup_target_d40r1_test)
set_tests_properties(pickup_target_d40r1 PROPERTIES TIMEOUT 30)

add_executable(dialogue_membership19c2_test dialogue_membership19c2_test.cpp)
target_include_directories(dialogue_membership19c2_test PRIVATE ../src)
add_test(NAME dialogue_membership19c2 COMMAND dialogue_membership19c2_test)

execute_process(COMMAND "${Python3_EXECUTABLE}" "${CMAKE_CURRENT_SOURCE_DIR}/../tools/extract_dialogue_camera19c2.py" RESULT_VARIABLE DIALOGUE_CAMERA19C2_EXTRACT)
if(NOT DIALOGUE_CAMERA19C2_EXTRACT EQUAL 0)
 message(FATAL_ERROR "Dialogue camera callback extraction failed")
endif()
set_property(DIRECTORY APPEND PROPERTY CMAKE_CONFIGURE_DEPENDS
 ../src/camera_hook.cpp ../tools/extract_dialogue_camera19c2.py)
add_executable(dialogue_camera19c2_test dialogue_camera19c2_test.cpp)
target_include_directories(dialogue_camera19c2_test PRIVATE ../src)
add_test(NAME dialogue_camera19c2 COMMAND dialogue_camera19c2_test)
# C38 dialogue anchor low-pass + late-sample hold (pure)
add_executable(dialogue_smooth38_test dialogue_smooth38_test.cpp)
target_include_directories(dialogue_smooth38_test PRIVATE ../src)
add_test(NAME dialogue_smooth38 COMMAND dialogue_smooth38_test)

# Current production native selector body; mock only native reads/original call.
execute_process(COMMAND "${Python3_EXECUTABLE}" "${CMAKE_CURRENT_SOURCE_DIR}/../tools/extract_dialogue_selector19c2.py"
  "${CMAKE_CURRENT_BINARY_DIR}/dialogue_selector_adapter19c2.inc" RESULT_VARIABLE DIALOGUE_SELECTOR19C2_EXTRACT)
if(NOT DIALOGUE_SELECTOR19C2_EXTRACT EQUAL 0)
 message(FATAL_ERROR "Could not extract current dialogue camera selector")
endif()
set_property(DIRECTORY APPEND PROPERTY CMAKE_CONFIGURE_DEPENDS
 ../src/activity_lens59c10r1.cpp ../tools/extract_dialogue_selector19c2.py)
add_executable(dialogue_selector19c2_test dialogue_selector19c2_test.cpp)
target_include_directories(dialogue_selector19c2_test PRIVATE "${CMAKE_CURRENT_BINARY_DIR}")
add_test(NAME dialogue_selector19c2 COMMAND dialogue_selector19c2_test)

add_executable(dialogue_input_owner19c3_test dialogue_input_owner19c3_test.cpp)
target_include_directories(dialogue_input_owner19c3_test PRIVATE ../src)
add_test(NAME dialogue_input_owner19c3 COMMAND dialogue_input_owner19c3_test)

# Actual F11 automatic-reload admission/owner producer (native leaves substituted).
execute_process(COMMAND "${Python3_EXECUTABLE}" "${CMAKE_CURRENT_SOURCE_DIR}/../tools/extract_reload_auto19c3.py"
  RESULT_VARIABLE RELOAD_AUTO19C3_EXTRACT)
if(NOT RELOAD_AUTO19C3_EXTRACT EQUAL 0)
 message(FATAL_ERROR "Could not extract automatic reload admission producer")
endif()
add_executable(reload_auto19c3_test reload_auto19c3_test.cpp)
add_test(NAME reload_auto19c3 COMMAND reload_auto19c3_test)
add_executable(native_door19c4_test native_door19c4_test.cpp)
add_test(NAME native_door19c4 COMMAND native_door19c4_test)
add_executable(alchemy_input19c4_test alchemy_input19c4_test.cpp)
target_compile_features(alchemy_input19c4_test PRIVATE cxx_std_20)
target_link_libraries(alchemy_input19c4_test PRIVATE Threads::Threads)
add_test(NAME alchemy_input19c4 COMMAND alchemy_input19c4_test)
if(MSVC)
 target_compile_definitions(activity_native175_bridge_test PRIVATE _ReturnAddress=HostReturnAddress)
endif()
set_property(DIRECTORY APPEND PROPERTY CMAKE_CONFIGURE_DEPENDS
 "${CMAKE_CURRENT_SOURCE_DIR}/../src/diagnostics/geometry_hit178r27.inl"
 "${CMAKE_CURRENT_SOURCE_DIR}/../src/diagnostics/native_defense_offer19c4.inl"
 "${CMAKE_CURRENT_SOURCE_DIR}/../src/diagnostics/native_defense_intent19c4.h")
execute_process(COMMAND "${Python3_EXECUTABLE}" "${CMAKE_CURRENT_SOURCE_DIR}/../tools/extract_defense_intent19c4.py"
 RESULT_VARIABLE DEFENSE_INTENT_EXTRACT19C4)
if(NOT DEFENSE_INTENT_EXTRACT19C4 EQUAL 0)
 message(FATAL_ERROR "Could not extract current native defense intent transaction")
endif()
add_executable(defense_intent19c4_test defense_intent19c4_test.cpp)
add_test(NAME defense_intent19c4 COMMAND defense_intent19c4_test)
add_executable(native_door_io19c4_test native_door_io19c4_test.cpp)
add_test(NAME native_door_io19c4 COMMAND native_door_io19c4_test)
add_executable(door_candidates38_test door_candidates38_test.cpp)
add_test(NAME door_candidates38 COMMAND door_candidates38_test)
execute_process(COMMAND "${Python3_EXECUTABLE}" "${CMAKE_CURRENT_SOURCE_DIR}/../tools/extract_door_request38.py"
 RESULT_VARIABLE DOOR_REQUEST_EXTRACT38)
if(NOT DOOR_REQUEST_EXTRACT38 EQUAL 0)
 message(FATAL_ERROR "Could not extract current immutable native door request witness")
endif()
add_executable(door_request38_test door_request38_test.cpp)
add_test(NAME door_request38 COMMAND door_request38_test)
add_executable(door_palm38_test door_palm38_test.cpp)
add_test(NAME door_palm38 COMMAND door_palm38_test)

# C4R4 actual callback/owner bodies, with only native leaves substituted.
foreach(r4 native_door_script19c4r4 door_channel19c4r4)
 add_executable(${r4}_test ${r4}_test.cpp)
 target_link_libraries(${r4}_test PRIVATE Threads::Threads)
 add_test(NAME ${r4} COMMAND ${r4}_test)
endforeach()
foreach(r4 manual_reload_attachment19c4r4 manual_bow_handoff19c4r4)
 execute_process(COMMAND "${Python3_EXECUTABLE}" "${CMAKE_CURRENT_SOURCE_DIR}/../tools/extract_${r4}.py"
   RESULT_VARIABLE R4_EXTRACTION)
 if(NOT R4_EXTRACTION EQUAL 0)
  message(FATAL_ERROR "Could not extract current ${r4} adapter")
 endif()
 add_executable(${r4}_test ${r4}_test.cpp)
 add_test(NAME ${r4} COMMAND ${r4}_test)
endforeach()

add_executable(reading_pose26c1_test reading_pose26c1_test.cpp)
add_test(NAME reading_pose26c1 COMMAND reading_pose26c1_test)

add_executable(alchemy_palm27_test alchemy_palm27_test.cpp)
add_test(NAME alchemy_palm27 COMMAND alchemy_palm27_test)
add_executable(grab_reach43_test grab_reach43_test.cpp)
add_test(NAME grab_reach43 COMMAND grab_reach43_test)
add_executable(decap_joint44_test decap_joint44_test.cpp) # C44 loose neck: retail pe_params_joint layout, unused markers, loose build, head impulse, ragdoll wait
add_test(NAME decap_joint44 COMMAND decap_joint44_test)

set_property(DIRECTORY APPEND PROPERTY CMAKE_CONFIGURE_DEPENDS ../src/diagnostics/native_defense_install27.inl ../tools/extract_native_defense27.py)
execute_process(COMMAND "${Python3_EXECUTABLE}" "${CMAKE_CURRENT_SOURCE_DIR}/../tools/extract_native_defense27.py" RESULT_VARIABLE DEFENSE27_EXTRACT)
if(NOT DEFENSE27_EXTRACT EQUAL 0)
 message(FATAL_ERROR "Could not extract native defense receipt retirement")
endif()
add_executable(native_defense_receipt27_test native_defense_receipt27_test.cpp)
add_test(NAME native_defense_receipt27 COMMAND native_defense_receipt27_test)
add_executable(dialogue_camera_policy28_test dialogue_camera_policy28_test.cpp)
target_compile_features(dialogue_camera_policy28_test PRIVATE cxx_std_20)
add_test(NAME dialogue_camera_policy28 COMMAND dialogue_camera_policy28_test)
add_executable(dialogue_native_head28_test dialogue_native_head28_test.cpp)
target_compile_features(dialogue_native_head28_test PRIVATE cxx_std_20)
add_test(NAME dialogue_native_head28 COMMAND dialogue_native_head28_test)
set_property(DIRECTORY APPEND PROPERTY CMAKE_CONFIGURE_DEPENDS
 "${CMAKE_CURRENT_SOURCE_DIR}/../src/frik_pose_probe.cpp"
 "${CMAKE_CURRENT_SOURCE_DIR}/../tools/extract_dialogue_native_bridge28.py")
execute_process(COMMAND "${Python3_EXECUTABLE}" "${CMAKE_CURRENT_SOURCE_DIR}/../tools/extract_dialogue_native_bridge28.py"
 "${CMAKE_CURRENT_BINARY_DIR}/dialogue_native_bridge28_extracted.inc"
 RESULT_VARIABLE DIALOGUE_BRIDGE28_EXTRACT)
if(NOT DIALOGUE_BRIDGE28_EXTRACT EQUAL 0)
 message(FATAL_ERROR "Could not extract actual native dialogue head bridge")
endif()
add_executable(dialogue_native_bridge28_test dialogue_native_bridge28_test.cpp)
target_compile_features(dialogue_native_bridge28_test PRIVATE cxx_std_20)
target_include_directories(dialogue_native_bridge28_test PRIVATE "${CMAKE_CURRENT_BINARY_DIR}")
target_link_libraries(dialogue_native_bridge28_test PRIVATE Threads::Threads)
add_test(NAME dialogue_native_bridge28 COMMAND dialogue_native_bridge28_test)
set_property(DIRECTORY APPEND PROPERTY CMAKE_CONFIGURE_DEPENDS
 "${CMAKE_CURRENT_SOURCE_DIR}/../src/vrik/physical_native.inl"
 "${CMAKE_CURRENT_SOURCE_DIR}/../tools/extract_direct_consumer28.py")
execute_process(COMMAND "${Python3_EXECUTABLE}" "${CMAKE_CURRENT_SOURCE_DIR}/../tools/extract_direct_consumer28.py"
 RESULT_VARIABLE DIRECT28_EXTRACT)
if(NOT DIRECT28_EXTRACT EQUAL 0)
 message(FATAL_ERROR "Could not extract actual DIRECT command consumer")
endif()
add_executable(direct_forward_onset28_test direct_forward_onset28_test.cpp)
add_test(NAME direct_forward_onset28 COMMAND direct_forward_onset28_test)

add_executable(graphics_owner31_test graphics_owner31_test.cpp)
target_compile_features(graphics_owner31_test PRIVATE cxx_std_17)
add_test(NAME graphics_owner31 COMMAND graphics_owner31_test)

add_executable(startup_graphics32_test startup_graphics32_test.cpp)
target_compile_features(startup_graphics32_test PRIVATE cxx_std_17)
add_test(NAME startup_graphics32 COMMAND startup_graphics32_test)

add_executable(reading_pose32_test reading_pose32_test.cpp)
target_link_libraries(reading_pose32_test PRIVATE Threads::Threads)
add_test(NAME reading_pose32 COMMAND reading_pose32_test)

set_property(DIRECTORY APPEND PROPERTY CMAKE_CONFIGURE_DEPENDS ../src/xr_manager.cpp ../tools/extract_hud_lag32.py)
execute_process(COMMAND "${Python3_EXECUTABLE}" "${CMAKE_CURRENT_SOURCE_DIR}/../tools/extract_hud_lag32.py"
 RESULT_VARIABLE HUD_LAG32_EXTRACT)
if(NOT HUD_LAG32_EXTRACT EQUAL 0)
 message(FATAL_ERROR "Could not extract actual HUD compositor lag")
endif()
add_executable(hud_lag32_test hud_lag32_test.cpp)
add_test(NAME hud_lag32 COMMAND hud_lag32_test)

add_executable(door_signal32_lifecycle_test door_signal32_lifecycle_test.cpp)
target_include_directories(door_signal32_lifecycle_test BEFORE PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/syntax_shims")
add_test(NAME door_signal32_lifecycle COMMAND door_signal32_lifecycle_test)

add_executable(alchemy_point32_test alchemy_point32_test.cpp)
add_test(NAME alchemy_point32 COMMAND alchemy_point32_test)

# C32 actual Windows proxy scanner/trace/worker/retry bodies; no product DLL load.
if(WIN32)
 set_property(DIRECTORY APPEND PROPERTY CMAKE_CONFIGURE_DEPENDS
  "${CMAKE_CURRENT_SOURCE_DIR}/../src/mod.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/../tools/extract_proxy_worker32.py")
 execute_process(COMMAND "${Python3_EXECUTABLE}" "${CMAKE_CURRENT_SOURCE_DIR}/../tools/extract_proxy_worker32.py"
  RESULT_VARIABLE PROXY32_EXTRACT)
 if(NOT PROXY32_EXTRACT EQUAL 0)
  message(FATAL_ERROR "Could not extract current proxy startup worker and retry call")
 endif()
 add_executable(proxy_startup32_test proxy_startup32_test.cpp)
 add_test(NAME proxy_startup32 COMMAND proxy_startup32_test)
endif()

# Actual physical-bow producer, after Python discovery above.
set_property(DIRECTORY APPEND PROPERTY CMAKE_CONFIGURE_DEPENDS ../src/frik_pose_probe.cpp ../tools/extract_bow_camera35.py)
execute_process(COMMAND "${Python3_EXECUTABLE}" "${CMAKE_CURRENT_SOURCE_DIR}/../tools/extract_bow_camera35.py"
 RESULT_VARIABLE BOW_CAMERA35_EXTRACT)
if(NOT BOW_CAMERA35_EXTRACT EQUAL 0)
 message(FATAL_ERROR "Could not extract actual bow camera producer")
endif()
add_executable(bow_camera35_test bow_camera35_test.cpp)
add_test(NAME bow_camera35 COMMAND bow_camera35_test)

# C36 actual physical kernel guard-to-forward-stroke contract.
add_executable(guard_cut36_test guard_cut36_test.cpp)
target_include_directories(guard_cut36_test PRIVATE ../src)
add_test(NAME guard_cut36 COMMAND guard_cut36_test)

add_executable(direct_response36_test direct_response36_test.cpp)
target_include_directories(direct_response36_test PRIVATE ../src)
add_test(NAME direct_response36 COMMAND direct_response36_test)

add_executable(counter_capture36_test counter_capture36_test.cpp)
target_include_directories(counter_capture36_test PRIVATE ../src)
add_test(NAME counter_capture36 COMMAND counter_capture36_test)
# C37 exact player identity ledger (NPC SetIK interleave, same-frame, replacement, save/load, lease).
add_executable(player_identity37_test player_identity37_test.cpp)
target_include_directories(player_identity37_test PRIVATE ../src)
add_test(NAME player_identity37 COMMAND player_identity37_test)

# C37 read-only NPC defense / action-slot observation layouts.
add_executable(defense_observe37_test defense_observe37_test.cpp)
target_include_directories(defense_observe37_test PRIVATE ../src)
add_test(NAME defense_observe37 COMMAND defense_observe37_test)

add_executable(kick_observe37_test kick_observe37_test.cpp)
add_test(NAME kick_observe37 COMMAND kick_observe37_test)

add_executable(kick_realhit37c2_test kick_realhit37c2_test.cpp)
add_test(NAME kick_realhit37c2 COMMAND kick_realhit37c2_test)

# C38: VRIK over-reach hard limit (production soft cap + close-reach + solve_arm) and the F5 reach/visibility receipt logic.
add_executable(reach_view38_test reach_view38_test.cpp)
add_test(NAME reach_view38 COMMAND reach_view38_test)

add_executable(finger_tuning40_test finger_tuning40_test.cpp)
add_test(NAME finger_tuning40 COMMAND finger_tuning40_test)

add_executable(release_nock40_test release_nock40_test.cpp)
add_test(NAME release_nock40 COMMAND release_nock40_test)
add_executable(direct_counter39_test direct_counter39_test.cpp) # C39 DIRECT guard out of stagger + counter receipts
add_test(NAME direct_counter39 COMMAND direct_counter39_test)
add_executable(counter_stab40_test counter_stab40_test.cpp) # C40 DIRECT counter stab qualification + strength bounds + ability lookup
add_test(NAME counter_stab40 COMMAND counter_stab40_test)
add_executable(master_gate47_test master_gate47_test.cpp)
add_executable(pickup_use_fallback_r120_test pickup_use_fallback_r120_test.cpp)
add_executable(constraint60_test constraint60_test.cpp)
add_executable(grab_request60_test grab_request60_test.cpp)
add_executable(grab_connected_fixture_test grab_connected_fixture_test.cpp)
add_executable(grab_backend60_test grab_backend60_test.cpp) # Codex: backend identity/ACK/retire fixture
add_test(NAME grab_backend60 COMMAND grab_backend60_test)
add_executable(kick_policy60_test kick_policy60_test.cpp)
add_executable(counter_outcome81_test counter_outcome81_test.cpp)
add_executable(impale_hold60_test impale_hold60_test.cpp) # Claude CL-IMPALE60 pure hold
add_test(NAME impale_hold60 COMMAND impale_hold60_test) # DS-F81 master our-outcome
add_test(NAME counter_outcome81 COMMAND counter_outcome81_test) # DS-F80 kick cannot-kill tier
add_test(NAME kick_policy60 COMMAND kick_policy60_test)
add_executable(grab_producer60_test grab_producer60_test.cpp) # Codex: production-connected producer test
add_executable(grab_release_input_h21_test grab_release_input_h21_test.cpp)
target_compile_features(grab_release_input_h21_test PRIVATE cxx_std_20)
if(MSVC)
 target_compile_options(grab_release_input_h21_test PRIVATE /Gy /Gw)
 target_link_options(grab_release_input_h21_test PRIVATE /OPT:REF /INCREMENTAL:NO)
endif()
add_test(NAME grab_release_input_h21 COMMAND grab_release_input_h21_test)
add_test(NAME grab_producer60 COMMAND grab_producer60_test) # DS-F69 connected proxy fixture
add_test(NAME grab_connected_fixture COMMAND grab_connected_fixture_test) # DS-F63 grab request policy
add_test(NAME grab_request60 COMMAND grab_request60_test)
add_executable(native_hand_proxy_t4_test native_hand_proxy_t4_test.cpp) # DS-F52 hand grip proxy lifecycle policy
add_test(NAME native_hand_proxy_t4 COMMAND native_hand_proxy_t4_test)
add_executable(constraint_packet60_test constraint_packet60_test.cpp) # DS-F43 retail add-constraint packet builder
add_test(NAME constraint_packet60 COMMAND constraint_packet60_test)
add_executable(constraint_adapter60_test constraint_adapter60_test.cpp) # DS-F39 machine-FIFO adapter bridge
add_test(NAME constraint_adapter60 COMMAND constraint_adapter60_test)
add_executable(door_press_edge_test door_press_edge_test.cpp) # DS-F25 press-edge door token chain
add_test(NAME door_press_edge COMMAND door_press_edge_test)
add_executable(constraint_op60_test constraint_op60_test.cpp) # DS-F20 constraint op-drain queue
add_test(NAME constraint_op60 COMMAND constraint_op60_test) # DS-F11 typed add/remove-constraint service state machine
add_test(NAME constraint60 COMMAND constraint60_test) # C47 master-strike gate policy + kill-move rule
add_test(NAME master_gate47 COMMAND master_gate47_test)
add_test(NAME pickup_use_fallback_r120 COMMAND pickup_use_fallback_r120_test)

add_executable(defense_direction40_test defense_direction40_test.cpp)
add_test(NAME defense_direction40 COMMAND defense_direction40_test)

add_executable(reading_pages42_test reading_pages42_test.cpp)
add_test(NAME reading_pages42 COMMAND reading_pages42_test)
add_executable(book_pages45_test book_pages45_test.cpp) # C45 thumbrest / flick / native page arbiter
add_test(NAME book_pages45 COMMAND book_pages45_test)
add_executable(deliberate_cut42_test deliberate_cut42_test.cpp)
add_test(NAME deliberate_cut42 COMMAND deliberate_cut42_test)

execute_process(COMMAND "${Python3_EXECUTABLE}" "${CMAKE_CURRENT_SOURCE_DIR}/../tools/extract_held_visual43.py" RESULT_VARIABLE HELD43_EXTRACT)
if(NOT HELD43_EXTRACT EQUAL 0)
 message(FATAL_ERROR "Could not extract held-prop visual permission")
endif()
add_executable(held_visual43_test held_visual43_test.cpp)
add_test(NAME held_visual43 COMMAND held_visual43_test)
add_executable(recovery43_test recovery43_test.cpp)
add_test(NAME recovery43 COMMAND recovery43_test)
add_executable(handling44_test handling44_test.cpp)
add_test(NAME handling44 COMMAND handling44_test)
add_executable(holster45_test holster45_test.cpp)
add_test(NAME holster45 COMMAND holster45_test)

add_executable(recovery47_test recovery47_test.cpp)
add_test(NAME recovery47 COMMAND recovery47_test)

# The monolithic weapon fixture keeps many State/Input/Output objects in main.
# ASAN redzones and Debug non-reused locals exceed the default Windows 1MB test stack. This changes only
# this host test executable, not the ASI or any game-thread stack setting.
if(MSVC)
 target_link_options(physical_weapon_test PRIVATE /STACK:8388608)
endif()

# C43 review: actual activity camera cull/publication cache transaction.
set_property(DIRECTORY APPEND PROPERTY CMAKE_CONFIGURE_DEPENDS
 ../src/camera_hook.cpp ../tools/extract_activity_eye_camera43.py)
execute_process(COMMAND "${Python3_EXECUTABLE}" "${CMAKE_CURRENT_SOURCE_DIR}/../tools/extract_activity_eye_camera43.py"
 RESULT_VARIABLE ACTIVITY_EYE_CAMERA43_EXTRACT)
if(NOT ACTIVITY_EYE_CAMERA43_EXTRACT EQUAL 0)
 message(FATAL_ERROR "Could not extract actual activity eye camera adapter")
endif()
add_executable(activity_eye_camera43_test activity_eye_camera43_test.cpp)
add_test(NAME activity_eye_camera43 COMMAND activity_eye_camera43_test)

execute_process(COMMAND "${Python3_EXECUTABLE}" "${CMAKE_CURRENT_SOURCE_DIR}/../tools/extract_guided_candidate55.py"
 WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/.." RESULT_VARIABLE guided_extract55)
if(NOT guided_extract55 EQUAL 0)
 message(FATAL_ERROR "Could not extract actual generic guided candidate producer")
endif()
# C46R1 native interaction producer / ownership contracts.
foreach(c46 guided_crafting55 guided_crafting55_native_bridge book_input46 interaction_receipts46 workstation_sample46 native_activity_hands46 semi_workstations46 alchemy_semi46 semi_smith46 door_use46 door_producer46 door_service46)
 add_executable(${c46}_test ${c46}_test.cpp)
 add_test(NAME ${c46} COMMAND ${c46}_test)
endforeach()
add_executable(door_gap_h31_test door_gap_h31_test.cpp)
add_test(NAME door_gap_h31 COMMAND door_gap_h31_test)
add_executable(door_direct54r1_test door_direct54r1_test.cpp)
target_include_directories(door_direct54r1_test PRIVATE syntax_shims ../src)
target_link_libraries(door_direct54r1_test PRIVATE Threads::Threads)
add_test(NAME door_direct54r1 COMMAND door_direct54r1_test)
add_executable(door_flow64_test door_flow64_test.cpp)
target_include_directories(door_flow64_test PRIVATE syntax_shims ../src)
target_link_libraries(door_flow64_test PRIVATE Threads::Threads)
add_test(NAME door_flow64 COMMAND door_flow64_test)
add_executable(door_flow65_test door_flow65_test.cpp)
target_include_directories(door_flow65_test PRIVATE syntax_shims ../src)
target_link_libraries(door_flow65_test PRIVATE Threads::Threads)
add_test(NAME door_flow65 COMMAND door_flow65_test)
execute_process(COMMAND "${Python3_EXECUTABLE}" "${CMAKE_CURRENT_SOURCE_DIR}/../tools/extract_door_origin_h12.py"
 RESULT_VARIABLE door_origin_h12_extract)
if(NOT door_origin_h12_extract EQUAL 0)
 message(FATAL_ERROR "Could not extract current door origin native readers")
endif()
add_executable(door_origin_h12_test door_origin_h12_test.cpp)
target_include_directories(door_origin_h12_test PRIVATE ../src)
add_test(NAME door_origin_h12 COMMAND door_origin_h12_test)
add_executable(door_surface_query65_test door_surface_query65_test.cpp)
target_include_directories(door_surface_query65_test PRIVATE syntax_shims ../src)
add_test(NAME door_surface_query65 COMMAND door_surface_query65_test)
add_executable(native_door_hold65_test native_door_hold65_test.cpp)
target_include_directories(native_door_hold65_test PRIVATE syntax_shims ../src)
target_link_libraries(native_door_hold65_test PRIVATE Threads::Threads)
add_test(NAME native_door_hold65 COMMAND native_door_hold65_test)

foreach(c46native alchemy_semi46_native semi_smith46_native)
 add_executable(${c46native}_test ${c46native}_test.cpp)
 target_include_directories(${c46native}_test PRIVATE syntax_shims ../src)
 target_link_libraries(${c46native}_test PRIVATE Threads::Threads)
 if(MSVC)
  target_compile_options(${c46native}_test PRIVATE /Gy /Gw /bigobj)
  target_link_options(${c46native}_test PRIVATE /OPT:REF)
 else()
  target_compile_options(${c46native}_test PRIVATE -fms-extensions -ffunction-sections -fdata-sections)
  target_link_options(${c46native}_test PRIVATE -Wl,--gc-sections)
 endif()
 add_test(NAME ${c46native} COMMAND ${c46native}_test)
endforeach()

# C51R1: actual animation event detours and scoped tracking publication.
add_executable(combat_events51_bridge combat_events51_bridge.cpp)
target_include_directories(combat_events51_bridge PRIVATE syntax_shims openxr_sdk)
target_compile_definitions(combat_events51_bridge PRIVATE _ReturnAddress=HostReturnAddress)
add_test(NAME combat_events51_bridge COMMAND combat_events51_bridge)
add_executable(workstation_publication51_bridge workstation_publication51_bridge_test.cpp bridge_external_stubs112.cpp ../src/subsystems/doors/module.cpp)
target_include_directories(workstation_publication51_bridge PRIVATE syntax_shims openxr_sdk)
target_compile_definitions(workstation_publication51_bridge PRIVATE _ReturnAddress=HostReturnAddress)
target_link_libraries(workstation_publication51_bridge PRIVATE Threads::Threads)
if(MSVC)
 target_compile_options(workstation_publication51_bridge PRIVATE /Gy /Gw /bigobj)
 target_link_options(workstation_publication51_bridge PRIVATE /OPT:REF /INCREMENTAL:NO)
else()
 target_compile_options(workstation_publication51_bridge PRIVATE -fms-extensions -ffunction-sections -fdata-sections)
 target_link_options(workstation_publication51_bridge PRIVATE -Wl,--gc-sections)
endif()
add_test(NAME workstation_publication51_bridge COMMAND workstation_publication51_bridge)

add_executable(comfort_vignette51_test comfort_vignette51_test.cpp)
add_test(NAME comfort_vignette51 COMMAND comfort_vignette51_test)
if(WIN32)
 add_executable(presentation_shader_h29_test presentation_shader_h29_test.cpp)
 target_link_libraries(presentation_shader_h29_test PRIVATE d3dcompiler)
 add_test(NAME presentation_shader_h29 COMMAND presentation_shader_h29_test "${CMAKE_CURRENT_SOURCE_DIR}/../src/xr_manager.cpp")
endif()

# C51R1: shared tracked frame, actual coherent pose, and current camera reader.
set_property(DIRECTORY APPEND PROPERTY CMAKE_CONFIGURE_DEPENDS
 ../src/frik_pose_probe.cpp ../src/vrik/tracked_frame51.h
 ../src/vrik/coherent_pose.inl ../tools/extract_tracked_frame51.py)
execute_process(COMMAND "${Python3_EXECUTABLE}" "${CMAKE_CURRENT_SOURCE_DIR}/../tools/extract_tracked_frame51.py"
 RESULT_VARIABLE TRACKED_FRAME51_EXTRACT)
if(NOT TRACKED_FRAME51_EXTRACT EQUAL 0)
 message(FATAL_ERROR "Could not extract current tracked-frame camera reader")
endif()
foreach(c51frame tracked_frame51 tracked_frame51_pose tracked_frame51_camera)
 add_executable(${c51frame}_test ${c51frame}_test.cpp)
 target_include_directories(${c51frame}_test PRIVATE ../src)
 target_link_libraries(${c51frame}_test PRIVATE Threads::Threads)
 add_test(NAME ${c51frame} COMMAND ${c51frame}_test)
endforeach()

# Collect after all executable declarations, including later review fixtures.
# A focused ASAN target must work without first building an unrelated test.
function(kcd2vr_stage_test_asan54)
 get_filename_component(KCD2VR_TEST_COMPILER_DIR "${CMAKE_CXX_COMPILER}" DIRECTORY)
 find_file(KCD2VR_TEST_ASAN_DLL clang_rt.asan_dynamic-x86_64.dll
   PATHS "${KCD2VR_TEST_COMPILER_DIR}" NO_DEFAULT_PATH REQUIRED)
 get_property(KCD2VR_TEST_TARGETS DIRECTORY PROPERTY BUILDSYSTEM_TARGETS)
 foreach(test_target IN LISTS KCD2VR_TEST_TARGETS)
  get_target_property(test_type "${test_target}" TYPE)
  if(test_type STREQUAL "EXECUTABLE")
   add_custom_command(TARGET "${test_target}" POST_BUILD
     COMMAND "${CMAKE_COMMAND}" -E copy_if_different "${KCD2VR_TEST_ASAN_DLL}" "$<TARGET_FILE_DIR:${test_target}>")
  endif()
 endforeach()
endfunction()
if(MSVC AND KCD2VR_HOST_ASAN)
 cmake_language(DEFER CALL kcd2vr_stage_test_asan54)
endif()

add_executable(purposeful_cut52_test purposeful_cut52_test.cpp)
add_test(NAME purposeful_cut52 COMMAND purposeful_cut52_test)

add_executable(controls54_test controls54_test.cpp)
add_test(NAME controls54 COMMAND controls54_test)

# C54 exact native prop grip presentation; full coherent-pose production helper.
add_executable(grasp_finger54_bridge grasp_finger54_bridge.cpp bridge_external_stubs112.cpp ../src/subsystems/doors/module.cpp)
target_include_directories(grasp_finger54_bridge PRIVATE syntax_shims openxr_sdk)
target_compile_definitions(grasp_finger54_bridge PRIVATE _ReturnAddress=HostReturnAddress)
target_link_libraries(grasp_finger54_bridge PRIVATE Threads::Threads)
if(MSVC)
 target_compile_options(grasp_finger54_bridge PRIVATE /Gy /Gw /bigobj)
 target_link_options(grasp_finger54_bridge PRIVATE /OPT:REF /INCREMENTAL:NO)
else()
 target_compile_options(grasp_finger54_bridge PRIVATE -fms-extensions -ffunction-sections -fdata-sections)
 target_link_options(grasp_finger54_bridge PRIVATE -Wl,--gc-sections)
endif()
add_test(NAME grasp_finger54_bridge COMMAND grasp_finger54_bridge)

# C55 actual XR publisher -> scoped Picking pose input, full production frik TU.
add_executable(pickup_ingress55_bridge pickup_ingress55_bridge_test.cpp bridge_external_stubs112.cpp ../src/subsystems/doors/module.cpp)
target_include_directories(pickup_ingress55_bridge PRIVATE syntax_shims openxr_sdk)
target_compile_definitions(pickup_ingress55_bridge PRIVATE _ReturnAddress=HostReturnAddress)
target_link_libraries(pickup_ingress55_bridge PRIVATE Threads::Threads)
if(MSVC)
 target_compile_options(pickup_ingress55_bridge PRIVATE /Gy /Gw /bigobj)
 target_link_options(pickup_ingress55_bridge PRIVATE /OPT:REF /INCREMENTAL:NO)
else()
 target_compile_options(pickup_ingress55_bridge PRIVATE -fms-extensions -ffunction-sections -fdata-sections)
 target_link_options(pickup_ingress55_bridge PRIVATE -Wl,--gc-sections)
endif()
add_test(NAME pickup_ingress55_bridge COMMAND pickup_ingress55_bridge)

add_executable(kick_profile52_test kick_profile52_test.cpp)
add_test(NAME kick_profile52 COMMAND kick_profile52_test)
add_executable(hybrid_contact54_test hybrid_contact54_test.cpp)
target_link_libraries(hybrid_contact54_test PRIVATE Threads::Threads)
add_test(NAME hybrid_contact54 COMMAND hybrid_contact54_test)
set_property(DIRECTORY APPEND PROPERTY CMAKE_CONFIGURE_DEPENDS ../src/subsystems/hybrid/drain_cost_claim.inl ../tools/extract_hybrid_quiet_guard_h20.py)
execute_process(COMMAND "${Python3_EXECUTABLE}" "${CMAKE_CURRENT_SOURCE_DIR}/../tools/extract_hybrid_quiet_guard_h20.py" RESULT_VARIABLE HYBRID_H20_EXTRACT)
if(NOT HYBRID_H20_EXTRACT EQUAL 0)
 message(FATAL_ERROR "Could not extract H20 native-cost contact context")
endif()
add_executable(hybrid_quiet_guard_h20_test hybrid_quiet_guard_h20_test.cpp)
target_link_libraries(hybrid_quiet_guard_h20_test PRIVATE Threads::Threads)
add_test(NAME hybrid_quiet_guard_h20 COMMAND hybrid_quiet_guard_h20_test)
# C56 review (Claude): per-site F3 receipt budget (replaces the shared per-category Allow counter)
add_executable(receipt_budget56_test receipt_budget56_test.cpp)
add_test(NAME receipt_budget56 COMMAND receipt_budget56_test)
add_executable(answer_receipt56_test answer_receipt56_test.cpp)
target_link_libraries(answer_receipt56_test PRIVATE Threads::Threads)
add_test(NAME answer_receipt56 COMMAND answer_receipt56_test)

# C54 bounded own-sword/empty-hand presentation and completed-pose pat edge.
add_executable(self_contact54_test self_contact54_test.cpp)
add_test(NAME self_contact54 COMMAND self_contact54_test)
if(MSVC)
 add_executable(self_contact54_bridge self_contact54_bridge.cpp bridge_external_stubs112.cpp ../src/subsystems/doors/module.cpp)
 target_include_directories(self_contact54_bridge PRIVATE syntax_shims openxr_sdk)
 target_compile_definitions(self_contact54_bridge PRIVATE _ReturnAddress=HostReturnAddress)
 target_compile_options(self_contact54_bridge PRIVATE /Gy /Gw /bigobj)
 target_link_options(self_contact54_bridge PRIVATE /OPT:REF /INCREMENTAL:NO)
 add_test(NAME self_contact54_bridge COMMAND self_contact54_bridge)
endif()

add_executable(dagger_chest54_test dagger_chest54_test.cpp)
add_test(NAME dagger_chest54 COMMAND dagger_chest54_test)
add_executable(utility_gestures54_test utility_gestures54_test.cpp)
add_test(NAME utility_gestures54 COMMAND utility_gestures54_test)

add_executable(hud_semantic54_test hud_semantic54_test.cpp)
target_link_libraries(hud_semantic54_test PRIVATE Threads::Threads)
add_test(NAME hud_semantic54 COMMAND hud_semantic54_test)

add_executable(native_cost_packet54_test native_cost_packet54_test.cpp)
add_test(NAME native_cost_packet54 COMMAND native_cost_packet54_test)

if(MSVC)
 add_executable(handling55_bridge handling55_bridge.cpp bridge_external_stubs112.cpp ../src/subsystems/doors/module.cpp)
 target_compile_features(handling55_bridge PRIVATE cxx_std_23)
 target_include_directories(handling55_bridge PRIVATE syntax_shims openxr_sdk)
 target_compile_definitions(handling55_bridge PRIVATE _ReturnAddress=HostReturnAddress)
 target_compile_options(handling55_bridge PRIVATE /Gy /Gw /bigobj)
 target_link_options(handling55_bridge PRIVATE /OPT:REF /INCREMENTAL:NO)
 add_test(NAME handling55_bridge COMMAND handling55_bridge)
endif()

add_executable(blood_anchor55_test blood_anchor55_test.cpp)
add_test(NAME blood_anchor55 COMMAND blood_anchor55_test)
# C55 actual coherent physical caller: weapons stay checked while hands are optional.
add_executable(weapon_contact_policy55_test weapon_contact_policy55_test.cpp)
add_test(NAME weapon_contact_policy55 COMMAND weapon_contact_policy55_test)

# C56 exact manual native producer -> physical draw handoff, synthetic native leaves.
add_executable(reload_handoff56_test reload_handoff56_test.cpp)
add_test(NAME reload_handoff56 COMMAND reload_handoff56_test)

# C57R1 actual context producer/publisher, independent of HUD visibility.
execute_process(COMMAND ${Python3_EXECUTABLE} "${CMAKE_CURRENT_SOURCE_DIR}/../tools/extract_pose_policy57.py" COMMAND_ERROR_IS_FATAL ANY)
add_executable(pose_policy57_test pose_policy57_test.cpp)
target_compile_features(pose_policy57_test PRIVATE cxx_std_20)
add_test(NAME pose_policy57 COMMAND pose_policy57_test)
add_executable(receipt_capture57_test receipt_capture57_test.cpp)
add_test(NAME receipt_capture57 COMMAND receipt_capture57_test)

if(MSVC)
 foreach(t2 native_script_t2 held_map_feed_t2)
  add_executable(${t2}_test ${t2}_test.cpp)
  target_compile_features(${t2}_test PRIVATE cxx_std_23)
  target_include_directories(${t2}_test PRIVATE syntax_shims)
  target_link_libraries(${t2}_test PRIVATE Threads::Threads)
  add_test(NAME ${t2} COMMAND ${t2}_test)
 endforeach()
endif()

add_executable(own_shield_contact58_test own_shield_contact58_test.cpp)
add_test(NAME own_shield_contact58 COMMAND own_shield_contact58_test)

execute_process(COMMAND "${Python3_EXECUTABLE}" "${CMAKE_CURRENT_SOURCE_DIR}/../tools/extract_environment_delivery58.py"
 "${CMAKE_CURRENT_BINARY_DIR}/environment_delivery58.inc" RESULT_VARIABLE ENVIRONMENT_DELIVERY58_EXTRACT)
if(NOT ENVIRONMENT_DELIVERY58_EXTRACT EQUAL 0)
 message(FATAL_ERROR "Could not extract actual environment FX delivery")
endif()
add_executable(environment_delivery58_test environment_delivery58_test.cpp)
target_include_directories(environment_delivery58_test PRIVATE "${CMAKE_CURRENT_BINARY_DIR}")
add_test(NAME environment_delivery58 COMMAND environment_delivery58_test)

# T3: local own-equipment constraint uses only the already world-checked paths.
add_executable(own_shield_constraint60_test own_shield_constraint60_test.cpp)
add_test(NAME own_shield_constraint60 COMMAND own_shield_constraint60_test)
if(MSVC)
 add_executable(own_shield_constraint60_bridge own_shield_constraint60_bridge.cpp bridge_external_stubs112.cpp ../src/subsystems/doors/module.cpp)
 target_compile_features(own_shield_constraint60_bridge PRIVATE cxx_std_23)
 target_include_directories(own_shield_constraint60_bridge PRIVATE syntax_shims openxr_sdk)
 target_compile_definitions(own_shield_constraint60_bridge PRIVATE _ReturnAddress=HostReturnAddress)
 target_compile_options(own_shield_constraint60_bridge PRIVATE /Gy /Gw /bigobj)
 target_link_options(own_shield_constraint60_bridge PRIVATE /OPT:REF /INCREMENTAL:NO)
 add_test(NAME own_shield_constraint60_bridge COMMAND own_shield_constraint60_bridge)
endif()

# T3: restored native quantity-one release and finite inventory-owned hip stock.
add_executable(native_supply_t2_test native_supply_t2_test.cpp)
add_test(NAME supply_t2 COMMAND native_supply_t2_test)
# Combined T15: actual inventory holster policy and extracted native rearm block.
set_property(DIRECTORY APPEND PROPERTY CMAKE_CONFIGURE_DEPENDS ../src/vrik/physical_native.inl ../tools/extract_release_rearm_t15.py)
add_executable(ghost_holsters_t15_test ghost_holsters_t15_test.cpp)
add_test(NAME ghost_holsters_t15 COMMAND ghost_holsters_t15_test)
execute_process(COMMAND "${Python3_EXECUTABLE}" "${CMAKE_CURRENT_SOURCE_DIR}/../tools/extract_release_rearm_t15.py"
 "${CMAKE_CURRENT_BINARY_DIR}/release_rearm_t15_bridge.cpp" COMMAND_ERROR_IS_FATAL ANY)
add_executable(release_rearm_t15_bridge "${CMAKE_CURRENT_BINARY_DIR}/release_rearm_t15_bridge.cpp")
add_test(NAME release_rearm_t15_bridge COMMAND release_rearm_t15_bridge)
set_property(DIRECTORY APPEND PROPERTY CMAKE_CONFIGURE_DEPENDS ../src/vr_menu.cpp ../src/subsystems/base/pose_physical_executor.inl ../src/subsystems/experimental/deepseek_options_h29.h ../tools/extract_deepseek_options_h31.py)
execute_process(COMMAND "${Python3_EXECUTABLE}" "${CMAKE_CURRENT_SOURCE_DIR}/../tools/extract_deepseek_options_h31.py"
 "${CMAKE_CURRENT_BINARY_DIR}/deepseek_options_h31_actual.cpp" COMMAND_ERROR_IS_FATAL ANY)
add_executable(deepseek_options_h31_test "${CMAKE_CURRENT_BINARY_DIR}/deepseek_options_h31_actual.cpp")
add_test(NAME deepseek_options_h31 COMMAND deepseek_options_h31_test)
add_executable(wall_backout_h31_test wall_backout_h31_test.cpp)
add_test(NAME wall_backout_h31 COMMAND wall_backout_h31_test)


if(MSVC)
 add_executable(weapon_release_t1_bridge weapon_release_t1_bridge.cpp bridge_external_stubs112.cpp ../src/subsystems/doors/module.cpp)
 target_compile_features(weapon_release_t1_bridge PRIVATE cxx_std_23)
 target_include_directories(weapon_release_t1_bridge PRIVATE syntax_shims openxr_sdk)
 target_compile_definitions(weapon_release_t1_bridge PRIVATE _ReturnAddress=HostReturnAddress)
 target_compile_options(weapon_release_t1_bridge PRIVATE /Gy /Gw /bigobj)
 target_link_options(weapon_release_t1_bridge PRIVATE /OPT:REF /INCREMENTAL:NO)
 add_test(NAME weapon_release_t1_bridge COMMAND weapon_release_t1_bridge)
endif()

# T4: actual session-only F9 control and deferred legacy-proxy restoration.
execute_process(COMMAND "${Python3_EXECUTABLE}" "${CMAKE_CURRENT_SOURCE_DIR}/../tools/extract_physical_f9_t4.py"
 "${CMAKE_CURRENT_BINARY_DIR}/physical_f9_t4.inc" COMMAND_ERROR_IS_FATAL ANY)
add_executable(physical_f9_t4_test physical_f9_t4_test.cpp)
target_compile_features(physical_f9_t4_test PRIVATE cxx_std_20)
target_include_directories(physical_f9_t4_test PRIVATE "${CMAKE_CURRENT_BINARY_DIR}")
add_test(NAME physical_f9_t4 COMMAND physical_f9_t4_test)

if(MSVC)
 add_executable(touch_addimpact_h6_test touch_addimpact_h6_test.cpp)
 target_compile_features(touch_addimpact_h6_test PRIVATE cxx_std_20)
 add_test(NAME touch_addimpact_h6 COMMAND touch_addimpact_h6_test)
endif()

add_executable(native_physics_phase_t4_test native_physics_phase_t4_test.cpp)
target_compile_features(native_physics_phase_t4_test PRIVATE cxx_std_20)
add_test(NAME native_physics_phase_t4 COMMAND native_physics_phase_t4_test)

add_executable(native_articulated_phase_h21_test native_articulated_phase_h21_test.cpp)
target_compile_features(native_articulated_phase_h21_test PRIVATE cxx_std_20)
add_test(NAME native_articulated_phase_h21 COMMAND native_articulated_phase_h21_test)
if(MSVC)
 add_executable(impale_constraint_h21_test impale_constraint_h21_test.cpp)
 target_compile_features(impale_constraint_h21_test PRIVATE cxx_std_20)
 add_test(NAME impale_constraint_h21 COMMAND impale_constraint_h21_test)
endif()

add_executable(native_held_pose_t4_test native_held_pose_t4_test.cpp)
target_compile_features(native_held_pose_t4_test PRIVATE cxx_std_20)
add_test(NAME native_held_pose_t4 COMMAND native_held_pose_t4_test)

if(MSVC)
 add_executable(native_weapon_body_t4_test native_weapon_body_t4_test.cpp)
 target_compile_features(native_weapon_body_t4_test PRIVATE cxx_std_23)
 target_include_directories(native_weapon_body_t4_test PRIVATE syntax_shims)
 target_link_libraries(native_weapon_body_t4_test PRIVATE Threads::Threads)
 add_test(NAME native_weapon_body_t4 COMMAND native_weapon_body_t4_test)
 add_executable(native_held_owner_t4_bridge native_held_owner_t4_bridge.cpp bridge_external_stubs112.cpp ../src/subsystems/doors/module.cpp)
 target_compile_features(native_held_owner_t4_bridge PRIVATE cxx_std_23)
 target_include_directories(native_held_owner_t4_bridge PRIVATE syntax_shims openxr_sdk)
 target_compile_definitions(native_held_owner_t4_bridge PRIVATE _ReturnAddress=HostReturnAddress)
 target_compile_options(native_held_owner_t4_bridge PRIVATE /Gy /Gw /bigobj)
 target_link_options(native_held_owner_t4_bridge PRIVATE /OPT:REF /INCREMENTAL:NO)
 add_test(NAME native_held_owner_t4_bridge COMMAND native_held_owner_t4_bridge)
endif()

add_executable(finger_joint59_test finger_joint59_test.cpp)
target_link_libraries(finger_joint59_test PRIVATE Threads::Threads)
add_test(NAME finger_joint59 COMMAND finger_joint59_test)

add_executable(guard_origin59_test guard_origin59_test.cpp)
add_test(NAME guard_origin59 COMMAND guard_origin59_test)

add_executable(gore_visibility59_test gore_visibility59_test.cpp)
add_test(NAME gore_visibility59 COMMAND gore_visibility59_test)

add_executable(native_observation59_test native_observation59_test.cpp)
add_test(NAME native_observation59 COMMAND native_observation59_test)

add_executable(impale_native_handoff59_test impale_native_handoff59_test.cpp)
add_test(NAME impale_native_handoff59 COMMAND impale_native_handoff59_test)
execute_process(COMMAND "${Python3_EXECUTABLE}" "${CMAKE_CURRENT_SOURCE_DIR}/../tools/extract_impale_successor_h18.py"
 --living-c40 "${CMAKE_CURRENT_BINARY_DIR}/actual_living_anchor.inc" RESULT_VARIABLE LIVING_C40_EXTRACT)
if(NOT LIVING_C40_EXTRACT EQUAL 0)
 message(FATAL_ERROR "Could not extract actual C40 evaluated-joint display attachment")
endif()
execute_process(COMMAND "${Python3_EXECUTABLE}" "${CMAKE_CURRENT_SOURCE_DIR}/../tools/extract_impale_successor_h18.py"
 --impale-options "${CMAKE_CURRENT_BINARY_DIR}/actual_impale_options.inc" RESULT_VARIABLE IMPALE_OPTIONS_EXTRACT)
if(NOT IMPALE_OPTIONS_EXTRACT EQUAL 0)
 message(FATAL_ERROR "Could not extract actual C40 physical option ingress")
endif()
add_executable(living_impale_c40_test impale_native_handoff59_test.cpp)
target_compile_definitions(living_impale_c40_test PRIVATE KCD2VR_LIVING_IMPALE_C40=1)
target_include_directories(living_impale_c40_test PRIVATE "${CMAKE_CURRENT_BINARY_DIR}")
add_test(NAME living_impale_c40 COMMAND living_impale_c40_test)
add_executable(impale_pending59_test impale_pending59_test.cpp)
add_test(NAME impale_pending59 COMMAND impale_pending59_test)

add_executable(hybrid_serial59_test hybrid_serial59_test.cpp)
add_test(NAME hybrid_serial59 COMMAND hybrid_serial59_test)
execute_process(COMMAND "${Python3_EXECUTABLE}" "${CMAKE_CURRENT_SOURCE_DIR}/../tools/extract_hybrid_order59.py"
 "${CMAKE_CURRENT_BINARY_DIR}/hybrid_order59.inc" RESULT_VARIABLE HYBRID_ORDER59_EXTRACT)
if(NOT HYBRID_ORDER59_EXTRACT EQUAL 0)
 message(FATAL_ERROR "Could not extract actual HYBRID contact queue")
endif()
add_executable(hybrid_order59_test hybrid_order59_test.cpp)
target_include_directories(hybrid_order59_test PRIVATE "${CMAKE_CURRENT_BINARY_DIR}")
add_test(NAME hybrid_order59 COMMAND hybrid_order59_test)
add_executable(hybrid_entry59_test hybrid_entry59_test.cpp)
target_include_directories(hybrid_entry59_test PRIVATE "${CMAKE_CURRENT_BINARY_DIR}")
add_test(NAME hybrid_entry59 COMMAND hybrid_entry59_test)

add_executable(feint_chain59_test feint_chain59_test.cpp)
add_test(NAME feint_chain59 COMMAND feint_chain59_test)

# C62: exercise the actually merged lab policy under the normal host runner.
foreach(lab lab58 lab_prop58)
 add_executable(${lab}_test ${lab}_test.cpp)
 add_test(NAME ${lab} COMMAND ${lab}_test)
endforeach()

add_executable(cut_feint62_test cut_feint62_test.cpp)
add_test(NAME cut_feint62 COMMAND cut_feint62_test)
add_executable(feint_hybrid_fe_test feint_hybrid_fe_test.cpp) # D34 FE VrFeintHybrid
add_test(NAME feint_hybrid_fe COMMAND feint_hybrid_fe_test)

add_executable(cut_feint62_driver_test cut_feint62_driver_test.cpp)
add_test(NAME cut_feint62_driver COMMAND cut_feint62_driver_test)

# C62: actual production contact-submit witness, native reads explicitly doubled.
execute_process(COMMAND "${Python3_EXECUTABLE}" "${CMAKE_CURRENT_SOURCE_DIR}/../tools/extract_part_namespace62.py"
 "${CMAKE_CURRENT_BINARY_DIR}/pre_submit_namespace62.inc" COMMAND_ERROR_IS_FATAL ANY)
add_executable(part_namespace62_test part_namespace62_test.cpp)
target_include_directories(part_namespace62_test PRIVATE "${CMAKE_CURRENT_BINARY_DIR}")
add_test(NAME part_namespace62 COMMAND part_namespace62_test)

# Execute the actual native-frame torch consumer; native leaves are explicit doubles.
add_executable(native_torch62_test native_torch62_test.cpp)
target_include_directories(native_torch62_test PRIVATE syntax_shims)
add_test(NAME native_torch62 COMMAND native_torch62_test)

add_executable(helmet_overlay63_test helmet_overlay63_test.cpp)
add_test(NAME helmet_overlay63 COMMAND helmet_overlay63_test)

execute_process(COMMAND "${Python3_EXECUTABLE}" "${CMAKE_CURRENT_SOURCE_DIR}/../tools/extract_helmet_overlay63.py"
 "${CMAKE_CURRENT_BINARY_DIR}/helmet_runtime63.inc" COMMAND_ERROR_IS_FATAL ANY)
add_executable(helmet_runtime63_test helmet_runtime63_test.cpp)
target_include_directories(helmet_runtime63_test PRIVATE "${CMAKE_CURRENT_BINARY_DIR}")
add_test(NAME helmet_runtime63 COMMAND helmet_runtime63_test)

execute_process(COMMAND "${Python3_EXECUTABLE}" "${CMAKE_CURRENT_SOURCE_DIR}/../tools/extract_held_capture63.py"
 COMMAND_ERROR_IS_FATAL ANY)
add_executable(native_held_capture63_test native_held_capture63_test.cpp)
set_property(TARGET native_held_capture63_test PROPERTY CXX_STANDARD 20)
add_test(NAME native_held_capture63 COMMAND native_held_capture63_test)

# C63: actual original-first native LookAt resolver, explicit native leaves.
add_executable(npc_look63_test npc_look63_test.cpp)
target_include_directories(npc_look63_test PRIVATE syntax_shims)
add_test(NAME npc_look63 COMMAND npc_look63_test)
add_test(NAME npc_look63_disable_fail COMMAND npc_look63_test disable-fail)
add_test(NAME npc_look63_enable_fail COMMAND npc_look63_test enable-fail)

if(MSVC)
 execute_process(COMMAND "${Python3_EXECUTABLE}" "${CMAKE_CURRENT_SOURCE_DIR}/../tools/extract_npc_look_read63.py"
  "${CMAKE_CURRENT_BINARY_DIR}/npc_look_read63.inc" COMMAND_ERROR_IS_FATAL ANY)
 add_executable(npc_look63_fault_test npc_look63_fault_test.cpp)
 target_include_directories(npc_look63_fault_test PRIVATE "${CMAKE_CURRENT_BINARY_DIR}")
 add_test(NAME npc_look63_fault COMMAND npc_look63_fault_test)
endif()

# C63: support-aware controller intent and actual geometric bow draw/preflight.
add_executable(two_hand_intent63_test two_hand_intent63_test.cpp)
add_test(NAME two_hand_intent63 COMMAND two_hand_intent63_test)
execute_process(COMMAND "${Python3_EXECUTABLE}" "${CMAKE_CURRENT_SOURCE_DIR}/../tools/extract_bow_release63.py"
 "${CMAKE_CURRENT_BINARY_DIR}/bow_release63.inc" COMMAND_ERROR_IS_FATAL ANY)
add_executable(bow_pull63_test bow_pull63_test.cpp)
target_include_directories(bow_pull63_test PRIVATE "${CMAKE_CURRENT_BINARY_DIR}")
add_test(NAME bow_pull63 COMMAND bow_pull63_test)

# C63: actual original-first renderer bounds decision with explicit native leaves.
execute_process(COMMAND "${Python3_EXECUTABLE}" "${CMAKE_CURRENT_SOURCE_DIR}/../tools/extract_render_bounds63.py" COMMAND_ERROR_IS_FATAL ANY)
add_executable(render_bounds63_adapter_test render_bounds63_adapter_test.cpp)
add_test(NAME render_bounds63_adapter COMMAND render_bounds63_adapter_test)

add_executable(render_bounds63_lifecycle_test render_bounds63_lifecycle_test.cpp)
target_include_directories(render_bounds63_lifecycle_test PRIVATE syntax_shims)
add_test(NAME render_bounds63_lifecycle COMMAND render_bounds63_lifecycle_test)
add_test(NAME render_bounds63_disable_fail COMMAND render_bounds63_lifecycle_test fail-disable)
add_test(NAME render_bounds63_enable_fail COMMAND render_bounds63_lifecycle_test fail-enable)

# C63R2 actual native-ammo presentation and current nock-helper consumer.
execute_process(COMMAND "${Python3_EXECUTABLE}" "${CMAKE_CURRENT_SOURCE_DIR}/../tools/extract_manual_arrow64.py" COMMAND_ERROR_IS_FATAL ANY)
add_executable(manual_arrow64_test manual_arrow64_test.cpp)
target_compile_features(manual_arrow64_test PRIVATE cxx_std_20)
add_test(NAME manual_arrow64 COMMAND manual_arrow64_test)
# C66: inverse authored rigid door-leaf path, no native calls or animation writes.
add_executable(door_path66_test door_path66_test.cpp)
add_test(NAME door_path66 COMMAND door_path66_test)
add_executable(door_curve_native66_test door_curve_native66_test.cpp)
add_test(NAME door_curve_native66 COMMAND door_curve_native66_test)
execute_process(COMMAND "${Python3_EXECUTABLE}" "${CMAKE_CURRENT_SOURCE_DIR}/../tools/extract_door_drive66.py"
 "${CMAKE_CURRENT_BINARY_DIR}/door_drive66.inc" COMMAND_ERROR_IS_FATAL ANY)
add_executable(door_drive66_test door_drive66_test.cpp)
target_include_directories(door_drive66_test PRIVATE "${CMAKE_CURRENT_BINARY_DIR}")
add_test(NAME door_drive66 COMMAND door_drive66_test)

# C66: actual controller-time owner and exact native root-correction adapter.
add_executable(native_door_motion66_test native_door_motion66_test.cpp)
add_test(NAME native_door_motion66 COMMAND native_door_motion66_test)
add_executable(door_motion_native66_test door_motion_native66_test.cpp)
target_include_directories(door_motion_native66_test PRIVATE syntax_shims ../src)
add_test(NAME door_motion_native66 COMMAND door_motion_native66_test)
add_test(NAME door_motion_native66_failed_enable COMMAND door_motion_native66_test failed-enable)
add_executable(door_channel66_test door_channel66_test.cpp)
add_test(NAME door_channel66 COMMAND door_channel66_test)
add_executable(native_door_fingers66_test native_door_fingers66_test.cpp)
add_test(NAME native_door_fingers66 COMMAND native_door_fingers66_test)

# M1: static lifecycle contract, actual extracted Door service and source patch boundaries.
add_executable(subsystem_contract_test subsystem_contract_test.cpp ../src/subsystems/subsystem.cpp)
add_test(NAME subsystem_contract COMMAND subsystem_contract_test)
add_executable(doors_service_m1_test doors_service_m1_test.cpp ../src/subsystems/doors/module.cpp)
add_test(NAME doors_service_m1 COMMAND doors_service_m1_test)
add_test(NAME subsystem_patch COMMAND "${Python3_EXECUTABLE}" "${CMAKE_CURRENT_SOURCE_DIR}/test_subsystem_patch.py")

add_executable(subsystem_catalog_test subsystem_catalog_test.cpp ../src/subsystems/subsystem.cpp ../src/subsystems/catalog.cpp
 ../src/subsystems/hybrid/module.cpp
 ../src/subsystems/direct/module.cpp
 ../src/subsystems/masterstrike_combos/module.cpp
 ../src/subsystems/physics/module.cpp
 ../src/subsystems/interactions/module.cpp
 ../src/subsystems/alchemy/module.cpp
 ../src/subsystems/smithing/module.cpp
 ../src/subsystems/doors/module.cpp
 ../src/subsystems/dialogues/module.cpp
 ../src/subsystems/hud_ui_controls/module.cpp
 ../src/subsystems/vrik/module.cpp
 ../src/subsystems/vrmap/module.cpp
 ../src/subsystems/gore/module.cpp
 ../src/subsystems/throw_drop/module.cpp
 ../src/subsystems/ranged_reload/module.cpp
 ../src/subsystems/experimental/module.cpp
)
add_test(NAME subsystem_catalog COMMAND subsystem_catalog_test)

add_test(NAME subsystem_sources COMMAND "${Python3_EXECUTABLE}" "${CMAKE_CURRENT_SOURCE_DIR}/../tools/check_subsystems.py")

# M1: execute actual immutable XR pointer/display math with explicit tracking leaves.
execute_process(COMMAND "${Python3_EXECUTABLE}" "${CMAKE_CURRENT_SOURCE_DIR}/../tools/extract_ui_laser_m1.py" COMMAND_ERROR_IS_FATAL ANY)
add_executable(ui_laser_frame_m1_test ui_laser_frame_m1_test.cpp)
target_include_directories(ui_laser_frame_m1_test PRIVATE ui175/mocks syntax_shims openxr_sdk)
add_test(NAME ui_laser_frame_m1 COMMAND ui_laser_frame_m1_test)

add_executable(forge_camera_m1_bridge_test forge_camera_m1_bridge_test.cpp)
target_include_directories(forge_camera_m1_bridge_test PRIVATE syntax_shims openxr_sdk)
if(MSVC)
 target_compile_definitions(forge_camera_m1_bridge_test PRIVATE _ReturnAddress=HostReturnAddress)
 target_compile_options(forge_camera_m1_bridge_test PRIVATE /Gy /Gw /bigobj)
 target_link_options(forge_camera_m1_bridge_test PRIVATE /OPT:REF /INCREMENTAL:NO)
endif()
add_test(NAME forge_camera_m1_bridge COMMAND forge_camera_m1_bridge_test)

execute_process(COMMAND "${Python3_EXECUTABLE}" "${CMAKE_CURRENT_SOURCE_DIR}/../tools/extract_xr_b_valid_m1.py"
 "${CMAKE_CURRENT_BINARY_DIR}/xr_b_valid_m1.inc" COMMAND_ERROR_IS_FATAL ANY)
add_executable(xr_b_valid_m1_test xr_b_valid_m1_test.cpp)
target_include_directories(xr_b_valid_m1_test PRIVATE "${CMAKE_CURRENT_BINARY_DIR}")
add_test(NAME xr_b_valid_m1 COMMAND xr_b_valid_m1_test)

# G1: exact crafting intent, source arbitration and native tool representations.
foreach(g1 near_selection67 gesture_work67 input67 bridge67 native_return67 modes67)
 add_executable(g1_${g1}_test subsystems/alchemy/${g1}_test.cpp)
 target_include_directories(g1_${g1}_test PRIVATE ../src)
 add_test(NAME g1_${g1} COMMAND g1_${g1}_test)
endforeach()
execute_process(COMMAND "${Python3_EXECUTABLE}" "${CMAKE_CURRENT_SOURCE_DIR}/subsystems/smithing/generate_native_pins_fixture.py"
 --out "${CMAKE_CURRENT_BINARY_DIR}" COMMAND_ERROR_IS_FATAL ANY)
add_executable(g1_smith_representation_test subsystems/smithing/representation_g1_test.cpp)
target_include_directories(g1_smith_representation_test PRIVATE ../src "${CMAKE_CURRENT_BINARY_DIR}")
add_test(NAME g1_smith_representation COMMAND g1_smith_representation_test)
foreach(g1 gesture_grip67 mode_clock67 bound_tools67)
 add_executable(g1_${g1}_test subsystems/smithing/${g1}_test.cpp)
 target_include_directories(g1_${g1}_test PRIVATE ../src)
 add_test(NAME g1_${g1} COMMAND g1_${g1}_test)
endforeach()
add_executable(g1_native_mode_clock67_test subsystems/smithing/native_mode_clock67_test.cpp)
target_include_directories(g1_native_mode_clock67_test PRIVATE syntax_shims ../src)
target_link_libraries(g1_native_mode_clock67_test PRIVATE Threads::Threads)
if(MSVC)
 target_compile_options(g1_native_mode_clock67_test PRIVATE /Gy /Gw /bigobj)
 target_link_options(g1_native_mode_clock67_test PRIVATE /OPT:REF /INCREMENTAL:NO)
endif()
add_test(NAME g1_native_mode_clock67 COMMAND g1_native_mode_clock67_test)
add_executable(g1_native_return67_connected_test subsystems/alchemy/native_return67_connected_test.cpp)
target_include_directories(g1_native_return67_connected_test PRIVATE syntax_shims ../src)
target_link_libraries(g1_native_return67_connected_test PRIVATE Threads::Threads)
if(MSVC)
 target_compile_options(g1_native_return67_connected_test PRIVATE /Gy /Gw /bigobj)
 target_link_options(g1_native_return67_connected_test PRIVATE /OPT:REF /INCREMENTAL:NO)
endif()
add_test(NAME g1_native_return67_connected COMMAND g1_native_return67_connected_test)

# G1R1: real native-bounds arbitration and the actual scoped retail wrappers.
add_executable(g1r1_geometry68_test subsystems/alchemy/geometry68_test.cpp)
add_test(NAME g1r1_geometry68 COMMAND g1r1_geometry68_test)
add_executable(g1r1_native_table68_test subsystems/alchemy/native_table68_test.cpp)
add_test(NAME g1r1_native_table68 COMMAND g1r1_native_table68_test)
if(MSVC)
 add_executable(g1r1_native_selection68_test subsystems/alchemy/native_selection68_test.cpp)
 target_compile_options(g1r1_native_selection68_test PRIVATE /Gy /Gw /bigobj)
 target_link_options(g1r1_native_selection68_test PRIVATE /OPT:REF /INCREMENTAL:NO)
 add_test(NAME g1r1_native_selection68 COMMAND g1r1_native_selection68_test)
 add_executable(g1r1_native_smith_contact68_test subsystems/smithing/native_contact68_test.cpp)
 target_compile_options(g1r1_native_smith_contact68_test PRIVATE /Gy /Gw /bigobj)
 target_link_options(g1r1_native_smith_contact68_test PRIVATE /OPT:REF /INCREMENTAL:NO)
 add_test(NAME g1r1_native_smith_contact68 COMMAND g1r1_native_smith_contact68_test)
endif()

# G2: production mortar input and native Stop/retained-pair handoff.
foreach(g2 mortar_input69 native_mortar69)
 add_executable(g2_${g2}_test subsystems/alchemy/${g2}_test.cpp)
 target_include_directories(g2_${g2}_test PRIVATE ../src)
 add_test(NAME g2_${g2} COMMAND g2_${g2}_test)
endforeach()

add_test(NAME source_text_m2 COMMAND "${Python3_EXECUTABLE}" "${CMAKE_CURRENT_SOURCE_DIR}/test_source_text_m2.py")
add_test(NAME composition_generator_m2 COMMAND "${Python3_EXECUTABLE}" "${CMAKE_CURRENT_SOURCE_DIR}/test_subsystem_composition_generator_m2.py")
execute_process(COMMAND "${Python3_EXECUTABLE}" "${CMAKE_CURRENT_SOURCE_DIR}/../tools/extract_bow_basis_m2.py" COMMAND_ERROR_IS_FATAL ANY)
add_executable(bow_basis_m2_test bow_basis_m2_test.cpp)
add_test(NAME bow_basis_m2 COMMAND bow_basis_m2_test)

# B2 actual named wait card wrapper and generation checks.
execute_process(COMMAND "${Python3_EXECUTABLE}" "${CMAKE_CURRENT_SOURCE_DIR}/../tools/extract_wait_overlay_b2.py"
 "${CMAKE_CURRENT_BINARY_DIR}/wait_runtime_b2.inc" COMMAND_ERROR_IS_FATAL ANY)
add_executable(wait_runtime_b2_test wait_runtime_b2_test.cpp)
target_include_directories(wait_runtime_b2_test PRIVATE "${CMAKE_CURRENT_BINARY_DIR}")
add_test(NAME wait_runtime_b2 COMMAND wait_runtime_b2_test)

# G3 reviewed mortar overlay; existing B2 wait target retained.
foreach(g3 mortar_pour70 native_pour70)
 add_executable(g3_${g3}_test subsystems/alchemy/${g3}_test.cpp)
 target_include_directories(g3_${g3}_test PRIVATE ../src)
 if(MSVC)
  target_compile_options(g3_${g3}_test PRIVATE /Gy /Gw /bigobj)
  target_link_options(g3_${g3}_test PRIVATE /OPT:REF /INCREMENTAL:NO)
 endif()
 add_test(NAME g3_${g3} COMMAND g3_${g3}_test)
endforeach()
target_include_directories(g3_native_pour70_test PRIVATE syntax_shims)
target_link_libraries(g3_native_pour70_test PRIVATE Threads::Threads)
add_executable(g3_binding70_test subsystems/alchemy/binding70_test.cpp)
target_include_directories(g3_binding70_test PRIVATE ../src)
add_test(NAME g3_binding70 COMMAND g3_binding70_test)

# T7 bounded native-player inventory startup catch-up.
add_executable(inventory_delivery_t7_test inventory_delivery_t7_test.cpp)
target_compile_features(inventory_delivery_t7_test PRIVATE cxx_std_20)
add_test(NAME inventory_delivery_t7 COMMAND inventory_delivery_t7_test)
# B3 separates tracked attachment presentation from native alchemy work eligibility.
add_executable(b3_presentation71_test subsystems/alchemy/presentation71_test.cpp)
target_include_directories(b3_presentation71_test PRIVATE ../src)
add_test(NAME b3_presentation71 COMMAND b3_presentation71_test)

add_executable(b3_elbow_endpoint_test elbow_endpoint_b3_test.cpp)
target_include_directories(b3_elbow_endpoint_test PRIVATE ../src "${CMAKE_CURRENT_BINARY_DIR}")
if(MSVC)
 target_compile_options(b3_elbow_endpoint_test PRIVATE /Gy /Gw /bigobj)
 target_link_options(b3_elbow_endpoint_test PRIVATE /OPT:REF /INCREMENTAL:NO)
endif()
add_test(NAME b3_elbow_endpoint COMMAND b3_elbow_endpoint_test)

add_executable(elbow_conditioning_h15_test elbow_conditioning_h15_test.cpp)
target_include_directories(elbow_conditioning_h15_test PRIVATE ../src "${CMAKE_CURRENT_BINARY_DIR}")
if(MSVC)
 target_compile_options(elbow_conditioning_h15_test PRIVATE /Gy /Gw /bigobj)
 target_link_options(elbow_conditioning_h15_test PRIVATE /OPT:REF /INCREMENTAL:NO)
endif()
add_test(NAME elbow_conditioning_h15 COMMAND elbow_conditioning_h15_test)

add_executable(b3_hybrid_preparation_cost_test hybrid_preparation_cost_b3_test.cpp)
target_include_directories(b3_hybrid_preparation_cost_test PRIVATE ../src)
target_link_libraries(b3_hybrid_preparation_cost_test PRIVATE Threads::Threads)
add_test(NAME b3_hybrid_preparation_cost COMMAND b3_hybrid_preparation_cost_test)

# B4: separate processes prove terminal install failures without test-only reset.
if(MSVC) # the actual Windows-native adapter fixture exists only here
foreach(case register-refusal phase-refusal fault-refusal clearance-preflight phase-preflight)
 add_test(NAME native_weapon_install_b4_${case} COMMAND native_weapon_body_t4_test ${case})
endforeach()
endif()

# A72: deliberate input selects a source; only proved pre-call refusal may rearm.
foreach(a72 selection_intent72 mortar_retry72)
 add_executable(alchemy_${a72}_test subsystems/alchemy/${a72}_test.cpp)
 target_include_directories(alchemy_${a72}_test PRIVATE ../src)
 add_test(NAME alchemy_${a72} COMMAND alchemy_${a72}_test)
endforeach()

# S1: actual native activity parent fitter with substituted Flash leaves.
add_executable(activity_overlay_s1_test subsystems/hud_ui_controls/activity_overlay_s1_test.cpp)
add_test(NAME activity_overlay_s1 COMMAND activity_overlay_s1_test)

add_executable(impale_observe_h7_test impale_observe_h7_test.cpp)
target_compile_features(impale_observe_h7_test PRIVATE cxx_std_17)
add_test(NAME impale_observe_h7 COMMAND impale_observe_h7_test)

add_executable(corpse_contact_h7_test corpse_contact_h7_test.cpp)
add_test(NAME corpse_contact_h7 COMMAND corpse_contact_h7_test)

if(MSVC)
 execute_process(COMMAND "${Python3_EXECUTABLE}" "${CMAKE_CURRENT_SOURCE_DIR}/../tools/extract_corpse_contact_h7.py" "${CMAKE_CURRENT_BINARY_DIR}/corpse_contact_native_h7.inc" COMMAND_ERROR_IS_FATAL ANY)
 add_executable(corpse_contact_native_h7_test corpse_contact_native_h7_test.cpp)
 target_compile_features(corpse_contact_native_h7_test PRIVATE cxx_std_20)
 target_include_directories(corpse_contact_native_h7_test PRIVATE "${CMAKE_CURRENT_BINARY_DIR}")
 add_test(NAME corpse_contact_native_h7 COMMAND corpse_contact_native_h7_test)
endif()

# H8 actual Claude-options menu plus owned state/apply/save/load contributions.
add_executable(claude_menu_h8_test claude_menu_h8_test.cpp)
add_test(NAME claude_menu_h8 COMMAND claude_menu_h8_test)

if(MSVC)
 execute_process(COMMAND "${Python3_EXECUTABLE}" "${CMAKE_CURRENT_SOURCE_DIR}/../tools/extract_impale_probe_h8.py" "${CMAKE_CURRENT_BINARY_DIR}/impale_probe_h8.inc" COMMAND_ERROR_IS_FATAL ANY)
 add_executable(impale_probe_h8_test impale_probe_h8_test.cpp)
 target_compile_features(impale_probe_h8_test PRIVATE cxx_std_20)
 target_include_directories(impale_probe_h8_test PRIVATE "${CMAKE_CURRENT_BINARY_DIR}")
 add_test(NAME impale_probe_h8 COMMAND impale_probe_h8_test)
endif()

# H12 actual native start -> ordinary receiver -> ProcessHit fist ancestry.
if(MSVC)
 add_executable(fist_causal_h12_bridge fist_causal_h12_bridge_test.cpp bridge_external_stubs112.cpp ../src/subsystems/doors/module.cpp)
 target_include_directories(fist_causal_h12_bridge PRIVATE syntax_shims openxr_sdk)
 target_compile_definitions(fist_causal_h12_bridge PRIVATE _ReturnAddress=HostReturnAddress BRIDGE_HAS_COMBAT_DIAG_IMPL)
 target_compile_options(fist_causal_h12_bridge PRIVATE /Gy /Gw /bigobj)
 target_link_options(fist_causal_h12_bridge PRIVATE /OPT:REF /INCREMENTAL:NO)
 target_link_libraries(fist_causal_h12_bridge PRIVATE Threads::Threads)
 add_test(NAME fist_causal_h12_bridge COMMAND fist_causal_h12_bridge)
 add_executable(fist_prediction_h31_bridge fist_prediction_h31_bridge_test.cpp bridge_external_stubs112.cpp ../src/subsystems/doors/module.cpp)
 target_include_directories(fist_prediction_h31_bridge PRIVATE syntax_shims openxr_sdk)
 target_compile_definitions(fist_prediction_h31_bridge PRIVATE _ReturnAddress=HostReturnAddress BRIDGE_HAS_COMBAT_DIAG_IMPL)
 target_compile_options(fist_prediction_h31_bridge PRIVATE /Gy /Gw /bigobj)
 target_link_options(fist_prediction_h31_bridge PRIVATE /OPT:REF /INCREMENTAL:NO)
 target_link_libraries(fist_prediction_h31_bridge PRIVATE Threads::Threads)
 add_test(NAME fist_prediction_h31_bridge COMMAND fist_prediction_h31_bridge)
 execute_process(COMMAND "${Python3_EXECUTABLE}" "${CMAKE_CURRENT_SOURCE_DIR}/../tools/extract_public_hybrid_h31.py" COMMAND_ERROR_IS_FATAL ANY)
 add_executable(public_hybrid_h31_bridge public_hybrid_h31_bridge_test.cpp bridge_external_stubs112.cpp ../src/subsystems/doors/module.cpp)
 target_include_directories(public_hybrid_h31_bridge PRIVATE syntax_shims openxr_sdk)
 target_compile_definitions(public_hybrid_h31_bridge PRIVATE _ReturnAddress=HostReturnAddress BRIDGE_HAS_MODE_STUBS BRIDGE_VERIFIED_ACTIVATION_H24)
 target_compile_options(public_hybrid_h31_bridge PRIVATE /Gy /Gw /bigobj)
 target_link_options(public_hybrid_h31_bridge PRIVATE /OPT:REF /INCREMENTAL:NO)
 target_link_libraries(public_hybrid_h31_bridge PRIVATE Threads::Threads)
 add_test(NAME public_hybrid_h31_bridge COMMAND public_hybrid_h31_bridge)
endif()

# H18: delayed death-body admission with a NEW current checked blade contact.
add_executable(impale_successor_h18_test impale_successor_h18_test.cpp)
add_test(NAME impale_successor_h18 COMMAND impale_successor_h18_test)
add_executable(impale_successor_h18_coordinator_test impale_successor_h18_coordinator_test.cpp)
add_test(NAME impale_successor_h18_coordinator COMMAND impale_successor_h18_coordinator_test)
if(MSVC)
 execute_process(COMMAND "${Python3_EXECUTABLE}" "${CMAKE_CURRENT_SOURCE_DIR}/../tools/extract_impale_successor_h18.py" "${CMAKE_CURRENT_BINARY_DIR}/impale_successor_h18_native.inc" COMMAND_ERROR_IS_FATAL ANY)
 add_executable(impale_successor_h18_native_test impale_successor_h18_native_test.cpp)
 target_compile_features(impale_successor_h18_native_test PRIVATE cxx_std_20)
 target_include_directories(impale_successor_h18_native_test PRIVATE "${CMAKE_CURRENT_BINARY_DIR}")
 add_test(NAME impale_successor_h18_native COMMAND impale_successor_h18_native_test)
endif()














if(WIN32)
 add_executable(startup_setup_s15_test startup_setup_s15_test.cpp)
 target_compile_features(startup_setup_s15_test PRIVATE cxx_std_23)
 target_link_libraries(startup_setup_s15_test PRIVATE user32 gdi32 shell32)
 add_test(NAME startup_setup_s15 COMMAND startup_setup_s15_test)
endif()

if(MSVC)
 execute_process(COMMAND "${Python3_EXECUTABLE}" "${CMAKE_CURRENT_SOURCE_DIR}/../tools/extract_master_eligibility_h21.py" "${CMAKE_CURRENT_BINARY_DIR}/master_eligibility_h21_actual.inc" COMMAND_ERROR_IS_FATAL ANY)
 add_executable(master_eligibility_h21_actual_test master_eligibility_h21_actual_test.cpp)
 target_include_directories(master_eligibility_h21_actual_test PRIVATE "${CMAKE_CURRENT_BINARY_DIR}")
 add_test(NAME master_eligibility_h21_actual COMMAND master_eligibility_h21_actual_test)
 add_executable(master_arm_h21_test master_arm_h21_test.cpp)
 foreach(mode factory inflight observe allocation)
  add_test(NAME master_arm_h21_${mode} COMMAND master_arm_h21_test ${mode})
 endforeach()
 foreach(site RANGE 0 10)
  foreach(committed RANGE 0 1)
   add_test(NAME master_arm_h21_enable_${site}_${committed} COMMAND master_arm_h21_test enable ${site} ${committed})
   add_test(NAME master_arm_h21_disable_${site}_${committed} COMMAND master_arm_h21_test disable ${site} ${committed})
  endforeach()
 endforeach()
endif()

# H21: compile exact native access templates against packed and aligned storage.
execute_process(COMMAND "${Python3_EXECUTABLE}" "${CMAKE_CURRENT_SOURCE_DIR}/../tools/extract_scalar_access_h21.py"
 RESULT_VARIABLE SCALAR_ACCESS_EXTRACT_H21)
if(NOT SCALAR_ACCESS_EXTRACT_H21 EQUAL 0)
 message(FATAL_ERROR "Could not extract production scalar access helpers")
endif()
add_executable(scalar_access_h21_test scalar_access_h21_test.cpp)
target_compile_features(scalar_access_h21_test PRIVATE cxx_std_17)
add_test(NAME scalar_access_h21 COMMAND scalar_access_h21_test)

# H21 held mortar destination aim; native selection/operation/effects stay owned.
add_executable(g1_mortar_aim_h21_test subsystems/alchemy/mortar_aim_h21_test.cpp)
add_test(NAME g1_mortar_aim_h21 COMMAND g1_mortar_aim_h21_test)

add_executable(guard_geometry_h24_test guard_geometry_h24_test.cpp)
add_test(NAME guard_geometry_h24 COMMAND guard_geometry_h24_test)

add_executable(block_cooldown_h24_test block_cooldown_h24_test.cpp)
add_test(NAME block_cooldown_h24 COMMAND block_cooldown_h24_test)

if(WIN32)
 add_executable(block_indicator_h24_test block_indicator_h24_test.cpp)
 target_link_libraries(block_indicator_h24_test PRIVATE user32 gdi32)
 target_compile_definitions(block_indicator_h24_test PRIVATE WIN32_LEAN_AND_MEAN NOMINMAX)
 add_test(NAME block_indicator_h24 COMMAND block_indicator_h24_test)
endif()

execute_process(COMMAND "${Python3_EXECUTABLE}" "${CMAKE_CURRENT_SOURCE_DIR}/../tools/extract_collision_event_h24.py" RESULT_VARIABLE COLLISION_EXTRACT_H24)
if(NOT COLLISION_EXTRACT_H24 EQUAL 0)
 message(FATAL_ERROR "Could not extract actual PhysCollisionDetour")
endif()
add_executable(native_weapon_contact_h24_test native_weapon_contact_h24_test.cpp)
add_test(NAME native_weapon_contact_h24 COMMAND native_weapon_contact_h24_test)

add_executable(native_weapon_parry_join_h24_test native_weapon_parry_join_h24_test.cpp)
add_test(NAME native_weapon_parry_join_h24 COMMAND native_weapon_parry_join_h24_test)
add_executable(native_held_intent_h28_test native_held_intent_h28_test.cpp)
target_include_directories(native_held_intent_h28_test PRIVATE ../src)
add_test(NAME native_held_intent_h28 COMMAND native_held_intent_h28_test)
add_executable(native_weapon_actor_join_h28_test native_weapon_actor_join_h28_test.cpp)
target_include_directories(native_weapon_actor_join_h28_test PRIVATE ../src)
add_test(NAME native_weapon_actor_join_h28 COMMAND native_weapon_actor_join_h28_test)

# This bridge supplies its own explicit H24 presence leaves.
target_compile_definitions(fist_bridge PRIVATE BRIDGE_OWNS_PRESENCE_H24)

foreach(h29 stroke_rebase_h29 fist_qualification_h29 vrik_trace_h29 door_path_c54r3 hit_chain_c54r3 punch_path_c54r3 diagnostic_health_c54r3)
 add_executable(${h29}_test ${h29}_test.cpp)
 target_include_directories(${h29}_test PRIVATE ../src)
 add_test(NAME ${h29} COMMAND ${h29}_test)
endforeach()
add_executable(finger_middle_c54_bridge finger_middle_c54_bridge.cpp bridge_external_stubs112.cpp ../src/subsystems/doors/module.cpp)
target_include_directories(finger_middle_c54_bridge PRIVATE syntax_shims openxr_sdk)
target_compile_definitions(finger_middle_c54_bridge PRIVATE _ReturnAddress=HostReturnAddress)
target_compile_options(finger_middle_c54_bridge PRIVATE /Gy /Gw /bigobj)
target_link_options(finger_middle_c54_bridge PRIVATE /OPT:REF /INCREMENTAL:NO)
add_test(NAME finger_middle_c54 COMMAND finger_middle_c54_bridge)
add_executable(finger_translation_h31_bridge finger_translation_h31_bridge.cpp bridge_external_stubs112.cpp ../src/subsystems/doors/module.cpp)
target_include_directories(finger_translation_h31_bridge PRIVATE syntax_shims openxr_sdk)
target_compile_definitions(finger_translation_h31_bridge PRIVATE _ReturnAddress=HostReturnAddress)
target_compile_options(finger_translation_h31_bridge PRIVATE /Gy /Gw /bigobj)
target_link_options(finger_translation_h31_bridge PRIVATE /OPT:REF /INCREMENTAL:NO)
add_test(NAME finger_translation_h31 COMMAND finger_translation_h31_bridge)
set_property(DIRECTORY APPEND PROPERTY CMAKE_CONFIGURE_DEPENDS ../src/vrik/coherent_pose.inl ../tools/extract_finger_contact_h31.py)
execute_process(COMMAND "${Python3_EXECUTABLE}" "${CMAKE_CURRENT_SOURCE_DIR}/../tools/extract_finger_contact_h31.py" "${CMAKE_CURRENT_BINARY_DIR}" COMMAND_ERROR_IS_FATAL ANY)
add_executable(finger_contact_h31_bridge finger_contact_h31_bridge.cpp finger_contact_h31_world.cpp bridge_external_stubs112.cpp ../src/subsystems/doors/module.cpp)
target_include_directories(finger_contact_h31_bridge PRIVATE syntax_shims openxr_sdk "${CMAKE_CURRENT_BINARY_DIR}")
target_compile_definitions(finger_contact_h31_bridge PRIVATE _ReturnAddress=HostReturnAddress)
target_compile_options(finger_contact_h31_bridge PRIVATE /Gy /Gw /bigobj)
target_link_options(finger_contact_h31_bridge PRIVATE /OPT:REF /INCREMENTAL:NO)
add_test(NAME finger_contact_h31 COMMAND finger_contact_h31_bridge)


target_sources(diagnostic_health_c54r3_test PRIVATE ../src/diagnostic_trace.cpp)
include(diagnostic_logger_d81.cmake)
add_executable(h29_settings_test h29_settings_test.cpp)
add_test(NAME h29_settings COMMAND h29_settings_test)
add_executable(solver_combat_h29_test solver_combat_h29_test.cpp)
target_include_directories(solver_combat_h29_test PRIVATE ../src)
add_test(NAME solver_combat_h29 COMMAND solver_combat_h29_test)

# Current coherent publication -> verbatim native bounds decision.
execute_process(COMMAND "${Python3_EXECUTABLE}" "${CMAKE_CURRENT_SOURCE_DIR}/../tools/extract_hand_render_owner_h5.py"
 "${CMAKE_CURRENT_BINARY_DIR}/hand_render_owner_h5" COMMAND_ERROR_IS_FATAL ANY)
set_property(DIRECTORY APPEND PROPERTY CMAKE_CONFIGURE_DEPENDS
 ../src/vrik/coherent_pose.inl ../src/vrik/render_bounds63.inl ../src/vrik/render_bounds63.h
 ../src/vrik/tracking_grace111.h pose_adapter_test.cpp ../tools/extract_hand_render_owner_h5.py)
add_executable(hand_render_owner_h5_test hand_render_owner_h5_test.cpp)
target_include_directories(hand_render_owner_h5_test PRIVATE "${CMAKE_CURRENT_BINARY_DIR}/hand_render_owner_h5")
if(MSVC)
 target_compile_options(hand_render_owner_h5_test PRIVATE /Od /Gy /Gw /bigobj)
 target_link_options(hand_render_owner_h5_test PRIVATE /OPT:REF /INCREMENTAL:NO /STACK:8388608)
endif()
add_test(NAME hand_render_owner_h5 COMMAND hand_render_owner_h5_test)

# F5/F7 observers include the actual current coherent/door callback owners.
execute_process(COMMAND "${Python3_EXECUTABLE}" "${CMAKE_CURRENT_SOURCE_DIR}/../tools/extract_gesture_quarantine_h8.py"
 "${CMAKE_CURRENT_BINARY_DIR}/gesture_quarantine_h8" COMMAND_ERROR_IS_FATAL ANY)
set_property(DIRECTORY APPEND PROPERTY CMAKE_CONFIGURE_DEPENDS
 ../src/subsystems/hybrid/drain_cost_claim.inl ../tools/extract_gesture_quarantine_h8.py ../tools/source_text.py)
add_executable(gesture_quarantine_h8_bridge gesture_quarantine_h8_bridge_test.cpp bridge_external_stubs112.cpp ../src/subsystems/doors/module.cpp)
target_include_directories(gesture_quarantine_h8_bridge PRIVATE syntax_shims openxr_sdk "${CMAKE_CURRENT_BINARY_DIR}/gesture_quarantine_h8")
target_compile_definitions(gesture_quarantine_h8_bridge PRIVATE _ReturnAddress=HostReturnAddress BRIDGE_HAS_MODE_STUBS BRIDGE_VERIFIED_ACTIVATION_H24)
target_link_libraries(gesture_quarantine_h8_bridge PRIVATE Threads::Threads)
if(MSVC)
 target_compile_options(gesture_quarantine_h8_bridge PRIVATE /Gy /Gw /bigobj)
 target_link_options(gesture_quarantine_h8_bridge PRIVATE /OPT:REF /INCREMENTAL:NO /STACK:8388608)
endif()
add_test(NAME gesture_quarantine_h8 COMMAND gesture_quarantine_h8_bridge)

execute_process(COMMAND "${Python3_EXECUTABLE}" "${CMAKE_CURRENT_SOURCE_DIR}/../tools/extract_vrik_callback_h29r4.py"
 "${CMAKE_CURRENT_BINARY_DIR}/vrik_callback_h29r4" COMMAND_ERROR_IS_FATAL ANY)
set_property(DIRECTORY APPEND PROPERTY CMAKE_CONFIGURE_DEPENDS
 ../src/frik_pose_probe.cpp ../tools/extract_vrik_callback_h29r4.py ../tools/source_text.py)
foreach(f57 interaction_cycle_h4 vrik_cycle_h29r4 vrik_render_h29r4 vrik_account_h6 vrik_cycle_h6)
 add_executable(${f57}_test ${f57}_test.cpp)
 target_compile_features(${f57}_test PRIVATE cxx_std_23)
 target_include_directories(${f57}_test PRIVATE syntax_shims openxr_sdk)
 if(MSVC)
  target_compile_options(${f57}_test PRIVATE /Od /Gy /Gw /bigobj)
  target_link_options(${f57}_test PRIVATE /OPT:REF /INCREMENTAL:NO /STACK:33554432)
 endif()
 add_test(NAME ${f57} COMMAND ${f57}_test)
endforeach()
foreach(f57 interaction_native_h4 vrik_callback_h29r4)
 add_executable(${f57}_bridge ${f57}_bridge.cpp bridge_external_stubs112.cpp ../src/subsystems/doors/module.cpp)
 target_compile_features(${f57}_bridge PRIVATE cxx_std_23)
 target_include_directories(${f57}_bridge PRIVATE syntax_shims openxr_sdk "${CMAKE_CURRENT_BINARY_DIR}/vrik_callback_h29r4")
 target_compile_definitions(${f57}_bridge PRIVATE _ReturnAddress=HostReturnAddress)
 target_link_libraries(${f57}_bridge PRIVATE Threads::Threads)
 if(MSVC)
  target_compile_options(${f57}_bridge PRIVATE /Od /Gy /Gw /bigobj)
  target_link_options(${f57}_bridge PRIVATE /OPT:REF /INCREMENTAL:NO /STACK:33554432)
 endif()
 add_test(NAME ${f57} COMMAND ${f57}_bridge)
endforeach()

# Repair3 experiment integration: actual production owners, native leaves explicit.
set_property(DIRECTORY APPEND PROPERTY CMAKE_CONFIGURE_DEPENDS
 ../src/vrik/fist_native.inl ../src/vrik/native_combat104.inl ../tools/extract_face_guard_h6.py)
execute_process(COMMAND "${Python3_EXECUTABLE}" "${CMAKE_CURRENT_SOURCE_DIR}/../tools/extract_face_guard_h6.py" --out "${CMAKE_CURRENT_BINARY_DIR}" COMMAND_ERROR_IS_FATAL ANY)
foreach(name door_ray_owner_h5 pickup_handoff_h5 door_entry_mode_h5 face_guard_h6 face_guard_native_h6)
 add_executable(${name}_test ${name}_test.cpp)
 target_include_directories(${name}_test PRIVATE syntax_shims ../src "${CMAKE_CURRENT_BINARY_DIR}")
 target_link_libraries(${name}_test PRIVATE Threads::Threads)
 target_compile_features(${name}_test PRIVATE cxx_std_20)
 if(MSVC)
  target_compile_options(${name}_test PRIVATE /Gy /Gw /bigobj)
  target_link_options(${name}_test PRIVATE /OPT:REF /STACK:8388608)
 endif()
 add_test(NAME ${name} COMMAND ${name}_test)
 set_tests_properties(${name} PROPERTIES TIMEOUT 30)
endforeach()
target_compile_definitions(door_ray_owner_h5_test PRIVATE _ReturnAddress=HostReturnAddress)
add_executable(alchemy_acquisition_owner_h5_test subsystems/alchemy/acquisition_owner_h5_test.cpp)
target_include_directories(alchemy_acquisition_owner_h5_test PRIVATE syntax_shims ../src)
target_compile_features(alchemy_acquisition_owner_h5_test PRIVATE cxx_std_20)
if(MSVC)
 target_compile_options(alchemy_acquisition_owner_h5_test PRIVATE /Gy /Gw /bigobj)
 target_link_options(alchemy_acquisition_owner_h5_test PRIVATE /OPT:REF /STACK:8388608)
endif()
add_test(NAME alchemy_acquisition_owner_h5 COMMAND alchemy_acquisition_owner_h5_test)

execute_process(COMMAND "${Python3_EXECUTABLE}" "${CMAKE_CURRENT_SOURCE_DIR}/../tools/extract_door_ray_owner_h5.py" COMMAND_ERROR_IS_FATAL ANY)

add_executable(authored_fist_h34_bridge authored_fist_h34_bridge.cpp bridge_external_stubs112.cpp ../src/subsystems/doors/module.cpp)
target_include_directories(authored_fist_h34_bridge PRIVATE syntax_shims openxr_sdk "${CMAKE_CURRENT_BINARY_DIR}/vrik_callback_h29r4")
target_compile_definitions(authored_fist_h34_bridge PRIVATE _ReturnAddress=HostReturnAddress)
target_compile_options(authored_fist_h34_bridge PRIVATE /Gy /Gw /bigobj)
target_link_options(authored_fist_h34_bridge PRIVATE /OPT:REF /INCREMENTAL:NO /STACK:8388608)
add_test(NAME authored_fist_h34 COMMAND authored_fist_h34_bridge)

add_executable(grasp_finger_identity_h34_test grasp_finger_identity_h34_test.cpp)
add_test(NAME grasp_finger_identity_h34 COMMAND grasp_finger_identity_h34_test)
add_executable(grasp_mapping_h34_test grasp_mapping_h34_test.cpp)
target_link_libraries(grasp_mapping_h34_test PRIVATE Threads::Threads)
if(MSVC)
 target_compile_options(grasp_mapping_h34_test PRIVATE /Gy /Gw /bigobj)
 target_link_options(grasp_mapping_h34_test PRIVATE /OPT:REF /INCREMENTAL:NO /STACK:8388608)
else()
 target_compile_options(grasp_mapping_h34_test PRIVATE -ffunction-sections -fdata-sections)
 target_link_options(grasp_mapping_h34_test PRIVATE -Wl,--gc-sections)
endif()
add_test(NAME grasp_mapping_h34 COMMAND grasp_mapping_h34_test)

add_executable(native_clearance_tls_h5_test native_clearance_tls_h5_test.cpp)
target_compile_features(native_clearance_tls_h5_test PRIVATE cxx_std_23)
target_include_directories(native_clearance_tls_h5_test PRIVATE syntax_shims)
target_link_libraries(native_clearance_tls_h5_test PRIVATE Threads::Threads)
add_test(NAME native_clearance_tls_h5 COMMAND native_clearance_tls_h5_test)

# Frozen XR34 native executor and its narrow current Reloading hand passenger.
execute_process(COMMAND "${Python3_EXECUTABLE}" "${CMAKE_CURRENT_SOURCE_DIR}/../tools/extract_crossbow_pose_xr34.py" COMMAND_ERROR_IS_FATAL ANY)
execute_process(COMMAND "${Python3_EXECUTABLE}" "${CMAKE_CURRENT_SOURCE_DIR}/../tools/extract_reload_passenger_xr34.py" COMMAND_ERROR_IS_FATAL ANY)
foreach(name crossbow_native_xr34 crossbow_pose_xr34 reload_working_hand_xr34 reload_passenger_native_xr34 reload_animation_xr34)
 add_executable(${name}_test ${name}_test.cpp)
 target_compile_features(${name}_test PRIVATE cxx_std_20)
 target_include_directories(${name}_test PRIVATE syntax_shims openxr_sdk)
 target_link_libraries(${name}_test PRIVATE Threads::Threads)
 if(MSVC)
  target_compile_options(${name}_test PRIVATE /Gy /Gw /bigobj)
  target_link_options(${name}_test PRIVATE /OPT:REF /INCREMENTAL:NO /STACK:8388608)
 endif()
 add_test(NAME ${name} COMMAND ${name}_test)
endforeach()

add_executable(reload_registration_xr34_test reload_registration_xr34_test.cpp reload_bootstrap_pose_xr34_bridge.cpp)
target_compile_features(reload_registration_xr34_test PRIVATE cxx_std_20)
target_include_directories(reload_registration_xr34_test PRIVATE syntax_shims openxr_sdk)
target_link_libraries(reload_registration_xr34_test PRIVATE Threads::Threads)
if(MSVC)
 target_compile_options(reload_registration_xr34_test PRIVATE /Gy /Gw /bigobj)
 target_link_options(reload_registration_xr34_test PRIVATE /OPT:REF /INCREMENTAL:NO /STACK:8388608)
endif()
add_test(NAME reload_registration_xr34 COMMAND reload_registration_xr34_test)

# Exact retail Start entry ABI is outside the ordinary host-test adapter block.
execute_process(COMMAND "${Python3_EXECUTABLE}" "${CMAKE_CURRENT_SOURCE_DIR}/../tools/extract_reload_start_entry_xr34.py" COMMAND_ERROR_IS_FATAL ANY)
set_property(DIRECTORY APPEND PROPERTY CMAKE_CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/../src/subsystems/ranged_reload/crossbow_native_xr34.inl" "${CMAKE_CURRENT_SOURCE_DIR}/../tools/extract_reload_start_entry_xr34.py")
add_executable(reload_start_entry_xr34_test reload_start_entry_xr34_test.cpp)
target_compile_features(reload_start_entry_xr34_test PRIVATE cxx_std_20)
add_test(NAME reload_start_entry_xr34 COMMAND reload_start_entry_xr34_test)


add_executable(f9_scale_rearm_h5_test f9_scale_rearm_h5_test.cpp)
target_compile_features(f9_scale_rearm_h5_test PRIVATE cxx_std_23)
target_include_directories(f9_scale_rearm_h5_test PRIVATE syntax_shims ../src)
target_link_libraries(f9_scale_rearm_h5_test PRIVATE Threads::Threads)
add_test(NAME f9_scale_rearm_h5 COMMAND f9_scale_rearm_h5_test)

execute_process(COMMAND "${Python3_EXECUTABLE}" "${CMAKE_CURRENT_SOURCE_DIR}/../tools/extract_quick_talk_h4.py" COMMAND_ERROR_IS_FATAL ANY)
foreach(talk quick_talk_arbitration_h4 quick_talk_host_h4)
 add_executable(${talk}_test ${talk}_test.cpp)
 target_include_directories(${talk}_test PRIVATE syntax_shims openxr_sdk)
 target_compile_features(${talk}_test PRIVATE cxx_std_20)
 add_test(NAME ${talk} COMMAND ${talk}_test)
endforeach()
add_executable(quick_talk_combat_h37_test quick_talk_combat_h37_test.cpp)
target_include_directories(quick_talk_combat_h37_test PRIVATE syntax_shims openxr_sdk)
target_compile_features(quick_talk_combat_h37_test PRIVATE cxx_std_20)
foreach(case weapon-charge chooser-in-combat capacitive-grip-only upper-point-delayed raise-only left-weapon support-hand occupied-hand unknown-hand fist partial-trigger setting-off nonfinite-trigger partial-trigger-cancels fist-cancels tracking-loss context-revoke grip-release held-jitter choice-1 choice-2 choice-3 choice-4 choice-owner-tracking choice-owner-occupied choice-owner-near choice-owner-grip-nan choice-owner-trigger-nan choice-owner-forward-nan choice-owner-up-inf choice-owner-lease-grip-nan choice-owner-lease-trigger-nan choice-owner-lease-forward-nan choice-owner-lease-up-inf choice-owner-relaxed)
 add_test(NAME quick_talk_combat_h37_${case} COMMAND quick_talk_combat_h37_test ${case})
endforeach()
# R93R1 optional index pointing: same extracted current host and actual native pad.
add_executable(quick_talk_point_r93r1_test quick_talk_point_r93r1_test.cpp)
target_include_directories(quick_talk_point_r93r1_test PRIVATE syntax_shims openxr_sdk)
target_compile_features(quick_talk_point_r93r1_test PRIVATE cxx_std_20)
foreach(case normal dog-low index-on setting-off no-target held support unknown holster pickup corpse door near-grab trigger inward behind too-close too-low nonfinite raise self-pat x choice1 choice2 choice3 choice4 grip-release tracking-loss new-held menu rearm)
 add_test(NAME quick_talk_point_r93r1_${case} COMMAND quick_talk_point_r93r1_test ${case})
endforeach()
add_executable(quick_talk_holster_h4_bridge quick_talk_holster_h4_bridge.cpp bridge_external_stubs112.cpp ../src/subsystems/doors/module.cpp)
target_include_directories(quick_talk_holster_h4_bridge PRIVATE syntax_shims openxr_sdk)
target_compile_definitions(quick_talk_holster_h4_bridge PRIVATE _ReturnAddress=HostReturnAddress)
target_compile_options(quick_talk_holster_h4_bridge PRIVATE /Gy /Gw /bigobj)
target_link_options(quick_talk_holster_h4_bridge PRIVATE /OPT:REF /INCREMENTAL:NO /STACK:33554432)
target_link_libraries(quick_talk_holster_h4_bridge PRIVATE Threads::Threads)
add_test(NAME quick_talk_holster_h4 COMMAND quick_talk_holster_h4_bridge)

add_executable(living_reference_obligation_h5_test living_reference_obligation_h5_test.cpp)
target_compile_features(living_reference_obligation_h5_test PRIVATE cxx_std_20)
target_include_directories(living_reference_obligation_h5_test PRIVATE syntax_shims)
foreach(case ordinary native-zero native-negative independent-owner positive-fault positive-context-loss positive-world-loss positive-entity-loss)
 add_test(NAME living_obligation_h5_${case} COMMAND living_reference_obligation_h5_test ${case})
endforeach()

add_executable(wall_response_h29r4_test wall_response_h29r4_test.cpp)
target_include_directories(wall_response_h29r4_test PRIVATE ../src)
target_compile_features(wall_response_h29r4_test PRIVATE cxx_std_17)
if(MSVC)
 target_link_options(wall_response_h29r4_test PRIVATE /STACK:8388608)
endif()
add_test(NAME wall_response_h29r4 COMMAND wall_response_h29r4_test)

# Current captured source + typed native weapon/Henry pair owner.
execute_process(COMMAND "${Python3_EXECUTABLE}" "${CMAKE_CURRENT_SOURCE_DIR}/../tools/extract_f9_scale_fixture_h5.py" COMMAND_ERROR_IS_FATAL ANY)
execute_process(COMMAND "${Python3_EXECUTABLE}" "${CMAKE_CURRENT_SOURCE_DIR}/../tools/extract_weapon_self_pair_capture_h5.py" COMMAND_ERROR_IS_FATAL ANY)
add_executable(weapon_self_pair_h5_test "${CMAKE_CURRENT_SOURCE_DIR}/../tests/weapon_self_pair_h5_test.cpp")
target_compile_features(weapon_self_pair_h5_test PRIVATE cxx_std_23)
target_include_directories(weapon_self_pair_h5_test PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/../tests/syntax_shims" "${CMAKE_CURRENT_SOURCE_DIR}/../src")
if(MSVC)
 target_compile_options(weapon_self_pair_h5_test PRIVATE /UNDEBUG /Gy /Gw /bigobj /FI"${CMAKE_CURRENT_SOURCE_DIR}/../tests/host_test_runtime.h")
 target_link_options(weapon_self_pair_h5_test PRIVATE /OPT:REF /INCREMENTAL:NO /STACK:8388608)
endif()
foreach(self_pair_case lifecycle default-off public-capture pending refused refused-ref source-stale-word retained-revoked pending-timeout uncertain toggle-during-add open-revoke class-fault no-absent remove-refused remove-fault release-fault class-close-refused detach-refused already-absent unknown-present external-pin reset shutdown replaced wrong-pair source-loss henry-replaced tracking-loss menu-loss offhand-item cancel f9-off request-change item-change body-epoch-change tracking-epoch-change policy-epoch-change collider-change packet-stale same-address-aba aba id-high-bit pending-reset pending-shutdown remove-timeout double-toggle-removal positive-reference-fault external-pin-reset off-in-class off-in-read-predicate aba-in-read-predicate off-in-contact-read aba-in-contact-read off-in-status off-in-motor off-in-pair-check long-open-remove)
 add_test(NAME weapon_self_pair_${self_pair_case} COMMAND weapon_self_pair_h5_test ${self_pair_case})
 set_tests_properties(weapon_self_pair_${self_pair_case} PROPERTIES TIMEOUT 30)
endforeach()
add_test(NAME weapon_self_pair_capture_h5_extraction COMMAND "${Python3_EXECUTABLE}" "${CMAKE_CURRENT_SOURCE_DIR}/../tools/extract_weapon_self_pair_capture_h5.py" --check)

# Append in tests/CMakeLists.txt after Python3 discovery; Windows actual-GDI fixture.
if(MSVC)
 set(H5_MENU_GENERATED "${CMAKE_CURRENT_BINARY_DIR}/experimental_features_h5_generated")
 set_property(DIRECTORY APPEND PROPERTY CMAKE_CONFIGURE_DEPENDS
  "${CMAKE_CURRENT_SOURCE_DIR}/../src/vr_menu.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/../src/vr_menu.h"
  "${CMAKE_CURRENT_SOURCE_DIR}/../src/subsystems/experimental/features_menu_h5.h"
  "${CMAKE_CURRENT_SOURCE_DIR}/../src/subsystems/experimental/deepseek_options_h29.h"
  "${CMAKE_CURRENT_SOURCE_DIR}/../tools/extract_experimental_features_h5.py")
 execute_process(COMMAND "${Python3_EXECUTABLE}" "${CMAKE_CURRENT_SOURCE_DIR}/../tools/extract_experimental_features_h5.py" "${H5_MENU_GENERATED}" COMMAND_ERROR_IS_FATAL ANY)
 add_executable(experimental_features_h5_test experimental_features_h5_test.cpp)
 target_compile_features(experimental_features_h5_test PRIVATE cxx_std_23)
 target_include_directories(experimental_features_h5_test PRIVATE "${H5_MENU_GENERATED}" "${CMAKE_CURRENT_SOURCE_DIR}/../src")
 target_compile_options(experimental_features_h5_test PRIVATE /UNDEBUG /Gy /Gw /bigobj /FI"${CMAKE_CURRENT_SOURCE_DIR}/host_test_runtime.h")
 target_link_options(experimental_features_h5_test PRIVATE /OPT:REF /INCREMENTAL:NO /STACK:8388608)
 target_link_libraries(experimental_features_h5_test PRIVATE gdi32 user32)
 set(H5_MENU_SCREENSHOTS "${CMAKE_CURRENT_BINARY_DIR}/experimental_features_h5_screenshots")
 file(MAKE_DIRECTORY "${H5_MENU_SCREENSHOTS}")
 add_test(NAME experimental_features_h5 COMMAND experimental_features_h5_test "${H5_MENU_SCREENSHOTS}")
 set_tests_properties(experimental_features_h5 PROPERTIES TIMEOUT 30)
endif()

# M34 map-only completed hand ownership; actual existing fist predicate and
# physical support producer, never a new gameplay gate or native API.
set(M34_MAP_GENERATED "${CMAKE_CURRENT_BINARY_DIR}/map_owner_m34_generated")
file(MAKE_DIRECTORY "${M34_MAP_GENERATED}")
execute_process(COMMAND "${Python3_EXECUTABLE}" "${CMAKE_CURRENT_SOURCE_DIR}/../tools/extract_map_hand_owner_m34.py" "${M34_MAP_GENERATED}" COMMAND_ERROR_IS_FATAL ANY)
add_executable(map_hand_owner_m34_test map_hand_owner_m34_test.cpp)
target_compile_features(map_hand_owner_m34_test PRIVATE cxx_std_23)
target_include_directories(map_hand_owner_m34_test PRIVATE "${M34_MAP_GENERATED}")
add_test(NAME map_hand_owner_m34 COMMAND map_hand_owner_m34_test)

# A34: original native activity witness and protected physical consumers.
if(MSVC)
 set(A34_GENERATED "${CMAKE_CURRENT_BINARY_DIR}/passenger_a34_generated")
 set_property(DIRECTORY APPEND PROPERTY CMAKE_CONFIGURE_DEPENDS
  "${CMAKE_CURRENT_SOURCE_DIR}/../src/action_time_owner59c11.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/../src/animation_filter.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/../src/frik_pose_probe.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/../src/subsystems/interactions/native_passenger_a34.h"
  "${CMAKE_CURRENT_SOURCE_DIR}/../src/subsystems/interactions/runtime/native_passenger_a34.inl"
  "${CMAKE_CURRENT_SOURCE_DIR}/../src/subsystems/physics/native_held_owner_t4.inl"
  "${CMAKE_CURRENT_SOURCE_DIR}/../src/vrik/hand_presence_h24.inl"
  "${CMAKE_CURRENT_SOURCE_DIR}/../tools/test_passenger_composition_a34.py"
  "${CMAKE_CURRENT_SOURCE_DIR}/../tools/extract_passenger_source_a34.py")
 execute_process(COMMAND "${Python3_EXECUTABLE}" "${CMAKE_CURRENT_SOURCE_DIR}/../tools/test_passenger_composition_a34.py" --out "${A34_GENERATED}" COMMAND_ERROR_IS_FATAL ANY)
 execute_process(COMMAND "${Python3_EXECUTABLE}" "${CMAKE_CURRENT_SOURCE_DIR}/../tools/extract_passenger_source_a34.py" "${A34_GENERATED}/generated_passenger_source_a34.inl" COMMAND_ERROR_IS_FATAL ANY)
 add_executable(passenger_composition_a34_test "${A34_GENERATED}/a34_current_composition.cpp")
 target_include_directories(passenger_composition_a34_test PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/../src")
 target_compile_features(passenger_composition_a34_test PRIVATE cxx_std_17)
 add_test(NAME passenger_composition_a34 COMMAND passenger_composition_a34_test)
 add_executable(passenger_source_a34_test native_passenger_source_a34_test.cpp)
 target_compile_features(passenger_source_a34_test PRIVATE cxx_std_23)
 target_include_directories(passenger_source_a34_test PRIVATE "${A34_GENERATED}" syntax_shims "${CMAKE_CURRENT_SOURCE_DIR}/../src")
 target_compile_options(passenger_source_a34_test PRIVATE /UNDEBUG /Gy /Gw /bigobj)
 target_link_options(passenger_source_a34_test PRIVATE /OPT:REF /INCREMENTAL:NO)
 target_link_libraries(passenger_source_a34_test PRIVATE Threads::Threads)
 add_test(NAME passenger_source_a34 COMMAND passenger_source_a34_test)
endif()

# H36 isolated source candidate: actual holster producer/native queue prefix and
# source/Stage ownership. Native request leaf is intentionally not executed.
execute_process(COMMAND "${Python3_EXECUTABLE}" "${CMAKE_CURRENT_SOURCE_DIR}/../tools/extract_holster_side_retirement_h36.py" COMMAND_ERROR_IS_FATAL ANY)
add_executable(holster_side_retirement_h36_bridge holster_side_retirement_h36_bridge.cpp bridge_external_stubs112.cpp ../src/subsystems/doors/module.cpp)
target_include_directories(holster_side_retirement_h36_bridge PRIVATE syntax_shims openxr_sdk)
target_compile_definitions(holster_side_retirement_h36_bridge PRIVATE _ReturnAddress=HostReturnAddress)
target_compile_features(holster_side_retirement_h36_bridge PRIVATE cxx_std_23)
target_compile_options(holster_side_retirement_h36_bridge PRIVATE /Gy /Gw /bigobj)
target_link_options(holster_side_retirement_h36_bridge PRIVATE /OPT:REF /INCREMENTAL:NO /STACK:33554432)
target_link_libraries(holster_side_retirement_h36_bridge PRIVATE Threads::Threads)
add_test(NAME holster_side_retirement_h36 COMMAND holster_side_retirement_h36_bridge)


# H36R1 standard-cycle original native call/return and actual current attachment
# memory proof; primitive native execution is an explicit host service.
execute_process(COMMAND "${Python3_EXECUTABLE}" "${CMAKE_CURRENT_SOURCE_DIR}/../tools/extract_holster_completion_h36.py" COMMAND_ERROR_IS_FATAL ANY)
add_executable(holster_completion_h36_bridge holster_completion_h36_bridge.cpp bridge_external_stubs112.cpp ../src/subsystems/doors/module.cpp)
target_include_directories(holster_completion_h36_bridge PRIVATE syntax_shims openxr_sdk)
target_compile_definitions(holster_completion_h36_bridge PRIVATE _ReturnAddress=HostReturnAddress H36R1_HAS_COMPLETION=1 H36R2_HAS_INITIAL_OWNER=1)
target_compile_features(holster_completion_h36_bridge PRIVATE cxx_std_23)
target_compile_options(holster_completion_h36_bridge PRIVATE /Gy /Gw /bigobj)
target_link_options(holster_completion_h36_bridge PRIVATE /OPT:REF /INCREMENTAL:NO /STACK:33554432)
target_link_libraries(holster_completion_h36_bridge PRIVATE Threads::Threads)
add_test(NAME holster_completion_h36 COMMAND holster_completion_h36_bridge)

add_executable(holster_initial_draw_h36r3_bridge holster_initial_draw_h36r3_bridge.cpp bridge_external_stubs112.cpp ../src/subsystems/doors/module.cpp)
target_include_directories(holster_initial_draw_h36r3_bridge PRIVATE syntax_shims openxr_sdk)
target_compile_definitions(holster_initial_draw_h36r3_bridge PRIVATE _ReturnAddress=HostReturnAddress H36R1_HAS_COMPLETION=1 H36R2_HAS_INITIAL_OWNER=1)
target_compile_features(holster_initial_draw_h36r3_bridge PRIVATE cxx_std_23)
target_compile_options(holster_initial_draw_h36r3_bridge PRIVATE /Gy /Gw /bigobj)
target_link_options(holster_initial_draw_h36r3_bridge PRIVATE /OPT:REF /INCREMENTAL:NO /STACK:33554432)
target_link_libraries(holster_initial_draw_h36r3_bridge PRIVATE Threads::Threads)
add_test(NAME holster_initial_draw_h36r3 COMMAND holster_initial_draw_h36r3_bridge)

# This target deliberately uses the REAL Windows SDK, not syntax_shims. It
# exercises the same actual __try/__finally helper under production /EHsc.
if(MSVC)
 add_executable(initial_draw_seh_h36r3_test initial_draw_seh_h36r3_test.cpp)
 target_compile_features(initial_draw_seh_h36r3_test PRIVATE cxx_std_23)
 add_test(NAME initial_draw_seh_h36r3 COMMAND initial_draw_seh_h36r3_test)
endif()

add_executable(initial_draw_slot_h36r3_test initial_draw_slot_h36r3_test.cpp)
target_compile_features(initial_draw_slot_h36r3_test PRIVATE cxx_std_23)
target_link_libraries(initial_draw_slot_h36r3_test PRIVATE Threads::Threads)
add_test(NAME initial_draw_slot_h36r3 COMMAND initial_draw_slot_h36r3_test)

# Exact entry wrappers with REAL Windows SEH, no syntax Windows shim.
execute_process(COMMAND "${Python3_EXECUTABLE}" "${CMAKE_CURRENT_SOURCE_DIR}/../tools/extract_reload_callback_counter_xr34.py" COMMAND_ERROR_IS_FATAL ANY)
if(MSVC)
 add_executable(reload_callback_counter_xr34_test reload_callback_counter_xr34_test.cpp)
 target_compile_features(reload_callback_counter_xr34_test PRIVATE cxx_std_23)
 add_test(NAME reload_callback_counter_xr34 COMMAND reload_callback_counter_xr34_test)
endif()

# H40: actual completed-pose, Grip input and original-first service composition.
# Native engine leaves are modeled host services; no headset acceptance is implied.
add_executable(hand_only_h40_full_bridge hand_only_h40_full_bridge.cpp bridge_external_stubs112.cpp ../src/subsystems/doors/module.cpp)
target_include_directories(hand_only_h40_full_bridge PRIVATE syntax_shims openxr_sdk)
target_compile_definitions(hand_only_h40_full_bridge PRIVATE _ReturnAddress=HostReturnAddress H36R1_HAS_COMPLETION=1 H36R2_HAS_INITIAL_OWNER=1)
target_compile_features(hand_only_h40_full_bridge PRIVATE cxx_std_23)
target_compile_options(hand_only_h40_full_bridge PRIVATE /Gy /Gw /bigobj)
target_link_options(hand_only_h40_full_bridge PRIVATE /OPT:REF /INCREMENTAL:NO /STACK:33554432)
target_link_libraries(hand_only_h40_full_bridge PRIVATE Threads::Threads)
add_test(NAME hand_only_h40_full_bridge COMMAND hand_only_h40_full_bridge)

# Preserve the prior H39 strict same-hand fist/holster producer contract.
add_executable(holster_fist_h39_bridge holster_fist_h39_bridge.cpp bridge_external_stubs112.cpp ../src/subsystems/doors/module.cpp)
target_include_directories(holster_fist_h39_bridge PRIVATE syntax_shims openxr_sdk)
target_compile_definitions(holster_fist_h39_bridge PRIVATE _ReturnAddress=HostReturnAddress)
target_compile_features(holster_fist_h39_bridge PRIVATE cxx_std_23)
target_compile_options(holster_fist_h39_bridge PRIVATE /Gy /Gw /bigobj)
target_link_options(holster_fist_h39_bridge PRIVATE /OPT:REF /INCREMENTAL:NO /STACK:33554432)
target_link_libraries(holster_fist_h39_bridge PRIVATE Threads::Threads)
add_test(NAME holster_fist_h39_bridge COMMAND holster_fist_h39_bridge)

# H41 actual per-hand door/throw transaction and current T24/XR debt readers.
add_executable(door_throw_admission_h41_test door_throw_admission_h41_test.cpp)
target_compile_features(door_throw_admission_h41_test PRIVATE cxx_std_20)
add_test(NAME door_throw_admission_h41 COMMAND door_throw_admission_h41_test)

# Actual F9 committed-contact, current native shape mapping and AddImpact owner.
# Native calls are explicit test leaves; this is not game movement acceptance.
add_executable(native_touch_h9_test native_touch_h9_test.cpp)
target_compile_features(native_touch_h9_test PRIVATE cxx_std_20)
add_test(NAME native_touch_h9 COMMAND native_touch_h9_test)

# H50: actual coherent pose/physical inputs, native services modeled by retained fixture.
add_executable(hand_smooth_h50_bridge_test hand_smooth_h50_bridge_test.cpp)
target_compile_features(hand_smooth_h50_bridge_test PRIVATE cxx_std_23)
if(MSVC)
 target_compile_options(hand_smooth_h50_bridge_test PRIVATE /bigobj)
 target_link_options(hand_smooth_h50_bridge_test PRIVATE /STACK:33554432)
endif()
add_test(NAME hand_smooth_h50_bridge COMMAND hand_smooth_h50_bridge_test)
add_executable(npc_reacts_to_blade_nr1_test npc_reacts_to_blade_nr1_test.cpp)
add_test(NAME npc_reacts_to_blade_nr1 COMMAND npc_reacts_to_blade_nr1_test)
add_executable(physical_finisher_cb1_test physical_finisher_cb1_test.cpp)
add_test(NAME physical_finisher_cb1 COMMAND physical_finisher_cb1_test)
add_executable(finger_rig_w1_test finger_rig_w1_test.cpp)
target_include_directories(finger_rig_w1_test PRIVATE ../src)
add_test(NAME finger_rig_w1 COMMAND finger_rig_w1_test)
add_executable(hand_same_frame_hj1_test hand_same_frame_hj1_test.cpp)
target_include_directories(hand_same_frame_hj1_test PRIVATE ../src)
add_test(NAME hand_same_frame_hj1 COMMAND hand_same_frame_hj1_test)
add_executable(smooth_lab_f2_test smooth_lab_f2_test.cpp)
target_include_directories(smooth_lab_f2_test PRIVATE ../src)
add_test(NAME smooth_lab_f2 COMMAND smooth_lab_f2_test)
add_executable(craft_attempt_w4_test craft_attempt_w4_test.cpp)
target_include_directories(craft_attempt_w4_test PRIVATE ../src)
add_test(NAME craft_attempt_w4 COMMAND craft_attempt_w4_test)
add_executable(native_agree_w4_test subsystems/alchemy/native_agree_w4_test.cpp)
target_include_directories(native_agree_w4_test PRIVATE ../src)
add_test(NAME native_agree_w4 COMMAND native_agree_w4_test)
add_executable(physics_lab_w5_test physics_lab_w5_test.cpp)
target_include_directories(physics_lab_w5_test PRIVATE ../src)
add_test(NAME physics_lab_w5 COMMAND physics_lab_w5_test)
add_executable(contact_glide_w5_test contact_glide_w5_test.cpp)
target_include_directories(contact_glide_w5_test PRIVATE ../src)
add_test(NAME contact_glide_w5 COMMAND contact_glide_w5_test)
# J27 station torso behind the head + smith tool contact rules
add_executable(station_ws_j27_test station_ws_j27_test.cpp)
target_include_directories(station_ws_j27_test PRIVATE ../src)
add_test(NAME station_ws_j27 COMMAND station_ws_j27_test)
add_executable(r8b_bellows_type_test subsystems/alchemy/bellows_type_r8b_test.cpp)
target_include_directories(r8b_bellows_type_test PRIVATE subsystems/alchemy)
add_test(NAME r8b_bellows_type COMMAND r8b_bellows_type_test)
add_executable(r8b_craft_recorder_test craft_recorder_r8b_test.cpp)
add_test(NAME r8b_craft_recorder COMMAND r8b_craft_recorder_test)
add_executable(ammo_readiness_k32_test ammo_readiness_k32_test.cpp)
add_test(NAME ammo_readiness_k32 COMMAND ammo_readiness_k32_test)
add_executable(frik_elbow_lab_test frik_elbow_lab_test.cpp)
target_include_directories(frik_elbow_lab_test PRIVATE ../src)
add_test(NAME frik_elbow_lab COMMAND frik_elbow_lab_test)
add_executable(holster_diag_r8d_test holster_diag_r8d_test.cpp)
target_include_directories(holster_diag_r8d_test PRIVATE syntax_shims openxr_sdk)
add_test(NAME holster_diag_r8d COMMAND holster_diag_r8d_test)
add_executable(camera_hold_e1_test camera_hold_e1_test.cpp)
target_include_directories(camera_hold_e1_test PRIVATE ${CMAKE_SOURCE_DIR}/src ${CMAKE_CURRENT_SOURCE_DIR} syntax_shims openxr_sdk)
add_test(NAME camera_hold_e1 COMMAND camera_hold_e1_test)
add_executable(bow_arm_aim_r9_test bow_arm_aim_r9_test.cpp)
target_include_directories(bow_arm_aim_r9_test PRIVATE ${CMAKE_SOURCE_DIR}/src ${CMAKE_CURRENT_SOURCE_DIR} syntax_shims openxr_sdk)
add_test(NAME bow_arm_aim_r9 COMMAND bow_arm_aim_r9_test)
add_executable(door_press_why_r9_test door_press_why_r9_test.cpp)
target_include_directories(door_press_why_r9_test PRIVATE syntax_shims openxr_sdk)
add_test(NAME door_press_why_r9 COMMAND door_press_why_r9_test)
add_executable(station_fallback_r9c_test station_fallback_r9c_test.cpp)
target_include_directories(station_fallback_r9c_test PRIVATE ${CMAKE_SOURCE_DIR}/src syntax_shims openxr_sdk)
add_test(NAME station_fallback_r9c COMMAND station_fallback_r9c_test)
add_executable(smooth_hands_r9_test smooth_hands_r9_test.cpp)
target_include_directories(smooth_hands_r9_test PRIVATE ../src ${CMAKE_CURRENT_SOURCE_DIR} syntax_shims openxr_sdk)
add_test(NAME smooth_hands_r9 COMMAND smooth_hands_r9_test)
add_executable(combat_r9_test combat_r9_test.cpp)
target_include_directories(combat_r9_test PRIVATE ${CMAKE_SOURCE_DIR}/src ${CMAKE_CURRENT_SOURCE_DIR} syntax_shims openxr_sdk)
add_test(NAME combat_r9 COMMAND combat_r9_test)
add_executable(fist_reach_r10_test fist_reach_r10_test.cpp)
target_include_directories(fist_reach_r10_test PRIVATE ${CMAKE_SOURCE_DIR}/src ${CMAKE_CURRENT_SOURCE_DIR} syntax_shims openxr_sdk)
add_test(NAME fist_reach_r10 COMMAND fist_reach_r10_test)

# Current production fist clock/publisher; generated bodies stay in the build tree.
set(FIST_CLOCK_R11_GENERATED "${CMAKE_CURRENT_BINARY_DIR}/fist_clock_r11_generated")
set(FIST_CLOCK_R11_OUTPUTS
 "${FIST_CLOCK_R11_GENERATED}/fist_clock_r11_frame.inc"
 "${FIST_CLOCK_R11_GENERATED}/fist_clock_r11_publisher.inc"
 "${FIST_CLOCK_R11_GENERATED}/fist_clock_r11_callback.inc"
 "${FIST_CLOCK_R11_GENERATED}/fist_clock_r11_commit.inc")
add_custom_command(OUTPUT ${FIST_CLOCK_R11_OUTPUTS}
 COMMAND "${Python3_EXECUTABLE}" "${CMAKE_CURRENT_SOURCE_DIR}/../tools/extract_fist_clock_r11.py" "${FIST_CLOCK_R11_GENERATED}"
 DEPENDS ../tools/extract_fist_clock_r11.py ../src/vrik/fist_native.inl ../src/vrik/native_attack_clock50c2.inl ../src/vrik/coherent_pose.inl
 VERBATIM)
add_executable(fist_clock_r11_test fist_clock_r11_test.cpp ${FIST_CLOCK_R11_OUTPUTS})
target_include_directories(fist_clock_r11_test PRIVATE ../src "${FIST_CLOCK_R11_GENERATED}")
target_compile_features(fist_clock_r11_test PRIVATE cxx_std_17)
add_test(NAME fist_clock_r11 COMMAND fist_clock_r11_test)

# Actual scoped native-fist geometry receiver; engine bodies are explicit fixture leaves.
set(FIST_SHAPE_R11_GENERATED "${CMAKE_CURRENT_BINARY_DIR}/fist_shape_r11_generated")
set(FIST_SHAPE_R11_OUTPUTS
 "${FIST_SHAPE_R11_GENERATED}/fist_clock_r11_frame.inc"
 "${FIST_SHAPE_R11_GENERATED}/fist_clock_r11_publisher.inc"
 "${FIST_SHAPE_R11_GENERATED}/fist_clock_r11_callback.inc"
 "${FIST_SHAPE_R11_GENERATED}/fist_shape_r11_authority.inc"
 "${FIST_SHAPE_R11_GENERATED}/fist_shape_r11_receiver.inc")
add_custom_command(OUTPUT ${FIST_SHAPE_R11_OUTPUTS}
 COMMAND "${Python3_EXECUTABLE}" "${CMAKE_CURRENT_SOURCE_DIR}/../tools/extract_fist_shape_r11.py" "${FIST_SHAPE_R11_GENERATED}"
 DEPENDS ../tools/extract_fist_shape_r11.py ../tools/extract_fist_clock_r11.py
         ../src/vrik/fist_native.inl ../src/vrik/native_attack_clock50c2.inl
         ../src/vrik/native_prediction120.inl ../src/frik_pose_probe.cpp
 VERBATIM)
add_executable(fist_shape_r11_test fist_shape_r11_test.cpp ${FIST_SHAPE_R11_OUTPUTS})
target_include_directories(fist_shape_r11_test PRIVATE ../src "${FIST_SHAPE_R11_GENERATED}")
target_compile_features(fist_shape_r11_test PRIVATE cxx_std_17)
add_test(NAME fist_shape_r11 COMMAND fist_shape_r11_test)

# A3 real anvil (physics box selection + swept box collision)
add_executable(f9_status_fd_test f9_status_fd_test.cpp)
target_include_directories(f9_status_fd_test PRIVATE ../src)
add_test(NAME f9_status_fd COMMAND f9_status_fd_test)
add_executable(anvil_a3_test anvil_a3_test.cpp)
target_include_directories(anvil_a3_test PRIVATE ../src)
add_test(NAME anvil_a3 COMMAND anvil_a3_test)
add_executable(holster_history_hs_test holster_history_hs_test.cpp)
target_include_directories(holster_history_hs_test PRIVATE ../src)
add_test(NAME holster_history_hs COMMAND holster_history_hs_test)
# I10 (Claude): native kill animations switch (NF/NF2/NF3)
add_executable(native_kill_anim_nf_test native_kill_anim_nf_test.cpp)
find_package(Threads REQUIRED)
target_link_libraries(native_kill_anim_nf_test PRIVATE Threads::Threads)
target_include_directories(native_kill_anim_nf_test PRIVATE ${CMAKE_SOURCE_DIR}/src syntax_shims openxr_sdk)
add_test(NAME native_kill_anim_nf COMMAND native_kill_anim_nf_test)
# I10 (Claude): impale root type 2 + gave-up text, corpse-stab impale
add_executable(impale_root_r14_test impale_root_r14_test.cpp)
target_include_directories(impale_root_r14_test PRIVATE ${CMAKE_SOURCE_DIR}/src syntax_shims openxr_sdk)
add_test(NAME impale_root_r14 COMMAND impale_root_r14_test)
# R128: impale hold against gravity, full depth to the guard, plate rule, swing-off release
add_executable(impale_full_r128_test impale_full_r128_test.cpp)
target_include_directories(impale_full_r128_test PRIVATE ${CMAKE_SOURCE_DIR}/src syntax_shims openxr_sdk)
add_test(NAME impale_full_r128 COMMAND impale_full_r128_test)
add_executable(impale_corpse_r15_test impale_corpse_r15_test.cpp)
target_include_directories(impale_corpse_r15_test PRIVATE ${CMAKE_SOURCE_DIR}/src syntax_shims openxr_sdk)
add_test(NAME impale_corpse_r15 COMMAND impale_corpse_r15_test)
add_executable(master_gate_side_i10_test master_gate_side_i10_test.cpp)
target_include_directories(master_gate_side_i10_test PRIVATE ${CMAKE_SOURCE_DIR}/src syntax_shims openxr_sdk)
add_test(NAME master_gate_side_i10 COMMAND master_gate_side_i10_test)
add_executable(frame_summary_d30_test frame_summary_d30_test.cpp)
target_include_directories(frame_summary_d30_test PRIVATE ../src)
add_test(NAME frame_summary_d30 COMMAND frame_summary_d30_test)
add_executable(frame_summary_d44_test frame_summary_d44_test.cpp)
target_compile_definitions(frame_summary_d44_test PRIVATE D44_CLOCKED)
target_include_directories(frame_summary_d44_test PRIVATE ../src)
add_test(NAME frame_summary_d44 COMMAND frame_summary_d44_test)
add_executable(combo_carrier_cb_test combo_carrier_cb_test.cpp)
target_include_directories(combo_carrier_cb_test PRIVATE ../src)
add_test(NAME combo_carrier_cb COMMAND combo_carrier_cb_test)
add_executable(page_memo_pf_test page_memo_pf_test.cpp)
target_include_directories(page_memo_pf_test PRIVATE ../src)
add_test(NAME page_memo_pf COMMAND page_memo_pf_test)
add_executable(rifle_hint_d32_test rifle_hint_d32_test.cpp)
target_include_directories(rifle_hint_d32_test PRIVATE ../src)
add_test(NAME rifle_hint_d32 COMMAND rifle_hint_d32_test)
add_executable(gesture_talk_policy_d33_test gesture_talk_policy_d33_test.cpp)
target_include_directories(gesture_talk_policy_d33_test PRIVATE ../src)
add_test(NAME gesture_talk_policy_d33 COMMAND gesture_talk_policy_d33_test)
add_executable(stowed_hide_hw_test stowed_hide_hw_test.cpp)
target_include_directories(stowed_hide_hw_test PRIVATE ../src)
add_test(NAME stowed_hide_hw COMMAND stowed_hide_hw_test)

# D37: native XI must honor selected snap turning without duplicate analog yaw.
add_executable(snap_turn_d37_test snap_turn_d37_test.cpp)
target_include_directories(snap_turn_d37_test PRIVATE syntax_shims openxr_sdk)
add_test(NAME snap_turn_d37 COMMAND snap_turn_d37_test)

# Controller-authoritative wrist / shoulder accommodation regression.
add_executable(controller_authority_r38_test controller_authority_r38_test.cpp)
target_include_directories(controller_authority_r38_test PRIVATE ../src)
add_test(NAME controller_authority_r38 COMMAND controller_authority_r38_test)

# R38 head-led body posture, yaw wrap and foot transaction regression.
add_executable(body_follow_r38_test body_follow_r38_test.cpp)
target_include_directories(body_follow_r38_test PRIVATE ../src)
add_test(NAME body_follow_r38 COMMAND body_follow_r38_test)

add_executable(torso_anchors_r38_test torso_anchors_r38_test.cpp)
target_include_directories(torso_anchors_r38_test PRIVATE ../src)
add_test(NAME torso_anchors_r38 COMMAND torso_anchors_r38_test)

add_executable(world_turn_coverage_d44_test world_turn_coverage_d44_test.cpp)
target_compile_definitions(world_turn_coverage_d44_test PRIVATE D44_ROOT_COVERAGE)
target_include_directories(world_turn_coverage_d44_test PRIVATE ../src)
add_test(NAME world_turn_coverage_d44 COMMAND world_turn_coverage_d44_test)

add_executable(elbow_condition_trace_d44_test elbow_condition_trace_d44_test.cpp)
target_include_directories(elbow_condition_trace_d44_test PRIVATE ../src)
add_test(NAME elbow_condition_trace_d44 COMMAND elbow_condition_trace_d44_test)

add_executable(pickup_visual_use_d44_test pickup_visual_use_d44_test.cpp)
add_test(NAME pickup_visual_use_d44 COMMAND pickup_visual_use_d44_test)

add_executable(holster_instant_d44_test holster_instant_d44_test.cpp)
target_include_directories(holster_instant_d44_test PRIVATE syntax_shims ../src)
add_test(NAME holster_instant_d44 COMMAND holster_instant_d44_test)

add_executable(initial_acquire_sweep_d44_test initial_acquire_sweep_d44_test.cpp)
target_compile_definitions(initial_acquire_sweep_d44_test PRIVATE D44_INITIAL_SWEEP)
target_include_directories(initial_acquire_sweep_d44_test PRIVATE ../src)
add_test(NAME initial_acquire_sweep_d44 COMMAND initial_acquire_sweep_d44_test)

# D46 combined source: focused production-leaf regression fixtures.
add_executable(alchemy_gaze_assist_d44_test alchemy_gaze_assist_d44_test.cpp)
target_include_directories(alchemy_gaze_assist_d44_test PRIVATE ../src syntax_shims openxr_sdk)
add_test(NAME alchemy_gaze_assist_d44 COMMAND alchemy_gaze_assist_d44_test)
add_executable(bow_acquisition_authority_d44_test bow_acquisition_authority_d44_test.cpp)
target_include_directories(bow_acquisition_authority_d44_test PRIVATE ../src syntax_shims openxr_sdk)
add_test(NAME bow_acquisition_authority_d44 COMMAND bow_acquisition_authority_d44_test)
add_executable(draw_visibility_d45_test draw_visibility_d45_test.cpp)
target_include_directories(draw_visibility_d45_test PRIVATE ../src syntax_shims openxr_sdk)
add_test(NAME draw_visibility_d45 COMMAND draw_visibility_d45_test)
add_executable(early_frame_pacing_d45_test early_frame_pacing_d45_test.cpp)
target_include_directories(early_frame_pacing_d45_test PRIVATE ../src syntax_shims openxr_sdk)
add_test(NAME early_frame_pacing_d45 COMMAND early_frame_pacing_d45_test)
add_executable(elbow_shoulder_d45_test elbow_shoulder_d45_test.cpp)
target_include_directories(elbow_shoulder_d45_test PRIVATE ../src syntax_shims openxr_sdk)
add_test(NAME elbow_shoulder_d45 COMMAND elbow_shoulder_d45_test)
add_executable(initial_acquire_burst_d45_test initial_acquire_burst_d45_test.cpp)
target_include_directories(initial_acquire_burst_d45_test PRIVATE ../src syntax_shims openxr_sdk)
add_test(NAME initial_acquire_burst_d45 COMMAND initial_acquire_burst_d45_test)
add_executable(pickup_controller_d44_test pickup_controller_d44_test.cpp)
target_include_directories(pickup_controller_d44_test PRIVATE ../src syntax_shims openxr_sdk)
add_test(NAME pickup_controller_d44 COMMAND pickup_controller_d44_test)
add_executable(release_transport_d44_test release_transport_d44_test.cpp)
target_include_directories(release_transport_d44_test PRIVATE ../src syntax_shims openxr_sdk)
add_test(NAME release_transport_d44 COMMAND release_transport_d44_test)
add_executable(root_turn_sweep_d44_test root_turn_sweep_d44_test.cpp)
target_include_directories(root_turn_sweep_d44_test PRIVATE ../src syntax_shims openxr_sdk)
add_test(NAME root_turn_sweep_d44 COMMAND root_turn_sweep_d44_test)
add_executable(static_letter_d44_test static_letter_d44_test.cpp)
target_include_directories(static_letter_d44_test PRIVATE ../src syntax_shims openxr_sdk)
add_test(NAME static_letter_d44 COMMAND static_letter_d44_test)
add_executable(vrik_trace_d45_test vrik_trace_d45_test.cpp)
target_include_directories(vrik_trace_d45_test PRIVATE ../src syntax_shims openxr_sdk)
add_test(NAME vrik_trace_d45 COMMAND vrik_trace_d45_test)
find_package(Python3 COMPONENTS Interpreter QUIET)
if(Python3_Interpreter_FOUND)
 add_test(NAME draw_native_d45_source_match COMMAND "${Python3_EXECUTABLE}" "${CMAKE_CURRENT_SOURCE_DIR}/../tools/extract_draw_native_d45_test.py" --check)
endif()

add_executable(d48_diagnostic_policy_test d48_diagnostic_policy_test.cpp)
add_test(NAME d48_diagnostic_policy COMMAND d48_diagnostic_policy_test)

add_executable(d48_interaction_intent_test d48_interaction_intent_test.cpp)
add_test(NAME d48_interaction_intent COMMAND d48_interaction_intent_test)

add_executable(d48_door_packet_test d48_door_packet_test.cpp)
add_test(NAME d48_door_packet COMMAND d48_door_packet_test)

# D48: execute the current production arm/FK/publication and SetIK observer
# bodies. Extraction is into this build tree, so no stale source-side fixture
# or previous CMake run can satisfy the generated include dependency.
find_package(Python3 COMPONENTS Interpreter REQUIRED)
set(VRIK_D47_GENERATED_DIR "${CMAKE_CURRENT_BINARY_DIR}/generated_vrik_d47")
# Generated includes/provenance must follow incremental production edits too.
set_property(DIRECTORY APPEND PROPERTY CMAKE_CONFIGURE_DEPENDS
 "${CMAKE_CURRENT_SOURCE_DIR}/../tools/extract_vrik_arm_d47.py"
 "${CMAKE_CURRENT_SOURCE_DIR}/../tools/source_text.py")
foreach(vrik_d47_source
 src/frik_pose_probe.cpp
 src/vrik/coherent_pose.inl
 src/subsystems/vrik/pose_anatomy.inl
 src/subsystems/vrik/pose_reach_input.inl
 src/subsystems/vrik/pose_fingers.inl
 src/subsystems/vrik/pose_leg_solver.inl
 src/subsystems/vrik/pose_arm_solver.inl
 src/subsystems/vrik/native_ik_observer.inl
 src/vrik/elbow_continuity.h
 src/vrik/frik_elbow_lab.h
 src/vrik/elbow_shoulder_d45.h
 src/vrik/pose_math.h
 src/vrik/arm_constraints.h
 src/vrik/upper_body.h
 src/vrik/visual_selection59c7f.h
 src/vrik/shoulder_reach_d38.h)
 set_property(DIRECTORY APPEND PROPERTY CMAKE_CONFIGURE_DEPENDS
  "${CMAKE_CURRENT_SOURCE_DIR}/../${vrik_d47_source}")
endforeach()
execute_process(
 COMMAND "${Python3_EXECUTABLE}" "${CMAKE_CURRENT_SOURCE_DIR}/../tools/extract_vrik_arm_d47.py"
         --out "${VRIK_D47_GENERATED_DIR}"
 RESULT_VARIABLE VRIK_D47_EXTRACT_RESULT)
if(NOT VRIK_D47_EXTRACT_RESULT EQUAL 0)
 message(FATAL_ERROR "Could not extract current production VRIK D47/D48 adapters")
endif()
foreach(target vrik_arm_adapter_d47_test vrik_native_observer_d47_test d48_fist_mask_test)
 add_executable(${target} ${target}.cpp)
 target_compile_features(${target} PRIVATE cxx_std_23)
 target_include_directories(${target} PRIVATE "${VRIK_D47_GENERATED_DIR}" "${CMAKE_CURRENT_SOURCE_DIR}/../src")
 target_link_libraries(${target} PRIVATE Threads::Threads)
 add_test(NAME ${target} COMMAND ${target})
endforeach()

# D49/D50: register the reviewed publication, input and ownership fixtures.
# Fresh extraction belongs to the build tree and follows production edits.
set(D50_GENERATED_DIR "${CMAKE_CURRENT_BINARY_DIR}/generated_d50")
foreach(extractor pose_publication_d49 pose_reader_d50 prompt_callbacks_d50)
 set_property(DIRECTORY APPEND PROPERTY CMAKE_CONFIGURE_DEPENDS
  "${CMAKE_CURRENT_SOURCE_DIR}/../tools/extract_${extractor}.py")
 execute_process(
  COMMAND "${Python3_EXECUTABLE}" "${CMAKE_CURRENT_SOURCE_DIR}/../tools/extract_${extractor}.py"
          --source "${CMAKE_CURRENT_SOURCE_DIR}/.." --out "${D50_GENERATED_DIR}"
  RESULT_VARIABLE D50_EXTRACT_RESULT)
 if(NOT D50_EXTRACT_RESULT EQUAL 0)
  message(FATAL_ERROR "Could not extract current ${extractor} production code")
 endif()
endforeach()
set_property(DIRECTORY APPEND PROPERTY CMAKE_CONFIGURE_DEPENDS
 "${CMAKE_CURRENT_SOURCE_DIR}/../src/vrik/coherent_pose.inl"
 "${CMAKE_CURRENT_SOURCE_DIR}/../src/frik_pose_probe.cpp"
 "${CMAKE_CURRENT_SOURCE_DIR}/../src/subsystems/hud_ui_controls/runtime/prompt_device_lock.cpp")
foreach(target pose_publication_d49_test pose_reader_d50_test prompt_callbacks_d50_test
 diagnostic_routes_d49_test prompt_family_d49_test fist_stroke_d49_test
 door_holster_query_d49_test riding_ownership_d50_test)
 add_executable(${target} ${target}.cpp)
 target_compile_features(${target} PRIVATE cxx_std_23)
 target_include_directories(${target} PRIVATE "${D50_GENERATED_DIR}" ../src syntax_shims openxr_sdk)
 target_link_libraries(${target} PRIVATE Threads::Threads)
 if(NOT MSVC)
  target_compile_options(${target} PRIVATE -fms-extensions -ffunction-sections -fdata-sections)
  target_compile_definitions(${target} PRIVATE _ReturnAddress=HostReturnAddress __fastcall=)
  target_link_options(${target} PRIVATE -Wl,--gc-sections)
 else()
  target_compile_options(${target} PRIVATE /Gy)
  target_compile_definitions(${target} PRIVATE _ReturnAddress=HostReturnAddress)
  target_link_options(${target} PRIVATE /OPT:REF)
 endif()
 add_test(NAME ${target} COMMAND ${target})
endforeach()
add_executable(work_cadence_d49_test subsystems/alchemy/work_cadence_d49_test.cpp)
target_compile_features(work_cadence_d49_test PRIVATE cxx_std_20)
target_include_directories(work_cadence_d49_test PRIVATE ../src syntax_shims)
target_link_libraries(work_cadence_d49_test PRIVATE Threads::Threads)
add_test(NAME work_cadence_d49 COMMAND work_cadence_d49_test)

# The complete native owner fixture uses explicit host leaves. Its exception
# translation exercises normal flow only, never Windows SEH/access violations.
add_executable(alchemy_input_supersession_d50_test
 alchemy_input_supersession_d50_test.cpp crafting_native_stubs_d47.cpp)
target_compile_features(alchemy_input_supersession_d50_test PRIVATE cxx_std_20)
target_include_directories(alchemy_input_supersession_d50_test PRIVATE syntax_shims ../src .)
target_link_libraries(alchemy_input_supersession_d50_test PRIVATE Threads::Threads)
target_compile_definitions(alchemy_input_supersession_d50_test PRIVATE "__finally=catch(...) { throw; } if(true)")
if(MSVC)
 target_compile_options(alchemy_input_supersession_d50_test PRIVATE /Gy "/FI${CMAKE_CURRENT_SOURCE_DIR}/syntax_shims/windows.h")
 target_link_options(alchemy_input_supersession_d50_test PRIVATE /OPT:REF)
else()
 target_compile_options(alchemy_input_supersession_d50_test PRIVATE -fms-extensions -ffunction-sections -fdata-sections
  -include "${CMAKE_CURRENT_SOURCE_DIR}/syntax_shims/windows.h")
 target_link_options(alchemy_input_supersession_d50_test PRIVATE -Wl,--gc-sections)
endif()
add_test(NAME alchemy_input_supersession_d50 COMMAND alchemy_input_supersession_d50_test)

# D51: actual living-contact suspension and retired global-CVar service.
# Extraction is build-local; no stored generated fixture can replace current code.
set(D51_GENERATED_DIR "${CMAKE_CURRENT_BINARY_DIR}/generated_d51")
foreach(input tools/run_living_reaction_d51.py tests/native_touch_h9_test.cpp
 src/mod.cpp src/subsystems/physics/host.inl src/vrik/touch_react43.inl
 src/vrik/lab58.h src/vrik/lab58.inl src/diagnostics/capture168.h src/ragdoll_lease59.h)
 set_property(DIRECTORY APPEND PROPERTY CMAKE_CONFIGURE_DEPENDS
  "${CMAKE_CURRENT_SOURCE_DIR}/../${input}")
endforeach()
execute_process(
 COMMAND "${Python3_EXECUTABLE}" "${CMAKE_CURRENT_SOURCE_DIR}/../tools/run_living_reaction_d51.py"
  --source "${CMAKE_CURRENT_SOURCE_DIR}/.." --out "${D51_GENERATED_DIR}" --extract-only
 RESULT_VARIABLE D51_LIVING_EXTRACT_RESULT)
if(NOT D51_LIVING_EXTRACT_RESULT EQUAL 0)
 message(FATAL_ERROR "Could not extract current D51 living-reaction adapters")
endif()
foreach(target living_reaction_retirement_d51_test living_touch_suspension_d51_test
 living_reaction_service_d51_test bellows_pull_d51_test)
 add_executable(${target} ${target}.cpp)
 target_compile_features(${target} PRIVATE cxx_std_20)
 target_include_directories(${target} PRIVATE "${D51_GENERATED_DIR}" ../src syntax_shims openxr_sdk)
 target_link_libraries(${target} PRIVATE Threads::Threads)
 add_test(NAME ${target} COMMAND ${target})
endforeach()

# D51: selected native combat backups, executing complete callback bodies and
# the real animation-filter translation unit with explicit host native IO.
foreach(input tools/extract_combat_backups_d51.py
 src/subsystems/masterstrike_combos/physical_finisher_gate_cb1.inl
 src/vrik/master_gate47.inl src/vrik/native_bridges168.inl
 src/vrik/native_attack_clock50c2.inl src/vrik/native_input117.inl)
 set_property(DIRECTORY APPEND PROPERTY CMAKE_CONFIGURE_DEPENDS
  "${CMAKE_CURRENT_SOURCE_DIR}/../${input}")
endforeach()
execute_process(
 COMMAND "${Python3_EXECUTABLE}" "${CMAKE_CURRENT_SOURCE_DIR}/../tools/extract_combat_backups_d51.py"
  --source "${CMAKE_CURRENT_SOURCE_DIR}/.." --out "${D51_GENERATED_DIR}"
 RESULT_VARIABLE D51_COMBAT_EXTRACT_RESULT)
if(NOT D51_COMBAT_EXTRACT_RESULT EQUAL 0)
 message(FATAL_ERROR "Could not extract current D51 combat callbacks")
endif()
foreach(target native_paired_playback_d51_test finisher_backups_d51_test native_master_backups_d51_test combo_physical_route_d62_test)
 add_executable(${target} ${target}.cpp)
 target_compile_features(${target} PRIVATE cxx_std_23)
 target_include_directories(${target} PRIVATE "${D51_GENERATED_DIR}" ../src syntax_shims openxr_sdk)
 target_link_libraries(${target} PRIVATE Threads::Threads)
 if(NOT MSVC)
  target_compile_options(${target} PRIVATE -fms-extensions -ffunction-sections -fdata-sections)
  target_compile_definitions(${target} PRIVATE _ReturnAddress=HostReturnAddress __fastcall=)
  target_link_options(${target} PRIVATE -Wl,--gc-sections)
 else()
  target_compile_options(${target} PRIVATE /Gy)
  target_compile_definitions(${target} PRIVATE _ReturnAddress=HostReturnAddress)
  target_link_options(${target} PRIVATE /OPT:REF)
 endif()
 add_test(NAME ${target} COMMAND ${target})
endforeach()

include(d57_gap_tests.cmake)
include(d59_misc_tests.cmake)
include(d59_alchemy_tests.cmake)
include(d59r1_hud_tests.cmake)
include(d59r1_crafting_tests.cmake)

include(d60_root_tests.cmake)
include(d60_crafting_tests.cmake)
include(d62_crafting_tests.cmake)
include(d62_config_tests.cmake)

include(d65_repair_tests.cmake)

include(d75_tests.cmake)

include(d76_tests.cmake)
include(d77_tests.cmake)

include(d80_tests.cmake)

include(d84_tests.cmake)
