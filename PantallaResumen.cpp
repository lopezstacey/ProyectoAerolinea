#include "PantallaResumen.h"
#include "PantallaInicio.h"
#include "PantallaBuscarRutas.h"
#include "PantallaMisVuelos.h"
#include <sstream>
#include <iomanip>

static const float BTN_NUEVA_X = 350;
static const float BTN_FINALIZAR_X = 545;
static const float BTN_Y = 670;
static const float BTN_ANCHO = 185;
static const float BTN_ALTO = 45;
static const int FILAS_VISIBLES = 5;

static bool dentroDe(int x, int y, float rx, float ry, float rw, float rh)
{
	return x >= rx && x <= rx + rw && y >= ry && y <= ry + rh;
}

static vector<string> envolverTexto(const Font& fuente, const string& texto, int tamano, float anchoMaximo)
{
	vector<string> lineas;
	istringstream palabras(texto);
	string palabra, linea;
	while (palabras >> palabra)
	{
		string candidata = linea.empty() ? palabra : linea + " " + palabra;
		Text medida(candidata, fuente, tamano);
		if (!linea.empty() && medida.getLocalBounds().width > anchoMaximo)
		{
			lineas.push_back(linea);
			linea = palabra;
		}
		else
			linea = candidata;
	}
	if (!linea.empty())
		lineas.push_back(linea);
	return lineas;
}

static string recortarTexto(const Font& fuente, const string& texto, int tamano, float maxAncho)
{
	Text medida;
	medida.setFont(fuente);
	medida.setCharacterSize(tamano);
	medida.setString(texto);
	
	if (medida.getLocalBounds().width <= maxAncho)
		return texto;
	
	string recortado = texto;
	while (!recortado.empty())
	{
		recortado.erase(recortado.size() - 1);
		medida.setString(recortado + "...");
		if (medida.getLocalBounds().width <= maxAncho)
			break;
	}
	return recortado + "...";
}

static void textoCentrado(RenderWindow& ventana, const Font& fuente, const string& texto, float cx, float cy, int tamano, Color color)
{
	Text t;
	t.setFont(fuente);
	t.setString(texto);
	t.setCharacterSize(tamano);
	t.setFillColor(color);
	FloatRect b = t.getLocalBounds();
	t.setPosition(cx - b.width / 2 - b.left, cy - b.height / 2 - b.top);
	ventana.draw(t);
}

PantallaResumen::PantallaResumen(ListaPasajeros* listaPasajeros, ListaMaletas* listaMaletas, ListaVuelos* listaVuelos, ListaReservas* listaReservas, GestorPantallas* gestorPantallas, Grafo* grafo,
								 string identificacion, vector<string> codigosVuelos, vector<string> detallesRutas, float costoTotal, string codigoMaleta, bool modoConsulta)
{
	this->listaPasajeros = listaPasajeros;
	this->listaMaletas = listaMaletas;
	this->listaVuelos = listaVuelos;
	this->listaReservas = listaReservas;
	this->gestorPantallas = gestorPantallas;
	this->grafo = grafo;
	this->identificacion = identificacion;
	this->codigosVuelos = codigosVuelos;
	this->detallesRutas = detallesRutas;
	this->costoTotal = costoTotal;
	this->codigoMaleta = codigoMaleta;
	this->modoConsulta = modoConsulta;
	desplazamiento = 0;
	fuenteCargada = fuente.loadFromFile("resources/times.ttf");
}

void PantallaResumen::procesarEventos(RenderWindow& ventana, Event evento)
{
	(void)ventana;
	
	if (evento.type == Event::MouseWheelScrolled)
	{
		int x = evento.mouseWheelScroll.x;
		int y = evento.mouseWheelScroll.y;
		if (dentroDe(x, y, 770, 190, 680, 430))
		{
			int maximo = static_cast<int>(detallesRutas.size()) - FILAS_VISIBLES;
			if (maximo < 0)
				maximo = 0;
			if (evento.mouseWheelScroll.delta < 0 && desplazamiento < maximo)
				desplazamiento++;
			else if (evento.mouseWheelScroll.delta > 0 && desplazamiento > 0)
				desplazamiento--;
		}
		return;
	}
	
	if (evento.type != Event::MouseButtonPressed || evento.mouseButton.button != Mouse::Left)
		return;
	
	int x = evento.mouseButton.x;
	int y = evento.mouseButton.y;
	
	if (gestorPantallas == nullptr)
		return;
	
	if (dentroDe(x, y, BTN_NUEVA_X, BTN_Y, BTN_ANCHO, BTN_ALTO))
	{
		if (modoConsulta)
			gestorPantallas->solicitarCambioPantalla(new PantallaMisVuelos(listaPasajeros, listaMaletas, listaVuelos, listaReservas, gestorPantallas, grafo));
		else
			gestorPantallas->solicitarCambioPantalla(new PantallaBuscarRutas(grafo, listaPasajeros, listaMaletas, listaVuelos, listaReservas, gestorPantallas));
		return;
	}
	if (dentroDe(x, y, BTN_FINALIZAR_X, BTN_Y, BTN_ANCHO, BTN_ALTO))
	{
		gestorPantallas->solicitarCambioPantalla(new PantallaInicio());
		return;
	}
}

