#include "PantallaVuelos.h"
#include <sstream>
#include <iomanip>
#include <cctype>

static const int FILAS_VUELOS = 9;

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

static bool existeVueloDirecto(ListaVuelos* lista, const string& origen, const string& destino, const string& ignorarCodigo)
{
	for (NodoVuelo* actual = lista->getPrimero(); actual != nullptr; actual = actual->siguiente)
	{
		if (actual->vuelo.getCodigoOrigen() == origen &&
			actual->vuelo.getCodigoDestino() == destino &&
			actual->vuelo.getCodigo() != ignorarCodigo)
			return true;
	}
	return false;
}

PantallaVuelos::PantallaVuelos(ListaVuelos* listaVuelos, Grafo* grafo, ListaRutas* listaRutas)
{
	this->listaVuelos = listaVuelos;
	this->grafo = grafo;
	this->listaRutas = listaRutas;
	desplazamientoLista = 0;
	fuenteCargada = fuente.loadFromFile("resources/times.ttf");
	codigo = "";
	codigoOrigen = "";
	codigoDestino = "";
	costo = "";
	duracion = "";
	codigoSeleccionado = "";
	mensaje = "";
	campoActivo = 0;
	editando = false;
	confirmarEliminar = false;
	cursorVisible = true;
	tiempoCursor = 0;
	origenDesplegado = false;
	destinoDesplegado = false;
	desplazamientoOrigen = 0;
	desplazamientoDestino = 0;
	filtroOrigen = "";
	filtroDestino = "";
}

