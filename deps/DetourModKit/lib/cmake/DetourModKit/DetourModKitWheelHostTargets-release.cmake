#----------------------------------------------------------------
# Generated CMake target import file for configuration "Release".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "DetourModKit::WheelHost" for configuration "Release"
set_property(TARGET DetourModKit::WheelHost APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(DetourModKit::WheelHost PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_RELEASE "CXX"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/lib/libDetourModKitWheelHost.lib"
  )

list(APPEND _cmake_import_check_targets DetourModKit::WheelHost )
list(APPEND _cmake_import_check_files_for_DetourModKit::WheelHost "${_IMPORT_PREFIX}/lib/libDetourModKitWheelHost.lib" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
