#ifndef PANTALLATRIPULANTES_H
#define PANTALLATRIPULANTES_H

#include "Pantalla.h"
#include "ListaTripulantes.h"
#include "ListaAeronaves.h"
#include "GestorAsignaciones.h"
#include <SFML/Graphics.hpp>
#include <string>

using namespace sf;
using namespace std;

class PantallaTripulantes : public Pantalla
{
private:
	ListaTripulantes* listaTripulantes;
	ListaAeronaves* listaAeronaves;
	GestorAsignaciones* gestorAsignaciones;
	Font fuente;
	bool fuenteCargada;
	string identificacion;
	string nombre;
	string puesto;
	string estado;
	string matriculaAeronave;
	string identificacionSeleccionada;
	string mensaje;
	int campoActivo;
	bool editando;
	bool confirmarEliminar;
	bool cursorVisible;
	float tiempoCursor;
	bool desplegableMatriculas;
	int desplazamientoMatriculas;
	
public:
	PantallaTripulantes(ListaTripulantes* listaTripulantes, ListaAeronaves* listaAeronaves, GestorAsignaciones* gestorAsignaciones);
	void procesarEventos(RenderWindow& ventana, Event evento) override;
	void actualizar(float tiempo) override;
	void dibujar(RenderWindow& ventana) override;	
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
