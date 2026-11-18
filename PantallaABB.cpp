#include "PantallaABB.h"
#include <sstream>
#include <cmath>
#include <algorithm>

namespace
{
	const Color BLANCO(255, 255, 255);
	const Color BORDE(226, 233, 228);
	const Color VERDE_OSCURO(10, 76, 55);
	const Color VERDE(42, 112, 72);
	const Color VERDE_CLARO(228, 243, 229);
	const Color TEXTO(35, 65, 52);
	const Color GRIS(115, 125, 118);
	const Color HOVER(243, 248, 244);
	const Color NAVY(12, 31, 56);
	const Color TEXTO_CLARO(228, 236, 244);
	const Color COLOR_ORIGEN(46, 204, 113);
	const Color COLOR_ERROR(200, 60, 60);
	const Color COLOR_ESCALA(255, 196, 60);
	const Color COLOR_ESCALA_SUAVE(255, 236, 179);
	const Color COLOR_ESCALA_BORDE(200, 150, 20);
	const float TOP_X = 320.0f;
	const float TOP_Y = 64.0f;
	const float TOP_ANCHO = 1160.0f;
	const float TOP_ALTO = 96.0f;
	const float INPUT_X = 336.0f;
	const float INPUT_Y = 76.0f;
	const float INPUT_ANCHO = 420.0f;
	const float INPUT_ALTO = 36.0f;
	const float BUSCAR_X = 770.0f;
	const float BUSCAR_ANCHO = 110.0f;
	const float ARB_X = 320.0f;
	const float ARB_Y = 172.0f;
	const float ARB_ANCHO = 1160.0f;
	const float ARB_ALTO = 440.0f;
	const float RADIO = 18.0f;
	const float BAJO_X = 320.0f;
	const float BAJO_Y = 624.0f;
	const float BAJO_ANCHO = 1160.0f;
	const float BAJO_ALTO = 160.0f;
	const float BOTON_Y = 636.0f;
	const float BOTON_ALTO = 34.0f;
	const float BOTON_ANCHO = 160.0f;
	const float BOTON_PASO = 170.0f;
	const float BOTON_X0 = 336.0f;
	
	const float PASO_ANIMACION = 0.6f;
	const char* const NOMBRES_RECORRIDO[3] = { "INORDEN", "PREORDEN", "POSTORDEN" };
	
	string aTexto(int valor)
	{
		ostringstream salida;
		salida << valor;
		return salida.str();
	}
	
	string limpiar(const string& texto)
	{
		size_t inicio = texto.find_first_not_of(" \t\r\n");
		
		if (inicio == string::npos)
			return "";
		
		size_t fin = texto.find_last_not_of(" \t\r\n");
		return texto.substr(inicio, fin - inicio + 1);
	}
	
