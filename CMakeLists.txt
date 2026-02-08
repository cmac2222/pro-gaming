cmake_minimum_required(VERSION 3.15)
project(RustESP)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Source files
set(SOURCES
    main.cpp
    mem.cpp
    overlay.cpp
    game_functions.cpp
)

# Header files
set(HEADERS
    common.h
    mem.h
    overlay.h
    game_functions.h
    config.h
)

# ImGui files (must be downloaded separately)
set(IMGUI_SOURCES
    imgui.cpp
    imgui_demo.cpp
    imgui_draw.cpp
    imgui_tables.cpp
    imgui_widgets.cpp
    imgui_impl_win32.cpp
    imgui_impl_dx9.cpp
)

# Check if ImGui files exist
foreach(IMGUI_FILE ${IMGUI_SOURCES})
    if(NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/${IMGUI_FILE}")
        message(FATAL_ERROR "ImGui file not found: ${IMGUI_FILE}\n"
                           "Please download ImGui from: https://github.com/ocornut/imgui\n"
                           "Required files:\n"
                           "  - imgui/*.cpp and imgui/*.h\n"
                           "  - imgui/backends/imgui_impl_win32.cpp/h\n"
                           "  - imgui/backends/imgui_impl_dx9.cpp/h")
    endif()
endforeach()

# Create executable
add_executable(RustESP 
    ${SOURCES} 
    ${HEADERS}
    ${IMGUI_SOURCES}
)

# Include directories
target_include_directories(RustESP PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
)

# Link Windows libraries
target_link_libraries(RustESP
    d3d9
    dwmapi
)

# Compiler definitions
target_compile_definitions(RustESP PRIVATE
    UNICODE
    _UNICODE
    WIN32_LEAN_AND_MEAN
    NOMINMAX
)

# Compiler flags
if(MSVC)
    # MSVC-specific flags
    target_compile_options(RustESP PRIVATE
        /W4                 # Warning level 4
        /WX-                # Don't treat warnings as errors
        /O2                 # Optimize for speed
        /Oi                 # Enable intrinsic functions
        /GL                 # Whole program optimization
        /arch:SSE2          # Use SSE2 instructions
        /fp:fast            # Fast floating point
    )
    
    # Linker flags for MSVC
    set_target_properties(RustESP PROPERTIES
        LINK_FLAGS "/LTCG"  # Link-time code generation
    )
    
    # Set subsystem to Windows (not console)
    set_target_properties(RustESP PROPERTIES
        WIN32_EXECUTABLE FALSE  # Keep console for debug output
    )
else()
    # GCC/Clang flags
    target_compile_options(RustESP PRIVATE
        -Wall
        -Wextra
        -O2
        -march=native
        -msse2
    )
endif()

# Set output directory
set_target_properties(RustESP PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
    RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR}/bin/Debug
    RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/bin/Release
)

# Print build information
message(STATUS "===========================================")
message(STATUS "Rust ESP Build Configuration")
message(STATUS "===========================================")
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "C++ Standard: ${CMAKE_CXX_STANDARD}")
message(STATUS "Compiler: ${CMAKE_CXX_COMPILER_ID}")
message(STATUS "===========================================")

