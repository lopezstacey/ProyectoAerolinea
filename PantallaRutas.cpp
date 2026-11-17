#include "PantallaRutas.h"

static const int FILAS_RUTAS = 7;
static const int FILAS_DESPLEGABLE = 5;

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

static void dibujarBarra(RenderWindow& ventana, float x, float y, float alto, float ancho, int total, int visibles, int desplazamiento)
{
	RectangleShape pista(Vector2f(ancho, alto));
	pista.setPosition(x, y);
	pista.setFillColor(Color(225, 229, 224));
	ventana.draw(pista);
	
	int maximo = total - visibles;
	if (maximo < 0)
		maximo = 0;
	
	float altoIndicador = alto;
	if (total > visibles)
	{
		altoIndicador = alto * visibles / total;
		if (altoIndicador < 25.0f)
			altoIndicador = 25.0f;
	}
	
	float posicion = y;
	if (maximo > 0)
		posicion += (alto - altoIndicador) * desplazamiento / maximo;
	
	RectangleShape indicador(Vector2f(ancho, altoIndicador));
	indicador.setPosition(x, posicion);
	indicador.setFillColor(Color(90, 120, 95));
	ventana.draw(indicador);
}

static bool existeAsignacion(ListaRutas* lista, const string& matricula, const string& vuelo, const string& ignorarCodigo)
{
	for (NodoRuta* actual = lista->getPrimero(); actual != nullptr; actual = actual->siguiente)
	{
		if (actual->ruta.getMatriculaAeronave() == matricula &&
			actual->ruta.getCodigoVuelo() == vuelo &&
			actual->ruta.getCodigo() != ignorarCodigo)
			return true;
	}
	return false;
}

PantallaGestionRutas::PantallaGestionRutas(ListaRutas* listaRutas, ListaAeronaves* listaAeronaves, ListaVuelos* listaVuelos, Grafo* grafo)
{
	this->listaRutas = listaRutas;
	this->listaAeronaves = listaAeronaves;
	this->listaVuelos = listaVuelos;
	this->grafo = grafo;
	
	fuenteCargada = fuente.loadFromFile("resources/times.ttf");
	
	codigo = "";
	matriculaSeleccionada = "";
	vueloSeleccionado = "";
	codigoSeleccionado = "";
	mensaje = "";
	
	campoActivo = 0;
	editando = false;
	confirmarEliminar = false;
	desplegarAeronaves = false;
	desplegarVuelos = false;
	cursorVisible = true;
	tiempoCursor = 0;
	desplazamientoLista = 0;
	desplazamientoAeronaves = 0;
	desplazamientoVuelos = 0;
}

