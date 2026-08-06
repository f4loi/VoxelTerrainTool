#pragma once
#include <string>
#include <fstream>
#include <iostream>
#include "Config\thirdParties\json.hpp" 

using json = nlohmann::json;
enum class PaintBiome { DEFAULT, PLAINS, MOUNTAIN, RIVER };

struct TerrainConfig {
    float noiseScale{0.1f};
    int waterLevel{32};
    int seed{12345};
    

    bool isSquareBrush = false;
    bool needsRegen{true}; 
    bool needsUndoSave = false; // Avisa de que vamos a empezar a pintar
    bool needsUndo = false;     // Pide dar un paso atrás
    bool needsSave = false;     // Pide guardar config
    bool needsLoad = false;     // Pide cargar config
    bool needsMapUpdate = true;
    
    int brushSize = 1;
    PaintBiome selectedBiome = PaintBiome::PLAINS;
    
    bool isPainting = false;
    int paintX = 0;
    int paintZ = 0;
    
    void SaveToJson(const std::string& filepath) {
        json j;
        j["noiseScale"] = noiseScale;
        j["waterLevel"] = waterLevel;
        j["seed"] = seed;

        std::ofstream file(filepath);
        if (file.is_open()) {
            file << j.dump(4); 
            file.close();
            std::cout << "Configuración guardada en: " << filepath << "\n";
        }
    }

    
    void LoadFromJson(const std::string& filepath) {
        std::ifstream file(filepath);
        if (file.is_open()) {
            json j;
            file >> j;

            
            noiseScale = j.value("noiseScale", 0.1f);
            waterLevel = j.value("waterLevel", 32);
            seed = j.value("seed", 12345);

            needsRegen = true;
            file.close();
            std::cout << "Configuración cargada desde: " << filepath << "\n";
        } else {
            std::cout << "No se encontró el archivo. Usando valores por defecto.\n";
        }
    }
};