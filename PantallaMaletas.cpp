#include "PantallaMaletas.h"
#include <sstream>

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

PantallaMaletas::PantallaMaletas(ListaMaletas* lista, string id)
{
	listaMaletas = lista;
	grafo = nullptr;
	fuenteCargada = fuente.loadFromFile("resources/times.ttf");
	codigo = "";
	identificacionPasajero = id;
	identificacionBloqueada = !id.empty();
	peso = "";
	destino = "";
	estado = "Registrada";
	codigoSeleccionado = "";
	mensaje = "";
	campoActivo = 0;
	indiceInicioDestinos = 0;
	desplazamientoLista = 0;
	editando = false;
	confirmarEliminar = false;
	cursorVisible = true;
	listaDestinosAbierta = false;
	tiempoCursor = 0;
}

PantallaMaletas::PantallaMaletas(ListaMaletas* lista, Grafo* g, string id)
{
	listaMaletas = lista;
	grafo = g;
	fuenteCargada = fuente.loadFromFile("resources/times.ttf");
	codigo = "";
	identificacionPasajero = id;
	identificacionBloqueada = !id.empty();
	peso = "";
	destino = "";
	estado = "Registrada";
	codigoSeleccionado = "";
	mensaje = "";
	campoActivo = 0;
	indiceInicioDestinos = 0;
	desplazamientoLista = 0;
	editando = false;
	confirmarEliminar = false;
	cursorVisible = true;
	listaDestinosAbierta = false;
	tiempoCursor = 0;
}

