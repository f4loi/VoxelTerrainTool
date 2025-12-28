#include "PhysicSystem.h"

void PhysicSystem::Update(EntityManager &em)
{
    em.forAll(&PhysicSystem::UpdateOneEntity);
}

void PhysicSystem::UpdateOneEntity(Entity ent)
{
    PhysicCMP *physic = ent.getParent()->GetPhysic(ent.getNextId());

    if (physic != nullptr)
    {
        physic->x += physic->vx;
        if (physic->x > 800.0f)
        {
            physic->x = static_cast<float>(rand() % 100);
        }
    }
}