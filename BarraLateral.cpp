#include "BarraLateral.h"
#include <string>

using namespace sf;
using namespace std;

BarraLateral::BarraLateral()
{
	fuenteCargada = fuente.loadFromFile("resources/times.ttf");
	clienteDesplegado = false;
	administracionDesplegada = false;
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

int BarraLateral::procesarEvento(RenderWindow& ventana, Event evento)
{
	if (evento.type != Event::MouseButtonPressed || evento.mouseButton.button != Mouse::Left)
		return NINGUNA;
	
	int x = evento.mouseButton.x;
	int y = evento.mouseButton.y;
	
	if (x < 20 || x > 280)
		return NINGUNA;
	
	if (y >= 725 && y <= 763)
		return INICIO;
	
	if (y >= 135 && y <= 183)
	{
		clienteDesplegado = !clienteDesplegado;
		administracionDesplegada = false;
		return NINGUNA;
	}
	
	if (y >= 195 && y <= 243)
	{
		administracionDesplegada = !administracionDesplegada;
		clienteDesplegado = false;
		return NINGUNA;
	}
	
	if (clienteDesplegado)
	{
		if (y >= 255 && y <= 295)
			return EXPLORAR_DESTINOS;
		
		if (y >= 300 && y <= 340)
			return BUSCAR_RUTAS;
		
		if (y >= 345 && y <= 385)
			return PLANIFICAR_VIAJE;
		
		if (y >= 390 && y <= 430)
			return MIS_VUELOS;
		
		if (y >= 435 && y <= 475)
			return MI_EQUIPAJE;
	}
	
	if (administracionDesplegada)
	{
		if (y >= 255 && y <= 295)
			return GESTION_CIUDADES;
		
		if (y >= 300 && y <= 340)
			return GESTION_AERONAVES;
		
		if (y >= 345 && y <= 385)
			return GESTION_VUELOS;
		
		if (y >= 390 && y <= 430)
			return GESTION_RUTAS;
		
		if (y >= 435 && y <= 475)
			return GESTION_TRIPULACION;
		
		if (y >= 480 && y <= 520)
			return GESTION_PASAJEROS;
		
		if (y >= 525 && y <= 565)
			return GESTION_MALETAS;
		
		if (y >= 570 && y <= 610)
			return ARBOLES_GRAFOS;
	}
	
	return NINGUNA;
}

void BarraLateral::dibujar(RenderWindow& ventana)
{
	RectangleShape fondo(Vector2f(300, static_cast<float>(ventana.getSize().y)));
	fondo.setFillColor(Color::White);
	ventana.draw(fondo);
	
	dibujarTexto(ventana, 35, 35, "AERORUTAS", 29, Color(20, 30, 40));
	dibujarTexto(ventana, 35, 78, "AEROLINEA DE COSTA RICA", 12, Color(45, 55, 65));
	
	dibujarRectanguloRedondeado(ventana, 25, 115, 250, 2, 1, Color(190, 200, 210));
	
	dibujarBoton(ventana, 25, 135, 250, 48, "PARTE CLIENTE", clienteDesplegado ? Color(225, 125, 30) : Color(28, 91, 145));
	dibujarBoton(ventana, 25, 195, 250, 48, "PARTE ADMINISTRATIVA", administracionDesplegada ? Color(225, 125, 30) : Color(28, 91, 145));
	
	if (clienteDesplegado)
	{
		dibujarTexto(ventana, 45, 260, "EXPLORAR DESTINOS", 14, Color::Black);
		dibujarTexto(ventana, 45, 305, "BUSCAR RUTAS", 14, Color::Black);
		dibujarTexto(ventana, 45, 350, "PLANIFICAR VIAJE", 14, Color::Black);
		dibujarTexto(ventana, 45, 395, "MIS VUELOS", 14, Color::Black);
		dibujarTexto(ventana, 45, 440, "MI EQUIPAJE", 14, Color::Black);
	}
	
	if (administracionDesplegada)
	{
		dibujarTexto(ventana, 45, 260, "GESTIONAR CIUDADES", 14, Color::Black);
		dibujarTexto(ventana, 45, 305, "GESTIONAR AERONAVES", 14, Color::Black);
		dibujarTexto(ventana, 45, 350, "GESTIONAR VUELOS", 14, Color::Black);
		dibujarTexto(ventana, 45, 395, "GESTIONAR RUTAS", 14, Color::Black);
		dibujarTexto(ventana, 45, 440, "GESTIONAR TRIPULACION", 14, Color::Black);
		dibujarTexto(ventana, 45, 485, "GESTIONAR PASAJEROS", 14, Color::Black);
		dibujarTexto(ventana, 45, 530, "GESTIONAR MALETAS", 14, Color::Black);
		dibujarTexto(ventana, 45, 575, "ARBOLES Y GRAFOS", 14, Color::Black);
	}
	dibujarBoton(ventana, 25, 730, 250, 38, "VOLVER AL INICIO", Color(28, 91, 145));
}

int BarraLateral::getOpcionSeleccionada() const
{
	return opcionSeleccionada;
}

void BarraLateral::seleccionarOpcion(int opcion)
{
	opcionSeleccionada = opcion;
}
