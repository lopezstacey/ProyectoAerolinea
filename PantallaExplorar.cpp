#include "PantallaExplorar.h"
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <cmath>
#include <cctype>

namespace
{
	const Color FONDO(244, 247, 245);
	const Color BLANCO(255, 255, 255);
	const Color BORDE(226, 233, 228);
	const Color LINEA(236, 241, 237);
	const Color VERDE_OSCURO(10, 76, 55);
	const Color VERDE(42, 112, 72);
	const Color VERDE_CLARO(228, 243, 229);
	const Color TEXTO(35, 65, 52);
	const Color GRIS(115, 125, 118);
	const Color ENCABEZADO_TABLA(240, 246, 241);
	const Color SEGMENTO_FONDO(231, 238, 233);
	const Color HOVER(243, 248, 244);
	
	const Color NAVY(12, 31, 56);
	const Color NAVY_SUAVE(12, 31, 56, 205);
	const Color TEXTO_CLARO(228, 236, 244);
	const Color COLOR_ORIGEN(46, 204, 113);
	const Color COLOR_DESTINO(235, 87, 87);
	const Color COLOR_ESCALA(255, 196, 60);
	const Color COLOR_NEUTRO(255, 255, 255, 215);
	
	const float SEG_CONT_X = 1132.0f;
	const float SEG_CONT_Y = 22.0f;
	const float SEG_CONT_ANCHO = 348.0f;
	const float SEG_CONT_ALTO = 38.0f;
	const float SEG_A_X = 1136.0f;
	const float SEG_B_X = 1306.0f;
	const float SEG_Y = 26.0f;
	const float SEG_ANCHO = 170.0f;
	const float SEG_ALTO = 30.0f;
	
	const float IZQ_X = 320.0f;
	const float IZQ_Y = 90.0f;
	const float IZQ_ANCHO = 270.0f;
	const float IZQ_ALTO = 690.0f;
	
	const float BUSCADOR_X = 336.0f;
	const float BUSCADOR_Y = 134.0f;
	const float BUSCADOR_ANCHO = 238.0f;
	const float BUSCADOR_ALTO = 34.0f;
	
	const float LISTA_X = 336.0f;
	const float LISTA_ANCHO = 238.0f;
	const float LISTA_Y = 180.0f;
	const float FILA_PASO = 44.0f;
	const float FILA_ALTO = 38.0f;
	const int FILAS = 12;
	
	const float DER_X = 610.0f;
	const float DER_ANCHO = 870.0f;
	
	const float MAPA_X = DER_X;
	const float MAPA_Y = 90.0f;
	const float MAPA_ANCHO = DER_ANCHO;
	const float MAPA_ALTO = 454.0f;
	const float REF_ANCHO = 959.0f;
	const float REF_ALTO = 500.0f;
	
	const float OV_X = 626.0f;
	const float OV_Y = MAPA_Y + MAPA_ALTO - 80.0f;
	const float OV_ANCHO = 248.0f;
	const float OV_ALTO = 34.0f;
	const float COSTO_X = 629.0f;
	const float RAPIDO_X = 751.0f;
	const float SEGO_Y = OV_Y + 3.0f;
	const float SEGO_ANCHO = 120.0f;
	const float SEGO_ALTO = 28.0f;
	
	const float BAJO_Y = 556.0f;
	const float BAJO_ALTO = 224.0f;
	const float TABLA_X = 610.0f;
	const float TABLA_ANCHO = 560.0f;
	const float RUTA_X = 1186.0f;
	const float RUTA_ANCHO = 294.0f;
	const float TABLA_FILA_Y = 624.0f;
	const float TABLA_FILA_PASO = 30.0f;
	const float TABLA_FILA_ALTO = 28.0f;
	const int DEST_FILAS = 5;
	
	const float DIR_X = 634.0f;
	const float DIR_ANCHO = 822.0f;
	const float DIR_NOMBRE_Y = 106.0f;
	const float DIR_INFO_Y = 146.0f;
	const float DIR_ENCABEZADO_Y = 178.0f;
	const float DIR_ENCABEZADO_ALTO = 32.0f;
	const float DIR_FILA_Y = 216.0f;
	const float DIR_FILA_PASO = 44.0f;
	const float DIR_FILA_ALTO = 40.0f;
	const int DIR_FILAS = 12;
	
	float escalaMapa() { return std::min(MAPA_ANCHO / REF_ANCHO, MAPA_ALTO / REF_ALTO); }
	
	float mapaOrigenX() { return MAPA_X + (MAPA_ANCHO - REF_ANCHO * escalaMapa()) / 2.0f; }
	
	float mapaOrigenY() { return MAPA_Y + (MAPA_ALTO - REF_ALTO * escalaMapa()) / 2.0f; }
	
	Vector2f posicionEnMapa(float px, float py)
	{
		float e = escalaMapa();
		return Vector2f(mapaOrigenX() + px * e, mapaOrigenY() + py * e);
	}
	
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
	
