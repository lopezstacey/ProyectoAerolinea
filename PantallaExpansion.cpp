#include "PantallaExpansion.h"
#include <sstream>
#include <iomanip>
#include <cmath>
#include <algorithm>

namespace
{
	const Color BLANCO(255, 255, 255);
	const Color BORDE(226, 233, 228);
	const Color VERDE_OSCURO(10, 76, 55);
	const Color VERDE(42, 112, 72);
	const Color VERDE_CLARO(228, 243, 229);
	const Color ROJO_CLARO(250, 228, 228);
	const Color TEXTO(35, 65, 52);
	const Color GRIS(115, 125, 118);
	const Color HOVER(243, 248, 244);
	const Color NAVY(12, 31, 56);
	const Color NAVY_SUAVE(12, 31, 56, 205);
	const Color TEXTO_CLARO(228, 236, 244);
	const Color COLOR_ORIGEN(46, 204, 113);
	const Color COLOR_ERROR(200, 60, 60);
	const Color COLOR_RECHAZADA(235, 87, 87);
	const Color COLOR_ESCALA(255, 196, 60);
	const Color COLOR_NEUTRO(255, 255, 255, 215);
	const Color LINEA_BASE(255, 255, 255, 45);
	
	const float PASOS_X = 320.0f;
	const float PASOS_Y = 90.0f;
	const float PASOS_ANCHO = 270.0f;
	const float PASOS_ALTO = 690.0f;
	const float FILA_Y = 174.0f;
	const float FILA_PASO = 34.0f;
	const float FILA_ALTO = 30.0f;
	const int FILAS = 17;
	
	const float MAPA_X = 610.0f;
	const float MAPA_Y = 90.0f;
	const float MAPA_ANCHO = 870.0f;
	const float MAPA_ALTO = 454.0f;
	const float REF_ANCHO = 959.0f;
	const float REF_ALTO = 500.0f;
	
	const float ARBOL_X0 = MAPA_X + 50.0f;
	const float ARBOL_ANCHO_UTIL = MAPA_ANCHO - 100.0f;
	const float ARBOL_Y0 = MAPA_Y + 80.0f;
	const float ARBOL_ALTO_UTIL = 330.0f;
	
	const float BAJO_X = 610.0f;
	const float BAJO_Y = 556.0f;
	const float BAJO_ANCHO = 870.0f;
	const float BAJO_ALTO = 224.0f;
	const float DIVISOR_X = 915.0f;
	
	const float VISTA_Y = MAPA_Y + MAPA_ALTO - 74.0f;
	const float VISTA_ANCHO = 80.0f;
	const float VISTA_ALTO = 28.0f;
	const float VISTA_MAPA_X = 626.0f;
	const float VISTA_ARBOL_X = 712.0f;
	const float REC_Y = 568.0f;
	const float REC_ALTO = 32.0f;
	const float REC_X0 = 930.0f;
	const float REC_ANCHOS[4] = { 112.0f, 122.0f, 112.0f, 100.0f };
	const float REC_XS[4] = { 930.0f, 1048.0f, 1176.0f, 1294.0f };
	const float ACC_Y = 608.0f;
	const float REPETIR_X = 930.0f;
	const float REPETIR_ANCHO = 200.0f;
	const float TODO_X = 1136.0f;
	const float TODO_ANCHO = 150.0f;
	const float ANCHO_TEXTO_RECORRIDO = 540.0f;
	const char* const NOMBRES_RECORRIDO[3] = { "PREORDEN", "POSTORDEN", "ANCHURA" };
	const float PASO_CONSTRUCCION = 0.6f;
	const float PASO_RECORRIDO = 0.5f;
	
	float escalaMapa() { return min(MAPA_ANCHO / REF_ANCHO, MAPA_ALTO / REF_ALTO); }
	float mapaOrigenX() { return MAPA_X + (MAPA_ANCHO - REF_ANCHO * escalaMapa()) / 2.0f; }
	float mapaOrigenY() { return MAPA_Y + (MAPA_ALTO - REF_ALTO * escalaMapa()) / 2.0f; }
	
	Vector2f posicionEnMapa(float px, float py)
	{
		float e = escalaMapa();
		float x = mapaOrigenX() + px * e;
		float y = mapaOrigenY() + py * e;
		x = max(MAPA_X + 14.0f, min(x, MAPA_X + MAPA_ANCHO - 14.0f));
		y = max(MAPA_Y + 14.0f, min(y, MAPA_Y + MAPA_ALTO - 14.0f));
		return Vector2f(x, y);
	}
	
	float radioArbol(float sepX, float sepY)
	{
		float r = 17.0f;
		
		if (sepX > 0)
			r = min(r, sepX * 0.4f);
		
		if (sepY > 0)
			r = min(r, sepY * 0.4f);
		
		return max(9.0f, r);
	}
	
	View vistaRecorte(RenderWindow& w)
	{
		Vector2u s = w.getSize();
		View v(FloatRect(MAPA_X, MAPA_Y, MAPA_ANCHO, MAPA_ALTO));
		v.setViewport(FloatRect(MAPA_X / s.x, MAPA_Y / s.y, MAPA_ANCHO / s.x, MAPA_ALTO / s.y));
		return v;
	}
	
