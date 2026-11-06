# D60: production smith adapter boundary tests; no native engine execution.
add_executable(smith_contact_owner_d60_test smith_contact_owner_d60_test.cpp)
target_compile_features(smith_contact_owner_d60_test PRIVATE cxx_std_20)
target_include_directories(smith_contact_owner_d60_test PRIVATE ../src)
add_test(NAME smith_contact_owner_d60 COMMAND smith_contact_owner_d60_test)
# Complete native-owner TU uses the project's MSVC SEH scopes. The test retains
# native SEH syntax; platform/native services are explicitly substituted leaves.
if(MSVC)
  add_executable(alchemy_resource_receipt_d60_test alchemy_resource_receipt_d60_test.cpp crafting_native_stubs_d47.cpp)
  target_compile_features(alchemy_resource_receipt_d60_test PRIVATE cxx_std_20)
  target_include_directories(alchemy_resource_receipt_d60_test PRIVATE syntax_shims ../src)
  target_compile_options(alchemy_resource_receipt_d60_test PRIVATE /Gy /Gw /bigobj /UNDEBUG)
  target_link_options(alchemy_resource_receipt_d60_test PRIVATE /OPT:REF)
  target_link_libraries(alchemy_resource_receipt_d60_test PRIVATE Threads::Threads)
  add_test(NAME alchemy_resource_receipt_d60 COMMAND alchemy_resource_receipt_d60_test)
endif()
