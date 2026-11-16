#include "PantallaPasajeros.h"
#include "PantallaMaletas.h"

PantallaPasajeros::PantallaPasajeros(ListaPasajeros* listaPasajeros, ListaMaletas* listaMaletas, GestorPantallas* gestorPantallas)
{
	this->listaPasajeros = listaPasajeros;
	this->listaMaletas = listaMaletas;
	this->gestorPantallas = gestorPantallas;
	fuenteCargada = fuente.loadFromFile("resources/times.ttf");
	
	identificacion = "";
	nombre = "";
	telefono = "";
	correo = "";
	identificacionSeleccionada = "";
	mensaje = "";
	campoActivo = 0;
	editando = false;
	confirmarEliminar = false;
	llevaMaleta = false;
	cursorVisible = true;
	tiempoCursor = 0;
}

void PantallaPasajeros::procesarEventos(RenderWindow& ventana, Event evento)
{
	if (confirmarEliminar)
	{
		if (evento.type == Event::MouseButtonPressed && evento.mouseButton.button == Mouse::Left)
		{
			int x = evento.mouseButton.x;
			int y = evento.mouseButton.y;
			
			if (x >= 690 && x <= 790 && y >= 430 && y <= 475)
			{
				if (listaPasajeros->eliminar(identificacionSeleccionada))
					mensaje = "Pasajero eliminado correctamente";
				else
					mensaje = "No se pudo eliminar el pasajero";
				
				limpiarFormulario();
				confirmarEliminar = false;
			}
			else if (x >= 810 && x <= 910 && y >= 430 && y <= 475)
			{
				confirmarEliminar = false;
			}
		}
		return;
	}
	
	if (evento.type == Event::MouseButtonPressed && evento.mouseButton.button == Mouse::Left)
	{
		int x = evento.mouseButton.x;
		int y = evento.mouseButton.y;
		
		if (x >= 350 && x <= 730)
		{
			if (y >= 145 && y <= 190)
				campoActivo = 0;
			else if (y >= 205 && y <= 250)
				campoActivo = 1;
			else if (y >= 265 && y <= 310)
				campoActivo = 2;
			else if (y >= 325 && y <= 370)
				campoActivo = 3;
		}
		
		if (!editando && x >= 350 && x <= 560 && y >= 400 && y <= 430)
		{
			llevaMaleta = !llevaMaleta;
		}
		
		if (x >= 345 && x <= 510 && y >= 445 && y <= 490)
		{
			if (editando)
				modificarPasajero();
			else
				registrarPasajero();
		}
		else if (x >= 525 && x <= 690 && y >= 445 && y <= 490)
		{
			eliminarPasajero();
		}
		
		if (x >= 785 && x <= 1450 && y >= 165 && y <= 690)
		{
			int indice = (y - 165) / 38;
			NodoPasajero* actual = listaPasajeros->getPrimero();
			
			for (int i = 0; actual != nullptr && i < indice; i++)
				actual = actual->siguiente;
			
			if (actual != nullptr)
				seleccionarPasajero(actual->pasajero.getIdentificacion());
		}
	}
	
	if (evento.type == Event::KeyPressed && evento.key.code == Keyboard::Tab)
	{
		campoActivo++;
		if (campoActivo > 3)
			campoActivo = 0;
	}
	
	if (evento.type == Event::KeyPressed && evento.key.code == Keyboard::Escape)
	{
		limpiarFormulario();
		mensaje = "";
	}
	
	if (evento.type == Event::TextEntered)
	{
		if (evento.text.unicode == 8)
		{
			if (campoActivo == 0 && !identificacion.empty())
				identificacion.pop_back();
			else if (campoActivo == 1 && !nombre.empty())
				nombre.pop_back();
			else if (campoActivo == 2 && !telefono.empty())
				telefono.pop_back();
			else if (campoActivo == 3 && !correo.empty())
				correo.pop_back();
		}
		else if (evento.text.unicode >= 32 && evento.text.unicode < 128)
		{
			char caracter = static_cast<char>(evento.text.unicode);
			
			if (campoActivo == 0 && identificacion.size() < 25)
				identificacion += caracter;
			else if (campoActivo == 1 && nombre.size() < 45)
				nombre += caracter;
			else if (campoActivo == 2 && telefono.size() < 20)
				telefono += caracter;
			else if (campoActivo == 3 && correo.size() < 60)
				correo += caracter;
		}
	}
}

