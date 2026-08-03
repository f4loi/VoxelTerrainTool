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

// 1. Definimos todos los componentes de nuestro juego aquí
using ComponentList = std::tuple<PhysicsCMP, RenderCMP, ChunkCMP>;

// 2. Helper para transformar std::tuple<A, B> en std::tuple<Slotmap<A>, Slotmap<B>>
template <typename Tuple>
struct SlotmapTuple;
template <typename... Ts>
struct SlotmapTuple<std::tuple<Ts...>>
{
    using type = std::tuple<Slotmap<Ts, 1000>...>;
};

// 3. Helper para transformar std::tuple<A, B> en std::tuple<vector<KeyA>, vector<KeyB>>
template <typename Tuple>
struct KeysTuple;
template <typename... Ts>
struct KeysTuple<std::tuple<Ts...>>
{
    using type = std::tuple<std::vector<typename Slotmap<Ts, 1000>::key_type>...>;
};

class EntityManagerMeta
{
    std::vector<EntityMeta> activeEntities;

    // El compilador genera automáticamente todos los Slotmaps y Vectores
    SlotmapTuple<ComponentList>::type components;
    KeysTuple<ComponentList>::type keys;

public:
    EntityManagerMeta();

    EntityMeta createEntity();

    // Template genérico único: No necesitamos especializaciones manuales
    template <typename T>
    void addComponent(EntityMeta::EntityID id, T const &cmp)
    {
        // std::get extrae en tiempo de compilación el contenedor correcto de la tupla
        auto &compSlotmap = std::get<Slotmap<T, 1000>>(components);
        auto &compKeys = std::get<std::vector<typename Slotmap<T, 1000>::key_type>>(keys);

        if (id < compKeys.size())
        {
            auto key = compSlotmap.push_back(cmp);
            compKeys[id] = key;
        }
    }

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