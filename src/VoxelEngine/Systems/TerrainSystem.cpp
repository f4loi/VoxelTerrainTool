#include "VoxelEngine/Systems/TerrainSystem.h"
#include "VoxelEngine/CMP/ChunkCMP.h"
#include <stdlib.h>
#include <raymath.h>
#include <cmath>

/*
    Function: BuildColumn
    Description: Constructs a vertical column of voxels in a chunk based on noise and biome data.
    Parameters:
        - ChunkCMP *chunkData: Pointer to the chunk data where the column will be built.
        - int x, z: The local coordinates within the chunk for the column.
        - TerrainConfig &config: Reference to the terrain configuration settings.
*/
static void BuildColumn(ChunkCMP *chunkData, int x, int z, TerrainConfig &config)
{
    int flatIndex = z * CHUNK_SIZE + x;
    uint8_t noiseValue = chunkData->noiseMap[flatIndex];
    BiomeType biome = chunkData->biomeMap[flatIndex];
    float n = noiseValue / 255.0f;

    int terrainHeight = 0;
    VoxelType surface = VoxelType::GRASS;
    VoxelType subSurface = VoxelType::DIRT;

    int waterLvl = config.waterLevel;
    int dirtDepth = 3;

    // ==========================================
    // 1. GEOLOGÍA (Se calcula siempre)
    // ==========================================
    int globalX = chunkData->chunkX * CHUNK_SIZE + x;
    int globalZ = chunkData->chunkZ * CHUNK_SIZE + z;
    float macroNoise = (sin(globalX * 0.08f) + cos(globalZ * 0.08f)) * 0.5f;
    float microNoise = (sin(globalX * 0.25f) + cos(globalZ * 0.25f)) * 0.5f;

    dirtDepth = 3 + (int)((macroNoise + microNoise) * 2.0f);
    if (dirtDepth < 1)
        dirtDepth = 1;

    if (n < 0.35f)
    {
        float t = n / 0.35f;
        terrainHeight = (waterLvl - 15) + (int)(t * 15.0f);
        surface = VoxelType::SAND;
        subSurface = VoxelType::SAND;
    }
    else if (n < 0.45f)
    {
        float t = (n - 0.35f) / 0.10f;
        terrainHeight = waterLvl + (int)(t * 4.0f) + (int)(macroNoise * 1.5f);
        surface = (terrainHeight <= waterLvl + 1) ? VoxelType::SAND : VoxelType::GRASS;
        subSurface = (surface == VoxelType::SAND) ? VoxelType::SAND : VoxelType::DIRT;
    }
    else if (n < 0.65f)
    {
        float t = (n - 0.45f) / 0.20f;
        float baseHeight = t * t * (3.0f - 2.0f * t);
        float hillBumps = macroNoise * 5.0f * baseHeight + microNoise * 2.0f;
        terrainHeight = (waterLvl + 4) + (int)(baseHeight * 18.0f) + (int)hillBumps;
        surface = VoxelType::GRASS;
        subSurface = VoxelType::DIRT;
    }
    else
    {
        float t = (n - 0.65f) / 0.35f;
        float massif = t * t * (3.0f - 2.0f * t);
        float wave1 = sin(globalX * 0.03f + globalZ * 0.02f);
        float wave2 = cos(globalX * 0.02f - globalZ * 0.03f);
        float ridges = 1.0f - (fabs(wave1) * fabs(wave2));
        ridges = pow(ridges, 2.5f);
        float mountainShape = (massif * 45.0f) + (ridges * 70.0f * t);
        float ruggedness = (macroNoise + microNoise) * 6.0f * t;
        terrainHeight = (waterLvl + 22) + (int)(mountainShape) + (int)ruggedness;

        float snowLine = (waterLvl + 70) + macroNoise * 8.0f - (ridges * 10.0f);
        float rockLine = (waterLvl + 35) + microNoise * 5.0f;

        if (terrainHeight > snowLine)
        {
            surface = VoxelType::SNOW;
            subSurface = VoxelType::SNOW;
        }
        else if (terrainHeight > rockLine)
        {
            if (ridges > 0.5f || terrainHeight > rockLine + 12.0f)
            {
                surface = VoxelType::STONE;
                subSurface = VoxelType::STONE;
            }
            else
            {
                surface = VoxelType::GRASS;
                subSurface = VoxelType::STONE;
            }
        }
        else
        {
            surface = VoxelType::GRASS;
            subSurface = VoxelType::DIRT;
        }
    }

    // ==========================================
    // 2. MODIFICADORES DE BIOMA PINTADOS (El Río)
    // ==========================================
    if (biome == BiomeType::RIVER)
    {
        surface = VoxelType::WATER;   // El bloque superior será agua
        subSurface = VoxelType::DIRT; // El fondo del río será barro
        dirtDepth = 2;

        // Hundimos físicamente el agua un bloque para dar sensación de profundidad
        terrainHeight -= 1;
        if (terrainHeight < waterLvl)
            terrainHeight = waterLvl;
    }

    // ==========================================
    // 3. CONSTRUCCIÓN DE LA COLUMNA
    // ==========================================
    for (int y = 0; y < CHUNK_HEIGHT; y++)
    {
        if (y <= terrainHeight)
        {
            if (y == terrainHeight)
                chunkData->SetVoxel(x, y, z, surface);
            else if (y > terrainHeight - dirtDepth)
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

/*
    Function: TerrainSystem::Init
    Description: Initializes the terrain system by creating a dummy texture for the map.
    This function generates a plain black image and loads it as a texture, which will be used to represent the terrain map in the user interface.
*/
void TerrainSystem::Init()
{

    Image dummy = GenImageColor(WORLD_PIXELS, WORLD_PIXELS, BLACK);
    mapTexture = LoadTextureFromImage(dummy);
    SetTextureFilter(mapTexture, TEXTURE_FILTER_POINT);
    UnloadImage(dummy);
}

/*
    Function: TerrainSystem::GenerateTerrain
    Description: Generates terrain for the specified chunks based on noise and biome data.
    Parameters:
        - EntityManagerMeta &em: Reference to the entity manager that manages the chunks.
        - const std::vector<EntityMeta> &chunks: Vector of entity metadata representing the chunks to generate terrain for.
        - TerrainConfig &config: Reference to the terrain configuration settings.
*/
void TerrainSystem::GenerateTerrain(EntityManagerMeta &em, const std::vector<EntityMeta> &chunks, TerrainConfig &config)
{
    for (EntityMeta ent : chunks)
    {
        ChunkCMP *chunkData = em.getComponent<ChunkCMP>(ent.getNextId());
        if (chunkData == nullptr)
        {
            continue;
        }

        int offsetX = chunkData->chunkX * CHUNK_SIZE + config.seed;
        int offsetZ = chunkData->chunkZ * CHUNK_SIZE + config.seed;
        // Generate Perlin noise for the chunk using the specified noise scale and offsets
        Image noiseImage = GenImagePerlinNoise(CHUNK_SIZE, CHUNK_SIZE, offsetX, offsetZ, config.noiseScale);
        Color *colors = LoadImageColors(noiseImage);
        // Iterate through each voxel column in the chunk and build the terrain based on the noise and biome data
        for (int z = 0; z < CHUNK_SIZE; z++)
        {
            for (int x = 0; x < CHUNK_SIZE; x++)
            {
                chunkData->noiseMap[z * CHUNK_SIZE + x] = colors[z * CHUNK_SIZE + x].r;
                chunkData->biomeMap[z * CHUNK_SIZE + x] = BiomeType::DEFAULT;

                // Build the voxel column for the current (x, z) position in the chunk
                BuildColumn(chunkData, x, z, config);
            }
        }
        UnloadImageColors(colors);
        UnloadImage(noiseImage);
    }
    // Mark the configuration as not needing regeneration and indicate that the map texture needs to be updated
    config.needsRegen = false;
    config.needsMapUpdate = true;
}
/*
    Function: TerrainSystem::ApplyPaint
    Description: Applies painting to the terrain for the specified chunks based on the paint configuration.
    Parameters:
        - EntityManagerMeta &em: Reference to the entity manager that manages the chunks.
        - const std::vector<EntityMeta> &chunks: Vector of entity metadata representing the chunks to apply painting to.
        - TerrainConfig &config: Reference to the terrain configuration settings.
*/

void TerrainSystem::ApplyPaint(EntityManagerMeta &em, const std::vector<EntityMeta> &chunks, TerrainConfig &config)
{
    static float paintTimer = 0.0f;
    paintTimer += GetFrameTime();
    if (paintTimer < 0.05f)
        return;
    paintTimer = 0.0f;

    float paintSpeed = config.brushStrength * 0.05f;

    // HELPER PARA LEER RUIDO GLOBAL
    auto GetGlobalNoise = [&](int px, int pz, float fallback) -> float
    {
        if (px < 0 || px >= WORLD_PIXELS || pz < 0 || pz >= WORLD_PIXELS)
            return fallback;
        int chunkIndex = (px / CHUNK_SIZE) * WORLD_CHUNKS + (pz / CHUNK_SIZE);
        if (chunkIndex >= 0 && chunkIndex < chunks.size())
        {
            EntityMeta ent = chunks[chunkIndex];
            ChunkCMP *cData = em.getComponent<ChunkCMP>(ent.getNextId());
            if (cData)
                return cData->noiseMap[(pz % CHUNK_SIZE) * CHUNK_SIZE + (px % CHUNK_SIZE)] / 255.0f;
        }
        return fallback;
    };

    Vector2 brushCenter = {(float)config.paintX, (float)config.paintZ};
    float radius = (float)config.brushSize;

    for (int pZ = config.paintZ - config.brushSize - 1; pZ <= config.paintZ + config.brushSize + 1; pZ++)
    {
        for (int pX = config.paintX - config.brushSize - 1; pX <= config.paintX + config.brushSize + 1; pX++)
        {
            if (pX >= 0 && pX < WORLD_PIXELS && pZ >= 0 && pZ < WORLD_PIXELS)
            {
                float intensity = 0.0f;
                float dist = Vector2Distance(brushCenter, {(float)pX, (float)pZ});

                if (config.brushShape == BrushShape::CIRCLE_SOFT)
                {
                    if (dist > radius)
                        continue;
                    intensity = 1.0f - ((dist / radius) * (dist / radius));
                }
                else if (config.brushShape == BrushShape::CIRCLE_HARD)
                {
                    if (dist > radius)
                        continue;
                    intensity = 1.0f;
                }
                else if (config.brushShape == BrushShape::SQUARE)
                {
                    if (abs(pX - config.paintX) > radius || abs(pZ - config.paintZ) > radius)
                        continue;
                    intensity = 1.0f;
                }
                else if (config.brushShape == BrushShape::NOISE)
                {
                    if (dist > radius)
                        continue;
                    if (((rand() % 100) / 100.0f) > 0.7f)
                        intensity = 1.0f;
                    else
                        continue;
                }

                if (intensity <= 0.0f)
                    continue;

                int chunkIndex = (pX / CHUNK_SIZE) * WORLD_CHUNKS + (pZ / CHUNK_SIZE);
                if (chunkIndex >= 0 && chunkIndex < chunks.size())
                {
                    EntityMeta ent = chunks[chunkIndex];
                    ChunkCMP *chunkData = em.getComponent<ChunkCMP>(ent.getNextId());

                    if (chunkData)
                    {
                        int lX = pX % CHUNK_SIZE;
                        int lZ = pZ % CHUNK_SIZE;
                        int flatIndex = lZ * CHUNK_SIZE + lX;

                        float n = chunkData->noiseMap[flatIndex] / 255.0f;
                        float finalPower = intensity * paintSpeed;

                        if (config.activeBrush == BrushType::RAISE)
                        {
                            n = n + (1.0f - n) * finalPower;
                        }
                        else if (config.activeBrush == BrushType::LOWER)
                        {
                            n = n - n * finalPower;
                        }
                        else if (config.activeBrush == BrushType::FLATTEN)
                        {
                            n = n + (config.flattenTarget - n) * finalPower * 2.0f;
                        }
                        else if (config.activeBrush == BrushType::SMOOTH)
                        {
                            float avg = (GetGlobalNoise(pX - 1, pZ, n) + GetGlobalNoise(pX + 1, pZ, n) + GetGlobalNoise(pX, pZ - 1, n) + GetGlobalNoise(pX, pZ + 1, n)) / 4.0f;
                            n = n + (avg - n) * finalPower * 4.0f;
                        }
                        else if (config.activeBrush == BrushType::ROUGHEN)
                        {
                            n = n + (((rand() % 100) / 100.0f - 0.5f) * 0.1f) * finalPower * 4.0f;
                        }
                        else if (config.activeBrush == BrushType::TERRACE)
                        {
                            float terraces = 15.0f;
                            float stepped = round(n * terraces) / terraces;
                            n = n + (stepped - n) * finalPower * 3.0f;
                        }
                        else if (config.activeBrush == BrushType::SHARPEN)
                        {
                            float avg = (GetGlobalNoise(pX - 1, pZ, n) + GetGlobalNoise(pX + 1, pZ, n) + GetGlobalNoise(pX, pZ - 1, n) + GetGlobalNoise(pX, pZ + 1, n)) / 4.0f;
                            n = n + (n - avg) * finalPower * 5.0f;
                        }

                        if (n > 1.0f)
                            n = 1.0f;
                        if (n < 0.0f)
                            n = 0.0f;

                        chunkData->noiseMap[flatIndex] = (uint8_t)(n * 255.0f);
                        chunkData->biomeMap[flatIndex] = BiomeType::DEFAULT;

                        BuildColumn(chunkData, lX, lZ, config);
                    }
                }
            }
        }
    }
    config.needsMapUpdate = true;
}

/*
    Function: TerrainSystem::UpdateMapTexture
    Description: Updates the 2D map texture based on the current state of the terrain in the specified chunks.
    Parameters:
        - EntityManagerMeta &em: Reference to the entity manager that manages the chunks.
        - const std::vector<EntityMeta> &chunks: Vector of entity metadata representing the chunks to update the map texture for.
        - TerrainConfig &config: Reference to the terrain configuration settings.
*/
void TerrainSystem::UpdateMapTexture(EntityManagerMeta &em, const std::vector<EntityMeta> &chunks, TerrainConfig &config)
{
    // OPTIMIZACIÓN PROFESIONAL: Usamos un vector estático para reservar la memoria SOLO UNA VEZ en la vida del programa.
    // Esto elimina las costosas llamadas al Sistema Operativo (malloc/free) mientras el usuario pinta.
    static std::vector<Color> pixels(WORLD_PIXELS * WORLD_PIXELS);

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

                if (biome == BiomeType::PLAINS)
                {
                    c = GREEN;
                }
                else if (biome == BiomeType::MOUNTAIN)
                {
                    c = GRAY;
                }
                else if (biome == BiomeType::RIVER)
                {
                    c = BLUE;
                }
                else
                {
                    uint8_t noiseVal = chunkData->noiseMap[flatIndex];
                    float n = noiseVal / 255.0f;

                    if (n < 0.35f)
                        c = BLUE;
                    else if (n < 0.40f)
                        c = Color{238, 214, 175, 255};
                    else if (n < 0.60f)
                        c = GREEN;
                    else if (n < 0.85f)
                        c = GRAY;
                    else
                        c = WHITE;
                }
                pixels[(offsetZ + z) * WORLD_PIXELS + (offsetX + x)] = c;
            }
        }
    }
    // Subimos los píxeles directamente desde el vector pre-alocado
    UpdateTexture(mapTexture, pixels.data());
    config.needsMapUpdate = false;
}

