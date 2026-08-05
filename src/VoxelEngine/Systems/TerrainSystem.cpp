#include "VoxelEngine/Systems/TerrainSystem.h"
#include "VoxelEngine/CMP/ChunkCMP.h"
#include <stdlib.h>
#include <cmath>

static void BuildColumn(ChunkCMP *chunkData, int x, int z, TerrainConfig &config)
{
    int flatIndex = z * CHUNK_SIZE + x;
    uint8_t noiseValue = chunkData->noiseMap[flatIndex];
    BiomeType biome = chunkData->biomeMap[flatIndex];

    float n = noiseValue / 255.0f; // Ruido de 0.0 a 1.0

    int terrainHeight;
    VoxelType surface = VoxelType::GRASS;
    VoxelType subSurface = VoxelType::DIRT;

    // --- REGLAS MATEMÁTICAS SEGÚN EL BIOMA ---
    if (biome == BiomeType::RIVER)
    {
        // Los ríos se clavan por debajo del nivel del agua independientemente del ruido
        terrainHeight = config.waterLevel - 2;
        surface = VoxelType::DIRT; // Fondo de barro
    }
    else if (biome == BiomeType::PLAINS)
    {
        // Prados: Terreno bajo, variaciones muy suaves (multiplicador pequeño)
        terrainHeight = config.waterLevel + 1 + (int)(n * 8.0f);
        surface = VoxelType::GRASS;
    }
    else if (biome == BiomeType::MOUNTAIN)
    {
        // Montañas: Curva pronunciada, picos altísimos
        float curve = pow(n, 2.0f);
        terrainHeight = config.waterLevel + 5 + (int)(curve * 120.0f);
        surface = (terrainHeight > 90) ? VoxelType::STONE : VoxelType::GRASS; // Picos de piedra
        subSurface = (terrainHeight > 80) ? VoxelType::STONE : VoxelType::DIRT;
    }
    else
    {
        // DEFAULT (El terreno original virgen)
        float curve = pow(n, 2.5f);
        terrainHeight = 30 + (int)(curve * 110.0f);
        surface = VoxelType::GRASS;
    }

    // --- CONSTRUIR LA COLUMNA VERTICAL ---
    for (int y = 0; y < CHUNK_HEIGHT; y++)
    {
        if (y <= terrainHeight)
        {
            if (y == terrainHeight)
                chunkData->SetVoxel(x, y, z, surface);
            else if (y > terrainHeight - 4)
                chunkData->SetVoxel(x, y, z, subSurface);
            else
                chunkData->SetVoxel(x, y, z, VoxelType::STONE);
        }
        else if (y <= config.waterLevel)
        {
            chunkData->SetVoxel(x, y, z, VoxelType::WATER); // Llenar de agua si estamos por debajo del nivel
        }
        else
        {
            chunkData->SetVoxel(x, y, z, VoxelType::EMPTY); // Aire
        }
    }
}

void TerrainSystem::Init()
{
    // El mapa ahora es de 256x256 (WORLD_PIXELS)
    Image dummy = GenImageColor(WORLD_PIXELS, WORLD_PIXELS, BLACK);
    mapTexture = LoadTextureFromImage(dummy);
    SetTextureFilter(mapTexture, TEXTURE_FILTER_POINT);
    UnloadImage(dummy);
}

void TerrainSystem::GenerateTerrain(EntityManagerMeta &em, const std::vector<EntityMeta> &chunks, TerrainConfig &config)
{
    for (EntityMeta ent : chunks)
    {
        ChunkCMP *chunkData = em.getComponent<ChunkCMP>(ent.getNextId());
        if (chunkData == nullptr)
            continue;

        int offsetX = chunkData->chunkX * CHUNK_SIZE;
        int offsetZ = chunkData->chunkZ * CHUNK_SIZE;

        Image noiseImage = GenImagePerlinNoise(CHUNK_SIZE, CHUNK_SIZE, offsetX, offsetZ, config.noiseScale);
        Color *colors = LoadImageColors(noiseImage);

        for (int z = 0; z < CHUNK_SIZE; z++)
        {
            for (int x = 0; x < CHUNK_SIZE; x++)
            {
                // Guardamos el ruido inicial y reseteamos el bioma
                chunkData->noiseMap[z * CHUNK_SIZE + x] = colors[z * CHUNK_SIZE + x].r;
                chunkData->biomeMap[z * CHUNK_SIZE + x] = BiomeType::DEFAULT;

                // Construimos la columna
                BuildColumn(chunkData, x, z, config);
            }
        }
        UnloadImageColors(colors);
        UnloadImage(noiseImage);
    }
    config.needsRegen = false;
    config.needsMapUpdate = true;
}

