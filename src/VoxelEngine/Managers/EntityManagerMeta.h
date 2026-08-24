#pragma once
#include <vector>
#include <tuple>
#include <cstdint>
#include <iostream>

#include "VoxelEngine/Entity/EntityMeta.h"
#include "VoxelEngine/CMP/PhysicsCMP.h"
#include "VoxelEngine/CMP/RenderCMP.h"
#include "VoxelEngine/CMP/ChunkCMP.h"
#include "utils/slotmap.hpp"

// define the list of component types used in the EntityManagerMeta
using ComponentList = std::tuple<PhysicsCMP, RenderCMP, ChunkCMP>;

// helper to create a tuple of Slotmaps for each component type
template <typename Tuple>
struct SlotmapTuple;
template <typename... Ts>
struct SlotmapTuple<std::tuple<Ts...>>
{
    using type = std::tuple<Slotmap<Ts, 1000>...>;
};

// helper to create a tuple of vectors of keys for each component type
template <typename Tuple>
struct KeysTuple;
template <typename... Ts>
struct KeysTuple<std::tuple<Ts...>>
{
    using type = std::tuple<std::vector<typename Slotmap<Ts, 1000>::key_type>...>;
};

// EntityManagerMeta class manages entities and their components in the voxel engine.
class EntityManagerMeta
{
    std::vector<EntityMeta> activeEntities;

    // Tuples to hold Slotmaps and keys for each component type
    SlotmapTuple<ComponentList>::type components;
    KeysTuple<ComponentList>::type keys;

public:
    EntityManagerMeta();

    EntityMeta createEntity();

    // Template function to add a component of type T to an entity with the given ID.
    template <typename T>
    void addComponent(EntityMeta::EntityID id, T const &cmp)
    {
        // Get the Slotmap and keys for the component type T from the tuples
        auto &compSlotmap = std::get<Slotmap<T, 1000>>(components);
        // Get the vector of keys for the component type T from the tuple of keys
        auto &compKeys = std::get<std::vector<typename Slotmap<T, 1000>::key_type>>(keys);

        if (id < compKeys.size())
        {
            auto key = compSlotmap.push_back(cmp);
            compKeys[id] = key;
        }
    }

    /*
    Function: getComponent
    Description: Retrieves a pointer to the component of type T associated with the given entity ID.
    Returns: A pointer to the component if found, otherwise nullptr.
    */
    template <typename T>
    T *getComponent(EntityMeta::EntityID id)
    {
        auto &compSlotmap = std::get<Slotmap<T, 1000>>(components);
        auto &compKeys = std::get<std::vector<typename Slotmap<T, 1000>::key_type>>(keys);

        if (id < compKeys.size())
        {
            auto key = compKeys[id];
            if (compSlotmap.is_valid(key))
            {
                return &compSlotmap.get(key);
            }
        }
        return nullptr;
    }

    void forAll(void (*func)(EntityMeta));
};