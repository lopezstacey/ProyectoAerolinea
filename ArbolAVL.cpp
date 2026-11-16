
#include "ArbolAVL.h"

ArbolAVL::ArbolAVL()
{
	raiz = nullptr;
}

int ArbolAVL::obtenerAltura(NodoAVL* nodo) const
{
	if (nodo == nullptr)
		return 0;
	
	return nodo->altura;
}

int ArbolAVL::obtenerBalance(NodoAVL* nodo) const
{
	if (nodo == nullptr)
		return 0;
	
	return obtenerAltura(nodo->izquierdo) - obtenerAltura(nodo->derecho);
}

NodoAVL* ArbolAVL::rotacionDerecha(NodoAVL* nodo)
{
	NodoAVL* nuevo = nodo->izquierdo;
	NodoAVL* temporal = nuevo->derecho;
	
	nuevo->derecho = nodo;
	nodo->izquierdo = temporal;
	
	int alturaIzquierda = obtenerAltura(nodo->izquierdo);
	int alturaDerecha = obtenerAltura(nodo->derecho);
	nodo->altura = 1 + (alturaIzquierda > alturaDerecha ? alturaIzquierda : alturaDerecha);
	
	alturaIzquierda = obtenerAltura(nuevo->izquierdo);
	alturaDerecha = obtenerAltura(nuevo->derecho);
	nuevo->altura = 1 + (alturaIzquierda > alturaDerecha ? alturaIzquierda : alturaDerecha);
	
	return nuevo;
}

NodoAVL* ArbolAVL::rotacionIzquierda(NodoAVL* nodo)
{
	NodoAVL* nuevo = nodo->derecho;
	NodoAVL* temporal = nuevo->izquierdo;
	
	nuevo->izquierdo = nodo;
	nodo->derecho = temporal;
	
	int alturaIzquierda = obtenerAltura(nodo->izquierdo);
	int alturaDerecha = obtenerAltura(nodo->derecho);
	nodo->altura = 1 + (alturaIzquierda > alturaDerecha ? alturaIzquierda : alturaDerecha);
	
	alturaIzquierda = obtenerAltura(nuevo->izquierdo);
	alturaDerecha = obtenerAltura(nuevo->derecho);
	nuevo->altura = 1 + (alturaIzquierda > alturaDerecha ? alturaIzquierda : alturaDerecha);
	
	return nuevo;
}

NodoAVL* ArbolAVL::insertar(NodoAVL* nodo, Pasajero pasajero)
{
	if (nodo == nullptr)
		return new NodoAVL(pasajero);
	
	if (pasajero.getIdentificacion() < nodo->pasajero.getIdentificacion())
		nodo->izquierdo = insertar(nodo->izquierdo, pasajero);
	else if (pasajero.getIdentificacion() > nodo->pasajero.getIdentificacion())
		nodo->derecho = insertar(nodo->derecho, pasajero);
	else
		return nodo;
	
	int alturaIzquierda = obtenerAltura(nodo->izquierdo);
	int alturaDerecha = obtenerAltura(nodo->derecho);
	nodo->altura = 1 + (alturaIzquierda > alturaDerecha ? alturaIzquierda : alturaDerecha);
	
	int balance = obtenerBalance(nodo);
	
	if (balance > 1 && pasajero.getIdentificacion() < nodo->izquierdo->pasajero.getIdentificacion())
		return rotacionDerecha(nodo);
	
	if (balance < -1 && pasajero.getIdentificacion() > nodo->derecho->pasajero.getIdentificacion())
		return rotacionIzquierda(nodo);
	
	if (balance > 1 && pasajero.getIdentificacion() > nodo->izquierdo->pasajero.getIdentificacion())
	{
		nodo->izquierdo = rotacionIzquierda(nodo->izquierdo);
		return rotacionDerecha(nodo);
	}
	
	if (balance < -1 && pasajero.getIdentificacion() < nodo->derecho->pasajero.getIdentificacion())
	{
		nodo->derecho = rotacionDerecha(nodo->derecho);
		return rotacionIzquierda(nodo);
	}
	
	return nodo;
}

void ArbolAVL::insertar(Pasajero pasajero)
{
	raiz = insertar(raiz, pasajero);
}

NodoAVL* ArbolAVL::buscar(NodoAVL* nodo, string identificacion) const
{
	if (nodo == nullptr || nodo->pasajero.getIdentificacion() == identificacion)
		return nodo;
	
	if (identificacion < nodo->pasajero.getIdentificacion())
		return buscar(nodo->izquierdo, identificacion);
	
	return buscar(nodo->derecho, identificacion);
}

Pasajero* ArbolAVL::buscar(string identificacion) const
{
	NodoAVL* encontrado = buscar(raiz, identificacion);
	
	if (encontrado == nullptr)
		return nullptr;
	
	return &encontrado->pasajero;
}

NodoAVL* ArbolAVL::getRaiz() const
{
	return raiz;
}

void ArbolAVL::destruir(NodoAVL* nodo)
{
	if (nodo != nullptr)
	{
		destruir(nodo->izquierdo);
		destruir(nodo->derecho);
		delete nodo;
	}
}

void ArbolAVL::limpiar()
{
	destruir(raiz);
	raiz = nullptr;
}


ArbolAVL::~ArbolAVL()
{
	limpiar();
}
