
#include "Vuelo.h"

Vuelo::Vuelo()
{
	codigo = 0;
	ciudadOrigen = 0;
	ciudadDestino = 0;
	costo = 0;
	duracion = 0;
}

Vuelo::Vuelo(int codigo, int ciudadOrigen, int ciudadDestino, float costo, float duracion)
{
	this->codigo = codigo;
	this->ciudadOrigen = ciudadOrigen;
	this->ciudadDestino = ciudadDestino;
	this->costo = costo;
	this->duracion = duracion;
}

int Vuelo::getCodigo() const
{
	return codigo;
}

int Vuelo::getCiudadOrigen() const
{
	return ciudadOrigen;
}

int Vuelo::getCiudadDestino() const
{
	return ciudadDestino;
}

float Vuelo::getCosto() const
{
	return costo;
}

float Vuelo::getDuracion() const
{
	return duracion;
}

void Vuelo::setCosto(float costo)
{
	this->costo = costo;
}

void Vuelo::setDuracion(float duracion)
{
	this->duracion = duracion;
}

