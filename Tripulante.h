#ifndef TRIPULANTE_H
#define TRIPULANTE_H

#include <string>

using namespace std;

class Tripulante
{
private:
	string identificacion;
	string nombre;
	string puesto;
	string estado;
public:
	Tripulante();
	Tripulante(string identificacion, string nombre, string puesto, string estado);
	string getIdentificacion() const;
	string getNombre() const;
	string getPuesto() const;
	string getEstado() const;
	void setIdentificacion(string identificacion);
	void setNombre(string nombre);
	void setPuesto(string puesto);
	void setEstado(string estado);
};

#endif
