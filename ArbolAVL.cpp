#include "ArbolAVL.h"

ArbolAVL::ArbolAVL()
{
	raiz = nullptr;
}

int ArbolAVL::obtenerAltura(NodoAVL* nodo) const
{
	return nodo == nullptr ? 0 : nodo->altura;
}

int ArbolAVL::obtenerBalance(NodoAVL* nodo) const
{
	return nodo == nullptr ? 0 :
		obtenerAltura(nodo->izquierdo) - obtenerAltura(nodo->derecho);
}

NodoAVL* ArbolAVL::rotacionDerecha(NodoAVL* nodo)
{
	NodoAVL* nuevo = nodo->izquierdo;
	NodoAVL* temporal = nuevo->derecho;
	
	nuevo->derecho = nodo;
	nodo->izquierdo = temporal;
	
	int ai = obtenerAltura(nodo->izquierdo);
	int ad = obtenerAltura(nodo->derecho);
	nodo->altura = 1 + (ai > ad ? ai : ad);
	
	ai = obtenerAltura(nuevo->izquierdo);
	ad = obtenerAltura(nuevo->derecho);
	nuevo->altura = 1 + (ai > ad ? ai : ad);
	
	return nuevo;
}

NodoAVL* ArbolAVL::rotacionIzquierda(NodoAVL* nodo)
{
	NodoAVL* nuevo = nodo->derecho;
	NodoAVL* temporal = nuevo->izquierdo;
	
	nuevo->izquierdo = nodo;
	nodo->derecho = temporal;
	
	int ai = obtenerAltura(nodo->izquierdo);
	int ad = obtenerAltura(nodo->derecho);
	nodo->altura = 1 + (ai > ad ? ai : ad);
	
	ai = obtenerAltura(nuevo->izquierdo);
	ad = obtenerAltura(nuevo->derecho);
	nuevo->altura = 1 + (ai > ad ? ai : ad);
	
	return nuevo;
}

NodoAVL* ArbolAVL::insertar(NodoAVL* nodo, Pasajero pasajero, string codigoVuelo)
{
	if (nodo == nullptr)
		return new NodoAVL(pasajero, codigoVuelo);
	
	if (pasajero.getIdentificacion() < nodo->pasajero.getIdentificacion())
		nodo->izquierdo = insertar(nodo->izquierdo, pasajero, codigoVuelo);
	
	else if (pasajero.getIdentificacion() > nodo->pasajero.getIdentificacion())
		nodo->derecho = insertar(nodo->derecho, pasajero, codigoVuelo);
	
	else
	{
		nodo->pasajero = pasajero;
		
		if (!codigoVuelo.empty())
			nodo->codigoVuelo = codigoVuelo;
		
		return nodo;
	}
	
	int ai = obtenerAltura(nodo->izquierdo);
	int ad = obtenerAltura(nodo->derecho);
	
	nodo->altura = 1 + (ai > ad ? ai : ad);
	
	int balance = obtenerBalance(nodo);
	
	if (balance > 1 &&
		pasajero.getIdentificacion() < nodo->izquierdo->pasajero.getIdentificacion())
		return rotacionDerecha(nodo);
	
	if (balance < -1 &&
		pasajero.getIdentificacion() > nodo->derecho->pasajero.getIdentificacion())
		return rotacionIzquierda(nodo);
	
	if (balance > 1 &&
		pasajero.getIdentificacion() > nodo->izquierdo->pasajero.getIdentificacion())
	{
		nodo->izquierdo = rotacionIzquierda(nodo->izquierdo);
		return rotacionDerecha(nodo);
	}
		
		if (balance < -1 &&
			pasajero.getIdentificacion() < nodo->derecho->pasajero.getIdentificacion())
		{
			nodo->derecho = rotacionDerecha(nodo->derecho);
			return rotacionIzquierda(nodo);
		}
			
			return nodo;
}

void ArbolAVL::insertar(Pasajero pasajero)
{
	raiz = insertar(raiz, pasajero, "");
}

void ArbolAVL::insertar(Pasajero pasajero, string codigoVuelo)
{
	raiz = insertar(raiz, pasajero, codigoVuelo);
}

NodoAVL* ArbolAVL::buscar(NodoAVL* nodo, string identificacion) const
{
	if (nodo == nullptr ||
		nodo->pasajero.getIdentificacion() == identificacion)
		return nodo;
	
	if (identificacion < nodo->pasajero.getIdentificacion())
		return buscar(nodo->izquierdo, identificacion);
	
	return buscar(nodo->derecho, identificacion);
}

Pasajero* ArbolAVL::buscar(string identificacion) const
{
	NodoAVL* encontrado = buscar(raiz, identificacion);
	
	return encontrado == nullptr ? nullptr : &encontrado->pasajero;
}

bool ArbolAVL::asignarVuelo(string identificacion, string codigoVuelo)
{
	NodoAVL* encontrado = buscar(raiz, identificacion);
	
	if (encontrado == nullptr || codigoVuelo.empty())
		return false;
	
	encontrado->codigoVuelo = codigoVuelo;
	
	return true;
}

string ArbolAVL::buscarVuelo(string identificacion) const
{
	NodoAVL* encontrado = buscar(raiz, identificacion);
	
	return encontrado == nullptr ? "" : encontrado->codigoVuelo;
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
