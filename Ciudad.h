
#ifndef CIUDAD_H
#define CIUDAD_H

#include <string>
using namespace std;

class Ciudad
{
private:
	int codigo;
	string nombre;
	float posicionX;
	float posicionY;
	
public:
	Ciudad();
	
	Ciudad(int codigo, string nombre, float posicionX, float posicionY);
	
	int getCodigo() const;
	string getNombre() const;
	float getPosicionX() const;
	float getPosicionY() const;
	void setNombre(string nombre);
	void setPosicion(float x, float y);
};

#endif
