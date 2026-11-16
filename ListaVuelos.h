#ifndef LISTAVUELOS_H
#define LISTAVUELOS_H
#include "Vuelo.h"

struct NodoVuelo
{
	Vuelo vuelo;
	NodoVuelo* siguiente;
	
	NodoVuelo(Vuelo vuelo)
	{
		this->vuelo = vuelo;
		siguiente = nullptr;
	}
};

class ListaVuelos
{
private:
	NodoVuelo* primero;
	int cantidad;
public:
	ListaVuelos();
	~ListaVuelos();
	bool agregar(Vuelo vuelo);
	NodoVuelo* buscar(string codigo) const;
	bool modificar(string codigo, string codigoOrigen, string codigoDestino, float costo, float duracion);
	bool eliminar(string codigo);
	NodoVuelo* getPrimero() const;
	int getCantidad() const;
	void limpiar();
};

#endif
