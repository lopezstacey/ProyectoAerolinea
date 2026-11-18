#include "PantallaBuscarRutas.h"
#include "PantallaPasajeros.h"
#include "GestorPantallas.h"
#include "ListaPasajeros.h"
#include "ListaMaletas.h"
#include "ListaVuelos.h"
#include "ListaReservas.h"
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <cmath>
#include <vector>

namespace
{
	const float X0 = 320.0f;
	const float ENC_Y = 14.0f;
	const float ENC_ALTO = 62.0f;
	const float ENC_ANCHO = 1160.0f;
	const float PY = 88.0f;
	const float PH = 692.0f;
	
	const float PANEL_IZQ_ANCHO = 260.0f;
	const float IZQ_X = 334.0f;
	const float IZQ_ANCHO = 232.0f;
	const float CAMPO_ALTO = 52.0f;
	const float ORIGEN_Y = 102.0f;
	const float DESTINO_Y = 162.0f;
	const float CRITERIO_Y = 226.0f;
	const float BOTON_ALTO = 40.0f;
	const float BOTON_MITAD = 112.0f;
	const float COSTO_X = 334.0f;
	const float RAPIDO_X = 454.0f;
	const float ACCION_Y = 274.0f;
	const float BUSCAR_X = 334.0f;
	const float BUSCAR_ANCHO = 140.0f;
	const float LIMPIAR_X = 482.0f;
	const float LIMPIAR_ANCHO = 84.0f;
	const float LISTA_TITULO_Y = 326.0f;
	const float LISTA_X = 334.0f;
	const float LISTA_ANCHO = 232.0f;
	const float LISTA_Y0 = 358.0f;
	const float FILA_PASO = 42.0f;
	const float FILA_ALTO = 38.0f;
	const int FILAS_VISIBLES = 9;
	
	const float DER_X = 595.0f;
	const float DER_ANCHO = 885.0f;
	
	const float MAPA_X = 595.0f;
	const float MAPA_Y = 88.0f;
	const float MAPA_ANCHO = 885.0f;
	const float MAPA_ALTO = 461.0f;
	const float MAPA_REFERENCIA_ANCHO = 959.0f;
	const float MAPA_REFERENCIA_ALTO = 500.0f;
	
	const float MENSAJE_Y = 557.0f;
	const float MENSAJE_ALTO = 28.0f;
	
	const float PANELES_Y = 595.0f;
	const float PANELES_ALTO = 185.0f;
	
	const float RES_X = 595.0f;
	const float RES_ANCHO = 450.0f;
	const float DETALLE_Y = PANELES_Y + 90.0f;
	const float DETALLE_ALTO = 40.0f;
	const int LINEAS_DETALLE_VISIBLES = 2;
	const float AGREGAR_X = RES_X + 14.0f;
	const float AGREGAR_Y = PANELES_Y + 137.0f;
	const float AGREGAR_ANCHO = 422.0f;
	const float AGREGAR_ALTO = 34.0f;
	
	const float VIAJE_X = 1060.0f;
	const float VIAJE_ANCHO = 420.0f;
	const float VIAJE_LISTA_X = VIAJE_X + 14.0f;
	const float VIAJE_LISTA_Y = PANELES_Y + 50.0f;
	const float VIAJE_FILA_ANCHO = 392.0f;
	const float VIAJE_FILA_ALTO = 40.0f;
	const float VIAJE_PASO = 44.0f;
	const int VIAJE_FILAS_VISIBLES = 2;
	const float ELIMINAR_ANCHO = 64.0f;
	const float ELIMINAR_ALTO = 22.0f;
	const float CONFIRMAR_X = VIAJE_X + 14.0f;
	const float CONFIRMAR_Y = PANELES_Y + 143.0f;
	const float CONFIRMAR_ANCHO = 392.0f;
	const float CONFIRMAR_ALTO = 32.0f;
	
	float escalaMapa() { return std::min(MAPA_ANCHO / MAPA_REFERENCIA_ANCHO, MAPA_ALTO / MAPA_REFERENCIA_ALTO); }
	
	float mapaOrigenX() { return MAPA_X + (MAPA_ANCHO - MAPA_REFERENCIA_ANCHO * escalaMapa()) / 2.0f; }
	
	float mapaOrigenY() { return MAPA_Y + (MAPA_ALTO - MAPA_REFERENCIA_ALTO * escalaMapa()) / 2.0f; }
	
	Vector2f posicionEnMapa(float px, float py) { float e = escalaMapa(); return Vector2f(mapaOrigenX() + px * e, mapaOrigenY() + py * e); }
	
	Clock& relojAnimacion() { static Clock reloj; return reloj; }
	
	void dibujarLinea(RenderWindow& ventana, Vector2f a, Vector2f b, float grosor, Color color)
	{
		float dx = b.x - a.x, dy = b.y - a.y;
		RectangleShape l(Vector2f(std::sqrt(dx * dx + dy * dy), grosor));
		l.setOrigin(0, grosor / 2);
		l.setPosition(a);
		l.setRotation(std::atan2(dy, dx) * 180.0f / 3.14159265f);
		l.setFillColor(color);
		ventana.draw(l);
	}
	
	bool dentro(int x, int y, float rx, float ry, float rw, float rh)
	{
		return x >= rx && x <= rx + rw && y >= ry && y <= ry + rh;
	}
	
	float anchoTexto(const Font& fuente, const string& texto, unsigned int tamano, bool negrita)
	{
		Text t;
		t.setFont(fuente);
		t.setString(texto);
		t.setCharacterSize(tamano);
		if (negrita)
			t.setStyle(Text::Bold);
		return t.getLocalBounds().width;
	}
	