void PantallaVuelos::procesarEventos(RenderWindow& ventana, Event evento)
{
	if (confirmarEliminar)
	{
		if (evento.type == Event::MouseButtonPressed && evento.mouseButton.button == Mouse::Left)
		{
			float x = evento.mouseButton.x;
			float y = evento.mouseButton.y;
			if (x >= 610 && x <= 710 && y >= 420 && y <= 465)
			{
				string vueloEliminado = codigoSeleccionado;
				grafo->eliminarVuelo(vueloEliminado);
				if (listaVuelos->eliminar(vueloEliminado))
				{
					int rutasEliminadas = listaRutas != nullptr ? listaRutas->eliminarPorVuelo(vueloEliminado) : 0;
					if (rutasEliminadas > 0)
						mensaje = "Vuelo eliminado junto con " + to_string(rutasEliminadas) + " ruta(s) asignada(s).";
					else
						mensaje = "Vuelo eliminado correctamente.";
				}
				else
										  mensaje = "No se pudo eliminar el vuelo.";
				confirmarEliminar = false;
				limpiarFormulario();
			}
			else if (x >= 735 && x <= 835 && y >= 420 && y <= 465)
			{
				confirmarEliminar = false;
				mensaje = "Eliminacion cancelada.";
			}
		}
		if (evento.type == Event::KeyPressed && evento.key.code == Keyboard::Escape)
			confirmarEliminar = false;
		return;
	}
	
	if (evento.type == Event::MouseWheelScrolled)
	{
		if (origenDesplegado && evento.mouseWheelScroll.x >= 340 && evento.mouseWheelScroll.x <= 730 &&
			evento.mouseWheelScroll.y >= 242 && evento.mouseWheelScroll.y < 402)
		{
			int total = (int)obtenerCiudadesDisponibles(true).size();
			int maximo = total > 4 ? total - 4 : 0;
			desplazamientoOrigen -= evento.mouseWheelScroll.delta > 0 ? 1 : -1;
			if (desplazamientoOrigen < 0) desplazamientoOrigen = 0;
			if (desplazamientoOrigen > maximo) desplazamientoOrigen = maximo;
		}
		else if (destinoDesplegado && evento.mouseWheelScroll.x >= 340 && evento.mouseWheelScroll.x <= 730 &&
				 evento.mouseWheelScroll.y >= 302 && evento.mouseWheelScroll.y < 462)
		{
			int total = (int)obtenerCiudadesDisponibles(false).size();
			int maximo = total > 4 ? total - 4 : 0;
			desplazamientoDestino -= evento.mouseWheelScroll.delta > 0 ? 1 : -1;
			if (desplazamientoDestino < 0) desplazamientoDestino = 0;
			if (desplazamientoDestino > maximo) desplazamientoDestino = maximo;
		}
		else if (evento.mouseWheelScroll.x >= 780 && evento.mouseWheelScroll.x <= (int)ventana.getSize().x - 15 &&
				 evento.mouseWheelScroll.y >= 190 && evento.mouseWheelScroll.y < 550)
		{
			int maximo = listaVuelos->getCantidad() - FILAS_VUELOS;
			if (maximo < 0) maximo = 0;
			if (evento.mouseWheelScroll.delta < 0)
				desplazamientoLista += 1;
			else if (evento.mouseWheelScroll.delta > 0)
				desplazamientoLista -= 1;
			if (desplazamientoLista < 0) desplazamientoLista = 0;
			if (desplazamientoLista > maximo) desplazamientoLista = maximo;
		}
	}
	
	if (evento.type == Event::MouseButtonPressed && evento.mouseButton.button == Mouse::Left)
	{
		float x = evento.mouseButton.x;
		float y = evento.mouseButton.y;
		
		if (origenDesplegado)
		{
			if (x >= 340 && x <= 730 && y >= 242 && y < 402)
			{
				int fila = (int)((y - 242) / 40) + desplazamientoOrigen;
				vector<NodoVertice*> ciudades = obtenerCiudadesDisponibles(true);
				if (fila >= 0 && fila < (int)ciudades.size())
				{
					codigoOrigen = ciudades[fila]->ciudad.getCodigo();
					if (codigoOrigen == codigoDestino)
						codigoDestino = "";
					mensaje = "Ciudad de origen seleccionada.";
				}
				origenDesplegado = false;
				filtroOrigen = "";
				return;
			}
			origenDesplegado = false;
			filtroOrigen = "";
		}
		
		if (destinoDesplegado)
		{
			if (x >= 340 && x <= 730 && y >= 302 && y < 462)
			{
				int fila = (int)((y - 302) / 40) + desplazamientoDestino;
				vector<NodoVertice*> ciudades = obtenerCiudadesDisponibles(false);
				if (fila >= 0 && fila < (int)ciudades.size())
				{
					codigoDestino = ciudades[fila]->ciudad.getCodigo();
					mensaje = "Ciudad de destino seleccionada.";
				}
				destinoDesplegado = false;
				filtroDestino = "";
				return;
			}
			destinoDesplegado = false;
			filtroDestino = "";
		}
		
		if (x >= 340 && x <= 730)
		{
			if (y >= 150 && y <= 182)
			{
				campoActivo = 0;
				origenDesplegado = false;
				destinoDesplegado = false;
			}
			else if (y >= 210 && y <= 242)
			{
				campoActivo = 1;
				origenDesplegado = true;
				destinoDesplegado = false;
				filtroOrigen = "";
				desplazamientoOrigen = 0;
				return;
			}
			else if (y >= 270 && y <= 302)
			{
				campoActivo = 2;
				destinoDesplegado = true;
				origenDesplegado = false;
				filtroDestino = "";
				desplazamientoDestino = 0;
				return;
			}
			else if (y >= 330 && y <= 362)
			{
				campoActivo = 3;
				origenDesplegado = false;
				destinoDesplegado = false;
			}
			else if (y >= 390 && y <= 422)
			{
				campoActivo = 4;
				origenDesplegado = false;
				destinoDesplegado = false;
			}
		}
		
		if (x >= 340 && x <= 465 && y >= 450 && y <= 495)
			registrarVuelo();
		else if (x >= 475 && x <= 600 && y >= 450 && y <= 495)
			modificarVuelo();
		else if (x >= 610 && x <= 735 && y >= 450 && y <= 495)
			eliminarVuelo();
		else if (x >= 780 && x <= (int)ventana.getSize().x - 30 && y >= 190 && y < 550)
		{
			float relativo = y - 190;
			int indice = (int)(relativo / 40);
			float dentroFila = relativo - indice * 40;
			int fila = desplazamientoLista + indice;
			NodoVuelo* actual = listaVuelos->getPrimero();
			while (actual != nullptr && fila > 0)
			{
				actual = actual->siguiente;
				fila--;
			}
			if (actual != nullptr && dentroFila < 35)
				seleccionarVuelo(actual->vuelo.getCodigo());
		}
	}
	
	if (evento.type == Event::KeyPressed && evento.key.code == Keyboard::Tab)
	{
		campoActivo++;
		if (campoActivo > 4)
			campoActivo = 0;
		origenDesplegado = false;
		destinoDesplegado = false;
	}
	
	if (evento.type == Event::TextEntered)
	{
		if (origenDesplegado || destinoDesplegado)
		{
			string& filtro = origenDesplegado ? filtroOrigen : filtroDestino;
			int& desplazamiento = origenDesplegado ? desplazamientoOrigen : desplazamientoDestino;
			if (evento.text.unicode == 8)
			{
				if (!filtro.empty())
					filtro.pop_back();
			}
			else if (evento.text.unicode >= 32 && evento.text.unicode <= 255 && filtro.length() < 35)
			{
				char caracter = static_cast<char>(evento.text.unicode);
				if ((caracter >= 'A' && caracter <= 'Z') || (caracter >= 'a' && caracter <= 'z') ||
					(caracter >= '0' && caracter <= '9') || caracter == ' ' || caracter == '-')
					filtro.push_back(caracter);
			}
			desplazamiento = 0;
			return;
		}
		
		string* campo = nullptr;
		if (campoActivo == 0) campo = &codigo;
		else if (campoActivo == 3) campo = &costo;
		else if (campoActivo == 4) campo = &duracion;
		if (campoActivo == 0 && editando) campo = nullptr;   
		
		if (campo != nullptr)
		{
			if (evento.text.unicode == 8)
			{
				if (!campo->empty())
					campo->pop_back();
			}
			else if (evento.text.unicode >= 32 && evento.text.unicode < 127 && campo->length() < 25)
			{
				char caracter = (char)evento.text.unicode;
				if (campoActivo == 0)
					campo->push_back(caracter);
				else if ((caracter >= '0' && caracter <= '9') || (caracter == '.' && campo->find('.') == string::npos))
					campo->push_back(caracter);
			}
		}
	}
	
	if (evento.type == Event::KeyPressed && evento.key.code == Keyboard::Escape)
	{
		if (origenDesplegado || destinoDesplegado)
		{
			origenDesplegado = false;
			destinoDesplegado = false;
		}
		else
			limpiarFormulario();
	}
}

