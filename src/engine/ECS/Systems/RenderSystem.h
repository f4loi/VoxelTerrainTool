#pragma once
#include "../fachadaGrafica/MotorRaylib.h"
#include "../ECS/Managers/EntityManager.h"

class RenderSystem
{
private:
    MotorRaylib motorRaylib;
    
public:
    void Init();
    void Update(EntityManager& em);
};