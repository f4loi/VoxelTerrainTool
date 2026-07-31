#pragma once
#include <array>
#include <cstdint>
#include <stdexcept>
#include <cassert>

/*
 * Slotmap
 * --------
 * Estructura de datos genérica que permite almacenar elementos con acceso eficiente
 * utilizando claves únicas generadas dinámicamente. Implementa un sistema de manejo
 * de memoria que reutiliza los espacios liberados.
 *
 * INPUTS:
 *   - DataType: Tipo de los datos a almacenar.
 *   - Capacity: Capacidad máxima del Slotmap (número máximo de elementos).
 */
template <typename DataType, std::size_t Capacity>
struct Slotmap
{
    // Definiciones de tipos internos
    using value_type = DataType;      // Tipo de los datos almacenados
    using index_type = std::uint32_t; // Tipo para índices y referencias internas
    using gen_type = index_type;      // Tipo para la generación de las claves
    using key_type = struct           // Clave única que combina un índice y una generación
    {
        index_type id;  // Índice del elemento
        index_type gen; // Generación de la clave
    };
    using iterator = value_type *;             // Iterador para datos modificables
    using const_iterator = value_type const *; // Iterador para datos constantes

    /*
     * Constructor: Slotmap
     * ---------------------
     * Inicializa la estructura, configurando sus datos internos y preparando la lista de espacios libres.
     */
    constexpr explicit Slotmap() : size_(0), freelist_(0), generation_(0)
    {
        clear(); // Inicializa las estructuras internas
    }

    /*
     * Método: size
     * -------------
     * Devuelve el número actual de elementos almacenados en el Slotmap.
     * OUTPUT: std::size_t - Número de elementos almacenados.
     */
    [[nodiscard]] constexpr std::size_t size() const noexcept { return size_; }

    /*
     * Método: capacity
     * -----------------
     * Devuelve la capacidad máxima del Slotmap.
     * OUTPUT: std::size_t - Número máximo de elementos que se pueden almacenar.
     */
    [[nodiscard]] constexpr std::size_t capacity() const noexcept { return Capacity; }

    /*
     * Método: push_back (rvalue)
     * ---------------------------
     * Agrega un nuevo elemento al Slotmap moviendo su valor.
     * INPUT: value_type&& - Referencia rvalue al nuevo valor.
     * OUTPUT: key_type - Clave única asociada al nuevo elemento.
     */
    [[nodiscard]] constexpr key_type push_back(value_type &&newVal)
    {
        auto reserveid = allocate(); // Reserva un nuevo espacio en el Slotmap
        if (reserveid == -1)         // Si no hay espacio disponible, retorna una clave inválida
        {
            return {};
        }

        auto &slot = indices_[reserveid];
        data_[slot.id] = std::move(newVal); // Mueve el nuevo valor al espacio reservado
        erase_[slot.id] = reserveid;

        auto key{slot};
        key.id = reserveid;

        return key; // Retorna la clave única asociada
    }

    /*
     * Método: push_back (const reference)
     * ------------------------------------
     * Agrega un nuevo elemento al Slotmap copiando su valor.
     * INPUT: value_type const& - Referencia constante al nuevo valor.
     * OUTPUT: key_type - Clave única asociada al nuevo elemento.
     */
    [[nodiscard]] constexpr key_type push_back(value_type const &newVal)
    {
        return push_back(value_type{newVal}); // Crea una copia del valor y delega en el otro método
    }

    /*
     * Método: clear
     * --------------
     * Vacía todos los elementos del Slotmap y reinicia las estructuras internas.
     */
    constexpr void clear() noexcept
    {
        freelist_init(); // Reconfigura la lista de espacios libres
    }

    /*
     * Método: erase
     * --------------
     * Elimina un elemento del Slotmap utilizando su clave.
     * INPUT: key_type - Clave del elemento a eliminar.
     * OUTPUT: bool - True si la eliminación fue exitosa, False en caso contrario.
     */
    constexpr bool erase(key_type key) noexcept
    {
        if (!is_valid(key)) // Verifica si la clave es válida
            return false;
        free(key); // Libera el espacio asociado a la clave
        return true;
    }