	vector<string> crearLineasRuta(const Font& fuente, bool cargada, string* ruta, int cantidad, float maxAncho, unsigned int tamano)
	{
		vector<string> lineas;
		if (ruta == nullptr || cantidad <= 0)
			return lineas;
		
		string linea;
		for (int i = 0; i < cantidad; i++)
		{
			string candidata = linea.empty() ? ruta[i] : linea + " -> " + ruta[i];
			if (!linea.empty() && cargada && anchoTexto(fuente, candidata, tamano, false) > maxAncho)
			{
				lineas.push_back(linea);
				linea = ruta[i];
			}
			else
				linea = candidata;
		}
		if (!linea.empty())
			lineas.push_back(linea);
		return lineas;
	}
	
	string textoRuta(const vector<string>& ciudades)
	{
		string texto;
		for (size_t i = 0; i < ciudades.size(); i++)
		{
			if (i > 0)
				texto += " -> ";
			texto += ciudades[i];
		}
		return texto;
	}
	
	string recortar(const Font& fuente, bool cargada, const string& texto, unsigned int tamano, bool negrita, float maxAncho)
	{
		if (!cargada || anchoTexto(fuente, texto, tamano, negrita) <= maxAncho)
			return texto;
		
		string recortado = texto;
		while (!recortado.empty())
		{
			recortado.erase(recortado.size() - 1);
			if (anchoTexto(fuente, recortado + "...", tamano, negrita) <= maxAncho)
				break;
		}
		return recortado + "...";
	}
	
	void caja(RenderWindow& ventana, float x, float y, float ancho, float alto, Color relleno, Color borde, float grosor)
	{
		RectangleShape r(Vector2f(ancho, alto));
		r.setPosition(x, y);
		r.setFillColor(relleno);
		r.setOutlineThickness(grosor);
		r.setOutlineColor(borde);
		ventana.draw(r);
	}
	
	void textoCentrado(RenderWindow& ventana, const Font& fuente, bool cargada, const string& texto, float cx, float cy, unsigned int tamano, Color color, bool negrita)
	{
		if (!cargada)
			return;
		
		Text t;
		t.setFont(fuente);
		t.setString(texto);
		t.setCharacterSize(tamano);
		t.setFillColor(color);
		if (negrita)
			t.setStyle(Text::Bold);
		FloatRect b = t.getLocalBounds();
		t.setPosition(cx - b.width / 2 - b.left, cy - b.height / 2 - b.top);
		ventana.draw(t);
	}
	
	void etiquetaMapa(RenderWindow& ventana, const Font& fuente, bool cargada, const string& texto, Vector2f pos, bool destacada, Color fondo)
	{
		if (!cargada)
			return;
		
		float ancho = anchoTexto(fuente, texto, 12, destacada) + 10.0f;
		float x = pos.x + 14.0f;
		float y = pos.y - 10.0f;
		
		if (x + ancho > MAPA_X + MAPA_ANCHO - 4.0f)
			x = pos.x - 14.0f - ancho;
		if (y < MAPA_Y + 3.0f)
			y = MAPA_Y + 3.0f;
		if (y + 20.0f > MAPA_Y + MAPA_ALTO - 3.0f)
			y = MAPA_Y + MAPA_ALTO - 23.0f;
		
		caja(ventana, x, y, ancho, 20.0f, fondo, fondo, 0);
		
		Text t;
		t.setFont(fuente);
		t.setString(texto);
		t.setCharacterSize(12);
		t.setFillColor(Color::White);
		if (destacada)
			t.setStyle(Text::Bold);
		t.setPosition(x + 5.0f, y + 2.0f);
		ventana.draw(t);
	}
}

PantallaBuscarRutas::PantallaBuscarRutas(Grafo* grafo, ListaPasajeros* pasajeros, ListaMaletas* maletas,
										 ListaVuelos* vuelos, ListaReservas* reservas, GestorPantallas* gestorPantallas)
{
	this->grafo = grafo;
	listaPasajeros = pasajeros;
	listaMaletas = maletas;
	listaVuelos = vuelos;
	listaReservas = reservas;
	this->gestorPantallas = gestorPantallas;
	fuenteCargada = fuente.loadFromFile("resources/times.ttf");
	mapaCargado = texturaMapa.loadFromFile("resources/mapa.png");
	if (mapaCargado) mapa.setTexture(texturaMapa);
	codigoOrigen = "";
	codigoDestino = "";
	seleccionandoOrigen = true;
	porCosto = true;
	ruta = nullptr;
	codigosVuelosRuta = nullptr;
	cantidadRuta = 0;
	costoTotal = 0;
	duracionTotal = 0;
	resultadoCalculado = false;
	rutaEncontrada = false;
	mensaje = mapaCargado ? "Selecciona origen y destino para consultar una ruta." : "No se pudo cargar resources/mapa.png.";
	desplazamientoCiudades = 0;
	costoAcumulado = 0;
	desplazamientoRutas = 0;
	desplazamientoDetalleRuta = 0;
	viajeConfirmado = false;
}

PantallaBuscarRutas::~PantallaBuscarRutas()
{
	limpiarResultado();
}

void PantallaBuscarRutas::limpiarResultado()
{
	if (ruta != nullptr)
	{
		delete[] ruta;
		ruta = nullptr;
	}
	if (codigosVuelosRuta != nullptr)
	{
		delete[] codigosVuelosRuta;
		codigosVuelosRuta = nullptr;
	}
	
	cantidadRuta = 0;
	costoTotal = 0;
	duracionTotal = 0;
	resultadoCalculado = false;
	rutaEncontrada = false;
	desplazamientoDetalleRuta = 0;
}

void PantallaBuscarRutas::dibujarTexto(RenderWindow& ventana, float x, float y, const string& texto, unsigned int tamano, Color color, bool negrita)
{
	if (!fuenteCargada)
		return;
	
	Text etiqueta;
	etiqueta.setFont(fuente);
	etiqueta.setString(texto);
	etiqueta.setCharacterSize(tamano);
	etiqueta.setFillColor(color);
	
	if (negrita)
		etiqueta.setStyle(Text::Bold);
	
	etiqueta.setPosition(x, y);
	ventana.draw(etiqueta);
}

