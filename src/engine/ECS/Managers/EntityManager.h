#pragma once

#include "../Entities/Entity.h"
#include <vector>


class EntityManager
{
private:
    std::vector<Entity> entities{};
    int nextId = 0;
public:
    EntityManager();
    Entity& CreateEntity();
    void forAll(auto&& func)
    {
        for (auto& entity : entities)
        {
            func(entity);
        }
    }
    std::vector<Entity>& GetEntities() { return entities; }
};
