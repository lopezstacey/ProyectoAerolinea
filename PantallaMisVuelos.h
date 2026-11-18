#ifndef PANTALLAMISVUELOS_H
#define PANTALLAMISVUELOS_H
#include "Pantalla.h"
#include "ListaPasajeros.h"
#include "ListaMaletas.h"
#include "ListaVuelos.h"
#include "ListaReservas.h"
#include "GestorPantallas.h"
#include "Grafo.h"
#include <SFML/Graphics.hpp>
#include <string>
using namespace sf;
using namespace std;

class PantallaMisVuelos : public Pantalla
{
private:
	ListaPasajeros* listaPasajeros;
	ListaMaletas* listaMaletas;
	ListaVuelos* listaVuelos;
	ListaReservas* listaReservas;
	GestorPantallas* gestorPantallas;
	Grafo* grafo;
	Font fuente;
	bool fuenteCargada;
	string cedula;
	string mensaje;
	bool cursorVisible;
	float tiempoCursor;
	void buscar();
	void dibujarTexto(RenderWindow& ventana, float x, float y, string texto, int tamano, Color color);
public:
	PantallaMisVuelos(ListaPasajeros* listaPasajeros, ListaMaletas* listaMaletas, ListaVuelos* listaVuelos, ListaReservas* listaReservas, GestorPantallas* gestorPantallas, Grafo* grafo);
	void procesarEventos(RenderWindow& ventana, Event evento);
	void actualizar(float tiempo);
	void dibujar(RenderWindow& ventana);
};

#endif
