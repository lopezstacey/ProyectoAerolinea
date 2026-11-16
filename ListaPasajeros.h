#ifndef LISTAPASAJEROS_H
#define LISTAPASAJEROS_H
#include "Pasajero.h"

struct NodoPasajero
{
	Pasajero pasajero;
	NodoPasajero* siguiente;
	
	NodoPasajero(Pasajero pasajero)
	{
		this->pasajero = pasajero;
		siguiente = nullptr;
	}
};

class ListaPasajeros
{
private:
	NodoPasajero* primero;
	int cantidad;
public:
	ListaPasajeros();
	~ListaPasajeros();
	bool agregar(Pasajero pasajero);
	NodoPasajero* buscar(string identificacion) const;
	bool modificar(string identificacion, string nombre, string telefono, string correo);
	bool eliminar(string identificacion);
	NodoPasajero* getPrimero() const;
	int getCantidad() const;
	void limpiar();
};

#endif
