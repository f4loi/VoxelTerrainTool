#include "VoxelEngine\Systems\uiSys.h"
#include <raylib.h>
#include "imgui.h"
#include "rlImGui.h"

void uiSys::Init() {
    // Prepara el contexto de ImGui dentro de Raylib
    rlImGuiSetup(true); 
}

void uiSys::Draw(TerrainConfig& config) {
    // Inicia el frame de ImGui
    rlImGuiBegin();

    // ---------------------------------------------------
    // VENTANA PRINCIPAL DE CONTROLES
    // ---------------------------------------------------
    ImGui::Begin("Generador de Biomas 3D");

    ImGui::Text("Rendimiento: %.1f FPS", ImGui::GetIO().Framerate);
    ImGui::Separator();

    ImGui::Text("Parámetros del Terreno");
    
    // Controles que modifican la configuración
    if (ImGui::Button("Generar Nueva Semilla")) {
        config.seed = GetRandomValue(0, 99999);
        config.needsRegen = true;
    }
    
    if (ImGui::SliderFloat("Escala (Zoom Ruido)", &config.noiseScale, 1.0f, 20.0f)) {
        config.needsRegen = true;
    }
    
    if (ImGui::SliderInt("Nivel del Agua", &config.waterLevel, 0, 15)) {
        config.needsRegen = true;
    }

    ImGui::Separator();
    
    // Controles de Guardado/Carga (JSON)
    ImGui::Text("Persistencia de Datos");
    if (ImGui::Button("Guardar JSON")) {
        config.SaveToJson("config.json");
    }
    ImGui::SameLine();
    if (ImGui::Button("Cargar JSON")) {
        config.LoadFromJson("config.json");
    }

    ImGui::End();
    // ---------------------------------------------------

    // Finaliza y dibuja ImGui en pantalla
    rlImGuiEnd();
}

void uiSys::Close() {
    rlImGuiShutdown();
}