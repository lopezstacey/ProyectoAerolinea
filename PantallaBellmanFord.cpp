#include "PantallaBellmanFord.h"
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
	const float LISTA_Y = 260.0f;
	const float FILA_PASO = 51.0f;
	const float FILA_ALTO = 44.0f;
	const int FILAS = 9;
	
	const float CONT_X = 640.0f;
	const float CONT_ANCHO = 815.0f;
	
	const float BOTON_Y = 190.0f;
	const float BOTON_ALTO = 42.0f;
	const float BOTON_ANCHO = 200.0f;
	const float COSTO_X = CONT_X;
	const float RAPIDO_X = COSTO_X + BOTON_ANCHO + 15.0f;
	const float CALCULAR_X = RAPIDO_X + BOTON_ANCHO + 15.0f;
	const float CALCULAR_ANCHO = CONT_X + CONT_ANCHO - CALCULAR_X;
	
	const float ETIQUETA_Y = 244.0f;
	
	const float GRID_X = CONT_X;
	const float GRID_Y = 268.0f;
	const float CELDA_ANCHO = 155.0f;
	const float CELDA_ALTO = 34.0f;
	const float CELDA_PASO_X = 165.0f;
	const float CELDA_PASO_Y = 42.0f;
	const int COLUMNAS = 5;
	const int FILAS_GRID = 2;
	const float GRID_ALTO = (FILAS_GRID - 1) * CELDA_PASO_Y + CELDA_ALTO;
	
	const float MENSAJE_X = CONT_X;
	const float MENSAJE_Y = 354.0f;
	const float MENSAJE_ALTO = 32.0f;
	
	const float MAPA_X = CONT_X;
	const float MAPA_Y = 396.0f;
	const float MAPA_ANCHO = CONT_ANCHO;
	const float MAPA_ALTO = 339.0f;
	const float REF_ANCHO = 959.0f;
	const float REF_ALTO = 500.0f;
	
	float escalaMapa() { return std::min(MAPA_ANCHO / REF_ANCHO, MAPA_ALTO / REF_ALTO); }
	
	float mapaOrigenX() { return MAPA_X + (MAPA_ANCHO - REF_ANCHO * escalaMapa()) / 2.0f; }
	
	float mapaOrigenY() { return MAPA_Y + (MAPA_ALTO - REF_ALTO * escalaMapa()) / 2.0f; }
	
	Vector2f posicionEnMapa(float px, float py) { float e = escalaMapa(); return Vector2f(mapaOrigenX() + px * e, mapaOrigenY() + py * e); }
	
	void dibujarLinea(RenderWindow& w, Vector2f a, Vector2f b, float grosor, Color color)
	{
		float dx = b.x - a.x, dy = b.y - a.y;
		RectangleShape l(Vector2f(std::sqrt(dx * dx + dy * dy), grosor));
		l.setOrigin(0, grosor / 2);
		l.setPosition(a);
		l.setRotation(std::atan2(dy, dx) * 180.0f / 3.14159265f);
		l.setFillColor(color);
		w.draw(l);
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
	
	void dibujarCaja(RenderWindow& w, float x, float y, float a, float h, Color fondo, Color borde = Color(215, 225, 220), float grosor = 1.0f)
	{
		RectangleShape r(Vector2f(a, h));
		r.setPosition(x, y);
		r.setFillColor(fondo);
		r.setOutlineThickness(grosor);
		r.setOutlineColor(borde);
		w.draw(r);
	}
	
	void textoCentrado(RenderWindow& w, const Font& fuente, bool cargada, const string& texto, float cx, float cy, unsigned int tamano, Color color, bool negrita)
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
		w.draw(t);
	}
}

PantallaBellmanFord::PantallaBellmanFord(Grafo* grafo)
{
	this->grafo = grafo;
	fuenteCargada = fuente.loadFromFile("resources/times.ttf");
	mapaCargado = texturaMapa.loadFromFile("resources/mapa.png");
	if (mapaCargado)
		mapa.setTexture(texturaMapa);
	
	codigoOrigen = "";
	porCosto = true;
	desplazamientoCiudades = 0;
	resultadoSeleccionado = -1;
	desplazamientoResultados = 0;
	resultados = nullptr;
	cantidadResultados = 0;
	calculado = false;
	mensaje = "Selecciona una ciudad de origen para calcular destinos alcanzables.";
}

PantallaBellmanFord::~PantallaBellmanFord()
{
	liberarResultados();
}

