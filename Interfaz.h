#ifndef INTERFAZ_H
#define INTERFAZ_H

#include "Grafo.h"

class Interfaz
{
private:
    Grafo* grafo;

public:
    Interfaz(Grafo* grafo);
    void iniciar();
};

#endif
