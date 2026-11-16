#ifndef MALETA_H
#define MALETA_H
#include <string>
using namespace std;

class Maleta
{
private:
	string codigo;
	string identificacionPasajero;
	float peso;
	string destino;
	string estado;
public:
	Maleta();
	Maleta(string codigo, string identificacionPasajero, float peso, string destino, string estado);
	string getCodigo() const;
	string getIdentificacionPasajero() const;
	float getPeso() const;
	string getDestino() const;
	string getEstado() const;
	void setCodigo(string codigo);
	void setIdentificacionPasajero(string identificacionPasajero);
	void setPeso(float peso);
	void setDestino(string destino);
	void setEstado(string estado);
};

#endif
