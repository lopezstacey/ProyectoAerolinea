#ifndef BARRALATERAL_H
#define BARRALATERAL_H
#include <SFML/Graphics.hpp>
#include <string>

using namespace sf;
using namespace std;

class BarraLateral
{
public:
	enum Opcion
	{
		NINGUNA,
		INICIO,
		EXPLORAR_DESTINOS,
		BUSCAR_RUTAS,
		PLANIFICAR_VIAJE,
		MIS_VUELOS,
		MI_EQUIPAJE,
		GESTION_CIUDADES,
		GESTION_AERONAVES,
		GESTION_VUELOS,
		GESTION_RUTAS,
		GESTION_TRIPULACION,
		GESTION_PASAJEROS,
		GESTION_MALETAS,
		ARBOLES_GRAFOS
	};
	
private:
	Font fuente;
	bool fuenteCargada;
	bool clienteDesplegado;
	bool administracionDesplegada;
	int opcionSeleccionada;
	void dibujarBoton(RenderWindow& ventana, float x, float y, float ancho, float alto, const string& texto, Color color);
	void dibujarTexto(RenderWindow& ventana, float x, float y, const string& texto, int tamano, Color color);
	void dibujarRectanguloRedondeado(RenderWindow& ventana, float x, float y, float ancho, float alto, float radio, Color color);
	
public:
	BarraLateral();
	int procesarEvento(RenderWindow& ventana, Event evento);
	void dibujar(RenderWindow& ventana);
	int getOpcionSeleccionada() const;
	void seleccionarOpcion(int opcion);
};

#endif
