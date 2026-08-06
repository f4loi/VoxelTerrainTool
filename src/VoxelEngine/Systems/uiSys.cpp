#include "VoxelEngine/Systems/uiSys.h"
#include "imgui.h"
#include "rlImGui.h"
#include "VoxelEngine/Managers/EntityManagerMeta.h"

void uiSys::Init()
{
    rlImGuiSetup(true);
    ImGuiIO &io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
}

void uiSys::Draw(TerrainConfig &config, RenderTexture2D *target3D, Texture2D *mapTexture)
{
    rlImGuiBegin();

    // El contenedor principal magnético
    ImGui::DockSpaceOverViewport(ImGui::GetMainViewport()->ID, ImGui::GetMainViewport(), ImGuiDockNodeFlags_PassthruCentralNode);

    // ==========================================
    // COLUMNA IZQUIERDA: ENTORNO 2D
    // ==========================================
    ImGui::Begin("Entorno 2D");

    // --- 1. HERRAMIENTAS ZONA 2D ---
    ImGui::Text("SISTEMA DE ARCHIVOS");
    if (ImGui::Button("Guardar Config (JSON)"))
        config.needsSave = true;
    ImGui::SameLine();
    if (ImGui::Button("Cargar Config (JSON)"))
        config.needsLoad = true;

    ImGui::Separator();
    ImGui::Text("GENERACIÓN BASE");
    ImGui::SliderFloat("Escala de Ruido", &config.noiseScale, 0.01f, 1.5f, "%.2f");
    if (ImGui::IsItemDeactivatedAfterEdit())
        config.needsRegen = true;

    ImGui::SliderInt("Nivel de Agua", &config.waterLevel, 1, 128);
    if (ImGui::IsItemDeactivatedAfterEdit())
        config.needsRegen = true;

    ImGui::Separator();
    ImGui::Text("PINCEL DE BIOMAS");

    // Atajo de teclado (Ctrl + Z)
    if (IsKeyDown(KEY_LEFT_CONTROL) && IsKeyPressed(KEY_Z))
        config.needsUndo = true;
    if (ImGui::Button("Deshacer (Ctrl+Z)"))
        config.needsUndo = true;
    ImGui::SameLine();
    if (ImGui::Button("Resetear Mundo"))
        config.needsRegen = true;

    // Forma del Pincel (Desplegable)
    const char *shapeNames[] = {"Círculo (Suave)", "Cubo (Plano)"};
    int currentShape = config.isSquareBrush ? 1 : 0;
    if (ImGui::Combo("Forma", &currentShape, shapeNames, 2))
    {
        config.isSquareBrush = (currentShape == 1);
    }

    ImGui::SliderInt("Tamaño del Pincel", &config.brushSize, 1, 30);

    // Biomas (Desplegable en lugar de RadioButtons)
    const char *biomeNames[] = {"Prado (Llanura)", "Montaña (Picos)", "Río (Cauce)"};
    int currentBiome = (int)config.selectedBiome - 1; // Ajustamos el enum (DEFAULT es 0)
    if (currentBiome < 0)
        currentBiome = 0;
    if (ImGui::Combo("Bioma", &currentBiome, biomeNames, 3))
    {
        config.selectedBiome = (PaintBiome)(currentBiome + 1);
    }

    ImGui::Separator();

    // --- 2. VIEWPORT 2D (Lienzo interactivo) ---
    ImGui::Text("VIEWPORT 2D (Vista Top-Down)");
    ImVec2 size2D = ImGui::GetContentRegionAvail();

    if (mapTexture != nullptr && mapTexture->id != 0)
    {
        float minSize = (size2D.x < size2D.y) ? size2D.x : size2D.y;
        float offsetX = (size2D.x - minSize) * 0.5f;
        float offsetY = (size2D.y - minSize) * 0.5f;

        ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPosX() + offsetX, ImGui::GetCursorPosY() + offsetY));

        // --- LA MAGIA DE LA PINTURA ---
        // Guardamos dónde empieza la imagen para calcular el ratón
        ImVec2 imgPos = ImGui::GetCursorScreenPos();

        // Dibujamos la textura
        rlImGuiImageSize(mapTexture, (int)minSize, (int)minSize);

        // Si mantenemos clic izquierdo SOBRE la imagen...
        if (ImGui::IsItemHovered() && ImGui::IsMouseDown(ImGuiMouseButton_Left))
        {
            if (ImGui::IsMouseClicked(ImGuiMouseButton_Left))
            {
                config.needsUndoSave = true;
            }

            ImVec2 mousePos = ImGui::GetMousePos();

            // Calculamos el porcentaje (0.0 a 1.0) de dónde hemos hecho clic
            float u = (mousePos.x - imgPos.x) / minSize;
            float v = (mousePos.y - imgPos.y) / minSize;

            if (u >= 0.0f && u <= 1.0f && v >= 0.0f && v <= 1.0f)
            {
                // Ahora multiplicamos por el tamaño TOTAL del mundo en píxeles (256)
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
        ImGui::Dummy(size2D);
    }

    ImGui::End();

    // ==========================================
    // COLUMNA DERECHA: ENTORNO 3D
    // ==========================================
    ImGui::Begin("Entorno 3D");

    // --- 1. HERRAMIENTAS ZONA 3D ---
    ImGui::Text("HERRAMIENTAS ZONA 3D");
    // (Aquí irán estadísticas de FPS, botón de alambre (wireframe), etc.)
    ImGui::Text("FPS: %d", GetFPS());

    ImGui::Separator();

    // --- 2. VIEWPORT 3D ---
    // Dibujamos el mundo 3D llenando todo el espacio sobrante debajo de las herramientas
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