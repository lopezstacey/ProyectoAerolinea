#ifndef RUTA_H
#define RUTA_H
#include <string>

using namespace std;

class Ruta
{
private:
	string codigo;
	string matriculaAeronave;
	string codigoVuelo;
public:
	Ruta();
	Ruta(string codigo, string matriculaAeronave, string codigoVuelo);
	string getCodigo() const;
	string getMatriculaAeronave() const;
	string getCodigoVuelo() const;
	void setCodigo(string codigo);
	void setMatriculaAeronave(string matriculaAeronave);
	void setCodigoVuelo(string codigoVuelo);
};

#endif