void PantallaVuelos::actualizar(float tiempo)
{
	tiempoCursor += tiempo;
	if (tiempoCursor >= 0.5f)
	{
		cursorVisible = !cursorVisible;
		tiempoCursor = 0;
	}
}

void PantallaVuelos::dibujar(RenderWindow& ventana)
{
	ventana.setView(ventana.getDefaultView());
	float ancho = ventana.getSize().x;
	float alto = ventana.getSize().y;
	RectangleShape fondo(Vector2f(ancho - 300, alto));
	fondo.setPosition(300, 0);
	fondo.setFillColor(Color::White);
	ventana.draw(fondo);
	
	dibujarTexto(ventana, 335, 35, "GESTION DE VUELOS", 28, Color(55, 70, 60));
	dibujarTexto(ventana, 335, 85, "REGISTRO DE VUELOS", 20, Color(120, 125, 120));
	dibujarFormulario(ventana);
	dibujarListado(ventana);
	
	if (origenDesplegado)
		dibujarListaCiudades(ventana, true);
	if (destinoDesplegado)
		dibujarListaCiudades(ventana, false);
	
	if (!mensaje.empty())
		dibujarTexto(ventana, 340, 520, mensaje, 16, Color(55, 70, 60));
	if (confirmarEliminar)
		dibujarConfirmacion(ventana);
}

