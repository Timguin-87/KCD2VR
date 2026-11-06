# D59R1: test the actual current adapter, not a stored reconstruction.
set(D59R1_CRAFTING_GENERATED "${CMAKE_CURRENT_BINARY_DIR}/generated_d59r1_crafting")
foreach(input tools/run_crafting_d59r1.py src/vrik/semi_workstations46.h)
  set_property(DIRECTORY APPEND PROPERTY CMAKE_CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/../${input}")
endforeach()
execute_process(
  COMMAND "${Python3_EXECUTABLE}" "${CMAKE_CURRENT_SOURCE_DIR}/../tools/run_crafting_d59r1.py"
    --out "${D59R1_CRAFTING_GENERATED}" --extract-only
  RESULT_VARIABLE D59R1_CRAFTING_EXTRACT_RESULT)
if(NOT D59R1_CRAFTING_EXTRACT_RESULT EQUAL 0)
  message(FATAL_ERROR "Could not extract the current D59R1 alchemy selection adapter")
endif()

add_executable(alchemy_selection_stamp_d59r1_test subsystems/alchemy/selection_stamp_d59r1_test.cpp)
target_compile_features(alchemy_selection_stamp_d59r1_test PRIVATE cxx_std_20)
target_include_directories(alchemy_selection_stamp_d59r1_test PRIVATE ../src)
target_compile_definitions(alchemy_selection_stamp_d59r1_test PRIVATE
  "D59R1_SELECTION_ADAPTER=\"${D59R1_CRAFTING_GENERATED}/production_selection_adapter.inc\"")
# The reused production-input fixture uses assert; keep its checks in Release.
if(MSVC)
  target_compile_options(alchemy_selection_stamp_d59r1_test PRIVATE /UNDEBUG)
else()
  target_compile_options(alchemy_selection_stamp_d59r1_test PRIVATE -UNDEBUG)
endif()
add_test(NAME alchemy_selection_stamp_d59r1 COMMAND alchemy_selection_stamp_d59r1_test)

add_executable(smith_native_height_d59r1_test smith_native_height_d59r1_test.cpp)
target_compile_features(smith_native_height_d59r1_test PRIVATE cxx_std_20)
target_include_directories(smith_native_height_d59r1_test PRIVATE ../src)
add_test(NAME smith_native_height_d59r1 COMMAND smith_native_height_d59r1_test)

add_executable(smith_stroke_wiring_d59r1_test smith_stroke_wiring_d57_test.cpp)
target_compile_features(smith_stroke_wiring_d59r1_test PRIVATE cxx_std_20)
target_include_directories(smith_stroke_wiring_d59r1_test PRIVATE ../src)
add_test(NAME smith_stroke_wiring_d59r1 COMMAND smith_stroke_wiring_d59r1_test)
