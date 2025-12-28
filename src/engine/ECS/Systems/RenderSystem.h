/**
 * @file RenderSystem.h
 * @brief Sistema de render.
 * @date 22-12-2025
 */

#pragma once
#include "engine/IGrafica/MotorRaylib.h"
#include "engine/ECS/Managers/EntityManager.h"

/**
 * @class RenderSystem
 * @brief Sistema encargado de dibujar las entidades en pantalla.
 *
 * Este sistema itera sobre las entidades que tienen componentes de renderizado
 * y utiliza la fachada MotorRaylib para dibujarlas en la posición correcta.
 * Se encarga de abrir la ventana y gestionar el bucle de dibujo (BeginDraw/EndDraw).
 */
class RenderSystem
{
private:
    /**
     * @brief Instancia de la fachada gráfica.
     * Encapsula las llamadas de bajo nivel a la librería Raylib.
     */
    MotorRaylib motorRaylib;
    
public:
    /**
     * @brief Inicializa el contexto gráfico.
     *
     * Configura la ventana, resolución y carga los recursos necesarios
     * antes de empezar el bucle de renderizado.
     */
    void Init();

    /**
     * @brief Ejecuta el ciclo de renderizado de un frame.
     *
     * 1. Limpia la pantalla.
     * 2. Itera sobre el EntityManage` buscando entidades con RenderCMP y PhysicCMP.
     * 3. Dibuja cada entidad válida.
     * 4. renderiza el frame en pantalla.
     *
     * @param em Referencia al manager de entidades para acceder a los componentes.
     */
    void Update(EntityManager& em);
};