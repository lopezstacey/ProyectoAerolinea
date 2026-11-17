#ifndef PANTALLAEXPLORAR_H
#define PANTALLAEXPLORAR_H
#include "Pantalla.h"
#include "Grafo.h"
#include <SFML/Graphics.hpp>
#include <string>

using namespace sf;
using namespace std;

class PantallaExplorar : public Pantalla
{
private:
	Grafo* grafo;
	Font fuente;
	bool fuenteCargada;
	string textoBusqueda;
	string codigoCiudadSeleccionada;
	int desplazamientoCiudades;
	void dibujarTexto(RenderWindow& ventana, float x, float y, const string& texto, unsigned int tamano, Color color, bool negrita = false);
	void dibujarPanel(RenderWindow& ventana, float x, float y, float ancho, float alto, Color color);
	void seleccionarPrimeraCiudadVisible();
	
public:
	PantallaExplorar(Grafo* grafo);
	void procesarEventos(RenderWindow& ventana, Event evento);
	void actualizar(float tiempo);
	void dibujar(RenderWindow& ventana);
};

#endif