/*
    Function: TerrainSystem::Unload
    Description: Unloads the map texture from GPU memory to free up resources.
    This function checks if the map texture has been loaded (i.e., its ID is not zero) and calls the appropriate function to unload it from GPU memory (VRAM).
*/
void TerrainSystem::Unload()
{
    if (mapTexture.id != 0)
    {
        UnloadTexture(mapTexture);
    }
}

/*
    Function: TerrainSystem::SaveUndoState
    Description: Captures a snapshot of the current 2D noise map across all chunks
                 and stores it in the undo history stack.
*/
void TerrainSystem::SaveUndoState(EntityManagerMeta &em, const std::vector<EntityMeta> &chunks)
{
    TerrainState state;
    state.noise.resize(WORLD_PIXELS * WORLD_PIXELS);

    for (EntityMeta ent : chunks)
    {
        ChunkCMP *chunkData = em.getComponent<ChunkCMP>(ent.getNextId());
        if (!chunkData)
        {
            continue;
        }

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
    // Add the captured state to the undo history stack
    undoHistory.push_back(state);

    if (undoHistory.size() > 20)
    {
        undoHistory.erase(undoHistory.begin());
    }
}

/*
    Function: TerrainSystem::Undo
    Description: Restores the terrain to the previous state by applying the last saved noise map from the undo history.
                 This function iterates through all chunks, restoring their noise and biome maps, and rebuilding the voxel columns accordingly.
*/
void TerrainSystem::Undo(EntityManagerMeta &em, const std::vector<EntityMeta> &chunks, TerrainConfig &config)
{
    if (undoHistory.empty())
    {
        return;
    }
    // Retrieve and remove the most recent snapshot from the history stack
    TerrainState state = undoHistory.back();
    undoHistory.pop_back();

    for (EntityMeta ent : chunks)
    {
        ChunkCMP *chunkData = em.getComponent<ChunkCMP>(ent.getNextId());
        if (!chunkData)
        {
            continue;
        }

        int offsetX = chunkData->chunkX * CHUNK_SIZE;
        int offsetZ = chunkData->chunkZ * CHUNK_SIZE;

        for (int z = 0; z < CHUNK_SIZE; z++)
        {
            for (int x = 0; x < CHUNK_SIZE; x++)
            {
                int flatIndex = z * CHUNK_SIZE + x;
                int globalIndex = (offsetZ + z) * WORLD_PIXELS + (offsetX + x);

                // Restore the previous noise value from the global history state
                chunkData->noiseMap[flatIndex] = state.noise[globalIndex];

                // Reset the biome map to DEFAULT so the procedural rules take over again
                chunkData->biomeMap[flatIndex] = BiomeType::DEFAULT;

                // Force the complete regeneration of the 3D voxel column based on the restored noise
                BuildColumn(chunkData, x, z, config);
            }
        }
    }
    config.needsMapUpdate = true;
}