#include "PantallaAVL.h"
#include <algorithm>
#include <cmath>
#include <sstream>

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
	const float ID_X = 336.0f;
	const float ID_Y = 76.0f;
	const float ID_ANCHO = 245.0f;
	const float VUELO_X = 594.0f;
	const float VUELO_ANCHO = 225.0f;
	const float INPUT_ALTO = 36.0f;
	const float BUSCAR_X = 832.0f;
	const float BUSCAR_ANCHO = 110.0f;
	const float ARB_X = 320.0f;
	const float ARB_Y = 172.0f;
	const float ARB_ANCHO = 1160.0f;
	const float ARB_ALTO = 440.0f;
	const float BAJO_X = 320.0f;
	const float BAJO_Y = 624.0f;
	const float BAJO_ANCHO = 1160.0f;
	const float BAJO_ALTO = 160.0f;
	const float BOTON_Y = 636.0f;
	const float BOTON_ALTO = 34.0f;
	const float BOTON_ANCHO = 160.0f;
	const float BOTON_PASO = 170.0f;
	const float BOTON_X0 = 336.0f;
	const float PASO_ANIMACION = 0.45f;
	const char* const NOMBRES_RECORRIDO[3] = { "INORDEN", "PREORDEN", "POSTORDEN" };
	
	string aTexto(int valor)
	{
		ostringstream salida;
		salida << valor;
		return salida.str();
	}
	
	bool dentro(int x, int y, float rx, float ry, float rw, float rh)
	{
		return x >= rx && x <= rx + rw && y >= ry && y <= ry + rh;
	}
	
	void dibujarLinea(RenderWindow& ventana, Vector2f inicio, Vector2f fin, float grosor, Color color)
	{
		float dx = fin.x - inicio.x;
		float dy = fin.y - inicio.y;
		RectangleShape linea(Vector2f(sqrt(dx * dx + dy * dy), grosor));
		linea.setOrigin(0, grosor / 2);
		linea.setPosition(inicio);
		linea.setRotation(atan2(dy, dx) * 180.0f / 3.14159265f);
		linea.setFillColor(color);
		ventana.draw(linea);
	}
	
	void dibujarTarjeta(RenderWindow& ventana, float x, float y, float ancho, float alto)
	{
		RectangleShape tarjeta(Vector2f(ancho, alto));
		tarjeta.setPosition(x, y);
		tarjeta.setFillColor(BLANCO);
		tarjeta.setOutlineThickness(1);
		tarjeta.setOutlineColor(BORDE);
		ventana.draw(tarjeta);
	}
	
	string recortar(const string& texto, size_t maximo)
	{
		if (texto.size() <= maximo)
			return texto;
		if (maximo < 4)
			return texto.substr(0, maximo);
		return texto.substr(0, maximo - 3) + "...";
	}
}

PantallaAVL::PantallaAVL(ArbolAVL* arbol, ListaPasajeros* listaPasajeros, ListaReservas* listaReservas)
{
	this->arbol = arbol;
	this->listaPasajeros = listaPasajeros;
	this->listaReservas = listaReservas;
	fuenteCargada = fuente.loadFromFile("resources/times.ttf");
	identificacion = "";
	codigoVuelo = "";
	mensaje = "Busque una cedula y, opcionalmente, un codigo de vuelo para verificar la reserva.";
	tituloSecuencia = "";
	campoActivo = 0;
	modo = MODO_NINGUNO;
	tipoRecorrido = -1;
	visibles = 0;
	estadoMensaje = 0;
	acumulado = 0;
	reloj = 0;
	separacionX = 0;
	separacionY = 0;
	finalizado = false;
	encontrado = nullptr;
	
	reconstruirArbol();
	calcularPosiciones();
}