void PantallaResumen::actualizar(float tiempo)
{
	(void)tiempo;
}

void PantallaResumen::dibujar(RenderWindow& ventana)
{
	ventana.setView(ventana.getDefaultView());
	
	RectangleShape fondo(Vector2f(1200, 800));
	fondo.setPosition(300, 0);
	fondo.setFillColor(Color::White);
	ventana.draw(fondo);
	
	dibujarTexto(ventana, 335, 35, modoConsulta ? "MIS VUELOS" : "RESUMEN DE LA RESERVA", 28, Color(55, 70, 60));
	dibujarTexto(ventana, 335, 75, modoConsulta ? "VUELOS Y EQUIPAJE DEL PASAJERO - SOLO CONSULTA" : "RESERVA CONFIRMADA - SOLO CONSULTA", 14, Color(120, 125, 120));
	
	dibujarPasajero(ventana);
	dibujarEquipaje(ventana);
	dibujarItinerarios(ventana);
	dibujarBotones(ventana);
}

void PantallaResumen::dibujarCampo(RenderWindow& ventana, float x, float y, string etiqueta, string valor)
{
	dibujarTexto(ventana, x, y, etiqueta, 12, Color(120, 125, 120));
	
	if (fuenteCargada)
	{
		Text medida;
		medida.setFont(fuente);
		medida.setCharacterSize(15);
		string texto = valor;
		medida.setString(texto);
		while (!texto.empty() && medida.getLocalBounds().width > 370)
		{
			texto.erase(texto.size() - 1);
			medida.setString(texto + "...");
		}
		if (texto.size() != valor.size())
			texto += "...";
		dibujarTexto(ventana, x, y + 16, texto, 15, Color::Black);
	}
}

void PantallaResumen::dibujarPasajero(RenderWindow& ventana)
{
	RectangleShape panel(Vector2f(430, 620));
	panel.setPosition(325, 115);
	panel.setFillColor(Color(248, 249, 247));
	panel.setOutlineThickness(1);
	panel.setOutlineColor(Color(215, 220, 215));
	ventana.draw(panel);
	
	RectangleShape encabezado(Vector2f(430, 40));
	encabezado.setPosition(325, 115);
	encabezado.setFillColor(Color(90, 120, 95));
	ventana.draw(encabezado);
	dibujarTexto(ventana, 342, 125, "DATOS DEL PASAJERO", 17, Color::White);
	
	string nombre = "-", telefono = "-", correo = "-";
	NodoPasajero* pasajero = listaPasajeros != nullptr ? listaPasajeros->buscar(identificacion) : nullptr;
	if (pasajero != nullptr)
	{
		nombre = pasajero->pasajero.getNombre();
		telefono = pasajero->pasajero.getTelefono();
		correo = pasajero->pasajero.getCorreo();
	}
	
	dibujarCampo(ventana, 350, 170, "Identificacion", identificacion);
	dibujarCampo(ventana, 350, 218, "Nombre completo", nombre);
	dibujarCampo(ventana, 350, 266, "Telefono", telefono);
	dibujarCampo(ventana, 350, 314, "Correo electronico", correo);
}

