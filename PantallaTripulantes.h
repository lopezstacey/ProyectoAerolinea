#ifndef PANTALLATRIPULANTES_H
#define PANTALLATRIPULANTES_H
#include "Pantalla.h"
#include "ListaTripulantes.h"
#include <SFML/Graphics.hpp>
#include <string>
using namespace sf;
using namespace std;

class PantallaTripulantes : public Pantalla
{
private:
	ListaTripulantes* listaTripulantes;
	Font fuente;
	bool fuenteCargada;
	string identificacion;
	string nombre;
	string puesto;
	string estado;
	string identificacionSeleccionada;
	string mensaje;
	int campoActivo;
	bool editando;
	bool confirmarEliminar;
	bool cursorVisible;
	float tiempoCursor;
public:
	PantallaTripulantes(ListaTripulantes* listaTripulantes);
	void procesarEventos(RenderWindow& ventana, Event evento);
	void actualizar(float tiempo);
	void dibujar(RenderWindow& ventana);
	void registrarTripulante();
	void seleccionarTripulante(string identificacion);
	void modificarTripulante();
	void eliminarTripulante();
	void limpiarFormulario();
	void dibujarFormulario(RenderWindow& ventana);
	void dibujarListado(RenderWindow& ventana);
	void dibujarConfirmacion(RenderWindow& ventana);
	void dibujarTexto(RenderWindow& ventana, float x, float y, string texto, int tamano, Color color);
};

#endif