void PantallaPasajeros::actualizar(float tiempo)
{
	tiempoCursor += tiempo;
	
	if (tiempoCursor >= 0.5f)
	{
		cursorVisible = !cursorVisible;
		tiempoCursor = 0;
	}
}

void PantallaPasajeros::dibujar(RenderWindow& ventana)
{
	RectangleShape fondo(Vector2f(1200, 800));
	fondo.setPosition(300, 0);
	fondo.setFillColor(Color::White);
	ventana.draw(fondo);
	
	dibujarTexto(ventana, 335, 35, "GESTION DE PASAJEROS", 28, Color(20, 30, 40));
	dibujarTexto(ventana, 335, 75, "REGISTRO Y ADMINISTRACION DE PASAJEROS", 14, Color(90, 100, 110));
	
	dibujarFormulario(ventana);
	dibujarListado(ventana);
	
	if (!mensaje.empty())
		dibujarTexto(ventana, 335, 755, mensaje, 16, Color(28, 91, 145));
	
	if (confirmarEliminar)
		dibujarConfirmacion(ventana);
}

void PantallaPasajeros::dibujarFormulario(RenderWindow& ventana)
{
	RectangleShape panel(Vector2f(430, 450));
	panel.setPosition(325, 115);
	panel.setFillColor(Color(245, 247, 250));
	panel.setOutlineThickness(1);
	panel.setOutlineColor(Color(210, 218, 225));
	ventana.draw(panel);
	
	dibujarTexto(ventana, 350, 125, "DATOS DEL PASAJERO", 18, Color(25, 45, 65));
	
	string valores[4] = {identificacion, nombre, telefono, correo};
	string etiquetas[4] = {"Identificacion", "Nombre completo", "Telefono", "Correo electronico"};
	
	for (int i = 0; i < 4; i++)
	{
		float y = 155 + i * 60;
		dibujarTexto(ventana, 350, y, etiquetas[i], 14, Color(45, 55, 65));
		
		RectangleShape campo(Vector2f(380, 32));
		campo.setPosition(350, y + 20);
		campo.setFillColor(Color::White);
		campo.setOutlineThickness(campoActivo == i ? 2 : 1);
		campo.setOutlineColor(campoActivo == i ? Color(28, 91, 145) : Color(190, 200, 210));
		ventana.draw(campo);
		
		string texto = valores[i];
		if (campoActivo == i && cursorVisible)
			texto += "|";
		
		dibujarTexto(ventana, 360, y + 25, texto, 14, Color::Black);
	}
	
	RectangleShape casilla(Vector2f(20, 20));
	casilla.setPosition(350, 400);
	casilla.setFillColor(llevaMaleta ? Color(28, 145, 85) : Color::White);
	casilla.setOutlineThickness(2);
	casilla.setOutlineColor(Color(28, 91, 145));
	ventana.draw(casilla);
	
	if (llevaMaleta)
	{
		RectangleShape marca1(Vector2f(11, 3));
		marca1.setPosition(354, 411);
		marca1.setRotation(45);
		marca1.setFillColor(Color::White);
		ventana.draw(marca1);
		
		RectangleShape marca2(Vector2f(13, 3));
		marca2.setPosition(360, 410);
		marca2.setRotation(-45);
		marca2.setFillColor(Color::White);
		ventana.draw(marca2);
	}
	
	dibujarTexto(ventana, 380, 400, "Lleva maleta", 14, Color(45, 55, 65));
	
	Color colorBoton = editando ? Color(225, 125, 30) : Color(28, 91, 145);
	RectangleShape botonGuardar(Vector2f(165, 45));
	botonGuardar.setPosition(345, 445);
	botonGuardar.setFillColor(colorBoton);
	ventana.draw(botonGuardar);
	
	dibujarTexto(ventana, editando ? 375 : 390, 457, editando ? "GUARDAR CAMBIOS" : "REGISTRAR", 14, Color::White);
	
	RectangleShape botonEliminar(Vector2f(165, 45));
	botonEliminar.setPosition(525, 445);
	botonEliminar.setFillColor(Color(180, 55, 55));
	ventana.draw(botonEliminar);
	
	dibujarTexto(ventana, 575, 457, "ELIMINAR", 14, Color::White);
	
	dibujarTexto(ventana, 350, 505, "Seleccione un pasajero del listado para modificarlo", 12, Color(100, 110, 120));
}