void TerrainSystem::ApplyPaint(EntityManagerMeta &em, const std::vector<EntityMeta> &chunks, TerrainConfig &config)
{
    for (int pZ = config.paintZ - config.brushSize + 1; pZ < config.paintZ + config.brushSize; pZ++)
    {
        for (int pX = config.paintX - config.brushSize + 1; pX < config.paintX + config.brushSize; pX++)
        {

            if (pX >= 0 && pX < WORLD_PIXELS && pZ >= 0 && pZ < WORLD_PIXELS)
            {
                int cX = pX / CHUNK_SIZE;
                int cZ = pZ / CHUNK_SIZE;
                int lX = pX % CHUNK_SIZE;
                int lZ = pZ % CHUNK_SIZE;

                for (EntityMeta ent : chunks)
                {
                    ChunkCMP *chunkData = em.getComponent<ChunkCMP>(ent.getNextId());
                    if (chunkData && chunkData->chunkX == cX && chunkData->chunkZ == cZ)
                    {

                        // 1. Aplicamos el Bioma elegido en el pincel
                        BiomeType newBiome = BiomeType::DEFAULT;
                        if (config.selectedBiome == PaintBiome::PLAINS)
                            newBiome = BiomeType::PLAINS;
                        if (config.selectedBiome == PaintBiome::MOUNTAIN)
                            newBiome = BiomeType::MOUNTAIN;
                        if (config.selectedBiome == PaintBiome::RIVER)
                            newBiome = BiomeType::RIVER;

                        chunkData->biomeMap[lZ * CHUNK_SIZE + lX] = newBiome;

                        // 2. Re-generamos toda esta columna de bloques basándonos en las reglas del nuevo bioma
                        BuildColumn(chunkData, lX, lZ, config);

                        break;
                    }
                }
            }
        }
    }
    config.needsMapUpdate = true;
}

void TerrainSystem::UpdateMapTexture(EntityManagerMeta &em, const std::vector<EntityMeta> &chunks, TerrainConfig &config)
{
    Color *pixels = (Color *)malloc(WORLD_PIXELS * WORLD_PIXELS * sizeof(Color));

    for (EntityMeta ent : chunks)
    {
        ChunkCMP *chunkData = em.getComponent<ChunkCMP>(ent.getNextId());
        if (chunkData == nullptr)
            continue;

        int offsetX = chunkData->chunkX * CHUNK_SIZE;
        int offsetZ = chunkData->chunkZ * CHUNK_SIZE;

        for (int z = 0; z < CHUNK_SIZE; z++)
        {
            for (int x = 0; x < CHUNK_SIZE; x++)
            {
                int flatIndex = z * CHUNK_SIZE + x;
                BiomeType biome = chunkData->biomeMap[flatIndex];
                Color c;

                // Coloreamos el mapa 2D basándonos en el bioma pintado
                if (biome == BiomeType::PLAINS)
                    c = GREEN;
                else if (biome == BiomeType::MOUNTAIN)
                    c = GRAY;
                else if (biome == BiomeType::RIVER)
                    c = BLUE;
                else
                {
                    uint8_t n = chunkData->noiseMap[flatIndex];
                    c = Color{n, n, n, 255}; // Gris por defecto
                }

                pixels[(offsetZ + z) * WORLD_PIXELS + (offsetX + x)] = c;
            }
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