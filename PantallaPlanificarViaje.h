#ifndef PANTALLAPLANIFICARVIAJE_H
#define PANTALLAPLANIFICARVIAJE_H
#include "Pantalla.h"
#include "ListaPasajeros.h"
#include "ListaVuelos.h"
#include "ArbolAVL.h"
#include "GestorAsignaciones.h"
#include "Grafo.h"
#include <SFML/Graphics.hpp>
#include <string>
using namespace sf;
using namespace std;

class PantallaPlanificarViaje : public Pantalla
{
private:
	ListaPasajeros* listaPasajeros;
	ListaVuelos* listaVuelos;
	ArbolAVL* arbolPasajeros;
	GestorAsignaciones* gestorAsignaciones;
	Grafo* grafo;
	Font fuente;
	bool fuenteCargada;
	string identificacionSeleccionada;
	string vueloSeleccionado;
	string mensaje;
	int desplazamientoPasajeros;
	int desplazamientoVuelos;
	void dibujarTexto(RenderWindow&, float, float, const string&, unsigned int, Color, bool negrita = false);
	void dibujarPasajeros(RenderWindow&);
	void dibujarVuelos(RenderWindow&);
	string nombreCiudad(const string&);
	void asignarVuelo();
public:
	PantallaPlanificarViaje(ListaPasajeros*, ListaVuelos*, ArbolAVL*, GestorAsignaciones*, Grafo* grafo = nullptr);
	void procesarEventos(RenderWindow&, Event);
	void actualizar(float);
	void dibujar(RenderWindow&);
};
#endif
