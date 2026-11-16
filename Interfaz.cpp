#include "Interfaz.h"
#include "Aplicacion.h"

Interfaz::Interfaz(Grafo* grafo)
{
    this->grafo = grafo;
}

void Interfaz::iniciar()
{
    Aplicacion aplicacion(grafo);
    aplicacion.ejecutar();
}
