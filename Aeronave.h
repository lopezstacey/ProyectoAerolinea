#ifndef AERONAVE_H
#define AERONAVE_H
#include <string>
using namespace std;

class Aeronave
{
private:
	string matricula;
	string modelo;
	int capacidad;
	string estado;
	string imagen;
public:
	Aeronave();
	Aeronave(string matricula, string modelo, int capacidad, string estado, string imagen = "");
	string getMatricula() const;
	string getModelo() const;
	int getCapacidad() const;
	string getEstado() const;
	string getImagen() const;
	void setMatricula(string matricula);
	void setModelo(string modelo);
	void setCapacidad(int capacidad);
	void setEstado(string estado);
	void setImagen(string imagen);
};

#endif
