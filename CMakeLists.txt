cmake_minimum_required(VERSION 3.16)
project(GameEngine LANGUAGES CXX)

# Configuración global
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Flags
set(CMAKE_CXX_FLAGS_DEBUG "-g -O0")
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -DNDEBUG")

# --- Archivos fuente ---
file(GLOB_RECURSE ENGINE_SOURCES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/src/engine/*.cpp")
file(GLOB_RECURSE DEMO_SOURCES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/src/demo/game/*.cpp")


file(GLOB_RECURSE UTILS_SOURCES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/src/utils/*.cpp")

set(MAIN_SRC "${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp")

# --- Raylib ---
find_package(raylib REQUIRED)

# --- Crear ejecutable ---
add_executable(GameEngine
    ${MAIN_SRC}
    ${ENGINE_SOURCES}
    ${DEMO_SOURCES}
    ${UTILS_SOURCES}  
)

target_include_directories(GameEngine PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/src"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/utils"
    ${RAYLIB_INCLUDE_DIRS}
)

target_link_libraries(GameEngine PRIVATE raylib)