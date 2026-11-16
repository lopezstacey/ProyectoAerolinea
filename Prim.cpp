#include "Prim.h"
#include <limits>

bool Prim::calcular(Grafo& grafo, ResultadoPrim*& resultado, int& cantidad, float& costoTotal)
{
	resultado = nullptr;
	cantidad = 0;
	costoTotal = 0;
	
	int total = grafo.getCantidadVertices();
	
	if (total == 0)
		return false;
	
	if (total == 1)
		return true;
	
	NodoVertice** vertices = new NodoVertice*[total];
	bool* visitados = new bool[total];
	ResultadoPrim* seleccionados = new ResultadoPrim[total - 1];
	
	NodoVertice* actual = grafo.getPrimero();
	
	for (int i = 0; i < total; i++)
	{
		vertices[i] = actual;
		visitados[i] = false;
		actual = actual->siguiente;
	}
	
	visitados[0] = true;
	
	while (cantidad < total - 1)
	{
		float menorCosto = numeric_limits<float>::max();
		int indiceOrigen = -1;
		int indiceDestino = -1;
		NodoArista* mejorArista = nullptr;
		
		for (int i = 0; i < total; i++)
		{
			NodoArista* arista = vertices[i]->conexiones;
			
			while (arista != nullptr)
			{
				int destino = -1;
				
				for (int j = 0; j < total; j++)
				{
					if (vertices[j]->ciudad.getCodigo() == arista->vuelo.getCodigoDestino())
					{
						destino = j;
						break;
					}
				}
				
				if (destino != -1 && i != destino && visitados[i] != visitados[destino])
				{
					if (arista->vuelo.getCosto() < menorCosto)
					{
						menorCosto = arista->vuelo.getCosto();
						indiceOrigen = i;
						indiceDestino = destino;
						mejorArista = arista;
					}
				}
				
				arista = arista->siguiente;
			}
		}
		
		if (mejorArista == nullptr)
		{
			delete[] vertices;
			delete[] visitados;
			delete[] seleccionados;
			return false;
		}
		
		seleccionados[cantidad].codigoOrigen = mejorArista->vuelo.getCodigoOrigen();
		seleccionados[cantidad].codigoDestino = mejorArista->vuelo.getCodigoDestino();
		seleccionados[cantidad].costo = mejorArista->vuelo.getCosto();
		seleccionados[cantidad].duracion = mejorArista->vuelo.getDuracion();
		
		costoTotal += mejorArista->vuelo.getCosto();
		visitados[indiceOrigen] = true;
		visitados[indiceDestino] = true;
		cantidad++;
	}
	
	resultado = seleccionados;
	
	delete[] vertices;
	delete[] visitados;
	
	return true;
}
