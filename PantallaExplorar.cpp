#include "PantallaExplorar.h"
#include <cctype>
#include <sstream>
#include <iomanip>

PantallaExplorar::PantallaExplorar(Grafo* grafo)
{
	this->grafo = grafo;
	fuenteCargada = fuente.loadFromFile("resources/times.ttf");
	textoBusqueda = "";
	codigoCiudadSeleccionada = "";
	desplazamientoCiudades = 0;
	
	if (grafo != nullptr && grafo->getPrimero() != nullptr)
		codigoCiudadSeleccionada = grafo->getPrimero()->ciudad.getCodigo();
}

void PantallaExplorar::dibujarTexto(RenderWindow& ventana, float x, float y, const string& texto, unsigned int tamano, Color color, bool negrita)
{
	if (!fuenteCargada)
		return;
	
	Text etiqueta;
	etiqueta.setFont(fuente);
	etiqueta.setString(texto);
	etiqueta.setCharacterSize(tamano);
	etiqueta.setFillColor(color);
	if (negrita)
		etiqueta.setStyle(Text::Bold);
	etiqueta.setPosition(x, y);
	ventana.draw(etiqueta);
}

void PantallaExplorar::dibujarPanel(RenderWindow& ventana, float x, float y, float ancho, float alto, Color color)
{
	RectangleShape panel(Vector2f(ancho, alto));
	panel.setPosition(x, y);
	panel.setFillColor(color);
	panel.setOutlineThickness(1);
	panel.setOutlineColor(Color(215, 225, 220));
	ventana.draw(panel);
}

void PantallaExplorar::seleccionarPrimeraCiudadVisible()
{
	if (grafo == nullptr || grafo->getPrimero() == nullptr)
	{
		codigoCiudadSeleccionada = "";
		return;
	}
	
	codigoCiudadSeleccionada = grafo->getPrimero()->ciudad.getCodigo();
}

void PantallaExplorar::procesarEventos(RenderWindow& ventana, Event evento)
{
	if (evento.type == Event::TextEntered)
	{
		if (evento.text.unicode == 8)
		{
			if (!textoBusqueda.empty())
				textoBusqueda.erase(textoBusqueda.size() - 1);
			desplazamientoCiudades = 0;
		}
		else if (evento.text.unicode >= 32 && evento.text.unicode < 127)
		{
			if (textoBusqueda.size() < 35)
				textoBusqueda += static_cast<char>(evento.text.unicode);
			desplazamientoCiudades = 0;
		}
		return;
	}
	
	if (evento.type == Event::MouseWheelScrolled)
	{
		Vector2i mouse = Mouse::getPosition(ventana);
		if (mouse.x >= 320 && mouse.x <= 675 &&
			mouse.y >= 300 && mouse.y <= 735 && grafo != nullptr)
		{
			int total = grafo->getCantidadVertices();
			int maximo = total > 9 ? total - 9 : 0;
			
			if (evento.mouseWheelScroll.delta < 0 && desplazamientoCiudades < maximo)
				desplazamientoCiudades++;
			else if (evento.mouseWheelScroll.delta > 0 && desplazamientoCiudades > 0)
				desplazamientoCiudades--;
		}
		return;
	}
	
	if (evento.type == Event::MouseButtonPressed &&
		evento.mouseButton.button == Mouse::Left)
	{
		int x = evento.mouseButton.x;
		int y = evento.mouseButton.y;
		
		if (x >= 320 && x <= 675 && y >= 300 && y <= 735 && grafo != nullptr)
		{
			int fila = (y - 300) / 48;
			int indiceObjetivo = desplazamientoCiudades + fila;
			
			NodoVertice* actual = grafo->getPrimero();
			int indice = 0;
			
			while (actual != nullptr && indice < indiceObjetivo)
			{
				actual = actual->siguiente;
				indice++;
			}
			
			if (actual != nullptr)
				codigoCiudadSeleccionada = actual->ciudad.getCodigo();
		}
	}
}

void PantallaExplorar::actualizar(float tiempo)
{
}

