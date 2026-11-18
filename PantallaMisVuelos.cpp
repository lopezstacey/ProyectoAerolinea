#include "PantallaMisVuelos.h"
#include "PantallaResumen.h"
#include <sstream>
#include <iomanip>

static const float PANEL_X = 640;
static const float PANEL_Y = 200;
static const float PANEL_ANCHO = 520;
static const float PANEL_ALTO = 300;
static const float CAMPO_X = 665;
static const float CAMPO_Y = 285;
static const float CAMPO_ANCHO = 470;
static const float CAMPO_ALTO = 36;
static const float BOTON_Y = 345;
static const float BOTON_ALTO = 45;

static bool dentroDe(int x, int y, float rx, float ry, float rw, float rh)
{
	return x >= rx && x <= rx + rw && y >= ry && y <= ry + rh;
}

static string nombreCiudad(Grafo* grafo, const string& codigo)
{
	if (grafo == nullptr)
		return codigo;
	
	NodoVertice* ciudad = grafo->buscarCiudad(codigo);
	if (ciudad == nullptr)
		return codigo;
	
	return ciudad->ciudad.getNombre();
}

PantallaMisVuelos::PantallaMisVuelos(ListaPasajeros* listaPasajeros, ListaMaletas* listaMaletas, ListaVuelos* listaVuelos, ListaReservas* listaReservas, GestorPantallas* gestorPantallas, Grafo* grafo)
{
	this->listaPasajeros = listaPasajeros;
	this->listaMaletas = listaMaletas;
	this->listaVuelos = listaVuelos;
	this->listaReservas = listaReservas;
	this->gestorPantallas = gestorPantallas;
	this->grafo = grafo;
	fuenteCargada = fuente.loadFromFile("resources/times.ttf");
	cedula = "";
	mensaje = "";
	cursorVisible = true;
	tiempoCursor = 0;
}

void PantallaMisVuelos::procesarEventos(RenderWindow& ventana, Event evento)
{
	(void)ventana;
	
	if (evento.type == Event::MouseButtonPressed && evento.mouseButton.button == Mouse::Left)
	{
		if (dentroDe(evento.mouseButton.x, evento.mouseButton.y, CAMPO_X, BOTON_Y, CAMPO_ANCHO, BOTON_ALTO))
			buscar();
		return;
	}
	
	if (evento.type == Event::KeyPressed && evento.key.code == Keyboard::Return)
	{
		buscar();
		return;
	}
	
	if (evento.type == Event::KeyPressed && evento.key.code == Keyboard::Escape)
	{
		cedula = "";
		mensaje = "";
		return;
	}
	
	if (evento.type == Event::TextEntered)
	{
		if (evento.text.unicode == 8)
		{
			if (!cedula.empty())
				cedula.pop_back();
		}
		else if (evento.text.unicode >= 32 && evento.text.unicode < 127 && cedula.size() < 25)
			cedula += static_cast<char>(evento.text.unicode);
	}
}

void PantallaMisVuelos::actualizar(float tiempo)
{
	tiempoCursor += tiempo;
	if (tiempoCursor >= 0.5f)
	{
		cursorVisible = !cursorVisible;
		tiempoCursor = 0;
	}
}

void PantallaMisVuelos::buscar()
{
	if (listaPasajeros == nullptr || listaReservas == nullptr || listaVuelos == nullptr ||
		listaMaletas == nullptr || gestorPantallas == nullptr)
	{
		mensaje = "No se pudo consultar: faltan datos del sistema.";
		return;
	}
	
	if (cedula.empty())
	{
		mensaje = "Ingrese la cedula del pasajero.";
		return;
	}
	
	if (listaPasajeros->buscar(cedula) == nullptr)
	{
		mensaje = "No hay ningun pasajero registrado con esa cedula.";
		return;
	}
	
	vector<string> codigosVuelos;
	vector<string> detalles;
	float total = 0;
	
	for (NodoReserva* reserva = listaReservas->getPrimero(); reserva != nullptr; reserva = reserva->siguiente)
	{
		if (reserva->identificacionPasajero != cedula)
			continue;
		
		codigosVuelos.push_back(reserva->codigoVuelo);
		
		ostringstream detalle;
		NodoVuelo* nodo = listaVuelos->buscar(reserva->codigoVuelo);
		if (nodo == nullptr)
		{
			detalle << "Vuelo " << reserva->codigoVuelo << " | este vuelo ya no esta disponible";
		}
		else
		{
			const Vuelo& vuelo = nodo->vuelo;
			detalle << "Vuelo " << vuelo.getCodigo() << " | "
				<< nombreCiudad(grafo, vuelo.getCodigoOrigen()) << " -> "
				<< nombreCiudad(grafo, vuelo.getCodigoDestino())
				<< " | CRC " << fixed << setprecision(2) << vuelo.getCosto()
				<< " | " << setprecision(1) << vuelo.getDuracion() << " h";
			total += vuelo.getCosto();
		}
		detalles.push_back(detalle.str());
	}
	
	if (codigosVuelos.empty())
	{
		mensaje = "Ese pasajero no tiene vuelos reservados.";
		return;
	}
	
	gestorPantallas->solicitarCambioPantalla(new PantallaResumen(listaPasajeros, listaMaletas, listaVuelos, listaReservas, gestorPantallas, grafo, cedula, codigosVuelos, detalles, total, "", true));
}

