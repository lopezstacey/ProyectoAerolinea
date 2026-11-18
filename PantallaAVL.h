#ifndef PANTALLAAVL_H
#define PANTALLAAVL_H
#include "Pantalla.h"
#include "ArbolAVL.h"
#include "ListaPasajeros.h"
#include "ListaReservas.h"
#include <SFML/Graphics.hpp>
#include <string>
#include <vector>

using namespace sf;
using namespace std;

class PantallaAVL : public Pantalla
{
private:
	enum Modo { MODO_NINGUNO, MODO_BUSQUEDA, MODO_RECORRIDO };
	ArbolAVL* arbol;
	ListaPasajeros* listaPasajeros;
	ListaReservas* listaReservas;
	Font fuente;
	bool fuenteCargada;
	string identificacion;
	string codigoVuelo;
	string mensaje;
	string tituloSecuencia;
	vector<NodoAVL*> secuencia;
	int campoActivo;
	int modo;
	int tipoRecorrido;
	int visibles;
	int estadoMensaje;
	float acumulado;
	float reloj;
	float separacionX;
	float separacionY;
	bool finalizado;
	NodoAVL* encontrado;
	void reconstruirArbol();
	void calcularPosiciones();
	void asignarPosiciones(NodoAVL* nodo, int profundidad, int& contador);
	void iniciarBusqueda();
	void iniciarRecorrido(int tipo);
	void dibujarTexto(RenderWindow& ventana, float x, float y, const string& texto, unsigned int tamano, Color color, bool negrita = false);
	void dibujarTextoCentrado(RenderWindow& ventana, float cx, float cy, const string& texto, unsigned int tamano, Color color, bool negrita = false);
	void dibujarBoton(RenderWindow& ventana, float x, float y, float ancho, float alto, const string& texto, bool activo);
	void dibujarControles(RenderWindow& ventana);
	void dibujarAristas(RenderWindow& ventana, NodoAVL* nodo);
	void dibujarNodos(RenderWindow& ventana, NodoAVL* nodo);
	void dibujarArbol(RenderWindow& ventana);
	void dibujarSecuencia(RenderWindow& ventana);
	int indiceVisible(NodoAVL* nodo) const;
	
public:
	PantallaAVL(ArbolAVL* arbol, ListaPasajeros* listaPasajeros, ListaReservas* listaReservas);
	void procesarEventos(RenderWindow& ventana, Event evento);
	void actualizar(float tiempo);
	void dibujar(RenderWindow& ventana);
};

#endif
