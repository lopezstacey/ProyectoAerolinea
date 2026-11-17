#include "ListaVuelos.h"

ListaVuelos::ListaVuelos()
{
	primero = nullptr;
	cantidad = 0;
}

ListaVuelos::~ListaVuelos()
{
	limpiar();
}

bool ListaVuelos::agregar(Vuelo vuelo)
{
	if (buscar(vuelo.getCodigo()) != nullptr)
		return false;
	
	NodoVuelo* nuevo = new NodoVuelo(vuelo);
	
	if (primero == nullptr)
		primero = nuevo;
	else
	{
		NodoVuelo* actual = primero;
		while (actual->siguiente != nullptr)
			actual = actual->siguiente;
		actual->siguiente = nuevo;
	}
	
	cantidad++;
	return true;
}

NodoVuelo* ListaVuelos::buscar(string codigo) const
{
	NodoVuelo* actual = primero;
	while (actual != nullptr)
	{
		if (actual->vuelo.getCodigo() == codigo)
			return actual;
		actual = actual->siguiente;
	}
	return nullptr;
}

bool ListaVuelos::modificar(string codigo, string codigoOrigen, string codigoDestino, float costo, float duracion)
{
	NodoVuelo* actual = buscar(codigo);
	if (actual == nullptr)
		return false;
	
	actual->vuelo.setCodigoOrigen(codigoOrigen);
	actual->vuelo.setCodigoDestino(codigoDestino);
	actual->vuelo.setCosto(costo);
	actual->vuelo.setDuracion(duracion);
	return true;
}

bool ListaVuelos::eliminar(string codigo)
{
	if (primero == nullptr)
		return false;
	
	if (primero->vuelo.getCodigo() == codigo)
	{
		NodoVuelo* eliminado = primero;
		primero = primero->siguiente;
		delete eliminado;
		cantidad--;
		return true;
	}
	
	NodoVuelo* actual = primero;
	while (actual->siguiente != nullptr)
	{
		if (actual->siguiente->vuelo.getCodigo() == codigo)
		{
			NodoVuelo* eliminado = actual->siguiente;
			actual->siguiente = eliminado->siguiente;
			delete eliminado;
			cantidad--;
			return true;
		}
		actual = actual->siguiente;
	}
	return false;
}

int ListaVuelos::eliminarPorCiudad(string codigoCiudad, ListaRutas* rutas)
{
	int eliminados = 0;
	NodoVuelo* anterior = nullptr;
	NodoVuelo* actual = primero;
	
	while (actual != nullptr)
	{
		if (actual->vuelo.getCodigoOrigen() == codigoCiudad ||
			actual->vuelo.getCodigoDestino() == codigoCiudad)
		{
			NodoVuelo* eliminado = actual;
			actual = actual->siguiente;
			
			if (anterior == nullptr)
				primero = actual;
			else
				anterior->siguiente = actual;
			
			if (rutas != nullptr)
				rutas->eliminarPorVuelo(eliminado->vuelo.getCodigo());
			
			delete eliminado;
			cantidad--;
			eliminados++;
		}
		else
		{
			anterior = actual;
			actual = actual->siguiente;
		}
	}
	
	return eliminados;
}

NodoVuelo* ListaVuelos::getPrimero() const
{
	return primero;
}

int ListaVuelos::getCantidad() const
{
	return cantidad;
}

void ListaVuelos::limpiar()
{
	while (primero != nullptr)
	{
		NodoVuelo* eliminado = primero;
		primero = primero->siguiente;
		delete eliminado;
	}
	cantidad = 0;
}