void PantallaAVL::reconstruirArbol()
{
	if (arbol == nullptr || listaPasajeros == nullptr || listaReservas == nullptr)
	{
		mensaje = "No se pudo cargar el AVL: faltan datos del sistema.";
		estadoMensaje = 2;
		return;
	}
	
	arbol->limpiar();
	for (NodoReserva* reserva = listaReservas->getPrimero(); reserva != nullptr; reserva = reserva->siguiente)
	{
		NodoPasajero* pasajero = listaPasajeros->buscar(reserva->identificacionPasajero);
		if (pasajero != nullptr && !reserva->codigoVuelo.empty())
			arbol->insertar(pasajero->pasajero, reserva->codigoVuelo);
	}
}

void PantallaAVL::calcularPosiciones()
{
	if (arbol == nullptr || arbol->getCantidad() == 0)
		return;
	
	separacionX = ARB_ANCHO / (arbol->getCantidad() + 1.0f);
	int altura = arbol->getRaiz()->altura;
	separacionY = altura > 1
		? max(14.0f, min(58.0f, (ARB_ALTO - 105.0f) / (altura - 1.0f)))
		: 0.0f;
	int contador = 0;
	asignarPosiciones(arbol->getRaiz(), 0, contador);
}

void PantallaAVL::asignarPosiciones(NodoAVL* nodo, int profundidad, int& contador)
{
	if (nodo == nullptr)
		return;
	
	asignarPosiciones(nodo->izquierdo, profundidad + 1, contador);
	nodo->x = ARB_X + separacionX * (contador + 1);
	nodo->y = ARB_Y + 66.0f + profundidad * separacionY;
	contador++;
	asignarPosiciones(nodo->derecho, profundidad + 1, contador);
}

void PantallaAVL::iniciarBusqueda()
{
	if (arbol == nullptr || identificacion.empty())
	{
		mensaje = "Ingrese la cedula del pasajero.";
		estadoMensaje = 2;
		return;
	}
	
	secuencia.clear();
	for (NodoAVL* nodo = arbol->getRaiz(); nodo != nullptr;)
	{
		secuencia.push_back(nodo);
		if (identificacion == nodo->pasajero.getIdentificacion())
			break;
		nodo = identificacion < nodo->pasajero.getIdentificacion() ? nodo->izquierdo : nodo->derecho;
	}
	
	Pasajero* pasajeroEncontrado = arbol->buscar(identificacion);
	encontrado = pasajeroEncontrado != nullptr && !secuencia.empty() &&
		secuencia.back()->pasajero.getIdentificacion() == identificacion ? secuencia.back() : nullptr;
	visibles = secuencia.empty() ? 0 : 1;
	acumulado = 0;
	modo = MODO_BUSQUEDA;
	tipoRecorrido = -1;
	finalizado = false;
	tituloSecuencia = "Camino de busqueda por cedula; comparaciones: " + aTexto(static_cast<int>(secuencia.size()));
	mensaje = "Buscando pasajero en el arbol AVL...";
	estadoMensaje = 0;
}

void PantallaAVL::iniciarRecorrido(int tipo)
{
	if (arbol == nullptr || arbol->getCantidad() == 0)
	{
		mensaje = "No hay pasajeros con vuelos reservados para recorrer.";
		estadoMensaje = 2;
		return;
	}
	
	secuencia.resize(arbol->getCantidad());
	if (tipo == 0)
		arbol->recorrerInorden(secuencia.data());
	else if (tipo == 1)
		arbol->recorrerPreorden(secuencia.data());
	else
		arbol->recorrerPostorden(secuencia.data());
	
	tituloSecuencia = string("Recorrido ") + NOMBRES_RECORRIDO[tipo] +
		" de los pasajeros; identificadores: " + aTexto(static_cast<int>(secuencia.size()));
	visibles = 1;
	acumulado = 0;
	modo = MODO_RECORRIDO;
	tipoRecorrido = tipo;
	finalizado = false;
	encontrado = nullptr;
	mensaje = tituloSecuencia;
	estadoMensaje = 0;
}

