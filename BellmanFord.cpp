#include "BellmanFord.h"
#include <limits>

bool BellmanFord::calcular(Grafo& grafo, string origen, bool porCosto, ResultadoBellmanFord*& resultados, int& cantidad)
{
	resultados = nullptr;
	cantidad = 0;
	
	int total = grafo.getCantidadVertices();
	
	if (total == 0)
		return false;
	
	NodoVertice** vertices = new NodoVertice*[total];
	float* distancias = new float[total];
	float* costos = new float[total];
	float* duraciones = new float[total];
	
	NodoVertice* actual = grafo.getPrimero();
	
	for (int i = 0; i < total; i++)
	{
		vertices[i] = actual;
		distancias[i] = numeric_limits<float>::max();
		costos[i] = 0;
		duraciones[i] = 0;
		actual = actual->siguiente;
	}
	
	int indiceOrigen = -1;
	
	for (int i = 0; i < total; i++)
	{
		if (vertices[i]->ciudad.getCodigo() == origen)
		{
			indiceOrigen = i;
			break;
		}
	}
	
	if (indiceOrigen == -1)
	{
		delete[] vertices;
		delete[] distancias;
		delete[] costos;
		delete[] duraciones;
		return false;
	}
	
	distancias[indiceOrigen] = 0;
	
	for (int i = 0; i < total - 1; i++)
	{
		bool huboCambios = false;
		
		for (int j = 0; j < total; j++)
		{
			if (distancias[j] == numeric_limits<float>::max())
				continue;
			
			NodoArista* arista = vertices[j]->conexiones;
			
			while (arista != nullptr)
			{
				int indiceDestino = -1;
				
				for (int k = 0; k < total; k++)
				{
					if (vertices[k]->ciudad.getCodigo() == arista->vuelo.getCodigoDestino())
					{
						indiceDestino = k;
						break;
					}
				}
				
				if (indiceDestino != -1)
				{
					float peso = porCosto ? arista->vuelo.getCosto() : arista->vuelo.getDuracion();
					float nuevaDistancia = distancias[j] + peso;
					
					if (nuevaDistancia < distancias[indiceDestino])
					{
						distancias[indiceDestino] = nuevaDistancia;
						costos[indiceDestino] = costos[j] + arista->vuelo.getCosto();
						duraciones[indiceDestino] = duraciones[j] + arista->vuelo.getDuracion();
						huboCambios = true;
					}
				}
				
				arista = arista->siguiente;
			}
		}
		
		if (!huboCambios)
			break;
	}
	
	for (int i = 0; i < total; i++)
	{
		if (distancias[i] == numeric_limits<float>::max())
			continue;
		
		NodoArista* arista = vertices[i]->conexiones;
		
		while (arista != nullptr)
		{
			int indiceDestino = -1;
			
			for (int j = 0; j < total; j++)
			{
				if (vertices[j]->ciudad.getCodigo() == arista->vuelo.getCodigoDestino())
				{
					indiceDestino = j;
					break;
				}
			}
			
			if (indiceDestino != -1)
			{
				float peso = porCosto ? arista->vuelo.getCosto() : arista->vuelo.getDuracion();
				
				if (distancias[i] + peso < distancias[indiceDestino])
				{
					delete[] vertices;
					delete[] distancias;
					delete[] costos;
					delete[] duraciones;
					return false;
				}
			}
			
			arista = arista->siguiente;
		}
	}
	
	for (int i = 0; i < total; i++)
	{
		if (distancias[i] != numeric_limits<float>::max())
			cantidad++;
	}
	
	resultados = new ResultadoBellmanFord[cantidad];
	
	int posicion = 0;
	
	for (int i = 0; i < total; i++)
	{
		if (distancias[i] != numeric_limits<float>::max())
		{
			resultados[posicion].codigoCiudad = vertices[i]->ciudad.getCodigo();
			resultados[posicion].nombreCiudad = vertices[i]->ciudad.getNombre();
			resultados[posicion].distancia = distancias[i];
			resultados[posicion].costoTotal = costos[i];
			resultados[posicion].duracionTotal = duraciones[i];
			posicion++;
		}
	}
	
	delete[] vertices;
	delete[] distancias;
	delete[] costos;
	delete[] duraciones;
	
	return true;
}
