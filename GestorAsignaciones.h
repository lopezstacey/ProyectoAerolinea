#ifndef GESTORASIGNACIONES_H
#define GESTORASIGNACIONES_H

#include <string>
#include "ArbolAVL.h"
#include "Grafo.h"
#include "Dijkstra.h"
using namespace std;

struct NodoAsignacionTripulante
{
    string identificacionTripulante;
    string matriculaAeronave;
    NodoAsignacionTripulante* siguiente;

    NodoAsignacionTripulante(string identificacion, string matricula)
    {
        identificacionTripulante = identificacion;
        matriculaAeronave = matricula;
        siguiente = nullptr;
    }
};

class GestorAsignaciones
{
private:
    NodoAsignacionTripulante* primeroTripulacion;
    int cantidadTripulacion;

public:
    GestorAsignaciones();
    ~GestorAsignaciones();

    bool asignarTripulante(string identificacionTripulante, string matriculaAeronave);
    bool cambiarAeronaveTripulante(string identificacionTripulante, string nuevaMatricula);
    bool quitarTripulante(string identificacionTripulante);
    string buscarAeronaveTripulante(string identificacionTripulante) const;
    NodoAsignacionTripulante* getPrimeroTripulacion() const;
    int getCantidadTripulacion() const;

    bool asignarPasajeroVuelo(ArbolAVL& arbol, Pasajero pasajero, string codigoVuelo);
    string buscarVueloPasajero(ArbolAVL& arbol, string identificacion) const;

    bool calcularRutaEconomica(Grafo& grafo, string codigoOrigen, string codigoDestino,
                               string*& ruta, int& cantidad, float& costoTotal,
                               float& duracionTotal);
    void limpiar();
};

#endif