    /*
     * Método: is_valid
     * -----------------
     * Verifica si una clave es válida dentro del Slotmap.
     * INPUT: key_type - Clave a verificar.
     * OUTPUT: bool - True si la clave es válida, False en caso contrario.
     */
    [[nodiscard]] constexpr bool is_valid(key_type key) const noexcept
    {
        // La clave es válida si el índice está dentro del rango y la generación coincide
        return key.id < Capacity && indices_[key.id].gen == key.gen;
    }

    /*
     * Métodos: begin / end
     * ---------------------
     * Devuelven iteradores para recorrer los datos del Slotmap.
     */
    [[nodiscard]] constexpr iterator begin() noexcept { return data_.begin(); }
    [[nodiscard]] constexpr iterator end() noexcept { return data_.begin() + size_; }

    /*
     * Métodos: cbegin / cend
     * -----------------------
     * Devuelven iteradores constantes para recorrer los datos del Slotmap.
     */
    [[nodiscard]] constexpr iterator cbegin() const noexcept { return data_.cbegin(); }
    [[nodiscard]] constexpr iterator cend() const noexcept { return data_.cbegin() + size_; }

    /*
     * Método: get
     * ------------
     * Devuelve una referencia al elemento asociado a una clave específica.
     * INPUT: key_type - Clave del elemento.
     * OUTPUT: value_type& - Referencia al elemento asociado.
     */
    constexpr value_type &get(key_type key)
    {
        assert(is_valid(key));             // Verifica que la clave es válida
        return data_[indices_[key.id].id]; // Devuelve el elemento correspondiente
    }

private:
    /*
     * Método: allocate
     * -----------------
     * Reserva un nuevo espacio en el Slotmap y devuelve su índice.
     * OUTPUT: index_type - Índice del espacio reservado o -1 si no hay espacio disponible.
     */
    [[nodiscard]] constexpr index_type allocate() noexcept
    {
        if (freelist_ >= Capacity) // Verifica si hay espacios disponibles
        {
            return -1; // Retorna -1 si no hay más espacio
        }
        assert(freelist_ < Capacity);

        auto slotid = freelist_;         // Obtiene el siguiente slot libre
        freelist_ = indices_[slotid].id; // Actualiza la lista de libres

        auto &slot = indices_[slotid];
        slot.id = size_;        // Asigna el índice actual
        slot.gen = generation_; // Asigna la generación actual

        ++size_;       // Incrementa el tamaño
        ++generation_; // Incrementa la generación global

        return slotid;
    }

    /*
     * Método: free
     * -------------
     * Libera un espacio en el Slotmap asociado a una clave específica.
     * INPUT: key_type - Clave del espacio a liberar.
     */
    constexpr void free(key_type key) noexcept
    {
        assert(is_valid(key)); // Verifica que la clave es válida

        auto &slot = indices_[key.id];
        auto dataid = slot.id;

        if (dataid != size_ - 1) // Si no es el último elemento, reorganiza los datos
        {
            data_[dataid] = data_[size_ - 1];
            erase_[dataid] = erase_[size_ - 1];
            indices_[erase_[dataid]].id = dataid;
        }

        slot.id = freelist_;    // Agrega el slot a la lista de libres
        slot.gen = generation_; // Incrementa la generación
        freelist_ = key.id;     // Actualiza freelist_
        --size_;
    }

    /*
     * Método: freelist_init
     * ----------------------
     * Inicializa la lista de espacios libres para reutilización.
     */
    constexpr void freelist_init() noexcept
    {
        for (index_type i = 0; i < indices_.size(); ++i)
        {
            indices_[i].id = i + 1; // Configura el siguiente espacio libre
        }
        freelist_ = 0; // Inicializa el primer espacio libre
    }

    // Atributos internos
    index_type size_{};                        // Número actual de elementos almacenados
    index_type freelist_{};                    // Índice del primer espacio libre
    gen_type generation_{};                    // Contador de generación para claves
    std::array<value_type, Capacity> data_{};  // Almacena los datos
    std::array<key_type, Capacity> indices_{}; // Indices que gestionan las claves
    std::array<index_type, Capacity> erase_{}; // Almacena referencias inversas para reordenar
};
