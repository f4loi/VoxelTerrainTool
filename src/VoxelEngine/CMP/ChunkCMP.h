#pragma once
#include <cstdint>
#include <vector>

constexpr int CHUNK_SIZE = 16;
constexpr int CHUNK_VOLUME = CHUNK_SIZE * CHUNK_SIZE * CHUNK_SIZE;

enum class VoxelType : uint8_t {
    EMPTY = 0,
    DIRT,
    GRASS,
    WATER,
    STONE
};

struct ChunkCMP {
    int chunkX{0}; 
    int chunkZ{0};
    
    std::vector<VoxelType> voxels{std::vector<VoxelType>(CHUNK_VOLUME, VoxelType::EMPTY)};
    
    VoxelType GetVoxel(int x, int y, int z) const {
        if (x < 0 || x >= CHUNK_SIZE || y < 0 || y >= CHUNK_SIZE || z < 0 || z >= CHUNK_SIZE) 
            return VoxelType::EMPTY;
        return voxels[x + y * CHUNK_SIZE + z * CHUNK_SIZE * CHUNK_SIZE];
    }

    void SetVoxel(int x, int y, int z, VoxelType type) {
        if (x >= 0 && x < CHUNK_SIZE && y >= 0 && y < CHUNK_SIZE && z >= 0 && z < CHUNK_SIZE) {
            voxels[x + y * CHUNK_SIZE + z * CHUNK_SIZE * CHUNK_SIZE] = type;
        }
    }
};