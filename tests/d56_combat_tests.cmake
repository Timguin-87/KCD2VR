# Include from a focused host-test configuration after enable_testing().
foreach(d56_combat_case gore_admission_d56 gore_eligibility_d56 gore_physical_d56 combat_finish_d41 guard_parry176 physical_effort_packet19 live_settings_d39 committed_stroke_h8 f9_observability_d56)
  if(NOT TARGET ${d56_combat_case}_test)
    add_executable(${d56_combat_case}_test "${CMAKE_CURRENT_LIST_DIR}/${d56_combat_case}_test.cpp")
    target_include_directories(${d56_combat_case}_test PRIVATE "${CMAKE_CURRENT_LIST_DIR}/../src")
    target_compile_features(${d56_combat_case}_test PRIVATE cxx_std_20)
  endif()
  add_test(NAME d56_combat_${d56_combat_case} COMMAND ${d56_combat_case}_test)
endforeach()
find_package(Python3 COMPONENTS Interpreter REQUIRED)
foreach(d56_combat_lua gore_eligibility_d56 gore_d52_lua gore_presentation_d54 gore_material_wrappers_d54)
  add_test(NAME d56_combat_${d56_combat_lua}_lua
    COMMAND "${Python3_EXECUTABLE}" "${CMAKE_CURRENT_LIST_DIR}/test_${d56_combat_lua}.py")
endforeach()
