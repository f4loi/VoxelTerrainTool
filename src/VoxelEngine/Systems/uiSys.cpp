#include "VoxelEngine/Systems/uiSys.h"
#include "imgui.h"
#include "rlImGui.h"
#include "VoxelEngine/Managers/EntityManagerMeta.h"

// =====================================================================
// INITIALIZATION & SHUTDOWN
// =====================================================================

void uiSys::Init()
{
    // Initialize the raylib-ImGui binding
    rlImGuiSetup(true);
    
    // Retrieve the ImGui IO context to modify core flags
    ImGuiIO &io = ImGui::GetIO();
    
    // Enable docking: This allows the user to snap, split, and merge 
    // the 2D and 3D environment windows dynamically across the screen
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
}

void uiSys::Close()
{
    // Properly shut down the ImGui context to free up memory and GPU resources
    rlImGuiShutdown();
}

// =====================================================================
// MAIN RENDERING LOOP
// =====================================================================

void uiSys::Draw(TerrainConfig &config, RenderTexture2D *target3D, Texture2D *mapTexture)
{
    // Begin a new ImGui frame
    rlImGuiBegin();

    // ---------------------------------------------------------
    //          MAIN WORKSPACE SETUP (DOCKSPACE)
    // ---------------------------------------------------------
    // Create a docking space that covers the entire main viewport.
    // 'PassthruCentralNode' ensures the application background remains visible
    // if no ImGui windows are actively docked in the central area.
    ImGui::DockSpaceOverViewport(ImGui::GetMainViewport()->ID, ImGui::GetMainViewport(), ImGuiDockNodeFlags_PassthruCentralNode);

    // ---------------------------------------------------------
    //          LEFT COLUMN: 2D ENVIRONMENT & SETTINGS
    // ---------------------------------------------------------
    ImGui::Begin("2D Environment");

    // --- FILE SYSTEM SETTINGS ---
    ImGui::Text("FILE SYSTEM");
    if (ImGui::Button("Save Config (JSON)"))
    {
        config.needsSave = true; // Flags the engine to serialize current state
    }
    ImGui::SameLine();
    if (ImGui::Button("Load Config (JSON)"))
    {
        config.needsLoad = true; // Flags the engine to parse the saved JSON state
    }
    ImGui::Separator();

    // --- EXPORT SETTINGS ---
    ImGui::Text("EXPORT");
    if (ImGui::Button("Export to UE5 (JSON)"))
    {
        config.needsExportUE5 = true; // Triggers the Unreal Engine 5 specific exporter
    }
    ImGui::Separator();

    // --- BASE PROCEDURAL GENERATION ---
    ImGui::Text("BASE GENERATION");
    
    // Seed input for deterministic procedural generation
    ImGui::InputInt("Seed", &config.seed);
    if (ImGui::IsItemDeactivatedAfterEdit())
    {
        config.needsRegen = true;
    }
    ImGui::SameLine();
    
    // Button to instantly generate a completely new random seed and refresh the terrain
    if (ImGui::Button("Random"))
    {
        config.seed = GetRandomValue(0, 9999999);
        config.needsRegen = true;
    }

    // Controls the global zoom/frequency of the Perlin Noise mapping
    ImGui::SliderFloat("Noise Scale", &config.noiseScale, 0.01f, 1.5f, "%.2f");
    if (ImGui::IsItemDeactivatedAfterEdit())
    {
        config.needsRegen = true; 
    }

    // Controls the global Y-axis height representing the ocean level
    ImGui::SliderInt("Water Level", &config.waterLevel, 1, 128);
    if (ImGui::IsItemDeactivatedAfterEdit())
    {
        config.needsRegen = true;
    }
    ImGui::Separator();

    // --- SCULPTING TOOLS & SHORTCUTS ---
    ImGui::Text("SCULPTING TOOLS");
    ImGui::SameLine();
    ImGui::TextDisabled("(?)"); // Interactive tooltip icon
    if (ImGui::IsItemHovered())
    {
        ImGui::BeginTooltip();
        ImGui::TextUnformatted("Keyboard Shortcuts:\n [ + / - ] Change brush size\n [ Up / Down ] Change brush strength\n [ Ctrl + Z ] Undo last stroke");
        ImGui::EndTooltip();
    }

    // Quality of Life (QoL): Keyboard shortcut polling for brush parameters
    if (IsKeyPressed(KEY_KP_ADD) || IsKeyPressed(KEY_EQUAL)) { config.brushSize++; }
    if (IsKeyPressed(KEY_KP_SUBTRACT) || IsKeyPressed(KEY_MINUS)) { config.brushSize--; if(config.brushSize < 1) config.brushSize = 1; }
    if (IsKeyPressed(KEY_UP)) { config.brushStrength += 0.05f; if(config.brushStrength > 1.0f) config.brushStrength = 1.0f; }
    if (IsKeyPressed(KEY_DOWN)) { config.brushStrength -= 0.05f; if(config.brushStrength < 0.01f) config.brushStrength = 0.01f; }

    // Undo system: Poll for Ctrl+Z shortcut or UI Button
    if ((IsKeyDown(KEY_LEFT_CONTROL) && IsKeyPressed(KEY_Z)) || ImGui::Button("Undo (Ctrl+Z)"))
    {
        config.needsUndo = true;
    }
    ImGui::SameLine();
    
    // Completely resets the sculpted world back to its original procedural noise state
    if (ImGui::Button("Reset World"))
    {
        config.needsRegen = true;
    }

    // --- BRUSH CONFIGURATION ---
    // Tool behavior dropdown
    const char *brushNames[] = {
        "Raise", "Lower", "Flatten",
        "Smooth", "Roughen",
        "Terrace", "Sharpen"
    };
    int currentBrush = (int)config.activeBrush;
    if (ImGui::Combo("Tool", &currentBrush, brushNames, 7))
    {
        config.activeBrush = (BrushType)currentBrush;
    }
    // Tooltip for advanced topological tools
    if (ImGui::IsItemHovered()) {
        ImGui::SetTooltip("Smooth: Softens peaks.\nTerrace: Creates geological steps.\nSharpen: Makes terrain sharp and steep.");
    }

    // Tool shape dropdown (Mathematical area of effect)
    const char *shapeNames[] = {"Circle (Soft)", "Circle (Hard)", "Square (Flat)", "Noise (Splatter)"};
    int currentShape = (int)config.brushShape;
    if (ImGui::Combo("Shape", &currentShape, shapeNames, 4))
    {
        config.brushShape = (BrushShape)currentShape;
    }

    // Global brush parameters
    ImGui::SliderInt("Brush Size", &config.brushSize, 1, 30);
    ImGui::SliderFloat("Brush Strength", &config.brushStrength, 0.01f, 1.0f, "%.2f");

    // Dynamic UI: Only show the target altitude slider if the 'Flatten' tool is actively selected
    if (config.activeBrush == BrushType::FLATTEN)
    {
        ImGui::SliderFloat("Flatten Target", &config.flattenTarget, 0.0f, 1.0f, "%.2f");
    }

    ImGui::Separator();

    // ---------------------------------------------------------
    //       2D VIEWPORT (MAP INTERACTION & RAYCASTING)
    // ---------------------------------------------------------
    ImGui::Text("2D VIEWPORT (Top-Down View)");

    // Retrieve available layout space to calculate max image dimensions
    ImVec2 size2D = ImGui::GetContentRegionAvail();
    if (mapTexture != nullptr && mapTexture->id != 0)
    {
        // Enforce an aspect ratio of 1:1 (Square) based on the smallest available dimension
        float minSize = (size2D.x < size2D.y) ? size2D.x : size2D.y;
        
        // Calculate offsets to perfectly center the map within the panel
        float offsetX = (size2D.x - minSize) * 0.5f;
        float offsetY = (size2D.y - minSize) * 0.5f;
        ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPosX() + offsetX, ImGui::GetCursorPosY() + offsetY));

        // Capture screen-space position to accurately map local mouse coordinates later
        ImVec2 imgPos = ImGui::GetCursorScreenPos();
        
        // Draw the generated 2D heightmap
        rlImGuiImageSize(mapTexture, (int)minSize, (int)minSize);

        // Process interaction ONLY if the mouse is currently over the 2D map texture
        if (ImGui::IsItemHovered())
        {
            ImVec2 mousePos = ImGui::GetMousePos();

            // --- VISUAL BRUSH OVERLAY ---
            // Render the tool's area of effect directly over the image in real-time
            ImDrawList *drawList = ImGui::GetWindowDrawList();
            
            // Transform the logical world brush radius into screen-space pixel radius
            float scale = minSize / (float)WORLD_PIXELS;
            float screenRadius = config.brushSize * scale;

            // Draw overlay according to the selected brush shape
            if (config.brushShape == BrushShape::SQUARE)
            {
                ImVec2 pMin = ImVec2(mousePos.x - screenRadius, mousePos.y - screenRadius);
                ImVec2 pMax = ImVec2(mousePos.x + screenRadius, mousePos.y + screenRadius);
                drawList->AddRect(pMin, pMax, IM_COL32(255, 255, 255, 200), 0.0f, 0, 2.0f);
            }
            else
            {
                // Soft, Hard, and Noise shapes all share a circular bounding box
                drawList->AddCircle(mousePos, screenRadius, IM_COL32(255, 255, 255, 200), 32, 2.0f);
            }

            // --- PAINTING LOGIC & RAYCASTING ---
            if (ImGui::IsMouseDown(ImGuiMouseButton_Left))
            {
                // Push current state to the Undo stack on the exact frame the click initiates
                if (ImGui::IsMouseClicked(ImGuiMouseButton_Left))
                {
                    config.needsUndoSave = true;
                }
                
                // Map the screen coordinates (pixels) to normalized UV coordinates (0.0 to 1.0)
                float u = (mousePos.x - imgPos.x) / minSize;
                float v = (mousePos.y - imgPos.y) / minSize;
                
                // Guardrail: Ensure coordinates remain strictly within the texture bounds
                if (u >= 0.0f && u <= 1.0f && v >= 0.0f && v <= 1.0f)
                {
                    // Scale UVs back to absolute world coordinates (Voxel Grid)
                    config.paintX = (int)(u * WORLD_PIXELS);
                    config.paintZ = (int)(v * WORLD_PIXELS);
                    
                    // Fire the painting event for the TerrainSystem to catch
                    config.isPainting = true;
                }
            }
            else 
            {
                // User is hovering but not clicking, halt painting
                config.isPainting = false; 
            }
        }
        else
        {
            // User mouse is outside the map boundaries, halt painting to prevent infinite drawing
            config.isPainting = false;
        }
    }
    else
    {
        // Render an invisible dummy box to preserve UI layout structure if the texture hasn't loaded
        ImGui::Dummy(size2D);
    }
    ImGui::End(); // End Left Column

    // ---------------------------------------------------------
    //       RIGHT COLUMN: 3D ENVIRONMENT & RENDER TARGET
    // ---------------------------------------------------------
    ImGui::Begin("3D Environment");

    // --- 3D AREA TOOLS (OVERLAYS) ---
    ImGui::Text("3D AREA TOOLS");
    ImGui::Text("FPS: %d", GetFPS()); // Real-time performance metric
    ImGui::Separator();

    // --- 3D VIEWPORT RENDERING ---
    // Draw the generated 3D World (rendered off-screen via Framebuffer) into this ImGui window.
    // 'true' forces the image to scale dynamically and fit the remaining window space.
    if (target3D != nullptr)
    {
        rlImGuiImageRenderTextureFit(target3D, true);
    }

    // Expose the hover state to the CameraManager so it knows whether to consume inputs (like right-click rotation)
    isViewport3DHovered = ImGui::IsWindowHovered();
    
    ImGui::End(); // End Right Column

    // Submit all UI draw calls to Raylib
    rlImGuiEnd();
}