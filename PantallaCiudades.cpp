#include "PantallaCiudades.h"
#include <cmath>
#include <cctype>

using namespace sf;
using namespace std;

static const float PANEL_ANCHO = 203;
static const float PANEL_DESDE_DERECHA = 219;   
static const float LISTA_DESDE_MAPA_Y = 84;     

static string limpiarTextoCiudad(const string& texto)
{
	size_t inicio = texto.find_first_not_of(" \t\r\n");
	if (inicio == string::npos) return "";
	size_t fin = texto.find_last_not_of(" \t\r\n");
	return texto.substr(inicio, fin - inicio + 1);
}

static string normalizarNombreCiudad(const string& texto)
{
	string resultado = limpiarTextoCiudad(texto);
	for (size_t i = 0; i < resultado.size(); i++)
		resultado[i] = static_cast<char>(tolower(static_cast<unsigned char>(resultado[i])));
	return resultado;
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

static float anchoTexto(const Font& fuente, const string& texto, int tamano)
{
	Text medida;
	medida.setFont(fuente);
	medida.setCharacterSize(tamano);
	medida.setString(texto);
	return medida.getLocalBounds().width;
}

PantallaCiudades::PantallaCiudades(Grafo* grafo)
{
	this->grafo = grafo;
	fuenteCargada = fuente.loadFromFile("resources/times.ttf");
	mapaCargado = texturaMapa.loadFromFile("resources/mapa.png");
	
	mapaX = 308;
	mapaY = 185;
	mapaAncho = 925;
	mapaAlto = 485;
	desplazamientoLista = 0;
	campoActivo = 0;
	editando = false;
	ubicacionElegida = false;
	posicionX = 0;
	posicionY = 0;
	
	if (mapaCargado)
		mapa.setTexture(texturaMapa);
	
	mensaje = grafo == nullptr ? "No se pudo acceder al registro de ciudades." : "Ingrese codigo y nombre, luego seleccione una ubicacion en el mapa.";
}

bool PantallaCiudades::dentroDe(int x, int y, float px, float py, float ancho, float alto) const
{
	return x >= px && x <= px + ancho && y >= py && y <= py + alto;
}

int PantallaCiudades::obtenerFilasVisibles() const
{
	float limiteInferior = mapaY + mapaAlto - 10;
	int filas = static_cast<int>((limiteInferior - (mapaY + LISTA_DESDE_MAPA_Y)) / 38.0f);
	return filas < 1 ? 1 : filas;
}

void PantallaCiudades::procesarEventos(RenderWindow& ventana, Event evento)
{
	if (grafo == nullptr) return;
	
	if (evento.type == Event::MouseWheelScrolled)
	{
		float panelX = static_cast<float>(ventana.getSize().x) - PANEL_DESDE_DERECHA;
		int filasVisibles = obtenerFilasVisibles();
		int maximo = grafo->getCantidadVertices() - filasVisibles;
		
		if (maximo < 0) maximo = 0;
		
		if (dentroDe(evento.mouseWheelScroll.x, evento.mouseWheelScroll.y, panelX, mapaY, PANEL_ANCHO, mapaAlto))
		{
			if (evento.mouseWheelScroll.delta < 0)
				desplazamientoLista += 1;
			else if (evento.mouseWheelScroll.delta > 0)
				desplazamientoLista -= 1;
			
			if (desplazamientoLista < 0) desplazamientoLista = 0;
			if (desplazamientoLista > maximo) desplazamientoLista = maximo;
		}
	}
	
	if (evento.type == Event::KeyPressed)
	{
		if (evento.key.code == Keyboard::Tab)
			campoActivo = editando ? 1 : (campoActivo == 0 ? 1 : 0);
		
		if (evento.key.code == Keyboard::BackSpace)
		{
			if (campoActivo == 0 && !codigo.empty() && !editando)
				codigo.pop_back();
			else if (campoActivo == 1 && !nombre.empty())
				nombre.pop_back();
		}
		
		if (evento.key.code == Keyboard::Escape)
		{
			limpiarFormulario();
			mensaje = "Formulario limpio. Puede registrar otra ciudad.";
		}
		
		if (evento.key.code == Keyboard::Return)
			guardarCiudad();
	}
	
	if (evento.type == Event::TextEntered && evento.text.unicode >= 32 && evento.text.unicode < 127)
	{
		char c = static_cast<char>(evento.text.unicode);
		
		if (campoActivo == 0 && !editando && codigo.size() < 10 && ((c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z') || (c >= '0' && c <= '9') || c == '-'))
		{
			codigo += static_cast<char>(toupper(static_cast<unsigned char>(c)));
		}
		
		if (campoActivo == 1 && nombre.size() < 40 && ((c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z') || c == ' ' || c == '-' || c == '\'') && (!fuenteCargada || anchoTexto(fuente, nombre + c, 16) <= 235))
		{
			nombre += c;
		}
	}
	
	if (evento.type == Event::MouseButtonPressed && evento.mouseButton.button == Mouse::Left)
	{
		int x = evento.mouseButton.x;
		int y = evento.mouseButton.y;
		
		float panelX = static_cast<float>(ventana.getSize().x) - PANEL_DESDE_DERECHA;
		float inicioLista = mapaY + LISTA_DESDE_MAPA_Y;
		float finLista = mapaY + mapaAlto - 10;
		
		float topeLista = inicioLista - 4;
		
		if (dentroDe(x, y, panelX + 8, topeLista, 187, finLista - topeLista))
		{
			float relativo = y - topeLista;
			int indiceFila = static_cast<int>(relativo / 38.0f);
			float dentroFila = relativo - indiceFila * 38.0f;
			
			if (indiceFila >= 0 && indiceFila < obtenerFilasVisibles() && dentroFila < 30.0f)
			{
				seleccionarCiudadPorIndice(desplazamientoLista + indiceFila);
			}
			return;
		}
		
		if (dentroDe(x, y, 316, 102, 190, 34))
		{
			if (!editando) campoActivo = 0;
		}
		else if (dentroDe(x, y, 512, 102, 260, 34))
		{
			campoActivo = 1;
		}
		else if (dentroDe(x, y, 790, 99, 180, 40))
		{
			guardarCiudad();
		}
		else if (dentroDe(x, y, 316, 145, 145, 30))
		{
			limpiarFormulario();
			mensaje = "Modo de registro: complete los datos y elija un punto del mapa.";
		}
		else if (dentroDe(x, y, 470, 145, 145, 30))
		{
			if (!codigoSeleccionado.empty())
			{
				cargarCiudadSeleccionada();
				editando = true;
				mensaje = "Edicion activada. Cambie el nombre o seleccione otra ubicacion.";
			}
			else
			{
				mensaje = "Seleccione primero una ciudad del mapa o del listado.";
			}
		}
		else if (dentroDe(x, y, 624, 145, 145, 30))
		{
			if (!codigoSeleccionado.empty())
			{
				if (grafo->eliminarCiudad(codigoSeleccionado))
				{
					limpiarFormulario();
					mensaje = "Ciudad eliminada junto con sus conexiones.";
				}
				else
				{
					mensaje = "No fue posible eliminar la ciudad.";
				}
			}
			else
			{
				mensaje = "Seleccione la ciudad que desea eliminar.";
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
	if (grafo == nullptr) return;
	
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
	{
		mensaje = "Nueva ubicacion marcada. Presione GUARDAR CAMBIOS.";
	}
	else
	{
		codigoSeleccionado = "";
		editando = false;
		mensaje = "Ubicacion seleccionada. Complete codigo y nombre y registre.";
	}
}

void PantallaCiudades::seleccionarCiudadPorIndice(int indice)
{
	if (grafo == nullptr || indice < 0) return;
	
	NodoVertice* actual = grafo->getPrimero();
	int posicion = 0;
	
	while (actual != nullptr && posicion < indice)
	{
		actual = actual->siguiente;
		posicion++;
	}
	
	if (actual == nullptr) return;
	
	codigoSeleccionado = actual->ciudad.getCodigo();
	cargarCiudadSeleccionada();
	mensaje = "Ciudad seleccionada: " + actual->ciudad.getNombre();
}

void PantallaCiudades::cargarCiudadSeleccionada()
{
	if (grafo == nullptr) return;
	
	NodoVertice* nodo = grafo->buscarCiudad(codigoSeleccionado);
	if (nodo == nullptr) return;
	
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
	if (grafo == nullptr)
	{
		mensaje = "No hay un grafo disponible para guardar ciudades.";
		return;
	}
	
	codigo = limpiarTextoCiudad(codigo);
	nombre = limpiarTextoCiudad(nombre);
	
	if (codigo.empty() || nombre.empty())
	{
		mensaje = "Debe ingresar el codigo y el nombre de la ciudad.";
		return;
	}
	
	if (!ubicacionElegida)
	{
		mensaje = "Seleccione primero una ubicacion dentro del mapa.";
		return;
	}
	
	if (!std::isfinite(posicionX) || !std::isfinite(posicionY) || posicionX < 0 || posicionY < 0 || posicionX > mapaAncho || posicionY > mapaAlto)
	{
		mensaje = "La ubicacion seleccionada esta fuera de los limites del mapa.";
		return;
	}
	
	string nombreNuevo = normalizarNombreCiudad(nombre);
	NodoVertice* actual = grafo->getPrimero();
	
	while (actual != nullptr)
	{
		string nombreExistente = normalizarNombreCiudad(actual->ciudad.getNombre());
		bool esLaMisma = editando && actual->ciudad.getCodigo() == codigoSeleccionado;
		
		if (!esLaMisma)
		{
			if (nombreNuevo == nombreExistente)
			{
				mensaje = "Ya existe una ciudad registrada con ese nombre. Ingrese otro nombre.";
				return;
			}
			
			float dx = posicionX - actual->ciudad.getPosicionX();
			float dy = posicionY - actual->ciudad.getPosicionY();
			
			if (dx * dx + dy * dy <= 18.0f * 18.0f)
			{
				mensaje = "Ya hay una ciudad en esa ubicacion. Elija otro punto del mapa.";
				return;
			}
		}
		
		actual = actual->siguiente;
	}
	
	if (editando && !codigoSeleccionado.empty())
	{
		if (grafo->modificarCiudad(codigoSeleccionado, nombre, posicionX, posicionY))
		{
			codigoSeleccionado = codigo;
			editando = false;
			mensaje = "Ciudad actualizada correctamente.";
		}
		else
		{
			mensaje = "No fue posible actualizar la ciudad.";
		}
	}
	else
	{
		Ciudad nueva(codigo, nombre, posicionX, posicionY);
		
		if (grafo->agregarCiudad(nueva))
		{
			mensaje = "Ciudad registrada correctamente.";
			limpiarFormulario();
		}
		else
		{
			mensaje = "No se registro. Verifique que el codigo no exista y los datos sean validos.";
		}
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
	if (!fuenteCargada) return;
	
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
	panel.setOutlineThickness(1);
	panel.setOutlineColor(Color(215, 220, 215));
	ventana.draw(panel);
}

void PantallaCiudades::dibujarBoton(RenderWindow& ventana, float x, float y, float ancho, float alto, const string& texto, Color color)
{
	Vector2i mouse = Mouse::getPosition(ventana);
	
	bool encima = mouse.x >= x && mouse.x <= x + ancho && mouse.y >= y && mouse.y <= y + alto;
	Color colorFinal = color;
	
	if (encima)
	{
		colorFinal.r = static_cast<Uint8>(color.r > 215 ? 255 : color.r + 20);
		colorFinal.g = static_cast<Uint8>(color.g > 215 ? 255 : color.g + 20);
		colorFinal.b = static_cast<Uint8>(color.b > 215 ? 255 : color.b + 20);
	}
	
	RectangleShape boton(Vector2f(ancho, alto));
	boton.setPosition(x, y);
	boton.setFillColor(colorFinal);
	
	if (encima)
	{
		boton.setOutlineThickness(1);
		boton.setOutlineColor(Color(210, 220, 210));
	}
	
	ventana.draw(boton);
	
	if (fuenteCargada)
	{
		Text t;
		t.setFont(fuente);
		t.setString(texto);
		t.setCharacterSize(14);
		t.setFillColor(Color::White);
		
		FloatRect b = t.getLocalBounds();
		t.setPosition(x + (ancho - b.width) / 2 - b.left, y + (alto - b.height) / 2 - b.top - 2);
		ventana.draw(t);
	}
}

void PantallaCiudades::dibujarMarcadores(RenderWindow& ventana)
{
	if (grafo == nullptr) return;
	
	NodoVertice* actual = grafo->getPrimero();
	
	while (actual != nullptr)
	{
		float x = mapaX + actual->ciudad.getPosicionX();
		float y = mapaY + actual->ciudad.getPosicionY();
		
		CircleShape marcador(9);
		marcador.setOrigin(9, 9);
		marcador.setPosition(x, y);
		marcador.setFillColor(actual->ciudad.getCodigo() == codigoSeleccionado ? Color(190, 125, 70) : Color(90, 120, 95));
		marcador.setOutlineThickness(3);
		marcador.setOutlineColor(Color::White);
		
		ventana.draw(marcador);
		actual = actual->siguiente;
	}
	
	if (ubicacionElegida && (codigoSeleccionado.empty() || editando))
	{
		CircleShape marcador(9);
		marcador.setOrigin(9, 9);
		marcador.setPosition(mapaX + posicionX, mapaY + posicionY);
		marcador.setFillColor(Color(190, 125, 70));
		marcador.setOutlineThickness(3);
		marcador.setOutlineColor(Color::White);
		ventana.draw(marcador);
	}
}

void PantallaCiudades::dibujarListado(RenderWindow& ventana)
{
	if (grafo == nullptr) return;
	
	NodoVertice* actual = grafo->getPrimero();
	float panelX = static_cast<float>(ventana.getSize().x) - PANEL_DESDE_DERECHA;
	float x = panelX + 8;
	float inicioLista = mapaY + LISTA_DESDE_MAPA_Y;
	int filasVisibles = obtenerFilasVisibles();
	int maximo = grafo->getCantidadVertices() - filasVisibles;
	
	if (maximo < 0) maximo = 0;
	if (desplazamientoLista > maximo) desplazamientoLista = maximo;
	if (desplazamientoLista < 0) desplazamientoLista = 0;
	
	int omitidas = 0;
	
	while (actual != nullptr && omitidas < desplazamientoLista)
	{
		actual = actual->siguiente;
		omitidas++;
	}
	
	float y = inicioLista;
	int fila = 0;
	
	while (actual != nullptr && fila < filasVisibles)
	{
		RectangleShape fondoFila(Vector2f(187, 30));
		fondoFila.setPosition(x, y - 4);
		fondoFila.setFillColor(actual->ciudad.getCodigo() == codigoSeleccionado ? Color(225, 237, 225) : (fila % 2 == 0 ? Color(250, 251, 249) : Color(242, 245, 241)));
		ventana.draw(fondoFila);
		
		string linea = recortarTexto(fuente, actual->ciudad.getCodigo() + " - " + actual->ciudad.getNombre(), 13, 170);
		Color color = actual->ciudad.getCodigo() == codigoSeleccionado ? Color(65, 95, 70) : Color(55, 65, 55);
		
		dibujarTexto(ventana, x + 8, y, linea, 13, color);
		
		y += 38;
		fila++;
		actual = actual->siguiente;
	}
	
	if (grafo->getCantidadVertices() == 0)
	{
		dibujarTexto(ventana, x + 5, inicioLista, "No hay ciudades registradas.", 12, Color(140, 145, 140));
	}
	
	int totalCiudades = grafo->getCantidadVertices();
	float altoPista = (filasVisibles - 1) * 38.0f + 30.0f;
	float pistaY = inicioLista - 4;
	
	RectangleShape pista(Vector2f(5, altoPista));
	pista.setPosition(panelX + 196, pistaY);
	pista.setFillColor(Color(225, 229, 224));
	ventana.draw(pista);
	
	float altoIndicador = altoPista;
	if (totalCiudades > filasVisibles)
	{
		altoIndicador = altoPista * filasVisibles / totalCiudades;
		if (altoIndicador < 35.0f)
			altoIndicador = 35.0f;
	}
	
	float recorrido = altoPista - altoIndicador;
	float posicionIndicador = pistaY;
	if (maximo > 0)
		posicionIndicador += recorrido * desplazamientoLista / maximo;
	
	RectangleShape indicador(Vector2f(5, altoIndicador));
	indicador.setPosition(panelX + 196, posicionIndicador);
	indicador.setFillColor(Color(90, 120, 95));
	ventana.draw(indicador);
}

void PantallaCiudades::dibujar(RenderWindow& ventana)
{
	ventana.setView(ventana.getDefaultView());
	
	float anchoVentana = static_cast<float>(ventana.getSize().x);
	float altoVentana = static_cast<float>(ventana.getSize().y);
	float panelListaX = anchoVentana - PANEL_DESDE_DERECHA;
	float panelListaAncho = PANEL_ANCHO;
	
	mapaX = 308;
	mapaY = 185;
	mapaAncho = panelListaX - mapaX - 14;
	mapaAlto = altoVentana - mapaY - 115;
	
	if (mapaAncho < 300) mapaAncho = 300;
	if (mapaAlto < 220) mapaAlto = 220;
	
	if (mapaCargado)
	{
		Vector2u tamano = texturaMapa.getSize();
		mapa.setPosition(mapaX, mapaY);
		mapa.setScale(mapaAncho / static_cast<float>(tamano.x), mapaAlto / static_cast<float>(tamano.y));
	}
	
	ventana.clear(Color(242, 244, 242));
	
	dibujarPanel(ventana, 300, 0, anchoVentana - 300, altoVentana, Color(242, 244, 242));
	
	dibujarPanel(ventana, 300, 0, anchoVentana - 300, 80, Color::White);
	
	dibujarTexto(ventana, 318, 25, "GESTION DE CIUDADES", 28, Color(55, 70, 60));
	dibujarTexto(ventana, 318, 58, "Registre destinos colocando su ubicacion directamente en el mapa", 14, Color(120, 125, 120));
	
	dibujarPanel(ventana, 308, 82, anchoVentana - 308 - 16, 100, Color(248, 249, 247));
	
	dibujarTexto(ventana, 323, 83, "CODIGO", 11, Color(75, 80, 75));
	dibujarTexto(ventana, 520, 83, "NOMBRE DE LA CIUDAD", 11, Color(75, 80, 75));
	
	RectangleShape campoCodigo(Vector2f(190, 34));
	campoCodigo.setPosition(316, 102);
	campoCodigo.setFillColor(Color(250, 251, 249));
	campoCodigo.setOutlineThickness(campoActivo == 0 ? 2 : 1);
	campoCodigo.setOutlineColor(campoActivo == 0 ? Color(115, 145, 120) : Color(210, 215, 210));
	ventana.draw(campoCodigo);
	
	RectangleShape campoNombre(Vector2f(260, 34));
	campoNombre.setPosition(512, 102);
	campoNombre.setFillColor(Color(250, 251, 249));
	campoNombre.setOutlineThickness(campoActivo == 1 ? 2 : 1);
	campoNombre.setOutlineColor(campoActivo == 1 ? Color(115, 145, 120) : Color(210, 215, 210));
	ventana.draw(campoNombre);
	
	dibujarTexto(ventana, 326, 106, codigo, 16, Color(45, 50, 45));
	dibujarTexto(ventana, 522, 106, nombre, 16, Color(45, 50, 45));
	
	dibujarBoton(ventana, 790, 99, 180, 40, editando ? "GUARDAR CAMBIOS" : "REGISTRAR CIUDAD", Color(90, 120, 95));
	dibujarBoton(ventana, 316, 145, 145, 30, "NUEVA", Color(90, 120, 95));
	dibujarBoton(ventana, 470, 145, 145, 30, "EDITAR", Color(120, 130, 120));
	dibujarBoton(ventana, 624, 145, 145, 30, "ELIMINAR", Color(165, 85, 80));
	
	if (mapaCargado)
	{
		ventana.draw(mapa);
	}
	else
	{
		dibujarPanel(ventana, mapaX, mapaY, mapaAncho, mapaAlto, Color(248, 249, 247));
	}
	
	dibujarMarcadores(ventana);
	
	dibujarPanel(ventana, panelListaX, mapaY, panelListaAncho, mapaAlto, Color::White);
	dibujarPanel(ventana, panelListaX, mapaY, panelListaAncho, 38, Color(230, 235, 230));
	
	dibujarTexto(ventana, panelListaX + 15, mapaY + 10, "CIUDADES REGISTRADAS", 14, Color(55, 70, 60));
	
	int total = grafo == nullptr ? 0 : grafo->getCantidadVertices();
	dibujarTexto(ventana, panelListaX + 15, mapaY + 48, "Total: " + to_string(total), 12, Color(90, 95, 90));
	
	RectangleShape separador(Vector2f(panelListaAncho - 30, 1));
	separador.setPosition(panelListaX + 15, mapaY + 72);
	separador.setFillColor(Color(220, 225, 220));
	ventana.draw(separador);
	
	dibujarListado(ventana);
	
	dibujarTexto(ventana, 318, altoVentana - 70, "Seleccione una ciudad del mapa o del listado. Use EDITAR o ELIMINAR para administrarla.", 12, Color(75, 80, 75));
	dibujarTexto(ventana, 318, altoVentana - 45, mensaje, 13, Color(90, 120, 95));
}