void PantallaResumen::dibujarEquipaje(RenderWindow& ventana)
{
	RectangleShape encabezado(Vector2f(430, 40));
	encabezado.setPosition(325, 380);
	encabezado.setFillColor(Color(90, 120, 95));
	ventana.draw(encabezado);
	dibujarTexto(ventana, 342, 390, "EQUIPAJE", 17, Color::White);
	
	if (modoConsulta)
	{
		const Color gris(120, 125, 120);
		dibujarTexto(ventana, 350, 432, "CODIGO", 11, gris);
		dibujarTexto(ventana, 440, 432, "PESO", 11, gris);
		dibujarTexto(ventana, 520, 432, "DESTINO", 11, gris);
		dibujarTexto(ventana, 640, 432, "ESTADO", 11, gris);
		
		const int maximoFilas = 5;
		int fila = 0;
		int total = 0;
		NodoMaleta* actual = listaMaletas != nullptr ? listaMaletas->getPrimero() : nullptr;
		for (; actual != nullptr; actual = actual->siguiente)
		{
			if (actual->maleta.getIdentificacionPasajero() != identificacion)
				continue;
			total++;
			if (fila >= maximoFilas)
				continue;
			
			float y = 452 + fila * 30;
			ostringstream peso;
			peso << actual->maleta.getPeso() << " kg";
			if (fuenteCargada)
			{
				dibujarTexto(ventana, 350, y, recortarTexto(fuente, actual->maleta.getCodigo(), 12, 85), 12, Color::Black);
				dibujarTexto(ventana, 440, y, recortarTexto(fuente, peso.str(), 12, 70), 12, Color::Black);
				dibujarTexto(ventana, 520, y, recortarTexto(fuente, actual->maleta.getDestino(), 12, 110), 12, Color::Black);
				dibujarTexto(ventana, 640, y, recortarTexto(fuente, actual->maleta.getEstado(), 12, 100), 12, Color::Black);
			}
			fila++;
		}
		
		if (total == 0)
			dibujarTexto(ventana, 350, 455, "El pasajero no tiene maletas registradas.", 14, Color(75, 80, 75));
		else if (total > maximoFilas)
			dibujarTexto(ventana, 350, 452 + maximoFilas * 30, "y " + to_string(total - maximoFilas) + " maleta(s) mas", 11, gris);
		return;
	}
	
	NodoMaleta* maleta = nullptr;
	if (!codigoMaleta.empty() && listaMaletas != nullptr)
		maleta = listaMaletas->buscar(codigoMaleta);
	
	if (maleta == nullptr)
	{
		dibujarTexto(ventana, 350, 440, "El pasajero no lleva maleta en esta reserva.", 14, Color(75, 80, 75));
		return;
	}
	
	ostringstream peso;
	peso << maleta->maleta.getPeso() << " kg";
	
	dibujarCampo(ventana, 350, 435, "Codigo de maleta", maleta->maleta.getCodigo());
	dibujarCampo(ventana, 350, 483, "Peso", peso.str());
	dibujarCampo(ventana, 350, 531, "Destino", maleta->maleta.getDestino());
	dibujarCampo(ventana, 350, 579, "Estado", maleta->maleta.getEstado());
}

