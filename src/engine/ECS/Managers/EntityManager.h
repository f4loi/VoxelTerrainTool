#pragma once

#include "../Entities/Entity.h"
#include <vector>

class EntityManager
{
private:
    std::vector<Entity> entities{};
    int nextId = 0;
public:
    EntityManager(){};
    auto& CreateEntity();

};
