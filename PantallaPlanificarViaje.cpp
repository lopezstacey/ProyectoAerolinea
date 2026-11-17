#include "PantallaPlanificarViaje.h"
#include <sstream>
#include <iomanip>

namespace
{
	const float X0 = 320;
	const float ENC_Y = 22;
	const float ENC_ALTO = 110;
	const float ENC_ANCHO = 1160;
	
	const float PY = 150;
	const float PH = 500;
	
	const float PAS_X = 320;
	const float PAS_ANCHO = 565;
	const float VUE_X = 915;
	const float VUE_ANCHO = 565;
	
	const float LISTA_Y0 = PY + 100;
	const float FILA_PASO = 46;
	const float FILA_ALTO = 40;
	const int FILAS_VISIBLES = 8;
	const float LISTA_ANCHO = 525;
	const float PAS_LISTA_X = 340;
	const float VUE_LISTA_X = 935;
	
	const float MSG_Y = 665;
	const float MSG_ALTO = 36;
	const float BTN_Y = 711;
	const float BTN_ALTO = 50;
	const float CONF_X = 915;
	const float CONF_ANCHO = 380;
	const float LIMP_X = 1315;
	const float LIMP_ANCHO = 165;
	
	bool dentro(int x, int y, float rx, float ry, float rw, float rh)
	{
		return x >= rx && x <= rx + rw && y >= ry && y <= ry + rh;
	}
	
	float anchoTexto(const Font& fuente, const string& texto, unsigned int tamano, bool negrita)
	{
		Text t;
		t.setFont(fuente);
		t.setString(texto);
		t.setCharacterSize(tamano);
		if (negrita)
			t.setStyle(Text::Bold);
		return t.getLocalBounds().width;
	}
	
	string recortar(const Font& fuente, bool cargada, const string& texto, unsigned int tamano, bool negrita, float maxAncho)
	{
		if (!cargada || anchoTexto(fuente, texto, tamano, negrita) <= maxAncho)
			return texto;
		
		string recortado = texto;
		while (!recortado.empty())
		{
			recortado.erase(recortado.size() - 1);
			if (anchoTexto(fuente, recortado + "...", tamano, negrita) <= maxAncho)
				break;
		}
		return recortado + "...";
	}
	
	void caja(RenderWindow& ventana, float x, float y, float ancho, float alto, Color relleno, Color borde, float grosor)
	{
		RectangleShape r(Vector2f(ancho, alto));
		r.setPosition(x, y);
		r.setFillColor(relleno);
		r.setOutlineThickness(grosor);
		r.setOutlineColor(borde);
		ventana.draw(r);
	}
	
	void textoCentrado(RenderWindow& ventana, const Font& fuente, bool cargada, const string& texto, float cx, float cy, unsigned int tamano, Color color, bool negrita)
	{
		if (!cargada)
			return;
		
		Text t;
		t.setFont(fuente);
		t.setString(texto);
		t.setCharacterSize(tamano);
		t.setFillColor(color);
		if (negrita)
			t.setStyle(Text::Bold);
		FloatRect b = t.getLocalBounds();
		t.setPosition(cx - b.width / 2 - b.left, cy - b.height / 2 - b.top);
		ventana.draw(t);
	}
	
	string entero(double valor)
	{
		ostringstream s;
		s << fixed << setprecision(0) << valor;
		return s.str();
	}
}

PantallaPlanificarViaje::PantallaPlanificarViaje(ListaPasajeros* pasajeros, ListaVuelos* vuelos, ArbolAVL* arbol, GestorAsignaciones* gestor, Grafo* grafo)
{
	listaPasajeros = pasajeros;
	listaVuelos = vuelos;
	arbolPasajeros = arbol;
	gestorAsignaciones = gestor;
	this->grafo = grafo;
	fuenteCargada = fuente.loadFromFile("resources/times.ttf");
	identificacionSeleccionada = "";
	vueloSeleccionado = "";
	mensaje = "Seleccione un pasajero y un vuelo.";
	desplazamientoPasajeros = 0;
	desplazamientoVuelos = 0;
}

string PantallaPlanificarViaje::nombreCiudad(const string& codigo)
{
	if (grafo != nullptr)
	{
		NodoVertice* c = grafo->buscarCiudad(codigo);
		if (c != nullptr)
			return c->ciudad.getNombre();
	}
	return codigo;
}

