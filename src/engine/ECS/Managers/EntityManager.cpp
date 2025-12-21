#include "EntityManager.h"

EntityManager::EntityManager()
{
    
    activeEntities.reserve(100);

    physicComponents.reserve(100);
    renderComponents.reserve(100);
    inputComponents.reserve(100);
}

EntityManager::EntityID EntityManager::CreateEntity()
{
    EntityID id = nextId++;

    
    activeEntities.push_back(id);

    
    if (id >= physicComponents.size())
    {
        int newSize = id + 100;
        physicComponents.resize(newSize);
        renderComponents.resize(newSize);
        inputComponents.resize(newSize);
    }

    return id;
}

void EntityManager::forAll(const std::function<void(EntityID)> &func)
{
    
    for (EntityID id : activeEntities)
    {
        func(id);
    }
}


void EntityManager::AddPhysic(EntityID id, PhysicCMP cmp)
{
    
    if (id < physicComponents.size())
    {
        physicComponents[id] = cmp;
    }
}

PhysicCMP *EntityManager::GetPhysic(EntityID id)
{
    
    if (id < physicComponents.size() && physicComponents[id].has_value())
    {
        return &physicComponents[id].value();
    }
    return nullptr;
}


void EntityManager::AddRender(EntityID id, RenderCMP cmp)
{
    if (id < renderComponents.size())
    {
        renderComponents[id] = cmp;
    }
}

RenderCMP *EntityManager::GetRender(EntityID id)
{
    if (id < renderComponents.size() && renderComponents[id].has_value())
    {
        return &renderComponents[id].value();
    }
    return nullptr;
}

void EntityManager::AddInput(EntityID id, InputCMP cmp)
{
    if (id < inputComponents.size())
    {
        inputComponents[id] = cmp;
    }
}

InputCMP *EntityManager::GetInput(EntityID id)
{
    if (id < inputComponents.size() && inputComponents[id].has_value())
    {
        return &inputComponents[id].value();
    }
    return nullptr;
}