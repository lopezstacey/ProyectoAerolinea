#include "Aeronave.h"

Aeronave::Aeronave()
{
	matricula = "";
	modelo = "";
	capacidad = 0;
	estado = "Disponible";
	imagen = "";
}

Aeronave::Aeronave(string matricula, string modelo, int capacidad, string estado, string imagen)
{
	this->matricula = matricula;
	this->modelo = modelo;
	this->capacidad = capacidad;
	this->estado = estado;
	this->imagen = imagen;
}

string Aeronave::getMatricula() const
{
	return matricula;
}

string Aeronave::getModelo() const
{
	return modelo;
}

int Aeronave::getCapacidad() const
{
	return capacidad;
}

string Aeronave::getEstado() const
{
	return estado;
}

string Aeronave::getImagen() const
{
	return imagen;
}

void Aeronave::setMatricula(string matricula)
{
	this->matricula = matricula;
}

void Aeronave::setModelo(string modelo)
{
	this->modelo = modelo;
}

void Aeronave::setCapacidad(int capacidad)
{
	this->capacidad = capacidad;
}

void Aeronave::setEstado(string estado)
{
	this->estado = estado;
}

void Aeronave::setImagen(string imagen)
{
	this->imagen = imagen;
}
