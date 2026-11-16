
#include "Aplicacion.h"
#include "PantallaInicio.h"
#include "PantallaCiudades.h"
#include "PantallaAeronaves.h"
#include "PantallaTripulantes.h"
#include "PantallaPasajeros.h"
#include "PantallaMaletas.h"

using namespace sf;

Aplicacion::Aplicacion() : grafo(&grafoInterno)
{
	ventana.create(VideoMode(1500, 800), "AERORUTAS - Aerolinea de Costa Rica");
	ventana.setFramerateLimit(60);
	ejecutando = true;
	gestorPantallas.cambiarPantalla(new PantallaInicio());
}

Aplicacion::Aplicacion(Grafo* grafoCompartido) : grafo(grafoCompartido != nullptr ? grafoCompartido : &grafoInterno)
{
	ventana.create(VideoMode(1500, 800), "AERORUTAS - Aerolinea de Costa Rica");
	ventana.setFramerateLimit(60);
	ejecutando = true;
	gestorPantallas.cambiarPantalla(new PantallaInicio());
}

void Aplicacion::ejecutar()
{
	while (ventana.isOpen() && ejecutando)
	{
		procesarEventos();
		actualizar();
		dibujar();
	}
}

void Aplicacion::procesarEventos()
{
	Event evento;
	
	while (ventana.pollEvent(evento))
	{
		if (evento.type == Event::Closed)
		{
			ventana.close();
			ejecutando = false;
			return;
		}
		
		int opcion = BarraLateral::NINGUNA;
		
		if (evento.type == Event::MouseButtonPressed && evento.mouseButton.button == Mouse::Left)
		{
			opcion = barraLateral.procesarEvento(ventana, evento);
		}
		
		if (opcion == BarraLateral::GESTION_CIUDADES)
		{
			gestorPantallas.cambiarPantalla(new PantallaCiudades(grafo));
			barraLateral.seleccionarOpcion(opcion);
			continue;
		}
		
		if (opcion == BarraLateral::GESTION_AERONAVES)
		{
			gestorPantallas.cambiarPantalla(new PantallaAeronaves(&listaAeronaves));
			barraLateral.seleccionarOpcion(opcion);
			continue;
		}
		
		if (opcion == BarraLateral::GESTION_TRIPULACION)
		{
			gestorPantallas.cambiarPantalla(new PantallaTripulantes(&listaTripulantes));
			barraLateral.seleccionarOpcion(opcion);
			continue;
		}
		
		if (opcion == BarraLateral::GESTION_PASAJEROS)
		{
			gestorPantallas.cambiarPantalla(new PantallaPasajeros(&listaPasajeros, &listaMaletas, &gestorPantallas));
			barraLateral.seleccionarOpcion(opcion);
			continue;
		}
		
		if (opcion == BarraLateral::GESTION_MALETAS)
		{
			gestorPantallas.cambiarPantalla(new PantallaMaletas(&listaMaletas));
			barraLateral.seleccionarOpcion(opcion);
			continue;
		}
		
		if (opcion == BarraLateral::INICIO)
		{
			gestorPantallas.cambiarPantalla(new PantallaInicio());
			barraLateral.seleccionarOpcion(opcion);
			continue;
		}
		
		if (opcion != BarraLateral::NINGUNA)
		{
			barraLateral.seleccionarOpcion(opcion);
			continue;
		}
		
		gestorPantallas.procesarEventos(ventana, evento);
	}
}

void Aplicacion::actualizar()
{
	float tiempo = reloj.restart().asSeconds();
	gestorPantallas.actualizar(tiempo);
}

void Aplicacion::dibujar()
{
	ventana.clear(Color(12, 31, 56));
	gestorPantallas.dibujar(ventana);
	barraLateral.dibujar(ventana);
	ventana.display();
}
