#pragma once
#include <cstdint>
#include <vector>
#include <raylib.h>

constexpr int CHUNK_SIZE = 16;
constexpr int CHUNK_HEIGHT = 256;
constexpr int CHUNK_VOLUME = CHUNK_SIZE * CHUNK_HEIGHT * CHUNK_SIZE;
constexpr int WORLD_CHUNKS = 16;
constexpr int WORLD_PIXELS = WORLD_CHUNKS * CHUNK_SIZE;

enum class VoxelType : uint8_t
{
    EMPTY = 0,
    DIRT,
    GRASS,
    WATER,
    STONE
};


enum class BiomeType : uint8_t
{
    DEFAULT = 0,
    PLAINS,
    MOUNTAIN,
    RIVER
};

struct ChunkCMP
{
    int chunkX{0};
    int chunkZ{0};

    std::vector<VoxelType> voxels{std::vector<VoxelType>(CHUNK_VOLUME, VoxelType::EMPTY)};
    Model meshModel = {0};
    bool isDirty = true;

   
    BiomeType biomeMap[CHUNK_SIZE * CHUNK_SIZE] = {BiomeType::DEFAULT};
    uint8_t noiseMap[CHUNK_SIZE * CHUNK_SIZE] = {0};

    VoxelType GetVoxel(int x, int y, int z) const
    {
        if (x < 0 || x >= CHUNK_SIZE || y < 0 || y >= CHUNK_HEIGHT || z < 0 || z >= CHUNK_SIZE)
            return VoxelType::EMPTY;
        return voxels[x + (z * CHUNK_SIZE) + (y * CHUNK_SIZE * CHUNK_SIZE)];
    }

    void SetVoxel(int x, int y, int z, VoxelType type)
    {
        if (x >= 0 && x < CHUNK_SIZE && y >= 0 && y < CHUNK_HEIGHT && z >= 0 && z < CHUNK_SIZE)
        {
            voxels[x + (z * CHUNK_SIZE) + (y * CHUNK_SIZE * CHUNK_SIZE)] = type;
            isDirty = true;
        }
    }
};