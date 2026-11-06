# Production notification transaction/registry. The existing hud_writer175_bridge
# target separately executes the full FlashUI translation-unit native detours.
add_executable(hud_notifications_d59r1_test hud_notifications_d59r1_test.cpp)
target_compile_features(hud_notifications_d59r1_test PRIVATE cxx_std_20)
target_include_directories(hud_notifications_d59r1_test PRIVATE ../src)
target_link_libraries(hud_notifications_d59r1_test PRIVATE Threads::Threads)
add_test(NAME hud_notifications_d59r1 COMMAND hud_notifications_d59r1_test)

# P1: same current production owners with per-child work and capture-only costs.
foreach(name hud_work_p1 hud_phase_cost_p1)
 add_executable(${name}_test ${name}_test.cpp)
 target_compile_features(${name}_test PRIVATE cxx_std_20)
 target_include_directories(${name}_test PRIVATE ui175/mocks syntax_shims openxr_sdk ../src)
 target_link_libraries(${name}_test PRIVATE Threads::Threads)
 if(MSVC)
  target_compile_options(${name}_test PRIVATE /Gy /Gw /bigobj)
  target_link_options(${name}_test PRIVATE /OPT:REF /INCREMENTAL:NO)
 else()
  target_compile_options(${name}_test PRIVATE -fms-extensions -ffunction-sections -fdata-sections -D_ReturnAddress=HostReturnAddress)
  target_link_options(${name}_test PRIVATE -Wl,--gc-sections)
 endif()
 add_test(NAME ${name} COMMAND ${name}_test)
endforeach()
