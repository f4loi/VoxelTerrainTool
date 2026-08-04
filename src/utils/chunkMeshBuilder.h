#pragma once
#include <raylib.h>
#include <vector>
#include "VoxelEngine/CMP/ChunkCMP.h"

class ChunkMeshBuilder {
public:
    static Model BuildMesh(const ChunkCMP& chunk);
};