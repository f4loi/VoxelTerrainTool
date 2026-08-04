#pragma once
#include <raylib.h>
#include "VoxelEngine/Managers/EntityManagerMeta.h"

class RenderSystemMeta {
private:
    RenderTexture2D target3D;

public:
    void Init();
    void Update(EntityManagerMeta& em, const Camera3D& camera); 
    void Unload(); 
    
    static void UpdateOneEntity(EntityMeta ent);
    bool WindowShouldClose();

    RenderTexture2D* GetTarget3D() { return &target3D; } 
};