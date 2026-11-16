#include "PantallaMaletas.h"
#include <sstream>

PantallaMaletas::PantallaMaletas(ListaMaletas* listaMaletas, string identificacionInicial)
{
	this->listaMaletas = listaMaletas;
	fuenteCargada = fuente.loadFromFile("resources/times.ttf");
	
	codigo = "";
	identificacionPasajero = identificacionInicial;
	identificacionBloqueada = !identificacionInicial.empty();
	peso = "";
	destino = "";
	estado = "Registrada";
	codigoSeleccionado = "";
	mensaje = "";
	campoActivo = 0;
	editando = false;
	confirmarEliminar = false;
	cursorVisible = true;
	tiempoCursor = 0;
}

void PantallaMaletas::procesarEventos(RenderWindow& ventana, Event evento)
{
	if (confirmarEliminar)
	{
		if (evento.type == Event::MouseButtonPressed && evento.mouseButton.button == Mouse::Left)
		{
			int x = evento.mouseButton.x;
			int y = evento.mouseButton.y;
			
			if (x >= 690 && x <= 790 && y >= 430 && y <= 475)
			{
				if (listaMaletas->eliminar(codigoSeleccionado))
					mensaje = "Maleta eliminada correctamente";
				else
					mensaje = "No se pudo eliminar la maleta";
				
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
			else if (y >= 205 && y <= 250 && !identificacionBloqueada)
				campoActivo = 1;
			else if (y >= 265 && y <= 310)
				campoActivo = 2;
			else if (y >= 325 && y <= 370)
				campoActivo = 3;
			else if (y >= 385 && y <= 430)
				campoActivo = 4;
		}
		
		if (x >= 345 && x <= 510 && y >= 445 && y <= 490)
		{
			if (editando)
				modificarMaleta();
			else
				registrarMaleta();
		}
		else if (x >= 525 && x <= 690 && y >= 445 && y <= 490)
		{
			eliminarMaleta();
		}
		
		if (x >= 785 && x <= 1450 && y >= 200 && y <= 690)
		{
			int indice = (y - 200) / 38;
			NodoMaleta* actual = listaMaletas->getPrimero();
			
			for (int i = 0; actual != nullptr && i < indice; i++)
				actual = actual->siguiente;
			
			if (actual != nullptr)
				seleccionarMaleta(actual->maleta.getCodigo());
		}
	}
	
	if (evento.type == Event::KeyPressed && evento.key.code == Keyboard::Tab)
	{
		campoActivo++;
		if (campoActivo > 4)
			campoActivo = 0;
		
		if (identificacionBloqueada && campoActivo == 1)
			campoActivo = 2;
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
			if (campoActivo == 0 && !codigo.empty())
				codigo.pop_back();
			else if (campoActivo == 1 && !identificacionBloqueada && !identificacionPasajero.empty())
				identificacionPasajero.pop_back();
			else if (campoActivo == 2 && !peso.empty())
				peso.pop_back();
			else if (campoActivo == 3 && !destino.empty())
				destino.pop_back();
			else if (campoActivo == 4 && !estado.empty())
				estado.pop_back();
		}
		else if (evento.text.unicode >= 32 && evento.text.unicode < 128)
		{
			char caracter = static_cast<char>(evento.text.unicode);
			
			if (campoActivo == 0 && codigo.size() < 25)
				codigo += caracter;
			else if (campoActivo == 1 && !identificacionBloqueada && identificacionPasajero.size() < 25)
				identificacionPasajero += caracter;
			else if (campoActivo == 2 && peso.size() < 10 &&
					 ((caracter >= '0' && caracter <= '9') || caracter == '.'))
				peso += caracter;
			else if (campoActivo == 3 && destino.size() < 45)
				destino += caracter;
			else if (campoActivo == 4 && estado.size() < 25)
				estado += caracter;
		}
	}
}

void PantallaMaletas::actualizar(float tiempo)
{
	tiempoCursor += tiempo;
	
	if (tiempoCursor >= 0.5f)
	{
		cursorVisible = !cursorVisible;
		tiempoCursor = 0;
	}
}

