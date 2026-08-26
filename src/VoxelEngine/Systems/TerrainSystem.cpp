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

    // Normalize the noise value to a range of 0.0 to 1.0 for terrain height calculations
    float n = noiseValue / 255.0f;

    int terrainHeight = 0;
    VoxelType surface = VoxelType::GRASS;
    VoxelType subSurface = VoxelType::DIRT;

    int waterLvl = config.waterLevel;
    int dirtDepth = 3;

    // ==========================================
    //           BASE TERRAIN
    // ==========================================
    int globalX = chunkData->chunkX * CHUNK_SIZE + x;
    int globalZ = chunkData->chunkZ * CHUNK_SIZE + z;

    // Generate spatial noise
    float macroNoise = (sin(globalX * 0.08f) + cos(globalZ * 0.08f)) * 0.5f;
    float microNoise = (sin(globalX * 0.25f) + cos(globalZ * 0.25f)) * 0.5f;

    // Variable dirt depth
    dirtDepth = 3 + (int)((macroNoise + microNoise) * 2.0f);
    if (dirtDepth < 1)
        dirtDepth = 1;

    // ZONE 1: Deep Ocean to Shallows
    if (n < 0.35f)
    {
        float t = n / 0.35f;
        terrainHeight = (waterLvl - 15) + (int)(t * 15.0f);
        surface = VoxelType::SAND;
        subSurface = VoxelType::SAND;
    }
    // ZONE 2: Coastlines and Low Plains
    else if (n < 0.45f)
    {
        float t = (n - 0.35f) / 0.10f;
        terrainHeight = waterLvl + (int)(t * 4.0f) + (int)(macroNoise * 1.5f);
        surface = (terrainHeight <= waterLvl + 1) ? VoxelType::SAND : VoxelType::GRASS;
        subSurface = (surface == VoxelType::SAND) ? VoxelType::SAND : VoxelType::DIRT;
    }
    // ZONE 3: Rolling Hills and Meadows
    else if (n < 0.65f)
    {
        float t = (n - 0.45f) / 0.20f;
        float baseHeight = t * t * (3.0f - 2.0f * t); // Smoothstep
        float hillBumps = macroNoise * 5.0f * baseHeight + microNoise * 2.0f;

        terrainHeight = (waterLvl + 4) + (int)(baseHeight * 18.0f) + (int)hillBumps;
        surface = VoxelType::GRASS;
        subSurface = VoxelType::DIRT;
    }
    // ZONE 4: Mountain Ranges and Peaks
    else
    {
        float t = (n - 0.65f) / 0.35f;

        // Massif: The base volumetric uplift of the mountain range
        float massif = t * t * (3.0f - 2.0f * t);

        // Ridges: Intersecting wave formulas
        float wave1 = sin(globalX * 0.03f + globalZ * 0.02f);
        float wave2 = cos(globalX * 0.02f - globalZ * 0.03f);
        float ridges = 1.0f - (fabs(wave1) * fabs(wave2));
        ridges = pow(ridges, 2.5f); // Sharpen crests

        float mountainShape = (massif * 45.0f) + (ridges * 70.0f * t);
        float ruggedness = (macroNoise + microNoise) * 6.0f * t;

        terrainHeight = (waterLvl + 22) + (int)(mountainShape) + (int)ruggedness;

        // Dynamic snow and rock lines based on spatial noise and ridge depth
        float snowLine = (waterLvl + 70) + macroNoise * 8.0f - (ridges * 10.0f);
        float rockLine = (waterLvl + 35) + microNoise * 5.0f;

        if (terrainHeight > snowLine)
        {
            surface = VoxelType::SNOW;
            subSurface = VoxelType::SNOW;
        }
        else if (terrainHeight > rockLine)
        {
            // Bare rock is exposed on sharp ridges or high altitudes
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
    //       EXPLICIT PAINTED BIOME MODIFIERS
    // ==========================================
    if (biome == BiomeType::RIVER)
    {
        surface = VoxelType::WATER;   // Top layer is water
        subSurface = VoxelType::DIRT; // Riverbed is dirt
        dirtDepth = 2;

        // Physically lower the terrain to carve a trench for the river
        terrainHeight -= 1;
        if (terrainHeight < waterLvl)
            terrainHeight = waterLvl;
    }

    // ==========================================
    // 3. COLUMN CONSTRUCTION (Voxel Assignment)
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
    Description: Initializes the terrain system by creating a dummy texture for the 2D map.
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
    Description: Populates chunks with procedural Perlin noise and builds the initial 3D mesh.
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

        // Apply seed to global offsets to generate different infinite worlds
        int offsetX = chunkData->chunkX * CHUNK_SIZE + config.seed;
        int offsetZ = chunkData->chunkZ * CHUNK_SIZE + config.seed;

        Image noiseImage = GenImagePerlinNoise(CHUNK_SIZE, CHUNK_SIZE, offsetX, offsetZ, config.noiseScale);
        Color *colors = LoadImageColors(noiseImage);

        for (int z = 0; z < CHUNK_SIZE; z++)
        {
            for (int x = 0; x < CHUNK_SIZE; x++)
            {
                chunkData->noiseMap[z * CHUNK_SIZE + x] = colors[z * CHUNK_SIZE + x].r;
                chunkData->biomeMap[z * CHUNK_SIZE + x] = BiomeType::DEFAULT;

                BuildColumn(chunkData, x, z, config);
            }
        }
        UnloadImageColors(colors);
        UnloadImage(noiseImage);
    }
    config.needsRegen = false;
    config.needsMapUpdate = true;
}

