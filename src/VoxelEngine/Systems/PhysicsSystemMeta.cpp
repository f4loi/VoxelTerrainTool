#include "VoxelEngine/Systems/PhysicsSystemMeta.h"
#include <cstdlib>

void PhysicsSystemMeta::Update(EntityManagerMeta &em)
{
    em.forAll(&PhysicsSystemMeta::UpdateOneEntity);
}

void PhysicsSystemMeta::UpdateOneEntity(EntityMeta ent)
{
}