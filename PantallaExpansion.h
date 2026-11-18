#ifndef PANTALLAEXPANSION_H
#define PANTALLAEXPANSION_H
#include "Pantalla.h"
#include "Grafo.h"
#include "ArbolExpansion.h"
#include "Prim.h"
#include "Kruskal.h"
#include <SFML/Graphics.hpp>
#include <string>

using namespace sf;
using namespace std;

class PantallaExpansion : public Pantalla
{
private:
	enum Vista { VISTA_MAPA, VISTA_ARBOL };
	
	Grafo* grafo;
	bool usarPrim;
	Prim prim;
	Kruskal kruskal;
	ResultadoExpansion resultado;
	ArbolExpansion arbol;
	Font fuente;
	bool fuenteCargada;
	Texture texturaMapa;
	Sprite mapa;
	bool mapaCargado;
	int vista;
	string mensaje;
	int pasoVisible;
	float acumulado;
	float reloj;
	int desplazamiento;
	bool* enArbol;
	NodoExpansion** secuencia;
	int cantidadSecuencia;
	int visiblesRecorrido;
	int tipoRecorrido;
	string tituloRecorrido;
	float arbolSepX;
	float arbolSepY;
	float arbolOrigenX;
	
	void recalcular(const string& codigoInicio);
	void liberarMemoria();
	void reiniciarAnimacion();
	void mostrarTodo();
	void iniciarRecorrido(int tipo);
	void limpiarRecorrido();
	void marcarCiudades();
	int indiceEnSecuencia(NodoExpansion* nodo) const;
	void seguirScroll();
	void calcularPosicionesArbol();
	float asignarPosicion(NodoExpansion* nodo, int profundidad, int& hoja);
	Vector2f posicionNodo(NodoExpansion* nodo) const;
	string textoPeso(float costo, float duracion) const;
	string recortarAncho(const string& texto, unsigned int tamano, bool negrita, float maxAncho) const;
	
	void dibujarTexto(RenderWindow& ventana, float x, float y, const string& texto, unsigned int tamano, Color color, bool negrita = false);
	void dibujarTextoCentrado(RenderWindow& ventana, float cx, float cy, const string& texto, unsigned int tamano, Color color, bool negrita = false);
	void dibujarTextoContorno(RenderWindow& ventana, float cx, float cy, const string& texto, unsigned int tamano, Color color);
	void dibujarBoton(RenderWindow& ventana, float x, float y, float ancho, float alto, const string& texto, bool activo);
	void dibujarEncabezado(RenderWindow& ventana);
	void dibujarPasos(RenderWindow& ventana);
	void dibujarVistaMapa(RenderWindow& ventana);
	void dibujarVistaArbol(RenderWindow& ventana);
	void dibujarAristasArbol(RenderWindow& ventana, NodoExpansion* nodo);
	void dibujarNodo(RenderWindow& ventana, NodoExpansion* nodo);
	void dibujarSelectorVista(RenderWindow& ventana);
	void dibujarResumen(RenderWindow& ventana);
	void dibujarRecorridos(RenderWindow& ventana);
	
public:
	PantallaExpansion(Grafo* grafo, bool usarPrim);
	~PantallaExpansion();
	void procesarEventos(RenderWindow& ventana, Event evento);
	void actualizar(float tiempo);
	void dibujar(RenderWindow& ventana);
};

#endif