void PantallaExplorar::dibujar(RenderWindow& ventana)
{
	Color verdeOscuro(10, 76, 55);
	Color verde(42, 112, 72);
	Color verdeClaro(225, 242, 224);
	Color textoOscuro(35, 65, 52);
	Color gris(105, 115, 108);
	Color blanco(255, 255, 255);
	
	RectangleShape fondo(Vector2f(1200, 800));
	fondo.setPosition(300, 0);
	fondo.setFillColor(Color(247, 250, 247));
	ventana.draw(fondo);
	
	dibujarPanel(ventana, 320, 22, 1155, 130, verdeOscuro);
	dibujarTexto(ventana, 355, 48, "EXPLORAR DESTINOS", 34, blanco, true);
	dibujarTexto(ventana, 357, 96, "Conoce las ciudades disponibles y sus conexiones", 19, Color(230, 245, 235));
	dibujarPanel(ventana, 320, 175, 420, 55, blanco);
	dibujarTexto(ventana, 338, 190, "Buscar ciudad:", 17, textoOscuro, true);
	dibujarTexto(ventana, 465, 190, textoBusqueda.empty() ? "Escribe el nombre..." : textoBusqueda, 17, textoBusqueda.empty() ? gris : textoOscuro);
	
	int totalCiudades = grafo != nullptr ? grafo->getCantidadVertices() : 0;
	int totalVuelos = 0;
	
	if (grafo != nullptr)
	{
		NodoVertice* v = grafo->getPrimero();
		while (v != nullptr)
		{
			NodoArista* a = v->conexiones;
			while (a != nullptr)
			{
				totalVuelos++;
				a = a->siguiente;
			}
			v = v->siguiente;
		}
	}
	
	dibujarPanel(ventana, 765, 175, 220, 55, verdeClaro);
	dibujarTexto(ventana, 785, 184, to_string(totalCiudades), 25, verdeOscuro, true);
	dibujarTexto(ventana, 850, 192, "Ciudades", 17, textoOscuro);	
	dibujarPanel(ventana, 1000, 175, 220, 55, verdeClaro);
	dibujarTexto(ventana, 1020, 184, to_string(totalVuelos), 25, verdeOscuro, true);
	dibujarTexto(ventana, 1085, 192, "Conexiones", 17, textoOscuro);
	dibujarPanel(ventana, 320, 250, 355, 500, blanco);
	dibujarTexto(ventana, 340, 267, "DESTINOS DISPONIBLES", 21, verdeOscuro, true);
	
	if (grafo == nullptr || totalCiudades == 0)
	{
		dibujarTexto(ventana, 345, 340, "Todavia no hay ciudades registradas.", 16, gris);
		dibujarTexto(ventana, 345, 370, "Registra ciudades desde Administracion.", 15, gris);
	}
	else
	{
		string filtro = textoBusqueda;
		for (size_t i = 0; i < filtro.size(); i++)
			filtro[i] = static_cast<char>(tolower(static_cast<unsigned char>(filtro[i])));
		
		NodoVertice* actual = grafo->getPrimero();
		int indice = 0;
		int filaVisible = 0;
		int coincidencias = 0;
		
		while (actual != nullptr)
		{
			string nombre = actual->ciudad.getNombre();
			string codigo = actual->ciudad.getCodigo();
			string nombreMinuscula = nombre;
			
			for (size_t i = 0; i < nombreMinuscula.size(); i++)
				nombreMinuscula[i] = static_cast<char>(tolower(static_cast<unsigned char>(nombreMinuscula[i])));
			
			bool coincide = filtro.empty() ||
				nombreMinuscula.find(filtro) != string::npos ||
				codigo.find(textoBusqueda) != string::npos;
			
			if (coincide)
			{
				if (coincidencias >= desplazamientoCiudades && filaVisible < 9)
				{
					float y = 305.0f + filaVisible * 48.0f;
					bool seleccionado = codigo == codigoCiudadSeleccionada;
					
					RectangleShape fila(Vector2f(315, 42));
					fila.setPosition(338, y);
					fila.setFillColor(seleccionado ? Color(218, 242, 205) : Color(250, 252, 250));
					fila.setOutlineThickness(1);
					fila.setOutlineColor(Color(225, 232, 226));
					ventana.draw(fila);
					
					dibujarTexto(ventana, 352, y + 8, nombre, 17,
								 seleccionado ? verdeOscuro : textoOscuro, seleccionado);
					dibujarTexto(ventana, 585, y + 10, codigo, 13, gris);
					
					filaVisible++;
				}
				coincidencias++;
			}
			
			actual = actual->siguiente;
			indice++;
		}
		
		if (coincidencias == 0)
			dibujarTexto(ventana, 345, 330, "No se encontraron ciudades.", 16, gris);
		
		int maximo = coincidencias > 9 ? coincidencias - 9 : 0;
		if (desplazamientoCiudades > maximo)
			desplazamientoCiudades = maximo;
	}
	
	dibujarPanel(ventana, 695, 250, 780, 500, blanco);
	
	NodoVertice* seleccionada = nullptr;
	if (grafo != nullptr && !codigoCiudadSeleccionada.empty())
		seleccionada = grafo->buscarCiudad(codigoCiudadSeleccionada);
	
	if (seleccionada == nullptr)
	{
		dibujarTexto(ventana, 725, 285, "Selecciona una ciudad", 26, verdeOscuro, true);
		dibujarTexto(ventana, 725, 330,
					 "Aqui apareceran sus conexiones y vuelos disponibles.", 17, gris);
		return;
	}
	
	dibujarTexto(ventana, 725, 270, seleccionada->ciudad.getNombre(), 29, verdeOscuro, true);
	dibujarTexto(ventana, 725, 310, "Codigo: " + seleccionada->ciudad.getCodigo(), 16, gris);
	
	ostringstream coordenadas;
	coordenadas << fixed << setprecision(1) << "Coordenadas: X " << seleccionada->ciudad.getPosicionX() << "   Y " << seleccionada->ciudad.getPosicionY();
	dibujarTexto(ventana, 725, 338, coordenadas.str(), 15, gris);
	dibujarTexto(ventana, 725, 390, "CONEXIONES DESDE ESTA CIUDAD", 21, verdeOscuro, true);
	
	RectangleShape encabezado(Vector2f(720, 38));
	encabezado.setPosition(720, 430);
	encabezado.setFillColor(verdeOscuro);
	ventana.draw(encabezado);
	
	dibujarTexto(ventana, 735, 438, "Destino", 15, blanco, true);
	dibujarTexto(ventana, 945, 438, "Costo", 15, blanco, true);
	dibujarTexto(ventana, 1090, 438, "Duracion", 15, blanco, true);
	dibujarTexto(ventana, 1260, 438, "Vuelo", 15, blanco, true);
	
	NodoArista* arista = seleccionada->conexiones;
	int fila = 0;
	
	while (arista != nullptr && fila < 6)
	{
		float y = 470.0f + fila * 42.0f;
		
		RectangleShape fondoFila(Vector2f(720, 40));
		fondoFila.setPosition(720, y);
		fondoFila.setFillColor(fila % 2 == 0 ? Color(246, 250, 246) : blanco);
		fondoFila.setOutlineThickness(1);
		fondoFila.setOutlineColor(Color(225, 232, 226));
		ventana.draw(fondoFila);
		
		NodoVertice* destino = grafo->buscarCiudad(arista->vuelo.getCodigoDestino());
		string nombreDestino = destino != nullptr ?
			destino->ciudad.getNombre() : arista->vuelo.getCodigoDestino();
		
		ostringstream costo;
		costo << fixed << setprecision(0) << arista->vuelo.getCosto();
		
		ostringstream duracion;
		duracion << fixed << setprecision(0) << arista->vuelo.getDuracion() << " h";
		
		dibujarTexto(ventana, 735, y + 9, nombreDestino, 15, textoOscuro);
		dibujarTexto(ventana, 945, y + 9, costo.str(), 15, textoOscuro);
		dibujarTexto(ventana, 1090, y + 9, duracion.str(), 15, textoOscuro);
		dibujarTexto(ventana, 1260, y + 9, arista->vuelo.getCodigo(), 15, textoOscuro);
		
		arista = arista->siguiente;
		fila++;
	}
	
	if (fila == 0)
		dibujarTexto(ventana, 740, 490, "Esta ciudad no tiene vuelos de salida registrados.", 16, gris);
}
