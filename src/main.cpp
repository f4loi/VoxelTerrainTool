#include <raylib.h>
#include "VoxelEngine/Managers/EntityManagerMeta.h"
#include "VoxelEngine/Systems/PhysicsSystemMeta.h"
#include "VoxelEngine/Systems/RenderSystemMeta.h"

int main() {
    // 1. Inicializar Raylib
    InitWindow(800, 600, "Biome Voxel Engine ECS");
    SetTargetFPS(60);

    // 2. Instanciar el Manager y los Sistemas
    EntityManagerMeta em;
    PhysicsSystemMeta physicsSys;
    RenderSystemMeta renderSys;

    renderSys.Init();

    // 3. Crear una entidad de prueba para probar la metaprogramación
    EntityMeta player = em.createEntity();
    
    // Le añadimos el componente de físicas (Posición X, Y, Velocidad X)
    em.addComponent(player.getNextId(), PhysicsCMP{10.0f, 300.0f, 5.0f});
    
    // Le añadimos el componente de renderizado
    em.addComponent(player.getNextId(), RenderCMP{true});

    // 4. Bucle principal del juego
    while (!renderSys.WindowShouldClose()) {
        // Actualizar físicas (el bloque se moverá hacia la derecha y se reseteará al pasar 800)
        physicsSys.Update(em);
        
        // Dibujar en pantalla
        renderSys.Update(em);
    }

    // 5. Limpieza
    CloseWindow();
    return 0;
}