void PantallaVuelos::dibujarFormulario(RenderWindow& ventana)
{
	RectangleShape panel(Vector2f(430, 440));
	panel.setPosition(325, 115);
	panel.setFillColor(Color(248, 249, 247));
	panel.setOutlineThickness(1);
	panel.setOutlineColor(Color(215, 220, 215));
	ventana.draw(panel);
	
	for (int i = 0; i < 5; i++)
	{
		RectangleShape campo(Vector2f(390, 32));
		campo.setPosition(340, 150 + i * 60);
		campo.setFillColor(i == 0 && editando ? Color(230, 235, 230) : Color::White);
		campo.setOutlineThickness(campoActivo == i ? 2 : 1);
		campo.setOutlineColor(campoActivo == i ? Color(90, 120, 95) : Color(210, 215, 210));
		ventana.draw(campo);
	}
	
	dibujarTexto(ventana, 345, 125, "Codigo del vuelo:", 16, Color(75, 80, 75));
	dibujarTexto(ventana, 345, 155, codigo, 16, Color::Black);
	
	NodoVertice* origen = grafo->buscarCiudad(codigoOrigen);
	NodoVertice* destino = grafo->buscarCiudad(codigoDestino);
	string nombreOrigen = origen == nullptr ? "Seleccione una ciudad" : origen->ciudad.getNombre();
	string nombreDestino = destino == nullptr ? "Seleccione una ciudad" : destino->ciudad.getNombre();
	
	dibujarTexto(ventana, 345, 185, "Ciudad de origen:", 16, Color(75, 80, 75));
	string textoOrigen = origenDesplegado ? (filtroOrigen.empty() ? "Escriba para filtrar..." : "Buscar: " + filtroOrigen) : nombreOrigen;
	dibujarTexto(ventana, 345, 215, textoOrigen, 16, origenDesplegado || origen == nullptr ? Color(120, 125, 120) : Color::Black);
	dibujarTexto(ventana, 345, 245, "Ciudad de destino:", 16, Color(75, 80, 75));
	string textoDestino = destinoDesplegado ? (filtroDestino.empty() ? "Escriba para filtrar..." : "Buscar: " + filtroDestino) : nombreDestino;
	dibujarTexto(ventana, 345, 275, textoDestino, 16, destinoDesplegado || destino == nullptr ? Color(120, 125, 120) : Color::Black);
	dibujarTexto(ventana, 345, 305, "Costo:", 16, Color(75, 80, 75));
	dibujarTexto(ventana, 345, 335, costo, 16, Color::Black);
	dibujarTexto(ventana, 345, 365, "Duracion (horas):", 16, Color(75, 80, 75));
	dibujarTexto(ventana, 345, 395, duracion, 16, Color::Black);
	
	dibujarTexto(ventana, 700, 215, "v", 16, Color(55, 70, 60));
	dibujarTexto(ventana, 700, 275, "v", 16, Color(55, 70, 60));
	
	if (cursorVisible && fuenteCargada && campoActivo != 1 && campoActivo != 2 && !(campoActivo == 0 && editando))
	{
		string contenido;
		if (campoActivo == 0) contenido = codigo;
		else if (campoActivo == 3) contenido = costo;
		else contenido = duracion;
		Text textoCursor;
		textoCursor.setFont(fuente);
		textoCursor.setString(contenido);
		textoCursor.setCharacterSize(16);
		RectangleShape cursor(Vector2f(2, 20));
		cursor.setPosition(345 + textoCursor.getLocalBounds().left + textoCursor.getLocalBounds().width, 157 + campoActivo * 60);
		cursor.setFillColor(Color::Black);
		ventana.draw(cursor);
	}
	
	Color verde(90, 120, 95);
	RectangleShape botonRegistrar(Vector2f(125, 45));
	botonRegistrar.setPosition(340, 450);
	botonRegistrar.setFillColor(verde);
	ventana.draw(botonRegistrar);
	dibujarTexto(ventana, 350, 462, "REGISTRAR", 15, Color::White);
	
	RectangleShape botonModificar(Vector2f(125, 45));
	botonModificar.setPosition(475, 450);
	botonModificar.setFillColor(verde);
	ventana.draw(botonModificar);
	dibujarTexto(ventana, 485, 462, "MODIFICAR", 15, Color::White);
	
	RectangleShape botonEliminar(Vector2f(125, 45));
	botonEliminar.setPosition(610, 450);
	botonEliminar.setFillColor(Color(165, 85, 80));
	ventana.draw(botonEliminar);
	dibujarTexto(ventana, 622, 462, "ELIMINAR", 15, Color::White);
}

