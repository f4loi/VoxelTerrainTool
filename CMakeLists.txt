cmake_minimum_required(VERSION 3.16)
project(VoxelEngine CXX)

# --- FIX: Evita el error de compatibilidad con versiones antiguas en Raylib ---
set(CMAKE_POLICY_VERSION_MINIMUM 3.5 CACHE STRING "Bypass para Raylib" FORCE)

# Exigir C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include(FetchContent)

# 1. Integrar Raylib
FetchContent_Declare(
    raylib
    URL https://github.com/raysan5/raylib/archive/refs/heads/master.tar.gz
)
FetchContent_MakeAvailable(raylib)

# 2. Descargar código fuente de ImGui
FetchContent_Declare(
    imgui
    GIT_REPOSITORY https://github.com/ocornut/imgui.git
    GIT_TAG master
)
FetchContent_GetProperties(imgui)
if(NOT imgui_POPULATED)
    FetchContent_Populate(imgui)
endif()

# 3. Descargar código fuente de rlImGui
FetchContent_Declare(
    rlImGui
    GIT_REPOSITORY https://github.com/raylib-extras/rlImGui.git
    GIT_TAG main
)
FetchContent_GetProperties(rlImGui)
if(NOT rlimgui_POPULATED)
    FetchContent_Populate(rlImGui)
endif()

# 4. Recopilar TUS archivos fuente (.cpp) dentro de src
file(GLOB_RECURSE SOURCES "src/*.cpp")

# 5. Especificar los archivos .cpp de ImGui y rlImGui
set(IMGUI_SOURCES
    ${imgui_SOURCE_DIR}/imgui.cpp
    ${imgui_SOURCE_DIR}/imgui_demo.cpp
    ${imgui_SOURCE_DIR}/imgui_draw.cpp
    ${imgui_SOURCE_DIR}/imgui_tables.cpp
    ${imgui_SOURCE_DIR}/imgui_widgets.cpp
    ${rlimgui_SOURCE_DIR}/rlImGui.cpp
)

# 6. Crear el ejecutable combinando TU código y el código de ImGui
add_executable(${PROJECT_NAME} ${SOURCES} ${IMGUI_SOURCES})

# 7. Decirle al compilador dónde están los .h
target_include_directories(${PROJECT_NAME} PRIVATE 
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${imgui_SOURCE_DIR}
    ${rlimgui_SOURCE_DIR}
)

# 8. Enlazar SOLAMENTE Raylib (ImGui ya está dentro del ejecutable)
target_link_libraries(${PROJECT_NAME} PRIVATE raylib)