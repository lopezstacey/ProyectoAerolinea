
#ifndef Lista_H
#define Lista_H

#include "Ciudad.h"

struct NodoCiudad
{
	Ciudad dato;
	NodoCiudad* next;
};

class Lista
{
private:
	NodoCiudad* primero;
	int cant;
	
public:
	Lista();
	bool estaVacia() const;
	int getCantidad() const;
	void insertar(Ciudad ciudad);
	void mostrar();	
	~Lista();
};

#endif
