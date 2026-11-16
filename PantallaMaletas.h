#ifndef PANTALLAMALETAS_H
#define PANTALLAMALETAS_H
#include "Pantalla.h"
#include "ListaMaletas.h"
#include <SFML/Graphics.hpp>
#include <string>

using namespace sf;
using namespace std;

class PantallaMaletas : public Pantalla
{
private:
	ListaMaletas* listaMaletas;
	Font fuente;
	bool fuenteCargada;
	string codigo;
	string identificacionPasajero;
	string peso;
	string destino;
	string estado;
	string codigoSeleccionado;
	string mensaje;
	int campoActivo;
	bool editando;
	bool identificacionBloqueada;
	bool confirmarEliminar;
	bool cursorVisible;
	float tiempoCursor;
	
public:
	PantallaMaletas(ListaMaletas* listaMaletas, string identificacionInicial = "");
	void procesarEventos(RenderWindow& ventana, Event evento);
	void actualizar(float tiempo);
	void dibujar(RenderWindow& ventana);
	void registrarMaleta();
	void seleccionarMaleta(string codigo);
	void modificarMaleta();
	void eliminarMaleta();
	void limpiarFormulario();
	void dibujarFormulario(RenderWindow& ventana);
	void dibujarListado(RenderWindow& ventana);
	void dibujarConfirmacion(RenderWindow& ventana);
	void dibujarTexto(RenderWindow& ventana, float x, float y, string texto, int tamano, Color color);
};

#endif
