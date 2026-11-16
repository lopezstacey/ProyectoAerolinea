#include "Ruta.h"

Ruta::Ruta()
{
	codigo = "";
	matriculaAeronave = "";
	codigoVuelo = "";
}

Ruta::Ruta(string codigo, string matriculaAeronave, string codigoVuelo)
{
	this->codigo = codigo;
	this->matriculaAeronave = matriculaAeronave;
	this->codigoVuelo = codigoVuelo;
}

string Ruta::getCodigo() const
{
	return codigo;
}

string Ruta::getMatriculaAeronave() const
{
	return matriculaAeronave;
}

string Ruta::getCodigoVuelo() const
{
	return codigoVuelo;
}

void Ruta::setCodigo(string codigo)
{
	this->codigo = codigo;
}

void Ruta::setMatriculaAeronave(string matriculaAeronave)
{
	this->matriculaAeronave = matriculaAeronave;
}

void Ruta::setCodigoVuelo(string codigoVuelo)
{
	this->codigoVuelo = codigoVuelo;
}
