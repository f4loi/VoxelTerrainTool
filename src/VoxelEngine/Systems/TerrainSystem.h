#pragma once
#include <raylib.h>
#include "VoxelEngine/Managers/EntityManagerMeta.h"
#include "Config/TerrainConfig.h"

class TerrainSystem {
private:
    Texture2D mapTexture = { 0 };
    struct TerrainState {
        std::vector<uint8_t> noise;
    };
    std::vector<TerrainState> undoHistory;

public:
    void Init();
    void GenerateTerrain(EntityManagerMeta& em, const std::vector<EntityMeta>& chunks, TerrainConfig& config);
    void ApplyPaint(EntityManagerMeta& em, const std::vector<EntityMeta>& chunks, TerrainConfig& config);
    void UpdateMapTexture(EntityManagerMeta& em, const std::vector<EntityMeta>& chunks, TerrainConfig& config);
    void Unload();
    void SaveUndoState(EntityManagerMeta& em, const std::vector<EntityMeta>& chunks);
    void Undo(EntityManagerMeta& em, const std::vector<EntityMeta>& chunks, TerrainConfig& config);

    // Getter para que la interfaz pueda dibujar el mapa 2D
    Texture2D* GetMapTexture() { return &mapTexture; } 
};