void PantallaPasajeros::dibujarListado(RenderWindow& ventana)
{
	RectangleShape panel(Vector2f(680, 575));
	panel.setPosition(770, 115);
	panel.setFillColor(Color(245, 247, 250));
	panel.setOutlineThickness(1);
	panel.setOutlineColor(Color(210, 218, 225));
	ventana.draw(panel);
	
	dibujarTexto(ventana, 795, 125, "PASAJEROS REGISTRADOS", 18, Color(25, 45, 65));
	
	RectangleShape encabezado(Vector2f(650, 35));
	encabezado.setPosition(785, 165);
	encabezado.setFillColor(Color(28, 91, 145));
	ventana.draw(encabezado);
	
	dibujarTexto(ventana, 795, 173, "IDENTIFICACION", 12, Color::White);
	dibujarTexto(ventana, 925, 173, "NOMBRE", 12, Color::White);
	dibujarTexto(ventana, 1110, 173, "TELEFONO", 12, Color::White);
	dibujarTexto(ventana, 1250, 173, "CORREO", 12, Color::White);
	
	NodoPasajero* actual = listaPasajeros->getPrimero();
	int fila = 0;
	
	while (actual != nullptr && fila < 13)
	{
		float y = 200 + fila * 38;
		
		if (actual->pasajero.getIdentificacion() == identificacionSeleccionada)
		{
			RectangleShape seleccion(Vector2f(650, 36));
			seleccion.setPosition(785, y);
			seleccion.setFillColor(Color(215, 232, 245));
			ventana.draw(seleccion);
		}
		else if (fila % 2 == 0)
		{
			RectangleShape fondoFila(Vector2f(650, 36));
			fondoFila.setPosition(785, y);
			fondoFila.setFillColor(Color(235, 239, 243));
			ventana.draw(fondoFila);
		}
		
		dibujarTexto(ventana, 795, y + 8, actual->pasajero.getIdentificacion(), 11, Color::Black);
		dibujarTexto(ventana, 925, y + 8, actual->pasajero.getNombre(), 11, Color::Black);
		dibujarTexto(ventana, 1110, y + 8, actual->pasajero.getTelefono(), 11, Color::Black);
		dibujarTexto(ventana, 1250, y + 8, actual->pasajero.getCorreo(), 11, Color::Black);
		
		actual = actual->siguiente;
		fila++;
	}
	
	dibujarTexto(ventana, 795, 705, "Total de pasajeros: " + to_string(listaPasajeros->getCantidad()), 14, Color(45, 55, 65));
}

