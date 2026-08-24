#pragma once
#include <string>
#include <fstream>
#include <iostream>
#include "Config\thirdParties\json.hpp"

// Json library
using json = nlohmann::json;

/*
    enum class PaintBiome: represents the different biomes that can be painted on the terrain.
*/
enum class PaintBiome
{
    DEFAULT,
    PLAINS,
    MOUNTAIN,
    RIVER
};

/*
    struct TerrainConfig: represents the configuration settings for terrain generation and painting.
*/
struct TerrainConfig
{
    /*
        world configuration parameters
    */
    float noiseScale{0.1f};
    int waterLevel{32};
    int seed{12345};

    /*
        tool and brush configuration parameters
    */
    bool isSquareBrush = false;
    bool needsRegen{true};
    bool needsUndoSave = false;
    bool needsUndo = false;
    bool needsSave = false;
    bool needsLoad = false;
    bool needsMapUpdate = true;

    int brushSize = 1;
    PaintBiome selectedBiome = PaintBiome::PLAINS;

    /*
        painting configuration parameters
    */
    bool isPainting = false;
    int paintX = 0;
    int paintZ = 0;

    /*
        save configuration to JSON file
        description: This function saves the current configuration settings to a JSON file specified by the filepath.
    */
    void SaveToJson(const std::string &filepath)
    {
        json j;
        j["noiseScale"] = noiseScale;
        j["waterLevel"] = waterLevel;
        j["seed"] = seed;

        std::ofstream file(filepath);
        if (file.is_open())
        {
            file << j.dump(4);
            file.close();
        }
    }

    /*
        load configuration from JSON file
        description: This function loads the configuration settings from a JSON file specified by the filepath.
    */
    void LoadFromJson(const std::string &filepath)
    {
        std::ifstream file(filepath);
        if (file.is_open())
        {
            json j;
            file >> j;

            noiseScale = j.value("noiseScale", 0.1f);
            waterLevel = j.value("waterLevel", 32);
            seed = j.value("seed", 12345);

            needsRegen = true;
            file.close();
        }
    }
};