void PantallaAVL::procesarEventos(RenderWindow& ventana, Event evento)
{
	(void)ventana;
	
	if (evento.type == Event::TextEntered)
	{
		string* campo = campoActivo == 0 ? &identificacion : &codigoVuelo;
		if (evento.text.unicode == 8)
		{
			if (!campo->empty())
				campo->erase(campo->size() - 1);
		}
		else if (evento.text.unicode == 13)
			iniciarBusqueda();
		else if (evento.text.unicode >= 32 && evento.text.unicode < 127 && campo->size() < 30)
			*campo += static_cast<char>(evento.text.unicode);
		return;
	}
	
	if (evento.type != Event::MouseButtonPressed || evento.mouseButton.button != Mouse::Left)
		return;
	
	int mx = evento.mouseButton.x;
	int my = evento.mouseButton.y;
	if (dentro(mx, my, ID_X, ID_Y, ID_ANCHO, INPUT_ALTO))
	{
		campoActivo = 0;
		return;
	}
	if (dentro(mx, my, VUELO_X, ID_Y, VUELO_ANCHO, INPUT_ALTO))
	{
		campoActivo = 1;
		return;
	}
	if (dentro(mx, my, BUSCAR_X, ID_Y, BUSCAR_ANCHO, INPUT_ALTO))
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
		secuencia.clear();
		visibles = 0;
		encontrado = nullptr;
		mensaje = "Busque una cedula y, opcionalmente, un codigo de vuelo para verificar la reserva.";
		estadoMensaje = 0;
	}
}

void PantallaAVL::actualizar(float tiempo)
{
	reloj += tiempo;
	if (modo == MODO_NINGUNO)
		return;
	
	if (visibles < static_cast<int>(secuencia.size()))
	{
		acumulado += tiempo;
		if (acumulado >= PASO_ANIMACION)
		{
			acumulado = 0;
			visibles++;
		}
	}
	
	if (modo == MODO_BUSQUEDA && !finalizado && visibles >= static_cast<int>(secuencia.size()))
	{
		finalizado = true;
		if (encontrado == nullptr)
		{
			mensaje = "No existe un pasajero con cedula " + identificacion + " y vuelos reservados.";
			estadoMensaje = 2;
			return;
		}
		
		string vuelos;
		for (NodoVueloAVL* vuelo = secuencia.back()->vuelos; vuelo != nullptr; vuelo = vuelo->siguiente)
		{
			if (!vuelos.empty())
				vuelos += ", ";
			vuelos += vuelo->codigo;
		}
		
		if (codigoVuelo.empty())
		{
			mensaje = "Pasajero: " + encontrado->pasajero.getNombre() + " | vuelos: " +
				(vuelos.empty() ? "ninguno" : vuelos);
			estadoMensaje = 1;
		}
		else if (arbol->tieneVuelo(identificacion, codigoVuelo))
		{
			mensaje = "SI esta asignado al vuelo " + codigoVuelo + " | vuelos: " + vuelos;
			estadoMensaje = 1;
		}
		else
		{
			mensaje = "NO esta asignado al vuelo " + codigoVuelo + " | vuelos: " +
				(vuelos.empty() ? "ninguno" : vuelos);
			estadoMensaje = 2;
		}
	}
}

int PantallaAVL::indiceVisible(NodoAVL* nodo) const
{
	for (int i = 0; i < visibles && i < static_cast<int>(secuencia.size()); i++)
		if (secuencia[i] == nodo)
			return i;
	return -1;
}

void PantallaAVL::dibujarTexto(RenderWindow& ventana, float x, float y, const string& texto, unsigned int tamano, Color color, bool negrita)
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

void PantallaAVL::dibujarTextoCentrado(RenderWindow& ventana, float cx, float cy, const string& texto, unsigned int tamano, Color color, bool negrita)
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
	FloatRect limites = etiqueta.getLocalBounds();
	etiqueta.setPosition(cx - limites.width / 2 - limites.left, cy - limites.height / 2 - limites.top);
	ventana.draw(etiqueta);
}

void PantallaAVL::dibujarBoton(RenderWindow& ventana, float x, float y, float ancho, float alto, const string& texto, bool activo)
{
	Vector2i mouse = Mouse::getPosition(ventana);
	bool encima = dentro(mouse.x, mouse.y, x, y, ancho, alto);
	RectangleShape boton(Vector2f(ancho, alto));
	boton.setPosition(x, y);
	boton.setFillColor(activo ? VERDE : (encima ? HOVER : BLANCO));
	boton.setOutlineThickness(1);
	boton.setOutlineColor(activo ? VERDE_OSCURO : BORDE);
	ventana.draw(boton);
	dibujarTextoCentrado(ventana, x + ancho / 2, y + alto / 2, texto, 15, activo ? BLANCO : TEXTO, true);
}