	void dibujarCaja(RenderWindow& w, float x, float y, float a, float h, Color fondo, Color borde = BORDE, float grosor = 1.0f)
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
	
	string aMinuscula(string texto)
	{
		for (size_t i = 0; i < texto.size(); i++)
			texto[i] = static_cast<char>(tolower(static_cast<unsigned char>(texto[i])));
		return texto;
	}
	
	string formatear(float valor, int decimales)
	{
		ostringstream s;
		s << fixed << setprecision(decimales) << valor;
		return s.str();
	}
	
	NodoVertice* buscarPorNombre(Grafo* grafo, const string& nombre)
	{
		NodoVertice* actual = grafo->getPrimero();
		while (actual != nullptr)
		{
			if (actual->ciudad.getNombre() == nombre)
				return actual;
			actual = actual->siguiente;
		}
		return nullptr;
	}
	
	int contarConexionesTotales(Grafo* grafo)
	{
		int total = 0;
		if (grafo == nullptr)
			return 0;
		
		NodoVertice* v = grafo->getPrimero();
		while (v != nullptr)
		{
			NodoArista* a = v->conexiones;
			while (a != nullptr)
			{
				total++;
				a = a->siguiente;
			}
			v = v->siguiente;
		}
		return total;
	}
	
	int contarConexionesDe(NodoVertice* vertice)
	{
		int total = 0;
		NodoArista* a = vertice->conexiones;
		while (a != nullptr)
		{
			total++;
			a = a->siguiente;
		}
		return total;
	}
}

PantallaExplorar::PantallaExplorar(Grafo* grafo)
{
	this->grafo = grafo;
	fuenteCargada = fuente.loadFromFile("resources/times.ttf");
	mapaCargado = texturaMapa.loadFromFile("resources/mapa.png");
	if (mapaCargado)
		mapa.setTexture(texturaMapa);
	
	textoBusqueda = "";
	codigoOrigen = "";
	modoDirectas = false;
	porCosto = true;
	desplazamientoCiudades = 0;
	desplazamientoConexiones = 0;
	
	resultados = nullptr;
	cantidadResultados = 0;
	resultadoSeleccionado = -1;
	desplazamientoResultados = 0;
	calculado = false;
	mensaje = "Selecciona una ciudad de origen para ver sus destinos alcanzables.";
}

PantallaExplorar::~PantallaExplorar()
{
	liberarResultados();
}

void PantallaExplorar::liberarResultados()
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

void PantallaExplorar::calcularResultados()
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
	mensaje = "Bellman-Ford encontro " + to_string(destinos) + " destinos alcanzables. Elige uno para ver su ruta.";
}

bool PantallaExplorar::coincideBusqueda(NodoVertice* vertice) const
{
	if (textoBusqueda.empty())
		return true;
	
	string filtro = aMinuscula(textoBusqueda);
	string nombre = aMinuscula(vertice->ciudad.getNombre());
	
	return nombre.find(filtro) != string::npos ||
		vertice->ciudad.getCodigo().find(textoBusqueda) != string::npos;
}

int PantallaExplorar::contarCoincidencias() const
{
	if (grafo == nullptr)
		return 0;
	
	int n = 0;
	NodoVertice* v = grafo->getPrimero();
	while (v != nullptr)
	{
		if (coincideBusqueda(v))
			n++;
		v = v->siguiente;
	}
	return n;
}

NodoVertice* PantallaExplorar::ciudadVisible(int posicion) const
{
	if (grafo == nullptr || posicion < 0)
		return nullptr;
	
	int n = 0;
	NodoVertice* v = grafo->getPrimero();
	while (v != nullptr)
	{
		if (coincideBusqueda(v))
		{
			if (n == posicion)
				return v;
			n++;
		}
		v = v->siguiente;
	}
	return nullptr;
}

