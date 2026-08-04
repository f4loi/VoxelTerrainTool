#include "VoxelEngine/Systems/RenderSystemMeta.h"

void RenderSystemMeta::Init()
{
    target3D = LoadRenderTexture(1920, 1080);
}

void RenderSystemMeta::Update(EntityManagerMeta &em, const Camera3D &camera)
{
    BeginTextureMode(target3D);

    ClearBackground(SKYBLUE);

    BeginMode3D(camera);
    em.forAll(&RenderSystemMeta::UpdateOneEntity);
    EndMode3D();

    EndTextureMode();
}

void RenderSystemMeta::UpdateOneEntity(EntityMeta ent)
{
    EntityManagerMeta *manager{ent.getParent()};
    uint16_t id{ent.getNextId()};

    if (manager != nullptr)
    {
        ChunkCMP *chunk = manager->getComponent<ChunkCMP>(id);
        RenderCMP *render = manager->getComponent<RenderCMP>(id);

        if (chunk != nullptr && render != nullptr && render->isRendered)
        {
            float startX = chunk->chunkX * CHUNK_SIZE;
            float startZ = chunk->chunkZ * CHUNK_SIZE;

            for (int y = 0; y < CHUNK_SIZE; y++)
            {
                for (int z = 0; z < CHUNK_SIZE; z++)
                {
                    for (int x = 0; x < CHUNK_SIZE; x++)
                    {

                        VoxelType type = chunk->GetVoxel(x, y, z);
                        if (type != VoxelType::EMPTY)
                        {
                            Vector3 pos = {startX + x, (float)y, startZ + z};
                            Color color = WHITE;

                            switch (type)
                            {
                            case VoxelType::DIRT:
                                color = BROWN;
                                break;
                            case VoxelType::GRASS:
                                color = GREEN;
                                break;
                            case VoxelType::WATER:
                                color = BLUE;
                                break;
                            case VoxelType::STONE:
                                color = GRAY;
                                break;
                            default:
                                break;
                            }

                            DrawCube(pos, 1.0f, 1.0f, 1.0f, color);
                            DrawCubeWires(pos, 1.0f, 1.0f, 1.0f, BLACK);
                        }
                    }
                }
            }
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