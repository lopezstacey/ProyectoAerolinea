#include "Tripulante.h"

Tripulante::Tripulante()
{
	identificacion = "";
	nombre = "";
	puesto = "";
	estado = "Disponible";
}

Tripulante::Tripulante(string identificacion, string nombre, string puesto, string estado)
{
	this->identificacion = identificacion;
	this->nombre = nombre;
	this->puesto = puesto;
	this->estado = estado;
}

string Tripulante::getIdentificacion() const
{
	return identificacion;
}

string Tripulante::getNombre() const
{
	return nombre;
}

string Tripulante::getPuesto() const
{
	return puesto;
}

string Tripulante::getEstado() const
{
	return estado;
}

void Tripulante::setIdentificacion(string identificacion)
{
	this->identificacion = identificacion;
}

void Tripulante::setNombre(string nombre)
{
	this->nombre = nombre;
}

void Tripulante::setPuesto(string puesto)
{
	this->puesto = puesto;
}

void Tripulante::setEstado(string estado)
{
	this->estado = estado;
}
