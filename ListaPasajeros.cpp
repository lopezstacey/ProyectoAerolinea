#include "ListaPasajeros.h"

ListaPasajeros::ListaPasajeros()
{
	primero = nullptr;
	cantidad = 0;
}

ListaPasajeros::~ListaPasajeros()
{
	limpiar();
}

bool ListaPasajeros::agregar(Pasajero pasajero)
{
	if (buscar(pasajero.getIdentificacion()) != nullptr)
		return false;
	
	NodoPasajero* nuevo = new NodoPasajero(pasajero);
	
	if (primero == nullptr)
	{
		primero = nuevo;
	}
	else
	{
		NodoPasajero* actual = primero;
		
		while (actual->siguiente != nullptr)
			actual = actual->siguiente;
		
		actual->siguiente = nuevo;
	}
	
	cantidad++;
	return true;
}

NodoPasajero* ListaPasajeros::buscar(string identificacion) const
{
	NodoPasajero* actual = primero;
	
	while (actual != nullptr)
	{
		if (actual->pasajero.getIdentificacion() == identificacion)
			return actual;
		
		actual = actual->siguiente;
	}
	
	return nullptr;
}

bool ListaPasajeros::modificar(string identificacion, string nombre, string telefono, string correo)
{
	NodoPasajero* encontrado = buscar(identificacion);
	
	if (encontrado == nullptr)
		return false;
	
	encontrado->pasajero.setNombre(nombre);
	encontrado->pasajero.setTelefono(telefono);
	encontrado->pasajero.setCorreo(correo);
	
	return true;
}

bool ListaPasajeros::eliminar(string identificacion)
{
	if (primero == nullptr)
		return false;
	
	if (primero->pasajero.getIdentificacion() == identificacion)
	{
		NodoPasajero* eliminado = primero;
		primero = primero->siguiente;
		delete eliminado;
		cantidad--;
		return true;
	}
	
	NodoPasajero* actual = primero;
	
	while (actual->siguiente != nullptr)
	{
		if (actual->siguiente->pasajero.getIdentificacion() == identificacion)
		{
			NodoPasajero* eliminado = actual->siguiente;
			actual->siguiente = eliminado->siguiente;
			delete eliminado;
			cantidad--;
			return true;
		}
		
		actual = actual->siguiente;
	}
	
	return false;
}

NodoPasajero* ListaPasajeros::getPrimero() const
{
	return primero;
}

int ListaPasajeros::getCantidad() const
{
	return cantidad;
}

void ListaPasajeros::limpiar()
{
	while (primero != nullptr)
	{
		NodoPasajero* eliminado = primero;
		primero = primero->siguiente;
		delete eliminado;
	}
	
	cantidad = 0;
}
