#include "PantallaMiEquipaje.h"
#include <sstream>
#include <iomanip>
#include <cmath>

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

static const float LISTA_Y = 200;
static const float LISTA_PASO = 44;
static const int LISTA_FILAS = 10;
static const float RUTA_Y = 222;
static const float RUTA_PASO = 56;
static const int RUTA_FILAS = 5;
static const float OTRA_X = 350;
static const float OTRA_Y = 670;
static const float OTRA_ANCHO = 380;
static const float OTRA_ALTO = 45;

static bool dentroDe(int x, int y, float rx, float ry, float rw, float rh)
{
	return x >= rx && x <= rx + rw && y >= ry && y <= ry + rh;
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

static string nombreCiudad(Grafo* grafo, const string& codigo)
{
	if (grafo == nullptr)
		return codigo;
	
	NodoVertice* ciudad = grafo->buscarCiudad(codigo);
	return ciudad != nullptr ? ciudad->ciudad.getNombre() : codigo;
}

static string codigoCiudad(Grafo* grafo, const string& texto)
{
	if (grafo == nullptr)
		return "";
	
	for (NodoVertice* v = grafo->getPrimero(); v != nullptr; v = v->siguiente)
		if (v->ciudad.getNombre() == texto || v->ciudad.getCodigo() == texto)
			return v->ciudad.getCodigo();
	
	return "";
}

static void agregarUnico(vector<string>& lista, const string& valor)
{
	for (size_t i = 0; i < lista.size(); i++)
		if (lista[i] == valor)
			return;
	lista.push_back(valor);
}

static string dinero(float valor)
{
	ostringstream s;
	s << fixed << setprecision(2) << valor;
	return s.str();
}

static string horas(float valor)
{
	ostringstream s;
	s << fixed << setprecision(1) << valor;
	return s.str();
}

PantallaMiEquipaje::PantallaMiEquipaje(Grafo* grafo, ListaPasajeros* listaPasajeros, ListaMaletas* listaMaletas, ListaVuelos* listaVuelos, ListaReservas* listaReservas)
{
	this->grafo = grafo;
	this->listaPasajeros = listaPasajeros;
	this->listaMaletas = listaMaletas;
	this->listaVuelos = listaVuelos;
	this->listaReservas = listaReservas;
	fuenteCargada = fuente.loadFromFile("resources/times.ttf");
	etapa = 0;
	cedula = "";
	mensaje = "";
	nombrePasajero = "";
	maletaSeleccionada = -1;
	desplazamientoMaletas = 0;
	desplazamientoRuta = 0;
	cursorVisible = true;
	tiempoCursor = 0;
	hayRuta = false;
	mensajeRuta = "";
	costoMaleta = 0;
	duracionMaleta = 0;
	hayRutaPasajero = false;
	costoPasajero = 0;
	duracionPasajero = 0;
	comparacion = "";
}

void PantallaMiEquipaje::buscar()
{
	if (listaPasajeros == nullptr || listaMaletas == nullptr || listaVuelos == nullptr ||
		listaReservas == nullptr || grafo == nullptr)
	{
		mensaje = "No se pudo consultar: faltan datos del sistema.";
		return;
	}
	
	if (cedula.empty())
	{
		mensaje = "Ingrese la cedula del pasajero.";
		return;
	}
	
	NodoPasajero* pasajero = listaPasajeros->buscar(cedula);
	if (pasajero == nullptr)
	{
		mensaje = "No hay ningun pasajero registrado con esa cedula.";
		return;
	}
	
	codigosMaletas.clear();
	for (NodoMaleta* m = listaMaletas->getPrimero(); m != nullptr; m = m->siguiente)
		if (m->maleta.getIdentificacionPasajero() == cedula)
			codigosMaletas.push_back(m->maleta.getCodigo());
	
	if (codigosMaletas.empty())
	{
		mensaje = "Ese pasajero no tiene maletas registradas.";
		return;
	}
	
	nombrePasajero = pasajero->pasajero.getNombre();
	desplazamientoMaletas = 0;
	etapa = 1;
	mensaje = "";
	seleccionarMaleta(0);
}

void PantallaMiEquipaje::seleccionarMaleta(int indice)
{
	if (indice < 0 || indice >= static_cast<int>(codigosMaletas.size()))
		return;
	
	NodoMaleta* nodo = listaMaletas->buscar(codigosMaletas[indice]);
	if (nodo == nullptr)
		return;
	
	maletaSeleccionada = indice;
	calcularRutas(nodo->maleta);
}

void PantallaMiEquipaje::calcularRutas(const Maleta& maleta)
{
	hayRuta = false;
	mensajeRuta = "";
	ciudadesMaleta.clear();
	vuelosMaleta.clear();
	costoMaleta = 0;
	duracionMaleta = 0;
	hayRutaPasajero = false;
	ciudadesPasajero.clear();
	vuelosPasajero.clear();
	costoPasajero = 0;
	duracionPasajero = 0;
	comparacion = "";
	desplazamientoRuta = 0;
	
	string destino = codigoCiudad(grafo, maleta.getDestino());
	if (destino.empty())
	{
		mensajeRuta = "No se reconoce el destino de la maleta (" + maleta.getDestino() + "). Verifique que la ciudad exista.";
		return;
	}
	
	vector<NodoVuelo*> reservados;
	for (NodoReserva* r = listaReservas->getPrimero(); r != nullptr; r = r->siguiente)
	{
		if (r->identificacionPasajero != cedula)
			continue;
		NodoVuelo* nodo = listaVuelos->buscar(r->codigoVuelo);
		if (nodo != nullptr)
			reservados.push_back(nodo);
	}
	
	if (reservados.empty())
	{
		mensajeRuta = "Este pasajero no tiene vuelos reservados, asi que no se puede saber desde donde sale la maleta.";
		return;
	}
	
	vector<string> candidatos;
	for (size_t i = 0; i < reservados.size(); i++)
	{
		string origen = reservados[i]->vuelo.getCodigoOrigen();
		if (origen == destino)
			continue;
		bool esInicio = true;
		for (size_t j = 0; j < reservados.size(); j++)
			if (j != i && reservados[j]->vuelo.getCodigoDestino() == origen)
				esInicio = false;
		if (esInicio)
			agregarUnico(candidatos, origen);
	}
	if (candidatos.empty())
		for (size_t i = 0; i < reservados.size(); i++)
			if (reservados[i]->vuelo.getCodigoOrigen() != destino)
				agregarUnico(candidatos, reservados[i]->vuelo.getCodigoOrigen());
	
	if (candidatos.empty())
	{
		mensajeRuta = "La maleta ya esta en su ciudad de destino.";
		return;
	}
	
	string origenUsado;
	for (size_t i = 0; i < candidatos.size() && !hayRuta; i++)
	{
		string* ruta = nullptr;
		string* codigos = nullptr;
		int cantidad = 0;
		float costo = 0;
		float duracion = 0;
		
		if (dijkstra.calcular(*grafo, candidatos[i], destino, true, ruta, cantidad, costo, duracion, codigos))
		{
			for (int k = 0; k < cantidad; k++)
				ciudadesMaleta.push_back(ruta[k]);
			for (int k = 0; k < cantidad - 1; k++)
				vuelosMaleta.push_back(codigos[k]);
			costoMaleta = costo;
			duracionMaleta = duracion;
			origenUsado = candidatos[i];
			hayRuta = true;
		}
		delete[] ruta;
		delete[] codigos;
	}
	
	if (!hayRuta)
	{
		mensajeRuta = "No existe una ruta con vuelos disponibles hasta " + maleta.getDestino() + ".";
		return;
	}
	
	vector<bool> usado(reservados.size(), false);
	string actual = origenUsado;
	bool llego = false;
	for (size_t paso = 0; paso < reservados.size() && !llego; paso++)
	{
		int elegido = -1;
		for (size_t j = 0; j < reservados.size(); j++)
			if (!usado[j] && reservados[j]->vuelo.getCodigoOrigen() == actual)
		{
				elegido = static_cast<int>(j);
				break;
		}
			if (elegido == -1)
				break;
			
			usado[elegido] = true;
			const Vuelo& v = reservados[elegido]->vuelo;
			if (ciudadesPasajero.empty())
				ciudadesPasajero.push_back(nombreCiudad(grafo, actual));
			ciudadesPasajero.push_back(nombreCiudad(grafo, v.getCodigoDestino()));
			vuelosPasajero.push_back(v.getCodigo());
			costoPasajero += v.getCosto();
			duracionPasajero += v.getDuracion();
			actual = v.getCodigoDestino();
			if (actual == destino)
				llego = true;
	}
	hayRutaPasajero = llego;
	
	if (!hayRutaPasajero)
	{
		comparacion = "No se pudo reconstruir tu ruta hasta ese destino para compararla.";
		return;
	}
	
	if (vuelosPasajero == vuelosMaleta)
	{
		comparacion = "Tu maleta viaja contigo por la misma ruta, que ya es la de menor costo.";
		return;
	}
	
	float ahorro = costoPasajero - costoMaleta;
	float diferenciaHoras = duracionMaleta - duracionPasajero;
	
	comparacion = "Tu maleta viaja por la ruta mas barata, no por la tuya. ";
	if (ahorro > 0.005f)
		comparacion += "Cuesta CRC " + dinero(ahorro) + " menos que tu ruta. ";
	else
		comparacion += "Cuesta lo mismo que tu ruta. ";
	
	if (diferenciaHoras > 0.05f)
		comparacion += "Tarda " + horas(diferenciaHoras) + " h mas que tu, asi que podria llegar despues que tu.";
	else if (diferenciaHoras < -0.05f)
		comparacion += "Tarda " + horas(-diferenciaHoras) + " h menos que tu.";
	else
		comparacion += "Tarda lo mismo que tu.";
}

void PantallaMiEquipaje::procesarEventos(RenderWindow& ventana, Event evento)
{
	(void)ventana;
	
	if (etapa == 0)
	{
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
		return;
	}
	
	if (evento.type == Event::MouseWheelScrolled)
	{
		int x = evento.mouseWheelScroll.x;
		int y = evento.mouseWheelScroll.y;
		int paso = evento.mouseWheelScroll.delta < 0 ? 1 : -1;
		
		if (dentroDe(x, y, 325, LISTA_Y, 430, LISTA_FILAS * LISTA_PASO))
		{
			int maximo = static_cast<int>(codigosMaletas.size()) - LISTA_FILAS;
			if (maximo < 0)
				maximo = 0;
			desplazamientoMaletas += paso;
			if (desplazamientoMaletas < 0)
				desplazamientoMaletas = 0;
			if (desplazamientoMaletas > maximo)
				desplazamientoMaletas = maximo;
		}
		else if (dentroDe(x, y, 785, RUTA_Y, 650, RUTA_FILAS * RUTA_PASO))
		{
			int maximo = static_cast<int>(ciudadesMaleta.size()) - RUTA_FILAS;
			if (maximo < 0)
				maximo = 0;
			desplazamientoRuta += paso;
			if (desplazamientoRuta < 0)
				desplazamientoRuta = 0;
			if (desplazamientoRuta > maximo)
				desplazamientoRuta = maximo;
		}
		return;
	}
	
	if (evento.type == Event::MouseButtonPressed && evento.mouseButton.button == Mouse::Left)
	{
		int x = evento.mouseButton.x;
		int y = evento.mouseButton.y;
		
		if (dentroDe(x, y, OTRA_X, OTRA_Y, OTRA_ANCHO, OTRA_ALTO))
		{
			etapa = 0;
			cedula = "";
			mensaje = "";
			return;
		}
		
		if (dentroDe(x, y, 340, LISTA_Y, 400, LISTA_FILAS * LISTA_PASO))
		{
			int indice = desplazamientoMaletas + static_cast<int>((y - LISTA_Y) / LISTA_PASO);
			seleccionarMaleta(indice);
		}
	}
}

void PantallaMiEquipaje::actualizar(float tiempo)
{
	tiempoCursor += tiempo;
	if (tiempoCursor >= 0.5f)
	{
		cursorVisible = !cursorVisible;
		tiempoCursor = 0;
	}
}

void PantallaMiEquipaje::dibujar(RenderWindow& ventana)
{
	ventana.setView(ventana.getDefaultView());
	
	RectangleShape fondo(Vector2f(1200, 800));
	fondo.setPosition(300, 0);
	fondo.setFillColor(Color::White);
	ventana.draw(fondo);
	
	dibujarTexto(ventana, 335, 35, "MI EQUIPAJE", 28, Color(55, 70, 60));
	
	if (etapa == 0)
	{
		dibujarTexto(ventana, 335, 75, "CONSULTE LA RUTA DE SUS MALETAS CON SU CEDULA", 14, Color(120, 125, 120));
		dibujarConsulta(ventana);
		return;
	}
	
	dibujarTexto(ventana, 335, 75, "RUTA QUE TOMARA CADA MALETA - SOLO CONSULTA", 14, Color(120, 125, 120));
	dibujarMaletas(ventana);
	dibujarRutaMaleta(ventana);
}

void PantallaMiEquipaje::dibujarConsulta(RenderWindow& ventana)
{
	Vector2i mouse = Mouse::getPosition(ventana);
	
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
	dibujarTexto(ventana, PANEL_X + 17, PANEL_Y + 10, "CONSULTAR MI EQUIPAJE", 17, Color::White);
	
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
		etiqueta.setString("VER LA RUTA DE MIS MALETAS");
		etiqueta.setCharacterSize(15);
		etiqueta.setFillColor(Color::White);
		FloatRect b = etiqueta.getLocalBounds();
		etiqueta.setPosition(CAMPO_X + CAMPO_ANCHO / 2 - b.width / 2 - b.left, BOTON_Y + BOTON_ALTO / 2 - b.height / 2 - b.top);
		ventana.draw(etiqueta);
	}
	
	if (!mensaje.empty())
		dibujarTexto(ventana, CAMPO_X, BOTON_Y + BOTON_ALTO + 18, mensaje, 14, Color(165, 85, 80));
	
	dibujarTexto(ventana, CAMPO_X, PANEL_Y + PANEL_ALTO + 18, "Las maletas siempre viajan por la ruta de menor costo.", 12, Color(120, 125, 120));
}

