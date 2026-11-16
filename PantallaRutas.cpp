#include "PantallaRutas.h"

PantallaGestionRutas::PantallaGestionRutas(ListaRutas* listaRutas, ListaAeronaves* listaAeronaves, ListaVuelos* listaVuelos)
{
	this->listaRutas = listaRutas;
	this->listaAeronaves = listaAeronaves;
	this->listaVuelos = listaVuelos;
	
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
	
	if (evento.type == Event::MouseButtonPressed && evento.mouseButton.button == Mouse::Left)
	{
		float x = evento.mouseButton.x;
		float y = evento.mouseButton.y;
		
		if (desplegarAeronaves)
		{
			if (x >= 340 && x <= 730 && y >= 244 && y <= 384)
			{
				int fila = (int)((y - 244) / 28);
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
				int fila = (int)((y - 304) / 28);
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
			return;
		}
		else if (x >= 340 && x <= 730 && y >= 270 && y <= 302)
		{
			desplegarVuelos = true;
			desplegarAeronaves = false;
			return;
		}
		
		if (x >= 340 && x <= 465 && y >= 400 && y <= 445)
			registrarRuta();
		else if (x >= 475 && x <= 600 && y >= 400 && y <= 445)
			modificarRuta();
		else if (x >= 610 && x <= 735 && y >= 400 && y <= 445)
			eliminarRuta();
		else if (x >= 780 && x <= ventana.getSize().x - 20 && y >= 195 && y <= 495)
		{
			int fila = (int)((y - 195) / 40);
			NodoRuta* actual = listaRutas->getPrimero();
			
			while (actual != nullptr && fila > 0)
			{
				actual = actual->siguiente;
				fila--;
			}
			
			if (actual != nullptr)
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
	float ancho = ventana.getSize().x;
	float alto = ventana.getSize().y;
	
	RectangleShape fondo(Vector2f(ancho - 300, alto));
	fondo.setPosition(300, 0);
	fondo.setFillColor(Color::White);
	ventana.draw(fondo);
	
	dibujarTexto(ventana, 335, 35, "GESTION DE RUTAS", 28, Color(25, 65, 105));
	dibujarTexto(ventana, 335, 85, "ASIGNACION DE RUTAS A AERONAVES", 20, Color::Black);
	
	dibujarFormulario(ventana);
	dibujarListado(ventana);
	
	if (!mensaje.empty())
		dibujarTexto(ventana, 340, 475, mensaje, 16, Color(25, 65, 105));
	
	dibujarDesplegables(ventana);
	
	if (confirmarEliminar)
		dibujarConfirmacion(ventana);
}

void PantallaGestionRutas::dibujarFormulario(RenderWindow& ventana)
{
	RectangleShape panel(Vector2f(430, 390));
	panel.setPosition(325, 115);
	panel.setFillColor(Color(245, 247, 250));
	panel.setOutlineThickness(1);
	panel.setOutlineColor(Color(210, 215, 220));
	ventana.draw(panel);
	
	for (int i = 0; i < 3; i++)
	{
		RectangleShape campo(Vector2f(390, 32));
		campo.setPosition(340, 150 + i * 60);
		campo.setFillColor(Color::White);
		campo.setOutlineThickness(campoActivo == i ? 2 : 1);
		campo.setOutlineColor(campoActivo == i ? Color(30, 110, 180) : Color(180, 185, 190));
		ventana.draw(campo);
	}
	
	dibujarTexto(ventana, 345, 125, "Codigo de ruta:", 16, Color::Black);
	dibujarTexto(ventana, 345, 155, codigo, 16, Color::Black);
	
	dibujarTexto(ventana, 345, 185, "Seleccione aeronave:", 16, Color::Black);
	dibujarTexto(ventana, 345, 215, matriculaSeleccionada.empty() ? "Seleccione una matricula" : matriculaSeleccionada, 16, Color::Black);
	
	dibujarTexto(ventana, 345, 245, "Seleccione vuelo:", 16, Color::Black);
	dibujarTexto(ventana, 345, 275, vueloSeleccionado.empty() ? "Seleccione un vuelo" : vueloSeleccionado, 16, Color::Black);
	
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
	
	Color azul(28, 91, 145);
	
	RectangleShape botonRegistrar(Vector2f(125, 45));
	botonRegistrar.setPosition(340, 400);
	botonRegistrar.setFillColor(azul);
	ventana.draw(botonRegistrar);
	dibujarTexto(ventana, 350, 412, "REGISTRAR", 15, Color::White);
	
	RectangleShape botonModificar(Vector2f(125, 45));
	botonModificar.setPosition(475, 400);
	botonModificar.setFillColor(azul);
	ventana.draw(botonModificar);
	dibujarTexto(ventana, 485, 412, "MODIFICAR", 15, Color::White);
	
	RectangleShape botonEliminar(Vector2f(125, 45));
	botonEliminar.setPosition(610, 400);
	botonEliminar.setFillColor(Color(190, 65, 65));
	ventana.draw(botonEliminar);
	dibujarTexto(ventana, 622, 412, "ELIMINAR", 15, Color::White);
}

void PantallaGestionRutas::dibujarListado(RenderWindow& ventana)
{
	float ancho = ventana.getSize().x;
	
	RectangleShape panel(Vector2f(ancho - 785, 390));
	panel.setPosition(770, 115);
	panel.setFillColor(Color(245, 247, 250));
	panel.setOutlineThickness(1);
	panel.setOutlineColor(Color(210, 215, 220));
	ventana.draw(panel);
	
	dibujarTexto(ventana, 790, 125, "RUTAS REGISTRADAS", 20, Color(25, 65, 105));
	dibujarTexto(ventana, 790, 165, "CODIGO       AERONAVE       VUELO", 14, Color::Black);
	
	NodoRuta* actual = listaRutas->getPrimero();
	int fila = 0;
	
	while (actual != nullptr)
	{
		float y = 195 + fila * 40;
		
		if (actual->ruta.getCodigo() == codigoSeleccionado)
		{
			RectangleShape seleccionado(Vector2f(ancho - 810, 35));
			seleccionado.setPosition(780, y - 5);
			seleccionado.setFillColor(Color(215, 232, 245));
			ventana.draw(seleccionado);
		}
		
		dibujarTexto(ventana, 790, y, actual->ruta.getCodigo(), 13, Color::Black);
		dibujarTexto(ventana, 925, y, actual->ruta.getMatriculaAeronave(), 13, Color::Black);
		dibujarTexto(ventana, 1080, y, actual->ruta.getCodigoVuelo(), 13, Color::Black);
		
		actual = actual->siguiente;
		fila++;
	}
	
	dibujarTexto(ventana, 790, 465, "Total de rutas: " + to_string(listaRutas->getCantidad()), 15, Color::Black);
}

void PantallaGestionRutas::dibujarDesplegables(RenderWindow& ventana)
{
	if (desplegarAeronaves)
	{
		RectangleShape panel(Vector2f(390, 140));
		panel.setPosition(340, 244);
		panel.setFillColor(Color::White);
		panel.setOutlineThickness(1);
		panel.setOutlineColor(Color(30, 110, 180));
		ventana.draw(panel);
		
		NodoAeronave* actual = listaAeronaves->getPrimero();
		int fila = 0;
		
		while (actual != nullptr && fila < 5)
		{
			dibujarTexto(ventana, 350, 248 + fila * 28,
						 actual->aeronave.getMatricula() + " - " + actual->aeronave.getModelo(),
						 13, Color::Black);
			
			actual = actual->siguiente;
			fila++;
		}
		
		if (fila == 0)
			dibujarTexto(ventana, 350, 250, "No hay aeronaves registradas.", 13, Color::Black);
	}
	
	if (desplegarVuelos)
	{
		RectangleShape panel(Vector2f(390, 140));
		panel.setPosition(340, 304);
		panel.setFillColor(Color::White);
		panel.setOutlineThickness(1);
		panel.setOutlineColor(Color(30, 110, 180));
		ventana.draw(panel);
		
		NodoVuelo* actual = listaVuelos->getPrimero();
		int fila = 0;
		
		while (actual != nullptr && fila < 5)
		{
			dibujarTexto(ventana, 350, 308 + fila * 28,
						 actual->vuelo.getCodigo() + " - " +
						 actual->vuelo.getCodigoOrigen() + " a " +
						 actual->vuelo.getCodigoDestino(),
						 13, Color::Black);
			
			actual = actual->siguiente;
			fila++;
		}
		
		if (fila == 0)
			dibujarTexto(ventana, 350, 310, "No hay vuelos registrados.", 13, Color::Black);
	}
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
	cuadro.setOutlineColor(Color(28, 91, 145));
	ventana.draw(cuadro);
	
	dibujarTexto(ventana, 585, 335, "CONFIRMAR ELIMINACION", 20, Color(25, 65, 105));
	dibujarTexto(ventana, 575, 375, "Esta seguro de eliminar esta ruta?", 16, Color::Black);
	
	RectangleShape botonSi(Vector2f(100, 45));
	botonSi.setPosition(610, 420);
	botonSi.setFillColor(Color(190, 65, 65));
	ventana.draw(botonSi);
	dibujarTexto(ventana, 645, 432, "SI", 16, Color::White);
	
	RectangleShape botonNo(Vector2f(100, 45));
	botonNo.setPosition(735, 420);
	botonNo.setFillColor(Color(28, 91, 145));
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
