#include "RenderSystem.h"

void RenderSystem::Init()
{
    
    motorRaylib.Init(800, 600, "Engine Portfolio");
    SetTargetFPS(60);
}

void RenderSystem::Update(EntityManager& em)
{
    motorRaylib.EmpezarDrawing();

    for (auto& entity : em.GetEntities())
    {
        auto x = entity.GetPhysicCMP().x;
        auto y = entity.GetPhysicCMP().y;
        motorRaylib.DrawRectangulo(x, y, 50, 255, 0, 0, 255);
    }

    motorRaylib.TerminarDrawing();
}

