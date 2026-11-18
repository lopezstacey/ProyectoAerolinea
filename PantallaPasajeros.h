#ifndef PANTALLAPASAJEROS_H
#define PANTALLAPASAJEROS_H
#include "Pantalla.h"
#include "ListaPasajeros.h"
#include "ListaMaletas.h"
#include "ListaVuelos.h"
#include "ListaReservas.h"
#include "GestorPantallas.h"
#include "Grafo.h"                      
#include <SFML/Graphics.hpp>
#include <string>
#include <vector>

using namespace sf;
using namespace std;

class PantallaPasajeros : public Pantalla
{
private:
	ListaPasajeros* listaPasajeros;
	ListaMaletas* listaMaletas;
	GestorPantallas* gestorPantallas;
	Grafo* grafo;                       
	ListaVuelos* listaVuelos;
	ListaReservas* listaReservas;
	vector<string> codigosVuelosReserva;
	vector<string> detallesRutasReserva;
	float costoReserva;
	bool modoReserva;
	bool reservaConfirmada;
	Font fuente;
	bool fuenteCargada;
	string identificacion;
	string nombre;
	string telefono;
	string correo;
	string pesoMaleta;
	string identificacionSeleccionada;
	string mensaje;
	int campoActivo;
	int desplazamientoLista;
	bool editando;
	bool confirmarEliminar;
	bool llevaMaleta;
	bool cursorVisible;
	float tiempoCursor;	
public:
	PantallaPasajeros(ListaPasajeros* listaPasajeros, ListaMaletas* listaMaletas,
					  GestorPantallas* gestorPantallas, Grafo* grafo = nullptr, ListaVuelos* vuelos = nullptr,
					  ListaReservas* reservas = nullptr, vector<string> codigosVuelos = vector<string>(),
					  vector<string> detallesRutas = vector<string>(), float costoReserva = 0.0f);
	void procesarEventos(RenderWindow& ventana, Event evento);
	void actualizar(float tiempo);
	void dibujar(RenderWindow& ventana);
	void registrarPasajero();
	void seleccionarPasajero(string identificacion);
	void modificarPasajero();
	void eliminarPasajero();
	void limpiarFormulario();
	void dibujarFormulario(RenderWindow& ventana);
	void dibujarListado(RenderWindow& ventana);
	void dibujarDatosReserva(RenderWindow& ventana);
	void dibujarConfirmacion(RenderWindow& ventana);
	void dibujarTexto(RenderWindow& ventana, float x, float y, string texto, int tamano, Color color);
};

#endif
