#pragma once
#include <raylib.h>
#include "VoxelEngine/Managers/EntityManagerMeta.h"
#include "Config/TerrainConfig.h"

class TerrainSystem {
private:
    Texture2D mapTexture = { 0 };

public:
    void Init();
    void GenerateTerrain(EntityManagerMeta& em, EntityMeta chunkEntity, TerrainConfig& config);
    void ApplyPaint(EntityManagerMeta& em, EntityMeta chunkEntity, TerrainConfig& config);
    void UpdateMapTexture(EntityManagerMeta& em, EntityMeta chunkEntity, TerrainConfig& config);
    void Unload();

    // Getter para que la interfaz pueda dibujar el mapa 2D
    Texture2D* GetMapTexture() { return &mapTexture; } 
};