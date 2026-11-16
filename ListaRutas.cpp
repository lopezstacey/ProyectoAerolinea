#include "ListaRutas.h"

ListaRutas::ListaRutas()
{
	primero = nullptr;
	cantidad = 0;
}

ListaRutas::~ListaRutas()
{
	limpiar();
}

bool ListaRutas::agregar(Ruta ruta)
{
	if (buscar(ruta.getCodigo()) != nullptr)
		return false;
	
	NodoRuta* nuevo = new NodoRuta(ruta);
	
	if (primero == nullptr)
		primero = nuevo;
	else
	{
		NodoRuta* actual = primero;
		
		while (actual->siguiente != nullptr)
			actual = actual->siguiente;
		
		actual->siguiente = nuevo;
	}
	
	cantidad++;
	return true;
}

NodoRuta* ListaRutas::buscar(string codigo) const
{
	NodoRuta* actual = primero;
	
	while (actual != nullptr)
	{
		if (actual->ruta.getCodigo() == codigo)
			return actual;
		
		actual = actual->siguiente;
	}
	
	return nullptr;
}

bool ListaRutas::modificar(string codigo, string matriculaAeronave, string codigoVuelo)
{
	NodoRuta* actual = buscar(codigo);
	
	if (actual == nullptr)
		return false;
	
	actual->ruta.setMatriculaAeronave(matriculaAeronave);
	actual->ruta.setCodigoVuelo(codigoVuelo);
	
	return true;
}

bool ListaRutas::eliminar(string codigo)
{
	if (primero == nullptr)
		return false;
	
	if (primero->ruta.getCodigo() == codigo)
	{
		NodoRuta* eliminado = primero;
		primero = primero->siguiente;
		delete eliminado;
		cantidad--;
		return true;
	}
	
	NodoRuta* actual = primero;
	
	while (actual->siguiente != nullptr)
	{
		if (actual->siguiente->ruta.getCodigo() == codigo)
		{
			NodoRuta* eliminado = actual->siguiente;
			actual->siguiente = eliminado->siguiente;
			delete eliminado;
			cantidad--;
			return true;
		}
		
		actual = actual->siguiente;
	}
	
	return false;
}

NodoRuta* ListaRutas::getPrimero() const
{
	return primero;
}

int ListaRutas::getCantidad() const
{
	return cantidad;
}

void ListaRutas::limpiar()
{
	while (primero != nullptr)
	{
		NodoRuta* eliminado = primero;
		primero = primero->siguiente;
		delete eliminado;
	}
	
	cantidad = 0;
}
