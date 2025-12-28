#pragma once
class EntityManager;

class Entity
{
private:
    int nextId = 0; ///< Entero que se utiliza para generar la siguiente id unica para una entidad
    EntityManager *parent;

public:
    /** * @brief Alias para el identificador de una entidad.
     * Se usa un entero simple para identificar cada objeto del juego.
     */
    using EntityID = int;

    Entity(EntityManager* parent) : parent(parent) {}


    EntityManager* getParent(){return parent;}


    int getNextId(){return nextId;}
    void setId(int newId) { nextId = newId; }
};