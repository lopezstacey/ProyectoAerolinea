#ifndef PANTALLARESUMEN_H
#define PANTALLARESUMEN_H
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

class PantallaResumen : public Pantalla
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
	string identificacion;
	vector<string> codigosVuelos;
	vector<string> detallesRutas;
	float costoTotal;
	string codigoMaleta;
	bool modoConsulta;
	int desplazamiento;
	void dibujarTexto(RenderWindow& ventana, float x, float y, string texto, int tamano, Color color);
	void dibujarCampo(RenderWindow& ventana, float x, float y, string etiqueta, string valor);
	void dibujarPasajero(RenderWindow& ventana);
	void dibujarEquipaje(RenderWindow& ventana);
	void dibujarItinerarios(RenderWindow& ventana);
	void dibujarBotones(RenderWindow& ventana);
public:
	PantallaResumen(ListaPasajeros* listaPasajeros, ListaMaletas* listaMaletas, ListaVuelos* listaVuelos, ListaReservas* listaReservas, GestorPantallas* gestorPantallas, Grafo* grafo,
					string identificacion, vector<string> codigosVuelos, vector<string> detallesRutas, float costoTotal, string codigoMaleta = "", bool modoConsulta = false);
	void procesarEventos(RenderWindow& ventana, Event evento);
	void actualizar(float tiempo);
	void dibujar(RenderWindow& ventana);
};

#endif
