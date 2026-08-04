#pragma once
#include "Config\TerrainConfig.h"
#include <raylib.h>

class uiSys
{
private:
    bool isViewport3DHovered = false;
public:
    void Init();
    void Draw(TerrainConfig &config, RenderTexture2D *target3D, Texture2D *mapTexture);
    void Close();
    bool GetIsViewport3DHovered() const { return isViewport3DHovered; }
};