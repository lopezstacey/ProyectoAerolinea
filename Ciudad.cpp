
#include "Ciudad.h"

Ciudad::Ciudad()
{
	codigo = 0;
	nombre = "";
	posicionX = 0;
	posicionY = 0;
}

Ciudad::Ciudad(int codigo, string nombre, float posicionX, float posicionY)
{
	this->codigo = codigo;
	this->nombre = nombre;
	this->posicionX = posicionX;
	this->posicionY = posicionY;
}

int Ciudad::getCodigo() const
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

void Ciudad::setNombre(string nombre)
{
	this->nombre = nombre;
}

void Ciudad::setPosicion(float x, float y)
{
	posicionX = x;
	posicionY = y;
}
