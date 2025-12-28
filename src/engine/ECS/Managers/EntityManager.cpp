#include "EntityManager.h"

EntityManager::EntityManager()
{

    activeEntities.reserve(100);

    physicComponents.reserve(100);
    renderComponents.reserve(100);
    inputComponents.reserve(100);
}

Entity::EntityID EntityManager::CreateEntity()
{
    Entity::EntityID newId = static_cast<Entity::EntityID>(activeEntities.size());
    Entity newEntity(this);
    newEntity.setId(newId);

    physicComponents.push_back(std::nullopt);
    renderComponents.push_back(std::nullopt);
    inputComponents.push_back(std::nullopt);

    activeEntities.push_back(newEntity);

    return newId;
}


void EntityManager::forAll(void (*func)(Entity))
{

    for (size_t i = 0; i < activeEntities.size(); ++i)
    {
        
        func(activeEntities[i]);
    }
}

// Const correct arreglar
void EntityManager::AddPhysic(Entity::EntityID id, PhysicCMP const &cmp)
{

    if (id < physicComponents.size())
    {
        physicComponents[id] = cmp;
    }
}

PhysicCMP *EntityManager::GetPhysic(Entity::EntityID id)
{
    PhysicCMP *phy{};

    if (id < physicComponents.size() && physicComponents[id].has_value())
    {
        // parentesis de orden
        phy = &(physicComponents[id].value());
    }
    return phy;
}

void EntityManager::AddRender(Entity::EntityID id, RenderCMP const &cmp)
{
    if (id < renderComponents.size())
    {
        renderComponents[id] = cmp;
    }
}

RenderCMP *EntityManager::GetRender(Entity::EntityID id)
{
    if (id < renderComponents.size() && renderComponents[id].has_value())
    {
        return &(renderComponents[id].value());
    }
    return nullptr;
}

void EntityManager::AddInput(Entity::EntityID id, InputCMP const &cmp)
{
    if (id < inputComponents.size())
    {
        inputComponents[id] = cmp;
    }
}

InputCMP *EntityManager::GetInput(Entity::EntityID id)
{
    if (id < inputComponents.size() && inputComponents[id].has_value())
    {
        return &(inputComponents[id].value());
    }
    return nullptr;
}