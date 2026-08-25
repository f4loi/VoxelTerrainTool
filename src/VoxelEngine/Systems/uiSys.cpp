#include "VoxelEngine/Systems/uiSys.h"
#include "imgui.h"
#include "rlImGui.h"
#include "VoxelEngine/Managers/EntityManagerMeta.h"

void uiSys::Init()
{
    rlImGuiSetup(true);
    ImGuiIO &io = ImGui::GetIO();
    // Enable docking to allow windows to snap, split, and merge together dynamically
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
}

void uiSys::Draw(TerrainConfig &config, RenderTexture2D *target3D, Texture2D *mapTexture)
{
    rlImGuiBegin();

    // ==========================================
    // MAIN WORKSPACE SETUP
    // ==========================================
    ImGui::DockSpaceOverViewport(ImGui::GetMainViewport()->ID, ImGui::GetMainViewport(), ImGuiDockNodeFlags_PassthruCentralNode);

    // ==========================================
    // LEFT COLUMN: 2D ENVIRONMENT & TOOLS
    // ==========================================
    ImGui::Begin("2D Environment");

    // --- FILE SYSTEM SETTINGS ---
    ImGui::Text("FILE SYSTEM");
    if (ImGui::Button("Save Config (JSON)"))
    {
        config.needsSave = true;
    }
    ImGui::SameLine();
    if (ImGui::Button("Load Config (JSON)"))
    {
        config.needsLoad = true;
    }
    ImGui::Separator();

    ImGui::Text("EXPORT");
    if (ImGui::Button("Export to UE5 (JSON)"))
    {
        config.needsExportUE5 = true;
    }

    // --- BASE PROCEDURAL GENERATION ---
    ImGui::Text("BASE GENERATION");
    ImGui::InputInt("Seed", &config.seed);
    if (ImGui::IsItemDeactivatedAfterEdit())
    {
        config.needsRegen = true;
    }
    ImGui::SameLine();
    
    // Botón para generar una semilla aleatoria nueva
    if (ImGui::Button("Random"))
    {
        config.seed = GetRandomValue(0, 9999999);
        config.needsRegen = true;
    }

    // Controls the zoom/frequency of the Perlin Noise
    ImGui::SliderFloat("Noise Scale", &config.noiseScale, 0.01f, 1.5f, "%.2f");
    if (ImGui::IsItemDeactivatedAfterEdit())
    {
        config.needsRegen = true; 
    }

    // Controls the global water level height (Y-axis)
    ImGui::SliderInt("Water Level", &config.waterLevel, 1, 128);
    if (ImGui::IsItemDeactivatedAfterEdit())
    {
        config.needsRegen = true;
    }
    ImGui::Separator();

    // --- SCULPTING TOOLS ---
    ImGui::Text("SCULPTING TOOLS");
    ImGui::SameLine();
    ImGui::TextDisabled("(?)");
    if (ImGui::IsItemHovered())
    {
        ImGui::BeginTooltip();
        ImGui::TextUnformatted("Keyboard Shortcuts:\n [ + / - ] Change brush size\n [ Up / Down ] Change brush strength\n [ Ctrl + Z ] Undo last stroke");
        ImGui::EndTooltip();
    }

    // --- QoL: KEYBOARD SHORTCUTS ---
    if (IsKeyPressed(KEY_KP_ADD) || IsKeyPressed(KEY_EQUAL)) { config.brushSize++; }
    if (IsKeyPressed(KEY_KP_SUBTRACT) || IsKeyPressed(KEY_MINUS)) { config.brushSize--; if(config.brushSize < 1) config.brushSize = 1; }
    if (IsKeyPressed(KEY_UP)) { config.brushStrength += 0.05f; if(config.brushStrength > 1.0f) config.brushStrength = 1.0f; }
    if (IsKeyPressed(KEY_DOWN)) { config.brushStrength -= 0.05f; if(config.brushStrength < 0.01f) config.brushStrength = 0.01f; }

    if (IsKeyDown(KEY_LEFT_CONTROL) && IsKeyPressed(KEY_Z))
    {
        config.needsUndo = true;
    }
    if (ImGui::Button("Undo (Ctrl+Z)"))
    {
        config.needsUndo = true;
    }
    ImGui::SameLine();
    if (ImGui::Button("Reset World"))
    {
        config.needsRegen = true;
    }

    // Tool selection
    const char *brushNames[] = {
        "Raise", "Lower", "Flatten",
        "Smooth", "Roughen",
        "Terrace", "Sharpen"};
    int currentBrush = (int)config.activeBrush;
    if (ImGui::Combo("Tool", &currentBrush, brushNames, 7))
    {
        config.activeBrush = (BrushType)currentBrush;
    }
    if (ImGui::IsItemHovered()) {
        ImGui::SetTooltip("Smooth: Softens peaks.\nTerrace: Creates geological steps.\nSharpen: Makes terrain sharp and steep.");
    }

    const char *shapeNames[] = {"Circle (Soft)", "Circle (Hard)", "Square (Flat)", "Noise (Splatter)"};
    int currentShape = (int)config.brushShape;
    if (ImGui::Combo("Shape", &currentShape, shapeNames, 4))
    {
        config.brushShape = (BrushShape)currentShape;
    }

    ImGui::SliderInt("Brush Size", &config.brushSize, 1, 30);
    ImGui::SliderFloat("Brush Strength", &config.brushStrength, 0.01f, 1.0f, "%.2f");

    if (config.activeBrush == BrushType::FLATTEN)
    {
        ImGui::SliderFloat("Flatten Target", &config.flattenTarget, 0.0f, 1.0f, "%.2f");
    }

    ImGui::Separator();

    // --- 2D VIEWPORT ---
    ImGui::Text("2D VIEWPORT (Top-Down View)");

    ImVec2 size2D = ImGui::GetContentRegionAvail();
    if (mapTexture != nullptr && mapTexture->id != 0)
    {
        float minSize = (size2D.x < size2D.y) ? size2D.x : size2D.y;
        float offsetX = (size2D.x - minSize) * 0.5f;
        float offsetY = (size2D.y - minSize) * 0.5f;
        ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPosX() + offsetX, ImGui::GetCursorPosY() + offsetY));

        ImVec2 imgPos = ImGui::GetCursorScreenPos();
        rlImGuiImageSize(mapTexture, (int)minSize, (int)minSize);

        if (ImGui::IsItemHovered())
        {
            ImVec2 mousePos = ImGui::GetMousePos();

            // ==========================================
            // QoL: BRUSH OVERLAY
            // ==========================================
            ImDrawList *drawList = ImGui::GetWindowDrawList();
            float scale = minSize / (float)WORLD_PIXELS;
            float screenRadius = config.brushSize * scale;

            if (config.brushShape == BrushShape::SQUARE)
            {
                ImVec2 pMin = ImVec2(mousePos.x - screenRadius, mousePos.y - screenRadius);
                ImVec2 pMax = ImVec2(mousePos.x + screenRadius, mousePos.y + screenRadius);
                drawList->AddRect(pMin, pMax, IM_COL32(255, 255, 255, 200), 0.0f, 0, 2.0f);
            }
            else
            {
                drawList->AddCircle(mousePos, screenRadius, IM_COL32(255, 255, 255, 200), 32, 2.0f);
            }

            // ==========================================
            // PAINTING INTERACTION
            // ==========================================
            if (ImGui::IsMouseDown(ImGuiMouseButton_Left))
            {
                if (ImGui::IsMouseClicked(ImGuiMouseButton_Left))
                {
                    config.needsUndoSave = true;
                }
                float u = (mousePos.x - imgPos.x) / minSize;
                float v = (mousePos.y - imgPos.y) / minSize;
                if (u >= 0.0f && u <= 1.0f && v >= 0.0f && v <= 1.0f)
                {
                    config.paintX = (int)(u * WORLD_PIXELS);
                    config.paintZ = (int)(v * WORLD_PIXELS);
                    config.isPainting = true;
                }
            }
            else 
            {
                config.isPainting = false; 
            }
        }
        else
        {
            config.isPainting = false;
        }
    }
    else
    {
        ImGui::Dummy(size2D);
    }
    ImGui::End();

    // ==========================================
    // RIGHT COLUMN: 3D ENVIRONMENT & RENDER
    // ==========================================
    ImGui::Begin("3D Environment");

    // --- 3D AREA TOOLS (OVERLAYS) ---
    ImGui::Text("3D AREA TOOLS");
    ImGui::Text("FPS: %d", GetFPS());
    ImGui::Separator();

    // --- 3D VIEWPORT ---
    if (target3D != nullptr)
    {
        rlImGuiImageRenderTextureFit(target3D, true);
    }

    isViewport3DHovered = ImGui::IsWindowHovered();
    ImGui::End();

    rlImGuiEnd();
}

void uiSys::Close()
{
    rlImGuiShutdown();
}