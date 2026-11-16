#ifndef APLICACION_H
#define APLICACION_H
#include <SFML/Graphics.hpp>
#include "GestorPantallas.h"
#include "BarraLateral.h"
#include "Grafo.h"
#include "ListaAeronaves.h"
using namespace sf;

class Aplicacion
{
private:
	RenderWindow ventana;
	Grafo grafoInterno;
	Grafo* grafo;
	ListaAeronaves listaAeronaves;
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
