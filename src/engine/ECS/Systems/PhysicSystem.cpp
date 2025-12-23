#include "PhysicSystem.h"

void PhysicSystem::Update(EntityManager &em)
{
    //Explicar captura y error
    em.forAll([&em](EntityManager::EntityID id)
              {
       
        PhysicCMP* physic = em.GetPhysic(id);

        
        if (physic != nullptr)
        {
            physic->x += physic->vx;
            if(physic->x > 800.0f)
            {
                physic->x = static_cast<float>(rand() % 100);
            }
        } });
}