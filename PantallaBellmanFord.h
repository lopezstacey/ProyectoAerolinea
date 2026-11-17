#ifndef PANTALLABELLMANFORD_H
#define PANTALLABELLMANFORD_H
#include "Pantalla.h"
#include "Grafo.h"
#include "BellmanFord.h"
#include <SFML/Graphics.hpp>
#include <string>

using namespace sf;
using namespace std;

class PantallaBellmanFord : public Pantalla
{
private:
	Grafo* grafo;
	BellmanFord algoritmo;
	Font fuente;
	Texture texturaMapa;
	Sprite mapa;
	bool fuenteCargada;
	bool mapaCargado;
	string codigoOrigen;
	bool porCosto;
	int desplazamientoCiudades;
	int resultadoSeleccionado;
	int desplazamientoResultados;
	ResultadoBellmanFord* resultados;
	int cantidadResultados;
	bool calculado;
	string mensaje;
	void liberarResultados();
	void calcularResultados();
	void dibujarTexto(RenderWindow& ventana, float x, float y, const string& texto, unsigned int tamano, Color color, bool negrita = false);
	void dibujarMapa(RenderWindow& ventana);
	void dibujarRutaSeleccionada(RenderWindow& ventana);
	void dibujarMarcadores(RenderWindow& ventana);
public:
	PantallaBellmanFord(Grafo* grafo);
	~PantallaBellmanFord();
	void procesarEventos(RenderWindow& ventana, Event evento);
	void actualizar(float tiempo);
	void dibujar(RenderWindow& ventana);
};

#endif
