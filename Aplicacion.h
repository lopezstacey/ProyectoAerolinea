#ifndef APLICACION_H
#define APLICACION_H
#include <SFML/Graphics.hpp>
#include "GestorPantallas.h"
#include "BarraLateral.h"
#include "Grafo.h"
#include "ListaAeronaves.h"
#include "ListaTripulantes.h"
#include "ListaPasajeros.h"
#include "ListaMaletas.h"
using namespace sf;

class Aplicacion
{
private:
	RenderWindow ventana;
	Grafo grafoInterno;
	Grafo* grafo;
	ListaAeronaves listaAeronaves;
	ListaTripulantes listaTripulantes;
	ListaPasajeros listaPasajeros;
	ListaMaletas listaMaletas;
	GestorPantallas gestorPantallas;
	BarraLateral barraLateral;
	Clock reloj;
	bool ejecutando;
	void procesarEventos();
	void actualizar();
	void dibujar();
public:
	Aplicacion();
	Aplicacion(Grafo* grafoCompartido);
	void ejecutar();
};

#endif
