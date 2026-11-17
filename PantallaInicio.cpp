
#include "PantallaInicio.h"
#include <iostream>

using namespace sf;
using namespace std;

PantallaInicio::PantallaInicio()
{
	imagenCargada = texturaFondo.loadFromFile("resources/fondo_aerorutas.png");
	
	if (imagenCargada)
	{
		imagenFondo.setTexture(texturaFondo);
		Vector2u tamano = texturaFondo.getSize();
		float anchoDisponible = 1200.0f;
		float altoDisponible = 800.0f;
		float escalaX = anchoDisponible / tamano.x;
		float escalaY = altoDisponible / tamano.y;
		imagenFondo.setPosition(300, 0);
		imagenFondo.setScale(escalaX, escalaY);
	}
	else
	{
		cout << "No se pudo cargar la imagen principal de AERORUTAS." << endl;
	}
}

void PantallaInicio::procesarEventos(RenderWindow& ventana, Event evento)
{
}

void PantallaInicio::actualizar(float tiempo)
{
}

void PantallaInicio::dibujar(RenderWindow& ventana)
{
	RectangleShape fondo(Vector2f(1200, 800));
	fondo.setPosition(300, 0);
	fondo.setFillColor(Color(20, 75, 130));
	
	ventana.draw(fondo);
	
	if (imagenCargada)
	{
		ventana.draw(imagenFondo);
	}
}
