#include "ArbolABB.h"
#include <cctype>

ArbolABB::ArbolABB()
{
	raiz = nullptr;
	cantidad = 0;
}

ArbolABB::~ArbolABB()
{
	limpiar();
}

string ArbolABB::minusculas(const string& texto)
{
	string resultado = texto;
	
	for (size_t i = 0; i < resultado.size(); i++)
		resultado[i] = static_cast<char>(tolower(static_cast<unsigned char>(resultado[i])));
	
	return resultado;
}

int ArbolABB::comparar(const string& a, const string& b)
{
	string x = minusculas(a);
	string y = minusculas(b);
	
	if (x < y)
		return -1;
	if (x > y)
		return 1;
	return 0;
}

NodoABB* ArbolABB::insertar(NodoABB* actual, NodoABB* nuevo, bool& insertado)
{
	if (actual == nullptr)
	{
		insertado = true;
		return nuevo;
	}
	
	int resultado = comparar(nuevo->nombre, actual->nombre);
	
	if (resultado < 0)
		actual->izquierdo = insertar(actual->izquierdo, nuevo, insertado);
	else if (resultado > 0)
		actual->derecho = insertar(actual->derecho, nuevo, insertado);
	
	return actual;
}

bool ArbolABB::insertar(string nombre, string codigo)
{
	if (nombre.empty())
		return false;
	
	NodoABB* nuevo = new NodoABB(nombre, codigo);
	bool insertado = false;
	
	raiz = insertar(raiz, nuevo, insertado);
	
	if (!insertado)
	{
		delete nuevo;
		return false;
	}
	
	cantidad++;
	return true;
}

NodoABB* ArbolABB::buscar(const string& nombre, NodoABB** camino, int& pasos) const
{
	pasos = 0;
	NodoABB* actual = raiz;
	
	while (actual != nullptr)
	{
		if (camino != nullptr)
			camino[pasos] = actual;
		pasos++;
		
		int resultado = comparar(nombre, actual->nombre);
		
		if (resultado == 0)
			return actual;
		
		actual = resultado < 0 ? actual->izquierdo : actual->derecho;
	}
	
	return nullptr;
}

bool ArbolABB::contiene(const string& nombre) const
{
	int pasos = 0;
	return buscar(nombre, nullptr, pasos) != nullptr;
}

int ArbolABB::getCantidad() const
{
	return cantidad;
}

int ArbolABB::altura(NodoABB* nodo) const
{
	if (nodo == nullptr)
		return 0;
	
	int izq = altura(nodo->izquierdo);
	int der = altura(nodo->derecho);
	
	return 1 + (izq > der ? izq : der);
}

int ArbolABB::getAltura() const
{
	return altura(raiz);
}

NodoABB* ArbolABB::getRaiz() const
{
	return raiz;
}

void ArbolABB::inorden(NodoABB* nodo, NodoABB** destino, int& n) const
{
	if (nodo == nullptr)
		return;
	
	inorden(nodo->izquierdo, destino, n);
	destino[n++] = nodo;
	inorden(nodo->derecho, destino, n);
}

void ArbolABB::preorden(NodoABB* nodo, NodoABB** destino, int& n) const
{
	if (nodo == nullptr)
		return;
	
	destino[n++] = nodo;
	preorden(nodo->izquierdo, destino, n);
	preorden(nodo->derecho, destino, n);
}

void ArbolABB::postorden(NodoABB* nodo, NodoABB** destino, int& n) const
{
	if (nodo == nullptr)
		return;
	
	postorden(nodo->izquierdo, destino, n);
	postorden(nodo->derecho, destino, n);
	destino[n++] = nodo;
}

int ArbolABB::recorrerInorden(NodoABB** destino) const
{
	int n = 0;
	inorden(raiz, destino, n);
	return n;
}

int ArbolABB::recorrerPreorden(NodoABB** destino) const
{
	int n = 0;
	preorden(raiz, destino, n);
	return n;
}

int ArbolABB::recorrerPostorden(NodoABB** destino) const
{
	int n = 0;
	postorden(raiz, destino, n);
	return n;
}

void ArbolABB::liberar(NodoABB* nodo)
{
	if (nodo == nullptr)
		return;
	
	liberar(nodo->izquierdo);
	liberar(nodo->derecho);
	delete nodo;
}

void ArbolABB::limpiar()
{
	liberar(raiz);
	raiz = nullptr;
	cantidad = 0;
}