	string aTexto(int valor)
	{
		ostringstream s;
		s << valor;
		return s.str();
	}
	
	string formatear(float valor, int decimales)
	{
		ostringstream s;
		s << fixed << setprecision(decimales) << valor;
		return s.str();
	}
	
	bool dentro(int x, int y, float rx, float ry, float rw, float rh)
	{
		return x >= rx && x <= rx + rw && y >= ry && y <= ry + rh;
	}
	
	float anchoTexto(const Font& f, const string& s, unsigned int tamano, bool negrita)
	{
		Text t;
		t.setFont(f);
		t.setString(s);
		t.setCharacterSize(tamano);
		if (negrita)
			t.setStyle(Text::Bold);
		return t.getLocalBounds().width;
	}
	
	void dibujarLinea(RenderWindow& w, Vector2f a, Vector2f b, float grosor, Color color)
	{
		float dx = b.x - a.x, dy = b.y - a.y;
		RectangleShape l(Vector2f(sqrt(dx * dx + dy * dy), grosor));
		l.setOrigin(0, grosor / 2);
		l.setPosition(a);
		l.setRotation(atan2(dy, dx) * 180.0f / 3.14159265f);
		l.setFillColor(color);
		w.draw(l);
	}
	
	void dibujarCaja(RenderWindow& w, float x, float y, float ancho, float alto, Color relleno, Color borde)
	{
		RectangleShape r(Vector2f(ancho, alto));
		r.setPosition(x, y);
		r.setFillColor(relleno);
		r.setOutlineThickness(1);
		r.setOutlineColor(borde);
		w.draw(r);
	}
}

PantallaExpansion::PantallaExpansion(Grafo* grafo, bool usarPrim)
{
	this->grafo = grafo;
	this->usarPrim = usarPrim;
	fuenteCargada = fuente.loadFromFile("resources/times.ttf");
	mapaCargado = texturaMapa.loadFromFile("resources/mapa.png");
	
	if (mapaCargado)
		mapa.setTexture(texturaMapa);
	
	vista = VISTA_MAPA;
	mensaje = "";
	pasoVisible = 0;
	acumulado = 0;
	reloj = 0;
	desplazamiento = 0;
	enArbol = nullptr;
	secuencia = nullptr;
	cantidadSecuencia = 0;
	visiblesRecorrido = 0;
	tipoRecorrido = -1;
	tituloRecorrido = "";
	arbolSepX = 0;
	arbolSepY = 0;
	arbolOrigenX = 0;
	
	string inicio = "";
	
	if (grafo != nullptr && grafo->getPrimero() != nullptr)
		inicio = grafo->getPrimero()->ciudad.getCodigo();
	
	recalcular(inicio);
}

PantallaExpansion::~PantallaExpansion()
{
	liberarMemoria();
}

void PantallaExpansion::liberarMemoria()
{
	delete[] enArbol;
	delete[] secuencia;
	enArbol = nullptr;
	secuencia = nullptr;
}

void PantallaExpansion::recalcular(const string& codigoInicio)
{
	liberarMemoria();
	resultado.liberar();
	arbol.limpiar();
	tipoRecorrido = -1;
	cantidadSecuencia = 0;
	visiblesRecorrido = 0;
	pasoVisible = 0;
	acumulado = 0;
	desplazamiento = 0;
	
	if (grafo == nullptr || grafo->getCantidadVertices() == 0)
	{
		mensaje = "No hay ciudades registradas. Agregalas en GESTIONAR CIUDADES.";
		return;
	}
	
	bool ok = usarPrim ? prim.calcular(*grafo, codigoInicio, resultado) : kruskal.calcular(*grafo, resultado);
	
	if (!ok)
	{
		mensaje = "No se pudo calcular el arbol de expansion.";
		return;
	}
	
	mensaje = "";
	arbol.construir(resultado, resultado.indiceInicio);
	
	int n = resultado.cantidadCiudades;
	enArbol = new bool[n];
	secuencia = new NodoExpansion*[n];
	
	for (int i = 0; i < n; i++)
		enArbol[i] = false;
	
	calcularPosicionesArbol();
}

void PantallaExpansion::reiniciarAnimacion()
{
	limpiarRecorrido();
	pasoVisible = 0;
	acumulado = 0;
	desplazamiento = 0;
	vista = VISTA_MAPA;
}

void PantallaExpansion::mostrarTodo()
{
	pasoVisible = resultado.cantidadPasos;
	acumulado = 0;
	seguirScroll();
}

void PantallaExpansion::limpiarRecorrido()
{
	tipoRecorrido = -1;
	cantidadSecuencia = 0;
	visiblesRecorrido = 0;
	tituloRecorrido = "";
}

void PantallaExpansion::iniciarRecorrido(int tipo)
{
	if (resultado.cantidadCiudades == 0 || secuencia == nullptr)
		return;
	
	mostrarTodo();
	
	if (tipo == 0)
	{
		cantidadSecuencia = arbol.recorrerPreorden(secuencia);
		tituloRecorrido = "Preorden: primero la ciudad y luego sus destinos, de izquierda a derecha";
	}
	else if (tipo == 1)
	{
		cantidadSecuencia = arbol.recorrerPostorden(secuencia);
		tituloRecorrido = "Postorden: primero los destinos y al final la ciudad";
	}
	else
	{
		cantidadSecuencia = arbol.recorrerAnchura(secuencia);
		tituloRecorrido = "Anchura: por niveles, usando una cola";
	}
	
	tipoRecorrido = tipo;
	visiblesRecorrido = 1;
	acumulado = 0;
}

