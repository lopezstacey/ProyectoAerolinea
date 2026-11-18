#ifndef PANTALLAMIEQUIPAJE_H
#define PANTALLAMIEQUIPAJE_H
#include "Pantalla.h"
#include "ListaPasajeros.h"
#include "ListaMaletas.h"
#include "ListaVuelos.h"
#include "ListaReservas.h"
#include "Grafo.h"
#include "Dijkstra.h"
#include <SFML/Graphics.hpp>
#include <string>
#include <vector>
using namespace sf;
using namespace std;


class PantallaMiEquipaje : public Pantalla
{
private:
	ListaPasajeros* listaPasajeros;
	ListaMaletas* listaMaletas;
	ListaVuelos* listaVuelos;
	ListaReservas* listaReservas;
	Grafo* grafo;
	Dijkstra dijkstra;
	Font fuente;
	bool fuenteCargada;
	int etapa;
	string cedula;
	string mensaje;
	string nombrePasajero;
	vector<string> codigosMaletas;
	int maletaSeleccionada;
	int desplazamientoMaletas;
	int desplazamientoRuta;
	bool cursorVisible;
	float tiempoCursor;
	bool hayRuta;
	string mensajeRuta;
	vector<string> ciudadesMaleta;
	vector<string> vuelosMaleta;
	float costoMaleta;
	float duracionMaleta;
	bool hayRutaPasajero;
	vector<string> ciudadesPasajero;
	vector<string> vuelosPasajero;
	float costoPasajero;
	float duracionPasajero;
	string comparacion;
	void buscar();
	void seleccionarMaleta(int indice);
	void calcularRutas(const Maleta& maleta);
	void dibujarTexto(RenderWindow& ventana, float x, float y, string texto, int tamano, Color color);
	void dibujarConsulta(RenderWindow& ventana);
	void dibujarMaletas(RenderWindow& ventana);
	void dibujarRutaMaleta(RenderWindow& ventana);
	void dibujarComparacion(RenderWindow& ventana);
public:
	PantallaMiEquipaje(Grafo* grafo, ListaPasajeros* listaPasajeros, ListaMaletas* listaMaletas, ListaVuelos* listaVuelos, ListaReservas* listaReservas);
	void procesarEventos(RenderWindow& ventana, Event evento);
	void actualizar(float tiempo);
	void dibujar(RenderWindow& ventana);
};

#endif
