#pragma once
#include "Config\TerrainConfig.h"

class uiSys {
public:
    void Init();
    void Draw(TerrainConfig& config);
    void Close();
};