#ifndef PANTALLAAERONAVES_H
#define PANTALLAAERONAVES_H
#include "Pantalla.h"
#include "ListaAeronaves.h"
#include <SFML/Graphics.hpp>
#include <string>
using namespace sf;
using namespace std;

class PantallaAeronaves : public Pantalla
{
private:
	ListaAeronaves* listaAeronaves;
	Font fuente;
	bool fuenteCargada;
	string matricula;
	string modelo;
	string estado;
	string matriculaSeleccionada;
	string mensaje;
	string rutaImagen;
	int capacidad;
	int campoActivo;
	int desplazamientoLista;
	bool editando;
	bool confirmarEliminar;
	bool imagenCargada;
	bool cursorVisible;
	float tiempoCursor;
	Texture texturaImagen;
	Sprite spriteImagen;
public:
	PantallaAeronaves(ListaAeronaves* listaAeronaves);
	void procesarEventos(RenderWindow& ventana, Event evento);
	void actualizar(float tiempo);
	void dibujar(RenderWindow& ventana);
	void registrarAeronave();
	void seleccionarAeronave(string matricula);
	void modificarAeronave();
	void eliminarAeronave();
	void limpiarFormulario();
	void cargarImagen();
	void dibujarImagen(RenderWindow& ventana);
	void dibujarConfirmacion(RenderWindow& ventana);
	void dibujarFormulario(RenderWindow& ventana);
	void dibujarListado(RenderWindow& ventana);
	void dibujarTexto(RenderWindow& ventana, float x, float y, string texto, int tamano, Color color);
};

#endif
