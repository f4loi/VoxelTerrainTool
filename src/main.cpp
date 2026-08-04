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

    EntityMeta chunkEntity = em.createEntity();
    em.addComponent(chunkEntity.getNextId(), ChunkCMP{});
    em.addComponent(chunkEntity.getNextId(), RenderCMP{true});

    config.needsRegen = true; 

    while (!renderSys.WindowShouldClose())
    {
        // --- 1. ACTUALIZAR CÁMARA ---
        camManager.Update(ui.GetIsViewport3DHovered());

        // --- 2. SISTEMA DE TERRENO (El Director de Orquesta) ---
        if (config.needsRegen) terrainSys.GenerateTerrain(em, chunkEntity, config);
        
        if (config.isPainting) terrainSys.ApplyPaint(em, chunkEntity, config);
        
        if (config.needsMapUpdate) terrainSys.UpdateMapTexture(em, chunkEntity, config);


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