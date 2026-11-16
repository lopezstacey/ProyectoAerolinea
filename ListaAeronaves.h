#ifndef LISTAAERONAVES_H
#define LISTAAERONAVES_H
#include "Aeronave.h"

struct NodoAeronave
{
	Aeronave aeronave;
	NodoAeronave* siguiente;
	
	NodoAeronave(Aeronave aeronave)
	{
		this->aeronave = aeronave;
		siguiente = nullptr;
	}
};

class ListaAeronaves
{
private:
	NodoAeronave* primero;
	int cantidad;
public:
	ListaAeronaves();
	~ListaAeronaves();
	bool agregar(Aeronave aeronave);
	NodoAeronave* buscar(string matricula) const;
	bool modificar(string matricula, string modelo, int capacidad, string estado);
	bool eliminar(string matricula);
	NodoAeronave* getPrimero() const;
	int getCantidad() const;
	void limpiar();
};

#endif
