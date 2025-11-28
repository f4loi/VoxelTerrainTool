#include "RenderSystem.h"

void RenderSystem::Init()
{

    motorRaylib.Init(800, 600, "Engine Portfolio");
    SetTargetFPS(60);
}

void RenderSystem::Update(EntityManager &em)
{
    motorRaylib.EmpezarDrawing();

    em.forAll([this](Entity &entity)
    {
        if (entity.GetRenderCMP().isRendered)
        {
            auto x = entity.GetPhysicCMP().x;
            auto y = entity.GetPhysicCMP().y;
            motorRaylib.DrawRectangulo(x, y, 15, 255, 0, 0, 255);
        }
    });
    em.forAll([this](Entity &entity)
    {
        if (entity.GetInputCMP().isKeyLPressed)
        {
            entity.GetRenderCMP().isRendered = false;
        }
        else
        {
            entity.GetRenderCMP().isRendered = true;
        }
    });

    motorRaylib.borrarBackground(0, 0, 0, 255);
    motorRaylib.TerminarDrawing();
}
