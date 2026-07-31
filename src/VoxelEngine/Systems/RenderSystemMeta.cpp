#include "VoxelEngine/Systems/RenderSystemMeta.h"

void RenderSystemMeta::Init() {
    // Si necesitas inicializar algo específico del render puedes hacerlo aquí.
    // Recuerda que InitWindow() de Raylib suele llamarse en el main().
}

void RenderSystemMeta::Update(EntityManagerMeta &em) {
    BeginDrawing();
    ClearBackground(BLACK); // BLACK equivale a {0, 0, 0, 255} en Raylib
    
    em.forAll(&RenderSystemMeta::UpdateOneEntity);
    
    EndDrawing();
}

void RenderSystemMeta::UpdateOneEntity(EntityMeta ent) {
    EntityManagerMeta *manager{ent.getParent()};
    uint16_t id{ent.getNextId()};

    if (manager != nullptr) {
        RenderCMP *render = manager->getComponent<RenderCMP>(id);
        PhysicsCMP *physic = manager->getComponent<PhysicsCMP>(id);

        if (render != nullptr && physic != nullptr && render->isRendered) {
            // Raylib usa DrawRectangle(int posX, int posY, int width, int height, Color color)
            DrawRectangle(
                static_cast<int>(physic->x),
                static_cast<int>(physic->y),
                15, // Ancho
                15, // Alto (asumo 15 basándome en el tamaño que le pasabas a la fachada)
                MAGENTA // Color MAGENTA para la versión de Metaprogramación
            ); 
        }
    }
}

bool RenderSystemMeta::WindowShouldClose() {
    // :: indica al compilador que llame a la función global de Raylib, 
    // no a la de esta propia clase.
    return ::WindowShouldClose();
}