void PantallaExplorar::procesarEventos(RenderWindow& ventana, Event evento)
{
	if (evento.type == Event::TextEntered)
	{
		if (evento.text.unicode == 8)
		{
			if (!textoBusqueda.empty())
				textoBusqueda.erase(textoBusqueda.size() - 1);
			desplazamientoCiudades = 0;
		}
		else if (evento.text.unicode >= 32 && evento.text.unicode < 127)
		{
			if (textoBusqueda.size() < 35)
				textoBusqueda += static_cast<char>(evento.text.unicode);
			desplazamientoCiudades = 0;
		}
		return;
	}
	
	if (evento.type == Event::MouseWheelScrolled)
	{
		Vector2i m = Mouse::getPosition(ventana);
		int paso = evento.mouseWheelScroll.delta < 0 ? 1 : -1;
		
		if (grafo != nullptr && dentro(m.x, m.y, IZQ_X, IZQ_Y, IZQ_ANCHO, IZQ_ALTO))
		{
			int total = contarCoincidencias();
			int maximo = total > FILAS ? total - FILAS : 0;
			desplazamientoCiudades += paso;
			if (desplazamientoCiudades < 0) desplazamientoCiudades = 0;
			if (desplazamientoCiudades > maximo) desplazamientoCiudades = maximo;
		}
		else if (modoDirectas)
		{
			if (grafo != nullptr && !codigoOrigen.empty() &&
				dentro(m.x, m.y, DER_X, DIR_FILA_Y, DER_ANCHO, DIR_FILAS * DIR_FILA_PASO))
			{
				NodoVertice* sel = grafo->buscarCiudad(codigoOrigen);
				if (sel != nullptr)
				{
					int total = contarConexionesDe(sel);
					int maximo = total > DIR_FILAS ? total - DIR_FILAS : 0;
					desplazamientoConexiones += paso;
					if (desplazamientoConexiones < 0) desplazamientoConexiones = 0;
					if (desplazamientoConexiones > maximo) desplazamientoConexiones = maximo;
				}
			}
		}
		else if (calculado && dentro(m.x, m.y, TABLA_X, BAJO_Y, TABLA_ANCHO, BAJO_ALTO))
		{
			int destinos = cantidadResultados > 0 ? cantidadResultados - 1 : 0;
			int maximo = destinos > DEST_FILAS ? destinos - DEST_FILAS : 0;
			desplazamientoResultados += paso;
			if (desplazamientoResultados < 0) desplazamientoResultados = 0;
			if (desplazamientoResultados > maximo) desplazamientoResultados = maximo;
		}
		return;
	}
	
	if (evento.type != Event::MouseButtonPressed || evento.mouseButton.button != Mouse::Left)
		return;
	
	int x = evento.mouseButton.x;
	int y = evento.mouseButton.y;
	
	if (dentro(x, y, SEG_A_X, SEG_Y, SEG_ANCHO, SEG_ALTO))
	{
		modoDirectas = false;
		return;
	}
	
	if (dentro(x, y, SEG_B_X, SEG_Y, SEG_ANCHO, SEG_ALTO))
	{
		modoDirectas = true;
		return;
	}
	
	if (!modoDirectas)
	{
		if (dentro(x, y, COSTO_X, SEGO_Y, SEGO_ANCHO, SEGO_ALTO))
		{
			porCosto = true;
			if (!codigoOrigen.empty())
				calcularResultados();
			else
				mensaje = "Selecciona una ciudad de origen.";
			return;
		}
		
		if (dentro(x, y, RAPIDO_X, SEGO_Y, SEGO_ANCHO, SEGO_ALTO))
		{
			porCosto = false;
			if (!codigoOrigen.empty())
				calcularResultados();
			else
				mensaje = "Selecciona una ciudad de origen.";
			return;
		}
	}
	
	if (grafo != nullptr && dentro(x, y, LISTA_X, LISTA_Y, LISTA_ANCHO, FILAS * FILA_PASO))
	{
		int fila = static_cast<int>((y - LISTA_Y) / FILA_PASO);
		float dentroFila = (y - LISTA_Y) - fila * FILA_PASO;
		if (fila < 0 || fila >= FILAS || dentroFila > FILA_ALTO)
			return;
		
		NodoVertice* elegida = ciudadVisible(desplazamientoCiudades + fila);
		if (elegida != nullptr)
		{
			codigoOrigen = elegida->ciudad.getCodigo();
			desplazamientoConexiones = 0;
			calcularResultados();
		}
		return;
	}
	
	if (!modoDirectas && calculado && resultados != nullptr &&
		dentro(x, y, TABLA_X, TABLA_FILA_Y, TABLA_ANCHO, DEST_FILAS * TABLA_FILA_PASO))
	{
		int fila = static_cast<int>((y - TABLA_FILA_Y) / TABLA_FILA_PASO);
		float dentroFila = (y - TABLA_FILA_Y) - fila * TABLA_FILA_PASO;
		if (fila < 0 || fila >= DEST_FILAS || dentroFila > TABLA_FILA_ALTO)
			return;
		
		int objetivo = desplazamientoResultados + fila;
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

void PantallaExplorar::actualizar(float tiempo)
{
}

void PantallaExplorar::dibujarTexto(RenderWindow& ventana, float x, float y, const string& texto, unsigned int tamano, Color color, bool negrita)
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
	ventana.draw(t);
}

void PantallaExplorar::dibujarEncabezado(RenderWindow& ventana)
{
	dibujarTexto(ventana, 320, 14, "Explorar destinos", 28, VERDE_OSCURO, true);
	
	string subtitulo = modoDirectas ? "Vuelos directos que salen de la ciudad seleccionada." : mensaje;
	dibujarTexto(ventana, 322, 52, recortar(fuente, fuenteCargada, subtitulo, 13, false, 780), 13, GRIS);
	
	dibujarCaja(ventana, SEG_CONT_X, SEG_CONT_Y, SEG_CONT_ANCHO, SEG_CONT_ALTO, SEGMENTO_FONDO, SEGMENTO_FONDO, 0.0f);
	
	if (!modoDirectas)
		dibujarCaja(ventana, SEG_A_X, SEG_Y, SEG_ANCHO, SEG_ALTO, BLANCO, BORDE, 1.0f);
	else
		dibujarCaja(ventana, SEG_B_X, SEG_Y, SEG_ANCHO, SEG_ALTO, BLANCO, BORDE, 1.0f);
	
	textoCentrado(ventana, fuente, fuenteCargada, "Destinos alcanzables", SEG_A_X + SEG_ANCHO / 2, SEG_Y + SEG_ALTO / 2, 14,
				  !modoDirectas ? VERDE_OSCURO : GRIS, !modoDirectas);
	textoCentrado(ventana, fuente, fuenteCargada, "Conexiones directas", SEG_B_X + SEG_ANCHO / 2, SEG_Y + SEG_ALTO / 2, 14,
				  modoDirectas ? VERDE_OSCURO : GRIS, modoDirectas);
}

void PantallaExplorar::dibujarListaCiudades(RenderWindow& ventana)
{
	Vector2i mouse = Mouse::getPosition(ventana);
	
	dibujarCaja(ventana, IZQ_X, IZQ_Y, IZQ_ANCHO, IZQ_ALTO, BLANCO, BORDE);
	dibujarTexto(ventana, 336, IZQ_Y + 14, "Ciudad de origen", 16, VERDE_OSCURO, true);
	
	dibujarCaja(ventana, BUSCADOR_X, BUSCADOR_Y, BUSCADOR_ANCHO, BUSCADOR_ALTO, FONDO, textoBusqueda.empty() ? BORDE : VERDE, 1.0f);
	if (textoBusqueda.empty())
		dibujarTexto(ventana, BUSCADOR_X + 10, BUSCADOR_Y + 8, "Buscar ciudad...", 14, GRIS);
	else
		dibujarTexto(ventana, BUSCADOR_X + 10, BUSCADOR_Y + 8,
					 recortar(fuente, fuenteCargada, textoBusqueda, 14, false, BUSCADOR_ANCHO - 20), 14, TEXTO);
	
	int totalVertices = grafo != nullptr ? grafo->getCantidadVertices() : 0;
	
	dibujarCaja(ventana, IZQ_X + 16, IZQ_Y + IZQ_ALTO - 40, IZQ_ANCHO - 32, 1, LINEA, LINEA, 0.0f);
	string resumen = to_string(totalVertices) + " ciudades   |   " + to_string(contarConexionesTotales(grafo)) + " conexiones";
	dibujarTexto(ventana, 336, IZQ_Y + IZQ_ALTO - 30, resumen, 12, GRIS);
	
	if (totalVertices == 0)
	{
		dibujarTexto(ventana, LISTA_X, LISTA_Y + 10, "No hay ciudades registradas.", 14, GRIS);
		return;
	}
	
	int coincidencias = contarCoincidencias();
	int maximo = coincidencias > FILAS ? coincidencias - FILAS : 0;
	if (desplazamientoCiudades > maximo)
		desplazamientoCiudades = maximo;
	
	if (coincidencias == 0)
		dibujarTexto(ventana, LISTA_X, LISTA_Y + 10, "No se encontraron ciudades.", 14, GRIS);
	
	NodoVertice* actual = grafo->getPrimero();
	int visto = 0;
	int fila = 0;
	
	while (actual != nullptr && fila < FILAS)
	{
		if (coincideBusqueda(actual))
		{
			if (visto >= desplazamientoCiudades)
			{
				float y = LISTA_Y + fila * FILA_PASO;
				bool seleccionada = actual->ciudad.getCodigo() == codigoOrigen;
				bool encima = dentro(mouse.x, mouse.y, LISTA_X, y, LISTA_ANCHO, FILA_ALTO);
				
				if (seleccionada)
				{
					dibujarCaja(ventana, LISTA_X, y, LISTA_ANCHO, FILA_ALTO, VERDE_CLARO, VERDE_CLARO, 0.0f);
					dibujarCaja(ventana, LISTA_X, y, 4, FILA_ALTO, VERDE, VERDE, 0.0f);
				}
				else if (encima)
					dibujarCaja(ventana, LISTA_X, y, LISTA_ANCHO, FILA_ALTO, HOVER, HOVER, 0.0f);
				
				string nombre = recortar(fuente, fuenteCargada, actual->ciudad.getNombre(), 15, seleccionada, 160);
				dibujarTexto(ventana, LISTA_X + 14, y + 10, nombre, 15, seleccionada ? VERDE_OSCURO : TEXTO, seleccionada);
				dibujarTexto(ventana, LISTA_X + LISTA_ANCHO - 38, y + 13, actual->ciudad.getCodigo(), 11, GRIS);
				fila++;
			}
			visto++;
		}
		actual = actual->siguiente;
	}
	
	if (coincidencias > FILAS)
		dibujarTexto(ventana, 336, IZQ_Y + IZQ_ALTO - 56, "Rueda del mouse para ver mas.", 11, GRIS);
}

void PantallaExplorar::dibujarControles(RenderWindow& ventana)
{
	Vector2i mouse = Mouse::getPosition(ventana);
	
	dibujarCaja(ventana, OV_X, OV_Y, OV_ANCHO, OV_ALTO, NAVY_SUAVE, NAVY_SUAVE, 0.0f);
	
	bool hCosto = dentro(mouse.x, mouse.y, COSTO_X, SEGO_Y, SEGO_ANCHO, SEGO_ALTO);
	bool hRapido = dentro(mouse.x, mouse.y, RAPIDO_X, SEGO_Y, SEGO_ANCHO, SEGO_ALTO);
	
	if (porCosto)
		dibujarCaja(ventana, COSTO_X, SEGO_Y, SEGO_ANCHO, SEGO_ALTO, BLANCO, BLANCO, 0.0f);
	else if (hCosto)
		dibujarCaja(ventana, COSTO_X, SEGO_Y, SEGO_ANCHO, SEGO_ALTO, Color(255, 255, 255, 45), Color(255, 255, 255, 45), 0.0f);
	
	if (!porCosto)
		dibujarCaja(ventana, RAPIDO_X, SEGO_Y, SEGO_ANCHO, SEGO_ALTO, BLANCO, BLANCO, 0.0f);
	else if (hRapido)
		dibujarCaja(ventana, RAPIDO_X, SEGO_Y, SEGO_ANCHO, SEGO_ALTO, Color(255, 255, 255, 45), Color(255, 255, 255, 45), 0.0f);
	
	textoCentrado(ventana, fuente, fuenteCargada, "Menor costo", COSTO_X + SEGO_ANCHO / 2, SEGO_Y + SEGO_ALTO / 2, 13,
				  porCosto ? VERDE_OSCURO : TEXTO_CLARO, porCosto);
	textoCentrado(ventana, fuente, fuenteCargada, "Menor duracion", RAPIDO_X + SEGO_ANCHO / 2, SEGO_Y + SEGO_ALTO / 2, 13,
				  !porCosto ? VERDE_OSCURO : TEXTO_CLARO, !porCosto);
}

void PantallaExplorar::dibujarAlcanzables(RenderWindow& ventana)
{
	dibujarMapa(ventana);
	dibujarControles(ventana);
	dibujarTablaDestinos(ventana);
	dibujarDetalleRuta(ventana);
}

void PantallaExplorar::dibujarTablaDestinos(RenderWindow& ventana)
{
	Vector2i mouse = Mouse::getPosition(ventana);
	
	dibujarCaja(ventana, TABLA_X, BAJO_Y, TABLA_ANCHO, BAJO_ALTO, BLANCO, BORDE);
	
	string titulo = "Destinos alcanzables";
	NodoVertice* origen = (grafo != nullptr && !codigoOrigen.empty()) ? grafo->buscarCiudad(codigoOrigen) : nullptr;
	if (origen != nullptr)
		titulo = "Destinos desde " + origen->ciudad.getNombre();
	dibujarTexto(ventana, TABLA_X + 16, BAJO_Y + 10, recortar(fuente, fuenteCargada, titulo, 15, true, 360), 15, VERDE_OSCURO, true);
	
	int destinos = (calculado && cantidadResultados > 0) ? cantidadResultados - 1 : 0;
	if (destinos > DEST_FILAS)
		dibujarTexto(ventana, TABLA_X + TABLA_ANCHO - 150, BAJO_Y + 13, "Rueda para ver mas", 11, GRIS);
	
	dibujarCaja(ventana, TABLA_X + 1, BAJO_Y + 38, TABLA_ANCHO - 2, 24, ENCABEZADO_TABLA, ENCABEZADO_TABLA, 0.0f);
	dibujarTexto(ventana, TABLA_X + 16, BAJO_Y + 42, "Destino", 12, GRIS, true);
	dibujarTexto(ventana, TABLA_X + 300, BAJO_Y + 42, "Costo", 12, GRIS, true);
	dibujarTexto(ventana, TABLA_X + 390, BAJO_Y + 42, "Duracion", 12, GRIS, true);
	dibujarTexto(ventana, TABLA_X + 480, BAJO_Y + 42, "Escalas", 12, GRIS, true);
	
	if (!calculado || resultados == nullptr)
	{
		dibujarTexto(ventana, TABLA_X + 16, TABLA_FILA_Y + 8, "Elige una ciudad de la lista para ver sus destinos.", 14, GRIS);
		return;
	}
	
	if (destinos == 0)
	{
		dibujarTexto(ventana, TABLA_X + 16, TABLA_FILA_Y + 8, "No hay otros destinos alcanzables.", 14, GRIS);
		return;
	}
	
	int celda = 0;
	int omitidos = 0;
	
	for (int i = 0; i < cantidadResultados && celda < DEST_FILAS; i++)
	{
		if (resultados[i].codigoCiudad == codigoOrigen)
			continue;
		
		if (omitidos < desplazamientoResultados)
		{
			omitidos++;
			continue;
		}
		
		float y = TABLA_FILA_Y + celda * TABLA_FILA_PASO;
		bool seleccionado = i == resultadoSeleccionado;
		bool encima = dentro(mouse.x, mouse.y, TABLA_X, y, TABLA_ANCHO, TABLA_FILA_ALTO);
		
		if (seleccionado)
		{
			dibujarCaja(ventana, TABLA_X + 1, y, TABLA_ANCHO - 2, TABLA_FILA_ALTO, VERDE_CLARO, VERDE_CLARO, 0.0f);
			dibujarCaja(ventana, TABLA_X + 1, y, 4, TABLA_FILA_ALTO, VERDE, VERDE, 0.0f);
		}
		else if (encima)
			dibujarCaja(ventana, TABLA_X + 1, y, TABLA_ANCHO - 2, TABLA_FILA_ALTO, HOVER, HOVER, 0.0f);
		
		int escalas = resultados[i].cantidadRuta > 2 ? resultados[i].cantidadRuta - 2 : 0;
		Color c = seleccionado ? VERDE_OSCURO : TEXTO;
		
		dibujarTexto(ventana, TABLA_X + 16, y + 5, recortar(fuente, fuenteCargada, resultados[i].nombreCiudad, 14, seleccionado, 260), 14, c, seleccionado);
		dibujarTexto(ventana, TABLA_X + 300, y + 5, formatear(resultados[i].costoTotal, 0), 14, c, seleccionado);
		dibujarTexto(ventana, TABLA_X + 390, y + 5, formatear(resultados[i].duracionTotal, 1) + " h", 14, c, seleccionado);
		dibujarTexto(ventana, TABLA_X + 480, y + 5, escalas == 0 ? "Directo" : to_string(escalas), 14, c, seleccionado);
		celda++;
	}
}

void PantallaExplorar::dibujarDetalleRuta(RenderWindow& ventana)
{
	dibujarCaja(ventana, RUTA_X, BAJO_Y, RUTA_ANCHO, BAJO_ALTO, BLANCO, BORDE);
	dibujarTexto(ventana, RUTA_X + 16, BAJO_Y + 10, "Ruta seleccionada", 15, VERDE_OSCURO, true);
	
	bool hayRuta = resultados != nullptr && resultadoSeleccionado >= 0 && resultadoSeleccionado < cantidadResultados;
	
	if (!hayRuta)
	{
		dibujarTexto(ventana, RUTA_X + 16, BAJO_Y + 48, "Elige un destino de la tabla", 14, GRIS);
		dibujarTexto(ventana, RUTA_X + 16, BAJO_Y + 68, "para ver sus escalas y totales.", 14, GRIS);
		return;
	}
	
	ResultadoBellmanFord& r = resultados[resultadoSeleccionado];
	int n = r.cantidadRuta;
	
	int indices[6];
	int cantidad = 0;
	if (n <= 6)
	{
		for (int i = 0; i < n; i++)
			indices[cantidad++] = i;
	}
	else
	{
		for (int i = 0; i < 4; i++)
			indices[cantidad++] = i;
		indices[cantidad++] = -1;
		indices[cantidad++] = n - 1;
	}
	
	float y0 = BAJO_Y + 46;
	float paso = 22.0f;
	
	for (int k = 0; k < cantidad; k++)
	{
		float y = y0 + k * paso;
		float cx = RUTA_X + 24;
		
		if (k < cantidad - 1)
			dibujarCaja(ventana, cx - 1, y + 10, 2, paso, BORDE, BORDE, 0.0f);
		
		if (indices[k] == -1)
		{
			dibujarTexto(ventana, cx + 16, y + 1, "+" + to_string(n - 5) + " escalas mas", 13, GRIS);
			continue;
		}
		
		Color c = COLOR_ESCALA;
		if (indices[k] == 0) c = COLOR_ORIGEN;
		else if (indices[k] == n - 1) c = COLOR_DESTINO;
		
		CircleShape punto(5);
		punto.setOrigin(5, 5);
		punto.setPosition(cx, y + 10);
		punto.setFillColor(c);
		ventana.draw(punto);
		
		bool extremo = indices[k] == 0 || indices[k] == n - 1;
		dibujarTexto(ventana, cx + 16, y + 1, recortar(fuente, fuenteCargada, r.ruta[indices[k]], 14, extremo, RUTA_ANCHO - 60), 14, TEXTO, extremo);
	}
	
	dibujarCaja(ventana, RUTA_X + 16, BAJO_Y + BAJO_ALTO - 38, RUTA_ANCHO - 32, 1, LINEA, LINEA, 0.0f);
	string totales = "Costo " + formatear(r.costoTotal, 0) + "   |   " + formatear(r.duracionTotal, 1) + " h";
	dibujarTexto(ventana, RUTA_X + 16, BAJO_Y + BAJO_ALTO - 28, totales, 14, VERDE_OSCURO, true);
}

void PantallaExplorar::dibujarRutaSeleccionada(RenderWindow& ventana)
{
	if (grafo == nullptr || resultados == nullptr || resultadoSeleccionado < 0 || resultadoSeleccionado >= cantidadResultados)
		return;
	
	ResultadoBellmanFord& r = resultados[resultadoSeleccionado];
	if (r.cantidadRuta < 2 || r.ruta == nullptr)
		return;
	
	for (int i = 0; i < r.cantidadRuta - 1; i++)
	{
		NodoVertice* a = buscarPorNombre(grafo, r.ruta[i]);
		NodoVertice* b = buscarPorNombre(grafo, r.ruta[i + 1]);
		
		if (a == nullptr || b == nullptr)
			continue;
		
		Vector2f pa = posicionEnMapa(a->ciudad.getPosicionX(), a->ciudad.getPosicionY());
		Vector2f pb = posicionEnMapa(b->ciudad.getPosicionX(), b->ciudad.getPosicionY());
		dibujarLinea(ventana, pa, pb, 8.0f, Color(12, 31, 56, 190));
		dibujarLinea(ventana, pa, pb, 4.0f, COLOR_ESCALA);
	}
}

void PantallaExplorar::dibujarMarcadores(RenderWindow& ventana)
{
	if (grafo == nullptr)
		return;
	
	for (int pasada = 0; pasada < 2; pasada++)
	{
		NodoVertice* actual = grafo->getPrimero();
		while (actual != nullptr)
		{
			int tipo = 0;
			
			if (actual->ciudad.getCodigo() == codigoOrigen)
				tipo = 3;
			
			if (resultados != nullptr && resultadoSeleccionado >= 0 && resultadoSeleccionado < cantidadResultados)
			{
				ResultadoBellmanFord& r = resultados[resultadoSeleccionado];
				for (int i = 1; i < r.cantidadRuta - 1; i++)
					if (r.ruta[i] == actual->ciudad.getNombre())
						tipo = 1;
				if (r.codigoCiudad == actual->ciudad.getCodigo())
					tipo = 2;
			}
			
			if ((pasada == 0) == (tipo == 0))
			{
				Vector2f pos = posicionEnMapa(actual->ciudad.getPosicionX(), actual->ciudad.getPosicionY());
				
				Color c = COLOR_NEUTRO;
				float radio = 6.0f;
				if (tipo == 1) { c = COLOR_ESCALA; radio = 8.0f; }
				if (tipo == 2) { c = COLOR_DESTINO; radio = 10.0f; }
				if (tipo == 3) { c = COLOR_ORIGEN; radio = 10.0f; }
				
				CircleShape punto(radio);
				punto.setOrigin(radio, radio);
				punto.setPosition(pos);
				punto.setFillColor(c);
				punto.setOutlineThickness(2);
				punto.setOutlineColor(NAVY);
				ventana.draw(punto);
			}
			
			actual = actual->siguiente;
		}
	}
}

void PantallaExplorar::dibujarMapa(RenderWindow& ventana)
{
	dibujarCaja(ventana, MAPA_X, MAPA_Y, MAPA_ANCHO, MAPA_ALTO, NAVY, BORDE);
	
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
		dibujarTexto(ventana, MAPA_X + 15, MAPA_Y + 15, "No se pudo cargar resources/mapa.png", 14, Color(255, 170, 160), true);
	
	dibujarRutaSeleccionada(ventana);
	dibujarMarcadores(ventana);
	
	float ly = MAPA_Y + MAPA_ALTO - 40;
	dibujarCaja(ventana, MAPA_X + 16, ly, 250, 26, NAVY_SUAVE, NAVY_SUAVE, 0.0f);
	
	CircleShape punto(5);
	punto.setOrigin(5, 5);
	float cy = ly + 13;
	punto.setOutlineThickness(0);
	punto.setPosition(MAPA_X + 32, cy);  punto.setFillColor(COLOR_ORIGEN);  ventana.draw(punto);
	dibujarTexto(ventana, MAPA_X + 42, ly + 6, "Origen", 12, TEXTO_CLARO);
	punto.setPosition(MAPA_X + 110, cy); punto.setFillColor(COLOR_DESTINO); ventana.draw(punto);
	dibujarTexto(ventana, MAPA_X + 120, ly + 6, "Destino", 12, TEXTO_CLARO);
	punto.setPosition(MAPA_X + 192, cy); punto.setFillColor(COLOR_ESCALA);  ventana.draw(punto);
	dibujarTexto(ventana, MAPA_X + 202, ly + 6, "Escala", 12, TEXTO_CLARO);
}

void PantallaExplorar::dibujarDirectas(RenderWindow& ventana)
{
	dibujarCaja(ventana, DER_X, IZQ_Y, DER_ANCHO, IZQ_ALTO, BLANCO, BORDE);
	
	NodoVertice* seleccionada = nullptr;
	if (grafo != nullptr && !codigoOrigen.empty())
		seleccionada = grafo->buscarCiudad(codigoOrigen);
	
	if (seleccionada == nullptr)
	{
		dibujarTexto(ventana, DIR_X, DIR_NOMBRE_Y, "Selecciona una ciudad", 24, VERDE_OSCURO, true);
		dibujarTexto(ventana, DIR_X, DIR_INFO_Y, "Aqui apareceran sus vuelos de salida directos.", 15, GRIS);
		return;
	}
	
	dibujarTexto(ventana, DIR_X, DIR_NOMBRE_Y, seleccionada->ciudad.getNombre(), 24, VERDE_OSCURO, true);
	
	string info = "Codigo " + seleccionada->ciudad.getCodigo() +
		"     X " + formatear(seleccionada->ciudad.getPosicionX(), 1) +
		"   Y " + formatear(seleccionada->ciudad.getPosicionY(), 1);
	dibujarTexto(ventana, DIR_X, DIR_INFO_Y, info, 13, GRIS);
	
	dibujarCaja(ventana, DIR_X, DIR_ENCABEZADO_Y, DIR_ANCHO, DIR_ENCABEZADO_ALTO, ENCABEZADO_TABLA, ENCABEZADO_TABLA, 0.0f);
	dibujarTexto(ventana, DIR_X + 14, DIR_ENCABEZADO_Y + 8, "Destino", 13, GRIS, true);
	dibujarTexto(ventana, DIR_X + 400, DIR_ENCABEZADO_Y + 8, "Costo", 13, GRIS, true);
	dibujarTexto(ventana, DIR_X + 540, DIR_ENCABEZADO_Y + 8, "Duracion", 13, GRIS, true);
	dibujarTexto(ventana, DIR_X + 690, DIR_ENCABEZADO_Y + 8, "Vuelo", 13, GRIS, true);
	
	int total = contarConexionesDe(seleccionada);
	int maximo = total > DIR_FILAS ? total - DIR_FILAS : 0;
	if (desplazamientoConexiones > maximo)
		desplazamientoConexiones = maximo;
	
	if (total == 0)
	{
		dibujarTexto(ventana, DIR_X + 14, DIR_FILA_Y + 12, "Esta ciudad no tiene vuelos de salida registrados.", 15, GRIS);
		return;
	}
	
	NodoArista* arista = seleccionada->conexiones;
	int saltados = 0;
	int fila = 0;
	
	while (arista != nullptr && fila < DIR_FILAS)
	{
		if (saltados < desplazamientoConexiones)
		{
			saltados++;
			arista = arista->siguiente;
			continue;
		}
		
		float y = DIR_FILA_Y + fila * DIR_FILA_PASO;
		dibujarCaja(ventana, DIR_X, y + DIR_FILA_ALTO, DIR_ANCHO, 1, LINEA, LINEA, 0.0f);
		
		NodoVertice* destino = grafo->buscarCiudad(arista->vuelo.getCodigoDestino());
		string nombreDestino = destino != nullptr ? destino->ciudad.getNombre() : arista->vuelo.getCodigoDestino();
		
		dibujarTexto(ventana, DIR_X + 14, y + 11, recortar(fuente, fuenteCargada, nombreDestino, 15, false, 360), 15, TEXTO);
		dibujarTexto(ventana, DIR_X + 400, y + 11, formatear(arista->vuelo.getCosto(), 0), 15, TEXTO);
		dibujarTexto(ventana, DIR_X + 540, y + 11, formatear(arista->vuelo.getDuracion(), 1) + " h", 15, TEXTO);
		dibujarTexto(ventana, DIR_X + 690, y + 11, arista->vuelo.getCodigo(), 15, TEXTO);
		
		arista = arista->siguiente;
		fila++;
	}
	
	if (total > DIR_FILAS)
		dibujarTexto(ventana, DIR_X, IZQ_Y + IZQ_ALTO - 26, "Scroll para ver mas conexiones.", 11, GRIS);
}

void PantallaExplorar::dibujar(RenderWindow& ventana)
{
	ventana.setView(ventana.getDefaultView());
	
	dibujarCaja(ventana, 300, 0, 1200, 800, FONDO, FONDO, 0.0f);
	dibujarEncabezado(ventana);
	dibujarListaCiudades(ventana);
	
	if (modoDirectas)
		dibujarDirectas(ventana);
	else
		dibujarAlcanzables(ventana);
}
