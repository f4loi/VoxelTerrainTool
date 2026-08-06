#include <raylib.h>
#include "VoxelEngine/Managers/EntityManagerMeta.h"
#include "VoxelEngine/Systems/RenderSystemMeta.h"
#include "VoxelEngine/Managers/CameraManager.h"
#include "VoxelEngine/Systems/uiSys.h"
#include "VoxelEngine/Systems/TerrainSystem.h" // <-- Tu nuevo sistema
#include "Config/TerrainConfig.h"

int main()
{
    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(1280, 720, "Generador de Biomas - Modo Editor");
    SetTargetFPS(60);

    EntityManagerMeta em;
    RenderSystemMeta renderSys;
    uiSys ui;
    CameraManager camManager;
    TerrainSystem terrainSys; // <-- Instanciamos el sistema
    TerrainConfig config;

    renderSys.Init();
    ui.Init();
    camManager.Init();
    terrainSys.Init(); // <-- Lo inicializamos

    config.LoadFromJson("config.json");

    std::vector<EntityMeta> worldChunks;
    worldChunks.reserve(WORLD_CHUNKS * WORLD_CHUNKS);

    for (int x = 0; x < WORLD_CHUNKS; x++)
    {
        for (int z = 0; z < WORLD_CHUNKS; z++)
        {
            EntityMeta chunkEntity = em.createEntity();

            ChunkCMP chunkData;
            chunkData.chunkX = x;
            chunkData.chunkZ = z;

            em.addComponent(chunkEntity.getNextId(), chunkData);
            em.addComponent(chunkEntity.getNextId(), RenderCMP{true});

            worldChunks.push_back(chunkEntity);
        }
    }

    config.needsRegen = true;

    while (!renderSys.WindowShouldClose())
    {
        if (config.needsLoad)
        {
            config.LoadFromJson("config.json");
            config.needsRegen = true;
            config.needsLoad = false;
        }
        if (config.needsSave)
        {
            config.SaveToJson("config.json");
            config.needsSave = false;
        }
        if (config.needsUndoSave)
        {
            terrainSys.SaveUndoState(em, worldChunks);
            config.needsUndoSave = false;
        }
        if (config.needsUndo)
        {
            terrainSys.Undo(em, worldChunks, config);
            config.needsUndo = false;
        }

        // --- 1. ACTUALIZAR CÁMARA ---
        camManager.Update(ui.GetIsViewport3DHovered());

        // --- 2. SISTEMA DE TERRENO (El Director de Orquesta) ---
        if (config.needsRegen)
            terrainSys.GenerateTerrain(em, worldChunks, config);

        if (config.isPainting)
            terrainSys.ApplyPaint(em, worldChunks, config);

        if (config.needsMapUpdate)
            terrainSys.UpdateMapTexture(em, worldChunks, config);

        // --- 3. RENDERIZAR MUNDO 3D (A textura invisible) ---
        renderSys.Update(em, camManager.GetCamera());

        // --- 4. RENDERIZAR LA INTERFAZ FINAL ---
        BeginDrawing();
        ClearBackground(DARKGRAY);

        // Le pasamos las texturas a la interfaz
        ui.Draw(config, renderSys.GetTarget3D(), terrainSys.GetMapTexture());

        EndDrawing();
    }

    // --- 5. LIMPIEZA ---
    terrainSys.Unload();
    renderSys.Unload();
    ui.Close();
    CloseWindow();

    return 0;
}