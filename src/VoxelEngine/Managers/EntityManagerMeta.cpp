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

    // METAPROGRAMACIÓN (C++17 std::apply + fold expression): 
    // Añadimos una clave inválida (9999) a TODOS los vectores de claves de la tupla a la vez.
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