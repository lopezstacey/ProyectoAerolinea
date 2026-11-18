#ifndef PANTALLAMALETAS_H
#define PANTALLAMALETAS_H
#include "Pantalla.h"
#include "ListaMaletas.h"
#include "ListaPasajeros.h"
#include "Grafo.h"
#include <SFML/Graphics.hpp>
#include <string>
#include <functional> 
using namespace sf;
using namespace std;

class PantallaMaletas : public Pantalla
{
private:
	ListaMaletas* listaMaletas;
	ListaPasajeros* listaPasajeros;
	Grafo* grafo;
	Font fuente;
	bool fuenteCargada;
	string codigo, identificacionPasajero, peso, destino, estado;
	string codigoSeleccionado, mensaje;
	int campoActivo, indiceInicioDestinos, desplazamientoLista;
	bool editando, identificacionBloqueada, confirmarEliminar;
	bool cursorVisible, listaDestinosAbierta;
	float tiempoCursor;
	function<void(string)> alRegistrar;
public:
	PantallaMaletas(ListaMaletas* listaMaletas, string identificacionInicial = "");
	PantallaMaletas(ListaMaletas* listaMaletas, Grafo* grafo, string identificacionInicial = "");
	void setListaPasajeros(ListaPasajeros* lista);
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
	void dibujarListaDestinos(RenderWindow& ventana);
	void seleccionarDestino(int indice);
	void dibujarTexto(RenderWindow& ventana, float x, float y, string texto, int tamano, Color color);
	void prepararReserva(string codigoSugerido, string destinoSugerido, function<void(string)> alRegistrar);
};
#endif
