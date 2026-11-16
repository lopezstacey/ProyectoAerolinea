#ifndef PANTALLA_H
#define PANTALLA_H
#include <SFML/Graphics.hpp>
using namespace sf;

class Pantalla
{
public:
	virtual void procesarEventos(RenderWindow& ventana, Event evento) = 0;
	virtual void actualizar(float tiempo) = 0;
	virtual void dibujar(RenderWindow& ventana) = 0;
	virtual ~Pantalla();
};

#endif