void PantallaExpansion::marcarCiudades()
{
	if (enArbol == nullptr)
		return;
	
	for (int i = 0; i < resultado.cantidadCiudades; i++)
		enArbol[i] = false;
	
	if (resultado.indiceInicio >= 0)
		enArbol[resultado.indiceInicio] = true;
	
	for (int i = 0; i < pasoVisible && i < resultado.cantidadPasos; i++)
	{
		if (resultado.pasos[i].aceptada)
		{
			enArbol[resultado.pasos[i].origen] = true;
			enArbol[resultado.pasos[i].destino] = true;
		}
	}
}

int PantallaExpansion::indiceEnSecuencia(NodoExpansion* nodo) const
{
	if (tipoRecorrido < 0)
		return -1;
	
	for (int i = 0; i < visiblesRecorrido && i < cantidadSecuencia; i++)
	{
		if (secuencia[i] == nodo)
			return i;
	}
	
	return -1;
}

void PantallaExpansion::seguirScroll()
{
	desplazamiento = max(0, pasoVisible - FILAS);
}

void PantallaExpansion::calcularPosicionesArbol()
{
	int hojas = arbol.contarHojas();
	
	if (hojas <= 0)
		return;
	
	arbolSepX = hojas > 1 ? min(90.0f, ARBOL_ANCHO_UTIL / (hojas - 1)) : 0.0f;
	arbolOrigenX = ARBOL_X0 + (ARBOL_ANCHO_UTIL - (hojas - 1) * arbolSepX) / 2.0f;
	
	int altura = arbol.getAltura();
	arbolSepY = altura > 1 ? min(80.0f, ARBOL_ALTO_UTIL / (altura - 1)) : 0.0f;
	
	int hoja = 0;
	
	for (int i = 0; i < arbol.getCantidadRaices(); i++)
		asignarPosicion(arbol.getRaiz(i), 0, hoja);
	
	float total = (altura > 1 ? altura - 1 : 0) * arbolSepY;
	float y0 = ARBOL_Y0 + (ARBOL_ALTO_UTIL - total) / 2.0f;
	
	for (int i = 0; i < arbol.getCantidadNodos(); i++)
	{
		NodoExpansion* nodo = arbol.getNodo(i);
		
		if (nodo != nullptr)
			nodo->py += y0;
	}
}

float PantallaExpansion::asignarPosicion(NodoExpansion* nodo, int profundidad, int& hoja)
{
	nodo->py = profundidad * arbolSepY;
	
	if (nodo->primerHijo == nullptr)
	{
		nodo->px = arbolOrigenX + hoja * arbolSepX;
		hoja++;
		return nodo->px;
	}
	
	float primero = 0;
	float ultimo = 0;
	bool esPrimero = true;
	
	for (NodoExpansion* h = nodo->primerHijo; h != nullptr; h = h->hermano)
	{
		float x = asignarPosicion(h, profundidad + 1, hoja);
		
		if (esPrimero)
		{
			primero = x;
			esPrimero = false;
		}
		
		ultimo = x;
	}
	
	nodo->px = (primero + ultimo) / 2.0f;
	return nodo->px;
}

Vector2f PantallaExpansion::posicionNodo(NodoExpansion* nodo) const
{
	if (vista == VISTA_ARBOL)
		return Vector2f(nodo->px, nodo->py);
	
	return posicionEnMapa(nodo->x, nodo->y);
}

string PantallaExpansion::textoPeso(float costo, float duracion) const
{
	return usarPrim ? formatear(costo, 0) : formatear(duracion, 1) + " h";
}

string PantallaExpansion::recortarAncho(const string& texto, unsigned int tamano, bool negrita, float maxAncho) const
{
	if (!fuenteCargada)
		return texto;
	
	Text t;
	t.setFont(fuente);
	t.setCharacterSize(tamano);
	if (negrita)
		t.setStyle(Text::Bold);
	
	string actual = texto;
	t.setString(actual);
	
	if (t.getLocalBounds().width <= maxAncho)
		return actual;
	
	while (!actual.empty())
	{
		actual.erase(actual.size() - 1);
		
		while (!actual.empty() && (static_cast<unsigned char>(actual[actual.size() - 1]) & 0xC0) == 0x80)
			actual.erase(actual.size() - 1);
		
		t.setString(actual + "...");
		
		if (t.getLocalBounds().width <= maxAncho)
			break;
	}
	
	return actual + "...";
}

