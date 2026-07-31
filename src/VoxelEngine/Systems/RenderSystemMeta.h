#pragma once
#include <raylib.h>
#include "VoxelEngine/Managers/EntityManagerMeta.h"

class RenderSystemMeta {
public:
    void Init();
    void Update(EntityManagerMeta& em);
    static void UpdateOneEntity(EntityMeta ent);
    bool WindowShouldClose();
};