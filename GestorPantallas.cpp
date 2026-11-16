#include "GestorPantallas.h"

GestorPantallas::GestorPantallas()
{
	pantallaActual = nullptr;
	pantallaPendiente = nullptr;
}

GestorPantallas::~GestorPantallas()
{
	delete pantallaActual;
	delete pantallaPendiente;
	pantallaActual = nullptr;
	pantallaPendiente = nullptr;
}

void GestorPantallas::cambiarPantalla(Pantalla* nuevaPantalla)
{
	if (pantallaActual != nuevaPantalla)
	{
		delete pantallaActual;
		pantallaActual = nuevaPantalla;
	}
}

void GestorPantallas::solicitarCambioPantalla(Pantalla* nuevaPantalla)
{
	delete pantallaPendiente;
	pantallaPendiente = nuevaPantalla;
}

void GestorPantallas::procesarEventos(RenderWindow& ventana, Event evento)
{
	if (pantallaActual != nullptr)
		pantallaActual->procesarEventos(ventana, evento);
	
	if (pantallaPendiente != nullptr)
	{
		delete pantallaActual;
		pantallaActual = pantallaPendiente;
		pantallaPendiente = nullptr;
	}
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