void PantallaBuscarRutas::dibujarPanel(RenderWindow& ventana, float x, float y, float ancho, float alto, Color color)
{
	RectangleShape panel(Vector2f(ancho, alto));
	panel.setPosition(x, y);
	panel.setFillColor(color);
	panel.setOutlineThickness(1);
	panel.setOutlineColor(Color(215, 225, 220));
	ventana.draw(panel);
}

void PantallaBuscarRutas::procesarEventos(RenderWindow& ventana, Event evento)
{
	if (evento.type == Event::MouseWheelScrolled)
	{
		Vector2i mouse = Mouse::getPosition(ventana);
		if (grafo != nullptr && dentro(mouse.x, mouse.y, X0, PY, PANEL_IZQ_ANCHO, PH))
		{
			int maximo = grafo->getCantidadVertices() > FILAS_VISIBLES ? grafo->getCantidadVertices() - FILAS_VISIBLES : 0;
			if (evento.mouseWheelScroll.delta < 0 && desplazamientoCiudades < maximo) desplazamientoCiudades++;
			else if (evento.mouseWheelScroll.delta > 0 && desplazamientoCiudades > 0) desplazamientoCiudades--;
		}
		else if (dentro(mouse.x, mouse.y, VIAJE_LISTA_X, VIAJE_LISTA_Y,
						VIAJE_FILA_ANCHO, VIAJE_FILAS_VISIBLES * VIAJE_PASO))
		{
			int maximo = static_cast<int>(rutasSeleccionadas.size()) - VIAJE_FILAS_VISIBLES;
			if (maximo < 0)
				maximo = 0;
			if (evento.mouseWheelScroll.delta < 0 && desplazamientoRutas < maximo) desplazamientoRutas++;
			else if (evento.mouseWheelScroll.delta > 0 && desplazamientoRutas > 0) desplazamientoRutas--;
		}
		else if (resultadoCalculado && rutaEncontrada && dentro(mouse.x, mouse.y, RES_X, DETALLE_Y, RES_ANCHO, DETALLE_ALTO))
		{
			vector<string> lineas = crearLineasRuta(fuente, fuenteCargada, ruta, cantidadRuta, RES_ANCHO - 28.0f, 13);
			int maximo = static_cast<int>(lineas.size()) > LINEAS_DETALLE_VISIBLES
				? static_cast<int>(lineas.size()) - LINEAS_DETALLE_VISIBLES : 0;
			if (evento.mouseWheelScroll.delta < 0 && desplazamientoDetalleRuta < maximo) desplazamientoDetalleRuta++;
			else if (evento.mouseWheelScroll.delta > 0 && desplazamientoDetalleRuta > 0) desplazamientoDetalleRuta--;
		}
		return;
	}
	if (evento.type != Event::MouseButtonPressed || evento.mouseButton.button != Mouse::Left) return;
	
	int x = evento.mouseButton.x;
	int y = evento.mouseButton.y;
	
	if (dentro(x, y, IZQ_X, ORIGEN_Y, IZQ_ANCHO, CAMPO_ALTO))
	{
		seleccionandoOrigen = true;
		mensaje = "Elige una ciudad de la lista para establecer el origen.";
		return;
	}
	if (dentro(x, y, IZQ_X, DESTINO_Y, IZQ_ANCHO, CAMPO_ALTO))
	{
		seleccionandoOrigen = false;
		mensaje = "Elige una ciudad de la lista para establecer el destino.";
		return;
	}
	if (dentro(x, y, COSTO_X, CRITERIO_Y, BOTON_MITAD, BOTON_ALTO))
	{
		porCosto = true; limpiarResultado(); mensaje = "Criterio: ruta de menor costo."; return;
	}
	if (dentro(x, y, RAPIDO_X, CRITERIO_Y, BOTON_MITAD, BOTON_ALTO))
	{
		porCosto = false; limpiarResultado(); mensaje = "Criterio: ruta de menor duracion."; return;
	}
	if (dentro(x, y, BUSCAR_X, ACCION_Y, BUSCAR_ANCHO, BOTON_ALTO))
	{
		limpiarResultado();
		if (grafo == nullptr || codigoOrigen.empty() || codigoDestino.empty()) { mensaje = "Selecciona una ciudad de origen y otra de destino."; return; }
		if (codigoOrigen == codigoDestino) { mensaje = "El origen y el destino no pueden ser la misma ciudad."; return; }
		rutaEncontrada = dijkstra.calcular(*grafo, codigoOrigen, codigoDestino, porCosto,
										   ruta, cantidadRuta, costoTotal, duracionTotal, codigosVuelosRuta);
		resultadoCalculado = true;
		if (rutaEncontrada)
			mensaje = "Ruta encontrada. Revisa el recorrido y agregalo a tu viaje.";
		else mensaje = "No existe una ruta entre esas dos ciudades.";
		return;
	}
	if (dentro(x, y, AGREGAR_X, AGREGAR_Y, AGREGAR_ANCHO, AGREGAR_ALTO))
	{
		if (!resultadoCalculado || !rutaEncontrada || ruta == nullptr)
		{
			mensaje = "Primero busca una ruta para poder agregarla a tu viaje.";
			return;
		}
		RutaSeleccionada seleccionada;
		for (int i = 0; i < cantidadRuta; i++)
			seleccionada.ciudades.push_back(ruta[i]);
		for (int i = 0; i < cantidadRuta - 1; i++)
		{
			if (codigosVuelosRuta == nullptr)
				continue;
			for (size_t j = 0; j < rutasSeleccionadas.size(); j++)
			{
				for (size_t k = 0; k < rutasSeleccionadas[j].codigosVuelos.size(); k++)
				{
					if (rutasSeleccionadas[j].codigosVuelos[k] == codigosVuelosRuta[i])
					{
						mensaje = "Ese vuelo ya forma parte del viaje seleccionado.";
						return;
					}
				}
			}
			seleccionada.codigosVuelos.push_back(codigosVuelosRuta[i]);
		}
		if (seleccionada.codigosVuelos.empty())
		{
			mensaje = "La ruta no contiene vuelos que se puedan reservar.";
			return;
		}
		seleccionada.costo = costoTotal;
		seleccionada.duracion = duracionTotal;
		rutasSeleccionadas.push_back(seleccionada);
		costoAcumulado += costoTotal;
		int maximo = static_cast<int>(rutasSeleccionadas.size()) > VIAJE_FILAS_VISIBLES
			? static_cast<int>(rutasSeleccionadas.size()) - VIAJE_FILAS_VISIBLES : 0;
		if (desplazamientoRutas < maximo)
			desplazamientoRutas = maximo;
		viajeConfirmado = false;
		mensaje = "Ruta agregada a mi viaje.";
		return;
	}
	if (dentro(x, y, CONFIRMAR_X, CONFIRMAR_Y, CONFIRMAR_ANCHO, CONFIRMAR_ALTO))
	{
		if (rutasSeleccionadas.empty())
		{
			mensaje = "Agrega al menos una ruta antes de confirmar la compra.";
			return;
		}
		if (gestorPantallas == nullptr || listaPasajeros == nullptr || listaMaletas == nullptr ||
			listaVuelos == nullptr || listaReservas == nullptr)
		{
			mensaje = "No se pudo abrir la reserva: faltan datos del sistema.";
			return;
		}
		vector<string> vuelosCompra;
		vector<string> detallesRutas;
		for (size_t i = 0; i < rutasSeleccionadas.size(); i++)
		{
			ostringstream detalle;
			detalle << "Vuelos ";
			for (size_t j = 0; j < rutasSeleccionadas[i].codigosVuelos.size(); j++)
			{
				if (j > 0)
					detalle << ", ";
				detalle << rutasSeleccionadas[i].codigosVuelos[j];
			}
			detalle << " | " << textoRuta(rutasSeleccionadas[i].ciudades)
				<< " | " << (rutasSeleccionadas[i].ciudades.size() > 2
							 ? rutasSeleccionadas[i].ciudades.size() - 2 : 0) << " escala(s)"
				<< " | CRC " << fixed << setprecision(2) << rutasSeleccionadas[i].costo
				<< " | " << setprecision(1) << rutasSeleccionadas[i].duracion << " h";
			detallesRutas.push_back(detalle.str());
			for (size_t j = 0; j < rutasSeleccionadas[i].codigosVuelos.size(); j++)
				vuelosCompra.push_back(rutasSeleccionadas[i].codigosVuelos[j]);
		}
		
		viajeConfirmado = true;
		gestorPantallas->solicitarCambioPantalla(new PantallaPasajeros(listaPasajeros, listaMaletas,
																	   gestorPantallas, grafo, listaVuelos, listaReservas, vuelosCompra, detallesRutas, costoAcumulado));
		return;
	}
	if (dentro(x, y, VIAJE_LISTA_X, VIAJE_LISTA_Y,
			   VIAJE_FILA_ANCHO, VIAJE_FILAS_VISIBLES * VIAJE_PASO))
	{
		int fila = static_cast<int>((y - VIAJE_LISTA_Y) / VIAJE_PASO);
		int indice = desplazamientoRutas + fila;
		if (fila < 0 || fila >= VIAJE_FILAS_VISIBLES || indice >= static_cast<int>(rutasSeleccionadas.size()))
			return;
		float eliminarX = VIAJE_LISTA_X + VIAJE_FILA_ANCHO - ELIMINAR_ANCHO - 8.0f;
		float eliminarY = VIAJE_LISTA_Y + fila * VIAJE_PASO + (VIAJE_FILA_ALTO - ELIMINAR_ALTO) / 2.0f;
		if (dentro(x, y, eliminarX, eliminarY, ELIMINAR_ANCHO, ELIMINAR_ALTO))
		{
			rutasSeleccionadas.erase(rutasSeleccionadas.begin() + indice);
			costoAcumulado = 0;
			for (size_t i = 0; i < rutasSeleccionadas.size(); i++)
				costoAcumulado += rutasSeleccionadas[i].costo;
			int maximo = static_cast<int>(rutasSeleccionadas.size()) - VIAJE_FILAS_VISIBLES;
			if (maximo < 0)
				maximo = 0;
			if (desplazamientoRutas > maximo)
				desplazamientoRutas = maximo;
			viajeConfirmado = false;
			mensaje = "Ruta eliminada de mi viaje.";
		}
		return;
	}
	if (dentro(x, y, LIMPIAR_X, ACCION_Y, LIMPIAR_ANCHO, BOTON_ALTO))
	{
		codigoOrigen = ""; codigoDestino = ""; seleccionandoOrigen = true;
		limpiarResultado(); mensaje = "Selecciona origen y destino para consultar una ruta."; return;
	}
	if (grafo != nullptr && dentro(x, y, LISTA_X, LISTA_Y0, LISTA_ANCHO, FILAS_VISIBLES * FILA_PASO))
	{
		int fila = static_cast<int>((y - LISTA_Y0) / FILA_PASO);
		float dentroFila = (y - LISTA_Y0) - fila * FILA_PASO;
		if (fila < 0 || fila >= FILAS_VISIBLES || dentroFila > FILA_ALTO) return;
		int objetivo = desplazamientoCiudades + fila;
		NodoVertice* actual = grafo->getPrimero();
		int indice = 0;
		while (actual != nullptr && indice < objetivo) { actual = actual->siguiente; indice++; }
		if (actual == nullptr) return;
		if (seleccionandoOrigen)
		{
			codigoOrigen = actual->ciudad.getCodigo(); seleccionandoOrigen = false;
			mensaje = "Origen seleccionado. Ahora selecciona el destino.";
		}
		else
		{
			codigoDestino = actual->ciudad.getCodigo();
			mensaje = "Destino seleccionado. Presiona BUSCAR.";
		}
		limpiarResultado();
	}
}

