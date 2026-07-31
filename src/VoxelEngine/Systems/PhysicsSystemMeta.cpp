#include "VoxelEngine/Systems/PhysicsSystemMeta.h"
#include <cstdlib>

void PhysicsSystemMeta::Update(EntityManagerMeta &em) {
    em.forAll(&PhysicsSystemMeta::UpdateOneEntity);
}

void PhysicsSystemMeta::UpdateOneEntity(EntityMeta ent) {
    PhysicsCMP *physic = ent.getParent()->getComponent<PhysicsCMP>(ent.getNextId());
    if (physic != nullptr) {
        physic->x += physic->vx;
        if (physic->x > 800.0f) {
            physic->x = static_cast<float>(rand() % 100);
        }
    }
}