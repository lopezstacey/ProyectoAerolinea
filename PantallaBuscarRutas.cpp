#include "PantallaBuscarRutas.h"
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <cmath>

namespace
{
	const float X0 = 320.0f;
	const float PY = 175.0f;
	const float PH = 575.0f;
	const float PANEL_IZQ_ANCHO = 280.0f;
	const float PANEL_DER_X = 620.0f;
	const float PANEL_DER_ANCHO = 855.0f;
	
	const float LISTA_X = 340.0f;
	const float LISTA_ANCHO = 240.0f;
	const float LISTA_Y0 = 260.0f;
	const float FILA_PASO = 51.0f;
	const float FILA_ALTO = 44.0f;
	const int FILAS_VISIBLES = 9;
	
	const float CONT_X = 640.0f;
	const float CONT_ANCHO = 815.0f;
	
	const float CAMPO_Y = 190.0f;
	const float CAMPO_ANCHO = 400.0f;
	const float CAMPO_ALTO = 50.0f;
	const float ORIGEN_X = CONT_X;
	const float DESTINO_X = CONT_X + CONT_ANCHO - CAMPO_ANCHO;
	
	const float BOTON_Y = 252.0f;
	const float BOTON_ALTO = 42.0f;
	const float BOTON_ANCHO = 200.0f;
	const float COSTO_X = CONT_X;
	const float RAPIDO_X = COSTO_X + BOTON_ANCHO + 15.0f;
	const float BUSCAR_X = RAPIDO_X + BOTON_ANCHO + 15.0f;
	const float BUSCAR_ANCHO = CONT_X + CONT_ANCHO - 140.0f - 15.0f - BUSCAR_X;
	
	const float MENSAJE_X = CONT_X;
	const float MENSAJE_Y = 306.0f;
	const float MENSAJE_ALTO = 34.0f;
	const float LIMPIAR_ANCHO = 140.0f;
	const float LIMPIAR_X = CONT_X + CONT_ANCHO - LIMPIAR_ANCHO;
	const float LIMPIAR_Y = BOTON_Y;
	const float LIMPIAR_ALTO = BOTON_ALTO;
	const float MENSAJE_ANCHO = CONT_ANCHO;
	
	const float MAPA_X = CONT_X;
	const float MAPA_Y = 352.0f;
	const float MAPA_ANCHO = CONT_ANCHO;
	const float MAPA_ALTO = 383.0f;
	const float MAPA_REFERENCIA_ANCHO = 959.0f;
	const float MAPA_REFERENCIA_ALTO = 500.0f;
	
	float escalaMapa() { return std::min(MAPA_ANCHO / MAPA_REFERENCIA_ANCHO, MAPA_ALTO / MAPA_REFERENCIA_ALTO); }
	
	float mapaOrigenX() { return MAPA_X + (MAPA_ANCHO - MAPA_REFERENCIA_ANCHO * escalaMapa()) / 2.0f; }
	
	float mapaOrigenY() { return MAPA_Y + (MAPA_ALTO - MAPA_REFERENCIA_ALTO * escalaMapa()) / 2.0f; }
	
	Vector2f posicionEnMapa(float px, float py) { float e = escalaMapa(); return Vector2f(mapaOrigenX() + px * e, mapaOrigenY() + py * e); }
	
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
}