void PantallaMiEquipaje::dibujarMaletas(RenderWindow& ventana)
{
	Vector2i mouse = Mouse::getPosition(ventana);
	
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
	dibujarTexto(ventana, 342, 125, "MIS MALETAS", 17, Color::White);
	
	if (fuenteCargada)
		dibujarTexto(ventana, 342, 163, recortarTexto(fuente, nombrePasajero + "  (" + cedula + ")", 13, 395), 13, Color(75, 80, 75));
	
	for (int fila = 0; fila < LISTA_FILAS; fila++)
	{
		int indice = desplazamientoMaletas + fila;
		if (indice >= static_cast<int>(codigosMaletas.size()))
			break;
		
		NodoMaleta* nodo = listaMaletas->buscar(codigosMaletas[indice]);
		if (nodo == nullptr)
			continue;
		
		float y = LISTA_Y + fila * LISTA_PASO;
		bool seleccionada = indice == maletaSeleccionada;
		bool encima = dentroDe(mouse.x, mouse.y, 340, y, 400, LISTA_PASO - 4);
		
		RectangleShape tarjeta(Vector2f(400, LISTA_PASO - 4));
		tarjeta.setPosition(340, y);
		tarjeta.setFillColor(seleccionada ? Color(225, 237, 225) : (encima ? Color(240, 244, 240) : Color(250, 251, 249)));
		tarjeta.setOutlineThickness(1);
		tarjeta.setOutlineColor(seleccionada ? Color(90, 120, 95) : Color(225, 230, 224));
		ventana.draw(tarjeta);
		
		ostringstream detalle;
		detalle << nodo->maleta.getPeso() << " kg  |  " << nodo->maleta.getDestino() << "  |  " << nodo->maleta.getEstado();
		dibujarTexto(ventana, 350, y + 4, nodo->maleta.getCodigo(), 13, Color(35, 65, 52));
		if (fuenteCargada)
			dibujarTexto(ventana, 350, y + 22, recortarTexto(fuente, detalle.str(), 11, 380), 11, Color(105, 115, 108));
	}
	
	if (static_cast<int>(codigosMaletas.size()) > LISTA_FILAS)
		dibujarTexto(ventana, 342, 645, "Usa la rueda para ver mas maletas", 11, Color(105, 115, 108));
	
	bool hOtra = dentroDe(mouse.x, mouse.y, OTRA_X, OTRA_Y, OTRA_ANCHO, OTRA_ALTO);
	RectangleShape boton(Vector2f(OTRA_ANCHO, OTRA_ALTO));
	boton.setPosition(OTRA_X, OTRA_Y);
	boton.setFillColor(hOtra ? Color(110, 140, 115) : Color(120, 130, 120));
	ventana.draw(boton);
	
	if (fuenteCargada)
	{
		Text etiqueta;
		etiqueta.setFont(fuente);
		etiqueta.setString("OTRA CEDULA");
		etiqueta.setCharacterSize(14);
		etiqueta.setFillColor(Color::White);
		FloatRect b = etiqueta.getLocalBounds();
		etiqueta.setPosition(OTRA_X + OTRA_ANCHO / 2 - b.width / 2 - b.left, OTRA_Y + OTRA_ALTO / 2 - b.height / 2 - b.top);
		ventana.draw(etiqueta);
	}
}

