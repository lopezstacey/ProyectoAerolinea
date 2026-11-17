#include "PantallaPasajeros.h"
#include "PantallaMaletas.h"

static const float DLG_ANCHO = 420;
static const float DLG_ALTO = 210;
static const float DLG_X = 690;                         
static const float DLG_Y = 300;
static const float BTN_ANCHO = 100;
static const float BTN_ALTO = 45;
static const float BTN_Y = DLG_Y + 140;
static const float BTN_SI_X = DLG_X + DLG_ANCHO / 2 - BTN_ANCHO - 10;
static const float BTN_NO_X = DLG_X + DLG_ANCHO / 2 + 10;static const float FORM_BTN_Y = 445;
static const float FORM_BTN_ALTO = 45;
static const float FORM_BTN_ED_ANCHO = 120;
static const float FORM_BTN_GUARDAR_X = 345;
static const float FORM_BTN_ELIMINAR_X = 480;
static const float FORM_BTN_NUEVO_X = 615;

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

static void dibujarTextoCentrado(RenderWindow& ventana, const Font& fuente, const string& texto, float cx, float cy, int tamano, Color color)
{
	Text t;
	t.setFont(fuente);
	t.setString(texto);
	t.setCharacterSize(tamano);
	t.setFillColor(color);
	FloatRect b = t.getLocalBounds();
	t.setPosition(cx - b.width / 2 - b.left, cy - b.height / 2 - b.top);
	ventana.draw(t);
}

static int contarMaletas(ListaMaletas* lista, const string& identificacion)
{
	int total = 0;
	
	for (NodoMaleta* actual = lista->getPrimero(); actual != nullptr; actual = actual->siguiente)
		if (actual->maleta.getIdentificacionPasajero() == identificacion)
			total++;
	
	return total;
}