void PantallaMaletas::procesarEventos(RenderWindow& ventana, Event e)
{
	if (confirmarEliminar)
	{
		if (e.type == Event::MouseButtonPressed && e.mouseButton.button == Mouse::Left)
		{
			int x = e.mouseButton.x, y = e.mouseButton.y;
			if (x >= 810 && x <= 910 && y >= 430 && y <= 475)
			{
				if (listaMaletas->eliminar(codigoSeleccionado))
					mensaje = "Maleta eliminada correctamente";
				else
					mensaje = "No se pudo eliminar la maleta";
				confirmarEliminar = false;
				limpiarFormulario();
			}
			else if (x >= 930 && x <= 1030 && y >= 430 && y <= 475)
				confirmarEliminar = false;
		}
		if (e.type == Event::KeyPressed && e.key.code == Keyboard::Escape)
			confirmarEliminar = false;
		return;
	}
	
	if (listaDestinosAbierta)
	{
		if (e.type == Event::MouseButtonPressed && e.mouseButton.button == Mouse::Left)
		{
			int x = e.mouseButton.x, y = e.mouseButton.y;
			if (x >= 350 && x <= 730 && y >= 435 && y < 675)
			{
				seleccionarDestino(indiceInicioDestinos + (y - 435) / 30);
				return;
			}
			listaDestinosAbierta = false;
		}
		if (e.type == Event::MouseWheelScrolled)
		{
			int total = grafo ? grafo->getCantidadVertices() : 0;
			if (e.mouseWheelScroll.delta < 0 && total > 8 && indiceInicioDestinos < total - 8)
				indiceInicioDestinos++;
			else if (e.mouseWheelScroll.delta > 0 && indiceInicioDestinos > 0)
				indiceInicioDestinos--;
			if (total <= 8) indiceInicioDestinos = 0;
			return;
		}
		if (e.type == Event::KeyPressed && e.key.code == Keyboard::Escape)
		{
			listaDestinosAbierta = false;
			return;
		}
	}
	
	// Scroll vertical de la lista de maletas.
	if (e.type == Event::MouseWheelScrolled)
	{
		int x = e.mouseWheelScroll.x;
		int y = e.mouseWheelScroll.y;
		
		if (x >= 785 && x <= 1450 && y >= 200 && y < 694)
		{
			int maximo = listaMaletas->getCantidad() - 13;
			if (maximo < 0) maximo = 0;
			
			if (e.mouseWheelScroll.delta < 0)
				desplazamientoLista += 1;
			else if (e.mouseWheelScroll.delta > 0)
				desplazamientoLista -= 1;
			
			if (desplazamientoLista < 0)
				desplazamientoLista = 0;
			if (desplazamientoLista > maximo)
				desplazamientoLista = maximo;
		}
		return;
	}
	
	if (e.type == Event::MouseButtonPressed && e.mouseButton.button == Mouse::Left)
	{
		int x = e.mouseButton.x, y = e.mouseButton.y;
		if (x >= 350 && x <= 730)
		{
			if (y >= 145 && y <= 190) campoActivo = 0;
			else if (y >= 205 && y <= 250 && !identificacionBloqueada) campoActivo = 1;
			else if (y >= 265 && y <= 310) campoActivo = 2;
			else if (y >= 325 && y <= 370)
			{
				campoActivo = 3;
				listaDestinosAbierta = true;
				indiceInicioDestinos = 0;
			}
			else if (y >= 385 && y <= 430) campoActivo = 4;
		}
		
		if (x >= 345 && x <= 510 && y >= 445 && y <= 490)
		{
			if (editando) modificarMaleta();
			else registrarMaleta();
		}
		else if (x >= 525 && x <= 690 && y >= 445 && y <= 490)
			eliminarMaleta();
		
		// Seleccion de fila (se excluye la barra de desplazamiento).
		if (x >= 785 && x <= 1435 && y >= 200 && y < 694)
		{
			int fila = desplazamientoLista + (y - 200) / 38;
			NodoMaleta* n = listaMaletas->getPrimero();
			while (n && fila-- > 0) n = n->siguiente;
			if (n) seleccionarMaleta(n->maleta.getCodigo());
		}
	}
	
	if (e.type == Event::KeyPressed && e.key.code == Keyboard::Tab)
	{
		campoActivo = (campoActivo + 1) % 5;
		if (identificacionBloqueada && campoActivo == 1) campoActivo = 2;
		listaDestinosAbierta = campoActivo == 3;
		indiceInicioDestinos = 0;
	}
	if (e.type == Event::KeyPressed && e.key.code == Keyboard::Escape)
		limpiarFormulario();
	
	if (e.type == Event::TextEntered && campoActivo != 3)
	{
		string* campo = nullptr;
		if (campoActivo == 0) campo = &codigo;
		if (campoActivo == 1 && !identificacionBloqueada) campo = &identificacionPasajero;
		if (campoActivo == 2) campo = &peso;
		if (campoActivo == 4) campo = &estado;
		if (campo)
		{
			if (e.text.unicode == 8)
			{
				if (!campo->empty()) campo->pop_back();
			}
			else if (e.text.unicode >= 32 && e.text.unicode < 128 && campo->size() < 30)
			{
				char c = (char)e.text.unicode;
				if (campoActivo != 2 || (c >= '0' && c <= '9') || c == '.')
					campo->push_back(c);
			}
		}
	}
}

void PantallaMaletas::actualizar(float t)
{
	tiempoCursor += t;
	if (tiempoCursor >= 0.5f)
	{
		cursorVisible = !cursorVisible;
		tiempoCursor = 0;
	}
}

void PantallaMaletas::dibujar(RenderWindow& v)
{
	v.setView(v.getDefaultView());
	
	RectangleShape bg(Vector2f(1200, 800));
	bg.setPosition(300, 0);
	bg.setFillColor(Color::White);
	v.draw(bg);
	dibujarTexto(v, 335, 35, "GESTION DE MALETAS", 28, Color(55,70,60));
	dibujarTexto(v, 335, 75, "REGISTRO Y CONTROL DE EQUIPAJE", 14, Color(120,125,120));
	dibujarFormulario(v);
	dibujarListado(v);
	if (listaDestinosAbierta) dibujarListaDestinos(v);
	if (!mensaje.empty()) dibujarTexto(v, 335, 755, mensaje, 15, Color(90,120,95));
	if (confirmarEliminar) dibujarConfirmacion(v);
}

