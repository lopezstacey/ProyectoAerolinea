#ifndef PANTALLAINICIO_H
#define PANTALLAINICIO_H

#include "Pantalla.h"

class PantallaInicio : public Pantalla
{
private:
	Texture texturaFondo;
	Sprite imagenFondo;
	bool imagenCargada;
public:
	PantallaInicio();
	void procesarEventos(RenderWindow& ventana, Event evento) override;
	void actualizar(float tiempo) override;
	void dibujar(RenderWindow& ventana) override;
};

#endif
