#include "PantallaTripulantes.h"

PantallaTripulantes::PantallaTripulantes(ListaTripulantes* listaTripulantes)
{
	this->listaTripulantes = listaTripulantes;
	
	fuenteCargada = fuente.loadFromFile("resources/times.ttf");
	
	identificacion = "";
	nombre = "";
	puesto = "";
	estado = "Disponible";
	identificacionSeleccionada = "";
	mensaje = "";
	
	campoActivo = 0;
	editando = false;
	confirmarEliminar = false;
	cursorVisible = true;
	tiempoCursor = 0;
}

void PantallaTripulantes::procesarEventos(RenderWindow& ventana, Event evento)
{
	if (confirmarEliminar)
	{
		if (evento.type == Event::MouseButtonPressed && evento.mouseButton.button == Mouse::Left)
		{
			float x = evento.mouseButton.x;
			float y = evento.mouseButton.y;
			
			if (x >= 610 && x <= 710 && y >= 420 && y <= 465)
			{
				if (listaTripulantes->eliminar(identificacionSeleccionada))
					mensaje = "Tripulante eliminado correctamente.";
				else
					mensaje = "No se pudo eliminar el tripulante.";
				
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
		
		if (x >= 340 && x <= 730)
		{
			if (y >= 150 && y <= 182)
				campoActivo = 0;
			else if (y >= 210 && y <= 242)
				campoActivo = 1;
			else if (y >= 270 && y <= 302)
				campoActivo = 2;
			else if (y >= 330 && y <= 362)
				campoActivo = 3;
		}
		
		if (x >= 340 && x <= 465 && y >= 400 && y <= 445)
			registrarTripulante();
		else if (x >= 475 && x <= 600 && y >= 400 && y <= 445)
			modificarTripulante();
		else if (x >= 610 && x <= 735 && y >= 400 && y <= 445)
			eliminarTripulante();
		else if (x >= 780 && x <= ventana.getSize().x - 20 && y >= 195 && y <= 495)
		{
			int fila = (int)((y - 195) / 40);
			NodoTripulante* actual = listaTripulantes->getPrimero();
			
			while (actual != nullptr && fila > 0)
			{
				actual = actual->siguiente;
				fila--;
			}
			
			if (actual != nullptr)
				seleccionarTripulante(actual->tripulante.getIdentificacion());
		}
	}
	
	if (evento.type == Event::KeyPressed && evento.key.code == Keyboard::Tab)
	{
		campoActivo++;
		
		if (campoActivo > 3)
			campoActivo = 0;
	}
	
	if (evento.type == Event::TextEntered)
	{
		string* campo = nullptr;
		
		if (campoActivo == 0)
			campo = &identificacion;
		else if (campoActivo == 1)
			campo = &nombre;
		else if (campoActivo == 2)
			campo = &puesto;
		else
			campo = &estado;
		
		if (evento.text.unicode == 8)
		{
			if (!campo->empty())
				campo->pop_back();
		}
		else if (evento.text.unicode >= 32 && evento.text.unicode < 127 && campo->length() < 35)
		{
			campo->push_back((char)evento.text.unicode);
		}
	}
	
	if (evento.type == Event::KeyPressed && evento.key.code == Keyboard::Escape)
		limpiarFormulario();
}

void PantallaTripulantes::actualizar(float tiempo)
{
	tiempoCursor += tiempo;
	
	if (tiempoCursor >= 0.5f)
	{
		cursorVisible = !cursorVisible;
		tiempoCursor = 0;
	}
}

void PantallaTripulantes::dibujar(RenderWindow& ventana)
{
	float ancho = ventana.getSize().x;
	float alto = ventana.getSize().y;
	
	RectangleShape fondo(Vector2f(ancho - 300, alto));
	fondo.setPosition(300, 0);
	fondo.setFillColor(Color::White);
	ventana.draw(fondo);
	
	dibujarTexto(ventana, 335, 35, "GESTION DE TRIPULANTES", 28, Color(25, 65, 105));
	dibujarTexto(ventana, 335, 85, "REGISTRO DE TRIPULANTES", 20, Color::Black);
	
	dibujarFormulario(ventana);
	dibujarListado(ventana);
	
	if (!mensaje.empty())
		dibujarTexto(ventana, 340, 475, mensaje, 16, Color(25, 65, 105));
	
	if (confirmarEliminar)
		dibujarConfirmacion(ventana);
}

void PantallaTripulantes::dibujarFormulario(RenderWindow& ventana)
{
	RectangleShape panel(Vector2f(430, 390));
	panel.setPosition(325, 115);
	panel.setFillColor(Color(245, 247, 250));
	panel.setOutlineThickness(1);
	panel.setOutlineColor(Color(210, 215, 220));
	ventana.draw(panel);
	
	for (int i = 0; i < 4; i++)
	{
		RectangleShape campo(Vector2f(390, 32));
		campo.setPosition(340, 150 + i * 60);
		campo.setFillColor(Color::White);
		campo.setOutlineThickness(campoActivo == i ? 2 : 1);
		campo.setOutlineColor(campoActivo == i ? Color(30, 110, 180) : Color(180, 185, 190));
		ventana.draw(campo);
	}
	
	dibujarTexto(ventana, 345, 125, "Identificacion:", 16, Color::Black);
	dibujarTexto(ventana, 345, 155, identificacion, 16, Color::Black);
	dibujarTexto(ventana, 345, 185, "Nombre:", 16, Color::Black);
	dibujarTexto(ventana, 345, 215, nombre, 16, Color::Black);
	dibujarTexto(ventana, 345, 245, "Puesto:", 16, Color::Black);
	dibujarTexto(ventana, 345, 275, puesto, 16, Color::Black);
	dibujarTexto(ventana, 345, 305, "Estado:", 16, Color::Black);
	dibujarTexto(ventana, 345, 335, estado, 16, Color::Black);
	
	if (cursorVisible && fuenteCargada)
	{
		string contenido;
		
		if (campoActivo == 0)
			contenido = identificacion;
		else if (campoActivo == 1)
			contenido = nombre;
		else if (campoActivo == 2)
			contenido = puesto;
		else
			contenido = estado;
		
		Text textoCursor;
		textoCursor.setFont(fuente);
		textoCursor.setString(contenido);
		textoCursor.setCharacterSize(16);
		
		RectangleShape cursor(Vector2f(2, 20));
		cursor.setPosition(345 + textoCursor.getLocalBounds().width, 157 + campoActivo * 60);
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

void PantallaTripulantes::dibujarListado(RenderWindow& ventana)
{
	float ancho = ventana.getSize().x;
	
	RectangleShape panel(Vector2f(ancho - 785, 390));
	panel.setPosition(770, 115);
	panel.setFillColor(Color(245, 247, 250));
	panel.setOutlineThickness(1);
	panel.setOutlineColor(Color(210, 215, 220));
	ventana.draw(panel);
	
	dibujarTexto(ventana, 790, 125, "TRIPULANTES REGISTRADOS", 20, Color(25, 65, 105));
	dibujarTexto(ventana, 790, 165, "IDENTIFICACION       NOMBRE             PUESTO             ESTADO", 13, Color::Black);
	
	NodoTripulante* actual = listaTripulantes->getPrimero();
	int fila = 0;
	
	while (actual != nullptr)
	{
		float y = 195 + fila * 40;
		
		if (actual->tripulante.getIdentificacion() == identificacionSeleccionada)
		{
			RectangleShape seleccionado(Vector2f(ancho - 810, 35));
			seleccionado.setPosition(780, y - 5);
			seleccionado.setFillColor(Color(215, 232, 245));
			ventana.draw(seleccionado);
		}
		
		dibujarTexto(ventana, 790, y, actual->tripulante.getIdentificacion(), 13, Color::Black);
		dibujarTexto(ventana, 930, y, actual->tripulante.getNombre(), 13, Color::Black);
		dibujarTexto(ventana, 1100, y, actual->tripulante.getPuesto(), 13, Color::Black);
		dibujarTexto(ventana, 1270, y, actual->tripulante.getEstado(), 13, Color::Black);
		
		actual = actual->siguiente;
		fila++;
	}
	
	dibujarTexto(ventana, 790, 465, "Total de tripulantes: " + to_string(listaTripulantes->getCantidad()), 15, Color::Black);
}

void PantallaTripulantes::dibujarConfirmacion(RenderWindow& ventana)
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
	dibujarTexto(ventana, 575, 375, "Esta segura de eliminar este tripulante?", 16, Color::Black);
	
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

void PantallaTripulantes::registrarTripulante()
{
	if (identificacion.empty() || nombre.empty() || puesto.empty() || estado.empty())
	{
		mensaje = "Complete todos los campos correctamente.";
		return;
	}
	
	Tripulante nuevo(identificacion, nombre, puesto, estado);
	
	if (listaTripulantes->agregar(nuevo))
	{
		mensaje = "Tripulante registrado correctamente.";
		limpiarFormulario();
	}
	else
	{
		mensaje = "Ya existe un tripulante con esa identificacion.";
	}
}

void PantallaTripulantes::seleccionarTripulante(string identificacion)
{
	NodoTripulante* actual = listaTripulantes->buscar(identificacion);
	
	if (actual == nullptr)
		return;
	
	identificacionSeleccionada = identificacion;
	this->identificacion = actual->tripulante.getIdentificacion();
	nombre = actual->tripulante.getNombre();
	puesto = actual->tripulante.getPuesto();
	estado = actual->tripulante.getEstado();
	
	editando = true;
	mensaje = "Tripulante seleccionado.";
}

void PantallaTripulantes::modificarTripulante()
{
	if (!editando || identificacionSeleccionada.empty())
	{
		mensaje = "Seleccione un tripulante del listado.";
		return;
	}
	
	if (nombre.empty() || puesto.empty() || estado.empty())
	{
		mensaje = "Complete los campos correctamente.";
		return;
	}
	
	if (listaTripulantes->modificar(identificacionSeleccionada, nombre, puesto, estado))
	{
		seleccionarTripulante(identificacionSeleccionada);
		mensaje = "Tripulante modificado correctamente.";
	}
	else
	{
		mensaje = "No se pudo modificar el tripulante.";
	}
}

void PantallaTripulantes::eliminarTripulante()
{
	if (identificacionSeleccionada.empty())
	{
		mensaje = "Seleccione un tripulante del listado.";
		return;
	}
	
	confirmarEliminar = true;
}

void PantallaTripulantes::limpiarFormulario()
{
	identificacion = "";
	nombre = "";
	puesto = "";
	estado = "Disponible";
	identificacionSeleccionada = "";
	
	campoActivo = 0;
	editando = false;
}

void PantallaTripulantes::dibujarTexto(RenderWindow& ventana, float x, float y, string texto, int tamano, Color color)
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