void PantallaMaletas::dibujarFormulario(RenderWindow& v)
{
	RectangleShape p(Vector2f(430,450));
	p.setPosition(325,115); p.setFillColor(Color(248,249,247));
	p.setOutlineThickness(1); p.setOutlineColor(Color(215,220,215)); v.draw(p);
	dibujarTexto(v,350,125,"DATOS DE LA MALETA",18,Color(55,70,60));
	string vals[5]={codigo,identificacionPasajero,peso,destino,estado};
	string labs[5]={"Codigo de maleta","Identificacion del pasajero","Peso en kilogramos","Destino (seleccione)","Estado"};
	for(int i=0;i<5;i++)
	{
		float y=150+i*60;
		dibujarTexto(v,350,y,labs[i],14,Color(75,80,75));
		RectangleShape c(Vector2f(380,32)); c.setPosition(350,y+20);
		c.setFillColor((i==1 && identificacionBloqueada)?Color(230,235,230):Color::White);
		c.setOutlineThickness(campoActivo==i?2:1);
		c.setOutlineColor(campoActivo==i?Color(90,120,95):Color(210,215,210)); v.draw(c);
		string t=vals[i];
		if(i==3) t+="   v";
		else if(campoActivo==i && cursorVisible) t+="|";
		dibujarTexto(v,360,y+25,t,14,Color::Black);
	}
	RectangleShape b1(Vector2f(165,45)); b1.setPosition(345,460);
	b1.setFillColor(editando?Color(120,130,120):Color(90,120,95)); v.draw(b1);
	dibujarTexto(v,editando?360:380,472,editando?"GUARDAR CAMBIOS":"REGISTRAR",14,Color::White);
	RectangleShape b2(Vector2f(165,45)); b2.setPosition(525,460);
	b2.setFillColor(Color(165,85,80)); v.draw(b2);
	dibujarTexto(v,575,472,"ELIMINAR",14,Color::White);
	dibujarTexto(v,350,520,"Seleccione una maleta del listado para modificarla",12,Color(100,110,120));
	dibujarTexto(v,350,545,"El destino se carga de las ciudades registradas.",12,Color(100,110,120));
}

void PantallaMaletas::dibujarListaDestinos(RenderWindow& v)
{
	RectangleShape p(Vector2f(380,272)); p.setPosition(350,405);
	p.setFillColor(Color::White); p.setOutlineThickness(2);
	p.setOutlineColor(Color(90,120,95)); v.draw(p);
	dibujarTexto(v,360,408,"SELECCIONE UNA CIUDAD",12,Color(90,120,95));
	if(!grafo || grafo->getCantidadVertices()==0)
	{
		dibujarTexto(v,360,445,"No hay ciudades registradas.",13,Color::Black);
		return;
	}
	NodoVertice* n=grafo->getPrimero();
	int i=0, mostradas=0;
	while(n && mostradas<8)
	{
		if(i>=indiceInicioDestinos)
		{
			float y=435+mostradas*30;
			RectangleShape r(Vector2f(370,29)); r.setPosition(355,y);
			r.setFillColor(mostradas%2==0?Color(242,245,241):Color::White); v.draw(r);
			dibujarTexto(v,365,y+5,n->ciudad.getNombre(),13,Color::Black);
			mostradas++;
		}
		n=n->siguiente; i++;
	}
	if(grafo->getCantidadVertices()>8)
		  dibujarTexto(v,360,675,"Use la rueda del mouse para ver mas ciudades.",10,Color(120,125,120));
}

void PantallaMaletas::seleccionarDestino(int indice)
{
	if(!grafo || indice<0) return;
	NodoVertice* n=grafo->getPrimero();
	for(int i=0;n && i<indice;i++) n=n->siguiente;
	if(!n) return;
	destino=n->ciudad.getNombre();
	campoActivo=4;
	listaDestinosAbierta=false;
	mensaje="Destino seleccionado: "+destino;
}

