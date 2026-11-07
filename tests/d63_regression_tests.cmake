# D63 regressions execute current adapters. Native Windows linking is a separate
# root build; these host fixtures do not establish in-game/headset acceptance.
include("${CMAKE_CURRENT_SOURCE_DIR}/d59r1_crafting_tests.cmake")
add_executable(alchemy_acquisition_d63_test subsystems/alchemy/acquisition_d63_test.cpp)
target_compile_features(alchemy_acquisition_d63_test PRIVATE cxx_std_20)
target_compile_definitions(alchemy_acquisition_d63_test PRIVATE
  "D59R1_SELECTION_ADAPTER=\"${D59R1_CRAFTING_GENERATED}/production_selection_adapter.inc\"")
if(MSVC)
  target_compile_options(alchemy_acquisition_d63_test PRIVATE /UNDEBUG)
else()
  target_compile_options(alchemy_acquisition_d63_test PRIVATE -UNDEBUG)
endif()
add_test(NAME alchemy_acquisition_d63 COMMAND alchemy_acquisition_d63_test)

add_executable(washing_d63_test washing_d59_test.cpp)
target_compile_features(washing_d63_test PRIVATE cxx_std_20)
add_test(NAME washing_d63 COMMAND washing_d63_test)

# D63R1: repaired current mixed-contact completion and exact attachment math.
foreach(repair contact_mixed_hit_d63 stowed_render_pose_d63)
  add_executable(${repair}_test ${repair}_test.cpp)
  target_compile_features(${repair}_test PRIVATE cxx_std_20)
  add_test(NAME ${repair} COMMAND ${repair}_test)
endforeach()
if(MSVC)
  # Uses real Windows SEH syntax, with explicitly modeled native engine leaves.
  add_executable(stowed_render_native_d63_test stowed_render_native_d63_test.cpp)
  target_compile_features(stowed_render_native_d63_test PRIVATE cxx_std_20)
  add_test(NAME stowed_render_native_d63 COMMAND stowed_render_native_d63_test)
endif()

if(NOT WIN32)
  foreach(lane combat_regressions contact_continuity stowed_entities weapon_exit)
    add_test(NAME ${lane}_d63_production COMMAND "${Python3_EXECUTABLE}"
      "${CMAKE_CURRENT_SOURCE_DIR}/../tools/run_${lane}_d63.py"
      --out "${CMAKE_CURRENT_BINARY_DIR}/d63_${lane}")
    set_tests_properties(${lane}_d63_production PROPERTIES RUN_SERIAL TRUE)
  endforeach()
endif()
