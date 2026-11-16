#ifndef PANTALLAGESTIONRUTAS_H
#define PANTALLAGESTIONRUTAS_H
#include "Pantalla.h"
#include "ListaRutas.h"
#include "ListaAeronaves.h"
#include "ListaVuelos.h"
#include <SFML/Graphics.hpp>
#include <string>

using namespace sf;
using namespace std;

class PantallaGestionRutas : public Pantalla
{
private:
	ListaRutas* listaRutas;
	ListaAeronaves* listaAeronaves;
	ListaVuelos* listaVuelos;
	Font fuente;
	bool fuenteCargada;
	string codigo;
	string matriculaSeleccionada;
	string vueloSeleccionado;
	string codigoSeleccionado;
	string mensaje;
	int campoActivo;
	bool editando;
	bool confirmarEliminar;
	bool desplegarAeronaves;
	bool desplegarVuelos;
	bool cursorVisible;
	float tiempoCursor;
public:
	PantallaGestionRutas(ListaRutas* listaRutas, ListaAeronaves* listaAeronaves, ListaVuelos* listaVuelos);
	void procesarEventos(RenderWindow& ventana, Event evento);
	void actualizar(float tiempo);
	void dibujar(RenderWindow& ventana);
	void registrarRuta();
	void modificarRuta();
	void eliminarRuta();
	void seleccionarRuta(string codigo);
	void limpiarFormulario();
	void dibujarFormulario(RenderWindow& ventana);
	void dibujarListado(RenderWindow& ventana);
	void dibujarDesplegables(RenderWindow& ventana);
	void dibujarConfirmacion(RenderWindow& ventana);
	void dibujarTexto(RenderWindow& ventana, float x, float y, string texto, int tamano, Color color);
};

#endif
