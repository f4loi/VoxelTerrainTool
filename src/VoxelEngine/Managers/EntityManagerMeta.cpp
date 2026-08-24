#include "VoxelEngine/Managers/EntityManagerMeta.h"

/*
    =========================================================================
                                EntityManagerMeta Implementation
    =========================================================================
    Description: This file contains the implementation of the EntityManagerMeta class, which manages entities and their components in the voxel engine.
*/
EntityManagerMeta::EntityManagerMeta()
{
    // Reserve space for active entities to optimize memory allocation and avoid frequent reallocations.
    activeEntities.reserve(100);
}

/*
    Function: createEntity
    Description: Creates a new entity, assigns it a unique ID, and initializes its component keys.
    Returns: The newly created EntityMeta object.
*/
EntityMeta EntityManagerMeta::createEntity()
{
    // Generate a new unique ID for the entity based on the current size of the activeEntities vector.
    uint16_t newId = static_cast<uint16_t>(activeEntities.size());

    EntityMeta newEntity(this);
    newEntity.setId(newId);

    /*
        Initialize the component keys for the new entity by pushing a default key (9999, 0) into each component's key vector.
        This ensures that each component type has a valid key for the new entity, even if it doesn't have any components yet.
    */
    std::apply([](auto&... keyVectors) {
        (keyVectors.push_back({ 9999, 0 }), ...);
    }, keys);

    activeEntities.push_back(newEntity);

    return newEntity;
}

/*
    Function: forAll
    Description: Iterates through all active entities and applies the given function to each one.
*/
void EntityManagerMeta::forAll(void (*func)(EntityMeta))
{
    for (size_t i = 0; i < activeEntities.size(); ++i)
    {
        func(activeEntities[i]);
    }
}