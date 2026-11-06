# The gameplay-only seed is immutable. Direct CMake
# builds must apply the same explicit source inputs as build_release.ps1.
set(KCD2VR_COMPANION_SOURCE_ROOT "" CACHE PATH "Prepared companion override; blank prepares current source automatically")
set(KCD2VR_OPTIONAL_GORE_SOURCE "" CACHE PATH "Optional standalone adult package ZIP/root; blank builds gameplay only")
if(NOT KCD2VR_COMPANION_SOURCE_ROOT OR KCD2VR_COMPANION_SOURCE_ROOT STREQUAL "${CMAKE_SOURCE_DIR}/companion/C63_UNIFIED")
    if(NOT Python3_EXECUTABLE)
        message(FATAL_ERROR "Python3 is required to prepare the current companion")
    endif()
    set(KCD2VR_COMPANION_SOURCE_ROOT "${CMAKE_BINARY_DIR}/d39_companion")
    # Observe both edits and newly generated sheath model files. No game path
    # is involved: this writes only a separate package in the build directory.
    file(GLOB_RECURSE KCD2VR_COMPANION_INPUTS_D39 CONFIGURE_DEPENDS
        "${CMAKE_SOURCE_DIR}/companion/source/*"
        "${CMAKE_SOURCE_DIR}/companion/BASE_GAMEPLAY/*"
        "${CMAKE_SOURCE_DIR}/companion/Mods/kcd2vr_lab/*"
        "${CMAKE_SOURCE_DIR}/mods/KCD2VRMap/*"
        "${CMAKE_SOURCE_DIR}/mods/KCD2VRDoors/*"
        "${CMAKE_SOURCE_DIR}/mods/KCD2VRStow/*"
        "${CMAKE_SOURCE_DIR}/mods/kcd2vr_tests/*"
        "${CMAKE_SOURCE_DIR}/src/subsystems/throw_drop/resources/*")
    set_property(DIRECTORY APPEND PROPERTY CMAKE_CONFIGURE_DEPENDS
        ${KCD2VR_COMPANION_INPUTS_D39}
        "${CMAKE_SOURCE_DIR}/src/build_identity.h"
        "${CMAKE_SOURCE_DIR}/tools/optional_gore_d74.py"
        "${CMAKE_SOURCE_DIR}/tools/optional_gore_r93.py"
        "${CMAKE_SOURCE_DIR}/tools/package_release_helpers_r19.py"
        "${CMAKE_SOURCE_DIR}/tools/companion_sources_d39.py")
    set(optional_content_args)
    if(KCD2VR_OPTIONAL_GORE_SOURCE)
        set_property(DIRECTORY APPEND PROPERTY CMAKE_CONFIGURE_DEPENDS
            "${CMAKE_SOURCE_DIR}/mods/KCD2VRGore/r93r3/zzkcd2vr_gore.lua"
            "${CMAKE_SOURCE_DIR}/mods/KCD2VRGore/r93r3/D77_DONOR.json")
        list(APPEND optional_content_args --optional-gore-source "${KCD2VR_OPTIONAL_GORE_SOURCE}")
        if(IS_DIRECTORY "${KCD2VR_OPTIONAL_GORE_SOURCE}")
            file(GLOB_RECURSE optional_inputs CONFIGURE_DEPENDS "${KCD2VR_OPTIONAL_GORE_SOURCE}/*")
        else()
            set(optional_inputs "${KCD2VR_OPTIONAL_GORE_SOURCE}")
        endif()
        set_property(DIRECTORY APPEND PROPERTY CMAKE_CONFIGURE_DEPENDS ${optional_inputs})
    endif()
    execute_process(COMMAND "${Python3_EXECUTABLE}"
        "${CMAKE_SOURCE_DIR}/tools/package_release_helpers_r19.py"
        --source "${CMAKE_SOURCE_DIR}" --prepare-companion
        --out "${KCD2VR_COMPANION_SOURCE_ROOT}" ${optional_content_args}
        COMMAND_ERROR_IS_FATAL ANY)
endif()
