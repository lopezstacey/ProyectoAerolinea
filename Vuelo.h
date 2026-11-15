
#ifndef VUELO_H
#define VUELO_H

class Vuelo
{
private:
	int codigo;
	int ciudadOrigen;
	int ciudadDestino;
	float costo;
	float duracion;
	
public:
	Vuelo();
	Vuelo(int codigo, int ciudadOrigen, int ciudadDestino, float costo, float duracion);
	int getCodigo() const;
	int getCiudadOrigen() const;
	int getCiudadDestino() const;
	float getCosto() const;
	float getDuracion() const;
	void setCosto(float costo);
	void setDuracion(float duracion);
};

#endif
