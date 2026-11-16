#include "PantallaCiudades.h"
#include <sstream>
#include <cmath>

using namespace sf;
using namespace std;

PantallaCiudades::PantallaCiudades(Grafo* grafo)
{
	this->grafo = grafo;
	fuenteCargada = fuente.loadFromFile("resources/times.ttf");
	mapaCargado = texturaMapa.loadFromFile("resources/mapa.png");
	
	mapaX = 325;
	mapaY = 185;
	mapaAncho = 925;
	mapaAlto = 485;
	desplazamientoLista = 0;
	
	if (mapaCargado)
	{
		mapa.setTexture(texturaMapa);
		Vector2u tamano = texturaMapa.getSize();
		mapa.setPosition(mapaX, mapaY);
		mapa.setScale(mapaAncho / tamano.x, mapaAlto / tamano.y);
	}
	
	campoActivo = 0;
	editando = false;
	ubicacionElegida = false;
	posicionX = 0;
	posicionY = 0;
	mensaje = "Ingrese codigo y nombre, luego seleccione el punto en el mapa.";
}

bool PantallaCiudades::dentroDe(int x, int y, float px, float py, float ancho, float alto)
{
	return x >= px && x <= px + ancho && y >= py && y <= py + alto;
}

void PantallaCiudades::procesarEventos(RenderWindow& ventana, Event evento)
{
	if (evento.type == Event::MouseWheelScrolled)
	{
		float panelListaX = static_cast<float>(ventana.getSize().x) - 235;
		float limiteInferior = mapaY + mapaAlto - 10;
		float inicioLista = 268;
		int filasVisibles = static_cast<int>((limiteInferior - inicioLista) / 38.0f);
		if (filasVisibles < 1)
			filasVisibles = 1;
		
		int maximoDesplazamiento = grafo->getCantidadVertices() - filasVisibles;
		if (maximoDesplazamiento < 0)
			maximoDesplazamiento = 0;
		
		int mouseX = evento.mouseWheelScroll.x;
		int mouseY = evento.mouseWheelScroll.y;
		
		if (dentroDe(mouseX, mouseY, panelListaX, mapaY + 10, 220, mapaAlto - 20))
		{
			desplazamientoLista -= static_cast<int>(evento.mouseWheelScroll.delta) * 3;
			
			if (desplazamientoLista < 0)
				desplazamientoLista = 0;
			if (desplazamientoLista > maximoDesplazamiento)
				desplazamientoLista = maximoDesplazamiento;
		}
	}
	
	if (evento.type == Event::KeyPressed)
	{
		if (evento.key.code == Keyboard::Tab)
			campoActivo = campoActivo == 0 ? 1 : 0;
		
		if (evento.key.code == Keyboard::BackSpace)
		{
			if (campoActivo == 0 && !codigo.empty())
				codigo.pop_back();
			else if (campoActivo == 1 && !nombre.empty())
				nombre.pop_back();
		}
		
		if (evento.key.code == Keyboard::Escape)
		{
			limpiarFormulario();
			mensaje = "Seleccione otra ciudad o registre una nueva.";
		}
		
		if (evento.key.code == Keyboard::Return)
			guardarCiudad();
	}
	
	if (evento.type == Event::TextEntered && evento.text.unicode >= 32 && evento.text.unicode < 127)
	{
		char c = static_cast<char>(evento.text.unicode);
		
		if (campoActivo == 0 && codigo.size() < 10 && ((c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z') || (c >= '0' && c <= '9') || c == '-'))
			codigo += c;
		
		if (campoActivo == 1 && nombre.size() < 35 && ((c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z') || c == ' ' || c == '-'))
			nombre += c;
	}
	
	if (evento.type == Event::MouseButtonPressed && evento.mouseButton.button == Mouse::Left)
	{
		int x = evento.mouseButton.x;
		int y = evento.mouseButton.y;
		
		if (dentroDe(x, y, 335, 92, 260, 42))
		{
			if (!editando)
				campoActivo = 0;
		}
		else if (dentroDe(x, y, 610, 92, 360, 42))
			campoActivo = 1;
		else if (dentroDe(x, y, 990, 92, 235, 42))
			guardarCiudad();
		else if (dentroDe(x, y, 335, 145, 180, 30))
		{
			limpiarFormulario();
			mensaje = "Modo de registro: complete los datos y elija un punto del mapa.";
		}
		else if (dentroDe(x, y, 335, 145, 180, 30) == false && dentroDe(x, y, 520, 145, 170, 30))
		{
			if (!codigoSeleccionado.empty())
			{
				cargarCiudadSeleccionada();
				editando = true;
				mensaje = "Edicion activada. Puede cambiar el nombre y mover el marcador.";
			}
		}
		else if (dentroDe(x, y, 700, 145, 170, 30))
		{
			if (!codigoSeleccionado.empty())
			{
				if (grafo->eliminarCiudad(codigoSeleccionado))
				{
					limpiarFormulario();
					mensaje = "Ciudad eliminada.";
				}
			}
		}
		else if (dentroDe(x, y, mapaX, mapaY, mapaAncho, mapaAlto))
		{
			seleccionarCiudad(static_cast<float>(x), static_cast<float>(y));
		}
	}
}

void PantallaCiudades::actualizar(float tiempo)
{
}

void PantallaCiudades::seleccionarCiudad(float x, float y)
{
	NodoVertice* actual = grafo->getPrimero();
	
	while (actual != nullptr)
	{
		float mx = mapaX + actual->ciudad.getPosicionX();
		float my = mapaY + actual->ciudad.getPosicionY();
		float dx = x - mx;
		float dy = y - my;
		
		if (dx * dx + dy * dy <= 18.0f * 18.0f)
		{
			codigoSeleccionado = actual->ciudad.getCodigo();
			cargarCiudadSeleccionada();
			mensaje = "Ciudad seleccionada: " + actual->ciudad.getNombre();
			return;
		}
		
		actual = actual->siguiente;
	}
	
	posicionX = x - mapaX;
	posicionY = y - mapaY;
	ubicacionElegida = true;
	
	if (editando && !codigoSeleccionado.empty())
		mensaje = "Nueva ubicacion marcada. Guarde los cambios.";
	else
	{
		codigoSeleccionado = "";
		editando = false;
		mensaje = "Ubicacion seleccionada. Complete codigo y nombre y guarde.";
	}
}

void PantallaCiudades::cargarCiudadSeleccionada()
{
	NodoVertice* nodo = grafo->buscarCiudad(codigoSeleccionado);
	
	if (nodo == nullptr)
		return;
	
	codigo = nodo->ciudad.getCodigo();
	nombre = nodo->ciudad.getNombre();
	posicionX = nodo->ciudad.getPosicionX();
	posicionY = nodo->ciudad.getPosicionY();
	ubicacionElegida = true;
	campoActivo = 1;
	editando = false;
}

void PantallaCiudades::guardarCiudad()
{
	if (codigo.empty() || nombre.empty() || nombre.find_first_not_of(' ') == string::npos)
	{
		mensaje = "Debe ingresar el codigo y el nombre.";
		return;
	}
	
	if (!ubicacionElegida)
	{
		mensaje = "Seleccione primero la ubicacion en el mapa.";
		return;
	}
	
	if (editando && !codigoSeleccionado.empty())
	{
		if (grafo->modificarCiudad(codigoSeleccionado, nombre, posicionX, posicionY))
		{
			mensaje = "Ciudad actualizada correctamente.";
			codigoSeleccionado = codigo;
			editando = false;
		}
		else
								   mensaje = "No fue posible actualizar la ciudad.";
	}
	else
	{
		Ciudad nueva(codigo, nombre, posicionX, posicionY);
		
		if (grafo->agregarCiudad(nueva))
		{
			codigoSeleccionado = codigo;
			mensaje = "Ciudad registrada correctamente.";
		}
		else
								 mensaje = "Ese codigo ya existe. Utilice uno diferente.";
	}
}

void PantallaCiudades::limpiarFormulario()
{
	codigo = "";
	nombre = "";
	codigoSeleccionado = "";
	campoActivo = 0;
	editando = false;
	ubicacionElegida = false;
	posicionX = 0;
	posicionY = 0;
}

void PantallaCiudades::dibujarTexto(RenderWindow& ventana, float x, float y, const string& texto, unsigned int tamano, Color color)
{
	if (!fuenteCargada)
		return;
	
	Text t;
	t.setFont(fuente);
	t.setString(texto);
	t.setCharacterSize(tamano);
	t.setFillColor(color);
	t.setPosition(x, y);
	ventana.draw(t);
}

void PantallaCiudades::dibujarPanel(RenderWindow& ventana, float x, float y, float ancho, float alto, Color color)
{
	RectangleShape panel(Vector2f(ancho, alto));
	panel.setPosition(x, y);
	panel.setFillColor(color);
	ventana.draw(panel);
}

void PantallaCiudades::dibujarBoton(RenderWindow& ventana, float x, float y, float ancho, float alto, const string& texto, Color color)
{
	Vector2i mouse = Mouse::getPosition(ventana);
	bool encima = mouse.x >= x && mouse.x <= x + ancho && mouse.y >= y && mouse.y <= y + alto;
	
	Color colorFinal = color;
	
	if (encima)
	{
		colorFinal.r = static_cast<Uint8>(color.r > 215 ? 255 : color.r + 40);
		colorFinal.g = static_cast<Uint8>(color.g > 215 ? 255 : color.g + 40);
		colorFinal.b = static_cast<Uint8>(color.b > 215 ? 255 : color.b + 40);
	}
	
	RectangleShape boton(Vector2f(ancho, alto));
	boton.setPosition(x, y);
	boton.setFillColor(colorFinal);
	if (encima)
	{
		boton.setOutlineThickness(2);
		boton.setOutlineColor(Color(235, 250, 255));
	}
	ventana.draw(boton);
	
	RectangleShape subrayado(Vector2f(ancho, encima ? 5 : 3));
	subrayado.setPosition(x, y + alto - (encima ? 5 : 3));
	subrayado.setFillColor(encima ? Color(235, 250, 255) : Color(
																 static_cast<Uint8>(color.r > 45 ? color.r - 35 : color.r),
																 static_cast<Uint8>(color.g > 45 ? color.g - 35 : color.g),
																 static_cast<Uint8>(color.b > 45 ? color.b - 35 : color.b)));
	ventana.draw(subrayado);
	
	if (fuenteCargada)
	{
		Text t;
		t.setFont(fuente);
		t.setString(texto);
		t.setCharacterSize(encima ? 15 : 14);
		t.setStyle(Text::Bold);
		t.setFillColor(Color(20, 30, 40));
		FloatRect b = t.getLocalBounds();
		t.setPosition(x + (ancho - b.width) / 2 - b.left, y + (alto - b.height) / 2 - b.top - 2);
		ventana.draw(t);
	}
}

void PantallaCiudades::dibujarMarcadores(RenderWindow& ventana)
{
	NodoVertice* actual = grafo->getPrimero();
	
	while (actual != nullptr)
	{
		float x = mapaX + actual->ciudad.getPosicionX();
		float y = mapaY + actual->ciudad.getPosicionY();
		
		CircleShape marcador(9);
		marcador.setOrigin(9, 9);
		marcador.setPosition(x, y);
		marcador.setFillColor(actual->ciudad.getCodigo() == codigoSeleccionado ? Color(255, 150, 35) : Color(30, 190, 220));
		marcador.setOutlineThickness(3);
		marcador.setOutlineColor(Color(20, 30, 40));
		ventana.draw(marcador);
		
		actual = actual->siguiente;
	}
	
	if (ubicacionElegida && codigoSeleccionado.empty())
	{
		CircleShape marcador(9);
		marcador.setOrigin(9, 9);
		marcador.setPosition(mapaX + posicionX, mapaY + posicionY);
		marcador.setFillColor(Color(255, 150, 35));
		marcador.setOutlineThickness(3);
		marcador.setOutlineColor(Color::White);
		ventana.draw(marcador);
	}
}

void PantallaCiudades::dibujarListado(RenderWindow& ventana)
{
	NodoVertice* actual = grafo->getPrimero();
	float panelX = static_cast<float>(ventana.getSize().x) - 235;
	float x = panelX + 12;
	float limiteInferior = mapaY + mapaAlto - 10;
	float inicioLista = 268;
	int filasVisibles = static_cast<int>((limiteInferior - inicioLista) / 38.0f);
	
	if (filasVisibles < 1)
		filasVisibles = 1;
	
	int maximoDesplazamiento = grafo->getCantidadVertices() - filasVisibles;
	if (maximoDesplazamiento < 0)
		maximoDesplazamiento = 0;
	
	if (desplazamientoLista > maximoDesplazamiento)
		desplazamientoLista = maximoDesplazamiento;
	if (desplazamientoLista < 0)
		desplazamientoLista = 0;
	
	int omitidas = 0;
	while (actual != nullptr && omitidas < desplazamientoLista)
	{
		actual = actual->siguiente;
		omitidas++;
	}
	
	float y = inicioLista;
	int cantidad = 0;
	
	while (actual != nullptr && cantidad < filasVisibles)
	{
		RectangleShape fila(Vector2f(196, 30));
		fila.setPosition(x, y - 4);
		fila.setFillColor(actual->ciudad.getCodigo() == codigoSeleccionado ? Color(220, 235, 248) : Color(235, 243, 249));
		ventana.draw(fila);
		
		string linea = actual->ciudad.getCodigo() + "   " + actual->ciudad.getNombre();
		Color color = actual->ciudad.getCodigo() == codigoSeleccionado ? Color(0, 105, 155) : Color(35, 65, 90);
		dibujarTexto(ventana, x + 8, y, linea, 13, color);
		
		y += 38;
		cantidad++;
		actual = actual->siguiente;
	}
}

void PantallaCiudades::dibujar(RenderWindow& ventana)
{
	float anchoVentana = static_cast<float>(ventana.getSize().x);
	float altoVentana = static_cast<float>(ventana.getSize().y);
	
	float panelListaX = anchoVentana - 235;
	float panelListaAncho = 220;
	
	mapaX = 325;
	mapaY = 185;
	mapaAncho = panelListaX - mapaX - 15;
	mapaAlto = altoVentana - mapaY - 125;
	
	if (mapaAncho < 450)
		mapaAncho = 450;
	if (mapaAlto < 300)
		mapaAlto = 300;
	
	if (mapaCargado)
	{
		Vector2u tamano = texturaMapa.getSize();
		mapa.setPosition(mapaX, mapaY);
		mapa.setScale(mapaAncho / static_cast<float>(tamano.x), mapaAlto / static_cast<float>(tamano.y));
	}
	
	dibujarPanel(ventana, 300, 0, anchoVentana - 300, altoVentana, Color::White);
	
	dibujarTexto(ventana, 335, 25, "GESTION DE CIUDADES", 28, Color::Black);
	dibujarTexto(ventana, 335, 58, "Registre destinos colocando su ubicacion directamente en el mapa", 14, Color::Black);
	
	dibujarPanel(ventana, 325, 82, anchoVentana - 350, 100, Color(245, 248, 251));
	dibujarTexto(ventana, 340, 83, "CODIGO", 11, Color(25, 35, 45));
	dibujarTexto(ventana, 615, 83, "NOMBRE DE LA CIUDAD", 11, Color(25, 35, 45));
	
	RectangleShape campoCodigo(Vector2f(260, 32));
	campoCodigo.setPosition(335, 102);
	campoCodigo.setFillColor(Color(250, 253, 255));
	campoCodigo.setOutlineThickness(campoActivo == 0 ? 2 : 1);
	campoCodigo.setOutlineColor(campoActivo == 0 ? Color(35, 180, 220) : Color(175, 195, 210));
	ventana.draw(campoCodigo);
	
	RectangleShape campoNombre(Vector2f(360, 32));
	campoNombre.setPosition(610, 102);
	campoNombre.setFillColor(Color(250, 253, 255));
	campoNombre.setOutlineThickness(campoActivo == 1 ? 2 : 1);
	campoNombre.setOutlineColor(campoActivo == 1 ? Color(35, 180, 220) : Color(175, 195, 210));
	ventana.draw(campoNombre);
	
	dibujarTexto(ventana, 345, 106, codigo, 16, Color(20, 30, 40));
	dibujarTexto(ventana, 620, 106, nombre, 16, Color(20, 30, 40));
	
	if (campoActivo == 0 && fuenteCargada)
	{
		Text cursor;
		cursor.setFont(fuente);
		cursor.setString("|");
		cursor.setCharacterSize(16);
		cursor.setFillColor(Color(20, 110, 155));
		cursor.setPosition(345 + Text(codigo, fuente, 16).getLocalBounds().width, 104);
		ventana.draw(cursor);
	}
	
	if (campoActivo == 1 && fuenteCargada)
	{
		Text cursor;
		cursor.setFont(fuente);
		cursor.setString("|");
		cursor.setCharacterSize(16);
		cursor.setFillColor(Color(20, 110, 155));
		Text textoNombre(nombre, fuente, 16);
		cursor.setPosition(620 + textoNombre.getLocalBounds().width, 104);
		ventana.draw(cursor);
	}
	
	dibujarBoton(ventana, 990, 99, 235, 40, editando ? "GUARDAR CAMBIOS" : "REGISTRAR CIUDAD", Color(0, 145, 205));
	
	dibujarBoton(ventana, 335, 145, 180, 30, "NUEVA", Color(0, 145, 195));
	dibujarBoton(ventana, 520, 145, 170, 30, "EDITAR", Color(235, 125, 25));
	dibujarBoton(ventana, 700, 145, 170, 30, "ELIMINAR", Color(220, 55, 70));
	
	if (mapaCargado)
		ventana.draw(mapa);
	else
		dibujarPanel(ventana, mapaX, mapaY, mapaAncho, mapaAlto, Color(238, 242, 246));
	
	dibujarMarcadores(ventana);
	
	dibujarPanel(ventana, panelListaX, mapaY + 10, panelListaAncho, mapaAlto - 20, Color(245, 248, 251));
	dibujarTexto(ventana, panelListaX + 15, mapaY + 25, "CIUDADES REGISTRADAS", 14, Color(20, 30, 40));
	dibujarTexto(ventana, panelListaX + 15, mapaY + 50, "Total: " + to_string(grafo->getCantidadVertices()), 12, Color(25, 35, 45));
	
	RectangleShape separador(Vector2f(panelListaAncho - 30, 1));
	separador.setPosition(panelListaX + 15, mapaY + 82);
	separador.setFillColor(Color(190, 210, 225));
	ventana.draw(separador);
	
	dibujarListado(ventana);
	
	dibujarTexto(ventana, 335, altoVentana - 45, mensaje, 13, Color(145, 85, 20));
	dibujarTexto(ventana, 335, altoVentana - 70, "Seleccione un marcador para consultar; use EDITAR o ELIMINAR para administrarlo.", 12, Color(35, 45, 55));
}
