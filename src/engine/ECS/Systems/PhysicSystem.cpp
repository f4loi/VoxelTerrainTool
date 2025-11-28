#include "PhysicSystem.h"

void PhysicSystem::Update(EntityManager &em)
{
    em.forAll([](Entity& entity)
    {
        auto& physic = entity.GetPhysicCMP();
        physic.x += physic.vx;
        if(physic.x > 800.0f)
        {
            physic.x = rand() % 100;
        }
    });

}