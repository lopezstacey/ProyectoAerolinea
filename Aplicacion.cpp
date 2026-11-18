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
#include "PantallaMisVuelos.h"
#include "PantallaABB.h"
#include "PantallaExpansion.h"

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
			gestorPantallas.cambiarPantalla(new PantallaBuscarRutas(grafo, &listaPasajeros,	&listaMaletas, &listaVuelos, &listaReservas, &gestorPantallas));
			barraLateral.seleccionarOpcion(opcion);
			continue;
		}
		
		if (opcion == BarraLateral::MIS_VUELOS)
		{
			gestorPantallas.cambiarPantalla(new PantallaMisVuelos(&listaPasajeros, &listaMaletas, &listaVuelos, &listaReservas, &gestorPantallas, grafo));
			barraLateral.seleccionarOpcion(opcion);
			continue;
		}
		if (opcion == BarraLateral::MI_EQUIPAJE)
		{
			gestorPantallas.cambiarPantalla(new PantallaMiEquipaje(grafo, &listaPasajeros, &listaMaletas, &listaVuelos, &listaReservas));
			barraLateral.seleccionarOpcion(opcion);
			continue;
		}
		
		if (opcion == BarraLateral::GESTION_PASAJEROS)
		{
			gestorPantallas.cambiarPantalla(new PantallaPasajeros(&listaPasajeros, &listaMaletas, &gestorPantallas, grafo, &listaVuelos, &listaReservas));
			barraLateral.seleccionarOpcion(opcion);
			continue;
		}
		if (opcion == BarraLateral::GESTION_MALETAS)
		{
			PantallaMaletas* pantallaMaletas = new PantallaMaletas(&listaMaletas, grafo);
			pantallaMaletas->setListaPasajeros(&listaPasajeros);
			gestorPantallas.cambiarPantalla(pantallaMaletas);
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
		if (opcion == BarraLateral::INICIO)
		{
			gestorPantallas.cambiarPantalla(new PantallaInicio());
			barraLateral.seleccionarOpcion(opcion);
			continue;
		}
		if (opcion == BarraLateral::ARBOL_ABB)
		{
			gestorPantallas.cambiarPantalla(new PantallaABB(grafo));
			barraLateral.seleccionarOpcion(opcion);
			continue;
		}
//		if (opcion == BarraLateral::ARBOL_AVL)
//		{
//			gestorPantallas.cambiarPantalla(new PantallaAVL(&listaPasajeros, &listaVuelos, &listaReservas));
//			barraLateral.seleccionarOpcion(opcion);
//			continue;
//		}
		if (opcion == BarraLateral::ARBOL_PRIM)
		{
			gestorPantallas.cambiarPantalla(new PantallaExpansion(grafo, true));
			barraLateral.seleccionarOpcion(opcion);
			continue;
		}
		if (opcion == BarraLateral::ARBOL_KRUSKAL)
		{
			gestorPantallas.cambiarPantalla(new PantallaExpansion(grafo, false));
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
