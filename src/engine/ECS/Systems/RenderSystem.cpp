#include "RenderSystem.h"

void RenderSystem::Init()
{
    motorRaylib.Init(800, 600, "Engine Portfolio");
}

void RenderSystem::Update(EntityManager& em)
{
    motorRaylib.BeginDrawing();

    em.forAll([this](Entity& entity) {
        auto x = entity.GetPhysicCMP().x;
        auto y = entity.GetPhysicCMP().y;
        motorRaylib.DrawSquare(x, y, 50, 255, 0, 0, 255);
    });

    motorRaylib.EndDrawing();
}

