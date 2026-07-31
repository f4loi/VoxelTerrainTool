cmake_minimum_required(VERSION 3.16)
project(VoxelEngine CXX)

# --- FIX: Evita el error de compatibilidad con versiones antiguas en Raylib ---
set(CMAKE_POLICY_VERSION_MINIMUM 3.5 CACHE STRING "Bypass para Raylib" FORCE)

# Exigir C++17 para los fold expressions y std::apply
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Configuración para descargar e integrar Raylib automáticamente
include(FetchContent)
FetchContent_Declare(
    raylib
    URL https://github.com/raysan5/raylib/archive/refs/tags/5.0.tar.gz
)
FetchContent_MakeAvailable(raylib)

# Recopilar todos los archivos fuente (.cpp) dentro de src
file(GLOB_RECURSE SOURCES "src/*.cpp")

# Crear el ejecutable principal
add_executable(${PROJECT_NAME} ${SOURCES})

# Hace que todos los #include busquen desde la carpeta src/
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src)

# Enlazar la librería de Raylib con tu motor
target_link_libraries(${PROJECT_NAME} PRIVATE raylib)