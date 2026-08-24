#include "VoxelEngine/Systems/RenderSystemMeta.h"
#include "utils/ChunkMeshBuilder.h"
#include <raymath.h>

// Static variable to hold the current camera state, making it accessible
// to static functions like UpdateOneEntity (useful for frustum/distance culling)
Camera3D RenderSystemMeta::currentCam = {0};

/*
    Function: Init
    Description: Initializes the render system by loading a render texture for 3D rendering.
*/
void RenderSystemMeta::Init()
{
    target3D = LoadRenderTexture(1920, 1080);
}

/*
    Function: Update
    Description: Updates the render system by rendering all entities with a RenderCMP and ChunkCMP.
    Parameters:
        - EntityManagerMeta& em: Reference to the entity manager containing all entities and their components.
        - const Camera3D& camera: The current camera state used for rendering.
*/
void RenderSystemMeta::Update(EntityManagerMeta &em, const Camera3D &camera)
{
    currentCam = camera;
    BeginTextureMode(target3D);
    ClearBackground(SKYBLUE);
    BeginMode3D(camera);

    // Render all entities with a RenderCMP and ChunkCMP
    em.forAll(&RenderSystemMeta::UpdateOneEntity);

    EndMode3D();
    EndTextureMode();
}

void RenderSystemMeta::UpdateOneEntity(EntityMeta ent)
{
    // Get the parent EntityManagerMeta to access components
    EntityManagerMeta *em = ent.getParent();
    if (em == nullptr)
    {
        return;
    }

    // Retrieve the RenderCMP and ChunkCMP components for the entity
    auto *render = em->getComponent<RenderCMP>(ent.getNextId());
    auto *chunk = em->getComponent<ChunkCMP>(ent.getNextId());

    // If the entity has both a RenderCMP and ChunkCMP, proceed with rendering
    if (render && render->isRendered && chunk)
    {
        // If the chunk is marked as dirty, rebuild its mesh model
        if (chunk->isDirty)
        {
            // If the chunk already has a mesh model, unload it to free resources   
            if (chunk->meshModel.meshCount > 0)
            {
                UnloadModel(chunk->meshModel);
            }
            chunk->meshModel = ChunkMeshBuilder::BuildMesh(*chunk);
            chunk->isDirty = false;
        }
        // If the chunk has a valid mesh model, draw it at the appropriate position
        if (chunk->meshModel.meshCount > 0)
        {
            Vector3 pos = {(float)chunk->chunkX * CHUNK_SIZE, 0.0f, (float)chunk->chunkZ * CHUNK_SIZE};
            DrawModel(chunk->meshModel, pos, 1.0f, WHITE);
        }
    }
}

void RenderSystemMeta::Unload()
{
    UnloadRenderTexture(target3D);
}

bool RenderSystemMeta::WindowShouldClose()
{
    return ::WindowShouldClose();
}