/*
    Function: TerrainSystem::ApplyPaint
    Description: Central hub for all topological sculpting tools and area-of-effect brushes.
*/
void TerrainSystem::ApplyPaint(EntityManagerMeta &em, const std::vector<EntityMeta> &chunks, TerrainConfig &config)
{
    // RATE LIMITER: Process brush strokes at ~20 FPS to prevent GPU mesh-rebuild bottlenecks
    static float paintTimer = 0.0f;
    paintTimer += GetFrameTime();
    if (paintTimer < 0.05f)
    {
        return;
    }

    paintTimer = 0.0f;

    float paintSpeed = config.brushStrength * 0.05f;

    // Safely read global noise data across chunk boundaries (Used by smoothing tools)
    auto GetGlobalNoise = [&](int px, int pz, float fallback) -> float
    {
        if (px < 0 || px >= WORLD_PIXELS || pz < 0 || pz >= WORLD_PIXELS)
        {
            return fallback;
        }

        int chunkIndex = (px / CHUNK_SIZE) * WORLD_CHUNKS + (pz / CHUNK_SIZE);
        if (chunkIndex >= 0 && chunkIndex < chunks.size())
        {
            EntityMeta ent = chunks[chunkIndex];
            ChunkCMP *cData = em.getComponent<ChunkCMP>(ent.getNextId());
            if (cData)
            {
                return cData->noiseMap[(pz % CHUNK_SIZE) * CHUNK_SIZE + (px % CHUNK_SIZE)] / 255.0f;
            }
        }
        return fallback;
    };

    Vector2 brushCenter = {(float)config.paintX, (float)config.paintZ};
    float radius = (float)config.brushSize;

    // Iterate over the bounding box of the brush
    for (int pZ = config.paintZ - config.brushSize - 1; pZ <= config.paintZ + config.brushSize + 1; pZ++)
    {
        for (int pX = config.paintX - config.brushSize - 1; pX <= config.paintX + config.brushSize + 1; pX++)
        {
            // Ensure we stay within world boundaries
            if (pX >= 0 && pX < WORLD_PIXELS && pZ >= 0 && pZ < WORLD_PIXELS)
            {
                float intensity = 0.0f;
                float dist = Vector2Distance(brushCenter, {(float)pX, (float)pZ});

                // --- BRUSH SHAPE EVALUATION ---
                if (config.brushShape == BrushShape::CIRCLE_SOFT)
                {
                    if (dist > radius)
                    {
                        continue;
                    }

                    // Quadratic falloff for smooth blending
                    intensity = 1.0f - ((dist / radius) * (dist / radius));
                }
                else if (config.brushShape == BrushShape::CIRCLE_HARD)
                {
                    if (dist > radius)
                    {
                        continue;
                    }
                    intensity = 1.0f;
                }
                else if (config.brushShape == BrushShape::SQUARE)
                {
                    if (abs(pX - config.paintX) > radius || abs(pZ - config.paintZ) > radius)
                    {
                        continue;
                    }
                    intensity = 1.0f;
                }
                else if (config.brushShape == BrushShape::NOISE)
                {
                    if (dist > radius)
                    {
                        continue;
                    }
                    // Splatter effect: Only affect ~30% of pixels randomly
                    if (((rand() % 100) / 100.0f) > 0.7f)
                        intensity = 1.0f;
                }

                if (intensity <= 0.0f)
                {
                    continue;
                }

                // chunk lookup
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

                        // --- SCULPTING TOOL APPLICATION ---
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
                            // Box Blur: Averages the adjacent cells to soften geometry
                            float avg = (GetGlobalNoise(pX - 1, pZ, n) + GetGlobalNoise(pX + 1, pZ, n) +
                                         GetGlobalNoise(pX, pZ - 1, n) + GetGlobalNoise(pX, pZ + 1, n)) /
                                        4.0f;
                            n = n + (avg - n) * finalPower * 4.0f;
                        }
                        else if (config.activeBrush == BrushType::ROUGHEN)
                        {
                            // Adds micro-jitter to create rough, natural rock formations
                            n = n + (((rand() % 100) / 100.0f - 0.5f) * 0.1f) * finalPower * 4.0f;
                        }
                        else if (config.activeBrush == BrushType::TERRACE)
                        {
                            // Quantize the noise to create geological steps
                            float terraces = 15.0f;
                            float stepped = round(n * terraces) / terraces;
                            n = n + (stepped - n) * finalPower * 3.0f;
                        }
                        else if (config.activeBrush == BrushType::SHARPEN)
                        {
                            // Pushes the terrain away from the local average, sharpening ridges
                            float avg = (GetGlobalNoise(pX - 1, pZ, n) + GetGlobalNoise(pX + 1, pZ, n) +
                                         GetGlobalNoise(pX, pZ - 1, n) + GetGlobalNoise(pX, pZ + 1, n)) /
                                        4.0f;
                            n = n + (n - avg) * finalPower * 5.0f;
                        }

                        // Clamp values to prevent heightmap overflow
                        if (n > 1.0f)
                            n = 1.0f;
                        if (n < 0.0f)
                            n = 0.0f;

                        chunkData->noiseMap[flatIndex] = (uint8_t)(n * 255.0f);

                        // Clear explicit biomes (like rivers) when sculpting over them
                        chunkData->biomeMap[flatIndex] = BiomeType::DEFAULT;

                        // Rebuild the physical 3D column with the new noise value
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
    Description: Updates the 2D map texture based on the current state of the terrain.
*/
void TerrainSystem::UpdateMapTexture(EntityManagerMeta &em, const std::vector<EntityMeta> &chunks, TerrainConfig &config)
{
    // Static vector prevents allocating/freeing memory (malloc/free)
    // 60 times per second during painting. Memory is allocated only once.
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

                // Apply explicit painted biome colors
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
                    // Procedural generation: Map the height (noise) to a topographical color palette
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

    // Upload the pixels directly from the pre-allocated vector to the GPU
    UpdateTexture(mapTexture, pixels.data());
    config.needsMapUpdate = false;
}

/*
    Function: TerrainSystem::Unload
    Description: Unloads the map texture from GPU memory to free up VRAM resources.
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
                 and stores it in the undo history stack before a brush stroke begins.
*/
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

    // Add the captured state to the undo history stack
    undoHistory.push_back(state);

    // Limit history stack size to prevent excessive RAM usage
    if (undoHistory.size() > 20)
    {
        undoHistory.erase(undoHistory.begin());
    }
}

/*
    Function: TerrainSystem::Undo
    Description: Restores the terrain to the previous snapshot by popping the undo history stack.
*/
void TerrainSystem::Undo(EntityManagerMeta &em, const std::vector<EntityMeta> &chunks, TerrainConfig &config)
{
    if (undoHistory.empty())
        return;

    // Retrieve and remove the most recent snapshot from the history stack
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

                // Restore the previous noise value from the global history state
                chunkData->noiseMap[flatIndex] = state.noise[globalIndex];

                // Reset the biome map to DEFAULT so procedural rules take over
                chunkData->biomeMap[flatIndex] = BiomeType::DEFAULT;

                // Force a complete regeneration of the 3D voxel column based on restored noise
                BuildColumn(chunkData, x, z, config);
            }
        }
    }
    config.needsMapUpdate = true;
}