void PantallaExpansion::procesarEventos(RenderWindow& ventana, Event evento)
{
	if (evento.type == Event::MouseWheelScrolled)
	{
		Vector2i m = Mouse::getPosition(ventana);
		
		if (dentro(m.x, m.y, PASOS_X, PASOS_Y, PASOS_ANCHO, PASOS_ALTO))
		{
			int maximo = max(0, pasoVisible - FILAS);
			desplazamiento += evento.mouseWheelScroll.delta < 0 ? 1 : -1;
			desplazamiento = max(0, min(desplazamiento, maximo));
		}
		
		return;
	}
	
	if (evento.type != Event::MouseButtonPressed || evento.mouseButton.button != Mouse::Left)
		return;
	
	int mx = evento.mouseButton.x;
	int my = evento.mouseButton.y;
	
	if (dentro(mx, my, VISTA_MAPA_X, VISTA_Y, VISTA_ANCHO, VISTA_ALTO))
	{
		vista = VISTA_MAPA;
		return;
	}
	
	if (dentro(mx, my, VISTA_ARBOL_X, VISTA_Y, VISTA_ANCHO, VISTA_ALTO))
	{
		if (resultado.cantidadCiudades > 0)
			mostrarTodo();
		vista = VISTA_ARBOL;
		return;
	}
	
	for (int i = 0; i < 3; i++)
	{
		if (dentro(mx, my, REC_XS[i], REC_Y, REC_ANCHOS[i], REC_ALTO))
		{
			iniciarRecorrido(i);
			return;
		}
	}
	
	if (dentro(mx, my, REC_XS[3], REC_Y, REC_ANCHOS[3], REC_ALTO))
	{
		limpiarRecorrido();
		return;
	}
	
	if (dentro(mx, my, REPETIR_X, ACC_Y, REPETIR_ANCHO, REC_ALTO))
	{
		if (resultado.cantidadCiudades > 0)
			reiniciarAnimacion();
		return;
	}
	
	if (dentro(mx, my, TODO_X, ACC_Y, TODO_ANCHO, REC_ALTO))
	{
		if (resultado.cantidadCiudades > 0)
			mostrarTodo();
		return;
	}
	
	if (usarPrim && vista == VISTA_MAPA && resultado.cantidadCiudades > 0 &&
		dentro(mx, my, MAPA_X, MAPA_Y, MAPA_ANCHO, MAPA_ALTO))
	{
		for (int i = 0; i < resultado.cantidadCiudades; i++)
		{
			Vector2f p = posicionEnMapa(resultado.ciudades[i].x, resultado.ciudades[i].y);
			float dx = mx - p.x;
			float dy = my - p.y;
			
			if (dx * dx + dy * dy <= 14.0f * 14.0f)
			{
				recalcular(resultado.ciudades[i].codigo);
				return;
			}
		}
	}
}

void PantallaExpansion::actualizar(float tiempo)
{
	reloj += tiempo;
	
	if (resultado.cantidadCiudades == 0)
		return;
	
	if (pasoVisible < resultado.cantidadPasos)
	{
		acumulado += tiempo;
		
		if (acumulado >= PASO_CONSTRUCCION)
		{
			acumulado = 0;
			pasoVisible++;
			seguirScroll();
		}
	}
	else if (tipoRecorrido >= 0 && visiblesRecorrido < cantidadSecuencia)
	{
		acumulado += tiempo;
		
		if (acumulado >= PASO_RECORRIDO)
		{
			acumulado = 0;
			visiblesRecorrido++;
		}
	}
}

void PantallaExpansion::dibujarTexto(RenderWindow& ventana, float x, float y, const string& texto, unsigned int tamano, Color color, bool negrita)
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

void PantallaExpansion::dibujarTextoCentrado(RenderWindow& ventana, float cx, float cy, const string& texto, unsigned int tamano, Color color, bool negrita)
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
	
	FloatRect lim = t.getLocalBounds();
	t.setPosition(cx - lim.width / 2 - lim.left, cy - lim.height / 2 - lim.top);
	ventana.draw(t);
}

void PantallaExpansion::dibujarTextoContorno(RenderWindow& ventana, float cx, float cy, const string& texto, unsigned int tamano, Color color)
{
	if (!fuenteCargada)
		return;
	
	Text t;
	t.setFont(fuente);
	t.setString(texto);
	t.setCharacterSize(tamano);
	t.setFillColor(color);
	t.setOutlineColor(NAVY);
	t.setOutlineThickness(1.5f);
	
	FloatRect lim = t.getLocalBounds();
	t.setPosition(cx - lim.width / 2 - lim.left, cy - lim.height / 2 - lim.top);
	ventana.draw(t);
}

void PantallaExpansion::dibujarBoton(RenderWindow& ventana, float x, float y, float ancho, float alto, const string& texto, bool activo)
{
	Vector2i m = Mouse::getPosition(ventana);
	bool encima = dentro(m.x, m.y, x, y, ancho, alto);
	
	dibujarCaja(ventana, x, y, ancho, alto, activo ? VERDE : (encima ? HOVER : BLANCO), activo ? VERDE_OSCURO : BORDE);
	dibujarTextoCentrado(ventana, x + ancho / 2, y + alto / 2, texto, 14, activo ? BLANCO : TEXTO, true);
}

