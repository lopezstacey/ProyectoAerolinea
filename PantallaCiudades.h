#ifndef PANTALLACIUDADES_H
#define PANTALLACIUDADES_H

#include "Pantalla.h"
#include "Grafo.h"
#include <SFML/Graphics.hpp>
#include <string>

using namespace sf;
using namespace std;

class PantallaCiudades : public Pantalla
{
private:
    Grafo* grafo;
    Font fuente;
    Texture texturaMapa;
    Sprite mapa;
    bool fuenteCargada;
    bool mapaCargado;

    string codigo;
    string nombre;
    string codigoSeleccionado;
    string mensaje;
    int campoActivo;
    bool editando;
    bool ubicacionElegida;
    float posicionX;
    float posicionY;

    float mapaX;
    float mapaY;
    float mapaAncho;
    float mapaAlto;

    void dibujarTexto(RenderWindow& ventana, float x, float y, const string& texto, unsigned int tamano, Color color);
    void dibujarPanel(RenderWindow& ventana, float x, float y, float ancho, float alto, Color color);
    void dibujarBoton(RenderWindow& ventana, float x, float y, float ancho, float alto, const string& texto, Color color);
    bool dentroDe(int x, int y, float px, float py, float ancho, float alto);
    void seleccionarCiudad(float x, float y);
    void cargarCiudadSeleccionada();
    void guardarCiudad();
    void limpiarFormulario();
    void dibujarMarcadores(RenderWindow& ventana);
    void dibujarListado(RenderWindow& ventana);

public:
    PantallaCiudades(Grafo* grafo);
    void procesarEventos(RenderWindow& ventana, Event evento) override;
    void actualizar(float tiempo) override;
    void dibujar(RenderWindow& ventana) override;
};

#endif
