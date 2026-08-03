#include "VoxelEngine/Managers/EntityManagerMeta.h"

EntityManagerMeta::EntityManagerMeta()
{
    activeEntities.reserve(100);
}

EntityMeta EntityManagerMeta::createEntity()
{
    uint16_t newId = static_cast<uint16_t>(activeEntities.size());

    EntityMeta newEntity(this);
    newEntity.setId(newId);

    std::apply([](auto&... keyVectors) {
        (keyVectors.push_back({ 9999, 0 }), ...);
    }, keys);

    activeEntities.push_back(newEntity);

    return newEntity;
}

void EntityManagerMeta::forAll(void (*func)(EntityMeta))
{
    for (size_t i = 0; i < activeEntities.size(); ++i)
    {
        func(activeEntities[i]);
    }
}