void PantallaResumen::dibujarItinerarios(RenderWindow& ventana)
{
	RectangleShape panel(Vector2f(680, 620));
	panel.setPosition(770, 115);
	panel.setFillColor(Color(248, 249, 247));
	panel.setOutlineThickness(1);
	panel.setOutlineColor(Color(215, 220, 215));
	ventana.draw(panel);
	
	RectangleShape encabezado(Vector2f(680, 40));
	encabezado.setPosition(770, 115);
	encabezado.setFillColor(Color(90, 120, 95));
	ventana.draw(encabezado);
	dibujarTexto(ventana, 790, 125, "VUELOS, ESCALAS Y FACTURA", 17, Color::White);
	dibujarTexto(ventana, 790, 170, modoConsulta ? "Vuelos reservados" : "Itinerarios confirmados", 15, Color(55, 70, 60));
	
	int inicio = desplazamiento;
	if (inicio < 0)
		inicio = 0;
	if (inicio > static_cast<int>(detallesRutas.size()))
		inicio = static_cast<int>(detallesRutas.size());
	
	for (int fila = 0; fila < FILAS_VISIBLES; fila++)
	{
		int indice = inicio + fila;
		if (indice >= static_cast<int>(detallesRutas.size()))
			break;
		
		float y = 198 + fila * 80;
		RectangleShape tarjeta(Vector2f(650, 72));
		tarjeta.setPosition(785, y);
		tarjeta.setFillColor(fila % 2 == 0 ? Color(242, 245, 241) : Color(250, 251, 249));
		tarjeta.setOutlineThickness(1);
		tarjeta.setOutlineColor(Color(225, 230, 224));
		ventana.draw(tarjeta);
		
		dibujarTexto(ventana, 798, y + 6, (modoConsulta ? "Vuelo " : "Ruta ") + to_string(indice + 1), 12, Color(55, 95, 65));
		vector<string> lineas;
		if (fuenteCargada)
			lineas = envolverTexto(fuente, detallesRutas[indice], 12, 610);
		else
			lineas.push_back(detallesRutas[indice]);
		for (int linea = 0; linea < 3 && linea < static_cast<int>(lineas.size()); linea++)
			dibujarTexto(ventana, 798, y + 24 + linea * 14, lineas[linea], 12, Color(55, 65, 58));
	}
	
	if (static_cast<int>(detallesRutas.size()) > FILAS_VISIBLES)
		dibujarTexto(ventana, 1110, 605, modoConsulta ? "Usa la rueda para ver mas vuelos" : "Usa la rueda para ver mas rutas", 11, Color(105, 115, 108));
	
	RectangleShape separador(Vector2f(650, 1));
	separador.setPosition(785, 632);
	separador.setFillColor(Color(210, 218, 210));
	ventana.draw(separador);
	
	dibujarTexto(ventana, 790, 642, "DETALLE DE FACTURA", 14, Color(55, 70, 60));
	int totalMaletas = codigoMaleta.empty() ? 0 : 1;
	if (modoConsulta)
	{
		totalMaletas = 0;
		for (NodoMaleta* m = listaMaletas != nullptr ? listaMaletas->getPrimero() : nullptr; m != nullptr; m = m->siguiente)
			if (m->maleta.getIdentificacionPasajero() == identificacion)
				totalMaletas++;
		dibujarTexto(ventana, 790, 670, "Vuelos: " + to_string(codigosVuelos.size()) + "    Maletas: " + to_string(totalMaletas), 12, Color(75, 80, 75));
	}
	else
		dibujarTexto(ventana, 790, 670, "Rutas: " + to_string(detallesRutas.size()) + "    Vuelos: " + to_string(codigosVuelos.size()) +
					 "    Maletas: " + to_string(totalMaletas), 12, Color(75, 80, 75));
	
	ostringstream total;
	total << fixed << setprecision(2) << costoTotal;
	dibujarTexto(ventana, 790, 694, (modoConsulta ? "Total de sus vuelos: CRC " : "Total pagado: CRC ") + total.str(), 16, Color(35, 95, 60));
}

void PantallaResumen::dibujarBotones(RenderWindow& ventana)
{
	Vector2i mouse = Mouse::getPosition(ventana);
	
	bool hNueva = dentroDe(mouse.x, mouse.y, BTN_NUEVA_X, BTN_Y, BTN_ANCHO, BTN_ALTO);
	RectangleShape botonNueva(Vector2f(BTN_ANCHO, BTN_ALTO));
	botonNueva.setPosition(BTN_NUEVA_X, BTN_Y);
	botonNueva.setFillColor(hNueva ? Color(110, 140, 115) : Color(120, 130, 120));
	ventana.draw(botonNueva);
	
	bool hFinalizar = dentroDe(mouse.x, mouse.y, BTN_FINALIZAR_X, BTN_Y, BTN_ANCHO, BTN_ALTO);
	RectangleShape botonFinalizar(Vector2f(BTN_ANCHO, BTN_ALTO));
	botonFinalizar.setPosition(BTN_FINALIZAR_X, BTN_Y);
	botonFinalizar.setFillColor(hFinalizar ? Color(45, 135, 90) : Color(35, 115, 75));
	ventana.draw(botonFinalizar);
	
	if (fuenteCargada)
	{
		textoCentrado(ventana, fuente, modoConsulta ? "OTRA CEDULA" : "NUEVA BUSQUEDA", BTN_NUEVA_X + BTN_ANCHO / 2, BTN_Y + BTN_ALTO / 2, 14, Color::White);
		textoCentrado(ventana, fuente, "FINALIZAR", BTN_FINALIZAR_X + BTN_ANCHO / 2, BTN_Y + BTN_ALTO / 2, 14, Color::White);
	}
}

void PantallaResumen::dibujarTexto(RenderWindow& ventana, float x, float y, string texto, int tamano, Color color)
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