PantallaBuscarRutas::PantallaBuscarRutas(Grafo* grafo)
{
	this->grafo = grafo;
	fuenteCargada = fuente.loadFromFile("resources/times.ttf");
	mapaCargado = texturaMapa.loadFromFile("resources/mapa.png");
	if (mapaCargado) mapa.setTexture(texturaMapa);
	codigoOrigen = "";
	codigoDestino = "";
	seleccionandoOrigen = true;
	porCosto = true;
	ruta = nullptr;
	cantidadRuta = 0;
	costoTotal = 0;
	duracionTotal = 0;
	resultadoCalculado = false;
	rutaEncontrada = false;
	mensaje = mapaCargado ? "Selecciona origen y destino para consultar una ruta." : "No se pudo cargar resources/mapa.png.";
	desplazamientoCiudades = 0;
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
	
	cantidadRuta = 0;
	costoTotal = 0;
	duracionTotal = 0;
	resultadoCalculado = false;
	rutaEncontrada = false;
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
		return;
	}
	if (evento.type != Event::MouseButtonPressed || evento.mouseButton.button != Mouse::Left) return;
	
	int x = evento.mouseButton.x;
	int y = evento.mouseButton.y;
	
	if (dentro(x, y, ORIGEN_X, CAMPO_Y, CAMPO_ANCHO, CAMPO_ALTO))
	{
		seleccionandoOrigen = true;
		mensaje = "Elige una ciudad de la lista para establecer el origen.";
		return;
	}
	if (dentro(x, y, DESTINO_X, CAMPO_Y, CAMPO_ANCHO, CAMPO_ALTO))
	{
		seleccionandoOrigen = false;
		mensaje = "Elige una ciudad de la lista para establecer el destino.";
		return;
	}
	if (dentro(x, y, COSTO_X, BOTON_Y, BOTON_ANCHO, BOTON_ALTO))
	{
		porCosto = true; limpiarResultado(); mensaje = "Criterio: ruta de menor costo."; return;
	}
	if (dentro(x, y, RAPIDO_X, BOTON_Y, BOTON_ANCHO, BOTON_ALTO))
	{
		porCosto = false; limpiarResultado(); mensaje = "Criterio: ruta de menor duracion."; return;
	}
	if (dentro(x, y, BUSCAR_X, BOTON_Y, BUSCAR_ANCHO, BOTON_ALTO))
	{
		limpiarResultado();
		if (grafo == nullptr || codigoOrigen.empty() || codigoDestino.empty()) { mensaje = "Selecciona una ciudad de origen y otra de destino."; return; }
		if (codigoOrigen == codigoDestino) { mensaje = "El origen y el destino no pueden ser la misma ciudad."; return; }
		rutaEncontrada = dijkstra.calcular(*grafo, codigoOrigen, codigoDestino, porCosto, ruta, cantidadRuta, costoTotal, duracionTotal);
		resultadoCalculado = true;
		if (rutaEncontrada)
		{
			ostringstream costo, duracion;
			costo << fixed << setprecision(0) << costoTotal;
			duracion << fixed << setprecision(1) << duracionTotal;
			int escalas = cantidadRuta > 2 ? cantidadRuta - 2 : 0;
			mensaje = "Costo: " + costo.str() + "   Duracion: " + duracion.str() + " h   Escalas: " + to_string(escalas);
		}
		else mensaje = "No existe una ruta entre esas dos ciudades.";
		return;
	}
	if (dentro(x, y, LIMPIAR_X, LIMPIAR_Y, LIMPIAR_ANCHO, LIMPIAR_ALTO))
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
			mensaje = "Destino seleccionado. Presiona BUSCAR RUTA.";
		}
		limpiarResultado();
	}
}