void PantallaMaletas::dibujar(RenderWindow& ventana)
{
	RectangleShape fondo(Vector2f(1200, 800));
	fondo.setPosition(300, 0);
	fondo.setFillColor(Color::White);
	ventana.draw(fondo);
	
	dibujarTexto(ventana, 335, 35, "GESTION DE MALETAS", 28, Color(20, 30, 40));
	dibujarTexto(ventana, 335, 75, "REGISTRO Y CONTROL DE EQUIPAJE", 14, Color(90, 100, 110));
	
	dibujarFormulario(ventana);
	dibujarListado(ventana);
	
	if (!mensaje.empty())
		dibujarTexto(ventana, 335, 755, mensaje, 16, Color(28, 91, 145));
	
	if (confirmarEliminar)
		dibujarConfirmacion(ventana);
}

void PantallaMaletas::dibujarFormulario(RenderWindow& ventana)
{
	RectangleShape panel(Vector2f(430, 450));
	panel.setPosition(325, 115);
	panel.setFillColor(Color(245, 247, 250));
	panel.setOutlineThickness(1);
	panel.setOutlineColor(Color(210, 218, 225));
	ventana.draw(panel);
	
	dibujarTexto(ventana, 350, 125, "DATOS DE LA MALETA", 18, Color(25, 45, 65));
	
	string valores[5] = {codigo, identificacionPasajero, peso, destino, estado};
	string etiquetas[5] = {"Codigo de maleta", "Identificacion del pasajero", "Peso en kilogramos", "Destino", "Estado"};
	
	for (int i = 0; i < 5; i++)
	{
		float y = 150 + i * 60;
		dibujarTexto(ventana, 350, y, etiquetas[i], 14, Color(45, 55, 65));
		
		RectangleShape campo(Vector2f(380, 32));
		campo.setPosition(350, y + 20);
		campo.setFillColor(Color::White);
		campo.setOutlineThickness(campoActivo == i ? 2 : 1);
		if (i == 1 && identificacionBloqueada)
		{
			campo.setFillColor(Color(230, 235, 240));
			campo.setOutlineColor(Color(160, 170, 180));
		}
		else
		{
			campo.setOutlineColor(campoActivo == i ? Color(28, 91, 145) : Color(190, 200, 210));
		}
		ventana.draw(campo);
		
		string texto = valores[i];
		if (campoActivo == i && cursorVisible)
			texto += "|";
		
		dibujarTexto(ventana, 360, y + 25, texto, 14, Color::Black);
	}
	
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
	
	dibujarTexto(ventana, 350, 505, "Seleccione una maleta del listado para modificarla", 12, Color(100, 110, 120));
}

void PantallaMaletas::dibujarListado(RenderWindow& ventana)
{
	RectangleShape panel(Vector2f(680, 575));
	panel.setPosition(770, 115);
	panel.setFillColor(Color(245, 247, 250));
	panel.setOutlineThickness(1);
	panel.setOutlineColor(Color(210, 218, 225));
	ventana.draw(panel);
	
	dibujarTexto(ventana, 795, 125, "MALETAS REGISTRADAS", 18, Color(25, 45, 65));
	
	RectangleShape encabezado(Vector2f(650, 35));
	encabezado.setPosition(785, 165);
	encabezado.setFillColor(Color(28, 91, 145));
	ventana.draw(encabezado);
	
	dibujarTexto(ventana, 795, 173, "CODIGO", 12, Color::White);
	dibujarTexto(ventana, 890, 173, "PASAJERO", 12, Color::White);
	dibujarTexto(ventana, 1040, 173, "PESO", 12, Color::White);
	dibujarTexto(ventana, 1120, 173, "DESTINO", 12, Color::White);
	dibujarTexto(ventana, 1290, 173, "ESTADO", 12, Color::White);
	
	NodoMaleta* actual = listaMaletas->getPrimero();
	int fila = 0;
	
	while (actual != nullptr && fila < 13)
	{
		float y = 200 + fila * 38;
		
		if (actual->maleta.getCodigo() == codigoSeleccionado)
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
		
		ostringstream pesoTexto;
		pesoTexto << actual->maleta.getPeso();
		
		dibujarTexto(ventana, 795, y + 8, actual->maleta.getCodigo(), 11, Color::Black);
		dibujarTexto(ventana, 890, y + 8, actual->maleta.getIdentificacionPasajero(), 11, Color::Black);
		dibujarTexto(ventana, 1040, y + 8, pesoTexto.str() + " kg", 11, Color::Black);
		dibujarTexto(ventana, 1120, y + 8, actual->maleta.getDestino(), 11, Color::Black);
		dibujarTexto(ventana, 1290, y + 8, actual->maleta.getEstado(), 11, Color::Black);
		
		actual = actual->siguiente;
		fila++;
	}
	
	dibujarTexto(ventana, 795, 705, "Total de maletas: " + to_string(listaMaletas->getCantidad()), 14, Color(45, 55, 65));
}