void PantallaAVL::dibujarControles(RenderWindow& ventana)
{
	dibujarTarjeta(ventana, TOP_X, TOP_Y, TOP_ANCHO, TOP_ALTO);
	
	RectangleShape campoId(Vector2f(ID_ANCHO, INPUT_ALTO));
	campoId.setPosition(ID_X, ID_Y);
	campoId.setFillColor(BLANCO);
	campoId.setOutlineThickness(1);
	campoId.setOutlineColor(campoActivo == 0 ? VERDE : BORDE);
	ventana.draw(campoId);
	
	RectangleShape campoVuelo(Vector2f(VUELO_ANCHO, INPUT_ALTO));
	campoVuelo.setPosition(VUELO_X, ID_Y);
	campoVuelo.setFillColor(BLANCO);
	campoVuelo.setOutlineThickness(1);
	campoVuelo.setOutlineColor(campoActivo == 1 ? VERDE : BORDE);
	ventana.draw(campoVuelo);
	
	string textoId = identificacion.empty() ? "Cedula del pasajero" : identificacion;
	string textoVuelo = codigoVuelo.empty() ? "Codigo de vuelo (opcional)" : codigoVuelo;
	if (campoActivo == 0 && fmod(reloj, 1.0f) < 0.5f)
		textoId += "|";
	if (campoActivo == 1 && fmod(reloj, 1.0f) < 0.5f)
		textoVuelo += "|";
	dibujarTexto(ventana, ID_X + 9, ID_Y + 7, textoId, 14, identificacion.empty() ? GRIS : TEXTO);
	dibujarTexto(ventana, VUELO_X + 9, ID_Y + 7, textoVuelo, 13, codigoVuelo.empty() ? GRIS : TEXTO);
	dibujarBoton(ventana, BUSCAR_X, ID_Y, BUSCAR_ANCHO, INPUT_ALTO, "BUSCAR", false);
	
	string datos = "Pasajeros: " + aTexto(arbol == nullptr ? 0 : arbol->getCantidad()) +
		"     Altura: " + aTexto(arbol == nullptr || arbol->getRaiz() == nullptr ? 0 : arbol->getRaiz()->altura);
	dibujarTexto(ventana, 970, ID_Y + 8, datos, 15, TEXTO, true);
	
	Color colorMensaje = estadoMensaje == 1 ? VERDE : (estadoMensaje == 2 ? COLOR_ERROR : GRIS);
	dibujarTexto(ventana, ID_X, ID_Y + 52, recortar(mensaje, 140), 14, colorMensaje, estadoMensaje != 0);
}

void PantallaAVL::dibujarAristas(RenderWindow& ventana, NodoAVL* nodo)
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