vector<NodoVertice*> PantallaVuelos::obtenerCiudadesDisponibles(bool origen)
{
	vector<NodoVertice*> ciudades;
	string filtro = origen ? filtroOrigen : filtroDestino;
	string filtroMinuscula = filtro;
	for (size_t i = 0; i < filtroMinuscula.size(); i++)
		filtroMinuscula[i] = (char)tolower((unsigned char)filtroMinuscula[i]);
	
	NodoVertice* actual = grafo->getPrimero();
	while (actual != nullptr)
	{
		bool permitida = origen || actual->ciudad.getCodigo() != codigoOrigen;
		if (permitida)
		{
			string nombre = actual->ciudad.getNombre();
			string codigoCiudad = actual->ciudad.getCodigo();
			for (size_t i = 0; i < nombre.size(); i++)
				nombre[i] = (char)tolower((unsigned char)nombre[i]);
			for (size_t i = 0; i < codigoCiudad.size(); i++)
				codigoCiudad[i] = (char)tolower((unsigned char)codigoCiudad[i]);
			if (filtroMinuscula.empty() || nombre.find(filtroMinuscula) != string::npos ||
				codigoCiudad.find(filtroMinuscula) != string::npos)
				ciudades.push_back(actual);
		}
		actual = actual->siguiente;
	}
	return ciudades;
}

void PantallaVuelos::dibujarListaCiudades(RenderWindow& ventana, bool origen)
{
	float yInicial = origen ? 242.0f : 302.0f;
	int desplazamiento = origen ? desplazamientoOrigen : desplazamientoDestino;
	vector<NodoVertice*> ciudades = obtenerCiudadesDisponibles(origen);
	RectangleShape fondo(Vector2f(390, 160));
	fondo.setPosition(340, yInicial);
	fondo.setFillColor(Color::White);
	fondo.setOutlineThickness(1);
	fondo.setOutlineColor(Color(90, 120, 95));
	ventana.draw(fondo);
	
	int fila = 0;
	for (int i = desplazamiento; i < (int)ciudades.size() && fila < 4; i++)
	{
		float y = yInicial + fila * 40;
		RectangleShape opcion(Vector2f(388, 39));
		opcion.setPosition(341, y);
		opcion.setFillColor(Color(242, 245, 241));
		ventana.draw(opcion);
		dibujarTexto(ventana, 350, y + 9, ciudades[i]->ciudad.getNombre(), 14, Color(55, 70, 60));
		fila++;
	}
	
	if (ciudades.empty())
		dibujarTexto(ventana, 350, yInicial + 12, "No hay coincidencias.", 13, Color(120, 125, 120));
	else if (ciudades.size() > 4)
		dibujarTexto(ventana, 610, yInicial + 145, "Use la rueda para ver mas", 11, Color(120, 125, 120));
}