void PantallaGestionRutas::procesarEventos(RenderWindow& ventana, Event evento)
{
	if (confirmarEliminar)
	{
		if (evento.type == Event::MouseButtonPressed && evento.mouseButton.button == Mouse::Left)
		{
			float x = evento.mouseButton.x;
			float y = evento.mouseButton.y;
			
			if (x >= 610 && x <= 710 && y >= 420 && y <= 465)
			{
				if (listaRutas->eliminar(codigoSeleccionado))
					mensaje = "Ruta eliminada correctamente.";
				else
					mensaje = "No se pudo eliminar la ruta.";
				
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
		float x = (float)evento.mouseWheelScroll.x;
		float y = (float)evento.mouseWheelScroll.y;
		
		int paso = 0;
		if (evento.mouseWheelScroll.delta < 0)
			paso = 1;
		else if (evento.mouseWheelScroll.delta > 0)
			paso = -1;
		
		if (desplegarAeronaves && x >= 340 && x <= 730 && y >= 244 && y < 384)
		{
			int maximo = listaAeronaves->getCantidad() - FILAS_DESPLEGABLE;
			if (maximo < 0) maximo = 0;
			desplazamientoAeronaves += paso;
			if (desplazamientoAeronaves < 0) desplazamientoAeronaves = 0;
			if (desplazamientoAeronaves > maximo) desplazamientoAeronaves = maximo;
		}
		else if (desplegarVuelos && x >= 340 && x <= 730 && y >= 304 && y < 444)
		{
			int maximo = listaVuelos->getCantidad() - FILAS_DESPLEGABLE;
			if (maximo < 0) maximo = 0;
			desplazamientoVuelos += paso;
			if (desplazamientoVuelos < 0) desplazamientoVuelos = 0;
			if (desplazamientoVuelos > maximo) desplazamientoVuelos = maximo;
		}
		else if (x >= 780 && x <= (int)ventana.getSize().x - 15 && y >= 190 && y < 470)
		{
			int maximo = listaRutas->getCantidad() - FILAS_RUTAS;
			if (maximo < 0) maximo = 0;
			desplazamientoLista += paso;
			if (desplazamientoLista < 0) desplazamientoLista = 0;
			if (desplazamientoLista > maximo) desplazamientoLista = maximo;
		}
		return;
	}
	
	if (evento.type == Event::MouseButtonPressed && evento.mouseButton.button == Mouse::Left)
	{
		float x = evento.mouseButton.x;
		float y = evento.mouseButton.y;
		
		if (desplegarAeronaves)
		{
			if (x >= 340 && x <= 730 && y >= 244 && y <= 384)
			{
				int fila = desplazamientoAeronaves + (int)((y - 244) / 28);
				NodoAeronave* actual = listaAeronaves->getPrimero();
				
				while (actual != nullptr && fila > 0)
				{
					actual = actual->siguiente;
					fila--;
				}
				
				if (actual != nullptr)
					matriculaSeleccionada = actual->aeronave.getMatricula();
				
				desplegarAeronaves = false;
				return;
			}
			
			desplegarAeronaves = false;
		}
		
		if (desplegarVuelos)
		{
			if (x >= 340 && x <= 730 && y >= 304 && y <= 444)
			{
				int fila = desplazamientoVuelos + (int)((y - 304) / 28);
				NodoVuelo* actual = listaVuelos->getPrimero();
				
				while (actual != nullptr && fila > 0)
				{
					actual = actual->siguiente;
					fila--;
				}
				
				if (actual != nullptr)
					vueloSeleccionado = actual->vuelo.getCodigo();
				
				desplegarVuelos = false;
				return;
			}
			
			desplegarVuelos = false;
		}
		
		if (x >= 340 && x <= 730 && y >= 150 && y <= 182)
		{
			campoActivo = 0;
		}
		else if (x >= 340 && x <= 730 && y >= 210 && y <= 242)
		{
			desplegarAeronaves = true;
			desplegarVuelos = false;
			desplazamientoAeronaves = 0;
			return;
		}
		else if (x >= 340 && x <= 730 && y >= 270 && y <= 302)
		{
			desplegarVuelos = true;
			desplegarAeronaves = false;
			desplazamientoVuelos = 0;
			return;
		}
		
		if (x >= 340 && x <= 465 && y >= 400 && y <= 445)
			registrarRuta();
		else if (x >= 475 && x <= 600 && y >= 400 && y <= 445)
			modificarRuta();
		else if (x >= 610 && x <= 735 && y >= 400 && y <= 445)
			eliminarRuta();
		else if (x >= 780 && x <= (int)ventana.getSize().x - 30 && y >= 190 && y < 470)
		{
			float relativo = y - 190;
			int indice = (int)(relativo / 40);
			float dentroFila = relativo - indice * 40;
			int fila = desplazamientoLista + indice;
			NodoRuta* actual = listaRutas->getPrimero();
			
			while (actual != nullptr && fila > 0)
			{
				actual = actual->siguiente;
				fila--;
			}
			
			if (actual != nullptr && dentroFila < 35)
				seleccionarRuta(actual->ruta.getCodigo());
		}
	}
	
	if (evento.type == Event::KeyPressed && evento.key.code == Keyboard::Tab)
		campoActivo = 0;
	
	if (evento.type == Event::TextEntered && campoActivo == 0)
	{
		if (evento.text.unicode == 8)
		{
			if (!codigo.empty())
				codigo.pop_back();
		}
		else if (evento.text.unicode >= 32 && evento.text.unicode < 127 && codigo.length() < 20)
		{
			codigo.push_back((char)evento.text.unicode);
		}
	}
	
	if (evento.type == Event::KeyPressed && evento.key.code == Keyboard::Escape)
		limpiarFormulario();
}

void PantallaGestionRutas::actualizar(float tiempo)
{
	tiempoCursor += tiempo;
	
	if (tiempoCursor >= 0.5f)
	{
		cursorVisible = !cursorVisible;
		tiempoCursor = 0;
	}
}

void PantallaGestionRutas::dibujar(RenderWindow& ventana)
{
	ventana.setView(ventana.getDefaultView());
	
	float ancho = ventana.getSize().x;
	float alto = ventana.getSize().y;
	
	RectangleShape fondo(Vector2f(ancho - 300, alto));
	fondo.setPosition(300, 0);
	fondo.setFillColor(Color::White);
	ventana.draw(fondo);
	
	dibujarTexto(ventana, 335, 35, "GESTION DE RUTAS", 28, Color(55, 70, 60));
	dibujarTexto(ventana, 335, 85, "ASIGNACION DE RUTAS A AERONAVES", 20, Color(120, 125, 120));
	
	dibujarFormulario(ventana);
	dibujarListado(ventana);
	
	if (!mensaje.empty())
		dibujarTexto(ventana, 340, 475, mensaje, 16, Color(55, 70, 60));
	
	dibujarDesplegables(ventana);
	
	if (confirmarEliminar)
		dibujarConfirmacion(ventana);
}

void PantallaGestionRutas::dibujarFormulario(RenderWindow& ventana)
{
	RectangleShape panel(Vector2f(430, 390));
	panel.setPosition(325, 115);
	panel.setFillColor(Color(248, 249, 247));
	panel.setOutlineThickness(1);
	panel.setOutlineColor(Color(215, 220, 215));
	ventana.draw(panel);
	
	for (int i = 0; i < 3; i++)
	{
		RectangleShape campo(Vector2f(390, 32));
		campo.setPosition(340, 150 + i * 60);
		campo.setFillColor(Color::White);
		campo.setOutlineThickness(campoActivo == i ? 2 : 1);
		campo.setOutlineColor(campoActivo == i ? Color(90, 120, 95) : Color(210, 215, 210));
		ventana.draw(campo);
	}
	
	dibujarTexto(ventana, 345, 125, "Codigo de ruta:", 16, Color(75, 80, 75));
	dibujarTexto(ventana, 345, 155, codigo, 16, Color::Black);
	
	dibujarTexto(ventana, 345, 185, "Seleccione aeronave:", 16, Color(75, 80, 75));
	dibujarTexto(ventana, 345, 215, matriculaSeleccionada.empty() ? "Seleccione una matricula" : matriculaSeleccionada, 16, Color::Black);
	
	dibujarTexto(ventana, 345, 245, "Seleccione vuelo:", 16, Color(75, 80, 75));
	dibujarTexto(ventana, 345, 275, vueloSeleccionado.empty() ? "Seleccione un vuelo" : recortarTexto(fuente, descripcionVuelo(vueloSeleccionado), 16, 370), 16, Color::Black);
	
	if (cursorVisible && campoActivo == 0 && fuenteCargada)
	{
		Text textoCursor;
		textoCursor.setFont(fuente);
		textoCursor.setString(codigo);
		textoCursor.setCharacterSize(16);
		
		RectangleShape cursor(Vector2f(2, 20));
		cursor.setPosition(345 + textoCursor.getLocalBounds().width, 157);
		cursor.setFillColor(Color::Black);
		ventana.draw(cursor);
	}
	
	Color verde(90, 120, 95);
	
	RectangleShape botonRegistrar(Vector2f(125, 45));
	botonRegistrar.setPosition(340, 400);
	botonRegistrar.setFillColor(verde);
	ventana.draw(botonRegistrar);
	dibujarTexto(ventana, 350, 412, "REGISTRAR", 15, Color::White);
	
	RectangleShape botonModificar(Vector2f(125, 45));
	botonModificar.setPosition(475, 400);
	botonModificar.setFillColor(verde);
	ventana.draw(botonModificar);
	dibujarTexto(ventana, 485, 412, "MODIFICAR", 15, Color::White);
	
	RectangleShape botonEliminar(Vector2f(125, 45));
	botonEliminar.setPosition(610, 400);
	botonEliminar.setFillColor(Color(165, 85, 80));
	ventana.draw(botonEliminar);
	dibujarTexto(ventana, 622, 412, "ELIMINAR", 15, Color::White);
}

void PantallaGestionRutas::dibujarListado(RenderWindow& ventana)
{
	float ancho = ventana.getSize().x;
	
	RectangleShape panel(Vector2f(ancho - 785, 390));
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
	
	dibujarTexto(ventana, 790, 125, "RUTAS REGISTRADAS", 20, Color::White);
	dibujarTexto(ventana, 790, 165, "CODIGO", 14, Color::White);
	dibujarTexto(ventana, 925, 165, "AERONAVE", 14, Color::White);
	dibujarTexto(ventana, 1080, 165, "VUELO", 14, Color::White);
	
	int totalRutas = listaRutas->getCantidad();
	int maximoDesplazamiento = totalRutas - FILAS_RUTAS;
	if (maximoDesplazamiento < 0)
		maximoDesplazamiento = 0;
	if (desplazamientoLista > maximoDesplazamiento)
		desplazamientoLista = maximoDesplazamiento;
	if (desplazamientoLista < 0)
		desplazamientoLista = 0;
	
	NodoRuta* actual = listaRutas->getPrimero();
	for (int i = 0; actual != nullptr && i < desplazamientoLista; i++)
		actual = actual->siguiente;
	
	int fila = 0;
	
	while (actual != nullptr && fila < FILAS_RUTAS)
	{
		float y = 195 + fila * 40;
		
		if (actual->ruta.getCodigo() == codigoSeleccionado)
		{
			RectangleShape seleccionado(Vector2f(ancho - 810, 35));
			seleccionado.setPosition(780, y - 5);
			seleccionado.setFillColor(Color(225, 237, 225));
			ventana.draw(seleccionado);
		}
		
		dibujarTexto(ventana, 790, y, recortarTexto(fuente, actual->ruta.getCodigo(), 13, 125), 13, Color::Black);
		dibujarTexto(ventana, 925, y, recortarTexto(fuente, actual->ruta.getMatriculaAeronave(), 13, 145), 13, Color::Black);
		dibujarTexto(ventana, 1080, y, recortarTexto(fuente, descripcionVuelo(actual->ruta.getCodigoVuelo()), 13, 300), 13, Color::Black);
		
		actual = actual->siguiente;
		fila++;
	}
	
	dibujarBarra(ventana, ancho - 26, 190, (FILAS_RUTAS - 1) * 40.0f + 35.0f, 6, totalRutas, FILAS_RUTAS, desplazamientoLista);
	
	dibujarTexto(ventana, 790, 475, "Total de rutas: " + to_string(totalRutas), 15, Color(75, 80, 75));
}

void PantallaGestionRutas::dibujarDesplegables(RenderWindow& ventana)
{
	if (desplegarAeronaves)
	{
		RectangleShape panel(Vector2f(390, 140));
		panel.setPosition(340, 244);
		panel.setFillColor(Color::White);
		panel.setOutlineThickness(1);
		panel.setOutlineColor(Color(90, 120, 95));
		ventana.draw(panel);
		
		int total = listaAeronaves->getCantidad();
		int maximo = total - FILAS_DESPLEGABLE;
		if (maximo < 0)
			maximo = 0;
		if (desplazamientoAeronaves > maximo)
			desplazamientoAeronaves = maximo;
		
		NodoAeronave* actual = listaAeronaves->getPrimero();
		for (int i = 0; actual != nullptr && i < desplazamientoAeronaves; i++)
			actual = actual->siguiente;
		
		int fila = 0;
		
		while (actual != nullptr && fila < FILAS_DESPLEGABLE)
		{
			dibujarTexto(ventana, 350, 248 + fila * 28,
						 recortarTexto(fuente, actual->aeronave.getMatricula() + " - " + actual->aeronave.getModelo(), 13, 360),
						 13, Color::Black);
			
			actual = actual->siguiente;
			fila++;
		}
		
		if (total == 0)
			dibujarTexto(ventana, 350, 250, "No hay aeronaves registradas.", 13, Color(120, 125, 120));
		else if (total > FILAS_DESPLEGABLE)
			dibujarBarra(ventana, 723, 246, 136, 4, total, FILAS_DESPLEGABLE, desplazamientoAeronaves);
	}
	
	if (desplegarVuelos)
	{
		RectangleShape panel(Vector2f(390, 140));
		panel.setPosition(340, 304);
		panel.setFillColor(Color::White);
		panel.setOutlineThickness(1);
		panel.setOutlineColor(Color(90, 120, 95));
		ventana.draw(panel);
		
		int total = listaVuelos->getCantidad();
		int maximo = total - FILAS_DESPLEGABLE;
		if (maximo < 0)
			maximo = 0;
		if (desplazamientoVuelos > maximo)
			desplazamientoVuelos = maximo;
		
		NodoVuelo* actual = listaVuelos->getPrimero();
		for (int i = 0; actual != nullptr && i < desplazamientoVuelos; i++)
			actual = actual->siguiente;
		
		int fila = 0;
		
		while (actual != nullptr && fila < FILAS_DESPLEGABLE)
		{
			dibujarTexto(ventana, 350, 308 + fila * 28, recortarTexto(fuente, nombreCiudad(actual->vuelo.getCodigoOrigen()) + " a " + nombreCiudad(actual->vuelo.getCodigoDestino()), 13, 360), 13, Color::Black);
			
			actual = actual->siguiente;
			fila++;
		}
		
		if (total == 0)
			dibujarTexto(ventana, 350, 310, "No hay vuelos registrados.", 13, Color(120, 125, 120));
		else if (total > FILAS_DESPLEGABLE)
			dibujarBarra(ventana, 723, 306, 136, 4, total, FILAS_DESPLEGABLE, desplazamientoVuelos);
	}
}

string PantallaGestionRutas::nombreCiudad(const string& codigo)
{
	if (grafo != nullptr)
	{
		NodoVertice* c = grafo->buscarCiudad(codigo);
		if (c != nullptr)
			return c->ciudad.getNombre();
	}
	return codigo;
}

string PantallaGestionRutas::descripcionVuelo(const string& codigoVuelo)
{
	NodoVuelo* v = listaVuelos->buscar(codigoVuelo);
	if (v == nullptr)
		return codigoVuelo;
	return nombreCiudad(v->vuelo.getCodigoOrigen()) + " a " + nombreCiudad(v->vuelo.getCodigoDestino());
}

void PantallaGestionRutas::registrarRuta()
{
	if (codigo.empty() || matriculaSeleccionada.empty() || vueloSeleccionado.empty())
	{
		mensaje = "Complete todos los campos.";
		return;
	}
	
	if (listaAeronaves->buscar(matriculaSeleccionada) == nullptr ||
		listaVuelos->buscar(vueloSeleccionado) == nullptr)
	{
		mensaje = "Seleccione una aeronave y un vuelo validos.";
		return;
	}
	
	if (existeAsignacion(listaRutas, matriculaSeleccionada, vueloSeleccionado, ""))
	{
		mensaje = "Esa aeronave ya esta asignada a ese vuelo.";
		return;
	}
	
	Ruta nueva(codigo, matriculaSeleccionada, vueloSeleccionado);
	
	if (listaRutas->agregar(nueva))
	{
		mensaje = "Ruta registrada correctamente.";
		limpiarFormulario();
	}
	else
	{
		mensaje = "Ya existe una ruta con ese codigo.";
	}
}

void PantallaGestionRutas::seleccionarRuta(string codigo)
{
	NodoRuta* actual = listaRutas->buscar(codigo);
	
	if (actual == nullptr)
		return;
	
	codigoSeleccionado = codigo;
	this->codigo = actual->ruta.getCodigo();
	matriculaSeleccionada = actual->ruta.getMatriculaAeronave();
	vueloSeleccionado = actual->ruta.getCodigoVuelo();
	
	editando = true;
	campoActivo = 0;
	mensaje = "Ruta seleccionada.";
}

void PantallaGestionRutas::modificarRuta()
{
	if (!editando || codigoSeleccionado.empty())
	{
		mensaje = "Seleccione una ruta del listado.";
		return;
	}
	
	if (codigo.empty() || matriculaSeleccionada.empty() || vueloSeleccionado.empty())
	{
		mensaje = "Complete todos los campos.";
		return;
	}
	
	if (listaAeronaves->buscar(matriculaSeleccionada) == nullptr ||
		listaVuelos->buscar(vueloSeleccionado) == nullptr)
	{
		mensaje = "Seleccione una aeronave y un vuelo validos.";
		return;
	}
	
	if (codigo != codigoSeleccionado && listaRutas->buscar(codigo) != nullptr)
	{
		mensaje = "Ya existe una ruta con ese codigo.";
		return;
	}
	
	if (existeAsignacion(listaRutas, matriculaSeleccionada, vueloSeleccionado, codigoSeleccionado))
	{
		mensaje = "Esa aeronave ya esta asignada a ese vuelo.";
		return;
	}
	
	if (codigo != codigoSeleccionado)
	{
		listaRutas->eliminar(codigoSeleccionado);
		codigoSeleccionado = codigo;
		listaRutas->agregar(Ruta(codigo, matriculaSeleccionada, vueloSeleccionado));
	}
	else
	{
		listaRutas->modificar(codigoSeleccionado, matriculaSeleccionada, vueloSeleccionado);
	}
	
	mensaje = "Ruta modificada correctamente.";
	limpiarFormulario();
}

void PantallaGestionRutas::eliminarRuta()
{
	if (codigoSeleccionado.empty())
	{
		mensaje = "Seleccione una ruta del listado.";
		return;
	}
	
	confirmarEliminar = true;
}

void PantallaGestionRutas::limpiarFormulario()
{
	codigo = "";
	matriculaSeleccionada = "";
	vueloSeleccionado = "";
	codigoSeleccionado = "";
	
	campoActivo = 0;
	editando = false;
	desplegarAeronaves = false;
	desplegarVuelos = false;
}

void PantallaGestionRutas::dibujarConfirmacion(RenderWindow& ventana)
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
	dibujarTexto(ventana, 575, 375, "Esta seguro de eliminar esta ruta?", 16, Color::Black);
	
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

void PantallaGestionRutas::dibujarTexto(RenderWindow& ventana, float x, float y, string texto, int tamano, Color color)
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
