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
#include "ListaVuelos.h"
#include "ListaRutas.h"
#include "ListaReservas.h"
#include "GestorAsignaciones.h"
#include "ArbolAVL.h"
#include "PantallaMiEquipaje.h"
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
	ListaVuelos listaVuelos;
	ListaRutas listaRutas;
	ListaReservas listaReservas;
	GestorAsignaciones gestorAsignaciones;
	ArbolAVL arbolPasajeros;
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
