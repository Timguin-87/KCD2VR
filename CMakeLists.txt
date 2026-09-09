cmake_minimum_required(VERSION 3.20)
project(KCD2VR LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# --- DetourModKit (AOB scanning, hook manager, config, logger) ---
# Uses the prebuilt MSVC package (DetourModKit_MSVC_vX.Y.Z.zip from their GitHub
# Releases page), NOT a from-source submodule build - far fewer things can go
# wrong for a first build. Extract that zip somewhere and point CMAKE_PREFIX_PATH
# at it when configuring (see README step-by-step).
find_package(DetourModKit CONFIG REQUIRED)

# --- OpenXR loader ---
# V62.9: vcpkg is optional. Reuse an installed OpenXR package if present;
# otherwise CMake downloads/builds Khronos' loader automatically.
find_package(OpenXR CONFIG QUIET)
if(NOT TARGET OpenXR::openxr_loader)
    include(FetchContent)
    set(BUILD_TESTS OFF CACHE BOOL "" FORCE)
    set(BUILD_API_LAYERS OFF CACHE BOOL "" FORCE)
    FetchContent_Declare(
        openxr_sdk_source
        URL https://github.com/KhronosGroup/OpenXR-SDK-Source/archive/refs/heads/main.zip
        DOWNLOAD_EXTRACT_TIMESTAMP TRUE
    )
    FetchContent_MakeAvailable(openxr_sdk_source)
    if(TARGET openxr_loader AND NOT TARGET OpenXR::openxr_loader)
        add_library(OpenXR::openxr_loader ALIAS openxr_loader)
    endif()
endif()
if(NOT TARGET OpenXR::openxr_loader)
    message(FATAL_ERROR "OpenXR loader unavailable. Check network access or provide an installed OpenXR package.")
endif()

# V41.1 sanity check: frame-locked stereo requires this translation unit.
if(NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/src/frame_sync.cpp")
    message(FATAL_ERROR "V46.3: src/frame_sync.cpp is missing; extract the complete source package.")
endif()
if(NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/src/recursive_view_probe.cpp")
    message(FATAL_ERROR "V46.3: src/recursive_view_probe.cpp is missing; extract the complete source package.")
endif()

add_library(KCD2VR SHARED
    src/dllmain.cpp
    src/mod.cpp
    src/camera_hook.cpp
    src/frame_sync.cpp
    src/recursive_view_probe.cpp
    src/device_hook.cpp
    src/depth_probe.cpp
    src/vr_menu.cpp
    src/hud_xml_mod.cpp
    src/xr_manager.cpp
    src/xr_input.cpp
    src/touch_bindings.cpp
    src/vrik_bridge.cpp
    src/frik_pose_probe.cpp
    src/flashui_probe.cpp
    src/debug_trace.cpp
    src/diagnostic_trace.cpp
)

target_include_directories(KCD2VR PRIVATE src)

target_link_libraries(KCD2VR PRIVATE
    DetourModKit::DetourModKit
    OpenXR::openxr_loader
    d3d12
    dxgi
    dxguid
    d3dcompiler
    comctl32
    gdi32
    user32
    winmm
)

# Build as KCD2VR.asi, not .dll — the ASI loader looks for this extension
set_target_properties(KCD2VR PROPERTIES
    OUTPUT_NAME "KCD2VR"
    SUFFIX ".asi"
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/release"
)

# Keep the supplied book art beside the built ASI so users do not need a
# separate manual conversion/copy step after building.
add_custom_command(TARGET KCD2VR POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${CMAKE_CURRENT_SOURCE_DIR}/MIS_MENU_BOOK.bmp"
        "$<TARGET_FILE_DIR:KCD2VR>/MIS_MENU_BOOK.bmp"
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${CMAKE_CURRENT_SOURCE_DIR}/KCD2VR.ini"
        "$<TARGET_FILE_DIR:KCD2VR>/KCD2VR.ini"
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${CMAKE_CURRENT_SOURCE_DIR}/OPTIONAL_UI_MODS/KCD2VR_UI_LIVE/KCD2VR_UI_LIVE.pak"
        "$<TARGET_FILE_DIR:KCD2VR>/KCD2VR_UI_LIVE.pak"
)

target_compile_definitions(KCD2VR PRIVATE
    WIN32_LEAN_AND_MEAN
    NOMINMAX
)