void PantallaExpansion::dibujarEncabezado(RenderWindow& ventana)
{
	string titulo = usarPrim ? "ARBOL DE EXPANSION MINIMA - PRIM (COSTO)" : "ARBOL DE EXPANSION MINIMA - KRUSKAL (DURACION)";
	dibujarTexto(ventana, 320, 14, titulo, 26, TEXTO_CLARO, true);
	
	string subtitulo = usarPrim
		? "Parte de una ciudad y siempre agrega la conexion mas barata hacia una ciudad nueva. Clic en una ciudad del mapa para cambiar el inicio."
		: "Ordena todas las conexiones por duracion y acepta cada una solo si no forma un ciclo (Union-Find).";
	dibujarTexto(ventana, 322, 54, recortarAncho(subtitulo, 14, false, 1150), 14, Color(160, 180, 200));
}

void PantallaExpansion::dibujarPasos(RenderWindow& ventana)
{
	dibujarCaja(ventana, PASOS_X, PASOS_Y, PASOS_ANCHO, PASOS_ALTO, BLANCO, BORDE);
	dibujarTexto(ventana, PASOS_X + 16, PASOS_Y + 12, "Pasos del algoritmo", 17, TEXTO, true);
	
	string progreso = aTexto(pasoVisible) + " de " + aTexto(resultado.cantidadPasos) + (usarPrim ? " aristas" : " aristas evaluadas");
	dibujarTexto(ventana, PASOS_X + 16, PASOS_Y + 38, progreso, 13, GRIS);
	
	dibujarTexto(ventana, PASOS_X + 12, PASOS_Y + 62, "#   Conexion", 12, GRIS, true);
	dibujarTexto(ventana, PASOS_X + 196, PASOS_Y + 62, usarPrim ? "Costo" : "Duracion", 12, GRIS, true);
	dibujarCaja(ventana, PASOS_X + 8, FILA_Y - 8, PASOS_ANCHO - 28, 1, BORDE, BORDE);
	
	for (int fila = 0; fila < FILAS; fila++)
	{
		int i = desplazamiento + fila;
		
		if (i >= pasoVisible || i >= resultado.cantidadPasos)
			break;
		
		const AristaExpansion& a = resultado.pasos[i];
		float y = FILA_Y + fila * FILA_PASO;
		bool actual = (i == pasoVisible - 1);
		
		if (actual)
			dibujarCaja(ventana, PASOS_X + 6, y, PASOS_ANCHO - 26, FILA_ALTO, a.aceptada ? VERDE_CLARO : ROJO_CLARO, a.aceptada ? VERDE_CLARO : ROJO_CLARO);
		
		Color color = a.aceptada ? TEXTO : COLOR_ERROR;
		string conexion = resultado.ciudades[a.origen].nombre + " - " + resultado.ciudades[a.destino].nombre;
		string peso = textoPeso(a.costo, a.duracion);
		
		float yTexto = a.aceptada ? y + 6 : y + 2;
		
		dibujarTexto(ventana, PASOS_X + 12, yTexto + 1, aTexto(i + 1), 12, GRIS);
		dibujarTexto(ventana, PASOS_X + 40, yTexto, recortarAncho(conexion, 13, false, 148), 13, color);
		dibujarTexto(ventana, PASOS_X + 196, yTexto, peso, 13, color);
		
		if (!a.aceptada)
			dibujarTexto(ventana, PASOS_X + 40, y + 17, "forma ciclo: se descarta", 10, COLOR_ERROR);
	}
	
	if (pasoVisible == 0 && resultado.cantidadCiudades > 0)
		dibujarTexto(ventana, PASOS_X + 16, FILA_Y + 4, "Iniciando...", 14, GRIS);
	
	if (pasoVisible > FILAS)
	{
		float pista = FILAS * FILA_PASO - 4;
		float asa = max(24.0f, pista * FILAS / pasoVisible);
		int maximo = pasoVisible - FILAS;
		float avance = maximo > 0 ? static_cast<float>(desplazamiento) / maximo : 0.0f;
		float x = PASOS_X + PASOS_ANCHO - 10;
		dibujarCaja(ventana, x, FILA_Y, 4, pista, HOVER, HOVER);
		dibujarCaja(ventana, x, FILA_Y + (pista - asa) * avance, 4, asa, GRIS, GRIS);
	}
}

void PantallaExpansion::dibujarSelectorVista(RenderWindow& ventana)
{
	dibujarCaja(ventana, VISTA_MAPA_X - 4, VISTA_Y - 4, VISTA_ANCHO * 2 + 14, VISTA_ALTO + 8, NAVY_SUAVE, NAVY_SUAVE);
	dibujarBoton(ventana, VISTA_MAPA_X, VISTA_Y, VISTA_ANCHO, VISTA_ALTO, "MAPA", vista == VISTA_MAPA);
	dibujarBoton(ventana, VISTA_ARBOL_X, VISTA_Y, VISTA_ANCHO, VISTA_ALTO, "ARBOL", vista == VISTA_ARBOL);
}

