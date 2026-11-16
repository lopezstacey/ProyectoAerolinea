#ifndef GRAFO_H
#define GRAFO_H

#include <string>
#include "Ciudad.h"
#include "Vuelo.h"

using namespace std;

struct NodoArista
{
    Vuelo vuelo;
    NodoArista* siguiente;

    NodoArista(Vuelo vuelo)
    {
        this->vuelo = vuelo;
        siguiente = nullptr;
    }
};

struct NodoVertice
{
    Ciudad ciudad;
    NodoArista* conexiones;
    NodoVertice* siguiente;

    NodoVertice(Ciudad ciudad)
    {
        this->ciudad = ciudad;
        conexiones = nullptr;
        siguiente = nullptr;
    }
};

class Grafo
{
private:
    NodoVertice* primero;
    int cantidadVertices;

public:
    Grafo();
    ~Grafo();
    bool agregarCiudad(Ciudad ciudad);
    bool agregarVuelo(Vuelo vuelo);
    bool existeCiudad(string codigo) const;
    NodoVertice* buscarCiudad(string codigo) const;
    NodoVertice* getPrimero() const;
    int getCantidadVertices() const;
    bool modificarCiudad(string codigo, string nombre, float x, float y);
    bool eliminarCiudad(string codigo);
    void limpiar();
};

#endif
