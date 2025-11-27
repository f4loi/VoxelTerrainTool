#include "EntityManager.h"


EntityManager::EntityManager()
{
    entities.reserve(100);
    nextId = 0;
}

auto& EntityManager::CreateEntity()
{
    auto &entity = entities.emplace_back();
    entity.SetId(nextId++);
    return entity;
}
