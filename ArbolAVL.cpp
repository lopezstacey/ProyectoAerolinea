#include "ArbolAVL.h"

ArbolAVL::ArbolAVL()
{
	raiz = nullptr;
}

int ArbolAVL::getCantidad() const
{
	return contar(raiz);
}

int ArbolAVL::contar(NodoAVL* nodo) const
{
	return nodo == nullptr ? 0 : 1 + contar(nodo->izquierdo) + contar(nodo->derecho);
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
			agregarVuelo(nodo, codigoVuelo);
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

bool ArbolAVL::agregarVuelo(NodoAVL* nodo, const string& codigoVuelo)
{
	if (nodo == nullptr || codigoVuelo.empty())
		return false;

	NodoVueloAVL* actual = nodo->vuelos;
	NodoVueloAVL* anterior = nullptr;
	while (actual != nullptr)
	{
		if (actual->codigo == codigoVuelo)
		{
			nodo->codigoVuelo = codigoVuelo;
			return true;
		}
		anterior = actual;
		actual = actual->siguiente;
	}

	NodoVueloAVL* nuevo = new NodoVueloAVL(codigoVuelo);
	if (anterior == nullptr)
		nodo->vuelos = nuevo;
	else
		anterior->siguiente = nuevo;
	nodo->codigoVuelo = codigoVuelo;
	return true;
}

bool ArbolAVL::asignarVuelo(string identificacion, string codigoVuelo)
{
	NodoAVL* encontrado = buscar(raiz, identificacion);
	
	if (encontrado == nullptr || codigoVuelo.empty())
		return false;
	
	return agregarVuelo(encontrado, codigoVuelo);
}

bool ArbolAVL::tieneVuelo(string identificacion, string codigoVuelo) const
{
	if (codigoVuelo.empty())
		return false;
	
	NodoAVL* encontrado = buscar(raiz, identificacion);
	for (NodoVueloAVL* vuelo = encontrado == nullptr ? nullptr : encontrado->vuelos;
	vuelo != nullptr; vuelo = vuelo->siguiente)
	{
		if (vuelo->codigo == codigoVuelo)
			return true;
	}
	return false;
}

string ArbolAVL::buscarVuelo(string identificacion) const
{
	NodoAVL* encontrado = buscar(raiz, identificacion);
	
	return encontrado == nullptr ? "" : encontrado->codigoVuelo;
}

void ArbolAVL::recorrerInorden(NodoAVL* nodo, NodoAVL** secuencia, int& cantidad) const
{
	if (nodo == nullptr)
		return;
	recorrerInorden(nodo->izquierdo, secuencia, cantidad);
	secuencia[cantidad++] = nodo;
	recorrerInorden(nodo->derecho, secuencia, cantidad);
}

void ArbolAVL::recorrerPreorden(NodoAVL* nodo, NodoAVL** secuencia, int& cantidad) const
{
	if (nodo == nullptr)
		return;
	secuencia[cantidad++] = nodo;
	recorrerPreorden(nodo->izquierdo, secuencia, cantidad);
	recorrerPreorden(nodo->derecho, secuencia, cantidad);
}

void ArbolAVL::recorrerPostorden(NodoAVL* nodo, NodoAVL** secuencia, int& cantidad) const
{
	if (nodo == nullptr)
		return;
	recorrerPostorden(nodo->izquierdo, secuencia, cantidad);
	recorrerPostorden(nodo->derecho, secuencia, cantidad);
	secuencia[cantidad++] = nodo;
}

int ArbolAVL::recorrerInorden(NodoAVL** secuencia) const
{
	int cantidad = 0;
	if (secuencia != nullptr)
		recorrerInorden(raiz, secuencia, cantidad);
	return cantidad;
}

int ArbolAVL::recorrerPreorden(NodoAVL** secuencia) const
{
	int cantidad = 0;
	if (secuencia != nullptr)
		recorrerPreorden(raiz, secuencia, cantidad);
	return cantidad;
}

int ArbolAVL::recorrerPostorden(NodoAVL** secuencia) const
{
	int cantidad = 0;
	if (secuencia != nullptr)
		recorrerPostorden(raiz, secuencia, cantidad);
	return cantidad;
}

NodoAVL* ArbolAVL::getRaiz() const
{
	return raiz;
}

void ArbolAVL::destruirVuelos(NodoVueloAVL* vuelo)
{
	while (vuelo != nullptr)
	{
		NodoVueloAVL* siguiente = vuelo->siguiente;
		delete vuelo;
		vuelo = siguiente;
	}
}

void ArbolAVL::destruir(NodoAVL* nodo)
{
	if (nodo != nullptr)
	{
		destruir(nodo->izquierdo);
		destruir(nodo->derecho);
		destruirVuelos(nodo->vuelos);
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
