#include "VoxelEngine/Systems/RenderSystemMeta.h"
#include "utils/ChunkMeshBuilder.h"
#include <raymath.h>

Camera3D RenderSystemMeta::currentCam = { 0 };

void RenderSystemMeta::Init() {
    target3D = LoadRenderTexture(1920, 1080);
}

void RenderSystemMeta::Update(EntityManagerMeta &em, const Camera3D &camera) {
    currentCam = camera;
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
            Vector3 pos = { (float)chunk->chunkX * CHUNK_SIZE, 0.0f, (float)chunk->chunkZ * CHUNK_SIZE };
            DrawModel(chunk->meshModel, pos, 1.0f, WHITE);
            
        }
    }
}

void RenderSystemMeta::Unload() {
    UnloadRenderTexture(target3D);
}

bool RenderSystemMeta::WindowShouldClose() {
    return ::WindowShouldClose();
}