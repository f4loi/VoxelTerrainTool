#include "VoxelEngine/Systems/TerrainSystem.h"
#include "VoxelEngine/CMP/ChunkCMP.h"
#include <stdlib.h>
#include <raymath.h>
#include <cmath>

static void BuildColumn(ChunkCMP *chunkData, int x, int z, TerrainConfig &config)
{
    int flatIndex = z * CHUNK_SIZE + x;
    uint8_t noiseValue = chunkData->noiseMap[flatIndex];
    BiomeType biome = chunkData->biomeMap[flatIndex];

    // Normalizamos el ruido a un valor entre 0.0 y 1.0
    float n = noiseValue / 255.0f;

    int terrainHeight = 0;
    VoxelType surface = VoxelType::GRASS;
    VoxelType subSurface = VoxelType::DIRT;

    int waterLvl = config.waterLevel;

    // =========================================================
    // 1. GENERACIÓN PROCEDURAL REALISTA (Bioma por defecto)
    // =========================================================
    if (biome == BiomeType::DEFAULT)
    {

        if (n < 0.35f)
        {
            // ZONA 1: FONDO MARINO (0.0 a 0.35)
            float localN = n / 0.35f;
            terrainHeight = 10 + (int)(localN * (waterLvl - 5 - 10)); // Sube lentamente hasta casi el nivel del agua
            surface = VoxelType::DIRT;
            subSurface = VoxelType::DIRT;
        }
        else if (n < 0.40f)
        {
            // ZONA 2: PLAYAS Y COSTAS (0.35 a 0.40)
            float localN = (n - 0.35f) / 0.05f;
            terrainHeight = (waterLvl - 5) + (int)(localN * 7); // Transición plana que sale justo del agua
            surface = VoxelType::SAND;
            subSurface = VoxelType::SAND;
        }
        else if (n < 0.60f)
        {
            // ZONA 3: PRADOS Y COLINAS SUAVES (0.40 a 0.60)
            float localN = (n - 0.40f) / 0.20f;
            float curve = pow(localN, 1.5f);                    // Curva suave para colinas
            terrainHeight = (waterLvl + 2) + (int)(curve * 30); // Sube hasta 30 bloques más
            surface = VoxelType::GRASS;
            subSurface = VoxelType::DIRT;
        }
        else
        {
            // ZONA 4: MONTAÑAS ESCARPADAS (0.60 a 1.0)
            float localN = (n - 0.60f) / 0.40f;

            // LA FÓRMULA REALISTA: Mezclamos un 40% de rampa suave (base) y un 60% de curva extrema (pico)
            float curve = (localN * 0.4f) + (pow(localN, 3.0f) * 0.6f);

            terrainHeight = (waterLvl + 32) + (int)(curve * 140); // Montañas colosales de hasta 140 bloques

            // Estratificación térmica (Capas realistas de la montaña)
            if (terrainHeight > waterLvl + 80)
            {
                surface = VoxelType::SNOW; // Cumbres nevadas altas
                subSurface = VoxelType::SNOW;
            }
            else if (terrainHeight > waterLvl + 50)
            {
                surface = VoxelType::STONE; // Zona intermedia rocosa
                subSurface = VoxelType::STONE;
            }
            else
            {
                surface = VoxelType::GRASS;    // Falda de la montaña (césped)
                subSurface = VoxelType::STONE; // Roca asomando bajo el césped
            }
        }
    }

    // =========================================================
    // CONSTRUCCIÓN DE LA COLUMNA
    // =========================================================
    for (int y = 0; y < CHUNK_HEIGHT; y++)
    {
        if (y <= terrainHeight)
        {
            if (y == terrainHeight)
                chunkData->SetVoxel(x, y, z, surface);
            else if (y > terrainHeight - 5)
                chunkData->SetVoxel(x, y, z, subSurface);
            else
                chunkData->SetVoxel(x, y, z, VoxelType::STONE);
        }
        else if (y <= config.waterLevel)
        {
            chunkData->SetVoxel(x, y, z, VoxelType::WATER);
        }
        else
        {
            chunkData->SetVoxel(x, y, z, VoxelType::EMPTY);
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
    // Centro del pincel y radio
    Vector2 brushCenter = {(float)config.paintX, (float)config.paintZ};
    float radius = (float)config.brushSize;

    for (int pZ = config.paintZ - config.brushSize - 1; pZ <= config.paintZ + config.brushSize + 1; pZ++)
    {
        for (int pX = config.paintX - config.brushSize - 1; pX <= config.paintX + config.brushSize + 1; pX++)
        {
            if (pX >= 0 && pX < WORLD_PIXELS && pZ >= 0 && pZ < WORLD_PIXELS)
            {
                float intensity = 1.0f;

                // --- NUEVO: ¿CÍRCULO O CUBO? ---
                if (!config.isSquareBrush)
                {
                    float dist = Vector2Distance(brushCenter, {(float)pX, (float)pZ});
                    if (dist > radius)
                        continue;
                    intensity = 1.0f - (dist / radius);
                    intensity = intensity * intensity;
                }
                else
                {
                    // Pincel cuadrado (Cubo 3D): Límites rectos
                    if (abs(pX - config.paintX) > radius || abs(pZ - config.paintZ) > radius)
                        continue;
                    intensity = 0.4f; // Intensidad constante pero suave para esculpir el cubo plano
                }

                int cX = pX / CHUNK_SIZE;
                int cZ = pZ / CHUNK_SIZE;
                int lX = pX % CHUNK_SIZE;
                int lZ = pZ % CHUNK_SIZE;

                for (EntityMeta ent : chunks)
                {
                    ChunkCMP *chunkData = em.getComponent<ChunkCMP>(ent.getNextId());
                    if (chunkData && chunkData->chunkX == cX && chunkData->chunkZ == cZ)
                    {
                        int flatIndex = lZ * CHUNK_SIZE + lX;

                        // Extraemos el ruido
                        float n = chunkData->noiseMap[flatIndex] / 255.0f;

                        // --- 2. ESCULPIMOS EL RUIDO SEGÚN EL BIOMA ---
                        if (config.selectedBiome == PaintBiome::MOUNTAIN)
                        {
                            n = n + (0.95f - n) * intensity * 0.6f;
                        }
                        else if (config.selectedBiome == PaintBiome::RIVER)
                        {
                            n = n + (0.25f - n) * intensity * 0.8f;
                        }
                        else if (config.selectedBiome == PaintBiome::PLAINS)
                        {
                            n = n + (0.45f - n) * intensity * 0.5f;
                        }

                        if (n > 1.0f)
                            n = 1.0f;
                        if (n < 0.0f)
                            n = 0.0f;

                        // --- 3. APLICAMOS LOS CAMBIOS ---
                        // Guardamos el nuevo ruido
                        chunkData->noiseMap[flatIndex] = (uint8_t)(n * 255.0f);

                        // Forzamos el bioma a DEFAULT para que las alturas decidan el material
                        chunkData->biomeMap[flatIndex] = BiomeType::DEFAULT;

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

void TerrainSystem::SaveUndoState(EntityManagerMeta &em, const std::vector<EntityMeta> &chunks)
{
    TerrainState state;
    state.noise.resize(WORLD_PIXELS * WORLD_PIXELS);

    for (EntityMeta ent : chunks)
    {
        ChunkCMP *chunkData = em.getComponent<ChunkCMP>(ent.getNextId());
        if (!chunkData)
            continue;
        int offsetX = chunkData->chunkX * CHUNK_SIZE;
        int offsetZ = chunkData->chunkZ * CHUNK_SIZE;

        for (int z = 0; z < CHUNK_SIZE; z++)
        {
            for (int x = 0; x < CHUNK_SIZE; x++)
            {
                int globalIndex = (offsetZ + z) * WORLD_PIXELS + (offsetX + x);
                state.noise[globalIndex] = chunkData->noiseMap[z * CHUNK_SIZE + x];
            }
        }
    }
    undoHistory.push_back(state);

    // Límite de 20 pasos hacia atrás para no saturar la RAM
    if (undoHistory.size() > 20)
        undoHistory.erase(undoHistory.begin());
}

void TerrainSystem::Undo(EntityManagerMeta &em, const std::vector<EntityMeta> &chunks, TerrainConfig &config)
{
    if (undoHistory.empty())
        return; // No hay más pasos atrás

    TerrainState state = undoHistory.back();
    undoHistory.pop_back();

    for (EntityMeta ent : chunks)
    {
        ChunkCMP *chunkData = em.getComponent<ChunkCMP>(ent.getNextId());
        if (!chunkData)
            continue;
        int offsetX = chunkData->chunkX * CHUNK_SIZE;
        int offsetZ = chunkData->chunkZ * CHUNK_SIZE;

        for (int z = 0; z < CHUNK_SIZE; z++)
        {
            for (int x = 0; x < CHUNK_SIZE; x++)
            {
                int flatIndex = z * CHUNK_SIZE + x;
                int globalIndex = (offsetZ + z) * WORLD_PIXELS + (offsetX + x);

                // Restauramos el ruido y forzamos la regeneración de la columna
                chunkData->noiseMap[flatIndex] = state.noise[globalIndex];
                chunkData->biomeMap[flatIndex] = BiomeType::DEFAULT;
                BuildColumn(chunkData, x, z, config);
            }
        }
    }
    config.needsMapUpdate = true;
}