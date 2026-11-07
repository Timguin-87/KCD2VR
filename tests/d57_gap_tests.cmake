# D57: finite diagnostics and correlation policy, no native/game entry.
foreach(target d57_reload_request_diagnostics_test map_diagnostics_d57_test
               smith_stroke_receipt_d57_test)
    add_executable(${target} ${target}.cpp)
    target_compile_features(${target} PRIVATE cxx_std_20)
    target_include_directories(${target} PRIVATE ../src)
    target_link_libraries(${target} PRIVATE Threads::Threads)
    add_test(NAME ${target} COMMAND ${target})
endforeach()
