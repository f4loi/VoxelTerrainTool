#pragma once
#include <cstdint>
#include <vector>
#include <raylib.h>

/*
    =========================================================================
                                Chunk Constants
    =========================================================================
*/
constexpr int CHUNK_SIZE = 16;
constexpr int CHUNK_HEIGHT = 256;
constexpr int CHUNK_VOLUME = CHUNK_SIZE * CHUNK_HEIGHT * CHUNK_SIZE;
constexpr int WORLD_CHUNKS = 16;
constexpr int WORLD_PIXELS = WORLD_CHUNKS * CHUNK_SIZE;

/*
    =========================================================================
                                Chunk Data Structures
    =========================================================================
*/
enum class VoxelType : uint8_t
{
    EMPTY = 0,
    DIRT,
    GRASS,
    WATER,
    STONE,
    SAND,
    SNOW
};

enum class BiomeType : uint8_t
{
    DEFAULT = 0,
    PLAINS,
    MOUNTAIN,
    RIVER
};

/*
    =========================================================================
                                ChunkCMP Structure
    =========================================================================
    Description: Represents a chunk of voxels in the voxel engine, including its position, voxel data, mesh model, and biome information.
*/
struct ChunkCMP
{
    // Position of the chunk in the world (chunk coordinates)
    int chunkX{0};
    int chunkZ{0};

    // Voxel data for the chunk, stored in a 1D vector (flattened 3D array)
    std::vector<VoxelType> voxels{std::vector<VoxelType>(CHUNK_VOLUME, VoxelType::EMPTY)};
    Model meshModel = {0};
    // Biome and noise maps for the chunk, used for terrain generation and biome assignment
    bool isDirty = true;
    BiomeType biomeMap[CHUNK_SIZE * CHUNK_SIZE] = {BiomeType::DEFAULT};
    uint8_t noiseMap[CHUNK_SIZE * CHUNK_SIZE] = {0};

    /*
        Function: GetVoxel
        Description: Retrieves the voxel type at the specified coordinates within the chunk.
        Parameters:
            - int x, y, z: The coordinates of the voxel to retrieve.
        Returns:
            - VoxelType: The type of the voxel at the specified coordinates.
    */
    VoxelType GetVoxel(int x, int y, int z) const
    {
        if (x < 0 || x >= CHUNK_SIZE || y < 0 || y >= CHUNK_HEIGHT || z < 0 || z >= CHUNK_SIZE)
        {
            return VoxelType::EMPTY; // Return EMPTY for out-of-bounds access
        }
        // Calculate the index in the 1D vector based on the 3D coordinates and return the voxel type
        return voxels[x + (z * CHUNK_SIZE) + (y * CHUNK_SIZE * CHUNK_SIZE)];
    }

    /*
        Function: SetVoxel
        Description: Sets the voxel type at the specified coordinates within the chunk.
        Parameters:
            - int x, y, z: The coordinates of the voxel to set.
            - VoxelType type: The type of the voxel to set.
    */
    void SetVoxel(int x, int y, int z, VoxelType type)
    {
        // Check if the coordinates are within the valid range of the chunk
        if (x >= 0 && x < CHUNK_SIZE && y >= 0 && y < CHUNK_HEIGHT && z >= 0 && z < CHUNK_SIZE)
        {
            // Calculate the index in the 1D vector based on the 3D coordinates and set the voxel type
            voxels[x + (z * CHUNK_SIZE) + (y * CHUNK_SIZE * CHUNK_SIZE)] = type;
            // Mark the chunk as dirty, indicating that its mesh needs to be rebuilt due to the voxel change
            isDirty = true;
        }
    }
};