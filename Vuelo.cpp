#include "Vuelo.h"

Vuelo::Vuelo()
{
	codigo = "";
	codigoOrigen = "";
	codigoDestino = "";
	costo = 0;
	duracion = 0;
}

Vuelo::Vuelo(string codigo, string codigoOrigen, string codigoDestino, float costo, float duracion)
{
	this->codigo = codigo;
	this->codigoOrigen = codigoOrigen;
	this->codigoDestino = codigoDestino;
	this->costo = costo;
	this->duracion = duracion;
}

string Vuelo::getCodigo() const
{
	return codigo;
}

string Vuelo::getCodigoOrigen() const
{
	return codigoOrigen;
}

string Vuelo::getCodigoDestino() const
{
	return codigoDestino;
}

float Vuelo::getCosto() const
{
	return costo;
}

float Vuelo::getDuracion() const
{
	return duracion;
}

void Vuelo::setCodigo(string codigo)
{
	this->codigo = codigo;
}

void Vuelo::setCodigoOrigen(string codigoOrigen)
{
	this->codigoOrigen = codigoOrigen;
}

void Vuelo::setCodigoDestino(string codigoDestino)
{
	this->codigoDestino = codigoDestino;
}

void Vuelo::setCosto(float costo)
{
	this->costo = costo;
}

void Vuelo::setDuracion(float duracion)
{
	this->duracion = duracion;
}
