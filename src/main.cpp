#include <raylib.h>
#include "VoxelEngine/Managers/EntityManagerMeta.h"
#include "VoxelEngine/Systems/RenderSystemMeta.h"
#include "VoxelEngine/Managers/CameraManager.h"
#include "VoxelEngine/Systems/uiSys.h"
#include "VoxelEngine/Systems/TerrainSystem.h"
#include "Config/TerrainConfig.h"

int main()
{
    // =====================================================================
    //                      ENGINE INITIALIZATION
    // =====================================================================

    // Configure the main application window to be resizable by the user
    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(1280, 720, "Generador de Biomas - Modo Editor");

    // Cap the framerate to 60 FPS
    SetTargetFPS(60);

    // =====================================================================
    //                      ECS & SYSTEM INSTANTIATION
    // =====================================================================

    // Instantiate the Entity-Component-System (ECS) manager and all subsystems
    EntityManagerMeta em;
    RenderSystemMeta renderSys;
    uiSys ui;
    CameraManager camManager;
    TerrainSystem terrainSys;
    TerrainConfig config;

    // Initialize subsystems (allocates textures, sets up ImGui context, etc.)
    renderSys.Init();
    ui.Init();
    camManager.Init();
    terrainSys.Init();

    // Attempt to load the user's previous configuration from disk
    config.LoadFromJson("config.json");

    // =====================================================================
    //                      WORLD GENERATION SETUP (CHUNK ALLOCATION)
    // =====================================================================

    std::vector<EntityMeta> worldChunks;

    // Pre-allocate memory for the chunks vector to avoid dynamic reallocation overhead
    worldChunks.reserve(WORLD_CHUNKS * WORLD_CHUNKS);

    // Create the grid of chunks that will make up the 3D world
    for (int x = 0; x < WORLD_CHUNKS; x++)
    {
        for (int z = 0; z < WORLD_CHUNKS; z++)
        {
            // Register a new entity in the ECS
            EntityMeta chunkEntity = em.createEntity();

            // Initialize the data component for this specific chunk
            ChunkCMP chunkData;
            chunkData.chunkX = x;
            chunkData.chunkZ = z;

            // Attach the necessary components to the entity
            em.addComponent(chunkEntity.getNextId(), chunkData);
            em.addComponent(chunkEntity.getNextId(), RenderCMP{true});

            // Store the entity reference in our active world list
            worldChunks.push_back(chunkEntity);
        }
    }

    // Flag the world to be generated procedurally on the very first frame
    config.needsRegen = true;

    // =====================================================================
    //                          MAIN GAME LOOP
    // =====================================================================

    while (!renderSys.WindowShouldClose())
    {
        // ---------------------------------------------------------
        //           UI COMMAND PROCESSING (EVENT POLLING)
        // ---------------------------------------------------------
        // Process any flags triggered by the user via the UI in the previous frame

        if (config.needsLoad)
        {
            config.LoadFromJson("config.json");
            // Force a rebuild with the loaded seed/noise
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
            // Take a snapshot of the terrain before the user starts painting
            terrainSys.SaveUndoState(em, worldChunks);
            config.needsUndoSave = false;
        }
        if (config.needsUndo)
        {
            // Restore the terrain to the last saved snapshot
            terrainSys.Undo(em, worldChunks, config);
            config.needsUndo = false;
        }

        // ---------------------------------------------------------
        //                   CAMERA UPDATE
        // ---------------------------------------------------------
        // Update the 3D camera transforms, but only if the user is interacting
        // with the 3D viewport (ignoring input if they are using the UI panels)
        camManager.Update(ui.GetIsViewport3DHovered());

        // ---------------------------------------------------------
        //                   TERRAIN PIPELINE
        // ---------------------------------------------------------
        // Step 1: Fully regenerate the world procedurally if requested
        if (config.needsRegen)
        {
            terrainSys.GenerateTerrain(em, worldChunks, config);
        }
        // Step 2: Apply localized sculpting/painting modifications
        if (config.isPainting)
        {
            terrainSys.ApplyPaint(em, worldChunks, config);
        }
        // Step 3: Synchronize the 2D heightmap/biome texture if the data changed
        if (config.needsMapUpdate)
        {
            terrainSys.UpdateMapTexture(em, worldChunks, config);
        }

        // ---------------------------------------------------------
        //                  OFF-SCREEN 3D RENDERING
        // ---------------------------------------------------------
        // Render the 3D world into a Framebuffer (RenderTexture) instead of
        // the main screen. This isolates the 3D scene from the UI layer.
        renderSys.Update(em, camManager.GetCamera());

        // ---------------------------------------------------------
        //                  MAIN DISPLAY & UI RENDERING
        // ---------------------------------------------------------
        // Final composition: Draw the ImGui interface on the main screen,
        // passing the generated 2D and 3D textures to be displayed inside the UI viewports.
        BeginDrawing();
        ClearBackground(DARKGRAY);

        ui.Draw(config, renderSys.GetTarget3D(), terrainSys.GetMapTexture());

        EndDrawing();
    }

    // =====================================================================
    //                      CLEANUP & SHUTDOWN
    // =====================================================================

    // Gracefully unload GPU assets (Textures, Models, VBOs) to prevent memory leaks
    terrainSys.Unload();
    renderSys.Unload();
    ui.Close();

    // Destroy the application window and OpenGL context
    CloseWindow();

    return 0;
}