void PantallaMiEquipaje::dibujarRutaMaleta(RenderWindow& ventana)
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
	dibujarTexto(ventana, 790, 125, "RUTA DE LA MALETA", 17, Color::White);
	
	NodoMaleta* nodo = maletaSeleccionada >= 0 && maletaSeleccionada < static_cast<int>(codigosMaletas.size()) ? listaMaletas->buscar(codigosMaletas[maletaSeleccionada]) : nullptr;
	
	if (nodo != nullptr)
	{
		ostringstream info;
		info << "Maleta " << nodo->maleta.getCodigo() << "  |  " << nodo->maleta.getPeso() << " kg  |  Destino: " << nodo->maleta.getDestino();
		if (fuenteCargada)
			dibujarTexto(ventana, 790, 168, recortarTexto(fuente, info.str(), 14, 640), 14, Color(55, 70, 60));
	}
	dibujarTexto(ventana, 790, 192, "Ruta de menor costo: las maletas siempre viajan por la ruta mas barata.", 12, Color(35, 95, 60));
	
	if (!hayRuta)
	{
		vector<string> lineas = fuenteCargada ? envolverTexto(fuente, mensajeRuta, 14, 630) : vector<string>(1, mensajeRuta);
		for (size_t i = 0; i < lineas.size() && i < 4; i++)
			dibujarTexto(ventana, 790, 240 + i * 22, lineas[i], 14, Color(165, 85, 80));
		return;
	}
	
	int total = static_cast<int>(ciudadesMaleta.size());
	int inicio = desplazamientoRuta;
	if (inicio > total)
		inicio = total;
	
	for (int fila = 0; fila < RUTA_FILAS; fila++)
	{
		int indice = inicio + fila;
		if (indice >= total)
			break;
		
		float y = RUTA_Y + fila * RUTA_PASO;
		bool esUltima = indice == total - 1;
		bool esPrimera = indice == 0;
		
		if (!esUltima)
		{
			RectangleShape linea(Vector2f(3, RUTA_PASO));
			linea.setPosition(798, y + 16);
			linea.setFillColor(Color(190, 205, 192));
			ventana.draw(linea);
		}
		
		CircleShape punto(9);
		punto.setPosition(791, y + 5);
		punto.setFillColor(esPrimera ? Color(35, 115, 75) : (esUltima ? Color(165, 85, 80) : Color(90, 120, 95)));
		ventana.draw(punto);
		
		dibujarTexto(ventana, 822, y + 2, ciudadesMaleta[indice], 15, Color::Black);
		dibujarTexto(ventana, 1340, y + 5, esPrimera ? "ORIGEN" : (esUltima ? "DESTINO" : "ESCALA"), 11, Color(120, 125, 120));
		
		if (!esUltima && indice < static_cast<int>(vuelosMaleta.size()))
		{
			string detalle = "Vuelo " + vuelosMaleta[indice];
			NodoVuelo* vuelo = listaVuelos->buscar(vuelosMaleta[indice]);
			if (vuelo != nullptr)
				detalle += "  |  CRC " + dinero(vuelo->vuelo.getCosto()) + "  |  " + horas(vuelo->vuelo.getDuracion()) + " h";
			dibujarTexto(ventana, 822, y + 24, detalle, 12, Color(105, 115, 108));
		}
	}
	
	if (total > RUTA_FILAS)
		dibujarTexto(ventana, 1290, 195, "Rueda: ver mas", 11, Color(105, 115, 108));
	
	RectangleShape separador(Vector2f(650, 1));
	separador.setPosition(785, 508);
	separador.setFillColor(Color(210, 218, 210));
	ventana.draw(separador);
	
	int escalas = total > 2 ? total - 2 : 0;
	dibujarTexto(ventana, 790, 518, "TOTAL DE LA MALETA", 13, Color(55, 70, 60));
	dibujarTexto(ventana, 790, 540, "CRC " + dinero(costoMaleta) + "   |   " + horas(duracionMaleta) + " h   |   " +  to_string(escalas) + " escala(s)", 16, Color(35, 95, 60));
	dibujarComparacion(ventana);
}