void PantallaBuscarRutas::actualizar(float tiempo)
{
	(void)tiempo;
}

NodoVertice* PantallaBuscarRutas::buscarCiudadPorNombre(const string& nombre) const
{
	if (grafo == nullptr) return nullptr;
	NodoVertice* actual = grafo->getPrimero();
	while (actual != nullptr)
	{
		if (actual->ciudad.getNombre() == nombre) return actual;
		actual = actual->siguiente;
	}
	return nullptr;
}

void PantallaBuscarRutas::dibujarRuta(RenderWindow& ventana)
{
	if (!resultadoCalculado || !rutaEncontrada || ruta == nullptr || cantidadRuta < 2) return;
	
	vector<Vector2f> puntos;
	for (int i = 0; i < cantidadRuta; i++)
	{
		NodoVertice* ciudad = buscarCiudadPorNombre(ruta[i]);
		if (ciudad != nullptr)
			puntos.push_back(posicionEnMapa(ciudad->ciudad.getPosicionX(), ciudad->ciudad.getPosicionY()));
	}
	
	float total = 0;
	for (size_t i = 0; i + 1 < puntos.size(); i++)
	{
		dibujarLinea(ventana, puntos[i], puntos[i + 1], 9.0f, Color(10, 76, 55));
		dibujarLinea(ventana, puntos[i], puntos[i + 1], 4.0f, Color::White);
		Vector2f d = puntos[i + 1] - puntos[i];
		total += std::sqrt(d.x * d.x + d.y * d.y);
	}
	
	if (puntos.size() < 2 || total <= 0)
		return;
	
	float recorrido = std::fmod(relojAnimacion().getElapsedTime().asSeconds() * 170.0f, total + 70.0f);
	if (recorrido > total)
		return;
	
	float acumulado = 0;
	for (size_t i = 0; i + 1 < puntos.size(); i++)
	{
		Vector2f d = puntos[i + 1] - puntos[i];
		float largo = std::sqrt(d.x * d.x + d.y * d.y);
		if (largo > 0 && recorrido <= acumulado + largo)
		{
			float fraccion = (recorrido - acumulado) / largo;
			CircleShape avion(11, 3);
			avion.setOrigin(11, 11);
			avion.setPosition(puntos[i] + d * fraccion);
			avion.setRotation(std::atan2(d.y, d.x) * 180.0f / 3.14159265f + 90.0f);
			avion.setFillColor(Color(235, 155, 35));
			avion.setOutlineThickness(2);
			avion.setOutlineColor(Color::White);
			ventana.draw(avion);
			return;
		}
		acumulado += largo;
	}
}

