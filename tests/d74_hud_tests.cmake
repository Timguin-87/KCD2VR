# Exact current prompt service plus finite diagnostic mailbox ownership.
execute_process(COMMAND "${Python3_EXECUTABLE}" "${CMAKE_CURRENT_SOURCE_DIR}/../tools/extract_station_prompt_d74.py"
 RESULT_VARIABLE D74_PROMPT_EXTRACT_RESULT)
if(NOT D74_PROMPT_EXTRACT_RESULT EQUAL 0)
 message(FATAL_ERROR "Could not extract current D74 station prompt service")
endif()
execute_process(COMMAND "${Python3_EXECUTABLE}" "${CMAKE_CURRENT_SOURCE_DIR}/../tools/extract_gold_callers_d74.py"
 RESULT_VARIABLE D74_GOLD_EXTRACT_RESULT)
if(NOT D74_GOLD_EXTRACT_RESULT EQUAL 0)
 message(FATAL_ERROR "Could not extract current D74 diagnostic display callers")
endif()
foreach(suite hud_station_prompt_d74 diagnostic_message_d74 visible_failure)
 add_executable(${suite}_test ${suite}_test.cpp)
 target_compile_features(${suite}_test PRIVATE cxx_std_20)
 target_include_directories(${suite}_test PRIVATE ../src .)
 add_test(NAME ${suite} COMMAND ${suite}_test)
endforeach()