void PantallaPlanificarViaje::procesarEventos(RenderWindow& ventana, Event evento)
{
	if (evento.type == Event::MouseWheelScrolled)
	{
		Vector2i mouse = Mouse::getPosition(ventana);
		float delta = evento.mouseWheelScroll.delta;
		
		if (dentro(mouse.x, mouse.y, PAS_X, PY, PAS_ANCHO, PH))
		{
			int total = listaPasajeros->getCantidad();
			int maximo = total > FILAS_VISIBLES ? total - FILAS_VISIBLES : 0;
			if (delta < 0 && desplazamientoPasajeros < maximo) desplazamientoPasajeros++;
			else if (delta > 0 && desplazamientoPasajeros > 0) desplazamientoPasajeros--;
		}
		else if (dentro(mouse.x, mouse.y, VUE_X, PY, VUE_ANCHO, PH))
		{
			int total = listaVuelos->getCantidad();
			int maximo = total > FILAS_VISIBLES ? total - FILAS_VISIBLES : 0;
			if (delta < 0 && desplazamientoVuelos < maximo) desplazamientoVuelos++;
			else if (delta > 0 && desplazamientoVuelos > 0) desplazamientoVuelos--;
		}
		return;
	}
	
	if (evento.type != Event::MouseButtonPressed || evento.mouseButton.button != Mouse::Left)
		return;
	
	int x = evento.mouseButton.x;
	int y = evento.mouseButton.y;
	
	if (dentro(x, y, PAS_LISTA_X, LISTA_Y0, LISTA_ANCHO, FILAS_VISIBLES * FILA_PASO))
	{
		int fila = static_cast<int>((y - LISTA_Y0) / FILA_PASO);
		float dentroDeFila = (y - LISTA_Y0) - fila * FILA_PASO;
		if (fila >= FILAS_VISIBLES || dentroDeFila > FILA_ALTO)
			return;
		
		NodoPasajero* n = listaPasajeros->getPrimero();
		for (int i = 0; n != nullptr && i < desplazamientoPasajeros + fila; ++i) n = n->siguiente;
		if (n != nullptr)
		{
			identificacionSeleccionada = n->pasajero.getIdentificacion();
			mensaje = "Pasajero seleccionado: " + n->pasajero.getNombre();
		}
		return;
	}
	
	if (dentro(x, y, VUE_LISTA_X, LISTA_Y0, LISTA_ANCHO, FILAS_VISIBLES * FILA_PASO))
	{
		int fila = static_cast<int>((y - LISTA_Y0) / FILA_PASO);
		float dentroDeFila = (y - LISTA_Y0) - fila * FILA_PASO;
		if (fila >= FILAS_VISIBLES || dentroDeFila > FILA_ALTO)
			return;
		
		NodoVuelo* n = listaVuelos->getPrimero();
		for (int i = 0; n != nullptr && i < desplazamientoVuelos + fila; ++i) n = n->siguiente;
		if (n != nullptr)
		{
			vueloSeleccionado = n->vuelo.getCodigo();
			mensaje = "Vuelo seleccionado: " + nombreCiudad(n->vuelo.getCodigoOrigen()) + " a " + nombreCiudad(n->vuelo.getCodigoDestino());
		}
		return;
	}
	
	if (dentro(x, y, CONF_X, BTN_Y, CONF_ANCHO, BTN_ALTO))
	{
		asignarVuelo();
		return;
	}
	
	if (dentro(x, y, LIMP_X, BTN_Y, LIMP_ANCHO, BTN_ALTO))
	{
		identificacionSeleccionada = "";
		vueloSeleccionado = "";
		mensaje = "Seleccione un pasajero y un vuelo.";
	}
}

void PantallaPlanificarViaje::actualizar(float tiempo)
{
	(void)tiempo;
}

void PantallaPlanificarViaje::asignarVuelo()
{
	if (identificacionSeleccionada.empty()) { mensaje = "Seleccione primero un pasajero."; return; }
	if (vueloSeleccionado.empty()) { mensaje = "Seleccione primero un vuelo."; return; }
	
	NodoPasajero* pasajero = listaPasajeros->buscar(identificacionSeleccionada);
	NodoVuelo* vuelo = listaVuelos->buscar(vueloSeleccionado);
	if (pasajero == nullptr || vuelo == nullptr)
	{
		mensaje = "El pasajero o el vuelo ya no esta disponible.";
		return;
	}
	
	string origen = nombreCiudad(vuelo->vuelo.getCodigoOrigen());
	string destino = nombreCiudad(vuelo->vuelo.getCodigoDestino());
	
	if (gestorAsignaciones->asignarPasajeroVuelo(*arbolPasajeros, pasajero->pasajero, vueloSeleccionado))
		mensaje = "Viaje planificado: " + pasajero->pasajero.getNombre() + " viaja de " + origen + " a " + destino + ".";
	else
		mensaje = "No fue posible realizar la asignacion.";
}