void PantallaVuelos::dibujarListado(RenderWindow& ventana)
{
	float ancho = ventana.getSize().x;
	RectangleShape panel(Vector2f(ancho - 785, 440));
	panel.setPosition(770, 115);
	panel.setFillColor(Color(248, 249, 247));
	panel.setOutlineThickness(1);
	panel.setOutlineColor(Color(215, 220, 215));
	ventana.draw(panel);
	
	RectangleShape encabezado(Vector2f(ancho - 785, 40));
	encabezado.setPosition(770, 115);
	encabezado.setFillColor(Color(90, 120, 95));
	ventana.draw(encabezado);
	
	RectangleShape encabezadoColumnas(Vector2f(ancho - 810, 28));
	encabezadoColumnas.setPosition(780, 160);
	encabezadoColumnas.setFillColor(Color(90, 120, 95));
	ventana.draw(encabezadoColumnas);
	
	dibujarTexto(ventana, 790, 125, "VUELOS REGISTRADOS", 20, Color::White);
	dibujarTexto(ventana, 790, 165, "CODIGO", 12, Color::White);
	dibujarTexto(ventana, 900, 165, "ORIGEN", 12, Color::White);
	dibujarTexto(ventana, 1030, 165, "DESTINO", 12, Color::White);
	dibujarTexto(ventana, 1150, 165, "COSTO", 12, Color::White);
	dibujarTexto(ventana, 1270, 165, "DURACION (h)", 12, Color::White);
	
	int totalVuelos = listaVuelos->getCantidad();
	int maximoDesplazamiento = totalVuelos - FILAS_VUELOS;
	if (maximoDesplazamiento < 0)
		maximoDesplazamiento = 0;
	if (desplazamientoLista > maximoDesplazamiento)
		desplazamientoLista = maximoDesplazamiento;
	if (desplazamientoLista < 0)
		desplazamientoLista = 0;
	
	NodoVuelo* actual = listaVuelos->getPrimero();
	for (int i = 0; actual != nullptr && i < desplazamientoLista; i++)
		actual = actual->siguiente;
	
	int fila = 0;
	while (actual != nullptr && fila < FILAS_VUELOS)
	{
		float y = 195 + fila * 40;
		if (actual->vuelo.getCodigo() == codigoSeleccionado)
		{
			RectangleShape seleccionado(Vector2f(ancho - 810, 35));
			seleccionado.setPosition(780, y - 5);
			seleccionado.setFillColor(Color(225, 237, 225));
			ventana.draw(seleccionado);
		}
		
		NodoVertice* origen = grafo->buscarCiudad(actual->vuelo.getCodigoOrigen());
		NodoVertice* destino = grafo->buscarCiudad(actual->vuelo.getCodigoDestino());
		string nombreOrigen = origen == nullptr ? actual->vuelo.getCodigoOrigen() : origen->ciudad.getNombre();
		string nombreDestino = destino == nullptr ? actual->vuelo.getCodigoDestino() : destino->ciudad.getNombre();
		
		stringstream costoFormateado;
		costoFormateado << fixed << setprecision(0) << actual->vuelo.getCosto();
		stringstream duracionFormateada;
		duracionFormateada << fixed << setprecision(1) << actual->vuelo.getDuracion();
		
		dibujarTexto(ventana, 790, y, recortarTexto(fuente, actual->vuelo.getCodigo(), 12, 100), 12, Color::Black);
		dibujarTexto(ventana, 900, y, recortarTexto(fuente, nombreOrigen, 12, 120), 12, Color::Black);
		dibujarTexto(ventana, 1030, y, recortarTexto(fuente, nombreDestino, 12, 110), 12, Color::Black);
		dibujarTexto(ventana, 1150, y, recortarTexto(fuente, costoFormateado.str(), 12, 110), 12, Color::Black);
		dibujarTexto(ventana, 1270, y, recortarTexto(fuente, duracionFormateada.str(), 12, 150), 12, Color::Black);
		
		actual = actual->siguiente;
		fila++;
	}
	
	const float altoPista = (FILAS_VUELOS - 1) * 40.0f + 35.0f;
	RectangleShape pista(Vector2f(6, altoPista));
	pista.setPosition(ancho - 26, 190);
	pista.setFillColor(Color(225, 229, 224));
	ventana.draw(pista);
	
	float altoIndicador = altoPista;
	if (totalVuelos > FILAS_VUELOS)
	{
		altoIndicador = altoPista * FILAS_VUELOS / totalVuelos;
		if (altoIndicador < 35.0f)
			altoIndicador = 35.0f;
	}
	
	float recorrido = altoPista - altoIndicador;
	float posicionIndicador = 190.0f;
	if (maximoDesplazamiento > 0)
		posicionIndicador += recorrido * desplazamientoLista / maximoDesplazamiento;
	
	RectangleShape indicador(Vector2f(6, altoIndicador));
	indicador.setPosition(ancho - 26, posicionIndicador);
	indicador.setFillColor(Color(90, 120, 95));
	ventana.draw(indicador);
	
	dibujarTexto(ventana, 790, 565, "Total de vuelos: " + to_string(totalVuelos), 15, Color(75, 80, 75));
}

