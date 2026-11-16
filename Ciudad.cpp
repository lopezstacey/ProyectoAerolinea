#include "Ciudad.h"

Ciudad::Ciudad()
{
	codigo = "";
	nombre = "";
	posicionX = 0;
	posicionY = 0;
}

Ciudad::Ciudad(string codigo, string nombre, float posicionX, float posicionY)
{
	this->codigo = codigo;
	this->nombre = nombre;
	this->posicionX = posicionX;
	this->posicionY = posicionY;
}

string Ciudad::getCodigo() const
{
	return codigo;
}

string Ciudad::getNombre() const
{
	return nombre;
}

float Ciudad::getPosicionX() const
{
	return posicionX;
}

float Ciudad::getPosicionY() const
{
	return posicionY;
}

void Ciudad::setCodigo(string codigo)
{
	this->codigo = codigo;
}

void Ciudad::setNombre(string nombre)
{
	this->nombre = nombre;
}

void Ciudad::setPosicionX(float posicionX)
{
	this->posicionX = posicionX;
}

void Ciudad::setPosicionY(float posicionY)
{
	this->posicionY = posicionY;
}
