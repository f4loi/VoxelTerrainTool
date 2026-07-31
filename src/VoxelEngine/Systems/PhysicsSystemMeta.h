// PhysicsSystemMeta.h
#pragma once
#include "VoxelEngine/Managers/EntityManagerMeta.h"


class PhysicsSystemMeta {
public:
    void Update(EntityManagerMeta& em);
    static void UpdateOneEntity(EntityMeta ent);
};