void PantallaBellmanFord::liberarResultados()
{
	if (resultados != nullptr)
	{
		for (int i = 0; i < cantidadResultados; i++)
			delete[] resultados[i].ruta;
		delete[] resultados;
	}
	resultados = nullptr;
	cantidadResultados = 0;
	resultadoSeleccionado = -1;
	desplazamientoResultados = 0;
	calculado = false;
}

void PantallaBellmanFord::calcularResultados()
{
	liberarResultados();
	
	if (grafo == nullptr || codigoOrigen.empty())
	{
		mensaje = "Selecciona primero una ciudad de origen.";
		return;
	}
	
	if (!algoritmo.calcular(*grafo, codigoOrigen, porCosto, resultados, cantidadResultados))
	{
		mensaje = "No se pudo calcular: revisa el origen o posibles ciclos negativos.";
		return;
	}
	
	calculado = true;
	resultadoSeleccionado = -1;
	
	int destinos = cantidadResultados > 0 ? cantidadResultados - 1 : 0;
	mensaje = "Bellman-Ford encontro " + to_string(destinos) + " destinos alcanzables. Selecciona uno para ver su ruta.";
}

void PantallaBellmanFord::dibujarTexto(RenderWindow& w, float x, float y, const string& texto, unsigned int tamano, Color color, bool negrita)
{
	if (!fuenteCargada)
		return;
	
	Text t;
	t.setFont(fuente);
	t.setString(texto);
	t.setCharacterSize(tamano);
	t.setFillColor(color);
	if (negrita)
		t.setStyle(Text::Bold);
	t.setPosition(x, y);
	w.draw(t);
}

void PantallaBellmanFord::procesarEventos(RenderWindow& ventana, Event evento)
{
	if (evento.type == Event::MouseWheelScrolled)
	{
		Vector2i m = Mouse::getPosition(ventana);
		if (grafo != nullptr && dentro(m.x, m.y, X0, PY, PANEL_IZQ_ANCHO, PH))
		{
			int total = grafo->getCantidadVertices();
			int maximo = total > FILAS ? total - FILAS : 0;
			if (evento.mouseWheelScroll.delta < 0 && desplazamientoCiudades < maximo)
				desplazamientoCiudades++;
			else if (evento.mouseWheelScroll.delta > 0 && desplazamientoCiudades > 0)
				desplazamientoCiudades--;
		}
		else if (calculado && dentro(m.x, m.y, GRID_X, GRID_Y, CONT_ANCHO, GRID_ALTO))
		{
			int destinos = cantidadResultados > 0 ? cantidadResultados - 1 : 0;
			int visibles = COLUMNAS * FILAS_GRID;
			int maximo = destinos > visibles ? destinos - visibles : 0;
			if (evento.mouseWheelScroll.delta < 0 && desplazamientoResultados < maximo)
				desplazamientoResultados++;
			else if (evento.mouseWheelScroll.delta > 0 && desplazamientoResultados > 0)
				desplazamientoResultados--;
		}
		return;
	}
	
	if (evento.type != Event::MouseButtonPressed || evento.mouseButton.button != Mouse::Left)
		return;
	
	int x = evento.mouseButton.x;
	int y = evento.mouseButton.y;
	
	if (dentro(x, y, COSTO_X, BOTON_Y, BOTON_ANCHO, BOTON_ALTO))
	{
		porCosto = true;
		if (!codigoOrigen.empty())
			calcularResultados();
		else
			mensaje = "Selecciona una ciudad de origen.";
		return;
	}
	
	if (dentro(x, y, RAPIDO_X, BOTON_Y, BOTON_ANCHO, BOTON_ALTO))
	{
		porCosto = false;
		if (!codigoOrigen.empty())
			calcularResultados();
		else
			mensaje = "Selecciona una ciudad de origen.";
		return;
	}
	
	if (dentro(x, y, CALCULAR_X, BOTON_Y, CALCULAR_ANCHO, BOTON_ALTO))
	{
		calcularResultados();
		return;
	}
	
	if (grafo != nullptr && dentro(x, y, LISTA_X, LISTA_Y, LISTA_ANCHO, FILAS * FILA_PASO))
	{
		int fila = static_cast<int>((y - LISTA_Y) / FILA_PASO);
		float dentroFila = (y - LISTA_Y) - fila * FILA_PASO;
		if (fila < 0 || fila >= FILAS || dentroFila > FILA_ALTO)
			return;
		int indiceObjetivo = desplazamientoCiudades + fila;
		NodoVertice* actual = grafo->getPrimero();
		int indice = 0;
		while (actual != nullptr && indice < indiceObjetivo)
		{
			actual = actual->siguiente;
			indice++;
		}
		
		if (actual != nullptr)
		{
			codigoOrigen = actual->ciudad.getCodigo();
			calcularResultados();
		}
		return;
	}
	
	if (calculado && resultados != nullptr && dentro(x, y, GRID_X, GRID_Y, CONT_ANCHO, GRID_ALTO))
	{
		int columna = static_cast<int>((x - GRID_X) / CELDA_PASO_X);
		int fila = static_cast<int>((y - GRID_Y) / CELDA_PASO_Y);
		float dentroX = (x - GRID_X) - columna * CELDA_PASO_X;
		float dentroY = (y - GRID_Y) - fila * CELDA_PASO_Y;
		if (columna >= COLUMNAS || fila >= FILAS_GRID || dentroX > CELDA_ANCHO || dentroY > CELDA_ALTO)
			return;
		int objetivo = desplazamientoResultados + fila * COLUMNAS + columna;
		int vistos = 0;
		
		for (int i = 0; i < cantidadResultados; i++)
		{
			if (resultados[i].codigoCiudad == codigoOrigen)
				continue;
			
			if (vistos == objetivo)
			{
				resultadoSeleccionado = i;
				mensaje = "Ruta seleccionada: " + resultados[i].nombreCiudad;
				return;
			}
			vistos++;
		}
	}
}

