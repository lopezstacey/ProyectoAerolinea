#ifndef VUELO_H
#define VUELO_H
#include <string>
using namespace std;

class Vuelo
{
private:
	string codigo;
	string codigoOrigen;
	string codigoDestino;
	float costo;
	float duracion;
public:
	Vuelo();
	Vuelo(string codigo, string codigoOrigen, string codigoDestino, float costo, float duracion);
	string getCodigo() const;
	string getCodigoOrigen() const;
	string getCodigoDestino() const;
	float getCosto() const;
	float getDuracion() const;
	void setCodigo(string codigo);
	void setCodigoOrigen(string codigoOrigen);
	void setCodigoDestino(string codigoDestino);
	void setCosto(float costo);
	void setDuracion(float duracion);
};

#endif