void PantallaBuscarRutas::actualizar(float tiempo)
{
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
	for (int i = 0; i < cantidadRuta - 1; i++)
	{
		NodoVertice* a = buscarCiudadPorNombre(ruta[i]);
		NodoVertice* b = buscarCiudadPorNombre(ruta[i + 1]);
		if (a == nullptr || b == nullptr) continue;
		Vector2f p1 = posicionEnMapa(a->ciudad.getPosicionX(), a->ciudad.getPosicionY());
		Vector2f p2 = posicionEnMapa(b->ciudad.getPosicionX(), b->ciudad.getPosicionY());
		dibujarLinea(ventana, p1, p2, 9.0f, Color(10, 76, 55));
		dibujarLinea(ventana, p1, p2, 5.0f, Color::White);
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
		
		CircleShape punto(8);
		punto.setOrigin(8, 8);
		punto.setPosition(pos);
		punto.setFillColor(color);
		punto.setOutlineThickness(2);
		punto.setOutlineColor(Color::White);
		ventana.draw(punto);
		
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
	else dibujarTexto(ventana, MAPA_X + 15, MAPA_Y + 15, "No se encontro resources/mapa.png", 14, Color(160, 70, 60), true);
	
	dibujarRuta(ventana);
	dibujarMarcadores(ventana);
	
	float ly = MAPA_Y + MAPA_ALTO - 28;
	caja(ventana, MAPA_X + 8, ly, 255, 22, Color(255, 255, 255), Color(215, 225, 220), 1);
	
	CircleShape punto(5);
	punto.setOrigin(5, 5);
	float cy = ly + 11;
	punto.setPosition(MAPA_X + 22, cy); punto.setFillColor(Color(35, 155, 75)); ventana.draw(punto);
	dibujarTexto(ventana, MAPA_X + 32, ly + 4, "Origen", 11, Color(45, 65, 50));
	punto.setPosition(MAPA_X + 100, cy); punto.setFillColor(Color(40, 110, 205)); ventana.draw(punto);
	dibujarTexto(ventana, MAPA_X + 110, ly + 4, "Destino", 11, Color(45, 65, 50));
	punto.setPosition(MAPA_X + 185, cy); punto.setFillColor(Color(235, 155, 35)); ventana.draw(punto);
	dibujarTexto(ventana, MAPA_X + 195, ly + 4, "Escala", 11, Color(45, 65, 50));
}

void PantallaBuscarRutas::dibujar(RenderWindow& ventana)
{
	Color verdeOscuro(10, 76, 55), verde(42, 112, 72), verdeClaro(225, 242, 224);
	Color azulClaro(222, 235, 248), textoOscuro(35, 65, 52), gris(105, 115, 108);
	Color blanco(255, 255, 255), azul(35, 91, 145), borde(215, 225, 220), normal(250, 252, 250);
	Vector2i mouse = Mouse::getPosition(ventana);
	ventana.setView(ventana.getDefaultView());
	
	caja(ventana, 300, 0, 1200, 800, Color(247, 250, 247), Color(247, 250, 247), 0);
	dibujarPanel(ventana, 320, 22, 1155, 130, verdeOscuro);
	dibujarTexto(ventana, 355, 48, "BUSCAR RUTAS", 34, blanco, true);
	dibujarTexto(ventana, 357, 96, "Selecciona ciudades y visualiza el recorrido sobre el mapa", 19, Color(230, 245, 235));
	
	dibujarPanel(ventana, X0, PY, PANEL_IZQ_ANCHO, PH, blanco);
	dibujarTexto(ventana, 340, PY + 17, "CIUDADES", 21, verdeOscuro, true);
	dibujarTexto(ventana, 340, PY + 47, "Selecciona origen o destino", 13, gris);
	if (grafo == nullptr || grafo->getCantidadVertices() == 0)
		dibujarTexto(ventana, LISTA_X, LISTA_Y0 + 10, "No hay ciudades registradas.", 14, gris);
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
				string nombre = recortar(fuente, fuenteCargada, ciudad->ciudad.getNombre(), 15, esOrigen || esDestino, 170);
				dibujarTexto(ventana, LISTA_X + 10, y + 6, nombre, 15, textoOscuro, esOrigen || esDestino);
				dibujarTexto(ventana, LISTA_X + LISTA_ANCHO - 40, y + 8, ciudad->ciudad.getCodigo(), 11, gris);
				if (esOrigen) dibujarTexto(ventana, LISTA_X + 10, y + 26, "ORIGEN", 9, verde, true);
				else if (esDestino) dibujarTexto(ventana, LISTA_X + 10, y + 26, "DESTINO", 9, azul, true);
				fila++;
			}
			ciudad = ciudad->siguiente; indice++;
		}
		if (grafo->getCantidadVertices() > FILAS_VISIBLES)
			dibujarTexto(ventana, 340, PY + PH - 28, "Usa la rueda del mouse para ver mas.", 11, gris);
	}
	
	dibujarPanel(ventana, PANEL_DER_X, PY, PANEL_DER_ANCHO, PH, blanco);
	NodoVertice* origen = (grafo && !codigoOrigen.empty()) ? grafo->buscarCiudad(codigoOrigen) : nullptr;
	NodoVertice* destino = (grafo && !codigoDestino.empty()) ? grafo->buscarCiudad(codigoDestino) : nullptr;
	string nombreOrigen = origen ? origen->ciudad.getNombre() : "Seleccionar origen";
	string nombreDestino = destino ? destino->ciudad.getNombre() : "Seleccionar destino";
	caja(ventana, ORIGEN_X, CAMPO_Y, CAMPO_ANCHO, CAMPO_ALTO, seleccionandoOrigen ? verdeClaro : normal, seleccionandoOrigen ? verde : borde, seleccionandoOrigen ? 2 : 1);
	caja(ventana, DESTINO_X, CAMPO_Y, CAMPO_ANCHO, CAMPO_ALTO, !seleccionandoOrigen ? azulClaro : normal, !seleccionandoOrigen ? azul : borde, !seleccionandoOrigen ? 2 : 1);
	dibujarTexto(ventana, ORIGEN_X + 14, CAMPO_Y + 5, "ORIGEN", 10, gris, true);
	dibujarTexto(ventana, ORIGEN_X + 14, CAMPO_Y + 22, recortar(fuente, fuenteCargada, nombreOrigen, 16, true, CAMPO_ANCHO - 28), 16, textoOscuro, true);
	dibujarTexto(ventana, DESTINO_X + 14, CAMPO_Y + 5, "DESTINO", 10, gris, true);
	dibujarTexto(ventana, DESTINO_X + 14, CAMPO_Y + 22, recortar(fuente, fuenteCargada, nombreDestino, 16, true, CAMPO_ANCHO - 28), 16, textoOscuro, true);
	
	bool hCosto = dentro(mouse.x, mouse.y, COSTO_X, BOTON_Y, BOTON_ANCHO, BOTON_ALTO);
	bool hRapido = dentro(mouse.x, mouse.y, RAPIDO_X, BOTON_Y, BOTON_ANCHO, BOTON_ALTO);
	bool hBuscar = dentro(mouse.x, mouse.y, BUSCAR_X, BOTON_Y, BUSCAR_ANCHO, BOTON_ALTO);
	caja(ventana, COSTO_X, BOTON_Y, BOTON_ANCHO, BOTON_ALTO, porCosto ? verdeClaro : (hCosto ? Color(238,244,238) : normal), porCosto ? verde : borde, porCosto ? 2 : 1);
	caja(ventana, RAPIDO_X, BOTON_Y, BOTON_ANCHO, BOTON_ALTO, !porCosto ? azulClaro : (hRapido ? Color(238,244,248) : normal), !porCosto ? azul : borde, !porCosto ? 2 : 1);
	caja(ventana, BUSCAR_X, BOTON_Y, BUSCAR_ANCHO, BOTON_ALTO, hBuscar ? Color(30,88,56) : verde, verde, 0);
	textoCentrado(ventana, fuente, fuenteCargada, "MENOR COSTO", COSTO_X + BOTON_ANCHO/2, BOTON_Y + BOTON_ALTO/2, 14, textoOscuro, true);
	textoCentrado(ventana, fuente, fuenteCargada, "MENOR DURACION", RAPIDO_X + BOTON_ANCHO/2, BOTON_Y + BOTON_ALTO/2, 14, textoOscuro, true);
	textoCentrado(ventana, fuente, fuenteCargada, "BUSCAR RUTA", BUSCAR_X + BUSCAR_ANCHO/2, BOTON_Y + BOTON_ALTO/2, 16, blanco, true);
	
	caja(ventana, MENSAJE_X, MENSAJE_Y, MENSAJE_ANCHO, MENSAJE_ALTO, Color(245,248,245), borde, 1);
	dibujarTexto(ventana, MENSAJE_X + 12, MENSAJE_Y + 9, recortar(fuente, fuenteCargada, mensaje, 13, false, MENSAJE_ANCHO - 24), 13, textoOscuro);
	bool hLimpiar = dentro(mouse.x, mouse.y, LIMPIAR_X, LIMPIAR_Y, LIMPIAR_ANCHO, LIMPIAR_ALTO);
	caja(ventana, LIMPIAR_X, LIMPIAR_Y, LIMPIAR_ANCHO, LIMPIAR_ALTO, hLimpiar ? Color(222,230,222) : Color(235,239,235), borde, 1);
	textoCentrado(ventana, fuente, fuenteCargada, "LIMPIAR", LIMPIAR_X + LIMPIAR_ANCHO/2, LIMPIAR_Y + LIMPIAR_ALTO/2, 12, textoOscuro, true);
	
	dibujarMapa(ventana);
}
