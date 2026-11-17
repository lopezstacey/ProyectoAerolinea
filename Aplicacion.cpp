#include "Aplicacion.h"
#include "PantallaInicio.h"
#include "PantallaCiudades.h"
#include "PantallaAeronaves.h"
#include "PantallaTripulantes.h"
#include "PantallaPasajeros.h"
#include "PantallaMaletas.h"
#include "PantallaVuelos.h"
#include "PantallaRutas.h"
#include "PantallaExplorar.h"
#include "PantallaBuscarRutas.h"
#include "PantallaPlanificarViaje.h"
#include "PantallaBellmanFord.h"

using namespace sf;

Aplicacion::Aplicacion() : grafo(&grafoInterno)
{
	ventana.create(VideoMode(1500, 800), "AERORUTAS - Aerolinea de Costa Rica", Style::Titlebar | Style::Close);
	ventana.setFramerateLimit(60);
	ejecutando = true;
	gestorPantallas.cambiarPantalla(new PantallaInicio());
}

Aplicacion::Aplicacion(Grafo* grafoCompartido) : grafo(grafoCompartido != nullptr ? grafoCompartido : &grafoInterno)
{
	ventana.create(VideoMode(1500, 800), "AERORUTAS - Aerolinea de Costa Rica", Style::Titlebar | Style::Close);
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
			opcion = barraLateral.procesarEvento(ventana, evento);
		
		if (opcion == BarraLateral::EXPLORAR_DESTINOS)
		{
			gestorPantallas.cambiarPantalla(new PantallaExplorar(grafo));
			barraLateral.seleccionarOpcion(opcion);
			continue;
		}
		
		if (opcion == BarraLateral::BUSCAR_RUTAS)
		{
			gestorPantallas.cambiarPantalla(new PantallaBuscarRutas(grafo));
			barraLateral.seleccionarOpcion(opcion);
			continue;
		}
		
		if (opcion == BarraLateral::DESTINOS_ALCANZABLES)
		{
			gestorPantallas.cambiarPantalla(new PantallaBellmanFord(grafo));
			barraLateral.seleccionarOpcion(opcion);
			continue;
		}
		
		if (opcion == BarraLateral::PLANIFICAR_VIAJE)
		{
			gestorPantallas.cambiarPantalla(new PantallaPlanificarViaje(&listaPasajeros, &listaVuelos, &arbolPasajeros, &gestorAsignaciones));
			barraLateral.seleccionarOpcion(opcion);
			continue;
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
		if (opcion == BarraLateral::GESTION_VUELOS)
		{
			gestorPantallas.cambiarPantalla(new PantallaVuelos(&listaVuelos, grafo));
			barraLateral.seleccionarOpcion(opcion);
			continue;
		}
		if (opcion == BarraLateral::GESTION_RUTAS)
		{
			gestorPantallas.cambiarPantalla(new PantallaGestionRutas(&listaRutas, &listaAeronaves, &listaVuelos,grafo));
			barraLateral.seleccionarOpcion(opcion);
			continue;
		}
		if (opcion == BarraLateral::GESTION_TRIPULACION)
		{
			gestorPantallas.cambiarPantalla(new PantallaTripulantes(&listaTripulantes, &listaAeronaves, &gestorAsignaciones));
			barraLateral.seleccionarOpcion(opcion);
			continue;
		}
		if (opcion == BarraLateral::GESTION_PASAJEROS)
		{
			gestorPantallas.cambiarPantalla(new PantallaPasajeros(&listaPasajeros, &listaMaletas, &gestorPantallas, grafo));
			barraLateral.seleccionarOpcion(opcion);
			continue;
		}
		if (opcion == BarraLateral::GESTION_MALETAS)
		{
			gestorPantallas.cambiarPantalla(new PantallaMaletas(&listaMaletas, grafo));
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
	ventana.setView(ventana.getDefaultView());
	ventana.clear(Color(12, 31, 56));
	gestorPantallas.dibujar(ventana);
	ventana.setView(ventana.getDefaultView());
	barraLateral.dibujar(ventana);
	ventana.display();
}
