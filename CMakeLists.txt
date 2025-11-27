cmake_minimum_required(VERSION 3.16)
project(GameEngine LANGUAGES CXX)

# Configuración global
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# --- Archivos del motor ---
# Meter aqui todos los archivos del motor
file(GLOB_RECURSE ENGINE_SOURCES CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/src/engine/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/engine/ECS/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/engine/ECS/CMP/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/engine/ECS/Systems/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/engine/ECS/Managers/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/engine/ECS/Entities/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/engine/fachadaGrafica/*.cpp"


)

# --- Archivos de la demo ---
# file(GLOB_RECURSE DEMO_SOURCES CONFIGURE_DEPENDS
#     "${CMAKE_CURRENT_SOURCE_DIR}/src/demo/*.cpp"
# )

# --- Archivo main ---
set(MAIN_SRC "${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp")

# --- Crear ejecutable ---
add_executable(GameEngine
    ${MAIN_SRC}
    ${ENGINE_SOURCES}
    #${DEMO_SOURCES}
)

# --- Raylib ---
find_package(raylib REQUIRED)
target_link_libraries(GameEngine PRIVATE raylib)
target_include_directories(GameEngine PRIVATE ${RAYLIB_INCLUDE_DIRS})
