#ifndef LISTAMALETAS_H
#define LISTAMALETAS_H
#include "Maleta.h"

struct NodoMaleta
{
	Maleta maleta;
	NodoMaleta* siguiente;
	
	NodoMaleta(Maleta maleta)
	{
		this->maleta = maleta;
		siguiente = nullptr;
	}
};

class ListaMaletas
{
private:
	NodoMaleta* primero;
	int cantidad;
public:
	ListaMaletas();
	~ListaMaletas();
	bool agregar(Maleta maleta);
	NodoMaleta* buscar(string codigo) const;
	bool modificar(string codigo, string identificacionPasajero, float peso, string destino, string estado);
	bool eliminar(string codigo);
	NodoMaleta* getPrimero() const;
	int getCantidad() const;
	void limpiar();
};

#endif
