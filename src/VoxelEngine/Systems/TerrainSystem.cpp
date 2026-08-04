#include "VoxelEngine/Systems/TerrainSystem.h"
#include <stdlib.h> // Para malloc y free

void TerrainSystem::Init()
{
    // Inicializamos la textura vacía (negra)
    Image dummy = GenImageColor(CHUNK_SIZE, CHUNK_SIZE, BLACK);
    mapTexture = LoadTextureFromImage(dummy);
    SetTextureFilter(mapTexture, TEXTURE_FILTER_POINT); // Nítida sin desenfoque
    UnloadImage(dummy);
}

void TerrainSystem::GenerateTerrain(EntityManagerMeta &em, EntityMeta chunkEntity, TerrainConfig &config)
{
    ChunkCMP *chunkData = em.getComponent<ChunkCMP>(chunkEntity.getNextId());
    if (chunkData == nullptr)
        return;

    Image noiseImage = GenImagePerlinNoise(CHUNK_SIZE, CHUNK_SIZE, config.seed, config.seed, config.noiseScale);
    Color *colors = LoadImageColors(noiseImage);

    for (int z = 0; z < CHUNK_SIZE; z++)
    {
        for (int x = 0; x < CHUNK_SIZE; x++)
        {
            for (int y = 0; y < CHUNK_SIZE; y++)
            {
                chunkData->SetVoxel(x, y, z, VoxelType::EMPTY); // Limpiamos el chunk
            }

            unsigned char noiseValue = colors[z * CHUNK_SIZE + x].r;
            int terrainHeight = (int)((noiseValue / 255.0f) * (CHUNK_SIZE - 2));

            for (int y = 0; y < CHUNK_SIZE; y++)
            {
                if (y <= terrainHeight)
                {
                    if (y == terrainHeight)
                        chunkData->SetVoxel(x, y, z, (y <= config.waterLevel) ? VoxelType::DIRT : VoxelType::GRASS);
                    else if (y > terrainHeight - 3)
                        chunkData->SetVoxel(x, y, z, VoxelType::DIRT);
                    else
                        chunkData->SetVoxel(x, y, z, VoxelType::STONE);
                }
                else if (y <= config.waterLevel)
                    chunkData->SetVoxel(x, y, z, VoxelType::WATER);
            }
        }
    }
    UnloadImageColors(colors);
    UnloadImage(noiseImage);

    config.needsRegen = false;
    config.needsMapUpdate = true; // Tras generar, forzamos la actualización del mapa 2D
}

void TerrainSystem::ApplyPaint(EntityManagerMeta &em, EntityMeta chunkEntity, TerrainConfig &config)
{
    ChunkCMP *chunkData = em.getComponent<ChunkCMP>(chunkEntity.getNextId());
    if (chunkData == nullptr)
        return;

    for (int z = config.paintZ - config.brushSize + 1; z < config.paintZ + config.brushSize; z++)
    {
        for (int x = config.paintX - config.brushSize + 1; x < config.paintX + config.brushSize; x++)
        {

            if (x >= 0 && x < CHUNK_SIZE && z >= 0 && z < CHUNK_SIZE)
            {
                int topY = -1;
                for (int y = CHUNK_SIZE - 1; y >= 0; y--)
                {
                    if (chunkData->GetVoxel(x, y, z) != VoxelType::EMPTY)
                    {
                        topY = y;
                        break;
                    }
                }

                if (topY != -1)
                {
                    VoxelType newType = VoxelType::GRASS;
                    if (config.selectedMaterial == PaintMaterial::WATER)
                        newType = VoxelType::WATER;
                    if (config.selectedMaterial == PaintMaterial::DIRT)
                        newType = VoxelType::DIRT;

                    chunkData->SetVoxel(x, topY, z, newType);
                }
            }
        }
    }
    config.needsMapUpdate = true; // Avisamos de que el 2D ha cambiado
}

void TerrainSystem::UpdateMapTexture(EntityManagerMeta &em, EntityMeta chunkEntity, TerrainConfig &config)
{
    ChunkCMP *chunkData = em.getComponent<ChunkCMP>(chunkEntity.getNextId());
    if (chunkData == nullptr)
        return;

    Color *pixels = (Color *)malloc(CHUNK_SIZE * CHUNK_SIZE * sizeof(Color));

    for (int z = 0; z < CHUNK_SIZE; z++)
    {
        for (int x = 0; x < CHUNK_SIZE; x++)
        {
            Color c = BLACK;
            for (int y = CHUNK_SIZE - 1; y >= 0; y--)
            {
                VoxelType v = chunkData->GetVoxel(x, y, z);
                if (v != VoxelType::EMPTY)
                {
                    if (v == VoxelType::WATER)
                        c = BLUE;
                    else if (v == VoxelType::DIRT)
                        c = BROWN;
                    else if (v == VoxelType::GRASS)
                        c = GREEN;
                    else if (v == VoxelType::STONE)
                        c = GRAY;
                    break;
                }
            }
            pixels[z * CHUNK_SIZE + x] = c;
        }
    }

    UpdateTexture(mapTexture, pixels);
    free(pixels);

    config.needsMapUpdate = false;
}

void TerrainSystem::Unload()
{
    if (mapTexture.id != 0)
        UnloadTexture(mapTexture);
}