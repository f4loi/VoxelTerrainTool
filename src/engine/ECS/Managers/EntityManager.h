#pragma once

#include <vector>
#include <optional>
#include <functional>

#include "../CMP/PhysicCMP.h"
#include "../CMP/RenderCMP.h"
#include "../CMP/InputCMP.h"

class EntityManager
{
public:
    using EntityID = int;

private:
    int nextId = 0;

    std::vector<EntityID> activeEntities;

    std::vector<std::optional<PhysicCMP>> physicComponents;
    std::vector<std::optional<RenderCMP>> renderComponents;
    std::vector<std::optional<InputCMP>> inputComponents;

public:
    EntityManager();

    EntityID CreateEntity();

    void AddPhysic(EntityID id, PhysicCMP cmp);
    PhysicCMP *GetPhysic(EntityID id);

    void AddRender(EntityID id, RenderCMP cmp);
    RenderCMP *GetRender(EntityID id);

    void AddInput(EntityID id, InputCMP cmp);
    InputCMP *GetInput(EntityID id);

    void forAll(const std::function<void(EntityID)> &func);

    const std::vector<EntityID> &GetEntities() { return activeEntities; }
};