void PantallaExpansion::dibujarNodo(RenderWindow& ventana, NodoExpansion* nodo)
{
	Vector2f pos = posicionNodo(nodo);
	bool enVistaArbol = (vista == VISTA_ARBOL);
	float base = enVistaArbol ? radioArbol(arbolSepX, arbolSepY) : 7.0f;
	float radio = base;
	Color relleno = COLOR_NEUTRO;
	Color borde = NAVY;
	
	if (enArbol != nullptr && enArbol[nodo->ciudad])
	{
		relleno = COLOR_ORIGEN;
		radio = enVistaArbol ? base : 9.0f;
	}
	
	bool esInicio = usarPrim && nodo->ciudad == resultado.indiceInicio;
	
	if (esInicio)
	{
		radio += 3.0f;
		borde = BLANCO;
	}
	
	int indice = indiceEnSecuencia(nodo);
	
	if (indice >= 0)
	{
		bool actual = (indice == visiblesRecorrido - 1);
		relleno = actual ? BLANCO : COLOR_ESCALA;
		borde = COLOR_ESCALA;
		
		if (actual && !enVistaArbol)
			radio = 11.0f;
	}
	
	CircleShape circulo(radio);
	circulo.setOrigin(radio, radio);
	circulo.setPosition(pos);
	circulo.setFillColor(relleno);
	circulo.setOutlineThickness(esInicio ? 3.0f : 2.0f);
	circulo.setOutlineColor(borde);
	ventana.draw(circulo);
	
	if (enVistaArbol)
	{
		dibujarTextoCentrado(ventana, pos.x, pos.y, nodo->codigo, base >= 14.0f ? 12 : 10, NAVY, true);
		
		if (arbolSepY <= 0 || arbolSepY >= 50.0f)
		{
			float maximo = arbolSepX > 0 ? max(56.0f, arbolSepX - 8.0f) : 160.0f;
			dibujarTextoContorno(ventana, pos.x, pos.y + radio + 11, recortarAncho(nodo->nombre, 12, false, maximo), 12, TEXTO_CLARO);
		}
		else if (fuenteCargada)
		{
			string nombre = recortarAncho(nodo->nombre, 12, false, 110.0f);
			float w = anchoTexto(fuente, nombre, 12, false);
			dibujarTextoContorno(ventana, pos.x + radio + 6 + w / 2, pos.y, nombre, 12, TEXTO_CLARO);
		}
	}
	else if (fuenteCargada)
	{
		string nombre = nodo->nombre;
		float w = anchoTexto(fuente, nombre, 12, false);
		float x = pos.x + radio + 6 + w / 2;
		
		if (pos.x + radio + 6 + w > MAPA_X + MAPA_ANCHO - 4)
			x = pos.x - radio - 6 - w / 2;
		
		dibujarTextoContorno(ventana, x, pos.y - 1, nombre, 12, TEXTO_CLARO);
	}
	
	if (indice >= 0)
	{
		float ix = pos.x - radio * 0.8f - 2;
		float iy = pos.y - radio - 6;
		CircleShape insignia(9);
		insignia.setOrigin(9, 9);
		insignia.setPosition(ix, iy);
		insignia.setFillColor(NAVY);
		insignia.setOutlineThickness(1);
		insignia.setOutlineColor(COLOR_ESCALA);
		ventana.draw(insignia);
		dibujarTextoCentrado(ventana, ix, iy, aTexto(indice + 1), 11, BLANCO, true);
	}
}

