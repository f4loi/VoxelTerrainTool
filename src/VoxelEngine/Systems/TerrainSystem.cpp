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
    // Extract the noise and biome data for the current column
    int flatIndex = z * CHUNK_SIZE + x;
    uint8_t noiseValue = chunkData->noiseMap[flatIndex];
    BiomeType biome = chunkData->biomeMap[flatIndex];

    // Normalize the noise value to a range of 0.0 to 1.0 for terrain height calculations
    float n = noiseValue / 255.0f;

    int terrainHeight = 0;
    VoxelType surface = VoxelType::GRASS;
    VoxelType subSurface = VoxelType::DIRT;

    int waterLvl = config.waterLevel;

    /*
        TERRAIN GENERATION LOGIC BASED ON BIOME AND NOISE
        This section determines the terrain height and voxel types based on the biome and noise value.
        Different biomes have different terrain characteristics, such as mountains, plains, and rivers.
    */
    if (biome == BiomeType::DEFAULT)
    {
        // Zone 1: Lowlands and Valleys (0.0 to 0.35)
        if (n < 0.35f)
        {
            float localN = n / 0.35f;
            terrainHeight = 10 + (int)(localN * (waterLvl - 5 - 10));
            surface = VoxelType::DIRT;
            subSurface = VoxelType::DIRT;
        }
        else if (n < 0.40f)
        {
            // Zone 2: Beaches and Coastlines (0.35 to 0.40)
            float localN = (n - 0.35f) / 0.05f;
            terrainHeight = (waterLvl - 5) + (int)(localN * 7);
            surface = VoxelType::SAND;
            subSurface = VoxelType::SAND;
        }
        else if (n < 0.60f)
        {
            // Zone 3: Plains and Gentle Hills (0.40 to 0.60)
            float localN = (n - 0.40f) / 0.20f;
            // Use a power function to create a more natural slope for the terrain height
            float curve = pow(localN, 1.5f);
            // Calculate the terrain height based on the water level and the curve value, allowing for gentle hills
            terrainHeight = (waterLvl + 2) + (int)(curve * 30);
            surface = VoxelType::GRASS;
            subSurface = VoxelType::DIRT;
        }
        else
        {
            // Zone 4: Steep Mountains (0.60 to 1.0)
            float localN = (n - 0.60f) / 0.40f;
            // Use a combination of linear and cubic functions to create steep mountain peaks with more variation
            float curve = (localN * 0.4f) + (pow(localN, 3.0f) * 0.6f);
            // Calculate the terrain height based on the water level and the curve value, allowing for steep mountains with peaks
            terrainHeight = (waterLvl + 32) + (int)(curve * 140);

            // Determine the surface and subsurface voxel types based on the terrain height, creating a transition from grass to stone to snow at higher elevations
            if (terrainHeight > waterLvl + 80)
            {
                surface = VoxelType::SNOW;
                subSurface = VoxelType::SNOW;
            }
            else if (terrainHeight > waterLvl + 50)
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
    }

    /*
        Voxel Column Construction
        This section constructs the vertical column of voxels based on the calculated terrain height and voxel types.
        It iterates through the y-axis of the chunk, setting the appropriate voxel type for each level based on the terrain height and water level.
    */
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

        int offsetX = chunkData->chunkX * CHUNK_SIZE;
        int offsetZ = chunkData->chunkZ * CHUNK_SIZE;
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
    // brushCenter represents the center of the brush in world coordinates, and radius is the size of the brush.
    Vector2 brushCenter = {(float)config.paintX, (float)config.paintZ};
    float radius = (float)config.brushSize;

    /*
        Iterate through the area affected by the brush, checking each voxel column within the brush's radius.
        For each column, calculate the intensity of the painting effect based on the distance from the brush center.
        Apply changes to the noise and biome maps of the affected chunks, and rebuild the voxel columns accordingly.
    */
    for (int pZ = config.paintZ - config.brushSize - 1; pZ <= config.paintZ + config.brushSize + 1; pZ++)
    {
        for (int pX = config.paintX - config.brushSize - 1; pX <= config.paintX + config.brushSize + 1; pX++)
        {
            // Check if the current position is within the bounds of the world
            if (pX >= 0 && pX < WORLD_PIXELS && pZ >= 0 && pZ < WORLD_PIXELS)
            {
                float intensity = 1.0f;

                // Determine the intensity of the painting effect based on the brush shape (circular or square) and the distance from the brush center.
                if (!config.isSquareBrush)
                {
                    // Circular Brush: Calculate the distance from the center
                    float dist = Vector2Distance(brushCenter, {(float)pX, (float)pZ});
                    if (dist > radius)
                    {
                        continue; // Skip if outside the brush radius
                    }
                    // Linear falloff: 1.0 at the center, 0.0 at the edge
                    intensity = 1.0f - (dist / radius);
                    // Quadratic falloff: Squaring the intensity creates a much smoother transition from the center to the edge of the brush, resulting in a more natural painting effect.
                    intensity = intensity * intensity;
                }
                else
                {
                    // Square Brush (3D Cube): Straight boundaries using absolute distance
                    if (abs(pX - config.paintX) > radius || abs(pZ - config.paintZ) > radius)
                    {
                        continue; // Skip if outside the square brush area
                    }

                    intensity = 0.4f; // Plane intensity for the square brush
                }
                // Calculate the chunk coordinates (cX, cZ) and local coordinates (lX, lZ) within the chunk for the current position (pX, pZ).
                int cX = pX / CHUNK_SIZE;
                int cZ = pZ / CHUNK_SIZE;
                int lX = pX % CHUNK_SIZE;
                int lZ = pZ % CHUNK_SIZE;

                /*
                    Iterate through the chunks to find the one that matches the calculated chunk coordinates (cX, cZ).
                    Once the matching chunk is found, extract the noise value for the current local coordinates (lX, lZ) and apply biome-specific modifications based on the selected biome in the configuration.
                    Update the noise and biome maps of the chunk, and rebuild the voxel column for the affected position.
                */
                for (EntityMeta ent : chunks)
                {
                    ChunkCMP *chunkData = em.getComponent<ChunkCMP>(ent.getNextId());
                    if (chunkData && chunkData->chunkX == cX && chunkData->chunkZ == cZ)
                    {
                        // Calculate the flat index for the local coordinates (lX, lZ) within the chunk's noise and biome maps.
                        int flatIndex = lZ * CHUNK_SIZE + lX;

                        // obtain the current noise value for the column and normalize it to a range of 0.0 to 1.0 for further processing.
                        float n = chunkData->noiseMap[flatIndex] / 255.0f;

                        // Apply biome-specific modifications to the noise value based on the selected biome in the configuration. Each biome has different characteristics that affect the terrain height and appearance.
                        if (config.selectedBiome == PaintBiome::MOUNTAIN)
                        {
                            // Increase the noise value to create higher terrain for mountains, with a stronger effect based on the intensity of the painting.
                            n = n + (0.95f - n) * intensity * 0.6f;
                        }
                        else if (config.selectedBiome == PaintBiome::RIVER)
                        {
                            // Decrease the noise value to create lower terrain for rivers, with a stronger effect based on the intensity of the painting.
                            n = n + (0.25f - n) * intensity * 0.8f;
                        }
                        else if (config.selectedBiome == PaintBiome::PLAINS)
                        {
                            // Slightly increase the noise value to create gentle hills for plains, with a moderate effect based on the intensity of the painting.
                            n = n + (0.45f - n) * intensity * 0.5f;
                        }

                        if (n > 1.0f)
                        {
                            n = 1.0f;
                        }

                        if (n < 0.0f)
                        {
                            n = 0.0f;
                        }

                        // Update the noise map of the chunk with the modified noise value, converting it back to an 8-bit unsigned integer representation (0-255) for storage.
                        chunkData->noiseMap[flatIndex] = (uint8_t)(n * 255.0f);

                        // Reset the biome map for the affected column to the default biome, as the painting effect may have changed the terrain characteristics.
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
    // Allocate memory for the pixel data of the map texture, which will be filled based on the biome and noise data of the chunks.
    Color *pixels = (Color *)malloc(WORLD_PIXELS * WORLD_PIXELS * sizeof(Color));

    for (EntityMeta ent : chunks)
    {
        ChunkCMP *chunkData = em.getComponent<ChunkCMP>(ent.getNextId());
        if (chunkData == nullptr)
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
                BiomeType biome = chunkData->biomeMap[flatIndex];
                Color c;

                // Color the 2D map based on the explicit biome applied by the brush
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
                    // Default procedural generation: Use the Perlin noise value as a grayscale color
                    uint8_t n = chunkData->noiseMap[flatIndex];
                    c = Color{n, n, n, 255};
                }

                pixels[(offsetZ + z) * WORLD_PIXELS + (offsetX + x)] = c;
            }
        }
    }

    UpdateTexture(mapTexture, pixels);
    free(pixels);
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