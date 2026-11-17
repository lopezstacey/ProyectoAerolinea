#include "BellmanFord.h"
#include <limits>

bool BellmanFord::calcular(Grafo& grafo, string origen, bool porCosto, ResultadoBellmanFord*& resultados, int& cantidad)
{
	resultados = nullptr;
	cantidad = 0;
	
	int total = grafo.getCantidadVertices();
	if (total <= 0)
		return false;
	
	NodoVertice** vertices = new NodoVertice*[total];
	float* distancias = new float[total];
	float* costos = new float[total];
	float* duraciones = new float[total];
	int* anteriores = new int[total];
	
	NodoVertice* actual = grafo.getPrimero();
	for (int i = 0; i < total; i++)
	{
		vertices[i] = actual;
		distancias[i] = numeric_limits<float>::max();
		costos[i] = 0.0f;
		duraciones[i] = 0.0f;
		anteriores[i] = -1;
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
		delete[] anteriores;
		return false;
	}
	
	distancias[indiceOrigen] = 0.0f;
	
	for (int vuelta = 0; vuelta < total - 1; vuelta++)
	{
		bool huboCambios = false;
		
		for (int i = 0; i < total; i++)
		{
			if (distancias[i] == numeric_limits<float>::max())
				continue;
			
			NodoArista* arista = vertices[i]->conexiones;
			while (arista != nullptr)
			{
				int destino = -1;
				for (int j = 0; j < total; j++)
				{
					if (vertices[j]->ciudad.getCodigo() ==
						arista->vuelo.getCodigoDestino())
					{
						destino = j;
						break;
					}
				}
				
				if (destino != -1)
				{
					float peso = porCosto ? arista->vuelo.getCosto()
						: arista->vuelo.getDuracion();
					float nueva = distancias[i] + peso;
					
					if (nueva < distancias[destino])
					{
						distancias[destino] = nueva;
						costos[destino] = costos[i] + arista->vuelo.getCosto();
						duraciones[destino] = duraciones[i] + arista->vuelo.getDuracion();
						anteriores[destino] = i;
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
			int destino = -1;
			for (int j = 0; j < total; j++)
			{
				if (vertices[j]->ciudad.getCodigo() ==
					arista->vuelo.getCodigoDestino())
				{
					destino = j;
					break;
				}
			}
			
			if (destino != -1)
			{
				float peso = porCosto ? arista->vuelo.getCosto()
					: arista->vuelo.getDuracion();
				if (distancias[i] + peso < distancias[destino])
				{
					delete[] vertices;
					delete[] distancias;
					delete[] costos;
					delete[] duraciones;
					delete[] anteriores;
					return false;
				}
			}
			arista = arista->siguiente;
		}
	}
	
	for (int i = 0; i < total; i++)
		if (distancias[i] != numeric_limits<float>::max())
			cantidad++;
	
	resultados = new ResultadoBellmanFord[cantidad];
	int posResultado = 0;
	
	for (int i = 0; i < total; i++)
	{
		if (distancias[i] == numeric_limits<float>::max())
			continue;
		
		ResultadoBellmanFord& r = resultados[posResultado];
		r.codigoCiudad = vertices[i]->ciudad.getCodigo();
		r.nombreCiudad = vertices[i]->ciudad.getNombre();
		r.distancia = distancias[i];
		r.costoTotal = costos[i];
		r.duracionTotal = duraciones[i];
		r.ruta = nullptr;
		r.cantidadRuta = 0;
		
		int* indices = new int[total];
		int cantidadIndices = 0;
		int cursor = i;
		
		while (cursor != -1 && cantidadIndices < total)
		{
			indices[cantidadIndices++] = cursor;
			cursor = anteriores[cursor];
		}
		
		r.cantidadRuta = cantidadIndices;
		r.ruta = new string[cantidadIndices];
		
		for (int k = 0; k < cantidadIndices; k++)
			r.ruta[k] = vertices[indices[cantidadIndices - 1 - k]]->ciudad.getNombre();
		
		delete[] indices;
		posResultado++;
	}
	
	delete[] vertices;
	delete[] distancias;
	delete[] costos;
	delete[] duraciones;
	delete[] anteriores;
	return true;
}