void PantallaExpansion::dibujarVistaMapa(RenderWindow& ventana)
{
	dibujarCaja(ventana, MAPA_X, MAPA_Y, MAPA_ANCHO, MAPA_ALTO, NAVY, BORDE);
	
	ventana.setView(vistaRecorte(ventana));
	
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
		dibujarTexto(ventana, MAPA_X + 15, MAPA_Y + 50, "No se pudo cargar resources/mapa.png", 14, Color(255, 170, 160), true);
	
	if (resultado.cantidadCiudades > 0)
	{
		NodoVertice* vertice = grafo->getPrimero();
		
		while (vertice != nullptr)
		{
			Vector2f a = posicionEnMapa(vertice->ciudad.getPosicionX(), vertice->ciudad.getPosicionY());
			NodoArista* arista = vertice->conexiones;
			
			while (arista != nullptr)
			{
				NodoVertice* destino = grafo->buscarCiudad(arista->vuelo.getCodigoDestino());
				
				if (destino != nullptr)
					dibujarLinea(ventana, a, posicionEnMapa(destino->ciudad.getPosicionX(), destino->ciudad.getPosicionY()), 1, LINEA_BASE);
				
				arista = arista->siguiente;
			}
			
			vertice = vertice->siguiente;
		}
		
		bool enCurso = pasoVisible < resultado.cantidadPasos;
		
		for (int i = 0; i < pasoVisible && i < resultado.cantidadPasos; i++)
		{
			const AristaExpansion& a = resultado.pasos[i];
			bool actual = (i == pasoVisible - 1);
			Vector2f p = posicionEnMapa(resultado.ciudades[a.origen].x, resultado.ciudades[a.origen].y);
			Vector2f q = posicionEnMapa(resultado.ciudades[a.destino].x, resultado.ciudades[a.destino].y);
			
			if (a.aceptada)
			{
				dibujarLinea(ventana, p, q, 3, (actual && enCurso) ? COLOR_ESCALA : COLOR_ORIGEN);
				dibujarTextoContorno(ventana, (p.x + q.x) / 2, (p.y + q.y) / 2 - 9, textoPeso(a.costo, a.duracion), 12, TEXTO_CLARO);
			}
			else if (actual)
				dibujarLinea(ventana, p, q, 2, COLOR_RECHAZADA);
		}
		
		marcarCiudades();
		
		for (int i = 0; i < resultado.cantidadCiudades; i++)
			dibujarNodo(ventana, arbol.getNodo(i));
	}
	
	ventana.setView(ventana.getDefaultView());
	
	if (resultado.cantidadCiudades == 0)
	{
		dibujarTextoCentrado(ventana, MAPA_X + MAPA_ANCHO / 2, MAPA_Y + MAPA_ALTO / 2, mensaje, 18, TEXTO_CLARO, true);
		return;
	}
	
	if (!fuenteCargada)
		return;
	
	struct ItemLeyenda
	{
		Color color;
		const char* texto;
		bool bordeBlanco;
	};
	
	ItemLeyenda items[3] = {
		{ COLOR_ORIGEN, "En el arbol", false },
	{ COLOR_ESCALA, "Arista nueva", false },
			usarPrim ? ItemLeyenda{ COLOR_ORIGEN, "Inicio (borde blanco)", true }
		: ItemLeyenda{ COLOR_RECHAZADA, "Descartada (ciclo)", false }
	};
	
	float ancho = 12.0f;
	
	for (int i = 0; i < 3; i++)
		ancho += 16.0f + anchoTexto(fuente, items[i].texto, 12, false) + 14.0f;
	
	float ly = MAPA_Y + MAPA_ALTO - 36;
	dibujarCaja(ventana, MAPA_X + 16, ly, ancho, 26, NAVY_SUAVE, NAVY_SUAVE);
	
	float lx = MAPA_X + 28;
	
	for (int i = 0; i < 3; i++)
	{
		CircleShape punto(5);
		punto.setOrigin(5, 5);
		punto.setPosition(lx + 5, ly + 13);
		punto.setFillColor(items[i].color);
		
		if (items[i].bordeBlanco)
		{
			punto.setOutlineThickness(2);
			punto.setOutlineColor(BLANCO);
		}
		
		ventana.draw(punto);
		dibujarTexto(ventana, lx + 16, ly + 6, items[i].texto, 12, TEXTO_CLARO);
		lx += 16.0f + anchoTexto(fuente, items[i].texto, 12, false) + 14.0f;
	}
}

void PantallaExpansion::dibujarAristasArbol(RenderWindow& ventana, NodoExpansion* nodo)
{
	float t = (arbolSepY >= 50.0f) ? 0.58f : 0.5f;
	
	for (NodoExpansion* h = nodo->primerHijo; h != nullptr; h = h->hermano)
	{
		Vector2f p = posicionNodo(nodo);
		Vector2f q = posicionNodo(h);
		dibujarLinea(ventana, p, q, 2, COLOR_ORIGEN);
		dibujarTextoContorno(ventana, p.x + (q.x - p.x) * t + 16, p.y + (q.y - p.y) * t, textoPeso(h->costoPadre, h->duracionPadre), 12, TEXTO_CLARO);
		dibujarAristasArbol(ventana, h);
	}
}

void PantallaExpansion::dibujarVistaArbol(RenderWindow& ventana)
{
	dibujarCaja(ventana, MAPA_X, MAPA_Y, MAPA_ANCHO, MAPA_ALTO, NAVY, BORDE);
	
	if (resultado.cantidadCiudades == 0)
	{
		dibujarTextoCentrado(ventana, MAPA_X + MAPA_ANCHO / 2, MAPA_Y + MAPA_ALTO / 2, mensaje, 18, TEXTO_CLARO, true);
		return;
	}
	
	string info = "Arbol generado: " + aTexto(arbol.getCantidadRaices()) + (arbol.getCantidadRaices() == 1 ? " arbol" : " arboles (grafo no conexo)") +
		"   Altura: " + aTexto(arbol.getAltura());
	float anchoInfo = fuenteCargada ? anchoTexto(fuente, info, 13, false) : 0.0f;
	dibujarTexto(ventana, MAPA_X + MAPA_ANCHO - 16 - anchoInfo, MAPA_Y + 12, info, 13, TEXTO_CLARO);
	
	ventana.setView(vistaRecorte(ventana));
	
	for (int i = 0; i < arbol.getCantidadRaices(); i++)
		dibujarAristasArbol(ventana, arbol.getRaiz(i));
	
	marcarCiudades();
	
	for (int i = 0; i < resultado.cantidadCiudades; i++)
		dibujarNodo(ventana, arbol.getNodo(i));
	
	ventana.setView(ventana.getDefaultView());
}