void PantallaMiEquipaje::dibujarComparacion(RenderWindow& ventana)
{
	RectangleShape separador(Vector2f(650, 1));
	separador.setPosition(785, 578);
	separador.setFillColor(Color(210, 218, 210));
	ventana.draw(separador);
	
	dibujarTexto(ventana, 790, 588, "TU RUTA COMO PASAJERO", 13, Color(55, 70, 60));
	
	float y = 612;
	if (hayRutaPasajero)
	{
		string recorrido;
		for (size_t i = 0; i < ciudadesPasajero.size(); i++)
			recorrido += (i > 0 ? " -> " : "") + ciudadesPasajero[i];
		
		if (fuenteCargada)
			dibujarTexto(ventana, 790, y, recortarTexto(fuente, recorrido, 13, 640), 13, Color::Black);
		dibujarTexto(ventana, 790, y + 20, "CRC " + dinero(costoPasajero) + "   |   " + horas(duracionPasajero) + " h", 13, Color(75, 80, 75));
		y += 50;
	}
	
	vector<string> lineas = fuenteCargada ? envolverTexto(fuente, comparacion, 13, 640) : vector<string>(1, comparacion);
	for (size_t i = 0; i < lineas.size() && i < 4; i++)
		dibujarTexto(ventana, 790, y + i * 19, lineas[i], 13, Color(35, 95, 60));
}

void PantallaMiEquipaje::dibujarTexto(RenderWindow& ventana, float x, float y, string texto, int tamano, Color color)
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
