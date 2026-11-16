#include "PantallaVuelos.h"
#include <sstream>
#include <iomanip>
#include <cctype>

PantallaVuelos::PantallaVuelos(ListaVuelos* listaVuelos, Grafo* grafo)
{
    this->listaVuelos = listaVuelos;
    this->grafo = grafo;
    fuenteCargada = fuente.loadFromFile("resources/times.ttf");
    codigo = "";
    codigoOrigen = "";
    codigoDestino = "";
    costo = "";
    duracion = "";
    codigoSeleccionado = "";
    mensaje = "";
    campoActivo = 0;
    editando = false;
    confirmarEliminar = false;
    cursorVisible = true;
    tiempoCursor = 0;
    origenDesplegado = false;
    destinoDesplegado = false;
    desplazamientoOrigen = 0;
    desplazamientoDestino = 0;
    filtroOrigen = "";
    filtroDestino = "";
}

void PantallaVuelos::procesarEventos(RenderWindow& ventana, Event evento)
{
    if (confirmarEliminar)
    {
        if (evento.type == Event::MouseButtonPressed && evento.mouseButton.button == Mouse::Left)
        {
            float x = evento.mouseButton.x;
            float y = evento.mouseButton.y;
            if (x >= 610 && x <= 710 && y >= 420 && y <= 465)
            {
                if (listaVuelos->eliminar(codigoSeleccionado))
                    mensaje = "Vuelo eliminado correctamente.";
                else
                    mensaje = "No se pudo eliminar el vuelo.";
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

    if (evento.type == Event::MouseWheelScrolled)
    {
        if (origenDesplegado && evento.mouseWheelScroll.x >= 340 && evento.mouseWheelScroll.x <= 730 &&
            evento.mouseWheelScroll.y >= 242 && evento.mouseWheelScroll.y < 402)
        {
            int total = (int)obtenerCiudadesDisponibles(true).size();
            int maximo = total > 4 ? total - 4 : 0;
            desplazamientoOrigen -= evento.mouseWheelScroll.delta > 0 ? 1 : -1;
            if (desplazamientoOrigen < 0) desplazamientoOrigen = 0;
            if (desplazamientoOrigen > maximo) desplazamientoOrigen = maximo;
        }
        else if (destinoDesplegado && evento.mouseWheelScroll.x >= 340 && evento.mouseWheelScroll.x <= 730 &&
                 evento.mouseWheelScroll.y >= 302 && evento.mouseWheelScroll.y < 462)
        {
            int total = (int)obtenerCiudadesDisponibles(false).size();
            int maximo = total > 4 ? total - 4 : 0;
            desplazamientoDestino -= evento.mouseWheelScroll.delta > 0 ? 1 : -1;
            if (desplazamientoDestino < 0) desplazamientoDestino = 0;
            if (desplazamientoDestino > maximo) desplazamientoDestino = maximo;
        }
    }

    if (evento.type == Event::MouseButtonPressed && evento.mouseButton.button == Mouse::Left)
    {
        float x = evento.mouseButton.x;
        float y = evento.mouseButton.y;

        if (origenDesplegado)
        {
            if (x >= 340 && x <= 730 && y >= 242 && y < 402)
            {
                int fila = (int)((y - 242) / 40) + desplazamientoOrigen;
                vector<NodoVertice*> ciudades = obtenerCiudadesDisponibles(true);
                if (fila >= 0 && fila < (int)ciudades.size())
                {
                    codigoOrigen = ciudades[fila]->ciudad.getCodigo();
                    if (codigoOrigen == codigoDestino)
                        codigoDestino = "";
                    mensaje = "Ciudad de origen seleccionada.";
                }
                origenDesplegado = false;
                filtroOrigen = "";
                return;
            }
            origenDesplegado = false;
            filtroOrigen = "";
        }

        if (destinoDesplegado)
        {
            if (x >= 340 && x <= 730 && y >= 302 && y < 462)
            {
                int fila = (int)((y - 302) / 40) + desplazamientoDestino;
                vector<NodoVertice*> ciudades = obtenerCiudadesDisponibles(false);
                if (fila >= 0 && fila < (int)ciudades.size())
                {
                    codigoDestino = ciudades[fila]->ciudad.getCodigo();
                    mensaje = "Ciudad de destino seleccionada.";
                }
                destinoDesplegado = false;
                filtroDestino = "";
                return;
            }
            destinoDesplegado = false;
            filtroDestino = "";
        }

        if (x >= 340 && x <= 730)
        {
            if (y >= 150 && y <= 182)
            {
                campoActivo = 0;
                origenDesplegado = false;
                destinoDesplegado = false;
            }
            else if (y >= 210 && y <= 242)
            {
                campoActivo = 1;
                origenDesplegado = true;
                destinoDesplegado = false;
                filtroOrigen = "";
                desplazamientoOrigen = 0;
                return;
            }
            else if (y >= 270 && y <= 302)
            {
                campoActivo = 2;
                destinoDesplegado = true;
                origenDesplegado = false;
                filtroDestino = "";
                desplazamientoDestino = 0;
                return;
            }
            else if (y >= 330 && y <= 362)
            {
                campoActivo = 3;
                origenDesplegado = false;
                destinoDesplegado = false;
            }
            else if (y >= 390 && y <= 422)
            {
                campoActivo = 4;
                origenDesplegado = false;
                destinoDesplegado = false;
            }
        }

        if (x >= 340 && x <= 465 && y >= 450 && y <= 495)
            registrarVuelo();
        else if (x >= 475 && x <= 600 && y >= 450 && y <= 495)
            modificarVuelo();
        else if (x >= 610 && x <= 735 && y >= 450 && y <= 495)
            eliminarVuelo();
        else if (x >= 780 && x <= ventana.getSize().x - 20 && y >= 195 && y <= 495)
        {
            int fila = (int)((y - 195) / 40);
            NodoVuelo* actual = listaVuelos->getPrimero();
            while (actual != nullptr && fila > 0)
            {
                actual = actual->siguiente;
                fila--;
            }
            if (actual != nullptr)
                seleccionarVuelo(actual->vuelo.getCodigo());
        }
    }

    if (evento.type == Event::KeyPressed && evento.key.code == Keyboard::Tab)
    {
        campoActivo++;
        if (campoActivo > 4)
            campoActivo = 0;
        origenDesplegado = false;
        destinoDesplegado = false;
    }

    if (evento.type == Event::TextEntered)
    {
        if (origenDesplegado || destinoDesplegado)
        {
            string& filtro = origenDesplegado ? filtroOrigen : filtroDestino;
            int& desplazamiento = origenDesplegado ? desplazamientoOrigen : desplazamientoDestino;
            if (evento.text.unicode == 8)
            {
                if (!filtro.empty())
                    filtro.pop_back();
            }
            else if (evento.text.unicode >= 32 && evento.text.unicode <= 255 && filtro.length() < 35)
            {
                char caracter = static_cast<char>(evento.text.unicode);
                if ((caracter >= 'A' && caracter <= 'Z') || (caracter >= 'a' && caracter <= 'z') ||
                    (caracter >= '0' && caracter <= '9') || caracter == ' ' || caracter == '-')
                    filtro.push_back(caracter);
            }
            desplazamiento = 0;
            return;
        }

        string* campo = nullptr;
        if (campoActivo == 0) campo = &codigo;
        else if (campoActivo == 3) campo = &costo;
        else if (campoActivo == 4) campo = &duracion;

        if (campo != nullptr)
        {
            if (evento.text.unicode == 8)
            {
                if (!campo->empty())
                    campo->pop_back();
            }
            else if (evento.text.unicode >= 32 && evento.text.unicode < 127 && campo->length() < 25)
            {
                char caracter = (char)evento.text.unicode;
                if (campoActivo == 0)
                    campo->push_back(caracter);
                else if ((caracter >= '0' && caracter <= '9') || (caracter == '.' && campo->find('.') == string::npos))
                    campo->push_back(caracter);
            }
        }
    }

    if (evento.type == Event::KeyPressed && evento.key.code == Keyboard::Escape)
    {
        if (origenDesplegado || destinoDesplegado)
        {
            origenDesplegado = false;
            destinoDesplegado = false;
        }
        else
            limpiarFormulario();
    }
}

void PantallaVuelos::actualizar(float tiempo)
{
    tiempoCursor += tiempo;
    if (tiempoCursor >= 0.5f)
    {
        cursorVisible = !cursorVisible;
        tiempoCursor = 0;
    }
}

void PantallaVuelos::dibujar(RenderWindow& ventana)
{
    float ancho = ventana.getSize().x;
    float alto = ventana.getSize().y;
    RectangleShape fondo(Vector2f(ancho - 300, alto));
    fondo.setPosition(300, 0);
    fondo.setFillColor(Color::White);
    ventana.draw(fondo);

    dibujarTexto(ventana, 335, 35, "GESTION DE VUELOS", 28, Color(25, 65, 105));
    dibujarTexto(ventana, 335, 85, "REGISTRO DE VUELOS", 20, Color::Black);
    dibujarFormulario(ventana);
    dibujarListado(ventana);

    if (origenDesplegado)
        dibujarListaCiudades(ventana, true);
    if (destinoDesplegado)
        dibujarListaCiudades(ventana, false);

    if (!mensaje.empty())
        dibujarTexto(ventana, 340, 520, mensaje, 16, Color(25, 65, 105));
    if (confirmarEliminar)
        dibujarConfirmacion(ventana);
}

void PantallaVuelos::dibujarFormulario(RenderWindow& ventana)
{
    RectangleShape panel(Vector2f(430, 440));
    panel.setPosition(325, 115);
    panel.setFillColor(Color(245, 247, 250));
    panel.setOutlineThickness(1);
    panel.setOutlineColor(Color(210, 215, 220));
    ventana.draw(panel);

    for (int i = 0; i < 5; i++)
    {
        RectangleShape campo(Vector2f(390, 32));
        campo.setPosition(340, 150 + i * 60);
        campo.setFillColor(Color::White);
        campo.setOutlineThickness(campoActivo == i ? 2 : 1);
        campo.setOutlineColor(campoActivo == i ? Color(30, 110, 180) : Color(180, 185, 190));
        ventana.draw(campo);
    }

    dibujarTexto(ventana, 345, 125, "Codigo del vuelo:", 16, Color::Black);
    dibujarTexto(ventana, 345, 155, codigo, 16, Color::Black);

    NodoVertice* origen = grafo->buscarCiudad(codigoOrigen);
    NodoVertice* destino = grafo->buscarCiudad(codigoDestino);
    string nombreOrigen = origen == nullptr ? "Seleccione una ciudad" : origen->ciudad.getNombre();
    string nombreDestino = destino == nullptr ? "Seleccione una ciudad" : destino->ciudad.getNombre();

    dibujarTexto(ventana, 345, 185, "Ciudad de origen:", 16, Color::Black);
    string textoOrigen = origenDesplegado ? (filtroOrigen.empty() ? "Escriba para filtrar..." : "Buscar: " + filtroOrigen) : nombreOrigen;
    dibujarTexto(ventana, 345, 215, textoOrigen, 16, origenDesplegado || origen == nullptr ? Color(130, 130, 130) : Color::Black);
    dibujarTexto(ventana, 345, 245, "Ciudad de destino:", 16, Color::Black);
    string textoDestino = destinoDesplegado ? (filtroDestino.empty() ? "Escriba para filtrar..." : "Buscar: " + filtroDestino) : nombreDestino;
    dibujarTexto(ventana, 345, 275, textoDestino, 16, destinoDesplegado || destino == nullptr ? Color(130, 130, 130) : Color::Black);
    dibujarTexto(ventana, 345, 305, "Costo:", 16, Color::Black);
    dibujarTexto(ventana, 345, 335, costo, 16, Color::Black);
    dibujarTexto(ventana, 345, 365, "Duracion (horas):", 16, Color::Black);
    dibujarTexto(ventana, 345, 395, duracion, 16, Color::Black);

    dibujarTexto(ventana, 700, 215, "v", 16, Color(25, 65, 105));
    dibujarTexto(ventana, 700, 275, "v", 16, Color(25, 65, 105));

    if (cursorVisible && fuenteCargada && campoActivo != 1 && campoActivo != 2)
    {
        string contenido;
        if (campoActivo == 0) contenido = codigo;
        else if (campoActivo == 3) contenido = costo;
        else contenido = duracion;
        Text textoCursor;
        textoCursor.setFont(fuente);
        textoCursor.setString(contenido);
        textoCursor.setCharacterSize(16);
        RectangleShape cursor(Vector2f(2, 20));
        cursor.setPosition(345 + textoCursor.getLocalBounds().left + textoCursor.getLocalBounds().width, 157 + campoActivo * 60);
        cursor.setFillColor(Color::Black);
        ventana.draw(cursor);
    }

    Color azul(28, 91, 145);
    RectangleShape botonRegistrar(Vector2f(125, 45));
    botonRegistrar.setPosition(340, 450);
    botonRegistrar.setFillColor(azul);
    ventana.draw(botonRegistrar);
    dibujarTexto(ventana, 350, 462, "REGISTRAR", 15, Color::White);

    RectangleShape botonModificar(Vector2f(125, 45));
    botonModificar.setPosition(475, 450);
    botonModificar.setFillColor(azul);
    ventana.draw(botonModificar);
    dibujarTexto(ventana, 485, 462, "MODIFICAR", 15, Color::White);

    RectangleShape botonEliminar(Vector2f(125, 45));
    botonEliminar.setPosition(610, 450);
    botonEliminar.setFillColor(Color(190, 65, 65));
    ventana.draw(botonEliminar);
    dibujarTexto(ventana, 622, 462, "ELIMINAR", 15, Color::White);
}

vector<NodoVertice*> PantallaVuelos::obtenerCiudadesDisponibles(bool origen)
{
    vector<NodoVertice*> ciudades;
    string filtro = origen ? filtroOrigen : filtroDestino;
    string filtroMinuscula = filtro;
    for (size_t i = 0; i < filtroMinuscula.size(); i++)
        filtroMinuscula[i] = (char)tolower((unsigned char)filtroMinuscula[i]);

    NodoVertice* actual = grafo->getPrimero();
    while (actual != nullptr)
    {
        bool permitida = origen || actual->ciudad.getCodigo() != codigoOrigen;
        if (permitida)
        {
            string nombre = actual->ciudad.getNombre();
            string codigoCiudad = actual->ciudad.getCodigo();
            for (size_t i = 0; i < nombre.size(); i++)
                nombre[i] = (char)tolower((unsigned char)nombre[i]);
            for (size_t i = 0; i < codigoCiudad.size(); i++)
                codigoCiudad[i] = (char)tolower((unsigned char)codigoCiudad[i]);
            if (filtroMinuscula.empty() || nombre.find(filtroMinuscula) != string::npos ||
                codigoCiudad.find(filtroMinuscula) != string::npos)
                ciudades.push_back(actual);
        }
        actual = actual->siguiente;
    }
    return ciudades;
}

void PantallaVuelos::dibujarListaCiudades(RenderWindow& ventana, bool origen)
{
    float yInicial = origen ? 242.0f : 302.0f;
    int desplazamiento = origen ? desplazamientoOrigen : desplazamientoDestino;
    vector<NodoVertice*> ciudades = obtenerCiudadesDisponibles(origen);
    RectangleShape fondo(Vector2f(390, 160));
    fondo.setPosition(340, yInicial);
    fondo.setFillColor(Color::White);
    fondo.setOutlineThickness(1);
    fondo.setOutlineColor(Color(30, 110, 180));
    ventana.draw(fondo);

    int fila = 0;
    for (int i = desplazamiento; i < (int)ciudades.size() && fila < 4; i++)
    {
        float y = yInicial + fila * 40;
        RectangleShape opcion(Vector2f(388, 39));
        opcion.setPosition(341, y);
        opcion.setFillColor(Color(235, 243, 249));
        ventana.draw(opcion);
        dibujarTexto(ventana, 350, y + 9, ciudades[i]->ciudad.getNombre(), 14, Color(25, 65, 105));
        fila++;
    }

    if (ciudades.empty())
        dibujarTexto(ventana, 350, yInicial + 12, "No hay coincidencias.", 13, Color(100, 100, 100));
    else if (ciudades.size() > 4)
        dibujarTexto(ventana, 610, yInicial + 145, "Use la rueda para ver mas", 11, Color(100, 100, 100));
}

void PantallaVuelos::dibujarListado(RenderWindow& ventana)
{
    float ancho = ventana.getSize().x;
    RectangleShape panel(Vector2f(ancho - 785, 440));
    panel.setPosition(770, 115);
    panel.setFillColor(Color(245, 247, 250));
    panel.setOutlineThickness(1);
    panel.setOutlineColor(Color(210, 215, 220));
    ventana.draw(panel);

    dibujarTexto(ventana, 790, 125, "VUELOS REGISTRADOS", 20, Color(25, 65, 105));
    dibujarTexto(ventana, 790, 165, "CODIGO       ORIGEN       DESTINO       COSTO       DURACION", 12, Color::Black);

    NodoVuelo* actual = listaVuelos->getPrimero();
    int fila = 0;
    while (actual != nullptr)
    {
        float y = 195 + fila * 40;
        if (actual->vuelo.getCodigo() == codigoSeleccionado)
        {
            RectangleShape seleccionado(Vector2f(ancho - 810, 35));
            seleccionado.setPosition(780, y - 5);
            seleccionado.setFillColor(Color(215, 232, 245));
            ventana.draw(seleccionado);
        }

        NodoVertice* origen = grafo->buscarCiudad(actual->vuelo.getCodigoOrigen());
        NodoVertice* destino = grafo->buscarCiudad(actual->vuelo.getCodigoDestino());
        string nombreOrigen = origen == nullptr ? actual->vuelo.getCodigoOrigen() : origen->ciudad.getNombre();
        string nombreDestino = destino == nullptr ? actual->vuelo.getCodigoDestino() : destino->ciudad.getNombre();

        dibujarTexto(ventana, 790, y, actual->vuelo.getCodigo(), 12, Color::Black);
        dibujarTexto(ventana, 900, y, nombreOrigen, 12, Color::Black);
        dibujarTexto(ventana, 1030, y, nombreDestino, 12, Color::Black);
        stringstream costoFormateado;
        costoFormateado << fixed << setprecision(0) << actual->vuelo.getCosto();
        stringstream duracionFormateada;
        duracionFormateada << fixed << setprecision(0) << actual->vuelo.getDuracion();
        dibujarTexto(ventana, 1150, y, costoFormateado.str(), 12, Color::Black);
        dibujarTexto(ventana, 1270, y, duracionFormateada.str(), 12, Color::Black);

        actual = actual->siguiente;
        fila++;
    }
    dibujarTexto(ventana, 790, 565, "Total de vuelos: " + to_string(listaVuelos->getCantidad()), 15, Color::Black);
}

void PantallaVuelos::dibujarConfirmacion(RenderWindow& ventana)
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
    dibujarTexto(ventana, 575, 375, "Esta segura de eliminar este vuelo?", 16, Color::Black);
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

void PantallaVuelos::registrarVuelo()
{
    float costoNumero;
    float duracionNumero;
    stringstream flujoCosto(costo);
    stringstream flujoDuracion(duracion);

    if (codigo.empty() || codigoOrigen.empty() || codigoDestino.empty() ||
        !(flujoCosto >> costoNumero) || !(flujoDuracion >> duracionNumero) ||
        costoNumero <= 0 || duracionNumero <= 0 || codigoOrigen == codigoDestino)
    {
        mensaje = "Complete los campos y verifique los datos.";
        return;
    }

    Vuelo nuevo(codigo, codigoOrigen, codigoDestino, costoNumero, duracionNumero);
    if (listaVuelos->agregar(nuevo))
    {
        mensaje = "Vuelo registrado correctamente.";
        limpiarFormulario();
    }
    else
        mensaje = "Ya existe un vuelo con ese codigo.";
}

void PantallaVuelos::seleccionarVuelo(string codigo)
{
    NodoVuelo* actual = listaVuelos->buscar(codigo);
    if (actual == nullptr)
        return;
    codigoSeleccionado = codigo;
    this->codigo = actual->vuelo.getCodigo();
    codigoOrigen = actual->vuelo.getCodigoOrigen();
    codigoDestino = actual->vuelo.getCodigoDestino();
    stringstream flujoCosto;
    flujoCosto << fixed << setprecision(2) << actual->vuelo.getCosto();
    costo = flujoCosto.str();
    stringstream flujoDuracion;
    flujoDuracion << fixed << setprecision(2) << actual->vuelo.getDuracion();
    duracion = flujoDuracion.str();
    editando = true;
    mensaje = "Vuelo seleccionado.";
}

void PantallaVuelos::modificarVuelo()
{
    if (!editando || codigoSeleccionado.empty())
    {
        mensaje = "Seleccione un vuelo del listado.";
        return;
    }
    float costoNumero;
    float duracionNumero;
    stringstream flujoCosto(costo);
    stringstream flujoDuracion(duracion);
    if (codigoOrigen.empty() || codigoDestino.empty() ||
        !(flujoCosto >> costoNumero) || !(flujoDuracion >> duracionNumero) ||
        costoNumero <= 0 || duracionNumero <= 0 || codigoOrigen == codigoDestino)
    {
        mensaje = "Complete los campos y verifique los datos.";
        return;
    }
    if (listaVuelos->modificar(codigoSeleccionado, codigoOrigen, codigoDestino, costoNumero, duracionNumero))
    {
        seleccionarVuelo(codigoSeleccionado);
        mensaje = "Vuelo modificado correctamente.";
    }
    else
        mensaje = "No se pudo modificar el vuelo.";
}

void PantallaVuelos::eliminarVuelo()
{
    if (codigoSeleccionado.empty())
    {
        mensaje = "Seleccione un vuelo del listado.";
        return;
    }
    confirmarEliminar = true;
}

void PantallaVuelos::limpiarFormulario()
{
    codigo = "";
    codigoOrigen = "";
    codigoDestino = "";
    costo = "";
    duracion = "";
    codigoSeleccionado = "";
    campoActivo = 0;
    editando = false;
    origenDesplegado = false;
    destinoDesplegado = false;
    filtroOrigen = "";
    filtroDestino = "";
}

void PantallaVuelos::dibujarTexto(RenderWindow& ventana, float x, float y, string texto, int tamano, Color color)
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
