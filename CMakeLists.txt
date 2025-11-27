cmake_minimum_required(VERSION 3.16)
project(GameEngine LANGUAGES CXX)

# Configuración global
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# --- Archivos del motor (Engine) ---
# ¡CORREGIDO! Se añade 'src/' a la ruta.
file(GLOB_RECURSE ENGINE_SOURCES CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/src/engine/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/engine/ECS/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/engine/ECS/CMP/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/engine/ECS/Systems/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/engine/ECS/Managers/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/engine/ECS/Entities/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/engine/fachadaGrafica/*.cpp"
)

# --- Archivos del juego/demo (Demo/Game) ---
# ¡CORREGIDO! Se añade 'src/' a la ruta.
file(GLOB_RECURSE DEMO_SOURCES CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/src/demo/game/*.cpp"
)

# --- Archivo main ---
# Esta ruta ya estaba correcta
set(MAIN_SRC "${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp")

# --- Crear ejecutable ---
add_executable(GameEngine
    ${MAIN_SRC}
    ${ENGINE_SOURCES}
    ${DEMO_SOURCES}  
)

# --- Directorios de Inclusión (Imprescindible para encontrar los .h) ---
# ¡CORREGIDO! Se añade 'src/' a las rutas de inclusión.
target_include_directories(GameEngine PRIVATE
    # Ruta base
    "${CMAKE_CURRENT_SOURCE_DIR}/src" 
    # Rutas específicas del motor (incluyendo 'src/')
    "${CMAKE_CURRENT_SOURCE_DIR}/src/engine"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/engine/ECS"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/engine/ECS/CMP"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/engine/ECS/Entities"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/engine/ECS/Managers"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/engine/ECS/Systems"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/engine/fachadaGrafica"
    # Ruta de los includes de la demo
    "${CMAKE_CURRENT_SOURCE_DIR}/src/demo/game"
)

# --- Raylib ---
find_package(raylib REQUIRED)
target_link_libraries(GameEngine PRIVATE raylib)
target_include_directories(GameEngine PRIVATE ${RAYLIB_INCLUDE_DIRS})