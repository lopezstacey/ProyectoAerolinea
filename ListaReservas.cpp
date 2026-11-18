#include "ListaReservas.h"

ListaReservas::ListaReservas()
{
	primero = nullptr;
	cantidad = 0;
}

ListaReservas::~ListaReservas()
{
	while (primero != nullptr)
	{
		NodoReserva* eliminar = primero;
		primero = primero->siguiente;
		delete eliminar;
	}
}

bool ListaReservas::contiene(const string& identificacion, const string& vuelo) const
{
	for (NodoReserva* actual = primero; actual != nullptr; actual = actual->siguiente)
		if (actual->identificacionPasajero == identificacion && actual->codigoVuelo == vuelo)
			return true;
	
	return false;
}

bool ListaReservas::agregar(const string& identificacion, const string& vuelo)
{
	if (identificacion.empty() || vuelo.empty() || contiene(identificacion, vuelo))
		return false;
	
	NodoReserva* nueva = new NodoReserva(identificacion, vuelo);
	
	if (primero == nullptr)
	{
		primero = nueva;
	}
	else
	{
		NodoReserva* actual = primero;
		
		while (actual->siguiente != nullptr)
			actual = actual->siguiente;
		
		actual->siguiente = nueva;
	}
	
	cantidad++;
	return true;
}

int ListaReservas::eliminarPorPasajero(const string& identificacion)
{
	int eliminadas = 0;
	NodoReserva* anterior = nullptr;
	NodoReserva* actual = primero;
	
	while (actual != nullptr)
	{
		NodoReserva* siguiente = actual->siguiente;
		
		if (actual->identificacionPasajero == identificacion)
		{
			if (anterior == nullptr)
				primero = siguiente;
			else
				anterior->siguiente = siguiente;
			
			delete actual;
			cantidad--;
			eliminadas++;
		}
		else
		{
			anterior = actual;
		}
		
		actual = siguiente;
	}
	
	return eliminadas;
}

int ListaReservas::eliminarPorVuelo(const string& codigoVuelo)
{
	int eliminadas = 0;
	NodoReserva* anterior = nullptr;
	NodoReserva* actual = primero;

	while (actual != nullptr)
	{
		NodoReserva* siguiente = actual->siguiente;
		if (actual->codigoVuelo == codigoVuelo)
		{
			if (anterior == nullptr)
				primero = siguiente;
			else
				anterior->siguiente = siguiente;

			delete actual;
			cantidad--;
			eliminadas++;
		}
		else
			anterior = actual;
		actual = siguiente;
	}

	return eliminadas;
}

NodoReserva* ListaReservas::getPrimero() const
{
	return primero;
}

int ListaReservas::getCantidad() const
{
	return cantidad;
}
