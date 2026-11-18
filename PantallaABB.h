#ifndef PANTALLAABB_H
#define PANTALLAABB_H
#include "Pantalla.h"
#include "Grafo.h"
#include "ArbolABB.h"
#include <SFML/Graphics.hpp>
#include <string>

using namespace sf;
using namespace std;

class PantallaABB : public Pantalla
{
private:
	enum Modo { MODO_NINGUNO, MODO_BUSQUEDA, MODO_RECORRIDO };
	Grafo* grafo;
	ArbolABB arbolVacio;	
	ArbolABB* arbol;	
	Font fuente;
	bool fuenteCargada;
	string textoBusqueda;
	string mensaje;
	int estadoMensaje;
	NodoABB** secuencia;
	int cantidadSecuencia;
	int visibles;
	float acumulado;
	float reloj;
	int modo;
	int tipoRecorrido;
	bool finalizado;
	NodoABB* encontrado;
	string tituloSecuencia;
	float separacionX;
	float separacionY;
	float offsetX;
	void calcularPosiciones();
	void asignarPosiciones(NodoABB* nodo, int profundidad, int& contador);
	void iniciarBusqueda();
	void iniciarRecorrido(int tipo);
	int indiceVisible(NodoABB* nodo) const;
	void dibujarTexto(RenderWindow& ventana, float x, float y, const string& texto, unsigned int tamano, Color color, bool negrita = false);
	void dibujarTextoCentrado(RenderWindow& ventana, float cx, float cy, const string& texto, unsigned int tamano, Color color, bool negrita = false);
	void dibujarBoton(RenderWindow& ventana, float x, float y, float ancho, float alto, const string& texto, bool activo);
	void dibujarControles(RenderWindow& ventana);
	void dibujarAristas(RenderWindow& ventana, NodoABB* nodo);
	void dibujarNodos(RenderWindow& ventana, NodoABB* nodo);
	void dibujarArbol(RenderWindow& ventana);
	void dibujarSecuencia(RenderWindow& ventana);
	
public:
	PantallaABB(Grafo* grafo);
	~PantallaABB();
	void procesarEventos(RenderWindow& ventana, Event evento);
	void actualizar(float tiempo);
	void dibujar(RenderWindow& ventana);
};

#endif
