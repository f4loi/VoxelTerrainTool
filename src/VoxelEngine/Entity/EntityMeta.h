#pragma once
#include <cstdint>
// Forward declaration of EntityManagerMeta to avoid circular dependency
class EntityManagerMeta;

/*
    Class: EntityMeta
    Description: A class to manage entities.
*/
class EntityMeta
{
private:
    // Unique identifier for the entity, used to track and manage entities within the EntityManagerMeta.
    uint16_t nextId{0};
    // Pointer to the parent EntityManagerMeta, allowing access to the entity manager's functionality and data.
    EntityManagerMeta *parent;

public:
    // Type alias for the entity ID, making it easier to refer to the entity's unique identifier.
    using EntityID = uint16_t;
    // Constructor that initializes the EntityMeta with a pointer to its parent EntityManagerMeta.
    EntityMeta(EntityManagerMeta *parent) : parent(parent) {}
    // Getter for the parent EntityManagerMeta, allowing access to the entity manager's functionality and data.
    EntityManagerMeta *getParent() { return parent; }
    // Getter for the next available entity ID, used to assign unique identifiers to new entities.
    EntityID getNextId() { return nextId; }
    // Setter for the next available entity ID, allowing manual control over the entity ID assignment.
    void setId(EntityID newId) { nextId = newId; }
};