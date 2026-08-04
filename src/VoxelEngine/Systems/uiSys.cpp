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
    ImGui::Text("GENERACIÓN");
    if (ImGui::SliderFloat("Escala de Ruido", &config.noiseScale, 1.0f, 50.0f))
        config.needsRegen = true;
    if (ImGui::SliderInt("Nivel de Agua", &config.waterLevel, 1, 15))
        config.needsRegen = true;

    ImGui::Separator();

    ImGui::Text("PINCEL DE TERRENO");
    ImGui::SliderInt("Tamaño del Pincel", &config.brushSize, 1, 5);

    // Botones de radio para elegir material
    int mat = (int)config.selectedMaterial;
    ImGui::RadioButton("Agua", &mat, 0);
    ImGui::SameLine();
    ImGui::RadioButton("Tierra", &mat, 1);
    ImGui::SameLine();
    ImGui::RadioButton("Césped", &mat, 2);
    config.selectedMaterial = (PaintMaterial)mat;

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
            ImVec2 mousePos = ImGui::GetMousePos();

            // Calculamos el porcentaje (0.0 a 1.0) de dónde hemos hecho clic
            float u = (mousePos.x - imgPos.x) / minSize;
            float v = (mousePos.y - imgPos.y) / minSize;

            if (u >= 0.0f && u <= 1.0f && v >= 0.0f && v <= 1.0f)
            {
                // Multiplicamos por el tamaño del chunk para obtener la coordenada exacta
                config.paintX = (int)(u * CHUNK_SIZE);
                config.paintZ = (int)(v * CHUNK_SIZE);
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