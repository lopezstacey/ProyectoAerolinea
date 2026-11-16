#include "Grafo.h"

Grafo::Grafo()
{
    primero = nullptr;
    cantidadVertices = 0;
}

Grafo::~Grafo()
{
    limpiar();
}

bool Grafo::agregarCiudad(Ciudad ciudad)
{
    if (existeCiudad(ciudad.getCodigo()))
        return false;

    NodoVertice* nuevo = new NodoVertice(ciudad);

    if (primero == nullptr)
        primero = nuevo;
    else
    {
        NodoVertice* actual = primero;
        while (actual->siguiente != nullptr)
            actual = actual->siguiente;
        actual->siguiente = nuevo;
    }

    cantidadVertices++;
    return true;
}

bool Grafo::agregarVuelo(Vuelo vuelo)
{
    NodoVertice* origen = buscarCiudad(vuelo.getCodigoOrigen());

    if (origen == nullptr || buscarCiudad(vuelo.getCodigoDestino()) == nullptr)
        return false;

    NodoArista* nuevo = new NodoArista(vuelo);

    if (origen->conexiones == nullptr)
        origen->conexiones = nuevo;
    else
    {
        NodoArista* actual = origen->conexiones;
        while (actual->siguiente != nullptr)
            actual = actual->siguiente;
        actual->siguiente = nuevo;
    }

    return true;
}

bool Grafo::existeCiudad(string codigo) const
{
    return buscarCiudad(codigo) != nullptr;
}

NodoVertice* Grafo::buscarCiudad(string codigo) const
{
    NodoVertice* actual = primero;

    while (actual != nullptr)
    {
        if (actual->ciudad.getCodigo() == codigo)
            return actual;
        actual = actual->siguiente;
    }

    return nullptr;
}

NodoVertice* Grafo::getPrimero() const
{
    return primero;
}

int Grafo::getCantidadVertices() const
{
    return cantidadVertices;
}

bool Grafo::modificarCiudad(string codigo, string nombre, float x, float y)
{
    NodoVertice* ciudad = buscarCiudad(codigo);

    if (ciudad == nullptr)
        return false;

    ciudad->ciudad.setNombre(nombre);
    ciudad->ciudad.setPosicionX(x);
    ciudad->ciudad.setPosicionY(y);

    return true;
}

bool Grafo::eliminarCiudad(string codigo)
{
    NodoVertice* anterior = nullptr;
    NodoVertice* actual = primero;

    while (actual != nullptr && actual->ciudad.getCodigo() != codigo)
    {
        anterior = actual;
        actual = actual->siguiente;
    }

    if (actual == nullptr)
        return false;

    NodoVertice* origen = primero;

    while (origen != nullptr)
    {
        NodoArista* arista = origen->conexiones;
        NodoArista* anteriorArista = nullptr;

        while (arista != nullptr)
        {
            if (arista->vuelo.getCodigoOrigen() == codigo || arista->vuelo.getCodigoDestino() == codigo)
            {
                NodoArista* eliminar = arista;

                if (anteriorArista == nullptr)
                    origen->conexiones = arista->siguiente;
                else
                    anteriorArista->siguiente = arista->siguiente;

                arista = arista->siguiente;
                delete eliminar;
            }
            else
            {
                anteriorArista = arista;
                arista = arista->siguiente;
            }
        }

        origen = origen->siguiente;
    }

    if (anterior == nullptr)
        primero = actual->siguiente;
    else
        anterior->siguiente = actual->siguiente;

    delete actual;
    cantidadVertices--;
    return true;
}

void Grafo::limpiar()
{
    NodoVertice* actual = primero;

    while (actual != nullptr)
    {
        NodoVertice* eliminarVertice = actual;
        NodoArista* arista = actual->conexiones;

        while (arista != nullptr)
        {
            NodoArista* eliminarArista = arista;
            arista = arista->siguiente;
            delete eliminarArista;
        }

        actual = actual->siguiente;
        delete eliminarVertice;
    }

    primero = nullptr;
    cantidadVertices = 0;
}
