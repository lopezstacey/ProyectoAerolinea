#include "PantallaAeronaves.h"
#include <windows.h>
#include <commdlg.h>

static const float CAMPO_X = 340;
static const float CAMPO_ANCHO = 390;
static const float CAMPO_ALTO = 32;
static const float CAMPO_Y0 = 200;
static const float CAMPO_PASO = 58;
static const float BTN_FORM_Y = 425;
static const float BTN_FORM_ALTO = 45;
static const float BTN_FORM_ANCHO = 125;
static const float BTN_REGISTRAR_X = 340;
static const float BTN_MODIFICAR_X = 475;
static const float BTN_ELIMINAR_X = 610;

static float anchoTexto(const Font& fuente, const string& texto, int tamano)
{
	Text medida;
	medida.setFont(fuente);
	medida.setCharacterSize(tamano);
	medida.setString(texto);
	return medida.getLocalBounds().width;
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

PantallaAeronaves::PantallaAeronaves(ListaAeronaves* listaAeronaves, ListaRutas* listaRutas, GestorAsignaciones* gestorAsignaciones)
{
	this->listaAeronaves = listaAeronaves;
	this->listaRutas = listaRutas;
	this->gestorAsignaciones = gestorAsignaciones;
	
	fuenteCargada = fuente.loadFromFile("resources/times.ttf");
	
	matricula = "";
	modelo = "";
	estado = "Disponible";
	matriculaSeleccionada = "";
	mensaje = "";
	rutaImagen = "";
	capacidad = 0;
	campoActivo = 0;
	desplazamientoLista = 0;
	editando = false;
	confirmarEliminar = false;
	imagenCargada = false;
	cursorVisible = true;
	tiempoCursor = 0;
}

void PantallaAeronaves::procesarEventos(RenderWindow& ventana, Event evento)
{
	if (confirmarEliminar)
	{
		if (evento.type == Event::MouseButtonPressed && evento.mouseButton.button == Mouse::Left)
		{
			float x = evento.mouseButton.x;
			float y = evento.mouseButton.y;
			
			if (x >= 635 && x <= 735 && y >= 420 && y <= 465)
			{
				if (listaAeronaves->eliminar(matriculaSeleccionada))
				{
					int rutasEliminadas = listaRutas != nullptr ? listaRutas->eliminarPorAeronave(matriculaSeleccionada) : 0;
					int asignacionesEliminadas = gestorAsignaciones != nullptr ? gestorAsignaciones->quitarAsignacionesAeronave(matriculaSeleccionada) : 0;
					mensaje = "Aeronave eliminada. Rutas: " + to_string(rutasEliminadas) + ", asignaciones: " + to_string(asignacionesEliminadas) + ".";
				}
				else
											 mensaje = "No se pudo eliminar la aeronave.";
				
				confirmarEliminar = false;
				limpiarFormulario();
			}
			else if (x >= 765 && x <= 865 && y >= 420 && y <= 465)
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
		
		if (x >= 780 && x <= ventana.getSize().x - 15 && y >= 190 && y < 470)
		{
			int maximo = listaAeronaves->getCantidad() - 7;
			if (maximo < 0)
				maximo = 0;
			
			if (evento.mouseWheelScroll.delta < 0)
				desplazamientoLista += 1;
			else if (evento.mouseWheelScroll.delta > 0)
				desplazamientoLista -= 1;
			
			if (desplazamientoLista < 0)
				desplazamientoLista = 0;
			if (desplazamientoLista > maximo)
				desplazamientoLista = maximo;
		}
		return;
	}
	
	if (evento.type == Event::MouseButtonPressed && evento.mouseButton.button == Mouse::Left)
	{
		float x = evento.mouseButton.x;
		float y = evento.mouseButton.y;
		
		if (x >= CAMPO_X && x <= CAMPO_X + CAMPO_ANCHO)
		{
			for (int i = 0; i < 4; i++)
			{
				float yCampo = CAMPO_Y0 + i * CAMPO_PASO;
				
				if (y >= yCampo - 20 && y <= yCampo + CAMPO_ALTO)
				{
					if (!(i == 0 && editando))
						campoActivo = i;
				}
			}
		}
		
		bool enFilaBotones = y >= BTN_FORM_Y && y <= BTN_FORM_Y + BTN_FORM_ALTO;
		
		if (enFilaBotones && x >= BTN_REGISTRAR_X && x <= BTN_REGISTRAR_X + BTN_FORM_ANCHO)
		{
			registrarAeronave();
		}
		else if (enFilaBotones && x >= BTN_MODIFICAR_X && x <= BTN_MODIFICAR_X + BTN_FORM_ANCHO)
		{
			modificarAeronave();
		}
		else if (enFilaBotones && x >= BTN_ELIMINAR_X && x <= BTN_ELIMINAR_X + BTN_FORM_ANCHO)
		{
			eliminarAeronave();
		}
		else if (x >= 345 && x <= 505 && y >= 575 && y <= 620)
		{
			cargarImagen();
		}
		else if (x >= 780 && x <= ventana.getSize().x - 30 && y >= 190 && y < 470)
		{
			int fila = desplazamientoLista + (int)((y - 190) / 40);
			NodoAeronave* actual = listaAeronaves->getPrimero();
			
			while (actual != nullptr && fila > 0)
			{
				actual = actual->siguiente;
				fila--;
			}
			
			if (actual != nullptr)
				seleccionarAeronave(actual->aeronave.getMatricula());
		}
	}
	
	if (evento.type == Event::KeyPressed && evento.key.code == Keyboard::Tab)
	{
		campoActivo++;
		
		if (campoActivo > 3)
			campoActivo = 0;
		
		if (editando && campoActivo == 0)
			campoActivo = 1;
	}
	
	if (evento.type == Event::TextEntered)
	{
		if (campoActivo == 0 || campoActivo == 1 || campoActivo == 3)
		{
			string* campo = nullptr;
			
			if (campoActivo == 0)
				campo = &matricula;
			else if (campoActivo == 1)
				campo = &modelo;
			else
				campo = &estado;
			
			if (campoActivo == 0 && editando)
				campo = nullptr;
			
			if (campo != nullptr)
			{
				if (evento.text.unicode == 8)
				{
					if (!campo->empty())
						campo->pop_back();
				}
				else if (evento.text.unicode >= 32 && evento.text.unicode < 127 && campo->length() < 30)
				{
					campo->push_back((char)evento.text.unicode);
				}
			}
		}
		else if (campoActivo == 2)
		{
			if (evento.text.unicode == 8)
			{
				capacidad /= 10;
			}
			else if (evento.text.unicode >= '0' && evento.text.unicode <= '9' && capacidad < 10000)
			{
				capacidad = capacidad * 10 + (evento.text.unicode - '0');
			}
		}
	}
	
	if (evento.type == Event::KeyPressed && evento.key.code == Keyboard::Escape)
		limpiarFormulario();
}

void PantallaAeronaves::actualizar(float tiempo)
{
	tiempoCursor += tiempo;
	
	if (tiempoCursor >= 0.5f)
	{
		cursorVisible = !cursorVisible;
		tiempoCursor = 0;
	}
}

void PantallaAeronaves::dibujar(RenderWindow& ventana)
{
	float ancho = static_cast<float>(ventana.getSize().x);
	
	ventana.clear(Color(242, 244, 242));
	
	RectangleShape fondo(Vector2f(ancho - 300, static_cast<float>(ventana.getSize().y)));
	fondo.setPosition(300, 0);
	fondo.setFillColor(Color(242, 244, 242));
	ventana.draw(fondo);
	
	RectangleShape encabezado(Vector2f(ancho - 300, 105));
	encabezado.setPosition(300, 0);
	encabezado.setFillColor(Color::White);
	ventana.draw(encabezado);
	
	RectangleShape linea(Vector2f(ancho - 300, 2));
	linea.setPosition(300, 102);
	linea.setFillColor(Color(125, 150, 135));
	ventana.draw(linea);
	
	dibujarTexto(ventana, 335, 25, "GESTION DE AERONAVES", 28, Color(55, 70, 60));
	dibujarTexto(ventana, 337, 65, "Administracion y control de la flota aerea", 14, Color(120, 125, 120));
	
	dibujarFormulario(ventana);
	dibujarListado(ventana);
	dibujarImagen(ventana);
	
	if (!mensaje.empty())
	{
		RectangleShape barraMensaje(Vector2f(285, 32));
		barraMensaje.setPosition(345, 650);
		barraMensaje.setFillColor(Color(232, 239, 233));
		barraMensaje.setOutlineThickness(1);
		barraMensaje.setOutlineColor(Color(200, 215, 202));
		ventana.draw(barraMensaje);
		
		dibujarTexto(ventana, 355, 657, recortarTexto(fuente, mensaje, 12, 265), 12, Color(65, 95, 70));
	}
	
	if (confirmarEliminar)
		dibujarConfirmacion(ventana);
}

void PantallaAeronaves::dibujarFormulario(RenderWindow& ventana)
{
	RectangleShape panel(Vector2f(430, 390));
	panel.setPosition(325, 115);
	panel.setFillColor(Color::White);
	panel.setOutlineThickness(1);
	panel.setOutlineColor(Color(215, 220, 215));
	ventana.draw(panel);
	
	RectangleShape encabezado(Vector2f(430, 42));
	encabezado.setPosition(325, 115);
	encabezado.setFillColor(editando ? Color(225, 237, 225) : Color(230, 235, 230));
	ventana.draw(encabezado);
	
	dibujarTexto(ventana, 345, 125, editando ? "EDITANDO AERONAVE" : "DATOS DE LA AERONAVE", 17, Color(55, 70, 60));
	
	string etiquetas[4] = {
		editando ? "Matricula (no se puede cambiar):" : "Matricula:",
			"Modelo:",
			"Capacidad de pasajeros:",
			"Estado:"
	};
	string valores[4] = {
		matricula,
			modelo,
			capacidad > 0 ? to_string(capacidad) : "",
			estado
	};
	
	for (int i = 0; i < 4; i++)
	{
		float y = CAMPO_Y0 + i * CAMPO_PASO;
		
		dibujarTexto(ventana, 345, y - 24, etiquetas[i], 13, Color(75, 80, 75));
		
		RectangleShape campo(Vector2f(CAMPO_ANCHO, CAMPO_ALTO));
		campo.setPosition(CAMPO_X, y);
		campo.setFillColor(i == 0 && editando ? Color(235, 238, 234) : Color(250, 251, 249));
		campo.setOutlineThickness(campoActivo == i ? 2 : 1);
		campo.setOutlineColor(campoActivo == i ? Color(115, 145, 120) : Color(210, 215, 210));
		ventana.draw(campo);
		
		dibujarTexto(ventana, 350, y + 5, valores[i], 15, Color(45, 50, 45));
		
		if (cursorVisible && fuenteCargada && campoActivo == i)
		{
			RectangleShape cursor(Vector2f(2, 19));
			cursor.setPosition(352 + anchoTexto(fuente, valores[i], 15), y + 6);
			cursor.setFillColor(Color(65, 85, 65));
			ventana.draw(cursor);
		}
	}
	
	Vector2i mouse = Mouse::getPosition(ventana);
	bool filaBotones = mouse.y >= BTN_FORM_Y && mouse.y <= BTN_FORM_Y + BTN_FORM_ALTO;
	
	RectangleShape botonRegistrar(Vector2f(BTN_FORM_ANCHO, BTN_FORM_ALTO));
	botonRegistrar.setPosition(BTN_REGISTRAR_X, BTN_FORM_Y);
	botonRegistrar.setFillColor(filaBotones && mouse.x >= BTN_REGISTRAR_X && mouse.x <= BTN_REGISTRAR_X + BTN_FORM_ANCHO ? Color(115, 145, 120) : Color(90, 120, 95));
	ventana.draw(botonRegistrar);
	dibujarTexto(ventana, BTN_REGISTRAR_X + (BTN_FORM_ANCHO - anchoTexto(fuente, "REGISTRAR", 14)) / 2, BTN_FORM_Y + 13, "REGISTRAR", 14, Color::White);
	
	RectangleShape botonModificar(Vector2f(BTN_FORM_ANCHO, BTN_FORM_ALTO));
	botonModificar.setPosition(BTN_MODIFICAR_X, BTN_FORM_Y);
	botonModificar.setFillColor(filaBotones && mouse.x >= BTN_MODIFICAR_X && mouse.x <= BTN_MODIFICAR_X + BTN_FORM_ANCHO ? Color(145, 150, 140) : Color(120, 130, 120));
	ventana.draw(botonModificar);
	dibujarTexto(ventana, BTN_MODIFICAR_X + (BTN_FORM_ANCHO - anchoTexto(fuente, "MODIFICAR", 14)) / 2, BTN_FORM_Y + 13, "MODIFICAR", 14, Color::White);
	
	RectangleShape botonEliminar(Vector2f(BTN_FORM_ANCHO, BTN_FORM_ALTO));
	botonEliminar.setPosition(BTN_ELIMINAR_X, BTN_FORM_Y);
	botonEliminar.setFillColor(filaBotones && mouse.x >= BTN_ELIMINAR_X && mouse.x <= BTN_ELIMINAR_X + BTN_FORM_ANCHO ? Color(190, 105, 100) : Color(165, 85, 80));
	ventana.draw(botonEliminar);
	dibujarTexto(ventana, BTN_ELIMINAR_X + (BTN_FORM_ANCHO - anchoTexto(fuente, "ELIMINAR", 14)) / 2, BTN_FORM_Y + 13, "ELIMINAR", 14, Color::White);
	
	if (editando)
		dibujarTexto(ventana, 345, 480, "Cambie los datos y presione MODIFICAR. Esc = cancelar.", 12, Color(90, 120, 95));
}

void PantallaAeronaves::dibujarListado(RenderWindow& ventana)
{
	float ancho = static_cast<float>(ventana.getSize().x);
	
	RectangleShape panel(Vector2f(ancho - 785, 390));
	panel.setPosition(770, 115);
	panel.setFillColor(Color::White);
	panel.setOutlineThickness(1);
	panel.setOutlineColor(Color(215, 220, 215));
	ventana.draw(panel);
	
	RectangleShape encabezado(Vector2f(ancho - 785, 42));
	encabezado.setPosition(770, 115);
	encabezado.setFillColor(Color(230, 235, 230));
	ventana.draw(encabezado);
	
	dibujarTexto(ventana, 790, 125, "AERONAVES REGISTRADAS", 17, Color(55, 70, 60));
	
	dibujarTexto(ventana, 790, 163, "MATRICULA", 12, Color(90, 95, 90));
	dibujarTexto(ventana, 900, 163, "MODELO", 12, Color(90, 95, 90));
	dibujarTexto(ventana, 1100, 163, "CAPACIDAD", 12, Color(90, 95, 90));
	dibujarTexto(ventana, 1220, 163, "ESTADO", 12, Color(90, 95, 90));
	
	RectangleShape separador(Vector2f(ancho - 815, 1));
	separador.setPosition(785, 184);
	separador.setFillColor(Color(220, 225, 220));
	ventana.draw(separador);
	
	int totalAeronaves = listaAeronaves->getCantidad();
	const int filasVisibles = 7;
	int maximoDesplazamiento = totalAeronaves - filasVisibles;
	if (maximoDesplazamiento < 0)
		maximoDesplazamiento = 0;
	if (desplazamientoLista > maximoDesplazamiento)
		desplazamientoLista = maximoDesplazamiento;
	
	NodoAeronave* actual = listaAeronaves->getPrimero();
	for (int i = 0; actual != nullptr && i < desplazamientoLista; i++)
		actual = actual->siguiente;
	
	int fila = 0;
	
	while (actual != nullptr && fila < filasVisibles)
	{
		float y = 195 + fila * 40;
		
		RectangleShape fondoFila(Vector2f(ancho - 810, 35));
		fondoFila.setPosition(780, y - 5);
		
		if (actual->aeronave.getMatricula() == matriculaSeleccionada)
		{
			fondoFila.setFillColor(Color(225, 237, 225));
			fondoFila.setOutlineThickness(1);
			fondoFila.setOutlineColor(Color(145, 170, 145));
		}
		else
		{
			fondoFila.setFillColor(fila % 2 == 0 ? Color(250, 251, 249) : Color(242, 245, 241));
		}
		
		ventana.draw(fondoFila);
		
		dibujarTexto(ventana, 790, y + 3, recortarTexto(fuente, actual->aeronave.getMatricula(), 13, 100), 13, Color(55, 65, 55));
		dibujarTexto(ventana, 900, y + 3, recortarTexto(fuente, actual->aeronave.getModelo(), 13, 185), 13, Color(55, 65, 55));
		dibujarTexto(ventana, 1100, y + 3, to_string(actual->aeronave.getCapacidad()), 13, Color(55, 65, 55));
		dibujarTexto(ventana, 1220, y + 3, recortarTexto(fuente, actual->aeronave.getEstado(), 13, 120), 13, Color(80, 110, 85));
		
		actual = actual->siguiente;
		fila++;
	}
	
	const float altoPista = 275.0f;
	RectangleShape pista(Vector2f(6, altoPista));
	pista.setPosition(ancho - 26, 190);
	pista.setFillColor(Color(225, 229, 224));
	ventana.draw(pista);
	
	float altoIndicador = altoPista;
	if (totalAeronaves > filasVisibles)
	{
		altoIndicador = altoPista * filasVisibles / totalAeronaves;
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
	
	dibujarTexto(ventana, 790, 477, "Total de aeronaves: " + to_string(totalAeronaves), 14, Color(65, 80, 65));
	
	if (listaAeronaves->getCantidad() == 0)
	{
		dibujarTexto(ventana, 790, 225, "Todavia no hay aeronaves registradas.", 14, Color(140, 145, 140));
	}
}

void PantallaAeronaves::dibujarImagen(RenderWindow& ventana)
{
	RectangleShape panel(Vector2f(1150, 260));
	panel.setPosition(325, 520);
	panel.setFillColor(Color::White);
	panel.setOutlineThickness(1);
	panel.setOutlineColor(Color(215, 220, 215));
	ventana.draw(panel);
	
	RectangleShape encabezado(Vector2f(1150, 42));
	encabezado.setPosition(325, 520);
	encabezado.setFillColor(Color(230, 235, 230));
	ventana.draw(encabezado);
	
	dibujarTexto(ventana, 345, 530, "FOTOGRAFIA DE LA AERONAVE", 17, Color(55, 70, 60));
	
	Vector2i mouse = Mouse::getPosition(ventana);
	bool sobreImagen = mouse.x >= 345 && mouse.x <= 505 && mouse.y >= 575 && mouse.y <= 620;
	
	RectangleShape botonImagen(Vector2f(160, 45));
	botonImagen.setPosition(345, 575);
	botonImagen.setFillColor(sobreImagen ? Color(115, 145, 120) : Color(90, 120, 95));
	ventana.draw(botonImagen);
	dibujarTexto(ventana, 345 + (160 - anchoTexto(fuente, "SUBIR IMAGEN", 13)) / 2, 588, "SUBIR IMAGEN", 13, Color::White);
	
	RectangleShape marcoImagen(Vector2f(800, 180));
	marcoImagen.setPosition(650, 570);
	marcoImagen.setFillColor(Color(248, 249, 247));
	marcoImagen.setOutlineThickness(1);
	marcoImagen.setOutlineColor(Color(215, 220, 215));
	ventana.draw(marcoImagen);
	
	if (matriculaSeleccionada.empty())
	{
		string aviso = "Seleccione una aeronave para ver su fotografia.";
		dibujarTexto(ventana, 650 + (800 - anchoTexto(fuente, aviso, 14)) / 2, 650, aviso, 14, Color(120, 125, 120));
	}
	else if (imagenCargada)
	{
		Vector2u dimensiones = texturaImagen.getSize();
		
		if (dimensiones.x > 0 && dimensiones.y > 0)
		{
			float escalaX = 790.0f / dimensiones.x;
			float escalaY = 170.0f / dimensiones.y;
			float escala = escalaX < escalaY ? escalaX : escalaY;
			
			spriteImagen.setTexture(texturaImagen, true);
			spriteImagen.setScale(escala, escala);
			spriteImagen.setPosition(655 + (790 - dimensiones.x * escala) / 2, 575 + (170 - dimensiones.y * escala) / 2);
			ventana.draw(spriteImagen);
		}
	}
	else
	{
		string aviso = "Esta aeronave no tiene una fotografia cargada.";
		dibujarTexto(ventana, 650 + (800 - anchoTexto(fuente, aviso, 14)) / 2, 650, aviso, 14, Color(120, 125, 120));
	}
}

void PantallaAeronaves::dibujarConfirmacion(RenderWindow& ventana)
{
	RectangleShape oscurecer(Vector2f(static_cast<float>(ventana.getSize().x), static_cast<float>(ventana.getSize().y)));
	oscurecer.setFillColor(Color(30, 35, 30, 120));
	ventana.draw(oscurecer);
	
	RectangleShape cuadro(Vector2f(430, 190));
	cuadro.setPosition(535, 300);
	cuadro.setFillColor(Color::White);
	cuadro.setOutlineThickness(1);
	cuadro.setOutlineColor(Color(200, 210, 200));
	ventana.draw(cuadro);
	
	RectangleShape encabezado(Vector2f(430, 45));
	encabezado.setPosition(535, 300);
	encabezado.setFillColor(Color(230, 235, 230));
	ventana.draw(encabezado);
	
	string titulo = "CONFIRMAR ELIMINACION";
	string pregunta = "Esta segura de eliminar esta aeronave?";
	
	dibujarTexto(ventana, 535 + (430 - anchoTexto(fuente, titulo, 19)) / 2, 311, titulo, 19, Color(55, 70, 60));
	dibujarTexto(ventana, 535 + (430 - anchoTexto(fuente, pregunta, 15)) / 2, 367, pregunta, 15, Color(65, 70, 65));
	
	RectangleShape botonSi(Vector2f(100, 45));
	botonSi.setPosition(635, 420);
	botonSi.setFillColor(Color(165, 85, 80));
	ventana.draw(botonSi);
	dibujarTexto(ventana, 635 + (100 - anchoTexto(fuente, "SI", 16)) / 2, 432, "SI", 16, Color::White);
	
	RectangleShape botonNo(Vector2f(100, 45));
	botonNo.setPosition(765, 420);
	botonNo.setFillColor(Color(90, 120, 95));
	ventana.draw(botonNo);
	dibujarTexto(ventana, 765 + (100 - anchoTexto(fuente, "NO", 16)) / 2, 432, "NO", 16, Color::White);
}

void PantallaAeronaves::registrarAeronave()
{
	if (matricula.empty() || modelo.empty() || capacidad <= 0 || estado.empty())
	{
		mensaje = "Complete todos los campos correctamente.";
		return;
	}
	
	Aeronave nueva(matricula, modelo, capacidad, estado, rutaImagen);
	
	if (listaAeronaves->agregar(nueva))
	{
		mensaje = "Aeronave registrada correctamente.";
		limpiarFormulario();
	}
	else
	{
		mensaje = "Ya existe una aeronave con esa matricula.";
	}
}

void PantallaAeronaves::seleccionarAeronave(string matricula)
{
	NodoAeronave* actual = listaAeronaves->buscar(matricula);
	
	if (actual == nullptr)
		return;
	
	matriculaSeleccionada = matricula;
	this->matricula = actual->aeronave.getMatricula();
	modelo = actual->aeronave.getModelo();
	capacidad = actual->aeronave.getCapacidad();
	estado = actual->aeronave.getEstado();
	rutaImagen = actual->aeronave.getImagen();
	
	imagenCargada = false;
	
	if (!rutaImagen.empty())
		imagenCargada = texturaImagen.loadFromFile(rutaImagen);
	
	editando = true;
	campoActivo = 1;   
	mensaje = "Aeronave seleccionada.";
}

void PantallaAeronaves::modificarAeronave()
{
	if (!editando || matriculaSeleccionada.empty())
	{
		mensaje = "Seleccione una aeronave del listado.";
		return;
	}
	
	if (modelo.empty() || capacidad <= 0 || estado.empty())
	{
		mensaje = "Complete los campos correctamente.";
		return;
	}
	
	if (listaAeronaves->modificar(matriculaSeleccionada, modelo, capacidad, estado))
	{
		NodoAeronave* actual = listaAeronaves->buscar(matriculaSeleccionada);
		
		if (actual != nullptr)
			actual->aeronave.setImagen(rutaImagen);
		
		limpiarFormulario();   
		mensaje = "Aeronave modificada correctamente.";
	}
	else
	{
		mensaje = "No se pudo modificar la aeronave.";
	}
}

void PantallaAeronaves::eliminarAeronave()
{
	if (matriculaSeleccionada.empty())
	{
		mensaje = "Seleccione una aeronave del listado.";
		return;
	}
	
	confirmarEliminar = true;
}

void PantallaAeronaves::limpiarFormulario()
{
	matricula = "";
	modelo = "";
	estado = "Disponible";
	matriculaSeleccionada = "";
	rutaImagen = "";
	
	capacidad = 0;
	campoActivo = 0;
	editando = false;
	imagenCargada = false;
}

void PantallaAeronaves::cargarImagen()
{
	if (matricula.empty())
	{
		mensaje = "Primero escriba la matricula o seleccione una aeronave.";
		return;
	}
	
	char nombreArchivo[MAX_PATH] = "";
	
	OPENFILENAMEA selector;
	ZeroMemory(&selector, sizeof(selector));
	
	selector.lStructSize = sizeof(selector);
	selector.hwndOwner = GetActiveWindow();
	selector.lpstrFile = nombreArchivo;
	selector.nMaxFile = MAX_PATH;
	selector.lpstrFilter = "Archivos de imagen\0*.jpg;*.jpeg;*.png;*.bmp\0Todos los archivos\0*.*\0";
	selector.nFilterIndex = 1;
	selector.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_EXPLORER | OFN_NOCHANGEDIR;
	
	if (GetOpenFileNameA(&selector))
	{
		rutaImagen = nombreArchivo;
		
		if (texturaImagen.loadFromFile(rutaImagen))
		{
			imagenCargada = true;
			
			if (!matriculaSeleccionada.empty())
			{
				NodoAeronave* actual = listaAeronaves->buscar(matriculaSeleccionada);
				
				if (actual != nullptr)
					actual->aeronave.setImagen(rutaImagen);
			}
			
			mensaje = "Imagen cargada correctamente.";
		}
		else
		{
			imagenCargada = false;
			mensaje = "No se pudo cargar la imagen.";
		}
	}
}

void PantallaAeronaves::dibujarTexto(RenderWindow& ventana, float x, float y, string texto, int tamano, Color color)
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
