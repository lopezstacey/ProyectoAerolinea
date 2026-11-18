#include "BarraLateral.h"
#include <string>

using namespace sf;
using namespace std;

namespace
{
	const float BOTON_X = 25.0f;
	const float BOTON_ANCHO = 250.0f;
	const float BOTON_ALTO = 48.0f;
	const float Y_CLIENTE = 135.0f;
	const float OPCION_ALTO = 40.0f;
	const float OPCION_PASO = 45.0f;
	const float Y_OPCIONES_CLIENTE = 190.0f;
	const float Y_OPCIONES_ADMIN = 255.0f;
	const float Y_ADMIN_CERRADO = 195.0f;
	const float Y_ADMIN_CON_CLIENTE = 425.0f;
	const float Y_ARBOLES_LIBRE = 255.0f;
	const float Y_ARBOLES_CON_CLIENTE = 490.0f;
	const float Y_ARBOLES_CON_ADMIN = 580.0f;
	const float SEPARACION_OPCIONES = 55.0f;
	const float Y_INICIO = 730.0f;
	const float INICIO_ALTO = 38.0f;
	const int CANTIDAD_CLIENTE = 4;
	const int CANTIDAD_ADMIN = 7;
	const int CANTIDAD_ARBOLES = 4;
	const char* const NOMBRES_CLIENTE[CANTIDAD_CLIENTE] = { "EXPLORAR DESTINOS", "BUSCAR RUTAS", "MIS VUELOS", "MI EQUIPAJE" };
	const int IDS_CLIENTE[CANTIDAD_CLIENTE] = { BarraLateral::EXPLORAR_DESTINOS, BarraLateral::BUSCAR_RUTAS, BarraLateral::MIS_VUELOS, BarraLateral::MI_EQUIPAJE };
	const char* const NOMBRES_ADMIN[CANTIDAD_ADMIN] = { "GESTIONAR CIUDADES", "GESTIONAR AERONAVES", "GESTIONAR VUELOS", "GESTIONAR RUTAS", "GESTIONAR TRIPULACION", "GESTIONAR PASAJEROS", "GESTIONAR MALETAS" };
	const int IDS_ADMIN[CANTIDAD_ADMIN] = { BarraLateral::GESTION_CIUDADES, BarraLateral::GESTION_AERONAVES, BarraLateral::GESTION_VUELOS, BarraLateral::GESTION_RUTAS, BarraLateral::GESTION_TRIPULACION, BarraLateral::GESTION_PASAJEROS, BarraLateral::GESTION_MALETAS };
	const char* const NOMBRES_ARBOLES[CANTIDAD_ARBOLES] = { "ARBOL ABB (CIUDADES)", "ARBOL AVL (PASAJEROS)", "ARBOL PRIM (COSTO)", "ARBOL KRUSKAL (TIEMPO)" };
	const int IDS_ARBOLES[CANTIDAD_ARBOLES] = { BarraLateral::ARBOL_ABB, BarraLateral::ARBOL_AVL, BarraLateral::ARBOL_PRIM, BarraLateral::ARBOL_KRUSKAL };
}

BarraLateral::BarraLateral()
{
	fuenteCargada = fuente.loadFromFile("resources/times.ttf");
	clienteDesplegado = false;
	administracionDesplegada = false;
	arbolesDesplegado = false;
	opcionSeleccionada = INICIO;
}

void BarraLateral::dibujarRectanguloRedondeado(RenderWindow& ventana, float x, float y, float ancho, float alto, float radio, Color color)
{
	RectangleShape centro(Vector2f(ancho - radio * 2, alto));
	centro.setPosition(x + radio, y);
	centro.setFillColor(color);
	ventana.draw(centro);
	
	RectangleShape medio(Vector2f(ancho, alto - radio * 2));
	medio.setPosition(x, y + radio);
	medio.setFillColor(color);
	ventana.draw(medio);
	
	CircleShape esquina(radio);
	esquina.setFillColor(color);
	esquina.setPosition(x, y);
	ventana.draw(esquina);
	esquina.setPosition(x + ancho - radio * 2, y);
	ventana.draw(esquina);
	esquina.setPosition(x, y + alto - radio * 2);
	ventana.draw(esquina);
	esquina.setPosition(x + ancho - radio * 2, y + alto - radio * 2);
	ventana.draw(esquina);
}