void PantallaMaletas::dibujarConfirmacion(RenderWindow& ventana)
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
	dibujarTexto(ventana, 735, 405, "Desea eliminar esta maleta?", 16, Color::Black);
	
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

void PantallaMaletas::registrarMaleta()
{
	if (codigo.empty() || identificacionPasajero.empty() || peso.empty() || destino.empty() || estado.empty())
	{
		mensaje = "Complete todos los campos";
		return;
	}
	
	try
	{
		float pesoNumerico = stof(peso);
		
		if (pesoNumerico <= 0)
		{
			mensaje = "El peso debe ser mayor que cero";
			return;
		}
		
		Maleta nueva(codigo, identificacionPasajero, pesoNumerico, destino, estado);
		
		if (listaMaletas->agregar(nueva))
		{
			mensaje = "Maleta registrada correctamente";
			limpiarFormulario();
		}
		else
		{
			mensaje = "Ya existe una maleta con ese codigo";
		}
	}
	catch (...)
	{
		mensaje = "Ingrese un peso valido";
	}
}

void PantallaMaletas::seleccionarMaleta(string codigo)
{
	NodoMaleta* encontrada = listaMaletas->buscar(codigo);
	
	if (encontrada == nullptr)
		return;
	
	codigoSeleccionado = codigo;
	this->codigo = encontrada->maleta.getCodigo();
	identificacionPasajero = encontrada->maleta.getIdentificacionPasajero();
	
	ostringstream pesoTexto;
	pesoTexto << encontrada->maleta.getPeso();
	peso = pesoTexto.str();
	
	destino = encontrada->maleta.getDestino();
	estado = encontrada->maleta.getEstado();
	editando = true;
	campoActivo = 0;
	mensaje = "";
}

void PantallaMaletas::modificarMaleta()
{
	if (!editando || codigoSeleccionado.empty())
	{
		mensaje = "Seleccione una maleta del listado";
		return;
	}
	
	if (codigo.empty() || identificacionPasajero.empty() || peso.empty() || destino.empty() || estado.empty())
	{
		mensaje = "Complete todos los campos";
		return;
	}
	
	if (codigo != codigoSeleccionado)
	{
		mensaje = "El codigo de la maleta no se puede cambiar";
		return;
	}
	
	try
	{
		float pesoNumerico = stof(peso);
		
		if (pesoNumerico <= 0)
		{
			mensaje = "El peso debe ser mayor que cero";
			return;
		}
		
		if (listaMaletas->modificar(codigoSeleccionado, identificacionPasajero, pesoNumerico, destino, estado))
			mensaje = "Maleta modificada correctamente";
		else
			mensaje = "No se pudo modificar la maleta";
	}
	catch (...)
	{
		mensaje = "Ingrese un peso valido";
	}
}

void PantallaMaletas::eliminarMaleta()
{
	if (codigoSeleccionado.empty())
	{
		mensaje = "Seleccione una maleta para eliminar";
		return;
	}
	
	confirmarEliminar = true;
}

void PantallaMaletas::limpiarFormulario()
{
	codigo = "";
	if (!identificacionBloqueada)
		identificacionPasajero = "";
	peso = "";
	destino = "";
	estado = "Registrada";
	codigoSeleccionado = "";
	campoActivo = 0;
	editando = false;
}

void PantallaMaletas::dibujarTexto(RenderWindow& ventana, float x, float y, string texto, int tamano, Color color)
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
