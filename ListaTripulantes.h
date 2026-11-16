#ifndef LISTATRIPULANTES_H
#define LISTATRIPULANTES_H
#include "Tripulante.h"

struct NodoTripulante
{
	Tripulante tripulante;
	NodoTripulante* siguiente;
	
	NodoTripulante(Tripulante tripulante)
	{
		this->tripulante = tripulante;
		siguiente = nullptr;
	}
};

class ListaTripulantes
{
private:
	NodoTripulante* primero;
	int cantidad;
public:
	ListaTripulantes();
	~ListaTripulantes();
	bool agregar(Tripulante tripulante);
	NodoTripulante* buscar(string identificacion) const;
	bool modificar(string identificacion, string nombre, string puesto, string estado);
	bool eliminar(string identificacion);
	NodoTripulante* getPrimero() const;
	int getCantidad() const;
	void limpiar();
};

#endif
