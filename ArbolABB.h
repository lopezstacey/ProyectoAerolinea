#ifndef ARBOLABB_H
#define ARBOLABB_H
#include "Ciudad.h"

struct NodoABB
{
	Ciudad ciudad;
	NodoABB* izquierdo;
	NodoABB* derecho;
	NodoABB(Ciudad ciudad)
	{
		this->ciudad = ciudad;
		izquierdo = nullptr;
		derecho = nullptr;
	}
};

class ArbolABB
{
private:
	NodoABB* raiz;
	NodoABB* insertar(NodoABB* actual, Ciudad ciudad);
	NodoABB* buscar(NodoABB* actual, string codigo) const;
	void recorridoInorden(NodoABB* actual) const;
	void recorridoPreorden(NodoABB* actual) const;
	void recorridoPostorden(NodoABB* actual) const;
	void destruir(NodoABB* actual);
public:
	ArbolABB();
	void insertar(Ciudad ciudad);
	Ciudad* buscar(string codigo) const;
	void inorden() const;
	void preorden() const;
	void postorden() const;
	NodoABB* getRaiz() const;
	void limpiar();
	~ArbolABB();
};

#endif
