#pragma once
#include <string>
#include <fstream>
#include <iostream>
#include "Config\thirdParties\json.hpp" 

using json = nlohmann::json;
enum class PaintMaterial { WATER, DIRT, GRASS };

struct TerrainConfig {
    float noiseScale{4.0f};
    int waterLevel{4};
    int seed{12345};
    
    bool needsRegen{true}; 

    bool needsMapUpdate = true; // Para repintar el mapa 2D sin regenerar montañas
    
    int brushSize = 1;
    PaintMaterial selectedMaterial = PaintMaterial::GRASS;
    
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

            
            noiseScale = j.value("noiseScale", 4.0f);
            waterLevel = j.value("waterLevel", 4);
            seed = j.value("seed", 12345);

            needsRegen = true;
            file.close();
            std::cout << "Configuración cargada desde: " << filepath << "\n";
        } else {
            std::cout << "No se encontró el archivo. Usando valores por defecto.\n";
        }
    }
};