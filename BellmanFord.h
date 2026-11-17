#ifndef BELLMANFORD_H
#define BELLMANFORD_H
#include "Grafo.h"
#include <string>

using namespace std;

struct ResultadoBellmanFord
{
	string codigoCiudad;
	string nombreCiudad;
	float distancia;
	float costoTotal;
	float duracionTotal;
	string* ruta;
	int cantidadRuta;
};

class BellmanFord
{
public:
	bool calcular(Grafo& grafo, string origen, bool porCosto, ResultadoBellmanFord*& resultados, int& cantidad);
};

#endif
