#ifndef PANTALLAVUELOS_H
#define PANTALLAVUELOS_H
#include "Pantalla.h"
#include "ListaVuelos.h"
#include "Grafo.h"
#include <SFML/Graphics.hpp>
#include <string>
#include <vector>
using namespace sf;
using namespace std;

class PantallaVuelos : public Pantalla
{
private:
    ListaVuelos* listaVuelos;
    Grafo* grafo;
    Font fuente;
    bool fuenteCargada;
    string codigo;
    string codigoOrigen;
    string codigoDestino;
    string costo;
    string duracion;
    string codigoSeleccionado;
    string mensaje;
    int campoActivo;
    bool editando;
    bool confirmarEliminar;
    bool cursorVisible;
    float tiempoCursor;
    bool origenDesplegado;
    bool destinoDesplegado;
    int desplazamientoOrigen;
    int desplazamientoDestino;
    string filtroOrigen;
    string filtroDestino;
    vector<NodoVertice*> obtenerCiudadesDisponibles(bool origen);
    void dibujarListaCiudades(RenderWindow& ventana, bool origen);
public:
    PantallaVuelos(ListaVuelos* listaVuelos, Grafo* grafo);
    void procesarEventos(RenderWindow& ventana, Event evento);
    void actualizar(float tiempo);
    void dibujar(RenderWindow& ventana);
    void registrarVuelo();
    void seleccionarVuelo(string codigo);
    void modificarVuelo();
    void eliminarVuelo();
    void limpiarFormulario();
    void dibujarConfirmacion(RenderWindow& ventana);
    void dibujarFormulario(RenderWindow& ventana);
    void dibujarListado(RenderWindow& ventana);
    void dibujarTexto(RenderWindow& ventana, float x, float y, string texto, int tamano, Color color);
};

#endif
