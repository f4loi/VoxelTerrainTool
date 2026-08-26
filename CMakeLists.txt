cmake_minimum_required(VERSION 3.16)
project(VoxelEngine CXX)

# =====================================================================
#                    CORE COMPILER SETTINGS
# =====================================================================
# Bypass old CMake policy warnings occasionally triggered by Raylib
set(CMAKE_POLICY_VERSION_MINIMUM 3.5 CACHE STRING "Bypass for Raylib" FORCE)

# Enforce C++17 Standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# QoL: Set 'Release' as the default build type if none is specified. 
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    set(CMAKE_BUILD_TYPE "Release" CACHE STRING "Choose the type of build." FORCE)
endif()

# =====================================================================
#                    DEPENDENCY MANAGEMENT (FETCHCONTENT)
# =====================================================================
include(FetchContent)

# Fetch Raylib (Graphics & Input Framework)
FetchContent_Declare(
    raylib
    GIT_REPOSITORY https://github.com/raysan5/raylib.git
    GIT_TAG master
)

# Fetch ImGui (UI Framework - Docking Branch)
FetchContent_Declare(
    imgui
    GIT_REPOSITORY https://github.com/ocornut/imgui.git
    GIT_TAG docking
)

# Fetch rlImGui (Raylib backend for ImGui)
FetchContent_Declare(
    rlImGui
    GIT_REPOSITORY https://github.com/raylib-extras/rlImGui.git
    GIT_TAG main
)

#Automatically populates and adds subdirectories for all declared targets
FetchContent_MakeAvailable(raylib imgui rlImGui)

# =====================================================================
#                SOURCE CODE COLLECTION
# =====================================================================
# CONFIGURE_DEPENDS ensures CMake re-runs automatically if you add a new .cpp file to /src
file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS "src/*.cpp")

# Define ImGui and rlImGui external source files
set(IMGUI_SOURCES
    ${imgui_SOURCE_DIR}/imgui.cpp
    ${imgui_SOURCE_DIR}/imgui_demo.cpp
    ${imgui_SOURCE_DIR}/imgui_draw.cpp
    ${imgui_SOURCE_DIR}/imgui_tables.cpp
    ${imgui_SOURCE_DIR}/imgui_widgets.cpp
    ${rlimgui_SOURCE_DIR}/rlImGui.cpp
)

# =====================================================================
#                    EXECUTABLE & LINKING
# =====================================================================
# Create the main executable combining engine sources and UI framework sources
add_executable(${PROJECT_NAME} ${SOURCES} ${IMGUI_SOURCES})

# Specify where the compiler should look for header files (.h / .hpp)
target_include_directories(${PROJECT_NAME} PRIVATE 
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${imgui_SOURCE_DIR}
    ${rlimgui_SOURCE_DIR}
)

# Link Raylib to the executable
target_link_libraries(${PROJECT_NAME} PRIVATE raylib)

# =====================================================================
#                        COMPILER OPTIMIZATIONS 
# =====================================================================
# Apply aggressive compiler optimizations for Release builds to maintain stable FPS
if(CMAKE_BUILD_TYPE STREQUAL "Release")
    if(MSVC)
        # Visual Studio flags
        target_compile_options(${PROJECT_NAME} PRIVATE /O2 /Ob2 /Oi /Ot)
    else()
        # GCC / Clang flags
        target_compile_options(${PROJECT_NAME} PRIVATE -O3 -ffast-math)
    endif()
endif()