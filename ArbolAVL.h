#ifndef ARBOLAVL_H
#define ARBOLAVL_H
#include "Pasajero.h"
#include <string>
using namespace std;

struct NodoVueloAVL
{
	string codigo;
	NodoVueloAVL* siguiente;

	NodoVueloAVL(const string& codigo)
		: codigo(codigo), siguiente(nullptr)
	{
	}
};

struct NodoAVL
{
	Pasajero pasajero;
	string codigoVuelo;
	NodoVueloAVL* vuelos;
	NodoAVL* izquierdo;
	NodoAVL* derecho;
	int altura;
	float x;
	float y;
	
	NodoAVL(Pasajero pasajero, string codigoVuelo = "")
	{
		this->pasajero = pasajero;
		this->codigoVuelo = codigoVuelo;
		vuelos = codigoVuelo.empty() ? nullptr : new NodoVueloAVL(codigoVuelo);
		izquierdo = nullptr;
		derecho = nullptr;
		altura = 1;
		x = 0;
		y = 0;
	}
};

class ArbolAVL
{
private:
	NodoAVL* raiz;
	int obtenerAltura(NodoAVL* nodo) const;
	int obtenerBalance(NodoAVL* nodo) const;
	NodoAVL* rotacionDerecha(NodoAVL* nodo);
	NodoAVL* rotacionIzquierda(NodoAVL* nodo);
	NodoAVL* insertar(NodoAVL* nodo, Pasajero pasajero, string codigoVuelo);
	NodoAVL* buscar(NodoAVL* nodo, string identificacion) const;
	bool agregarVuelo(NodoAVL* nodo, const string& codigoVuelo);
	void recorrerInorden(NodoAVL* nodo, NodoAVL** secuencia, int& cantidad) const;
	void recorrerPreorden(NodoAVL* nodo, NodoAVL** secuencia, int& cantidad) const;
	void recorrerPostorden(NodoAVL* nodo, NodoAVL** secuencia, int& cantidad) const;
	int contar(NodoAVL* nodo) const;
	void destruir(NodoAVL* nodo);
	void destruirVuelos(NodoVueloAVL* vuelo);
public:
	ArbolAVL();
	void insertar(Pasajero pasajero);
	void insertar(Pasajero pasajero, string codigoVuelo);
	bool asignarVuelo(string identificacion, string codigoVuelo);
	bool tieneVuelo(string identificacion, string codigoVuelo) const;
	string buscarVuelo(string identificacion) const;
	Pasajero* buscar(string identificacion) const;
	int recorrerInorden(NodoAVL** secuencia) const;
	int recorrerPreorden(NodoAVL** secuencia) const;
	int recorrerPostorden(NodoAVL** secuencia) const;
	NodoAVL* getRaiz() const;
	int getCantidad() const;
	void limpiar();
	~ArbolAVL();
};

#endif