void BarraLateral::dibujarTexto(RenderWindow& ventana, float x, float y, const string& texto, int tamano, Color color)
{
	if (fuenteCargada)
	{
		Text etiqueta;
		etiqueta.setFont(fuente);
		etiqueta.setString(texto);
		etiqueta.setCharacterSize(tamano);
		etiqueta.setFillColor(color);
		etiqueta.setPosition(x, y);
		ventana.draw(etiqueta);
	}
}

float BarraLateral::yAdministracion() const
{
	return clienteDesplegado ? Y_ADMIN_CON_CLIENTE : Y_ADMIN_CERRADO;
}

float BarraLateral::yBotonArboles() const
{
	if (clienteDesplegado)
		return Y_ARBOLES_CON_CLIENTE;
	
	if (administracionDesplegada)
		return Y_ARBOLES_CON_ADMIN;
	
	return Y_ARBOLES_LIBRE;
}

void BarraLateral::dibujarBoton(RenderWindow& ventana, float x, float y, float ancho, float alto, const string& texto, Color color)
{
	Vector2i mouse = Mouse::getPosition(ventana);
	bool encima = mouse.x >= x && mouse.x <= x + ancho && mouse.y >= y && mouse.y <= y + alto;
	
	Color colorFinal = color;
	
	if (encima)
	{
		colorFinal.r = static_cast<Uint8>(color.r > 230 ? 255 : color.r + 25);
		colorFinal.g = static_cast<Uint8>(color.g > 230 ? 255 : color.g + 25);
		colorFinal.b = static_cast<Uint8>(color.b > 230 ? 255 : color.b + 25);
	}
	
	dibujarRectanguloRedondeado(ventana, x, y, ancho, alto, 16, colorFinal);
	
	if (fuenteCargada)
	{
		Text etiqueta;
		etiqueta.setFont(fuente);
		etiqueta.setString(texto);
		etiqueta.setCharacterSize(16);
		etiqueta.setFillColor(Color::White);
		
		FloatRect limites = etiqueta.getLocalBounds();
		etiqueta.setPosition(x + (ancho - limites.width) / 2 - limites.left, y + (alto - limites.height) / 2 - limites.top);
		
		ventana.draw(etiqueta);
	}
}

void BarraLateral::dibujarOpcion(RenderWindow& ventana, float y, const string& texto, bool seleccionada)
{
	Vector2i mouse = Mouse::getPosition(ventana);
	bool encima = mouse.x >= BOTON_X && mouse.x <= BOTON_X + BOTON_ANCHO && mouse.y >= y && mouse.y <= y + OPCION_ALTO;
	
	if (seleccionada || encima)
		dibujarRectanguloRedondeado(ventana, BOTON_X, y, BOTON_ANCHO, OPCION_ALTO, 10, seleccionada ? Color(225, 238, 226) : Color(240, 244, 240));
	
	if (fuenteCargada)
	{
		Text etiqueta;
		etiqueta.setFont(fuente);
		etiqueta.setString(texto);
		etiqueta.setCharacterSize(14);
		etiqueta.setFillColor(seleccionada ? Color(35, 65, 52) : Color(75, 80, 75));
		if (seleccionada)
			etiqueta.setStyle(Text::Bold);
		
		FloatRect limites = etiqueta.getLocalBounds();
		etiqueta.setPosition(BOTON_X + 20 - limites.left, y + (OPCION_ALTO - limites.height) / 2 - limites.top);
		ventana.draw(etiqueta);
	}
}

