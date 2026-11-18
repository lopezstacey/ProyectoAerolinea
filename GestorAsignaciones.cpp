#include "GestorAsignaciones.h"

GestorAsignaciones::GestorAsignaciones()
{
    primeroTripulacion = nullptr;
    cantidadTripulacion = 0;
}

GestorAsignaciones::~GestorAsignaciones()
{
    limpiar();
}

bool GestorAsignaciones::asignarTripulante(string identificacionTripulante, string matriculaAeronave)
{
    if (identificacionTripulante.empty() || matriculaAeronave.empty())
        return false;

    NodoAsignacionTripulante* actual = primeroTripulacion;
    while (actual != nullptr)
    {
        if (actual->identificacionTripulante == identificacionTripulante)
        {
            actual->matriculaAeronave = matriculaAeronave;
            return true;
        }
        actual = actual->siguiente;
    }

    NodoAsignacionTripulante* nuevo = new NodoAsignacionTripulante(identificacionTripulante, matriculaAeronave);
    if (primeroTripulacion == nullptr)
        primeroTripulacion = nuevo;
    else
    {
        actual = primeroTripulacion;
        while (actual->siguiente != nullptr)
            actual = actual->siguiente;
        actual->siguiente = nuevo;
    }
    cantidadTripulacion++;
    return true;
}

bool GestorAsignaciones::cambiarAeronaveTripulante(string identificacionTripulante, string nuevaMatricula)
{
    if (nuevaMatricula.empty())
        return false;

    NodoAsignacionTripulante* actual = primeroTripulacion;
    while (actual != nullptr)
    {
        if (actual->identificacionTripulante == identificacionTripulante)
        {
            actual->matriculaAeronave = nuevaMatricula;
            return true;
        }
        actual = actual->siguiente;
    }
    return false;
}

bool GestorAsignaciones::quitarTripulante(string identificacionTripulante)
{
    NodoAsignacionTripulante* actual = primeroTripulacion;
    NodoAsignacionTripulante* anterior = nullptr;

    while (actual != nullptr)
    {
        if (actual->identificacionTripulante == identificacionTripulante)
        {
            if (anterior == nullptr)
                primeroTripulacion = actual->siguiente;
            else
                anterior->siguiente = actual->siguiente;
            delete actual;
            cantidadTripulacion--;
            return true;
        }
        anterior = actual;
        actual = actual->siguiente;
    }
    return false;
}

int GestorAsignaciones::quitarAsignacionesAeronave(string matriculaAeronave)
{
    int eliminadas = 0;
    NodoAsignacionTripulante* anterior = nullptr;
    NodoAsignacionTripulante* actual = primeroTripulacion;

    while (actual != nullptr)
    {
        NodoAsignacionTripulante* siguiente = actual->siguiente;
        if (actual->matriculaAeronave == matriculaAeronave)
        {
            if (anterior == nullptr)
                primeroTripulacion = siguiente;
            else
                anterior->siguiente = siguiente;
            delete actual;
            cantidadTripulacion--;
            eliminadas++;
        }
        else
            anterior = actual;
        actual = siguiente;
    }

    return eliminadas;
}

string GestorAsignaciones::buscarAeronaveTripulante(string identificacionTripulante) const
{
    NodoAsignacionTripulante* actual = primeroTripulacion;
    while (actual != nullptr)
    {
        if (actual->identificacionTripulante == identificacionTripulante)
            return actual->matriculaAeronave;
        actual = actual->siguiente;
    }
    return "";
}

NodoAsignacionTripulante* GestorAsignaciones::getPrimeroTripulacion() const
{
    return primeroTripulacion;
}

int GestorAsignaciones::getCantidadTripulacion() const
{
    return cantidadTripulacion;
}

bool GestorAsignaciones::asignarPasajeroVuelo(ArbolAVL& arbol, Pasajero pasajero, string codigoVuelo)
{
    if (pasajero.getIdentificacion().empty() || codigoVuelo.empty())
        return false;
    arbol.insertar(pasajero, codigoVuelo);
    return arbol.buscarVuelo(pasajero.getIdentificacion()) == codigoVuelo;
}

string GestorAsignaciones::buscarVueloPasajero(ArbolAVL& arbol, string identificacion) const
{
    return arbol.buscarVuelo(identificacion);
}

bool GestorAsignaciones::calcularRutaEconomica(Grafo& grafo, string codigoOrigen, string codigoDestino,
                                               string*& ruta, int& cantidad, float& costoTotal,
                                               float& duracionTotal)
{
    Dijkstra dijkstra;
    return dijkstra.calcular(grafo, codigoOrigen, codigoDestino, true,
                             ruta, cantidad, costoTotal, duracionTotal);
}

void GestorAsignaciones::limpiar()
{
    while (primeroTripulacion != nullptr)
    {
        NodoAsignacionTripulante* borrar = primeroTripulacion;
        primeroTripulacion = primeroTripulacion->siguiente;
        delete borrar;
    }
    cantidadTripulacion = 0;
}
