#include "PantallaAeronaves.h"
#include <windows.h>
#include <commdlg.h>

PantallaAeronaves::PantallaAeronaves(ListaAeronaves* listaAeronaves)
{
	this->listaAeronaves = listaAeronaves;
	
	fuenteCargada = fuente.loadFromFile("resources/times.ttf");
	
	matricula = "";
	modelo = "";
	estado = "";
	matriculaSeleccionada = "";
	mensaje = "";
	rutaImagen = "";
	
	capacidad = 0;
	campoActivo = 0;
	
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
			
			if (x >= 610 && x <= 710 && y >= 420 && y <= 465)
			{
				if (listaAeronaves->eliminar(matriculaSeleccionada))
					mensaje = "Aeronave eliminada correctamente.";
				else
					mensaje = "No se pudo eliminar la aeronave.";
				
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
		{
			registrarAeronave();
		}
		else if (x >= 475 && x <= 600 && y >= 400 && y <= 445)
		{
			modificarAeronave();
		}
		else if (x >= 610 && x <= 735 && y >= 400 && y <= 445)
		{
			eliminarAeronave();
		}
		else if (x >= 345 && x <= 505 && y >= 565 && y <= 610)
		{
			cargarImagen();
		}
		else if (x >= 780 && x <= ventana.getSize().x - 20 && y >= 195 && y <= 495)
		{
			int fila = (int)((y - 195) / 40);
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
		else if (campoActivo == 2)
		{
			if (evento.text.unicode == 8)
			{
				capacidad /= 10;
			}
			else if (evento.text.unicode >= '0' && evento.text.unicode <= '9')
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
	float ancho = ventana.getSize().x;
	float alto = ventana.getSize().y;
	
	RectangleShape fondo(Vector2f(ancho - 300, alto));
	fondo.setPosition(300, 0);
	fondo.setFillColor(Color::White);
	ventana.draw(fondo);
	
	dibujarTexto(ventana, 335, 35, "GESTION DE AERONAVES", 28, Color(25, 65, 105));
	dibujarTexto(ventana, 335, 85, "REGISTRO DE AERONAVES", 20, Color::Black);
	
	dibujarFormulario(ventana);
	dibujarListado(ventana);
	dibujarImagen(ventana);
	
	if (!mensaje.empty())
		dibujarTexto(ventana, 340, 475, mensaje, 16, Color(25, 65, 105));
	
	if (confirmarEliminar)
		dibujarConfirmacion(ventana);
}

void PantallaAeronaves::dibujarFormulario(RenderWindow& ventana)
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
	
	dibujarTexto(ventana, 345, 125, "Matricula:", 16, Color::Black);
	dibujarTexto(ventana, 345, 155, matricula, 16, Color::Black);
	dibujarTexto(ventana, 345, 185, "Modelo:", 16, Color::Black);
	dibujarTexto(ventana, 345, 215, modelo, 16, Color::Black);
	dibujarTexto(ventana, 345, 245, "Capacidad:", 16, Color::Black);
	dibujarTexto(ventana, 345, 275, to_string(capacidad), 16, Color::Black);
	dibujarTexto(ventana, 345, 305, "Estado:", 16, Color::Black);
	dibujarTexto(ventana, 345, 335, estado, 16, Color::Black);
	
	if (cursorVisible && fuenteCargada)
	{
		string contenido;
		
		if (campoActivo == 0)
			contenido = matricula;
		else if (campoActivo == 1)
			contenido = modelo;
		else if (campoActivo == 2)
			contenido = to_string(capacidad);
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

void PantallaAeronaves::dibujarListado(RenderWindow& ventana)
{
	float ancho = ventana.getSize().x;
	
	RectangleShape panel(Vector2f(ancho - 785, 390));
	panel.setPosition(770, 115);
	panel.setFillColor(Color(245, 247, 250));
	panel.setOutlineThickness(1);
	panel.setOutlineColor(Color(210, 215, 220));
	ventana.draw(panel);
	
	dibujarTexto(ventana, 790, 125, "AERONAVES REGISTRADAS", 20, Color(25, 65, 105));
	dibujarTexto(ventana, 790, 165, "MATRICULA       MODELO       CAPACIDAD       ESTADO", 13, Color::Black);
	
	NodoAeronave* actual = listaAeronaves->getPrimero();
	int fila = 0;
	
	while (actual != nullptr)
	{
		float y = 195 + fila * 40;
		
		if (actual->aeronave.getMatricula() == matriculaSeleccionada)
		{
			RectangleShape seleccionado(Vector2f(ancho - 810, 35));
			seleccionado.setPosition(780, y - 5);
			seleccionado.setFillColor(Color(215, 232, 245));
			ventana.draw(seleccionado);
		}
		
		dibujarTexto(ventana, 790, y, actual->aeronave.getMatricula(), 13, Color::Black);
		dibujarTexto(ventana, 900, y, actual->aeronave.getModelo(), 13, Color::Black);
		dibujarTexto(ventana, 1030, y, to_string(actual->aeronave.getCapacidad()), 13, Color::Black);
		dibujarTexto(ventana, 1110, y, actual->aeronave.getEstado(), 13, Color::Black);
		
		actual = actual->siguiente;
		fila++;
	}
	
	dibujarTexto(ventana, 790, 465, "Total de aeronaves: " + to_string(listaAeronaves->getCantidad()), 15, Color::Black);
}

void PantallaAeronaves::dibujarImagen(RenderWindow& ventana)
{
	RectangleShape panel(Vector2f(1150, 260));
	panel.setPosition(325, 520);
	panel.setFillColor(Color(245, 247, 250));
	panel.setOutlineThickness(1);
	panel.setOutlineColor(Color(210, 215, 220));
	ventana.draw(panel);
	
	dibujarTexto(ventana, 345, 535, "FOTOGRAFIA DE LA AERONAVE", 18, Color(25, 65, 105));
	
	RectangleShape botonImagen(Vector2f(160, 45));
	botonImagen.setPosition(345, 565);
	botonImagen.setFillColor(Color(28, 91, 145));
	ventana.draw(botonImagen);
	dibujarTexto(ventana, 360, 578, "CARGAR IMAGEN", 14, Color::White);
	
	if (matriculaSeleccionada.empty())
	{
		dibujarTexto(ventana, 345, 635, "Seleccione una aeronave para ver su fotografia.", 14, Color(90, 90, 90));
	}
	else if (imagenCargada)
	{
		Vector2u dimensiones = texturaImagen.getSize();
		
		if (dimensiones.x > 0 && dimensiones.y > 0)
		{
			float escalaX = 790.0f / dimensiones.x;
			float escalaY = 205.0f / dimensiones.y;
			float escala = escalaX;
			
			if (escalaY < escala)
				escala = escalaY;
			
			spriteImagen.setTexture(texturaImagen, true);
			spriteImagen.setScale(escala, escala);
			spriteImagen.setPosition(650 + (790 - dimensiones.x * escala) / 2, 545 + (205 - dimensiones.y * escala) / 2);
			ventana.draw(spriteImagen);
		}
	}
	else
	{
		dibujarTexto(ventana, 650, 630, "Esta aeronave no tiene una fotografia cargada.", 15, Color(90, 90, 90));
	}
}

void PantallaAeronaves::dibujarConfirmacion(RenderWindow& ventana)
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
	dibujarTexto(ventana, 575, 375, "Esta segura de eliminar esta aeronave?", 16, Color::Black);
	
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
		
		seleccionarAeronave(matriculaSeleccionada);
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
