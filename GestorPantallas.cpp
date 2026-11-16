#include "GestorPantallas.h"

GestorPantallas::GestorPantallas()
{
	pantallaActual = nullptr;
}

GestorPantallas::~GestorPantallas()
{
	delete pantallaActual;
	pantallaActual = nullptr;
}

void GestorPantallas::cambiarPantalla(Pantalla* nuevaPantalla)
{
	if (pantallaActual != nuevaPantalla)
	{
		delete pantallaActual;
		pantallaActual = nuevaPantalla;
	}
}

void GestorPantallas::procesarEventos(RenderWindow& ventana, Event evento)
{
	if (pantallaActual != nullptr)
		pantallaActual->procesarEventos(ventana, evento);
}

void GestorPantallas::actualizar(float tiempo)
{
	if (pantallaActual != nullptr)
		pantallaActual->actualizar(tiempo);
}

void GestorPantallas::dibujar(RenderWindow& ventana)
{
	if (pantallaActual != nullptr)
		pantallaActual->dibujar(ventana);
}
