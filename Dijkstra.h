#ifndef DIJKSTRA_H
#define DIJKSTRA_H
#include "Grafo.h"
#include <string>

using namespace std;

class Dijkstra
{
public:
	bool calcular(Grafo& grafo, string origen, string destino, bool porCosto, string*& ruta, int& cantidad, float& costoTotal, float& duracionTotal);
	bool calcular(Grafo& grafo, string origen, string destino, bool porCosto, string*& ruta, int& cantidad, float& costoTotal, float& duracionTotal, string*& codigosVuelos);
};

#endif