void PantallaPasajeros::dibujarConfirmacion(RenderWindow& ventana)
{
	RectangleShape fondo(Vector2f(1200, 800));
	fondo.setPosition(300, 0);
	fondo.setFillColor(Color(0, 0, 0, 100));
	ventana.draw(fondo);
	
	RectangleShape cuadro(Vector2f(420, 190));
	cuadro.setPosition(690, 330);
	cuadro.setFillColor(Color::White);
	cuadro.setOutlineThickness(2);
	cuadro.setOutlineColor(Color(28, 91, 145));
	ventana.draw(cuadro);
	
	dibujarTexto(ventana, 735, 365, "CONFIRMAR ELIMINACION", 20, Color(25, 45, 65));
	dibujarTexto(ventana, 735, 405, "Desea eliminar este pasajero?", 16, Color::Black);
	
	RectangleShape botonSi(Vector2f(100, 45));
	botonSi.setPosition(690, 430);
	botonSi.setFillColor(Color(180, 55, 55));
	ventana.draw(botonSi);
	dibujarTexto(ventana, 725, 442, "SI", 16, Color::White);
	
	RectangleShape botonNo(Vector2f(100, 45));
	botonNo.setPosition(810, 430);
	botonNo.setFillColor(Color(28, 91, 145));
	ventana.draw(botonNo);
	dibujarTexto(ventana, 842, 442, "NO", 16, Color::White);
}

void PantallaPasajeros::registrarPasajero()
{
	if (identificacion.empty() || nombre.empty() || telefono.empty() || correo.empty())
	{
		mensaje = "Complete todos los campos";
		return;
	}
	
	Pasajero nuevo(identificacion, nombre, telefono, correo);
	
	if (listaPasajeros->agregar(nuevo))
	{
		if (llevaMaleta)
		{
			string identificacionRegistrada = identificacion;
			gestorPantallas->solicitarCambioPantalla(new PantallaMaletas(listaMaletas, identificacionRegistrada));
			return;
		}
		
		mensaje = "Pasajero registrado correctamente";
		limpiarFormulario();
	}
	else
	{
		mensaje = "Ya existe un pasajero con esa identificacion";
	}
}

void PantallaPasajeros::seleccionarPasajero(string identificacion)
{
	NodoPasajero* encontrado = listaPasajeros->buscar(identificacion);
	
	if (encontrado == nullptr)
		return;
	
	identificacionSeleccionada = identificacion;
	this->identificacion = encontrado->pasajero.getIdentificacion();
	nombre = encontrado->pasajero.getNombre();
	telefono = encontrado->pasajero.getTelefono();
	correo = encontrado->pasajero.getCorreo();
	editando = true;
	llevaMaleta = false;
	campoActivo = 0;
	mensaje = "";
}

void PantallaPasajeros::modificarPasajero()
{
	if (!editando || identificacionSeleccionada.empty())
	{
		mensaje = "Seleccione un pasajero del listado";
		return;
	}
	
	if (identificacion.empty() || nombre.empty() || telefono.empty() || correo.empty())
	{
		mensaje = "Complete todos los campos";
		return;
	}
	
	if (identificacion != identificacionSeleccionada)
	{
		mensaje = "La identificacion no se puede cambiar";
		return;
	}
	
	if (listaPasajeros->modificar(identificacionSeleccionada, nombre, telefono, correo))
		mensaje = "Pasajero modificado correctamente";
	else
		mensaje = "No se pudo modificar el pasajero";
}

void PantallaPasajeros::eliminarPasajero()
{
	if (identificacionSeleccionada.empty())
	{
		mensaje = "Seleccione un pasajero para eliminar";
		return;
	}
	
	confirmarEliminar = true;
}

void PantallaPasajeros::limpiarFormulario()
{
	identificacion = "";
	nombre = "";
	telefono = "";
	correo = "";
	identificacionSeleccionada = "";
	campoActivo = 0;
	editando = false;
	llevaMaleta = false;
}

void PantallaPasajeros::dibujarTexto(RenderWindow& ventana, float x, float y, string texto, int tamano, Color color)
{
	if (fuenteCargada)
	{
		Text etiqueta;
		etiqueta.setFont(fuente);
		etiqueta.setString(texto);
		etiqueta.setCharacterSize(tamano);
		etiqueta.setFillColor(color);
		etiqueta.setPosition(x, y);
		ventana.draw(etiqueta);
	}
}
