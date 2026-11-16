
#include "Dijkstra.h"
#include <limits>

bool Dijkstra::calcular(Grafo& grafo, string origen, string destino, bool porCosto, string*& ruta, int& cantidad, float& costoTotal, float& duracionTotal)
{
	ruta = nullptr;
	cantidad = 0;
	costoTotal = 0;
	duracionTotal = 0;
	
	int total = grafo.getCantidadVertices();
	
	if (total == 0)
		return false;
	
	NodoVertice** vertices = new NodoVertice*[total];
	float* distancias = new float[total];
	float* costos = new float[total];
	float* duraciones = new float[total];
	int* anteriores = new int[total];
	bool* visitados = new bool[total];
	
	NodoVertice* actual = grafo.getPrimero();
	
	for (int i = 0; i < total; i++)
	{
		vertices[i] = actual;
		distancias[i] = numeric_limits<float>::max();
		costos[i] = 0;
		duraciones[i] = 0;
		anteriores[i] = -1;
		visitados[i] = false;
		actual = actual->siguiente;
	}
	
	int indiceOrigen = -1;
	int indiceDestino = -1;
	
	for (int i = 0; i < total; i++)
	{
		if (vertices[i]->ciudad.getCodigo() == origen)
			indiceOrigen = i;
		
		if (vertices[i]->ciudad.getCodigo() == destino)
			indiceDestino = i;
	}
	
	if (indiceOrigen == -1 || indiceDestino == -1)
	{
		delete[] vertices;
		delete[] distancias;
		delete[] costos;
		delete[] duraciones;
		delete[] anteriores;
		delete[] visitados;
		return false;
	}
	
	distancias[indiceOrigen] = 0;
	
	for (int i = 0; i < total; i++)
	{
		int menor = -1;
		float distanciaMenor = numeric_limits<float>::max();
		
		for (int j = 0; j < total; j++)
		{
			if (!visitados[j] && distancias[j] < distanciaMenor)
			{
				distanciaMenor = distancias[j];
				menor = j;
			}
		}
		
		if (menor == -1)
			break;
		
		if (menor == indiceDestino)
			break;
		
		visitados[menor] = true;
		
		NodoArista* arista = vertices[menor]->conexiones;
		
		while (arista != nullptr)
		{
			string codigoDestino = arista->vuelo.getCodigoDestino();
			int indiceVecino = -1;
			
			for (int j = 0; j < total; j++)
			{
				if (vertices[j]->ciudad.getCodigo() == codigoDestino)
				{
					indiceVecino = j;
					break;
				}
			}
			
			if (indiceVecino != -1 && !visitados[indiceVecino])
			{
				float peso = porCosto ? arista->vuelo.getCosto() : arista->vuelo.getDuracion();
				float nuevaDistancia = distancias[menor] + peso;
				
				if (nuevaDistancia < distancias[indiceVecino])
				{
					distancias[indiceVecino] = nuevaDistancia;
					anteriores[indiceVecino] = menor;
					costos[indiceVecino] = costos[menor] + arista->vuelo.getCosto();
					duraciones[indiceVecino] = duraciones[menor] + arista->vuelo.getDuracion();
				}
			}
			
			arista = arista->siguiente;
		}
	}
	
	if (distancias[indiceDestino] == numeric_limits<float>::max())
	{
		delete[] vertices;
		delete[] distancias;
		delete[] costos;
		delete[] duraciones;
		delete[] anteriores;
		delete[] visitados;
		return false;
	}
	
	int* indicesRuta = new int[total];
	int cantidadRuta = 0;
	int posicion = indiceDestino;
	
	while (posicion != -1)
	{
		indicesRuta[cantidadRuta] = posicion;
		cantidadRuta++;
		posicion = anteriores[posicion];
	}
	
	ruta = new string[cantidadRuta];
	
	for (int i = 0; i < cantidadRuta; i++)
		ruta[i] = vertices[indicesRuta[cantidadRuta - 1 - i]]->ciudad.getNombre();
	
	cantidad = cantidadRuta;
	costoTotal = costos[indiceDestino];
	duracionTotal = duraciones[indiceDestino];
	
	delete[] indicesRuta;
	delete[] vertices;
	delete[] distancias;
	delete[] costos;
	delete[] duraciones;
	delete[] anteriores;
	delete[] visitados;
	
	return true;
}