void PantallaExpansion::dibujarResumen(RenderWindow& ventana)
{
	dibujarCaja(ventana, BAJO_X, BAJO_Y, BAJO_ANCHO, BAJO_ALTO, BLANCO, BORDE);
	dibujarCaja(ventana, DIVISOR_X, BAJO_Y + 12, 1, BAJO_ALTO - 24, BORDE, BORDE);
	
	float x = BAJO_X + 16;
	float y = BAJO_Y + 12;
	dibujarTexto(ventana, x, y, "Resumen", 17, TEXTO, true);
	y += 32;
	
	if (resultado.cantidadCiudades == 0)
	{
		dibujarTexto(ventana, x, y, recortarAncho(mensaje, 14, false, DIVISOR_X - x - 12), 14, GRIS);
		return;
	}
	
	float costo = 0;
	float duracion = 0;
	int aceptadas = 0;
	
	for (int i = 0; i < pasoVisible && i < resultado.cantidadPasos; i++)
	{
		if (resultado.pasos[i].aceptada)
		{
			costo += resultado.pasos[i].costo;
			duracion += resultado.pasos[i].duracion;
			aceptadas++;
		}
	}
	
	bool terminado = pasoVisible >= resultado.cantidadPasos;
	
	if (usarPrim)
	{
		dibujarTexto(ventana, x, y, "Ciudad inicial: " + recortarAncho(resultado.ciudades[resultado.indiceInicio].nombre, 14, true, 170), 14, TEXTO, true);
		y += 24;
	}
	
	dibujarTexto(ventana, x, y, "Aristas en el arbol: " + aTexto(aceptadas), 14, TEXTO);
	y += 24;
	dibujarTexto(ventana, x, y, string(terminado ? "Costo total: " : "Costo acumulado: ") + formatear(costo, 0), 14, usarPrim ? VERDE_OSCURO : TEXTO, usarPrim);
	y += 24;
	dibujarTexto(ventana, x, y, string(terminado ? "Duracion total: " : "Duracion acumulada: ") + formatear(duracion, 1) + " h", 14, usarPrim ? TEXTO : VERDE_OSCURO, !usarPrim);
	y += 24;
	
	if (terminado)
	{
		dibujarTexto(ventana, x, y, "Componentes: " + aTexto(resultado.componentes), 14, TEXTO);
		y += 20;
		
		if (resultado.componentes > 1)
			dibujarTexto(ventana, x, y, "El grafo no es conexo: se forma un bosque.", 12, COLOR_ERROR);
	}
	else
		dibujarTexto(ventana, x, y, "Construyendo el arbol...", 13, GRIS);
	
	float yCriterio = BAJO_Y + BAJO_ALTO - 42;
	
	if (usarPrim)
	{
		dibujarTexto(ventana, x, yCriterio, "Criterio: costo.", 12, GRIS);
		dibujarTexto(ventana, x, yCriterio + 16, "Matriz de adyacencia, O(V^2).", 12, GRIS);
	}
	else
	{
		dibujarTexto(ventana, x, yCriterio, "Criterio: duracion.", 12, GRIS);
		dibujarTexto(ventana, x, yCriterio + 16, "Merge sort + Union-Find, O(E log E).", 12, GRIS);
	}
}

void PantallaExpansion::dibujarRecorridos(RenderWindow& ventana)
{
	for (int i = 0; i < 3; i++)
		dibujarBoton(ventana, REC_XS[i], REC_Y, REC_ANCHOS[i], REC_ALTO, NOMBRES_RECORRIDO[i], tipoRecorrido == i);
	
	dibujarBoton(ventana, REC_XS[3], REC_Y, REC_ANCHOS[3], REC_ALTO, "LIMPIAR", false);
	dibujarBoton(ventana, REPETIR_X, ACC_Y, REPETIR_ANCHO, REC_ALTO, "REPETIR ANIMACION", false);
	dibujarBoton(ventana, TODO_X, ACC_Y, TODO_ANCHO, REC_ALTO, "MOSTRAR TODO", false);
	
	if (tipoRecorrido < 0)
	{
		dibujarTexto(ventana, REC_X0, ACC_Y + 48, recortarAncho("Elige un recorrido para verlo animado sobre el arbol generado.", 13, false, ANCHO_TEXTO_RECORRIDO), 13, GRIS);
		return;
	}
	
	dibujarTexto(ventana, REC_X0, ACC_Y + 46, recortarAncho(tituloRecorrido, 13, true, ANCHO_TEXTO_RECORRIDO), 13, TEXTO, true);
	
	string linea = "";
	float y = ACC_Y + 68;
	int lineas = 0;
	
	for (int i = 0; i < visiblesRecorrido && i < cantidadSecuencia; i++)
	{
		string pieza = secuencia[i]->nombre + (i + 1 < cantidadSecuencia ? ", " : "");
		
		if (fuenteCargada && !linea.empty() && anchoTexto(fuente, linea + pieza, 14, false) > ANCHO_TEXTO_RECORRIDO)
		{
			dibujarTexto(ventana, REC_X0, y, linea, 14, TEXTO);
			y += 21;
			lineas++;
			linea = "";
			
			if (lineas >= 4)
			{
				dibujarTexto(ventana, REC_X0, y, "...", 14, GRIS);
				return;
			}
		}
		
		linea += pieza;
	}
	
	if (!linea.empty() && lineas < 4)
		dibujarTexto(ventana, REC_X0, y, linea, 14, TEXTO);
}

void PantallaExpansion::dibujar(RenderWindow& ventana)
{
	dibujarEncabezado(ventana);
	dibujarPasos(ventana);
	
	if (vista == VISTA_MAPA)
		dibujarVistaMapa(ventana);
	else
		dibujarVistaArbol(ventana);
	
	dibujarSelectorVista(ventana);
	dibujarResumen(ventana);
	dibujarRecorridos(ventana);
}
