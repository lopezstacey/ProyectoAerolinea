
#include "Kruskal.h"

static int buscarRaiz(int padre[], int posicion)
{
	if (padre[posicion] != posicion)
		padre[posicion] = buscarRaiz(padre, padre[posicion]);
	
	return padre[posicion];
}

static bool unirConjuntos(int padre[], int rango[], int origen, int destino)
{
	int raizOrigen = buscarRaiz(padre, origen);
	int raizDestino = buscarRaiz(padre, destino);
	
	if (raizOrigen == raizDestino)
		return false;
	
	if (rango[raizOrigen] < rango[raizDestino])
		padre[raizOrigen] = raizDestino;
	else if (rango[raizOrigen] > rango[raizDestino])
		padre[raizDestino] = raizOrigen;
	else
	{
		padre[raizDestino] = raizOrigen;
		rango[raizOrigen]++;
	}
	
	return true;
}

bool Kruskal::calcular(Grafo& grafo, ResultadoKruskal*& resultado, int& cantidad, float& duracionTotal)
{
	resultado = nullptr;
	cantidad = 0;
	duracionTotal = 0;
	
	int totalVertices = grafo.getCantidadVertices();
	
	if (totalVertices == 0)
		return false;
	
	if (totalVertices == 1)
		return true;
	
	int totalAristas = 0;
	NodoVertice* vertice = grafo.getPrimero();
	
	while (vertice != nullptr)
	{
		NodoArista* arista = vertice->conexiones;
		
		while (arista != nullptr)
		{
			totalAristas++;
			arista = arista->siguiente;
		}
		
		vertice = vertice->siguiente;
	}
	
	if (totalAristas == 0)
		return false;
	
	ResultadoKruskal* aristas = new ResultadoKruskal[totalAristas];
	NodoVertice** vertices = new NodoVertice*[totalVertices];
	
	vertice = grafo.getPrimero();
	
	for (int i = 0; i < totalVertices; i++)
	{
		vertices[i] = vertice;
		vertice = vertice->siguiente;
	}
	
	int posicion = 0;
	
	for (int i = 0; i < totalVertices; i++)
	{
		NodoArista* arista = vertices[i]->conexiones;
		
		while (arista != nullptr)
		{
			aristas[posicion].codigoOrigen = arista->vuelo.getCodigoOrigen();
			aristas[posicion].codigoDestino = arista->vuelo.getCodigoDestino();
			aristas[posicion].costo = arista->vuelo.getCosto();
			aristas[posicion].duracion = arista->vuelo.getDuracion();
			posicion++;
			
			arista = arista->siguiente;
		}
	}
	
	for (int i = 1; i < totalAristas; i++)
	{
		ResultadoKruskal actual = aristas[i];
		int j = i - 1;
		
		while (j >= 0 && aristas[j].duracion > actual.duracion)
		{
			aristas[j + 1] = aristas[j];
			j--;
		}
		
		aristas[j + 1] = actual;
	}
	
	int* padre = new int[totalVertices];
	int* rango = new int[totalVertices];
	
	for (int i = 0; i < totalVertices; i++)
	{
		padre[i] = i;
		rango[i] = 0;
	}
	
	ResultadoKruskal* seleccionadas = new ResultadoKruskal[totalVertices - 1];
	
	for (int i = 0; i < totalAristas && cantidad < totalVertices - 1; i++)
	{
		int indiceOrigen = -1;
		int indiceDestino = -1;
		
		for (int j = 0; j < totalVertices; j++)
		{
			if (vertices[j]->ciudad.getCodigo() == aristas[i].codigoOrigen)
				indiceOrigen = j;
			
			if (vertices[j]->ciudad.getCodigo() == aristas[i].codigoDestino)
				indiceDestino = j;
		}
		
		if (indiceOrigen == -1 || indiceDestino == -1 || indiceOrigen == indiceDestino)
			continue;
		
		if (unirConjuntos(padre, rango, indiceOrigen, indiceDestino))
		{
			seleccionadas[cantidad] = aristas[i];
			duracionTotal += aristas[i].duracion;
			cantidad++;
		}
	}
	
	delete[] aristas;
	delete[] vertices;
	delete[] padre;
	delete[] rango;
	
	if (cantidad != totalVertices - 1)
	{
		delete[] seleccionadas;
		return false;
	}
	
	resultado = seleccionadas;
	
	return true;
}
