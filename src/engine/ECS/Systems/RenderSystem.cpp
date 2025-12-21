#include "RenderSystem.h"

void RenderSystem::Init()
{
    motorRaylib.Init(800, 600, "Engine Portfolio");
    SetTargetFPS(60);
}

void RenderSystem::Update(EntityManager &em)
{
    motorRaylib.EmpezarDrawing();
    motorRaylib.borrarBackground(0, 0, 0, 255);

    em.forAll([this, &em](EntityManager::EntityID id)
    {
        RenderCMP* render = em.GetRender(id);
        PhysicCMP* physic = em.GetPhysic(id);

        if (render != nullptr && physic != nullptr && render->isRendered)
        {
            motorRaylib.DrawRectangulo(physic->x, physic->y, 15, 255, 0, 0, 255);
        }
    });

    motorRaylib.TerminarDrawing();
}