void PantallaMaletas::dibujarListado(RenderWindow& v)
{
	RectangleShape p(Vector2f(680,620)); p.setPosition(770,115);
	p.setFillColor(Color(248,249,247)); p.setOutlineThickness(1);
	p.setOutlineColor(Color(215,220,215)); v.draw(p);
	dibujarTexto(v,795,125,"MALETAS REGISTRADAS",18,Color(55,70,60));
	RectangleShape h(Vector2f(650,35)); h.setPosition(785,165);
	h.setFillColor(Color(90,120,95)); v.draw(h);
	dibujarTexto(v,795,173,"CODIGO",12,Color::White);
	dibujarTexto(v,890,173,"PASAJERO",12,Color::White);
	dibujarTexto(v,1040,173,"PESO",12,Color::White);
	dibujarTexto(v,1120,173,"DESTINO",12,Color::White);
	dibujarTexto(v,1290,173,"ESTADO",12,Color::White);
	
	int totalMaletas = listaMaletas->getCantidad();
	const int filasVisibles = 13;
	int maximoDesplazamiento = totalMaletas - filasVisibles;
	if (maximoDesplazamiento < 0)
		maximoDesplazamiento = 0;
	
	if (desplazamientoLista > maximoDesplazamiento)
		desplazamientoLista = maximoDesplazamiento;
	
	NodoMaleta* n = listaMaletas->getPrimero();
	for (int i = 0; n != nullptr && i < desplazamientoLista; i++)
		n = n->siguiente;
	
	int fila = 0;
	while (n != nullptr && fila < filasVisibles)
	{
		float y = 200 + fila * 38;
		
		if (n->maleta.getCodigo() == codigoSeleccionado)
		{
			RectangleShape s(Vector2f(650,36));
			s.setPosition(785,y);
			s.setFillColor(Color(225,237,225));
			v.draw(s);
		}
		else if (fila % 2 == 0)
		{
			RectangleShape s(Vector2f(650,36));
			s.setPosition(785,y);
			s.setFillColor(Color(242,245,241));
			v.draw(s);
		}
		
		ostringstream w;
		w << n->maleta.getPeso();
		dibujarTexto(v,795,y+8,recortarTexto(fuente,n->maleta.getCodigo(),11,85),11,Color::Black);
		dibujarTexto(v,890,y+8,recortarTexto(fuente,n->maleta.getIdentificacionPasajero(),11,140),11,Color::Black);
		dibujarTexto(v,1040,y+8,recortarTexto(fuente,w.str()+" kg",11,70),11,Color::Black);
		dibujarTexto(v,1120,y+8,recortarTexto(fuente,n->maleta.getDestino(),11,160),11,Color::Black);
		dibujarTexto(v,1290,y+8,recortarTexto(fuente,n->maleta.getEstado(),11,140),11,Color::Black);
		
		n = n->siguiente;
		fila++;
	}

	RectangleShape pista(Vector2f(6,494));
	pista.setPosition(1440,200);
	pista.setFillColor(Color(225,229,224));
	v.draw(pista);
	
	float altoIndicador = 494.0f;
	if (totalMaletas > filasVisibles)
	{
		altoIndicador = 494.0f * filasVisibles / totalMaletas;
		if (altoIndicador < 35.0f)
			altoIndicador = 35.0f;
	}
	
	float recorrido = 494.0f - altoIndicador;
	float posicionIndicador = 200.0f;
	if (maximoDesplazamiento > 0)
		posicionIndicador += recorrido * desplazamientoLista / maximoDesplazamiento;
	
	RectangleShape indicador(Vector2f(6,altoIndicador));
	indicador.setPosition(1440,posicionIndicador);
	indicador.setFillColor(Color(90,120,95));
	v.draw(indicador);
	
	dibujarTexto(v,795,705,"Total de maletas: "+to_string(listaMaletas->getCantidad()),14,Color(75,80,75));
}

