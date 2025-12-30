#include "RenderSystem.h"

MotorRaylib RenderSystem::motorRaylib;

void RenderSystem::Init()
{

    motorRaylib.Init(800, 600, "Engine Portfolio");
    SetTargetFPS(60);
}

void RenderSystem::Update(EntityManager &em)
{

    motorRaylib.EmpezarDrawing();
    motorRaylib.borrarBackground(0, 0, 0, 255);

    em.forAll(&RenderSystem::UpdateOneEntity);

    motorRaylib.TerminarDrawing();
}

void RenderSystem::UpdateOneEntity(Entity ent)
{

    EntityManager *manager = ent.getParent();
    uint16_t id = ent.getNextId();

    if (manager != nullptr)
    {

        RenderCMP *render = manager->GetRender(id);
        PhysicCMP *physic = manager->GetPhysic(id);

        if (render != nullptr && physic != nullptr && render->isRendered)
        {
            motorRaylib.DrawRectangulo(
                static_cast<uint16_t>(physic->x),
                static_cast<uint16_t>(physic->y),
                15,
                255,
                0, 0, 255);
        }
    }
}