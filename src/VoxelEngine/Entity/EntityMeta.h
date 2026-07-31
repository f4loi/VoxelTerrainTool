#pragma once
#include <cstdint>
class EntityManagerMeta;

class EntityMeta
{
private:
    uint16_t nextId{0};
    EntityManagerMeta *parent;

public:
    using EntityID = uint16_t;
    EntityMeta(EntityManagerMeta *parent) : parent(parent) {}
    EntityManagerMeta *getParent() { return parent; }
    EntityID getNextId() { return nextId; }
    void setId(EntityID newId) { nextId = newId; }
};