void PantallaMisVuelos::dibujar(RenderWindow& ventana)
{
	ventana.setView(ventana.getDefaultView());
	Vector2i mouse = Mouse::getPosition(ventana);
	
	RectangleShape fondo(Vector2f(1200, 800));
	fondo.setPosition(300, 0);
	fondo.setFillColor(Color::White);
	ventana.draw(fondo);
	
	dibujarTexto(ventana, 335, 35, "MIS VUELOS", 28, Color(55, 70, 60));
	dibujarTexto(ventana, 335, 75, "CONSULTE SUS RESERVAS CON SU CEDULA", 14, Color(120, 125, 120));
	
	RectangleShape panel(Vector2f(PANEL_ANCHO, PANEL_ALTO));
	panel.setPosition(PANEL_X, PANEL_Y);
	panel.setFillColor(Color(248, 249, 247));
	panel.setOutlineThickness(1);
	panel.setOutlineColor(Color(215, 220, 215));
	ventana.draw(panel);
	
	RectangleShape encabezado(Vector2f(PANEL_ANCHO, 40));
	encabezado.setPosition(PANEL_X, PANEL_Y);
	encabezado.setFillColor(Color(90, 120, 95));
	ventana.draw(encabezado);
	dibujarTexto(ventana, PANEL_X + 17, PANEL_Y + 10, "CONSULTAR MIS VUELOS", 17, Color::White);
	
	dibujarTexto(ventana, CAMPO_X, CAMPO_Y - 25, "Cedula del pasajero", 14, Color(75, 80, 75));
	
	RectangleShape campo(Vector2f(CAMPO_ANCHO, CAMPO_ALTO));
	campo.setPosition(CAMPO_X, CAMPO_Y);
	campo.setFillColor(Color::White);
	campo.setOutlineThickness(2);
	campo.setOutlineColor(Color(90, 120, 95));
	ventana.draw(campo);
	
	string texto = cedula;
	if (cursorVisible)
		texto += "|";
	dibujarTexto(ventana, CAMPO_X + 10, CAMPO_Y + 8, texto, 15, Color::Black);
	
	bool encima = dentroDe(mouse.x, mouse.y, CAMPO_X, BOTON_Y, CAMPO_ANCHO, BOTON_ALTO);
	RectangleShape boton(Vector2f(CAMPO_ANCHO, BOTON_ALTO));
	boton.setPosition(CAMPO_X, BOTON_Y);
	boton.setFillColor(encima ? Color(45, 135, 90) : Color(35, 115, 75));
	ventana.draw(boton);
	
	if (fuenteCargada)
	{
		Text etiqueta;
		etiqueta.setFont(fuente);
		etiqueta.setString("VER MIS VUELOS");
		etiqueta.setCharacterSize(15);
		etiqueta.setFillColor(Color::White);
		FloatRect b = etiqueta.getLocalBounds();
		etiqueta.setPosition(CAMPO_X + CAMPO_ANCHO / 2 - b.width / 2 - b.left, BOTON_Y + BOTON_ALTO / 2 - b.height / 2 - b.top);
		ventana.draw(etiqueta);
	}
	
	if (!mensaje.empty())
		dibujarTexto(ventana, CAMPO_X, BOTON_Y + BOTON_ALTO + 18, mensaje, 14, Color(165, 85, 80));
	
	dibujarTexto(ventana, CAMPO_X, PANEL_Y + PANEL_ALTO + 18, "Se mostrara el detalle de sus vuelos, el total pagado y su equipaje.", 12, Color(120, 125, 120));
}

void PantallaMisVuelos::dibujarTexto(RenderWindow& ventana, float x, float y, string texto, int tamano, Color color)
{
	if (!fuenteCargada)
		return;
	
	Text etiqueta;
	etiqueta.setFont(fuente);
	etiqueta.setString(texto);
	etiqueta.setCharacterSize(tamano);
	etiqueta.setFillColor(color);
	etiqueta.setPosition(x, y);
	ventana.draw(etiqueta);
}
