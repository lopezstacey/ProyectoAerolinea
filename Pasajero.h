#ifndef PASAJERO_H
#define PASAJERO_H

#include <string>

using namespace std;

class Pasajero
{
private:
	string identificacion;
	string nombre;
	string telefono;
	string correo;
public:
	Pasajero();
	Pasajero(string identificacion, string nombre, string telefono, string correo);
	string getIdentificacion() const;
	string getNombre() const;
	string getTelefono() const;
	string getCorreo() const;
	void setIdentificacion(string identificacion);
	void setNombre(string nombre);
	void setTelefono(string telefono);
	void setCorreo(string correo);
};

#endif