void PantallaBuscarRutas::dibujarMarcadores(RenderWindow& ventana)
{
	if (grafo == nullptr) return;
	
	NodoVertice* actual = grafo->getPrimero();
	while (actual != nullptr)
	{
		Vector2f pos = posicionEnMapa(actual->ciudad.getPosicionX(), actual->ciudad.getPosicionY());
		bool esOrigen = actual->ciudad.getCodigo() == codigoOrigen;
		bool esDestino = actual->ciudad.getCodigo() == codigoDestino;
		bool esEscala = false;
		if (resultadoCalculado && rutaEncontrada && ruta != nullptr)
		{
			for (int i = 1; i < cantidadRuta - 1; i++)
			{
				if (ruta[i] == actual->ciudad.getNombre()) { esEscala = true; break; }
			}
		}
		
		Color color(105, 120, 110);
		if (esOrigen) color = Color(35, 155, 75);
		else if (esDestino) color = Color(40, 110, 205);
		else if (esEscala) color = Color(235, 155, 35);
		
		CircleShape punto(9);
		punto.setOrigin(9, 9);
		punto.setPosition(pos);
		punto.setFillColor(color);
		punto.setOutlineThickness(2);
		punto.setOutlineColor(Color::White);
		ventana.draw(punto);
		
		actual = actual->siguiente;
	}
	
	actual = grafo->getPrimero();
	while (actual != nullptr)
	{
		Vector2f pos = posicionEnMapa(actual->ciudad.getPosicionX(), actual->ciudad.getPosicionY());
		bool esOrigen = actual->ciudad.getCodigo() == codigoOrigen;
		bool esDestino = actual->ciudad.getCodigo() == codigoDestino;
		bool esEscala = false;
		if (resultadoCalculado && rutaEncontrada && ruta != nullptr)
		{
			for (int i = 1; i < cantidadRuta - 1; i++)
			{
				if (ruta[i] == actual->ciudad.getNombre()) { esEscala = true; break; }
			}
		}
		
		Color fondo(0, 0, 0, 140);
		if (esOrigen) fondo = Color(25, 125, 60, 235);
		else if (esDestino) fondo = Color(30, 90, 175, 235);
		else if (esEscala) fondo = Color(200, 120, 20, 235);
		
		etiquetaMapa(ventana, fuente, fuenteCargada, actual->ciudad.getNombre(), pos, esOrigen || esDestino || esEscala, fondo);
		
		actual = actual->siguiente;
	}
}