void PantallaBellmanFord::actualizar(float tiempo)
{
}

void PantallaBellmanFord::dibujarRutaSeleccionada(RenderWindow& ventana)
{
	if (grafo == nullptr || resultados == nullptr || resultadoSeleccionado < 0 || resultadoSeleccionado >= cantidadResultados)
		return;
	
	ResultadoBellmanFord& r = resultados[resultadoSeleccionado];
	if (r.cantidadRuta < 2 || r.ruta == nullptr)
		return;
	
	for (int i = 0; i < r.cantidadRuta - 1; i++)
	{
		NodoVertice* a = nullptr;
		NodoVertice* b = nullptr;
		
		NodoVertice* actual = grafo->getPrimero();
		while (actual != nullptr)
		{
			if (actual->ciudad.getNombre() == r.ruta[i])
				a = actual;
			if (actual->ciudad.getNombre() == r.ruta[i + 1])
				b = actual;
			actual = actual->siguiente;
		}
		
		if (a == nullptr || b == nullptr)
			continue;
		
		Vector2f pa = posicionEnMapa(a->ciudad.getPosicionX(), a->ciudad.getPosicionY());
		Vector2f pb = posicionEnMapa(b->ciudad.getPosicionX(), b->ciudad.getPosicionY());
		dibujarLinea(ventana, pa, pb, 9.0f, Color(10, 76, 55));
		dibujarLinea(ventana, pa, pb, 5.0f, Color::White);
	}
}

void PantallaBellmanFord::dibujarMarcadores(RenderWindow& ventana)
{
	if (grafo == nullptr)
		return;
	
	NodoVertice* actual = grafo->getPrimero();
	while (actual != nullptr)
	{
		Vector2f pos = posicionEnMapa(actual->ciudad.getPosicionX(), actual->ciudad.getPosicionY());
		
		Color c(105, 120, 110);
		if (actual->ciudad.getCodigo() == codigoOrigen)
			c = Color(35, 155, 75);
		
		if (resultados != nullptr && resultadoSeleccionado >= 0 && resultadoSeleccionado < cantidadResultados)
		{
			ResultadoBellmanFord& r = resultados[resultadoSeleccionado];
			for (int i = 1; i < r.cantidadRuta - 1; i++)
				if (r.ruta[i] == actual->ciudad.getNombre())
					c = Color(235, 155, 35);
			if (r.codigoCiudad == actual->ciudad.getCodigo())
				c = Color(40, 110, 205);
		}
		
		CircleShape punto(8);
		punto.setOrigin(8, 8);
		punto.setPosition(pos);
		punto.setFillColor(c);
		punto.setOutlineThickness(2);
		punto.setOutlineColor(Color::White);
		ventana.draw(punto);
		
		actual = actual->siguiente;
	}
}

