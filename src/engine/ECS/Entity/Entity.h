#pragma once
class EntityManager;

class Entity
{
private:
    uint16_t nextId = 0; ///< Entero que se utiliza para generar la siguiente id unica para una entidad
    EntityManager *parent;

public:
    /** * @brief Alias para el identificador de una entidad.
     * Se usa un entero simple para identificar cada objeto del juego.
     */
    using EntityID = uint16_t;

    Entity(EntityManager* parent) : parent(parent) {}


    EntityManager* getParent(){return parent;}


    uint16_t getNextId(){return nextId;}
    void setId(uint16_t newId) { nextId = newId; }
};