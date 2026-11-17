#ifndef LISTARUTAS_H
#define LISTARUTAS_H
#include "Ruta.h"

struct NodoRuta
{
	Ruta ruta;
	NodoRuta* siguiente;
	
	NodoRuta(Ruta ruta)
	{
		this->ruta = ruta;
		siguiente = nullptr;
	}
};

class ListaRutas
{
private:
	NodoRuta* primero;
	int cantidad;	
public:
	ListaRutas();
	~ListaRutas();
	bool agregar(Ruta ruta);
	NodoRuta* buscar(string codigo) const;
	bool modificar(string codigo, string matriculaAeronave, string codigoVuelo);
	bool eliminar(string codigo);
	int eliminarPorVuelo(string codigoVuelo);
	int eliminarPorAeronave(string matriculaAeronave);
	NodoRuta* getPrimero() const;
	int getCantidad() const;
	void limpiar();
};

#endif