void PantallaPlanificarViaje::dibujarTexto(RenderWindow& ventana, float x, float y, const string& texto, unsigned int tamano, Color color, bool negrita)
{
	if (!fuenteCargada) return;
	Text t;
	t.setFont(fuente);
	t.setString(texto);
	t.setCharacterSize(tamano);
	t.setFillColor(color);
	if (negrita)
		t.setStyle(Text::Bold);
	t.setPosition(x, y);
	ventana.draw(t);
}

void PantallaPlanificarViaje::dibujarPasajeros(RenderWindow& ventana)
{
	Color verdeOscuro(10, 76, 55);
	Color verdeClaro(225, 242, 224);
	Color textoOscuro(35, 65, 52);
	Color gris(105, 115, 108);
	Color bordeSuave(215, 225, 220);
	Color cajaNormal(250, 252, 250);
	
	caja(ventana, PAS_X, PY, PAS_ANCHO, PH, Color(255, 255, 255), bordeSuave, 1);
	dibujarTexto(ventana, PAS_LISTA_X, PY + 20, "PASAJEROS REGISTRADOS", 21, verdeOscuro, true);
	dibujarTexto(ventana, PAS_LISTA_X, PY + 55, "Selecciona un pasajero de la lista", 15, gris);
	
	NodoPasajero* n = listaPasajeros->getPrimero();
	int indice = 0;
	int fila = 0;
	while (n != nullptr && fila < FILAS_VISIBLES)
	{
		if (indice >= desplazamientoPasajeros)
		{
			float y = LISTA_Y0 + fila * FILA_PASO;
			bool sel = n->pasajero.getIdentificacion() == identificacionSeleccionada;
			
			caja(ventana, PAS_LISTA_X, y, LISTA_ANCHO, FILA_ALTO, sel ? verdeClaro : cajaNormal, Color(225, 232, 226), 1);
			dibujarTexto(ventana, PAS_LISTA_X + 13, y + 4, recortar(fuente, fuenteCargada, n->pasajero.getNombre(), 16, sel, LISTA_ANCHO - 30), 16, sel ? verdeOscuro : textoOscuro, sel);
			dibujarTexto(ventana, PAS_LISTA_X + 13, y + 23, "ID: " + n->pasajero.getIdentificacion(), 12, gris);
			fila++;
		}
		n = n->siguiente;
		indice++;
	}
	
	if (listaPasajeros->getCantidad() == 0)
		dibujarTexto(ventana, PAS_LISTA_X + 5, LISTA_Y0 + 10, "No hay pasajeros registrados.", 16, gris);
	
	float yPie = LISTA_Y0 + FILAS_VISIBLES * FILA_PASO + 2;
	dibujarTexto(ventana, PAS_LISTA_X, yPie, "Total de pasajeros: " + to_string(listaPasajeros->getCantidad()), 13, gris, true);
	if (listaPasajeros->getCantidad() > FILAS_VISIBLES)
		dibujarTexto(ventana, PAS_LISTA_X + 270, yPie, "Usa la rueda del mouse para ver mas.", 13, gris);
}