void PantallaVuelos::dibujarConfirmacion(RenderWindow& ventana)
{
	RectangleShape oscurecer(Vector2f(ventana.getSize().x, ventana.getSize().y));
	oscurecer.setFillColor(Color(0, 0, 0, 110));
	ventana.draw(oscurecer);
	RectangleShape cuadro(Vector2f(430, 190));
	cuadro.setPosition(535, 300);
	cuadro.setFillColor(Color::White);
	cuadro.setOutlineThickness(2);
	cuadro.setOutlineColor(Color(90, 120, 95));
	ventana.draw(cuadro);
	dibujarTexto(ventana, 585, 335, "CONFIRMAR ELIMINACION", 20, Color(55, 70, 60));
	dibujarTexto(ventana, 575, 375, "Esta segura de eliminar este vuelo?", 16, Color::Black);
	RectangleShape botonSi(Vector2f(100, 45));
	botonSi.setPosition(610, 420);
	botonSi.setFillColor(Color(165, 85, 80));
	ventana.draw(botonSi);
	dibujarTexto(ventana, 645, 432, "SI", 16, Color::White);
	RectangleShape botonNo(Vector2f(100, 45));
	botonNo.setPosition(735, 420);
	botonNo.setFillColor(Color(90, 120, 95));
	ventana.draw(botonNo);
	dibujarTexto(ventana, 770, 432, "NO", 16, Color::White);
}

void PantallaVuelos::registrarVuelo()
{
	float costoNumero = 0;
	float duracionNumero = 0;
	stringstream flujoCosto(costo);
	stringstream flujoDuracion(duracion);
	
	if (codigo.empty() || codigoOrigen.empty() || codigoDestino.empty() ||
		!(flujoCosto >> costoNumero) || !(flujoDuracion >> duracionNumero) ||
		costoNumero <= 0 || duracionNumero <= 0 || codigoOrigen == codigoDestino)
	{
		mensaje = "Complete los campos y verifique los datos.";
		return;
	}
	
	if (grafo == nullptr || !grafo->existeCiudad(codigoOrigen) || !grafo->existeCiudad(codigoDestino))
	{
		mensaje = "La ciudad de origen o de destino ya no existe.";
		return;
	}
	
	if (listaVuelos->buscar(codigo) != nullptr)
	{
		mensaje = "Ya existe un vuelo con ese codigo.";
		return;
	}
	
	if (existeVueloDirecto(listaVuelos, codigoOrigen, codigoDestino, ""))
	{
		mensaje = "Ya existe un vuelo directo entre esas dos ciudades.";
		return;
	}
	
	Vuelo nuevo(codigo, codigoOrigen, codigoDestino, costoNumero, duracionNumero);
	
	if (!listaVuelos->agregar(nuevo))
	{
		mensaje = "No se pudo registrar el vuelo.";
		return;
	}
	
	if (!grafo->agregarVuelo(nuevo))
	{
		listaVuelos->eliminar(codigo);
		mensaje = "No se pudo agregar el vuelo al grafo.";
		return;
	}
	
	mensaje = "Vuelo registrado correctamente.";
	limpiarFormulario();
}

