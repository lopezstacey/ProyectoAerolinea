#ifndef PRIM_H
#define PRIM_H
#include "ArbolExpansion.h"
#include <string>

using namespace std;

class Prim
{
public:
	bool calcular(Grafo& grafo, string codigoInicio, ResultadoExpansion& resultado);
};

#endif