int BarraLateral::procesarEvento(RenderWindow& ventana, Event evento)
{
	if (evento.type != Event::MouseButtonPressed || evento.mouseButton.button != Mouse::Left)
		return NINGUNA;
	
	int x = evento.mouseButton.x;
	int y = evento.mouseButton.y;
	
	if (x < BOTON_X || x > BOTON_X + BOTON_ANCHO)
		return NINGUNA;
	
	if (y >= Y_INICIO && y <= Y_INICIO + INICIO_ALTO)
		return INICIO;
	
	if (y >= Y_CLIENTE && y <= Y_CLIENTE + BOTON_ALTO)
	{
		clienteDesplegado = !clienteDesplegado;
		administracionDesplegada = false;
		arbolesDesplegado = false;
		return NINGUNA;
	}
	
	float yAdmin = yAdministracion();
	
	if (y >= yAdmin && y <= yAdmin + BOTON_ALTO)
	{
		administracionDesplegada = !administracionDesplegada;
		clienteDesplegado = false;
		arbolesDesplegado = false;
		return NINGUNA;
	}
	
	if (clienteDesplegado)
	{
		for (int i = 0; i < CANTIDAD_CLIENTE; i++)
		{
			float arriba = Y_OPCIONES_CLIENTE + i * OPCION_PASO;
			if (y >= arriba && y <= arriba + OPCION_ALTO)
				return IDS_CLIENTE[i];
		}
	}
	
	if (administracionDesplegada)
	{
		for (int i = 0; i < CANTIDAD_ADMIN; i++)
		{
			float arriba = Y_OPCIONES_ADMIN + i * OPCION_PASO;
			if (y >= arriba && y <= arriba + OPCION_ALTO)
				return IDS_ADMIN[i];
		}
	}
	
	float yArboles = yBotonArboles();
	
	if (y >= yArboles && y <= yArboles + BOTON_ALTO)
	{
		arbolesDesplegado = !arbolesDesplegado;
		clienteDesplegado = false;
		administracionDesplegada = false;
		return NINGUNA;
	}
	
	if (arbolesDesplegado)
	{
		for (int i = 0; i < CANTIDAD_ARBOLES; i++)
		{
			float arriba = yArboles + SEPARACION_OPCIONES + i * OPCION_PASO;
			if (y >= arriba && y <= arriba + OPCION_ALTO)
				return IDS_ARBOLES[i];
		}
	}
	
	return NINGUNA;
}

void BarraLateral::dibujar(RenderWindow& ventana)
{
	RectangleShape fondo(Vector2f(300, static_cast<float>(ventana.getSize().y)));
	fondo.setFillColor(Color::White);
	ventana.draw(fondo);
	dibujarTexto(ventana, 35, 35, "AERORUTAS", 29, Color(55, 70, 60));
	dibujarTexto(ventana, 35, 78, "AEROLINEA DE COSTA RICA", 12, Color(120, 125, 120));
	dibujarRectanguloRedondeado(ventana, 25, 115, 250, 2, 1, Color(215, 220, 215));
	dibujarBoton(ventana, BOTON_X, Y_CLIENTE, BOTON_ANCHO, BOTON_ALTO, "PARTE CLIENTE", clienteDesplegado ? Color(55, 70, 60) : Color(90, 120, 95));
	
	if (clienteDesplegado)
	{
		for (int i = 0; i < CANTIDAD_CLIENTE; i++)
			dibujarOpcion(ventana, Y_OPCIONES_CLIENTE + i * OPCION_PASO, NOMBRES_CLIENTE[i], opcionSeleccionada == IDS_CLIENTE[i]);
	}
	
	dibujarBoton(ventana, BOTON_X, yAdministracion(), BOTON_ANCHO, BOTON_ALTO, "PARTE ADMINISTRATIVA", administracionDesplegada ? Color(55, 70, 60) : Color(90, 120, 95));
	
	if (administracionDesplegada)
	{
		for (int i = 0; i < CANTIDAD_ADMIN; i++)
			dibujarOpcion(ventana, Y_OPCIONES_ADMIN + i * OPCION_PASO, NOMBRES_ADMIN[i], opcionSeleccionada == IDS_ADMIN[i]);
	}
	
	float yArboles = yBotonArboles();
	dibujarBoton(ventana, BOTON_X, yArboles, BOTON_ANCHO, BOTON_ALTO, "ARBOLES", arbolesDesplegado ? Color(55, 70, 60) : Color(90, 120, 95));
	
	if (arbolesDesplegado)
	{
		for (int i = 0; i < CANTIDAD_ARBOLES; i++)
			dibujarOpcion(ventana, yArboles + SEPARACION_OPCIONES + i * OPCION_PASO, NOMBRES_ARBOLES[i], opcionSeleccionada == IDS_ARBOLES[i]);
	}
	
	dibujarBoton(ventana, BOTON_X, Y_INICIO, BOTON_ANCHO, INICIO_ALTO, "VOLVER AL INICIO", Color(90, 120, 95));
}

int BarraLateral::getOpcionSeleccionada() const
{
	return opcionSeleccionada;
}

void BarraLateral::seleccionarOpcion(int opcion)
{
	opcionSeleccionada = opcion;
}
