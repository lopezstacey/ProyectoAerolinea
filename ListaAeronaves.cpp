#include "ListaAeronaves.h"

ListaAeronaves::ListaAeronaves()
{
	primero = nullptr;
	cantidad = 0;
}

ListaAeronaves::~ListaAeronaves()
{
	limpiar();
}

bool ListaAeronaves::agregar(Aeronave aeronave)
{
	if (buscar(aeronave.getMatricula()) != nullptr)
		return false;
	
	NodoAeronave* nuevo = new NodoAeronave(aeronave);
	
	if (primero == nullptr)
	{
		primero = nuevo;
	}
	else
	{
		NodoAeronave* actual = primero;
		
		while (actual->siguiente != nullptr)
			actual = actual->siguiente;
		
		actual->siguiente = nuevo;
	}
	
	cantidad++;
	return true;
}

NodoAeronave* ListaAeronaves::buscar(string matricula) const
{
	NodoAeronave* actual = primero;
	
	while (actual != nullptr)
	{
		if (actual->aeronave.getMatricula() == matricula)
			return actual;
		
		actual = actual->siguiente;
	}
	
	return nullptr;
}

bool ListaAeronaves::modificar(string matricula, string modelo, int capacidad, string estado)
{
	NodoAeronave* actual = buscar(matricula);
	
	if (actual == nullptr)
		return false;
	
	actual->aeronave.setModelo(modelo);
	actual->aeronave.setCapacidad(capacidad);
	actual->aeronave.setEstado(estado);
	
	return true;
}

bool ListaAeronaves::eliminar(string matricula)
{
	if (primero == nullptr)
		return false;
	
	if (primero->aeronave.getMatricula() == matricula)
	{
		NodoAeronave* eliminado = primero;
		primero = primero->siguiente;
		delete eliminado;
		cantidad--;
		return true;
	}
	
	NodoAeronave* actual = primero;
	
	while (actual->siguiente != nullptr)
	{
		if (actual->siguiente->aeronave.getMatricula() == matricula)
		{
			NodoAeronave* eliminado = actual->siguiente;
			actual->siguiente = eliminado->siguiente;
			delete eliminado;
			cantidad--;
			return true;
		}
		
		actual = actual->siguiente;
	}
	
	return false;
}

NodoAeronave* ListaAeronaves::getPrimero() const
{
	return primero;
}

int ListaAeronaves::getCantidad() const
{
	return cantidad;
}

void ListaAeronaves::limpiar()
{
	while (primero != nullptr)
	{
		NodoAeronave* eliminado = primero;
		primero = primero->siguiente;
		delete eliminado;
	}
	
	cantidad = 0;
}
