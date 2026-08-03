#include "VoxelEngine/Systems/RenderSystemMeta.h"

void RenderSystemMeta::Init() {
    camera.position = Vector3{ -10.0f, 25.0f, -10.0f };
    camera.target = Vector3{ 8.0f, 0.0f, 8.0f };
    camera.up = Vector3{ 0.0f, 1.0f, 0.0f };
    camera.fovy = 60.0f;
    camera.projection = CAMERA_PERSPECTIVE;
}

void RenderSystemMeta::Update(EntityManagerMeta &em) {
    // Desactivar el movimiento de cámara si no mantenemos clic derecho
    if (IsMouseButtonDown(MOUSE_BUTTON_RIGHT)) {
        DisableCursor();
        UpdateCamera(&camera, CAMERA_FREE);
    } else {
        EnableCursor();
    }

    BeginMode3D(camera);
    em.forAll(&RenderSystemMeta::UpdateOneEntity);
    EndMode3D();
}

// ¡ESTA ES LA FUNCIÓN QUE FALTABA!
void RenderSystemMeta::UpdateOneEntity(EntityMeta ent) {
    EntityManagerMeta *manager{ent.getParent()};
    uint16_t id{ent.getNextId()};

    if (manager != nullptr) {
        ChunkCMP *chunk = manager->getComponent<ChunkCMP>(id);
        RenderCMP *render = manager->getComponent<RenderCMP>(id);

        if (chunk != nullptr && render != nullptr && render->isRendered) {
            float startX = chunk->chunkX * CHUNK_SIZE;
            float startZ = chunk->chunkZ * CHUNK_SIZE;

            for (int y = 0; y < CHUNK_SIZE; y++) {
                for (int z = 0; z < CHUNK_SIZE; z++) {
                    for (int x = 0; x < CHUNK_SIZE; x++) {
                        
                        VoxelType type = chunk->GetVoxel(x, y, z);
                        if (type != VoxelType::EMPTY) {
                            Vector3 pos = { startX + x, (float)y, startZ + z };
                            Color color = WHITE;
                            
                            switch(type) {
                                case VoxelType::DIRT: color = BROWN; break;
                                case VoxelType::GRASS: color = GREEN; break;
                                case VoxelType::WATER: color = BLUE; break;
                                case VoxelType::STONE: color = GRAY; break;
                                default: break;
                            }

                            // Dibujamos el bloque y sus bordes
                            DrawCube(pos, 1.0f, 1.0f, 1.0f, color);
                            DrawCubeWires(pos, 1.0f, 1.0f, 1.0f, BLACK);
                        }
                    }
                }
            }
        }
    }
}

bool RenderSystemMeta::WindowShouldClose() {
    return ::WindowShouldClose();
}