PantallaPasajeros::PantallaPasajeros(ListaPasajeros* listaPasajeros, ListaMaletas* listaMaletas, GestorPantallas* gestorPantallas, Grafo* grafo)
{
	this->listaPasajeros = listaPasajeros;
	this->listaMaletas = listaMaletas;
	this->gestorPantallas = gestorPantallas;
	this->grafo = grafo;
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
	desplazamientoLista = 0;
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
			
			if (x >= BTN_SI_X && x <= BTN_SI_X + BTN_ANCHO && y >= BTN_Y && y <= BTN_Y + BTN_ALTO)
			{
				if (listaPasajeros->eliminar(identificacionSeleccionada))
					mensaje = "Pasajero eliminado correctamente";
				else
					mensaje = "No se pudo eliminar el pasajero";
				
				limpiarFormulario();
				confirmarEliminar = false;
			}
			else if (x >= BTN_NO_X && x <= BTN_NO_X + BTN_ANCHO && y >= BTN_Y && y <= BTN_Y + BTN_ALTO)
			{
				confirmarEliminar = false;
			}
		}
		if (evento.type == Event::KeyPressed && evento.key.code == Keyboard::Escape)
			confirmarEliminar = false;
		return;
	}

	if (evento.type == Event::MouseWheelScrolled)
	{
		int x = evento.mouseWheelScroll.x;
		int y = evento.mouseWheelScroll.y;
		
		if (x >= 785 && x <= 1450 && y >= 200 && y < 694)
		{
			int maximo = listaPasajeros->getCantidad() - 13;
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
		
		if (x >= 350 && x <= 560 && y >= 400 && y <= 430)
		{
			llevaMaleta = !llevaMaleta;
		}
		
		if (editando)
		{
			bool enFila = y >= FORM_BTN_Y && y <= FORM_BTN_Y + FORM_BTN_ALTO;
			
			if (enFila && x >= FORM_BTN_GUARDAR_X && x <= FORM_BTN_GUARDAR_X + FORM_BTN_ED_ANCHO)
			{
				modificarPasajero();
			}
			else if (enFila && x >= FORM_BTN_ELIMINAR_X && x <= FORM_BTN_ELIMINAR_X + FORM_BTN_ED_ANCHO)
			{
				eliminarPasajero();
			}
			else if (enFila && x >= FORM_BTN_NUEVO_X && x <= FORM_BTN_NUEVO_X + FORM_BTN_ED_ANCHO)
			{
				limpiarFormulario();
				mensaje = "Formulario limpio. Puede registrar un nuevo pasajero.";
			}
		}
		else
		{
			if (x >= 345 && x <= 510 && y >= 445 && y <= 490)
				registrarPasajero();
			else if (x >= 525 && x <= 690 && y >= 445 && y <= 490)
				eliminarPasajero();
		}

		if (x >= 785 && x <= 1435 && y >= 200 && y < 694)
		{
			int indice = desplazamientoLista + (y - 200) / 38;
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
	ventana.setView(ventana.getDefaultView());
	
	RectangleShape fondo(Vector2f(1200, 800));
	fondo.setPosition(300, 0);
	fondo.setFillColor(Color::White);
	ventana.draw(fondo);
	
	dibujarTexto(ventana, 335, 35, "GESTION DE PASAJEROS", 28, Color(55, 70, 60));
	dibujarTexto(ventana, 335, 75, "REGISTRO Y ADMINISTRACION DE PASAJEROS", 14, Color(120, 125, 120));
	
	dibujarFormulario(ventana);
	dibujarListado(ventana);
	
	if (!mensaje.empty())
		dibujarTexto(ventana, 335, 755, mensaje, 16, Color(90, 120, 95));
	
	if (confirmarEliminar)
		dibujarConfirmacion(ventana);
}

void PantallaPasajeros::dibujarFormulario(RenderWindow& ventana)
{
	RectangleShape panel(Vector2f(430, 450));
	panel.setPosition(325, 115);
	panel.setFillColor(Color(248, 249, 247));
	panel.setOutlineThickness(1);
	panel.setOutlineColor(Color(215, 220, 215));
	ventana.draw(panel);
	
	RectangleShape encabezado(Vector2f(430, 40));
	encabezado.setPosition(325, 115);
	encabezado.setFillColor(Color(90, 120, 95));
	ventana.draw(encabezado);
	dibujarTexto(ventana, 342, 125, "DATOS DEL PASAJERO", 17, Color::White);
	
	string valores[4] = {identificacion, nombre, telefono, correo};
	string etiquetas[4] = {"Identificacion", "Nombre completo", "Telefono", "Correo electronico"};
	
	for (int i = 0; i < 4; i++)
	{
		float y = 155 + i * 60;
		dibujarTexto(ventana, 350, y, etiquetas[i], 14, Color(75, 80, 75));
		
		RectangleShape campo(Vector2f(380, 32));
		campo.setPosition(350, y + 20);
		campo.setFillColor(Color::White);
		campo.setOutlineThickness(campoActivo == i ? 2 : 1);
		campo.setOutlineColor(campoActivo == i ? Color(90, 120, 95) : Color(210, 215, 210));
		ventana.draw(campo);
		
		string texto = valores[i];
		if (campoActivo == i && cursorVisible)
			texto += "|";
		
		dibujarTexto(ventana, 360, y + 25, texto, 14, Color::Black);
	}
	
	RectangleShape casilla(Vector2f(20, 20));
	casilla.setPosition(350, 400);
	casilla.setFillColor(llevaMaleta ? Color(90, 120, 95) : Color::White);
	casilla.setOutlineThickness(2);
	casilla.setOutlineColor(Color(90, 120, 95));
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
	
	dibujarTexto(ventana, 380, 400, editando ? "Agregar maleta a este pasajero" : "Lleva maleta", 14, Color(75, 80, 75));
	
	if (editando)
	{
		RectangleShape botonGuardar(Vector2f(FORM_BTN_ED_ANCHO, FORM_BTN_ALTO));
		botonGuardar.setPosition(FORM_BTN_GUARDAR_X, FORM_BTN_Y);
		botonGuardar.setFillColor(Color(90, 120, 95));
		ventana.draw(botonGuardar);
		
		RectangleShape botonEliminar(Vector2f(FORM_BTN_ED_ANCHO, FORM_BTN_ALTO));
		botonEliminar.setPosition(FORM_BTN_ELIMINAR_X, FORM_BTN_Y);
		botonEliminar.setFillColor(Color(165, 85, 80));
		ventana.draw(botonEliminar);
		
		RectangleShape botonNuevo(Vector2f(FORM_BTN_ED_ANCHO, FORM_BTN_ALTO));
		botonNuevo.setPosition(FORM_BTN_NUEVO_X, FORM_BTN_Y);
		botonNuevo.setFillColor(Color(120, 130, 120));
		ventana.draw(botonNuevo);
		
		if (fuenteCargada)
		{
			float cy = FORM_BTN_Y + FORM_BTN_ALTO / 2;
			dibujarTextoCentrado(ventana, fuente, "GUARDAR", FORM_BTN_GUARDAR_X + FORM_BTN_ED_ANCHO / 2, cy, 13, Color::White);
			dibujarTextoCentrado(ventana, fuente, "ELIMINAR", FORM_BTN_ELIMINAR_X + FORM_BTN_ED_ANCHO / 2, cy, 13, Color::White);
			dibujarTextoCentrado(ventana, fuente, "NUEVO", FORM_BTN_NUEVO_X + FORM_BTN_ED_ANCHO / 2, cy, 13, Color::White);
		}
		
		int totalMaletas = contarMaletas(listaMaletas, identificacionSeleccionada);
		dibujarTexto(ventana, 350, 505, "Maletas de este pasajero: " + to_string(totalMaletas), 13, Color(75, 80, 75));
		
		if (llevaMaleta)
			dibujarTexto(ventana, 350, 528, "Al guardar se abrira Gestion de Maletas.", 12, Color(90, 120, 95));
	}
	else
	{
		RectangleShape botonGuardar(Vector2f(165, 45));
		botonGuardar.setPosition(345, 445);
		botonGuardar.setFillColor(Color(90, 120, 95));
		ventana.draw(botonGuardar);
		
		dibujarTexto(ventana, 390, 457, "REGISTRAR", 13, Color::White);
		
		RectangleShape botonEliminar(Vector2f(165, 45));
		botonEliminar.setPosition(525, 445);
		botonEliminar.setFillColor(Color(165, 85, 80));
		ventana.draw(botonEliminar);
		
		dibujarTexto(ventana, 575, 457, "ELIMINAR", 14, Color::White);
		
		dibujarTexto(ventana, 350, 505, "Seleccione un pasajero del listado para modificarlo", 12, Color(120, 125, 120));
	}
}

void PantallaPasajeros::dibujarListado(RenderWindow& ventana)
{
	RectangleShape panel(Vector2f(680, 620));
	panel.setPosition(770, 115);
	panel.setFillColor(Color(248, 249, 247));
	panel.setOutlineThickness(1);
	panel.setOutlineColor(Color(215, 220, 215));
	ventana.draw(panel);
	
	RectangleShape encabezado(Vector2f(680, 40));
	encabezado.setPosition(770, 115);
	encabezado.setFillColor(Color(90, 120, 95));
	ventana.draw(encabezado);
	dibujarTexto(ventana, 790, 125, "PASAJEROS REGISTRADOS", 17, Color::White);
	
	RectangleShape encabezadoColumnas(Vector2f(650, 35));
	encabezadoColumnas.setPosition(785, 165);
	encabezadoColumnas.setFillColor(Color(90, 120, 95));
	ventana.draw(encabezadoColumnas);
	
	dibujarTexto(ventana, 795, 173, "IDENTIFICACION", 12, Color::White);
	dibujarTexto(ventana, 925, 173, "NOMBRE", 12, Color::White);
	dibujarTexto(ventana, 1110, 173, "TELEFONO", 12, Color::White);
	dibujarTexto(ventana, 1250, 173, "CORREO", 12, Color::White);
	
	int totalPasajeros = listaPasajeros->getCantidad();
	const int filasVisibles = 13;
	int maximoDesplazamiento = totalPasajeros - filasVisibles;
	if (maximoDesplazamiento < 0)
		maximoDesplazamiento = 0;
	if (desplazamientoLista > maximoDesplazamiento)
		desplazamientoLista = maximoDesplazamiento;
	
	NodoPasajero* actual = listaPasajeros->getPrimero();
	for (int i = 0; actual != nullptr && i < desplazamientoLista; i++)
		actual = actual->siguiente;
	
	int fila = 0;
	while (actual != nullptr && fila < filasVisibles)
	{
		float y = 200 + fila * 38;
		
		if (actual->pasajero.getIdentificacion() == identificacionSeleccionada)
		{
			RectangleShape seleccion(Vector2f(650, 36));
			seleccion.setPosition(785, y);
			seleccion.setFillColor(Color(225, 237, 225));
			ventana.draw(seleccion);
		}
		else if (fila % 2 == 0)
		{
			RectangleShape fondoFila(Vector2f(650, 36));
			fondoFila.setPosition(785, y);
			fondoFila.setFillColor(Color(242, 245, 241));
			ventana.draw(fondoFila);
		}
		
		dibujarTexto(ventana, 795, y + 8, recortarTexto(fuente, actual->pasajero.getIdentificacion(), 11, 120), 11, Color::Black);
		dibujarTexto(ventana, 925, y + 8, recortarTexto(fuente, actual->pasajero.getNombre(), 11, 170), 11, Color::Black);
		dibujarTexto(ventana, 1110, y + 8, recortarTexto(fuente, actual->pasajero.getTelefono(), 11, 125), 11, Color::Black);
		dibujarTexto(ventana, 1250, y + 8, recortarTexto(fuente, actual->pasajero.getCorreo(), 11, 175), 11, Color::Black);
		
		actual = actual->siguiente;
		fila++;
	}
	
	RectangleShape pista(Vector2f(6, 494));
	pista.setPosition(1440, 200);
	pista.setFillColor(Color(225, 229, 224));
	ventana.draw(pista);
	
	float altoIndicador = 494.0f;
	if (totalPasajeros > filasVisibles)
	{
		altoIndicador = 494.0f * filasVisibles / totalPasajeros;
		if (altoIndicador < 35.0f)
			altoIndicador = 35.0f;
	}
	
	float recorrido = 494.0f - altoIndicador;
	float posicionIndicador = 200.0f;
	if (maximoDesplazamiento > 0)
		posicionIndicador += recorrido * desplazamientoLista / maximoDesplazamiento;
	
	RectangleShape indicador(Vector2f(6, altoIndicador));
	indicador.setPosition(1440, posicionIndicador);
	indicador.setFillColor(Color(90, 120, 95));
	ventana.draw(indicador);
	
	dibujarTexto(ventana, 795, 705, "Total de pasajeros: " + to_string(listaPasajeros->getCantidad()), 14, Color(75, 80, 75));
}

void PantallaPasajeros::dibujarConfirmacion(RenderWindow& ventana)
{
	RectangleShape fondo(Vector2f(1200, 800));
	fondo.setPosition(300, 0);
	fondo.setFillColor(Color(0, 0, 0, 100));
	ventana.draw(fondo);
	
	RectangleShape cuadro(Vector2f(DLG_ANCHO, DLG_ALTO));
	cuadro.setPosition(DLG_X, DLG_Y);
	cuadro.setFillColor(Color::White);
	cuadro.setOutlineThickness(2);
	cuadro.setOutlineColor(Color(90, 120, 95));
	ventana.draw(cuadro);
	
	RectangleShape franja(Vector2f(DLG_ANCHO, 6));
	franja.setPosition(DLG_X, DLG_Y);
	franja.setFillColor(Color(165, 85, 80));
	ventana.draw(franja);
	
	float centroX = DLG_X + DLG_ANCHO / 2;
	
	if (fuenteCargada)
	{
		dibujarTextoCentrado(ventana, fuente, "CONFIRMAR ELIMINACION", centroX, DLG_Y + 40, 20, Color(55, 70, 60));
		dibujarTextoCentrado(ventana, fuente, "Desea eliminar este pasajero?", centroX, DLG_Y + 82, 16, Color::Black);
		dibujarTextoCentrado(ventana, fuente, "Esta accion no se puede deshacer.", centroX, DLG_Y + 108, 13, Color(120, 125, 120));
	}
	
	RectangleShape botonSi(Vector2f(BTN_ANCHO, BTN_ALTO));
	botonSi.setPosition(BTN_SI_X, BTN_Y);
	botonSi.setFillColor(Color(165, 85, 80));
	ventana.draw(botonSi);
	
	RectangleShape botonNo(Vector2f(BTN_ANCHO, BTN_ALTO));
	botonNo.setPosition(BTN_NO_X, BTN_Y);
	botonNo.setFillColor(Color(90, 120, 95));
	ventana.draw(botonNo);
	
	if (fuenteCargada)
	{
		dibujarTextoCentrado(ventana, fuente, "SI", BTN_SI_X + BTN_ANCHO / 2, BTN_Y + BTN_ALTO / 2, 16, Color::White);
		dibujarTextoCentrado(ventana, fuente, "NO", BTN_NO_X + BTN_ANCHO / 2, BTN_Y + BTN_ALTO / 2, 16, Color::White);
	}
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
			gestorPantallas->solicitarCambioPantalla(new PantallaMaletas(listaMaletas, grafo, identificacionRegistrada));
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
	{
		if (llevaMaleta)
		{
			string identificacionModificada = identificacionSeleccionada;
			gestorPantallas->solicitarCambioPantalla(new PantallaMaletas(listaMaletas, grafo, identificacionModificada));
			return;
		}
		
		mensaje = "Pasajero modificado correctamente";
	}
	else
	{
		mensaje = "No se pudo modificar el pasajero";
	}
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
