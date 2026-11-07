# The retained Windows writer fixture now exercises the actual shared process logger.
# Keep the reviewed dependency in build output, exactly as the main DLL build does.
if(WIN32)
 if(NOT TARGET DetourModKit::DetourModKit)
  set(KCD2VR_TEST_DMK_D81 "${CMAKE_CURRENT_BINARY_DIR}/_deps/DetourModKit")
  execute_process(COMMAND "${Python3_EXECUTABLE}"
   "${CMAKE_CURRENT_SOURCE_DIR}/../tools/prepare_dmk_dependency.py"
   --package "${CMAKE_CURRENT_SOURCE_DIR}/../deps/DetourModKit"
   --out "${KCD2VR_TEST_DMK_D81}" COMMAND_ERROR_IS_FATAL ANY)
  set(DetourModKit_DIR "${KCD2VR_TEST_DMK_D81}/lib/cmake/DetourModKit")
  find_package(DetourModKit CONFIG REQUIRED)
 endif()
 target_compile_features(diagnostic_health_c54r3_test PRIVATE cxx_std_23)
 target_link_libraries(diagnostic_health_c54r3_test PRIVATE DetourModKit::DetourModKit)
endif()
