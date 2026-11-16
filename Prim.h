#ifndef PRIM_H
#define PRIM_H
#include "Grafo.h"
#include <string>
using namespace std;

struct ResultadoPrim
{
	string codigoOrigen;
	string codigoDestino;
	float costo;
	float duracion;
};

class Prim
{
public:
	bool calcular(Grafo& grafo, ResultadoPrim*& resultado, int& cantidad, float& costoTotal);
};

#endif
