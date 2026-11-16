#include "ListaTripulantes.h"

ListaTripulantes::ListaTripulantes()
{
	primero = nullptr;
	cantidad = 0;
}

ListaTripulantes::~ListaTripulantes()
{
	limpiar();
}

bool ListaTripulantes::agregar(Tripulante tripulante)
{
	if (buscar(tripulante.getIdentificacion()) != nullptr)
		return false;
	
	NodoTripulante* nuevo = new NodoTripulante(tripulante);
	
	if (primero == nullptr)
	{
		primero = nuevo;
	}
	else
	{
		NodoTripulante* actual = primero;
		
		while (actual->siguiente != nullptr)
		{
			actual = actual->siguiente;
		}
		
		actual->siguiente = nuevo;
	}
	
	cantidad++;
	return true;
}

NodoTripulante* ListaTripulantes::buscar(string identificacion) const
{
	NodoTripulante* actual = primero;
	
	while (actual != nullptr)
	{
		if (actual->tripulante.getIdentificacion() == identificacion)
			return actual;
		
		actual = actual->siguiente;
	}
	
	return nullptr;
}

bool ListaTripulantes::modificar(string identificacion, string nombre, string puesto, string estado)
{
	NodoTripulante* actual = buscar(identificacion);
	
	if (actual == nullptr)
		return false;
	
	actual->tripulante.setNombre(nombre);
	actual->tripulante.setPuesto(puesto);
	actual->tripulante.setEstado(estado);
	
	return true;
}

bool ListaTripulantes::eliminar(string identificacion)
{
	if (primero == nullptr)
		return false;
	
	if (primero->tripulante.getIdentificacion() == identificacion)
	{
		NodoTripulante* eliminar = primero;
		primero = primero->siguiente;
		delete eliminar;
		cantidad--;
		return true;
	}
	
	NodoTripulante* actual = primero;
	
	while (actual->siguiente != nullptr)
	{
		if (actual->siguiente->tripulante.getIdentificacion() == identificacion)
		{
			NodoTripulante* eliminar = actual->siguiente;
			actual->siguiente = eliminar->siguiente;
			delete eliminar;
			cantidad--;
			return true;
		}
		
		actual = actual->siguiente;
	}
	
	return false;
}

NodoTripulante* ListaTripulantes::getPrimero() const
{
	return primero;
}

int ListaTripulantes::getCantidad() const
{
	return cantidad;
}

void ListaTripulantes::limpiar()
{
	while (primero != nullptr)
	{
		NodoTripulante* eliminar = primero;
		primero = primero->siguiente;
		delete eliminar;
	}
	
	cantidad = 0;
}