void PantallaBuscarRutas::dibujarMapa(RenderWindow& ventana)
{
	caja(ventana, MAPA_X, MAPA_Y, MAPA_ANCHO, MAPA_ALTO, Color(248, 250, 247), Color(215, 225, 220), 1);
	
	if (mapaCargado)
	{
		Vector2u tam = texturaMapa.getSize();
		if (tam.x > 0 && tam.y > 0)
		{
			float e = escalaMapa();
			mapa.setPosition(mapaOrigenX(), mapaOrigenY());
			mapa.setScale(MAPA_REFERENCIA_ANCHO * e / static_cast<float>(tam.x), MAPA_REFERENCIA_ALTO * e / static_cast<float>(tam.y));
			ventana.draw(mapa);
		}
	}
	else dibujarTexto(ventana, MAPA_X + 15, MAPA_Y + 15, "No se encontro resources/mapa.png", 16, Color(160, 70, 60), true);
	
	dibujarRuta(ventana);
	dibujarMarcadores(ventana);
	
	float ly = MAPA_Y + MAPA_ALTO - 36;
	caja(ventana, MAPA_X + 10, ly, 270, 26, Color(255, 255, 255, 235), Color(215, 225, 220), 1);
	
	CircleShape punto(6);
	punto.setOrigin(6, 6);
	float cy = ly + 13;
	punto.setPosition(MAPA_X + 26, cy); punto.setFillColor(Color(35, 155, 75)); ventana.draw(punto);
	dibujarTexto(ventana, MAPA_X + 38, ly + 5, "Origen", 12, Color(45, 65, 50), true);
	punto.setPosition(MAPA_X + 113, cy); punto.setFillColor(Color(40, 110, 205)); ventana.draw(punto);
	dibujarTexto(ventana, MAPA_X + 125, ly + 5, "Destino", 12, Color(45, 65, 50), true);
	punto.setPosition(MAPA_X + 201, cy); punto.setFillColor(Color(235, 155, 35)); ventana.draw(punto);
	dibujarTexto(ventana, MAPA_X + 213, ly + 5, "Escala", 12, Color(45, 65, 50), true);
}

