#ifndef KRUSKAL_H
#define KRUSKAL_H
#include "Grafo.h"
#include <string>
using namespace std;

struct ResultadoKruskal
{
	string codigoOrigen;
	string codigoDestino;
	float costo;
	float duracion;
};

class Kruskal
{
public:
	bool calcular(Grafo& grafo, ResultadoKruskal*& resultado, int& cantidad, float& duracionTotal);
};

#endif
