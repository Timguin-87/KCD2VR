# Exact source guard prefixes + real temporary file I/O, not a native INI parser.
find_package(Python3 COMPONENTS Interpreter REQUIRED)
find_package(Threads REQUIRED)
add_executable(config_file_guard_d62_test config_file_guard_d62_test.cpp)
target_compile_features(config_file_guard_d62_test PRIVATE cxx_std_20)
target_compile_definitions(config_file_guard_d62_test PRIVATE NOMINMAX WIN32_LEAN_AND_MEAN)
target_link_libraries(config_file_guard_d62_test PRIVATE Threads::Threads)
add_test(NAME config_file_guard_d62 COMMAND config_file_guard_d62_test)
set(D62_CONFIG_EXTRACT "${CMAKE_CURRENT_BINARY_DIR}/d62_config/generated_config_guard_d62.inc")
add_custom_command(OUTPUT "${D62_CONFIG_EXTRACT}"
 COMMAND "${Python3_EXECUTABLE}" "${CMAKE_CURRENT_SOURCE_DIR}/../tools/extract_config_guard_d62.py"
  --out "${D62_CONFIG_EXTRACT}"
 DEPENDS ../src/mod.cpp ../src/subsystems/vrik/calibration_runtime.inl
  ../src/core/config_file_guard_d62.h ../tools/extract_config_guard_d62.py
 VERBATIM)
add_executable(config_guard_bridge_d62_test config_guard_bridge_d62_test.cpp "${D62_CONFIG_EXTRACT}")
target_compile_features(config_guard_bridge_d62_test PRIVATE cxx_std_20)
target_compile_definitions(config_guard_bridge_d62_test PRIVATE NOMINMAX WIN32_LEAN_AND_MEAN)
target_include_directories(config_guard_bridge_d62_test PRIVATE "${CMAKE_CURRENT_BINARY_DIR}/d62_config")
target_link_libraries(config_guard_bridge_d62_test PRIVATE Threads::Threads)
add_test(NAME config_guard_bridge_d62 COMMAND config_guard_bridge_d62_test)
set(D62_PENDING_EXTRACT "${CMAKE_CURRENT_BINARY_DIR}/d62_config/generated_pending_setup_d62.inc")
add_custom_command(OUTPUT "${D62_PENDING_EXTRACT}"
 COMMAND "${Python3_EXECUTABLE}" "${CMAKE_CURRENT_SOURCE_DIR}/../tools/extract_pending_setup_d62.py"
  --out "${D62_PENDING_EXTRACT}"
 DEPENDS ../src/subsystems/base/startup_setup_s15.inl ../src/core/config_file_guard_d62.h
  ../tools/extract_pending_setup_d62.py ../tools/extract_config_guard_d62.py
 VERBATIM)
add_executable(pending_setup_guard_d62_test pending_setup_guard_d62_test.cpp "${D62_PENDING_EXTRACT}")
target_compile_features(pending_setup_guard_d62_test PRIVATE cxx_std_20)
target_compile_definitions(pending_setup_guard_d62_test PRIVATE NOMINMAX WIN32_LEAN_AND_MEAN)
target_include_directories(pending_setup_guard_d62_test PRIVATE "${CMAKE_CURRENT_BINARY_DIR}/d62_config")
target_link_libraries(pending_setup_guard_d62_test PRIVATE Threads::Threads)
add_test(NAME pending_setup_guard_d62 COMMAND pending_setup_guard_d62_test)