void PantallaAVL::dibujarNodos(RenderWindow& ventana, NodoAVL* nodo)
{
	if (nodo == nullptr)
		return;
	
	int indice = indiceVisible(nodo);
	Color relleno = BLANCO;
	Color borde = VERDE;
	Color textoColor = TEXTO;
	if (indice >= 0)
	{
		bool actual = indice == visibles - 1;
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
	
	float radio = max(7.0f, min(19.0f, separacionX * 0.34f));
	CircleShape circulo(radio);
	circulo.setOrigin(radio, radio);
	circulo.setPosition(nodo->x, nodo->y);
	circulo.setFillColor(relleno);
	circulo.setOutlineThickness(2);
	circulo.setOutlineColor(borde);
	ventana.draw(circulo);
	
	unsigned int tamano = static_cast<unsigned int>(max(6.0f, min(13.0f, separacionX * 0.20f)));
	dibujarTextoCentrado(ventana, nodo->x, nodo->y, recortar(nodo->pasajero.getIdentificacion(), 9), tamano, textoColor, true);
	int balance = (nodo->izquierdo == nullptr ? 0 : nodo->izquierdo->altura) -
		(nodo->derecho == nullptr ? 0 : nodo->derecho->altura);
	string detalle = "h" + aTexto(nodo->altura) + " b" + aTexto(balance);
	dibujarTextoCentrado(ventana, nodo->x, nodo->y + radio + 8, detalle, 9, GRIS);
	
	if (separacionX >= 54.0f)
		dibujarTextoCentrado(ventana, nodo->x, nodo->y + radio + 20,
							 recortar(nodo->pasajero.getNombre(), 15), 9, TEXTO);
	
	dibujarNodos(ventana, nodo->izquierdo);
	dibujarNodos(ventana, nodo->derecho);
}

void PantallaAVL::dibujarArbol(RenderWindow& ventana)
{
	dibujarTarjeta(ventana, ARB_X, ARB_Y, ARB_ANCHO, ARB_ALTO);
	dibujarTexto(ventana, ARB_X + 16, ARB_Y + 10, "AVL ordenado por cedula | cada nodo muestra altura y factor de balance", 15, GRIS);
	
	if (arbol == nullptr || arbol->getRaiz() == nullptr)
	{
		dibujarTextoCentrado(ventana, ARB_X + ARB_ANCHO / 2, ARB_Y + ARB_ALTO / 2,
							 "No hay pasajeros con vuelos reservados.", 18, GRIS);
		return;
	}
	
	dibujarAristas(ventana, arbol->getRaiz());
	if (modo == MODO_BUSQUEDA)
		for (int i = 0; i + 1 < visibles && i + 1 < static_cast<int>(secuencia.size()); i++)
			dibujarLinea(ventana, Vector2f(secuencia[i]->x, secuencia[i]->y),
						 Vector2f(secuencia[i + 1]->x, secuencia[i + 1]->y), 4, COLOR_ESCALA);
	dibujarNodos(ventana, arbol->getRaiz());
}

void PantallaAVL::dibujarSecuencia(RenderWindow& ventana)
{
	dibujarTarjeta(ventana, BAJO_X, BAJO_Y, BAJO_ANCHO, BAJO_ALTO);
	for (int i = 0; i < 3; i++)
		dibujarBoton(ventana, BOTON_X0 + i * BOTON_PASO, BOTON_Y, BOTON_ANCHO, BOTON_ALTO,
					 NOMBRES_RECORRIDO[i], modo == MODO_RECORRIDO && tipoRecorrido == i);
	dibujarBoton(ventana, BOTON_X0 + 3 * BOTON_PASO, BOTON_Y, BOTON_ANCHO, BOTON_ALTO, "LIMPIAR", false);
	
	if (modo == MODO_NINGUNO || secuencia.empty())
	{
		dibujarTexto(ventana, BOTON_X0, BOTON_Y + 52,
					 "Elija un recorrido para animarlo o busque un pasajero para comprobar sus vuelos.", 15, GRIS);
		return;
	}
	
	dibujarTexto(ventana, BOTON_X0, BOTON_Y + 46,
				 recortar(tituloSecuencia.empty() ? mensaje : tituloSecuencia, 130), 14, TEXTO, true);
	string linea = "Orden: ";
	for (int i = 0; i < visibles && i < static_cast<int>(secuencia.size()); i++)
	{
		string pieza = secuencia[i]->pasajero.getIdentificacion();
		if (i + 1 < static_cast<int>(secuencia.size()))
			pieza += ", ";
		if (linea.size() + pieza.size() > 150)
		{
			linea += "...";
			break;
		}
		linea += pieza;
	}
	dibujarTexto(ventana, BOTON_X0, BOTON_Y + 72, linea, 13, TEXTO);
}

void PantallaAVL::dibujar(RenderWindow& ventana)
{
	dibujarTexto(ventana, 320, 18, "ARBOL AVL - PASAJEROS Y VUELOS", 26, TEXTO_CLARO, true);
	dibujarControles(ventana);
	dibujarArbol(ventana);
	dibujarSecuencia(ventana);
}