	bool dentro(int x, int y, float rx, float ry, float rw, float rh)
	{
		return x >= rx && x <= rx + rw && y >= ry && y <= ry + rh;
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
	
	void dibujarTarjeta(RenderWindow& w, float x, float y, float ancho, float alto)
	{
		RectangleShape t(Vector2f(ancho, alto));
		t.setPosition(x, y);
		t.setFillColor(BLANCO);
		t.setOutlineThickness(1);
		t.setOutlineColor(BORDE);
		w.draw(t);
	}
	
	string recortarNombre(const string& texto, size_t maximo)
	{
		if (texto.size() <= maximo)
			return texto;
		
		size_t corte = maximo - 1;
		while (corte > 0 && (static_cast<unsigned char>(texto[corte]) & 0xC0) == 0x80)
			corte--;
		
		return texto.substr(0, corte) + ".";
	}
}

PantallaABB::PantallaABB(Grafo* grafo)
{
	this->grafo = grafo;
	fuenteCargada = fuente.loadFromFile("resources/times.ttf");
	textoBusqueda = "";
	mensaje = "Escribe el nombre de una ciudad y presiona BUSCAR, o elige un recorrido.";
	estadoMensaje = 0;
	cantidadSecuencia = 0;
	visibles = 0;
	acumulado = 0;
	reloj = 0;
	modo = MODO_NINGUNO;
	tipoRecorrido = -1;
	finalizado = false;
	encontrado = nullptr;
	tituloSecuencia = "";
	separacionX = 0;
	separacionY = 0;
	offsetX = 0;
	arbol = grafo != nullptr ? grafo->getArbolCiudades() : &arbolVacio;
	secuencia = new NodoABB*[arbol->getCantidad() > 0 ? arbol->getCantidad() : 1];
	calcularPosiciones();
}

PantallaABB::~PantallaABB()
{
	delete[] secuencia;
}

void PantallaABB::calcularPosiciones()
{
	int n = arbol->getCantidad();
	
	if (n == 0)
		return;
	
	int altura = arbol->getAltura();
	separacionX = n > 1 ? min(90.0f, 1080.0f / (n - 1)) : 0.0f;
	offsetX = ARB_X + (ARB_ANCHO - (n - 1) * separacionX) / 2.0f;
	separacionY = altura > 1 ? min(85.0f, 335.0f / (altura - 1)) : 0.0f;
	int contador = 0;
	asignarPosiciones(arbol->getRaiz(), 0, contador);
}

void PantallaABB::asignarPosiciones(NodoABB* nodo, int profundidad, int& contador)
{
	if (nodo == nullptr)
		return;
	
	asignarPosiciones(nodo->izquierdo, profundidad + 1, contador);
	nodo->x = offsetX + contador * separacionX;
	nodo->y = ARB_Y + 45.0f + profundidad * separacionY;
	contador++;
	asignarPosiciones(nodo->derecho, profundidad + 1, contador);
}

void PantallaABB::iniciarBusqueda()
{
	string clave = limpiar(textoBusqueda);
	
	if (clave.empty())
	{
		mensaje = "Escribe el nombre de una ciudad para buscarla.";
		estadoMensaje = 2;
		return;
	}
	
	if (arbol->getCantidad() == 0)
	{
		mensaje = "El arbol esta vacio.";
		estadoMensaje = 2;
		return;
	}
	
	int pasos = 0;
	encontrado = arbol->buscar(clave, secuencia, pasos);
	cantidadSecuencia = pasos;
	visibles = 1;
	acumulado = 0;
	modo = MODO_BUSQUEDA;
	tipoRecorrido = -1;
	finalizado = false;
	tituloSecuencia = "Camino de busqueda (cada nodo visitado es una comparacion)";
	mensaje = "Buscando \"" + clave + "\"...";
	estadoMensaje = 0;
}

void PantallaABB::iniciarRecorrido(int tipo)
{
	if (arbol->getCantidad() == 0)
	{
		mensaje = "El arbol esta vacio.";
		estadoMensaje = 2;
		return;
	}
	
	if (tipo == 0)
	{
		cantidadSecuencia = arbol->recorrerInorden(secuencia);
		tituloSecuencia = "Inorden (izquierdo, raiz, derecho): devuelve las ciudades en orden alfabetico";
	}
	else if (tipo == 1)
	{
		cantidadSecuencia = arbol->recorrerPreorden(secuencia);
		tituloSecuencia = "Preorden (raiz, izquierdo, derecho)";
	}
	else
	{
		cantidadSecuencia = arbol->recorrerPostorden(secuencia);
		tituloSecuencia = "Postorden (izquierdo, derecho, raiz)";
	}
	
	visibles = 1;
	acumulado = 0;
	modo = MODO_RECORRIDO;
	tipoRecorrido = tipo;
	finalizado = false;
	encontrado = nullptr;
	mensaje = string("Recorrido en ") + NOMBRES_RECORRIDO[tipo] + ": " + aTexto(cantidadSecuencia) + " ciudades.";
	estadoMensaje = 0;
}

int PantallaABB::indiceVisible(NodoABB* nodo) const
{
	for (int i = 0; i < visibles && i < cantidadSecuencia; i++)
	{
		if (secuencia[i] == nodo)
			return i;
	}
	
	return -1;
}

void PantallaABB::procesarEventos(RenderWindow& ventana, Event evento)
{
	if (evento.type == Event::TextEntered)
	{
		if (evento.text.unicode == 8)
		{
			if (!textoBusqueda.empty())
				textoBusqueda.erase(textoBusqueda.size() - 1);
		}
		else if (evento.text.unicode == 13)
			iniciarBusqueda();
		else if (evento.text.unicode >= 32 && evento.text.unicode < 127)
		{
			if (textoBusqueda.size() < 35)
				textoBusqueda += static_cast<char>(evento.text.unicode);
		}
		return;
	}
	
	if (evento.type != Event::MouseButtonPressed || evento.mouseButton.button != Mouse::Left)
		return;
	
	int mx = evento.mouseButton.x;
	int my = evento.mouseButton.y;
	
	if (dentro(mx, my, BUSCAR_X, INPUT_Y, BUSCAR_ANCHO, INPUT_ALTO))
	{
		iniciarBusqueda();
		return;
	}
	
	for (int i = 0; i < 3; i++)
	{
		if (dentro(mx, my, BOTON_X0 + i * BOTON_PASO, BOTON_Y, BOTON_ANCHO, BOTON_ALTO))
		{
			iniciarRecorrido(i);
			return;
		}
	}
	
	if (dentro(mx, my, BOTON_X0 + 3 * BOTON_PASO, BOTON_Y, BOTON_ANCHO, BOTON_ALTO))
	{
		modo = MODO_NINGUNO;
		tipoRecorrido = -1;
		visibles = 0;
		cantidadSecuencia = 0;
		encontrado = nullptr;
		mensaje = "Escribe el nombre de una ciudad y presiona BUSCAR, o elige un recorrido.";
		estadoMensaje = 0;
	}
}

void PantallaABB::actualizar(float tiempo)
{
	reloj += tiempo;
	
	if (modo == MODO_NINGUNO)
		return;
	
	if (visibles < cantidadSecuencia)
	{
		acumulado += tiempo;
		
		if (acumulado >= PASO_ANIMACION)
		{
			acumulado = 0;
			visibles++;
		}
	}
	
	if (modo == MODO_BUSQUEDA && !finalizado && visibles >= cantidadSecuencia)
	{
		finalizado = true;
		string comparaciones = aTexto(cantidadSecuencia) + (cantidadSecuencia == 1 ? " comparacion" : " comparaciones");
		
		if (encontrado != nullptr)
		{
			mensaje = "SI esta en la ruta de la aerolinea: " + encontrado->nombre + " (" + encontrado->codigo + ") - " + comparaciones;
			estadoMensaje = 1;
		}
		else
		{
			mensaje = "\"" + limpiar(textoBusqueda) + "\" NO esta en la ruta de la aerolinea - " + comparaciones;
			estadoMensaje = 2;
		}
	}
}

void PantallaABB::dibujarTexto(RenderWindow& ventana, float x, float y, const string& texto, unsigned int tamano, Color color, bool negrita)
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

void PantallaABB::dibujarTextoCentrado(RenderWindow& ventana, float cx, float cy, const string& texto, unsigned int tamano, Color color, bool negrita)
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

void PantallaABB::dibujarBoton(RenderWindow& ventana, float x, float y, float ancho, float alto, const string& texto, bool activo)
{
	Vector2i m = Mouse::getPosition(ventana);
	bool encima = dentro(m.x, m.y, x, y, ancho, alto);
	
	RectangleShape r(Vector2f(ancho, alto));
	r.setPosition(x, y);
	r.setFillColor(activo ? VERDE : (encima ? HOVER : BLANCO));
	r.setOutlineThickness(1);
	r.setOutlineColor(activo ? VERDE_OSCURO : BORDE);
	ventana.draw(r);
	
	dibujarTextoCentrado(ventana, x + ancho / 2, y + alto / 2, texto, 15, activo ? BLANCO : TEXTO, true);
}

void PantallaABB::dibujarControles(RenderWindow& ventana)
{
	dibujarTarjeta(ventana, TOP_X, TOP_Y, TOP_ANCHO, TOP_ALTO);
	
	RectangleShape caja(Vector2f(INPUT_ANCHO, INPUT_ALTO));
	caja.setPosition(INPUT_X, INPUT_Y);
	caja.setFillColor(BLANCO);
	caja.setOutlineThickness(1);
	caja.setOutlineColor(VERDE);
	ventana.draw(caja);
	
	if (textoBusqueda.empty())
		dibujarTexto(ventana, INPUT_X + 10, INPUT_Y + 7, "Nombre de la ciudad a buscar...", 16, GRIS);
	
	string cursor = fmod(reloj, 1.0f) < 0.5f ? "|" : "";
	dibujarTexto(ventana, INPUT_X + 10, INPUT_Y + 7, textoBusqueda + cursor, 16, TEXTO);
	
	dibujarBoton(ventana, BUSCAR_X, INPUT_Y, BUSCAR_ANCHO, INPUT_ALTO, "BUSCAR", false);
	
	string datos = "Nodos: " + aTexto(arbol->getCantidad()) + "     Altura: " + aTexto(arbol->getAltura());
	dibujarTexto(ventana, 1100, INPUT_Y + 7, datos, 16, TEXTO, true);
	
	Color colorMensaje = estadoMensaje == 1 ? VERDE : (estadoMensaje == 2 ? COLOR_ERROR : GRIS);
	dibujarTexto(ventana, INPUT_X, INPUT_Y + 52, mensaje, 16, colorMensaje, estadoMensaje != 0);
}

void PantallaABB::dibujarAristas(RenderWindow& ventana, NodoABB* nodo)
{
	if (nodo == nullptr)
		return;
	
	if (nodo->izquierdo != nullptr)
		dibujarLinea(ventana, Vector2f(nodo->x, nodo->y), Vector2f(nodo->izquierdo->x, nodo->izquierdo->y), 2, Color(170, 185, 175));
	
	if (nodo->derecho != nullptr)
		dibujarLinea(ventana, Vector2f(nodo->x, nodo->y), Vector2f(nodo->derecho->x, nodo->derecho->y), 2, Color(170, 185, 175));
	
	dibujarAristas(ventana, nodo->izquierdo);
	dibujarAristas(ventana, nodo->derecho);
}

void PantallaABB::dibujarNodos(RenderWindow& ventana, NodoABB* nodo)
{
	if (nodo == nullptr)
		return;
	
	int indice = indiceVisible(nodo);
	Color relleno = BLANCO;
	Color borde = VERDE;
	Color textoColor = TEXTO;
	
	if (indice >= 0)
	{
		bool actual = (indice == visibles - 1);
		
		if (modo == MODO_BUSQUEDA)
		{
			if (finalizado && nodo == encontrado)
			{
				relleno = COLOR_ORIGEN;
				borde = VERDE_OSCURO;
				textoColor = BLANCO;
			}
			else
			{
				relleno = actual ? COLOR_ESCALA : COLOR_ESCALA_SUAVE;
				borde = COLOR_ESCALA_BORDE;
			}
		}
		else
		{
			relleno = actual ? VERDE : VERDE_CLARO;
			borde = VERDE_OSCURO;
			textoColor = actual ? BLANCO : TEXTO;
		}
	}
	
	CircleShape circulo(RADIO);
	circulo.setOrigin(RADIO, RADIO);
	circulo.setPosition(nodo->x, nodo->y);
	circulo.setFillColor(relleno);
	circulo.setOutlineThickness(2);
	circulo.setOutlineColor(borde);
	ventana.draw(circulo);
	
	dibujarTextoCentrado(ventana, nodo->x, nodo->y, nodo->codigo, 13, textoColor, true);
	dibujarTextoCentrado(ventana, nodo->x, nodo->y + RADIO + 9, recortarNombre(nodo->nombre, 20), 11, GRIS);
	
	if (modo == MODO_RECORRIDO && indice >= 0)
	{
		CircleShape insignia(9);
		insignia.setOrigin(9, 9);
		insignia.setPosition(nodo->x + RADIO * 0.8f, nodo->y - RADIO * 0.9f);
		insignia.setFillColor(NAVY);
		ventana.draw(insignia);
		dibujarTextoCentrado(ventana, nodo->x + RADIO * 0.8f, nodo->y - RADIO * 0.9f, aTexto(indice + 1), 11, BLANCO, true);
	}
	
	dibujarNodos(ventana, nodo->izquierdo);
	dibujarNodos(ventana, nodo->derecho);
}

void PantallaABB::dibujarArbol(RenderWindow& ventana)
{
	dibujarTarjeta(ventana, ARB_X, ARB_Y, ARB_ANCHO, ARB_ALTO);
	dibujarTexto(ventana, ARB_X + 16, ARB_Y + 10, "Arbol binario de busqueda (ordenado por nombre de ciudad)", 15, GRIS);
	
	if (arbol->getRaiz() == nullptr)
	{
		dibujarTextoCentrado(ventana, ARB_X + ARB_ANCHO / 2, ARB_Y + ARB_ALTO / 2, "No hay ciudades registradas. Agregalas en GESTIONAR CIUDADES.", 18, GRIS);
		return;
	}
	
	dibujarAristas(ventana, arbol->getRaiz());
	
	if (modo == MODO_BUSQUEDA)
	{
		for (int i = 0; i + 1 < visibles && i + 1 < cantidadSecuencia; i++)
			dibujarLinea(ventana, Vector2f(secuencia[i]->x, secuencia[i]->y), Vector2f(secuencia[i + 1]->x, secuencia[i + 1]->y), 4, COLOR_ESCALA);
	}
	
	dibujarNodos(ventana, arbol->getRaiz());
}

void PantallaABB::dibujarSecuencia(RenderWindow& ventana)
{
	dibujarTarjeta(ventana, BAJO_X, BAJO_Y, BAJO_ANCHO, BAJO_ALTO);
	
	for (int i = 0; i < 3; i++)
		dibujarBoton(ventana, BOTON_X0 + i * BOTON_PASO, BOTON_Y, BOTON_ANCHO, BOTON_ALTO, NOMBRES_RECORRIDO[i], modo == MODO_RECORRIDO && tipoRecorrido == i);
	
	dibujarBoton(ventana, BOTON_X0 + 3 * BOTON_PASO, BOTON_Y, BOTON_ANCHO, BOTON_ALTO, "LIMPIAR", false);
	
	if (modo == MODO_NINGUNO)
	{
		dibujarTexto(ventana, BOTON_X0, BOTON_Y + 52, "Elige un recorrido para verlo animado, o busca una ciudad para ver su camino en el arbol.", 15, GRIS);
		return;
	}
	
	dibujarTexto(ventana, BOTON_X0, BOTON_Y + 46, tituloSecuencia, 15, TEXTO, true);
	
	string linea = "";
	float y = BOTON_Y + 70;
	int lineas = 0;
	
	for (int i = 0; i < visibles && i < cantidadSecuencia; i++)
	{
		string pieza = secuencia[i]->nombre + (i + 1 < cantidadSecuencia ? ", " : "");
		
		if (fuenteCargada)
		{
			Text medida(linea + pieza, fuente, 14);
			
			if (medida.getLocalBounds().width > BAJO_ANCHO - 40 && !linea.empty())
			{
				dibujarTexto(ventana, BOTON_X0, y, linea, 14, TEXTO);
				y += 21;
				lineas++;
				linea = "";
				
				if (lineas >= 3)
					return;
			}
		}
		
		linea += pieza;
	}
	
	if (!linea.empty() && lineas < 3)
		dibujarTexto(ventana, BOTON_X0, y, linea, 14, TEXTO);
}

void PantallaABB::dibujar(RenderWindow& ventana)
{
	dibujarTexto(ventana, 320, 18, "ARBOL BINARIO DE BUSQUEDA - CIUDADES", 26, TEXTO_CLARO, true);
	dibujarControles(ventana);
	dibujarArbol(ventana);
	dibujarSecuencia(ventana);
}
