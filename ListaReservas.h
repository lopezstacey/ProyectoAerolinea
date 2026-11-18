#ifndef LISTARESERVAS_H
#define LISTARESERVAS_H
#include <string>

using namespace std;

struct NodoReserva
{
	string identificacionPasajero;
	string codigoVuelo;
	NodoReserva* siguiente;
	
	NodoReserva(const string& identificacion, const string& vuelo)
		: identificacionPasajero(identificacion), codigoVuelo(vuelo), siguiente(nullptr)
	{
	}
};

class ListaReservas
{
private:
	NodoReserva* primero;
	int cantidad;
	
public:
	ListaReservas();
	~ListaReservas();
	bool contiene(const string& identificacion, const string& vuelo) const;
	bool agregar(const string& identificacion, const string& vuelo);
	int eliminarPorPasajero(const string& identificacion);
	int eliminarPorVuelo(const string& codigoVuelo);
	NodoReserva* getPrimero() const;
	int getCantidad() const;
};

#endif
