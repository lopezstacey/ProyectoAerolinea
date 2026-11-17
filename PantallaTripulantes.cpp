#include "PantallaTripulantes.h"

PantallaTripulantes::PantallaTripulantes(ListaTripulantes* listaTripulantes, ListaAeronaves* listaAeronaves, GestorAsignaciones* gestorAsignaciones)
{
	this->listaTripulantes = listaTripulantes;
	this->listaAeronaves = listaAeronaves;
	this->gestorAsignaciones = gestorAsignaciones;
	
	fuenteCargada = fuente.loadFromFile("resources/times.ttf");
	identificacion = "";
	nombre = "";
	puesto = "";
	estado = "Disponible";
	matriculaAeronave = "";
	identificacionSeleccionada = "";
	mensaje = "";
	campoActivo = 0;
	editando = false;
	confirmarEliminar = false;
	cursorVisible = true;
	tiempoCursor = 0;
	desplegableMatriculas = false;
	desplazamientoMatriculas = 0;
}

void PantallaTripulantes::procesarEventos(RenderWindow& ventana, Event evento)
{
	if (confirmarEliminar)
	{
		if (evento.type == Event::MouseButtonPressed && evento.mouseButton.button == Mouse::Left)
		{
			int x = evento.mouseButton.x;
			int y = evento.mouseButton.y;
			
			if (x >= 610 && x <= 710 && y >= 420 && y <= 465)
			{
				if (listaTripulantes->eliminar(identificacionSeleccionada))
				{
					gestorAsignaciones->quitarTripulante(identificacionSeleccionada);
					mensaje = "Tripulante eliminado correctamente.";
				}
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
	
	if (evento.type == Event::MouseWheelScrolled && desplegableMatriculas)
	{
		if (evento.mouseWheelScroll.x >= 340 && evento.mouseWheelScroll.x <= 725 &&
			evento.mouseWheelScroll.y >= 459 && evento.mouseWheelScroll.y < 543)
		{
			int maximo = listaAeronaves == nullptr ? 0 : listaAeronaves->getCantidad() - 3;
			if (maximo < 0) maximo = 0;
			desplazamientoMatriculas -= static_cast<int>(evento.mouseWheelScroll.delta);
			if (desplazamientoMatriculas < 0) desplazamientoMatriculas = 0;
			if (desplazamientoMatriculas > maximo) desplazamientoMatriculas = maximo;
		}
	}
	
	if (evento.type == Event::MouseButtonPressed && evento.mouseButton.button == Mouse::Left)
	{
		int x = evento.mouseButton.x;
		int y = evento.mouseButton.y;
		
		if (desplegableMatriculas && x >= 340 && x <= 735 && y >= 459 && y < 543)
		{
			int fila = (y - 459) / 28;
			int indice = desplazamientoMatriculas + fila;
			NodoAeronave* aeronave = listaAeronaves == nullptr ? nullptr : listaAeronaves->getPrimero();
			while (aeronave != nullptr && indice > 0)
			{
				aeronave = aeronave->siguiente;
				indice--;
			}
			
			if (aeronave != nullptr)
			{
				matriculaAeronave = aeronave->aeronave.getMatricula();
				mensaje = "Aeronave seleccionada: " + matriculaAeronave;
			}
			desplegableMatriculas = false;
			return;
		}
		
		if (desplegableMatriculas)
			desplegableMatriculas = false;
		
		if (x >= 340 && x <= 735)
		{
			if (y >= 178 && y <= 210) campoActivo = 0;
			else if (y >= 240 && y <= 272) campoActivo = 1;
			else if (y >= 302 && y <= 334) campoActivo = 2;
			else if (y >= 364 && y <= 396) campoActivo = 3;
			else if (y >= 426 && y <= 458)
			{
				desplegableMatriculas = true;
				campoActivo = 4;
				desplazamientoMatriculas = 0;
			}
		}
		
		if (x >= 340 && x <= 465 && y >= 555 && y <= 600)
		{
			if (editando)
			{
				limpiarFormulario();
				mensaje = "Formulario limpio. Puede registrar un nuevo tripulante.";
			}
			else
				registrarTripulante();
		}
		else if (x >= 475 && x <= 600 && y >= 555 && y <= 600)
			modificarTripulante();
		else if (x >= 610 && x <= 735 && y >= 555 && y <= 600)
			eliminarTripulante();
		else if (x >= 780 && x <= static_cast<int>(ventana.getSize().x) - 20 && y >= 195 && y <= 475)
		{
			int fila = (y - 195) / 40;
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
		desplegableMatriculas = false;
		campoActivo++;
		if (campoActivo > 3) campoActivo = 0;
	}
	
	if (evento.type == Event::KeyPressed && evento.key.code == Keyboard::Escape)
	{
		desplegableMatriculas = false;
		limpiarFormulario();
	}
	
	if (evento.type == Event::KeyPressed && evento.key.code == Keyboard::BackSpace)
	{
		if (campoActivo == 0 && !identificacion.empty()) identificacion.pop_back();
		else if (campoActivo == 1 && !nombre.empty()) nombre.pop_back();
		else if (campoActivo == 2 && !puesto.empty()) puesto.pop_back();
		else if (campoActivo == 3 && !estado.empty()) estado.pop_back();
	}
	
	if (evento.type == Event::TextEntered && evento.text.unicode >= 32 && evento.text.unicode < 127)
	{
		char c = static_cast<char>(evento.text.unicode);
		if (campoActivo == 0 && identificacion.size() < 25) identificacion += c;
		else if (campoActivo == 1 && nombre.size() < 35) nombre += c;
		else if (campoActivo == 2 && puesto.size() < 30) puesto += c;
		else if (campoActivo == 3 && estado.size() < 20) estado += c;
	}
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
	ventana.setView(ventana.getDefaultView());
	float ancho = static_cast<float>(ventana.getSize().x);
	float alto = static_cast<float>(ventana.getSize().y);
	
	RectangleShape fondo(Vector2f(ancho - 300, alto));
	fondo.setPosition(300, 0);
	fondo.setFillColor(Color(242, 244, 242));
	ventana.draw(fondo);
	
	dibujarTexto(ventana, 335, 35, "GESTION DE TRIPULANTES", 28, Color(55, 70, 60));
	dibujarTexto(ventana, 335, 72, "Registre y asigne el personal a una aeronave", 15, Color(120, 125, 120));
	
	dibujarFormulario(ventana);
	dibujarListado(ventana);
	
	if (!mensaje.empty())
		dibujarTexto(ventana, 340, 635, mensaje, 14, Color(90, 120, 95));
	
	if (confirmarEliminar)
		dibujarConfirmacion(ventana);
}

void PantallaTripulantes::dibujarFormulario(RenderWindow& ventana)
{
	RectangleShape panel(Vector2f(430, 505));
	panel.setPosition(325, 110);
	panel.setFillColor(Color(248, 249, 247));
	panel.setOutlineThickness(1);
	panel.setOutlineColor(Color(215, 220, 215));
	ventana.draw(panel);
	
	RectangleShape encabezado(Vector2f(430, 40));
	encabezado.setPosition(325, 110);
	encabezado.setFillColor(Color(90, 120, 95));
	ventana.draw(encabezado);
	dibujarTexto(ventana, 342, 119, "DATOS DEL TRIPULANTE", 17, Color::White);
	
	const string etiquetas[4] = {"Identificacion", "Nombre completo", "Puesto", "Estado"};
	const string* valores[4] = {&identificacion, &nombre, &puesto, &estado};
	
	for (int i = 0; i < 4; i++)
	{
		float y = 160 + i * 62;
		dibujarTexto(ventana, 342, y, etiquetas[i], 13, Color(75, 80, 75));
		
		RectangleShape campo(Vector2f(395, 32));
		campo.setPosition(340, y + 18);
		campo.setFillColor(Color(250, 251, 249));
		campo.setOutlineThickness(campoActivo == i ? 2 : 1);
		campo.setOutlineColor(campoActivo == i ? Color(115, 145, 120) : Color(210, 215, 210));
		ventana.draw(campo);
		
		dibujarTexto(ventana, 350, y + 23, *valores[i], 14, Color(45, 50, 45));
		
		if (cursorVisible && campoActivo == i && fuenteCargada)
		{
			Text t;
			t.setFont(fuente);
			t.setString(*valores[i]);
			t.setCharacterSize(14);
			RectangleShape cursor(Vector2f(1.5f, 18));
			cursor.setPosition(350 + t.getLocalBounds().width, y + 25);
			cursor.setFillColor(Color(70, 90, 70));
			ventana.draw(cursor);
		}
	}
	
	dibujarTexto(ventana, 342, 408, "Matricula de aeronave", 13, Color(75, 80, 75));
	
	RectangleShape combo(Vector2f(395, 32));
	combo.setPosition(340, 426);
	combo.setFillColor(Color(250, 251, 249));
	combo.setOutlineThickness(desplegableMatriculas || campoActivo == 4 ? 2 : 1);
	combo.setOutlineColor(desplegableMatriculas || campoActivo == 4 ? Color(115, 145, 120) : Color(210, 215, 210));
	ventana.draw(combo);
	
	dibujarTexto(ventana, 350, 432, matriculaAeronave.empty() ? "Seleccione una aeronave..." : matriculaAeronave, 14, matriculaAeronave.empty() ? Color(140, 145, 140) : Color(45, 50, 45));
	dibujarTexto(ventana, 705, 431, desplegableMatriculas ? "^" : "v", 15, Color(90, 120, 95));
	
	if (desplegableMatriculas)
	{
		RectangleShape lista(Vector2f(395, 84));
		lista.setPosition(340, 459);
		lista.setFillColor(Color::White);
		lista.setOutlineThickness(1);
		lista.setOutlineColor(Color(190, 200, 190));
		ventana.draw(lista);
		
		NodoAeronave* actual = listaAeronaves == nullptr ? nullptr : listaAeronaves->getPrimero();
		int saltadas = 0;
		while (actual != nullptr && saltadas < desplazamientoMatriculas)
		{
			actual = actual->siguiente;
			saltadas++;
		}
		
		int fila = 0;
		while (actual != nullptr && fila < 3)
		{
			float y = 459 + fila * 28;
			RectangleShape opcion(Vector2f(393, 27));
			opcion.setPosition(341, y);
			opcion.setFillColor(actual->aeronave.getMatricula() == matriculaAeronave ? Color(225, 237, 225) : (fila % 2 == 0 ? Color(250, 251, 249) : Color(242, 245, 241)));
			ventana.draw(opcion);
			dibujarTexto(ventana, 351, y + 5, actual->aeronave.getMatricula(), 13, Color(55, 70, 60));
			actual = actual->siguiente;
			fila++;
		}
		
		if (listaAeronaves == nullptr || listaAeronaves->getCantidad() == 0)
			dibujarTexto(ventana, 351, 466, "No hay aeronaves registradas.", 13, Color(140, 145, 140));
	}
	
	Color verde(90, 120, 95);
	Color gris(120, 130, 120);
	Color rojo(165, 85, 80);
	
	RectangleShape botonRegistrar(Vector2f(125, 40));
	botonRegistrar.setPosition(340, 565);
	botonRegistrar.setFillColor(verde);
	ventana.draw(botonRegistrar);
	dibujarTexto(ventana, 350, 576, editando ? "NUEVO / LIMPIAR" : "REGISTRAR", 12, Color::White);
	
	RectangleShape botonModificar(Vector2f(125, 40));
	botonModificar.setPosition(475, 565);
	botonModificar.setFillColor(gris);
	ventana.draw(botonModificar);
	dibujarTexto(ventana, 485, 576, "MODIFICAR", 14, Color::White);
	
	RectangleShape botonEliminar(Vector2f(125, 40));
	botonEliminar.setPosition(610, 565);
	botonEliminar.setFillColor(rojo);
	ventana.draw(botonEliminar);
	dibujarTexto(ventana, 622, 576, "ELIMINAR", 14, Color::White);
}

void PantallaTripulantes::dibujarListado(RenderWindow& ventana)
{
	float ancho = static_cast<float>(ventana.getSize().x);
	float anchoPanel = ancho - 785;
	
	RectangleShape panel(Vector2f(anchoPanel, 505));
	panel.setPosition(770, 110);
	panel.setFillColor(Color(248, 249, 247));
	panel.setOutlineThickness(1);
	panel.setOutlineColor(Color(215, 220, 215));
	ventana.draw(panel);
	
	RectangleShape encabezado(Vector2f(anchoPanel, 40));
	encabezado.setPosition(770, 110);
	encabezado.setFillColor(Color(90, 120, 95));
	ventana.draw(encabezado);
	
	dibujarTexto(ventana, 790, 119, "TRIPULANTES REGISTRADOS", 17, Color::White);
	dibujarTexto(ventana, 790, 165, "IDENTIFICACION", 12, Color(75, 80, 75));
	dibujarTexto(ventana, 930, 165, "NOMBRE", 12, Color(75, 80, 75));
	dibujarTexto(ventana, 1080, 165, "PUESTO", 12, Color(75, 80, 75));
	dibujarTexto(ventana, 1200, 165, "ESTADO", 12, Color(75, 80, 75));
	dibujarTexto(ventana, 1320, 165, "MATRICULA", 12, Color(75, 80, 75));
	
	RectangleShape separador(Vector2f(anchoPanel - 30, 1));
	separador.setPosition(785, 185);
	separador.setFillColor(Color(220, 225, 220));
	ventana.draw(separador);
	
	NodoTripulante* actual = listaTripulantes == nullptr ? nullptr : listaTripulantes->getPrimero();
	int fila = 0;
	while (actual != nullptr && fila < 9)
	{
		float y = 195 + fila * 40;
		RectangleShape fondoFila(Vector2f(anchoPanel - 20, 35));
		fondoFila.setPosition(780, y - 5);
		fondoFila.setFillColor(actual->tripulante.getIdentificacion() == identificacionSeleccionada ? Color(225, 237, 225) : (fila % 2 == 0 ? Color(250, 251, 249) : Color(242, 245, 241)));
		ventana.draw(fondoFila);
		
		string matricula = gestorAsignaciones == nullptr ? "" : gestorAsignaciones->buscarAeronaveTripulante(actual->tripulante.getIdentificacion());
		dibujarTexto(ventana, 790, y, actual->tripulante.getIdentificacion(), 11, Color(55, 65, 55));
		dibujarTexto(ventana, 930, y, actual->tripulante.getNombre(), 11, Color(55, 65, 55));
		dibujarTexto(ventana, 1080, y, actual->tripulante.getPuesto(), 11, Color(55, 65, 55));
		dibujarTexto(ventana, 1200, y, actual->tripulante.getEstado(), 11, Color(90, 120, 95));
		dibujarTexto(ventana, 1320, y, matricula.empty() ? "Sin asignar" : matricula, 11, Color(55, 65, 55));
		
		actual = actual->siguiente;
		fila++;
	}
	
	if (listaTripulantes == nullptr || listaTripulantes->getCantidad() == 0)
		dibujarTexto(ventana, 790, 220, "Todavia no hay tripulantes registrados.", 14, Color(140, 145, 140));
	
	dibujarTexto(ventana, 790, 590, "Total de tripulantes: " + to_string(listaTripulantes == nullptr ? 0 : listaTripulantes->getCantidad()), 14, Color(90, 120, 95));
}

void PantallaTripulantes::dibujarConfirmacion(RenderWindow& ventana)
{
	RectangleShape oscurecer(Vector2f(static_cast<float>(ventana.getSize().x), static_cast<float>(ventana.getSize().y)));
	oscurecer.setFillColor(Color(0, 0, 0, 110));
	ventana.draw(oscurecer);
	
	RectangleShape cuadro(Vector2f(430, 190));
	cuadro.setPosition(535, 300);
	cuadro.setFillColor(Color::White);
	cuadro.setOutlineThickness(2);
	cuadro.setOutlineColor(Color(90, 120, 95));
	ventana.draw(cuadro);
	
	dibujarTexto(ventana, 585, 335, "CONFIRMAR ELIMINACION", 20, Color(55, 70, 60));
	dibujarTexto(ventana, 575, 375, "Esta segura de eliminar este tripulante?", 16, Color(55, 65, 55));
	
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

void PantallaTripulantes::registrarTripulante()
{
	if (identificacion.empty() || nombre.empty() || puesto.empty() || estado.empty() || matriculaAeronave.empty())
	{
		mensaje = "Complete todos los campos y seleccione una aeronave.";
		return;
	}
	
	if (listaAeronaves == nullptr || listaAeronaves->buscar(matriculaAeronave) == nullptr)
	{
		mensaje = "Seleccione una matricula de aeronave valida.";
		return;
	}
	
	Tripulante nuevo(identificacion, nombre, puesto, estado);
	if (listaTripulantes->agregar(nuevo))
	{
		if (gestorAsignaciones->asignarTripulante(identificacion, matriculaAeronave))
		{
			mensaje = "Tripulante registrado y asignado correctamente.";
			limpiarFormulario();
		}
		else
		{
			listaTripulantes->eliminar(identificacion);
			mensaje = "No se pudo guardar la asignacion.";
		}
	}
	else
								  mensaje = "Ya existe un tripulante con esa identificacion.";
}

void PantallaTripulantes::seleccionarTripulante(string identificacion)
{
	NodoTripulante* actual = listaTripulantes == nullptr ? nullptr : listaTripulantes->buscar(identificacion);
	if (actual == nullptr) return;
	
	identificacionSeleccionada = identificacion;
	this->identificacion = actual->tripulante.getIdentificacion();
	nombre = actual->tripulante.getNombre();
	puesto = actual->tripulante.getPuesto();
	estado = actual->tripulante.getEstado();
	matriculaAeronave = gestorAsignaciones == nullptr ? "" : gestorAsignaciones->buscarAeronaveTripulante(identificacion);
	editando = true;
	desplegableMatriculas = false;
	mensaje = "Tripulante seleccionado.";
}

void PantallaTripulantes::modificarTripulante()
{
	if (!editando || identificacionSeleccionada.empty())
	{
		mensaje = "Seleccione un tripulante del listado.";
		return;
	}
	
	if (nombre.empty() || puesto.empty() || estado.empty() || matriculaAeronave.empty())
	{
		mensaje = "Complete los campos y seleccione una aeronave.";
		return;
	}
	
	if (listaAeronaves == nullptr || listaAeronaves->buscar(matriculaAeronave) == nullptr)
	{
		mensaje = "Seleccione una matricula de aeronave valida.";
		return;
	}
	
	if (listaTripulantes->modificar(identificacionSeleccionada, nombre, puesto, estado))
	{
		if (gestorAsignaciones->cambiarAeronaveTripulante(identificacionSeleccionada, matriculaAeronave))
		{
			seleccionarTripulante(identificacionSeleccionada);
			mensaje = "Tripulante y aeronave modificados correctamente.";
		}
		else
														  mensaje = "No se pudo actualizar la aeronave asignada.";
	}
	else
									mensaje = "No se pudo modificar el tripulante.";
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
	matriculaAeronave = "";
	identificacionSeleccionada = "";
	campoActivo = 0;
	editando = false;
	desplegableMatriculas = false;
	desplazamientoMatriculas = 0;
}

void PantallaTripulantes::dibujarTexto(RenderWindow& ventana, float x, float y, string texto, int tamano, Color color)
{
	if (!fuenteCargada) return;
	Text textoDibujado;
	textoDibujado.setFont(fuente);
	textoDibujado.setString(texto);
	textoDibujado.setCharacterSize(tamano);
	textoDibujado.setFillColor(color);
	textoDibujado.setPosition(x, y);
	ventana.draw(textoDibujado);
}
