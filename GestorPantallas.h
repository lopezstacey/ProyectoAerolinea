#ifndef GESTORPANTALLAS_H
#define GESTORPANTALLAS_H
#include "Pantalla.h"

class GestorPantallas
{
private:
	Pantalla* pantallaActual;
	Pantalla* pantallaPendiente;
	
public:
	GestorPantallas();
	void cambiarPantalla(Pantalla* nuevaPantalla);
	void solicitarCambioPantalla(Pantalla* nuevaPantalla);
	void procesarEventos(RenderWindow& ventana, Event evento);
	void actualizar(float tiempo);
	void dibujar(RenderWindow& ventana);
	~GestorPantallas();
};

#endif
