#include "ArbolABB.h"
#include <iostream>

using namespace std;

ArbolABB::ArbolABB()
{
	raiz = nullptr;
}

ArbolABB::~ArbolABB()
{
	limpiar();
}

NodoABB* ArbolABB::insertar(NodoABB* actual, Ciudad ciudad)
{
	if (actual == nullptr)
		return new NodoABB(ciudad);
	
	if (ciudad.getCodigo() < actual->ciudad.getCodigo())
		actual->izquierdo = insertar(actual->izquierdo, ciudad);
	else if (ciudad.getCodigo() > actual->ciudad.getCodigo())
		actual->derecho = insertar(actual->derecho, ciudad);
	
	return actual;
}

void ArbolABB::insertar(Ciudad ciudad)
{
	raiz = insertar(raiz, ciudad);
}

NodoABB* ArbolABB::buscar(NodoABB* actual, string codigo) const
{
	if (actual == nullptr || actual->ciudad.getCodigo() == codigo)
		return actual;
	
	if (codigo < actual->ciudad.getCodigo())
		return buscar(actual->izquierdo, codigo);
	
	return buscar(actual->derecho, codigo);
}

Ciudad* ArbolABB::buscar(string codigo) const
{
	NodoABB* encontrado = buscar(raiz, codigo);
	
	if (encontrado == nullptr)
		return nullptr;
	
	return &encontrado->ciudad;
}

void ArbolABB::recorridoInorden(NodoABB* actual) const
{
	if (actual != nullptr)
	{
		recorridoInorden(actual->izquierdo);
		cout << actual->ciudad.getCodigo() << " - " << actual->ciudad.getNombre() << endl;
		recorridoInorden(actual->derecho);
	}
}

void ArbolABB::recorridoPreorden(NodoABB* actual) const
{
	if (actual != nullptr)
	{
		cout << actual->ciudad.getCodigo() << " - " << actual->ciudad.getNombre() << endl;
		recorridoPreorden(actual->izquierdo);
		recorridoPreorden(actual->derecho);
	}
}

void ArbolABB::recorridoPostorden(NodoABB* actual) const
{
	if (actual != nullptr)
	{
		recorridoPostorden(actual->izquierdo);
		recorridoPostorden(actual->derecho);
		cout << actual->ciudad.getCodigo() << " - " << actual->ciudad.getNombre() << endl;
	}
}

void ArbolABB::inorden() const
{
	recorridoInorden(raiz);
}

void ArbolABB::preorden() const
{
	recorridoPreorden(raiz);
}

void ArbolABB::postorden() const
{
	recorridoPostorden(raiz);
}

NodoABB* ArbolABB::getRaiz() const
{
	return raiz;
}

void ArbolABB::destruir(NodoABB* actual)
{
	if (actual != nullptr)
	{
		destruir(actual->izquierdo);
		destruir(actual->derecho);
		delete actual;
	}
}

void ArbolABB::limpiar()
{
	destruir(raiz);
	raiz = nullptr;
}
