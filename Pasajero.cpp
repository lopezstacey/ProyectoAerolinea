#include "Pasajero.h"

Pasajero::Pasajero()
{
	identificacion = "";
	nombre = "";
	telefono = "";
	correo = "";
}

Pasajero::Pasajero(string identificacion, string nombre, string telefono, string correo)
{
	this->identificacion = identificacion;
	this->nombre = nombre;
	this->telefono = telefono;
	this->correo = correo;
}

string Pasajero::getIdentificacion() const
{
	return identificacion;
}

string Pasajero::getNombre() const
{
	return nombre;
}

string Pasajero::getTelefono() const
{
	return telefono;
}

string Pasajero::getCorreo() const
{
	return correo;
}

void Pasajero::setIdentificacion(string identificacion)
{
	this->identificacion = identificacion;
}

void Pasajero::setNombre(string nombre)
{
	this->nombre = nombre;
}

void Pasajero::setTelefono(string telefono)
{
	this->telefono = telefono;
}

void Pasajero::setCorreo(string correo)
{
	this->correo = correo;
}