void PantallaPlanificarViaje::dibujarVuelos(RenderWindow& ventana)
{
	Color verdeOscuro(10, 76, 55);
	Color azulClaro(222, 235, 248);
	Color textoOscuro(35, 65, 52);
	Color gris(105, 115, 108);
	Color bordeSuave(215, 225, 220);
	Color cajaNormal(250, 252, 250);
	
	caja(ventana, VUE_X, PY, VUE_ANCHO, PH, Color(255, 255, 255), bordeSuave, 1);
	dibujarTexto(ventana, VUE_LISTA_X, PY + 20, "VUELOS DISPONIBLES", 21, verdeOscuro, true);
	dibujarTexto(ventana, VUE_LISTA_X, PY + 55, "Selecciona un vuelo de la lista", 15, gris);
	
	NodoVuelo* n = listaVuelos->getPrimero();
	int indice = 0;
	int fila = 0;
	while (n != nullptr && fila < FILAS_VISIBLES)
	{
		if (indice >= desplazamientoVuelos)
		{
			float y = LISTA_Y0 + fila * FILA_PASO;
			const Vuelo& v = n->vuelo;
			bool sel = v.getCodigo() == vueloSeleccionado;
			
			caja(ventana, VUE_LISTA_X, y, LISTA_ANCHO, FILA_ALTO, sel ? azulClaro : cajaNormal, Color(225, 232, 226), 1);
			
			string ruta = nombreCiudad(v.getCodigoOrigen()) + "  >  " + nombreCiudad(v.getCodigoDestino());
			dibujarTexto(ventana, VUE_LISTA_X + 13, y + 3, recortar(fuente, fuenteCargada, ruta, 15, true, LISTA_ANCHO - 30), 15, sel ? verdeOscuro : textoOscuro, true);
			dibujarTexto(ventana, VUE_LISTA_X + 13, y + 22, "Vuelo " + v.getCodigo() + "   |   Costo: " + entero(v.getCosto()) + "   |   Duracion: " + entero(v.getDuracion()) + " horas", 12, gris);
			fila++;
		}
		n = n->siguiente;
		indice++;
	}
	
	if (listaVuelos->getCantidad() == 0)
		dibujarTexto(ventana, VUE_LISTA_X + 5, LISTA_Y0 + 10, "No hay vuelos registrados.", 16, gris);
	
	float yPie = LISTA_Y0 + FILAS_VISIBLES * FILA_PASO + 2;
	dibujarTexto(ventana, VUE_LISTA_X, yPie, "Total de vuelos: " + to_string(listaVuelos->getCantidad()), 13, gris, true);
	if (listaVuelos->getCantidad() > FILAS_VISIBLES)
		dibujarTexto(ventana, VUE_LISTA_X + 270, yPie, "Usa la rueda del mouse para ver mas.", 13, gris);
}

void PantallaPlanificarViaje::dibujar(RenderWindow& ventana)
{
	Color verdeOscuro(10, 76, 55);
	Color verde(42, 112, 72);
	Color verdeHover(30, 88, 56);
	Color textoOscuro(35, 65, 52);
	Color bordeSuave(215, 225, 220);
	Color blanco(255, 255, 255);
	
	Vector2i mouse = Mouse::getPosition(ventana);
	
	RectangleShape fondo(Vector2f(1200, 800));
	fondo.setPosition(300, 0);
	fondo.setFillColor(Color(247, 250, 247));
	ventana.draw(fondo);
	
	caja(ventana, X0, ENC_Y, ENC_ANCHO, ENC_ALTO, verdeOscuro, verdeOscuro, 0);
	dibujarTexto(ventana, X0 + 35, ENC_Y + 16, "PLANIFICAR VIAJE", 34, blanco, true);
	dibujarTexto(ventana, X0 + 37, ENC_Y + 64, "Selecciona un pasajero y el vuelo que deseas asignarle", 19, Color(230, 245, 235));
	
	dibujarPasajeros(ventana);
	dibujarVuelos(ventana);
	
	caja(ventana, X0, MSG_Y, ENC_ANCHO, MSG_ALTO, Color(245, 248, 245), bordeSuave, 1);
	dibujarTexto(ventana, X0 + 15, MSG_Y + 9, recortar(fuente, fuenteCargada, mensaje, 15, false, ENC_ANCHO - 30), 15, textoOscuro);
	
	bool sobreConfirmar = dentro(mouse.x, mouse.y, CONF_X, BTN_Y, CONF_ANCHO, BTN_ALTO);
	caja(ventana, CONF_X, BTN_Y, CONF_ANCHO, BTN_ALTO, sobreConfirmar ? verdeHover : verde, sobreConfirmar ? verdeHover : verde, 0);
	textoCentrado(ventana, fuente, fuenteCargada, "CONFIRMAR VIAJE", CONF_X + CONF_ANCHO / 2, BTN_Y + BTN_ALTO / 2, 19, blanco, true);
	
	bool sobreLimpiar = dentro(mouse.x, mouse.y, LIMP_X, BTN_Y, LIMP_ANCHO, BTN_ALTO);
	caja(ventana, LIMP_X, BTN_Y, LIMP_ANCHO, BTN_ALTO, sobreLimpiar ? Color(222, 228, 222) : Color(235, 239, 235), bordeSuave, 1);
	textoCentrado(ventana, fuente, fuenteCargada, "LIMPIAR", LIMP_X + LIMP_ANCHO / 2, BTN_Y + BTN_ALTO / 2, 15, textoOscuro, true);
	
	if (!fuenteCargada)
		dibujarTexto(ventana, X0, 770, "No se encontro resources/times.ttf. Verifique la ruta de la fuente.", 12, Color(170, 55, 45));
}
