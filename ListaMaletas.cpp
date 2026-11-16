#include "ListaMaletas.h"

ListaMaletas::ListaMaletas()
{
	primero = nullptr;
	cantidad = 0;
}

ListaMaletas::~ListaMaletas()
{
	limpiar();
}

bool ListaMaletas::agregar(Maleta maleta)
{
	if (buscar(maleta.getCodigo()) != nullptr)
		return false;
	
	NodoMaleta* nuevo = new NodoMaleta(maleta);
	
	if (primero == nullptr)
	{
		primero = nuevo;
	}
	else
	{
		NodoMaleta* actual = primero;
		
		while (actual->siguiente != nullptr)
			actual = actual->siguiente;
		
		actual->siguiente = nuevo;
	}
	
	cantidad++;
	return true;
}

NodoMaleta* ListaMaletas::buscar(string codigo) const
{
	NodoMaleta* actual = primero;
	
	while (actual != nullptr)
	{
		if (actual->maleta.getCodigo() == codigo)
			return actual;
		
		actual = actual->siguiente;
	}
	
	return nullptr;
}

bool ListaMaletas::modificar(string codigo, string identificacionPasajero, float peso, string destino, string estado)
{
	NodoMaleta* encontrado = buscar(codigo);
	
	if (encontrado == nullptr)
		return false;
	
	encontrado->maleta.setIdentificacionPasajero(identificacionPasajero);
	encontrado->maleta.setPeso(peso);
	encontrado->maleta.setDestino(destino);
	encontrado->maleta.setEstado(estado);
	
	return true;
}

bool ListaMaletas::eliminar(string codigo)
{
	if (primero == nullptr)
		return false;
	
	if (primero->maleta.getCodigo() == codigo)
	{
		NodoMaleta* eliminado = primero;
		primero = primero->siguiente;
		delete eliminado;
		cantidad--;
		return true;
	}
	
	NodoMaleta* actual = primero;
	
	while (actual->siguiente != nullptr)
	{
		if (actual->siguiente->maleta.getCodigo() == codigo)
		{
			NodoMaleta* eliminado = actual->siguiente;
			actual->siguiente = eliminado->siguiente;
			delete eliminado;
			cantidad--;
			return true;
		}
		
		actual = actual->siguiente;
	}
	
	return false;
}

NodoMaleta* ListaMaletas::getPrimero() const
{
	return primero;
}

int ListaMaletas::getCantidad() const
{
	return cantidad;
}

void ListaMaletas::limpiar()
{
	while (primero != nullptr)
	{
		NodoMaleta* eliminado = primero;
		primero = primero->siguiente;
		delete eliminado;
	}
	
	cantidad = 0;
}