void PantallaBuscarRutas::dibujar(RenderWindow& ventana)
{
	Color verdeOscuro(10, 76, 55), verde(42, 112, 72), verdeClaro(225, 242, 224), verdeHover(30, 88, 56);
	Color azulClaro(222, 235, 248), textoOscuro(35, 65, 52), gris(105, 115, 108);
	Color blanco(255, 255, 255), azul(35, 91, 145), borde(215, 225, 220), normal(250, 252, 250);
	Vector2i mouse = Mouse::getPosition(ventana);
	ventana.setView(ventana.getDefaultView());
	
	caja(ventana, 300, 0, 1200, 800, Color(247, 250, 247), Color(247, 250, 247), 0);
	
	caja(ventana, X0, ENC_Y, ENC_ANCHO, ENC_ALTO, verdeOscuro, verdeOscuro, 0);
	dibujarTexto(ventana, X0 + 24, ENC_Y + 6, "BUSCAR RUTAS", 28, blanco, true);
	dibujarTexto(ventana, X0 + 26, ENC_Y + 39, "Busca vuelos por precio o duracion y revisa el itinerario en el mapa", 14, Color(230, 245, 235));
	
	dibujarPanel(ventana, X0, PY, PANEL_IZQ_ANCHO, PH, blanco);
	
	NodoVertice* origen = (grafo && !codigoOrigen.empty()) ? grafo->buscarCiudad(codigoOrigen) : nullptr;
	NodoVertice* destino = (grafo && !codigoDestino.empty()) ? grafo->buscarCiudad(codigoDestino) : nullptr;
	string nombreOrigen = origen ? origen->ciudad.getNombre() : "Seleccionar origen";
	string nombreDestino = destino ? destino->ciudad.getNombre() : "Seleccionar destino";
	
	caja(ventana, IZQ_X, ORIGEN_Y, IZQ_ANCHO, CAMPO_ALTO, seleccionandoOrigen ? verdeClaro : normal, seleccionandoOrigen ? verde : borde, seleccionandoOrigen ? 2 : 1);
	caja(ventana, IZQ_X, DESTINO_Y, IZQ_ANCHO, CAMPO_ALTO, !seleccionandoOrigen ? azulClaro : normal, !seleccionandoOrigen ? azul : borde, !seleccionandoOrigen ? 2 : 1);
	dibujarTexto(ventana, IZQ_X + 12, ORIGEN_Y + 6, "ORIGEN", 11, gris, true);
	dibujarTexto(ventana, IZQ_X + 12, ORIGEN_Y + 24, recortar(fuente, fuenteCargada, nombreOrigen, 16, true, IZQ_ANCHO - 24), 16, textoOscuro, true);
	dibujarTexto(ventana, IZQ_X + 12, DESTINO_Y + 6, "DESTINO", 11, gris, true);
	dibujarTexto(ventana, IZQ_X + 12, DESTINO_Y + 24, recortar(fuente, fuenteCargada, nombreDestino, 16, true, IZQ_ANCHO - 24), 16, textoOscuro, true);
	
	bool hCosto = dentro(mouse.x, mouse.y, COSTO_X, CRITERIO_Y, BOTON_MITAD, BOTON_ALTO);
	bool hRapido = dentro(mouse.x, mouse.y, RAPIDO_X, CRITERIO_Y, BOTON_MITAD, BOTON_ALTO);
	bool hBuscar = dentro(mouse.x, mouse.y, BUSCAR_X, ACCION_Y, BUSCAR_ANCHO, BOTON_ALTO);
	bool hLimpiar = dentro(mouse.x, mouse.y, LIMPIAR_X, ACCION_Y, LIMPIAR_ANCHO, BOTON_ALTO);
	caja(ventana, COSTO_X, CRITERIO_Y, BOTON_MITAD, BOTON_ALTO, porCosto ? verdeClaro : (hCosto ? Color(238, 244, 238) : normal), porCosto ? verde : borde, porCosto ? 2 : 1);
	caja(ventana, RAPIDO_X, CRITERIO_Y, BOTON_MITAD, BOTON_ALTO, !porCosto ? azulClaro : (hRapido ? Color(238, 244, 248) : normal), !porCosto ? azul : borde, !porCosto ? 2 : 1);
	caja(ventana, BUSCAR_X, ACCION_Y, BUSCAR_ANCHO, BOTON_ALTO, hBuscar ? verdeHover : verde, hBuscar ? verdeHover : verde, 0);
	caja(ventana, LIMPIAR_X, ACCION_Y, LIMPIAR_ANCHO, BOTON_ALTO, hLimpiar ? Color(222, 230, 222) : Color(235, 239, 235), borde, 1);
	textoCentrado(ventana, fuente, fuenteCargada, "PRECIO", COSTO_X + BOTON_MITAD / 2, CRITERIO_Y + BOTON_ALTO / 2, 13, textoOscuro, true);
	textoCentrado(ventana, fuente, fuenteCargada, "DURACION", RAPIDO_X + BOTON_MITAD / 2, CRITERIO_Y + BOTON_ALTO / 2, 13, textoOscuro, true);
	textoCentrado(ventana, fuente, fuenteCargada, "BUSCAR", BUSCAR_X + BUSCAR_ANCHO / 2, ACCION_Y + BOTON_ALTO / 2, 14, blanco, true);
	textoCentrado(ventana, fuente, fuenteCargada, "LIMPIAR", LIMPIAR_X + LIMPIAR_ANCHO / 2, ACCION_Y + BOTON_ALTO / 2, 13, textoOscuro, true);
	
	dibujarTexto(ventana, LISTA_X, LISTA_TITULO_Y, "CIUDADES", 16, verdeOscuro, true);
	dibujarTexto(ventana, LISTA_X + 130, LISTA_TITULO_Y + 4, seleccionandoOrigen ? "Elige el origen" : "Elige el destino", 11, gris);
	if (grafo == nullptr || grafo->getCantidadVertices() == 0)
		dibujarTexto(ventana, LISTA_X, LISTA_Y0 + 8, "No hay ciudades registradas.", 13, gris);
	else
	{
		NodoVertice* ciudad = grafo->getPrimero(); int indice = 0, fila = 0;
		while (ciudad != nullptr && fila < FILAS_VISIBLES)
		{
			if (indice >= desplazamientoCiudades)
			{
				float y = LISTA_Y0 + fila * FILA_PASO;
				bool esOrigen = ciudad->ciudad.getCodigo() == codigoOrigen;
				bool esDestino = ciudad->ciudad.getCodigo() == codigoDestino;
				Color fondo = esOrigen ? verdeClaro : (esDestino ? azulClaro : normal);
				caja(ventana, LISTA_X, y, LISTA_ANCHO, FILA_ALTO, fondo, Color(225, 232, 226), 1);
				string nombre = recortar(fuente, fuenteCargada, ciudad->ciudad.getNombre(), 14, esOrigen || esDestino, LISTA_ANCHO - 62);
				bool conEtiqueta = esOrigen || esDestino;
				dibujarTexto(ventana, LISTA_X + 9, y + (conEtiqueta ? 3 : 9), nombre, 14, textoOscuro, conEtiqueta);
				dibujarTexto(ventana, LISTA_X + LISTA_ANCHO - 44, y + (conEtiqueta ? 6 : 12), ciudad->ciudad.getCodigo(), 11, gris);
				if (esOrigen) dibujarTexto(ventana, LISTA_X + 9, y + 22, "ORIGEN", 9, verde, true);
				else if (esDestino) dibujarTexto(ventana, LISTA_X + 9, y + 22, "DESTINO", 9, azul, true);
				fila++;
			}
			ciudad = ciudad->siguiente; indice++;
		}
		if (grafo->getCantidadVertices() > FILAS_VISIBLES)
			dibujarTexto(ventana, LISTA_X, PY + PH - 24, "Usa la rueda para ver mas ciudades.", 11, gris);
	}
	
	dibujarMapa(ventana);
	
	caja(ventana, DER_X, MENSAJE_Y, DER_ANCHO, MENSAJE_ALTO, Color(245, 248, 245), borde, 1);
	dibujarTexto(ventana, DER_X + 14, MENSAJE_Y + 5, recortar(fuente, fuenteCargada, mensaje, 13, false, DER_ANCHO - 28), 13, textoOscuro);
	
	dibujarPanel(ventana, RES_X, PANELES_Y, RES_ANCHO, PANELES_ALTO, blanco);
	dibujarTexto(ventana, RES_X + 14, PANELES_Y + 9, "RESULTADO", 15, verdeOscuro, true);
	
	bool hayResultado = resultadoCalculado && rutaEncontrada;
	string valorEscalas = "-", valorPrecio = "-", valorDuracion = "-";
	if (hayResultado)
	{
		ostringstream costo, duracion;
		costo << fixed << setprecision(2) << costoTotal;
		duracion << fixed << setprecision(1) << duracionTotal;
		valorEscalas = to_string(cantidadRuta > 2 ? cantidadRuta - 2 : 0);
		valorPrecio = "CRC " + costo.str();
		valorDuracion = duracion.str() + " h";
	}
	dibujarTexto(ventana, RES_X + 14, PANELES_Y + 34, "ESCALAS", 10, gris, true);
	dibujarTexto(ventana, RES_X + 14, PANELES_Y + 47, valorEscalas, 15, textoOscuro, true);
	dibujarTexto(ventana, RES_X + 154, PANELES_Y + 34, "PRECIO", 10, gris, true);
	dibujarTexto(ventana, RES_X + 154, PANELES_Y + 47, valorPrecio, 15, textoOscuro, true);
	dibujarTexto(ventana, RES_X + 304, PANELES_Y + 34, "DURACION", 10, gris, true);
	dibujarTexto(ventana, RES_X + 304, PANELES_Y + 47, valorDuracion, 15, textoOscuro, true);
	
	dibujarTexto(ventana, RES_X + 14, PANELES_Y + 76, "RECORRIDO", 10, gris, true);
	if (hayResultado)
	{
		vector<string> lineas = crearLineasRuta(fuente, fuenteCargada, ruta, cantidadRuta, RES_ANCHO - 28.0f, 13);
		int inicio = desplazamientoDetalleRuta;
		if (inicio > static_cast<int>(lineas.size()))
			inicio = static_cast<int>(lineas.size());
		for (int i = 0; i < LINEAS_DETALLE_VISIBLES && inicio + i < static_cast<int>(lineas.size()); i++)
			dibujarTexto(ventana, RES_X + 14, DETALLE_Y + i * 19, lineas[inicio + i], 13, textoOscuro);
		if (static_cast<int>(lineas.size()) > LINEAS_DETALLE_VISIBLES)
			dibujarTexto(ventana, RES_X + RES_ANCHO - 104, PANELES_Y + 76, "Rueda: ver mas", 10, gris);
	}
	else
		dibujarTexto(ventana, RES_X + 14, DETALLE_Y, "Busca una ruta para ver el recorrido aqui.", 13, gris);
	
	bool hAgregar = dentro(mouse.x, mouse.y, AGREGAR_X, AGREGAR_Y, AGREGAR_ANCHO, AGREGAR_ALTO);
	Color fondoAgregar = hayResultado ? (hAgregar ? verdeHover : verde) : Color(165, 180, 170);
	caja(ventana, AGREGAR_X, AGREGAR_Y, AGREGAR_ANCHO, AGREGAR_ALTO, fondoAgregar, fondoAgregar, 0);
	textoCentrado(ventana, fuente, fuenteCargada, "ANADIR A MI VIAJE", AGREGAR_X + AGREGAR_ANCHO / 2, AGREGAR_Y + AGREGAR_ALTO / 2, 14, blanco, true);
	
	dibujarPanel(ventana, VIAJE_X, PANELES_Y, VIAJE_ANCHO, PANELES_ALTO, blanco);
	dibujarTexto(ventana, VIAJE_X + 14, PANELES_Y + 9, "MI VIAJE", 15, verdeOscuro, true);
	ostringstream total;
	total << fixed << setprecision(2) << costoAcumulado;
	dibujarTexto(ventana, VIAJE_X + 14, PANELES_Y + 29, "Rutas: " + to_string(rutasSeleccionadas.size()) + "   Total: CRC " + total.str(), 12, gris, true);
	if (static_cast<int>(rutasSeleccionadas.size()) > VIAJE_FILAS_VISIBLES)
		dibujarTexto(ventana, VIAJE_X + VIAJE_ANCHO - 104, PANELES_Y + 11, "Rueda: ver mas", 10, gris);
	if (rutasSeleccionadas.empty())
		dibujarTexto(ventana, VIAJE_LISTA_X, VIAJE_LISTA_Y + 12, "Agrega una ruta para comenzar.", 13, gris);
	
	for (int fila = 0; fila < VIAJE_FILAS_VISIBLES; fila++)
	{
		int indice = desplazamientoRutas + fila;
		if (indice >= static_cast<int>(rutasSeleccionadas.size()))
			break;
		float x = VIAJE_LISTA_X;
		float y = VIAJE_LISTA_Y + fila * VIAJE_PASO;
		const RutaSeleccionada& seleccionada = rutasSeleccionadas[indice];
		caja(ventana, x, y, VIAJE_FILA_ANCHO, VIAJE_FILA_ALTO, Color(248, 250, 248), Color(225, 232, 226), 1);
		string recorrido = "Ruta " + to_string(indice + 1) + ": " + textoRuta(seleccionada.ciudades);
		dibujarTexto(ventana, x + 8, y + 3,
					 recortar(fuente, fuenteCargada, recorrido, 13, true, VIAJE_FILA_ANCHO - ELIMINAR_ANCHO - 28), 13, textoOscuro, true);
		ostringstream detalle;
		detalle << fixed << setprecision(2) << seleccionada.costo << " | " << setprecision(1) << seleccionada.duracion << " h";
		dibujarTexto(ventana, x + 8, y + 22, "CRC " + detalle.str(), 11, gris);
		
		float eliminarX = x + VIAJE_FILA_ANCHO - ELIMINAR_ANCHO - 8.0f;
		float eliminarY = y + (VIAJE_FILA_ALTO - ELIMINAR_ALTO) / 2.0f;
		bool hEliminar = dentro(mouse.x, mouse.y, eliminarX, eliminarY, ELIMINAR_ANCHO, ELIMINAR_ALTO);
		caja(ventana, eliminarX, eliminarY, ELIMINAR_ANCHO, ELIMINAR_ALTO, hEliminar ? Color(250, 225, 220) : Color(245, 235, 232), Color(225, 195, 188), 1);
		textoCentrado(ventana, fuente, fuenteCargada, "Eliminar", eliminarX + ELIMINAR_ANCHO / 2, eliminarY + ELIMINAR_ALTO / 2, 11, Color(145, 65, 55), true);
	}
	
	bool hConfirmar = dentro(mouse.x, mouse.y, CONFIRMAR_X, CONFIRMAR_Y, CONFIRMAR_ANCHO, CONFIRMAR_ALTO);
	Color fondoConfirmar = viajeConfirmado ? Color(35, 115, 75) : (hConfirmar ? verdeHover : verde);
	caja(ventana, CONFIRMAR_X, CONFIRMAR_Y, CONFIRMAR_ANCHO, CONFIRMAR_ALTO, fondoConfirmar, fondoConfirmar, 0);
	textoCentrado(ventana, fuente, fuenteCargada, "CONFIRMAR VIAJE",
				  CONFIRMAR_X + CONFIRMAR_ANCHO / 2, CONFIRMAR_Y + CONFIRMAR_ALTO / 2, 14, blanco, true);
}
