#pragma once
#include <raylib.h>
#include "VoxelEngine/Managers/EntityManagerMeta.h"

class RenderSystemMeta
{
private:
    Camera3D camera = {0};

public:
    void Init();
    void Update(EntityManagerMeta &em);
    static void UpdateOneEntity(EntityMeta ent);
    bool WindowShouldClose();
};