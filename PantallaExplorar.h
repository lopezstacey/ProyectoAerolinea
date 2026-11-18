#ifndef PANTALLAEXPLORAR_H
#define PANTALLAEXPLORAR_H
#include "Pantalla.h"
#include "Grafo.h"
#include "BellmanFord.h"
#include <SFML/Graphics.hpp>
#include <string>

using namespace sf;
using namespace std;

class PantallaExplorar : public Pantalla
{
private:
	Grafo* grafo;
	BellmanFord algoritmo;
	
	Font fuente;
	bool fuenteCargada;
	Texture texturaMapa;
	Sprite mapa;
	bool mapaCargado;
	
	string textoBusqueda;
	string codigoOrigen;
	bool modoDirectas;
	bool porCosto;
	int desplazamientoCiudades;
	int desplazamientoConexiones;
	
	ResultadoBellmanFord* resultados;
	int cantidadResultados;
	int resultadoSeleccionado;
	int desplazamientoResultados;
	bool calculado;
	string mensaje;
	
	void liberarResultados();
	void calcularResultados();
	
	bool coincideBusqueda(NodoVertice* vertice) const;
	int contarCoincidencias() const;
	NodoVertice* ciudadVisible(int posicion) const;
	
	void dibujarTexto(RenderWindow& ventana, float x, float y, const string& texto, unsigned int tamano, Color color, bool negrita = false);
	void dibujarEncabezado(RenderWindow& ventana);
	void dibujarListaCiudades(RenderWindow& ventana);
	void dibujarControles(RenderWindow& ventana);
	void dibujarAlcanzables(RenderWindow& ventana);
	void dibujarTablaDestinos(RenderWindow& ventana);
	void dibujarDetalleRuta(RenderWindow& ventana);
	void dibujarDirectas(RenderWindow& ventana);
	void dibujarMapa(RenderWindow& ventana);
	void dibujarRutaSeleccionada(RenderWindow& ventana);
	void dibujarMarcadores(RenderWindow& ventana);
	
public:
	PantallaExplorar(Grafo* grafo);
	~PantallaExplorar();
	void procesarEventos(RenderWindow& ventana, Event evento);
	void actualizar(float tiempo);
	void dibujar(RenderWindow& ventana);
};

#endif
