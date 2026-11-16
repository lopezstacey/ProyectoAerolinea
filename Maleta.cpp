#include "Maleta.h"

Maleta::Maleta()
{
	codigo = "";
	identificacionPasajero = "";
	peso = 0;
	destino = "";
	estado = "Registrada";
}

Maleta::Maleta(string codigo, string identificacionPasajero, float peso, string destino, string estado)
{
	this->codigo = codigo;
	this->identificacionPasajero = identificacionPasajero;
	this->peso = peso;
	this->destino = destino;
	this->estado = estado;
}

string Maleta::getCodigo() const
{
	return codigo;
}

string Maleta::getIdentificacionPasajero() const
{
	return identificacionPasajero;
}

float Maleta::getPeso() const
{
	return peso;
}

string Maleta::getDestino() const
{
	return destino;
}

string Maleta::getEstado() const
{
	return estado;
}

void Maleta::setCodigo(string codigo)
{
	this->codigo = codigo;
}

void Maleta::setIdentificacionPasajero(string identificacionPasajero)
{
	this->identificacionPasajero = identificacionPasajero;
}

void Maleta::setPeso(float peso)
{
	this->peso = peso;
}

void Maleta::setDestino(string destino)
{
	this->destino = destino;
}

void Maleta::setEstado(string estado)
{
	this->estado = estado;
}
