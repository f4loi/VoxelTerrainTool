#include "EntityManager.h"


EntityManager::EntityManager()
{
    entities.reserve(100);
    nextId = 0;
}

Entity& EntityManager::CreateEntity() 
{
    auto &entity = entities.emplace_back();
    entity.SetId(nextId++);
    return entity;
}

void EntityManager::forAll( const std::function<void(Entity&)>& func )
{
    for (auto& entity : entities)
    {
        func(entity);
    }
}