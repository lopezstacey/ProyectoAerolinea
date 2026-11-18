#ifndef PANTALLABUSCARRUTAS_H
#define PANTALLABUSCARRUTAS_H
#include "Pantalla.h"
#include "Grafo.h"
#include "Dijkstra.h"
#include <SFML/Graphics.hpp>
#include <string>
#include <vector>

using namespace sf;
using namespace std;

class ListaPasajeros;
class ListaMaletas;
class ListaVuelos;
class ListaReservas;
class GestorPantallas;

class PantallaBuscarRutas : public Pantalla
{
private:
	struct RutaSeleccionada
	{
		vector<string> ciudades;
		vector<string> codigosVuelos;
		float costo;
		float duracion;
	};
	
	Grafo* grafo;
	ListaPasajeros* listaPasajeros;
	ListaMaletas* listaMaletas;
	ListaVuelos* listaVuelos;
	ListaReservas* listaReservas;
	GestorPantallas* gestorPantallas;
	Dijkstra dijkstra;
	Font fuente;
	Texture texturaMapa;
	Sprite mapa;
	bool fuenteCargada;
	bool mapaCargado;
	string codigoOrigen;
	string codigoDestino;
	bool seleccionandoOrigen;
	bool porCosto;
	string* ruta;
	string* codigosVuelosRuta;
	int cantidadRuta;
	float costoTotal;
	float duracionTotal;
	bool resultadoCalculado;
	bool rutaEncontrada;
	string mensaje;
	int desplazamientoCiudades;
	vector<RutaSeleccionada> rutasSeleccionadas;
	float costoAcumulado;
	int desplazamientoRutas;
	int desplazamientoDetalleRuta;
	bool viajeConfirmado;
	void dibujarTexto(RenderWindow& ventana, float x, float y, const string& texto, unsigned int tamano, Color color, bool negrita = false);
	void dibujarPanel(RenderWindow& ventana, float x, float y, float ancho, float alto, Color color);
	void limpiarResultado();
	void dibujarMapa(RenderWindow& ventana);
	void dibujarRuta(RenderWindow& ventana);
	void dibujarMarcadores(RenderWindow& ventana);
	NodoVertice* buscarCiudadPorNombre(const string& nombre) const;
public:
	PantallaBuscarRutas(Grafo* grafo, ListaPasajeros* pasajeros, ListaMaletas* maletas,
						ListaVuelos* vuelos, ListaReservas* reservas, GestorPantallas* gestorPantallas);
	~PantallaBuscarRutas();
	void procesarEventos(RenderWindow& ventana, Event evento);
	void actualizar(float tiempo);
	void dibujar(RenderWindow& ventana);
};

#endif