void PantallaMaletas::dibujarConfirmacion(RenderWindow& v)
{
	RectangleShape f(Vector2f(1200,800)); f.setPosition(300,0);
	f.setFillColor(Color(0,0,0,100)); v.draw(f);
	RectangleShape c(Vector2f(420,190)); c.setPosition(690,330);
	c.setFillColor(Color::White); c.setOutlineThickness(2);
	c.setOutlineColor(Color(90,120,95)); v.draw(c);
	dibujarTexto(v,735,365,"CONFIRMAR ELIMINACION",20,Color(55,70,60));
	dibujarTexto(v,735,405,"Desea eliminar esta maleta?",16,Color::Black);
	RectangleShape a(Vector2f(100,45)); a.setPosition(810,430);
	a.setFillColor(Color(165,85,80)); v.draw(a);
	dibujarTexto(v,845,442,"SI",16,Color::White);
	RectangleShape b(Vector2f(100,45)); b.setPosition(930,430);
	b.setFillColor(Color(90,120,95)); v.draw(b);
	dibujarTexto(v,965,442,"NO",16,Color::White);
}

void PantallaMaletas::registrarMaleta()
{
	if(codigo.empty()||identificacionPasajero.empty()||peso.empty()||destino.empty()||estado.empty())
	{ mensaje="Complete todos los campos"; return; }
	try
	{
		float p=stof(peso);
		if(p<=0){mensaje="El peso debe ser mayor que cero";return;}
		if(grafo)
		{
			bool valido=false; NodoVertice* n=grafo->getPrimero();
			while(n){if(n->ciudad.getNombre()==destino){valido=true;break;}n=n->siguiente;}
			if(!valido){mensaje="Seleccione un destino de la lista.";return;}
		}
		Maleta nueva(codigo,identificacionPasajero,p,destino,estado);
		if(listaMaletas->agregar(nueva)){mensaje="Maleta registrada correctamente";limpiarFormulario();}
		else mensaje="Ya existe una maleta con ese codigo";
	}
	catch(...){mensaje="Ingrese un peso valido";}
}

void PantallaMaletas::seleccionarMaleta(string c)
{
	NodoMaleta* n=listaMaletas->buscar(c); if(!n)return;
	codigoSeleccionado=c; codigo=n->maleta.getCodigo();
	identificacionPasajero=n->maleta.getIdentificacionPasajero();
	ostringstream w; w<<n->maleta.getPeso(); peso=w.str();
	destino=n->maleta.getDestino(); estado=n->maleta.getEstado();
	editando=true; campoActivo=0; listaDestinosAbierta=false; mensaje="";
}

void PantallaMaletas::modificarMaleta()
{
	if(!editando||codigoSeleccionado.empty()){mensaje="Seleccione una maleta del listado";return;}
	if(codigo.empty()||identificacionPasajero.empty()||peso.empty()||destino.empty()||estado.empty())
	{mensaje="Complete todos los campos";return;}
	if(codigo!=codigoSeleccionado){mensaje="El codigo de la maleta no se puede cambiar";return;}
	try
	{
		float p=stof(peso); if(p<=0){mensaje="El peso debe ser mayor que cero";return;}
		if(grafo)
		{
			bool valido=false; NodoVertice* n=grafo->getPrimero();
			while(n){if(n->ciudad.getNombre()==destino){valido=true;break;}n=n->siguiente;}
			if(!valido){mensaje="Seleccione un destino de la lista.";return;}
		}
		if(listaMaletas->modificar(codigoSeleccionado,identificacionPasajero,p,destino,estado))
		   mensaje="Maleta modificada correctamente";
		else mensaje="No se pudo modificar la maleta";
	}
	catch(...){mensaje="Ingrese un peso valido";}
}

void PantallaMaletas::eliminarMaleta()
{
	if(codigoSeleccionado.empty()){mensaje="Seleccione una maleta para eliminar";return;}
	confirmarEliminar=true;
}

void PantallaMaletas::limpiarFormulario()
{
	codigo="";
	if(!identificacionBloqueada) identificacionPasajero="";
	peso=""; destino=""; estado="Registrada"; codigoSeleccionado="";
	campoActivo=0; editando=false; listaDestinosAbierta=false; indiceInicioDestinos=0;
}

void PantallaMaletas::dibujarTexto(RenderWindow& v,float x,float y,string t,int size,Color color)
{
	if(!fuenteCargada)return;
	Text a; a.setFont(fuente); a.setString(t); a.setCharacterSize(size);
	a.setFillColor(color); a.setPosition(x,y); v.draw(a);
}