void PantallaBellmanFord::dibujarMapa(RenderWindow& ventana)
{
	dibujarCaja(ventana, MAPA_X, MAPA_Y, MAPA_ANCHO, MAPA_ALTO, Color(248, 250, 247));
	
	if (mapaCargado)
	{
		Vector2u t = texturaMapa.getSize();
		if (t.x > 0 && t.y > 0)
		{
			float e = escalaMapa();
			mapa.setPosition(mapaOrigenX(), mapaOrigenY());
			mapa.setScale(REF_ANCHO * e / static_cast<float>(t.x), REF_ALTO * e / static_cast<float>(t.y));
			ventana.draw(mapa);
		}
	}
	else
		dibujarTexto(ventana, MAPA_X + 15, MAPA_Y + 15, "No se pudo cargar resources/mapa.png", 14, Color(150, 70, 60), true);
	
	dibujarRutaSeleccionada(ventana);
	dibujarMarcadores(ventana);
	
	float ly = MAPA_Y + MAPA_ALTO - 28;
	dibujarCaja(ventana, MAPA_X + 8, ly, 255, 22, Color(255, 255, 255));
	
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

void PantallaBellmanFord::dibujar(RenderWindow& ventana)
{
	Color verdeOscuro(10, 76, 55), verde(42, 112, 72), verdeClaro(225, 242, 224), azulClaro(222, 235, 248), azul(35, 91, 145);
	Color textoOscuro(35, 65, 52), gris(105, 115, 108), borde(215, 225, 220), normal(250, 252, 250), blanco(255, 255, 255);
	Vector2i mouse = Mouse::getPosition(ventana);
	ventana.setView(ventana.getDefaultView());
	
	dibujarCaja(ventana, 300, 0, 1200, 800, Color(247, 250, 247), Color(247, 250, 247), 0);
	dibujarCaja(ventana, 320, 22, 1155, 130, verdeOscuro);
	dibujarTexto(ventana, 355, 48, "DESTINOS ALCANZABLES", 34, blanco, true);
	dibujarTexto(ventana, 357, 96, "Calcula desde una ciudad las rutas minimas hacia todos los destinos", 19, Color(230, 245, 235));
	
	dibujarCaja(ventana, X0, PY, PANEL_IZQ_ANCHO, PH, blanco);
	dibujarTexto(ventana, 340, PY + 17, "CIUDAD DE ORIGEN", 21, verdeOscuro, true);
	dibujarTexto(ventana, 340, PY + 47, "Selecciona una ciudad", 13, gris);
	
	NodoVertice* ciudad = grafo == nullptr ? nullptr : grafo->getPrimero();
	if (ciudad == nullptr)
		dibujarTexto(ventana, LISTA_X, LISTA_Y + 10, "No hay ciudades registradas.", 14, gris);
	
	int indice = 0;
	int fila = 0;
	while (ciudad != nullptr && fila < FILAS)
	{
		if (indice >= desplazamientoCiudades)
		{
			float y = LISTA_Y + fila * FILA_PASO;
			bool seleccionada = ciudad->ciudad.getCodigo() == codigoOrigen;
			dibujarCaja(ventana, LISTA_X, y, LISTA_ANCHO, FILA_ALTO, seleccionada ? verdeClaro : normal, Color(225, 232, 226));
			string nombre = recortar(fuente, fuenteCargada, ciudad->ciudad.getNombre(), 15, seleccionada, 170);
			dibujarTexto(ventana, LISTA_X + 10, y + 12, nombre, 15, seleccionada ? verdeOscuro : textoOscuro, seleccionada);
			dibujarTexto(ventana, LISTA_X + LISTA_ANCHO - 40, y + 15, ciudad->ciudad.getCodigo(), 11, gris);
			fila++;
		}
		ciudad = ciudad->siguiente;
		indice++;
	}
	
	if (grafo != nullptr && grafo->getCantidadVertices() > FILAS)
		dibujarTexto(ventana, 340, PY + PH - 28, "Usa la rueda del mouse para ver mas.", 11, gris);
	
	dibujarCaja(ventana, PANEL_DER_X, PY, PANEL_DER_ANCHO, PH, blanco);
	
	bool hCosto = dentro(mouse.x, mouse.y, COSTO_X, BOTON_Y, BOTON_ANCHO, BOTON_ALTO);
	bool hRapido = dentro(mouse.x, mouse.y, RAPIDO_X, BOTON_Y, BOTON_ANCHO, BOTON_ALTO);
	bool hCalcular = dentro(mouse.x, mouse.y, CALCULAR_X, BOTON_Y, CALCULAR_ANCHO, BOTON_ALTO);
	dibujarCaja(ventana, COSTO_X, BOTON_Y, BOTON_ANCHO, BOTON_ALTO, porCosto ? verdeClaro : (hCosto ? Color(238, 244, 238) : normal), porCosto ? verde : borde, porCosto ? 2 : 1);
	dibujarCaja(ventana, RAPIDO_X, BOTON_Y, BOTON_ANCHO, BOTON_ALTO, !porCosto ? azulClaro : (hRapido ? Color(238, 244, 248) : normal), !porCosto ? azul : borde, !porCosto ? 2 : 1);
	dibujarCaja(ventana, CALCULAR_X, BOTON_Y, CALCULAR_ANCHO, BOTON_ALTO, hCalcular ? Color(30, 88, 56) : verde, verde, 0);
	textoCentrado(ventana, fuente, fuenteCargada, "MENOR COSTO", COSTO_X + BOTON_ANCHO / 2, BOTON_Y + BOTON_ALTO / 2, 14, textoOscuro, true);
	textoCentrado(ventana, fuente, fuenteCargada, "MENOR DURACION", RAPIDO_X + BOTON_ANCHO / 2, BOTON_Y + BOTON_ALTO / 2, 14, textoOscuro, true);
	textoCentrado(ventana, fuente, fuenteCargada, "CALCULAR DESTINOS", CALCULAR_X + CALCULAR_ANCHO / 2, BOTON_Y + BOTON_ALTO / 2, 16, blanco, true);
	
	dibujarTexto(ventana, CONT_X, ETIQUETA_Y, "DESTINOS ALCANZABLES (selecciona uno)", 15, verdeOscuro, true);
	
	int celda = 0;
	int omitidos = 0;
	if (calculado && resultados != nullptr)
	{
		for (int i = 0; i < cantidadResultados && celda < COLUMNAS * FILAS_GRID; i++)
		{
			if (resultados[i].codigoCiudad == codigoOrigen)
				continue;
			
			if (omitidos < desplazamientoResultados)
			{
				omitidos++;
				continue;
			}
			
			float x = GRID_X + (celda % COLUMNAS) * CELDA_PASO_X;
			float y = GRID_Y + (celda / COLUMNAS) * CELDA_PASO_Y;
			bool seleccionado = i == resultadoSeleccionado;
			
			dibujarCaja(ventana, x, y, CELDA_ANCHO, CELDA_ALTO, seleccionado ? verdeClaro : normal, seleccionado ? verde : borde);
			textoCentrado(ventana, fuente, fuenteCargada, recortar(fuente, fuenteCargada, resultados[i].nombreCiudad, 13, seleccionado, CELDA_ANCHO - 14), x + CELDA_ANCHO / 2, y + CELDA_ALTO / 2, 13, textoOscuro, seleccionado);
			celda++;
		}
		if (celda == 0)
			dibujarTexto(ventana, GRID_X + 10, GRID_Y + 8, "No hay otros destinos alcanzables.", 13, gris);
	}
	else
		dibujarTexto(ventana, GRID_X + 10, GRID_Y + 8, "Selecciona origen y calcula para ver las opciones.", 13, gris);
	
	dibujarCaja(ventana, MENSAJE_X, MENSAJE_Y, CONT_ANCHO, MENSAJE_ALTO, Color(245, 248, 245), borde);
	bool hayInfo = resultados != nullptr && resultadoSeleccionado >= 0 && resultadoSeleccionado < cantidadResultados;
	dibujarTexto(ventana, MENSAJE_X + 12, MENSAJE_Y + 8, recortar(fuente, fuenteCargada, mensaje, 13, false, hayInfo ? 500.0f : CONT_ANCHO - 24), 13, textoOscuro);
	if (hayInfo)
	{
		ResultadoBellmanFord& r = resultados[resultadoSeleccionado];
		ostringstream s;
		s << fixed << setprecision(1) << (porCosto ? "Costo: " : "Duracion: ") << r.distancia << "   |   Ruta: " << r.cantidadRuta << " ciudades";
		float anchoInfo = anchoTexto(fuente, s.str(), 13, true);
		dibujarTexto(ventana, MENSAJE_X + CONT_ANCHO - 12 - anchoInfo, MENSAJE_Y + 8, s.str(), 13, verdeOscuro, true);
	}
	
	dibujarMapa(ventana);
}