void PantallaVuelos::seleccionarVuelo(string codigo)
{
	NodoVuelo* actual = listaVuelos->buscar(codigo);
	if (actual == nullptr)
		return;
	codigoSeleccionado = codigo;
	this->codigo = actual->vuelo.getCodigo();
	codigoOrigen = actual->vuelo.getCodigoOrigen();
	codigoDestino = actual->vuelo.getCodigoDestino();
	stringstream flujoCosto;
	flujoCosto << fixed << setprecision(2) << actual->vuelo.getCosto();
	costo = flujoCosto.str();
	stringstream flujoDuracion;
	flujoDuracion << fixed << setprecision(2) << actual->vuelo.getDuracion();
	duracion = flujoDuracion.str();
	editando = true;
	mensaje = "Vuelo seleccionado.";
}

void PantallaVuelos::modificarVuelo()
{
	if (!editando || codigoSeleccionado.empty())
	{
		mensaje = "Seleccione un vuelo del listado.";
		return;
	}
	float costoNumero = 0;
	float duracionNumero = 0;
	stringstream flujoCosto(costo);
	stringstream flujoDuracion(duracion);
	if (codigoOrigen.empty() || codigoDestino.empty() ||
		!(flujoCosto >> costoNumero) || !(flujoDuracion >> duracionNumero) ||
		costoNumero <= 0 || duracionNumero <= 0 || codigoOrigen == codigoDestino)
	{
		mensaje = "Complete los campos y verifique los datos.";
		return;
	}
	
	if (!grafo->existeCiudad(codigoOrigen) || !grafo->existeCiudad(codigoDestino))
	{
		mensaje = "La ciudad de origen o de destino ya no existe.";
		return;
	}
	
	if (existeVueloDirecto(listaVuelos, codigoOrigen, codigoDestino, codigoSeleccionado))
	{
		mensaje = "Ya existe otro vuelo directo entre esas dos ciudades.";
		return;
	}
	
	bool enGrafo;
	if (grafo->buscarVuelo(codigoSeleccionado) != nullptr)
		enGrafo = grafo->modificarVuelo(codigoSeleccionado, codigoOrigen, codigoDestino, costoNumero, duracionNumero);
	else
		enGrafo = grafo->agregarVuelo(Vuelo(codigoSeleccionado, codigoOrigen, codigoDestino, costoNumero, duracionNumero));
	
	if (!enGrafo)
	{
		mensaje = "No se pudo modificar el vuelo en el grafo.";
		return;
	}
	
	if (listaVuelos->modificar(codigoSeleccionado, codigoOrigen, codigoDestino, costoNumero, duracionNumero))
	{
		seleccionarVuelo(codigoSeleccionado);
		mensaje = "Vuelo modificado correctamente.";
	}
	else
							   mensaje = "No se pudo modificar el vuelo.";
}

void PantallaVuelos::eliminarVuelo()
{
	if (codigoSeleccionado.empty())
	{
		mensaje = "Seleccione un vuelo del listado.";
		return;
	}
	confirmarEliminar = true;
}

void PantallaVuelos::limpiarFormulario()
{
	codigo = "";
	codigoOrigen = "";
	codigoDestino = "";
	costo = "";
	duracion = "";
	codigoSeleccionado = "";
	campoActivo = 0;
	editando = false;
	origenDesplegado = false;
	destinoDesplegado = false;
	filtroOrigen = "";
	filtroDestino = "";
}

void PantallaVuelos::dibujarTexto(RenderWindow& ventana, float x, float y, string texto, int tamano, Color color)
{
	if (!fuenteCargada)
		return;
	Text textoDibujado;
	textoDibujado.setFont(fuente);
	textoDibujado.setString(texto);
	textoDibujado.setCharacterSize(tamano);
	textoDibujado.setFillColor(color);
	textoDibujado.setPosition(x, y);
	ventana.draw(textoDibujado);
}
