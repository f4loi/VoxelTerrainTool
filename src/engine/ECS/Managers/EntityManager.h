#pragma once

#include "../Entities/Entity.h"
#include <vector>
#include <functional>

class EntityManager
{
private:
    std::vector<Entity> entities{};
    int nextId = 0;
public:
    EntityManager();
    Entity& CreateEntity();
    void forAll( const std::function<void(Entity&)>& func );
    std::vector<Entity>& GetEntities() { return entities; }
};
