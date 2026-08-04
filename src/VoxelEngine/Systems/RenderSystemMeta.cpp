#include "VoxelEngine/Systems/RenderSystemMeta.h"
#include "utils/ChunkMeshBuilder.h"

void RenderSystemMeta::Init() {
    target3D = LoadRenderTexture(1920, 1080);
}

void RenderSystemMeta::Update(EntityManagerMeta &em, const Camera3D &camera) {
    BeginTextureMode(target3D);
    ClearBackground(SKYBLUE);
    BeginMode3D(camera);
    
    em.forAll(&RenderSystemMeta::UpdateOneEntity);
    
    EndMode3D();
    EndTextureMode();
}

void RenderSystemMeta::UpdateOneEntity(EntityMeta ent) {
    // 1. ¡Extraemos el EntityManager directamente del padre de la entidad!
    EntityManagerMeta* em = ent.getParent();
    if (em == nullptr) return;

    // 2. Pedimos los componentes usando ese 'em'
    auto* render = em->getComponent<RenderCMP>(ent.getNextId());
    auto* chunk  = em->getComponent<ChunkCMP>(ent.getNextId());

    // Fíjate que usamos 'isRendered' que es como lo llamaste en tu RenderCMP
    if (render && render->isRendered && chunk) {
                 
        // --- RECONSTRUCCIÓN DINÁMICA DE MALLA ---
        if (chunk->isDirty) {
            if (chunk->meshModel.meshCount > 0) {
                UnloadModel(chunk->meshModel); 
            }
            chunk->meshModel = ChunkMeshBuilder::BuildMesh(*chunk); 
            chunk->isDirty = false; 
        }

        // --- DIBUJO HIPER-RÁPIDO ---
        if (chunk->meshModel.meshCount > 0) {
            DrawModel(chunk->meshModel, Vector3{ 0, 0, 0 }, 1.0f, WHITE);
        }
    }
}

void RenderSystemMeta::Unload() {
    UnloadRenderTexture(target3D);
}

bool RenderSystemMeta::WindowShouldClose() {
    return ::WindowShouldClose();
}