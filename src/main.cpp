#include <raylib.h>
#include "VoxelEngine/Managers/EntityManagerMeta.h"
#include "VoxelEngine/Systems/RenderSystemMeta.h"
#include "VoxelEngine/Systems/uiSys.h"
#include "Config\TerrainConfig.h"

int main()
{

    InitWindow(1024, 768, "Generador de Biomas Pro");
    SetTargetFPS(60);

    EntityManagerMeta em;
    RenderSystemMeta renderSys;
    uiSys ui;
    TerrainConfig config;

    renderSys.Init();
    ui.Init();

    config.LoadFromJson("config.json");

    // 4. Creamos la entidad del Chunk en el ECS
    EntityMeta chunkEntity = em.createEntity();
    em.addComponent(chunkEntity.getNextId(), ChunkCMP{});
    em.addComponent(chunkEntity.getNextId(), RenderCMP{true});

    while (!renderSys.WindowShouldClose())
    {

        if (config.needsRegen)
        {
            ChunkCMP *chunkData = em.getComponent<ChunkCMP>(chunkEntity.getNextId());

            if (chunkData != nullptr)
            {

                Image noiseImage = GenImagePerlinNoise(CHUNK_SIZE, CHUNK_SIZE, config.seed, config.seed, config.noiseScale);
                Color *colors = LoadImageColors(noiseImage);

                for (int z = 0; z < CHUNK_SIZE; z++)
                {
                    for (int x = 0; x < CHUNK_SIZE; x++)
                    {

                        for (int y = 0; y < CHUNK_SIZE; y++)
                        {
                            chunkData->SetVoxel(x, y, z, VoxelType::EMPTY);
                        }

                        unsigned char noiseValue = colors[z * CHUNK_SIZE + x].r;
                        int terrainHeight = (int)((noiseValue / 255.0f) * (CHUNK_SIZE - 2));

                        for (int y = 0; y < CHUNK_SIZE; y++)
                        {
                            if (y <= terrainHeight)
                            {
                                if (y == terrainHeight)
                                {

                                    chunkData->SetVoxel(x, y, z, (y <= config.waterLevel) ? VoxelType::DIRT : VoxelType::GRASS);
                                }
                                else if (y > terrainHeight - 3)
                                {
                                    chunkData->SetVoxel(x, y, z, VoxelType::DIRT);
                                }
                                else
                                {
                                    chunkData->SetVoxel(x, y, z, VoxelType::STONE);
                                }
                            }
                            else if (y <= config.waterLevel)
                            {
                                chunkData->SetVoxel(x, y, z, VoxelType::WATER);
                            }
                        }
                    }
                }
                UnloadImageColors(colors);
                UnloadImage(noiseImage);
            }
            config.needsRegen = false;
        }

        // --- RENDERIZADO ---
        BeginDrawing();
        ClearBackground(SKYBLUE);

        renderSys.Update(em);

        ui.Draw(config);

        EndDrawing();
    }
    ui.Close();
    CloseWindow();

    return 0;
}