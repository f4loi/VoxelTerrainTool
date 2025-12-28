/**
 * @file EntityManager.h
 * @brief Sistema de gestión de Entidades y Componentes (ECS).
 * @date 22-12-2025
 * * Este archivo contiene la clase encargada de administrar el ciclo de vida
 * de las entidades y sus componentes asociados (Física, Renderizado, Input).
 */

#pragma once

#include <vector>
#include <optional>
#include <functional>

#include "engine/ECS/CMP/PhysicCMP.h"
#include "engine/ECS/CMP/RenderCMP.h"
#include "engine/ECS/CMP/InputCMP.h"

/**
 * @class EntityManager
 * @brief Manager de entidades y sus componentes.
 * * Esta clase se escargar de crear y destruir entidades y de crear y asociar cmp a esas entidades
 */
class EntityManager
{
public:
    /** * @brief Alias para el identificador de una entidad.
     * Se usa un entero simple para identificar cada objeto del juego.
     */
    using EntityID = int;

private:
    int nextId = 0; ///< Entero que se utiliza para generar la siguiente id unica para una entidad

    std::vector<EntityID> activeEntities; ///< Utilizamos un vector de EntityID(que es un int) para guardar las entidades que estan activas

    /** * @brief Optional
     * * Para almacenar los componentes utilizamos optional, es una libreria que nos permite indicar si una entidad tiene un componente o no.
     * Optional lo que hace es reservar espacio para todos los componentes que les indique, pero todos seguidos haciendo que los que no tengan valor puedas ponerlos a nulo.
     * Esta solucion tiene distintas ventajas como que todas als entidades y sus datos estan colocados seguidos en memoria.
     * El acceso a datos es de coste O(1) lo que quiere decir que es instantaneo ya que es solo una busqueda lineal.
     * Se puede ahorrar hacer new y delete ya que puedes poner todos los componentes a null y no hace falta hacer resize al vector.
     * Tambien tiene varias desventajas como que desperdiciamos mucha memoria ram, una explicacion de esta desventaja:
     * optional ocupa sizeof(T) + bool + padding, esto lo ocupa auqnue el componente este a null, ya que reserva tantos huecos como entidades tengas ya que cada entidad puede tener componentes o no,
     * y tienen que tenerlos seguidos en memoria para que la busqueda sea lineal.
     * Si tenemos 1000 entidades de tipo EntityID tendremos que optional reserva espacio para sizeof(EntityID) + bool + padding, y si solo tenemos 5 cmps de fisica estaremos reservando espacio de mas
     * para entidades sin fisica.
     * Esto nos lleva a la siguiente desventaja y es que al tener un vector tan grande con tanto espacio reservado al hacer un loop para poder iterar los cmps de fisica por ejemplo, cargaremos en cache
     * mucho espacio vacio lo cual es ineficiente para nuestro sistema que se basa en itrerar sobre todos los componentes.
     */
    std::vector<std::optional<PhysicCMP>> physicComponents; ///< Vector donde se guardan todos lo componentes de fisica.
    std::vector<std::optional<RenderCMP>> renderComponents; ///< Vector donde se guardan todos lo componentes de render.
    std::vector<std::optional<InputCMP>> inputComponents;   ///< Vector donde se guardan todos lo componentes de input.

public:
    /**
     * @brief Constructor por defecto.
     * Inicializa los vectores de componentes.
     */
    EntityManager();

    /**
     * @brief Crea una nueva entidad vacía.
     * * Genera un nuevo id único, redimensiona los vectores de componentes si es necesario
     * y registra la entidad como activa.
     * * @return EntityID El id único de la entidad creada.
     */
    EntityID CreateEntity();

    /**
     * @brief Asocia un componente físico a una entidad.
     * * @param id El id de la entidad a la que se le añadirá el componente.
     * @param cmp El cmp PhysicCMP .
     */
    void AddPhysic(EntityID id, PhysicCMP const&);

    /**
     * @brief Obtiene el componente de fisicas de una entidad.
     * * @param id El id de la entidad a consultar.
     * @return PhysicCMP* Puntero al componente para poder modificarlo, o nullptr si no tiene.
     */
    PhysicCMP *GetPhysic(EntityID id);

    /**
     * @brief Asocia un componente de render a una entidad.
     * * @param id El id de la entidad a la que se le añadirá el componente.
     * @param cmp El cmp RenderCMP .
     */
    void AddRender(EntityID id, RenderCMP const&);

    /**
     * @brief Obtiene el componente de fisicas de una entidad.
     * * @param id El id de la entidad a consultar.
     * @return RenderCMP* Puntero al componente para poder modificarlo, o nullptr si no tiene.
     */
    RenderCMP *GetRender(EntityID id);

    /**
     * @brief Asocia un componente de input a una entidad.
     * * @param id El id de la entidad a la que se le añadirá el componente.
     * @param cmp El cmp InputCMP .
     */
    void AddInput(EntityID id, InputCMP const&);

    /**
     * @brief Obtiene el componente de input de una entidad.
     * * @param id El id de la entidad a consultar.
     * @return InputCMP* Puntero al componente para poder modificarlo, o nullptr si no tiene.
     */
    InputCMP *GetInput(EntityID id);

    /**
     * @brief Itera sobre todas las entidades activas y ejecuta una funcion sobre ellas.
     *
     * Recorre la lista interna de entidades vivas (`activeEntities`) y llama a la
     * función proporcionada pasando el ID de la entidad actual.
     *
     * @param func funcion a procesar para cada entidad
     */
    void forAll(const std::function<void(EntityID)> &func);


    /**
     * @brief Devuelve la lista completa de entidades activas.
     * @return Referencia constante al vector de EntityID.
     */
    const std::vector<EntityID> &GetEntities() { return activeEntities; }
};