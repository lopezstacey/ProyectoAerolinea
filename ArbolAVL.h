#ifndef ARBOLAVL_H
#define ARBOLAVL_H
#include "Pasajero.h"

struct NodoAVL
{
	Pasajero pasajero;
	NodoAVL* izquierdo;
	NodoAVL* derecho;
	int altura;
	NodoAVL(Pasajero pasajero)
	{
		this->pasajero = pasajero;
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
	NodoAVL* insertar(NodoAVL* nodo, Pasajero pasajero);
	NodoAVL* buscar(NodoAVL* nodo, string identificacion) const;
	void destruir(NodoAVL* nodo);
public:
	ArbolAVL();
	void insertar(Pasajero pasajero);
	Pasajero* buscar(string identificacion) const;
	NodoAVL* getRaiz() const;
	void limpiar();
	~ArbolAVL();
};

#endif
