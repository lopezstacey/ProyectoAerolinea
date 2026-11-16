#ifndef CIUDAD_H
#define CIUDAD_H
#include <string>

using namespace std;

class Ciudad
{
private:
	string codigo;
	string nombre;
	float posicionX;
	float posicionY;
	
public:
	Ciudad();
	Ciudad(string codigo, string nombre, float posicionX, float posicionY);
	string getCodigo() const;
	string getNombre() const;
	float getPosicionX() const;
	float getPosicionY() const;
	void setCodigo(string codigo);
	void setNombre(string nombre);
	void setPosicionX(float posicionX);
	void setPosicionY(float posicionY);
};

#endif
