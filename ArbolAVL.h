#ifndef ARBOLAVL_H
#define ARBOLAVL_H
#include "Pasajero.h"
#include <string>
using namespace std;

struct NodoAVL
{
	Pasajero pasajero;
	string codigoVuelo;
	NodoAVL* izquierdo;
	NodoAVL* derecho;
	int altura;
	
	NodoAVL(Pasajero pasajero, string codigoVuelo = "")
	{
		this->pasajero = pasajero;
		this->codigoVuelo = codigoVuelo;
		izquierdo = nullptr;
		derecho = nullptr;
		altura = 1;
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
	void destruir(NodoAVL* nodo);
public:
	ArbolAVL();
	void insertar(Pasajero pasajero);
	void insertar(Pasajero pasajero, string codigoVuelo);
	bool asignarVuelo(string identificacion, string codigoVuelo);
	string buscarVuelo(string identificacion) const;
	Pasajero* buscar(string identificacion) const;
	NodoAVL* getRaiz() const;
	void limpiar();
	~ArbolAVL();
};

#endif
