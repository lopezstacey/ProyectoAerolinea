#include "Kruskal.h"

namespace
{
	void mezclar(AristaExpansion* a, AristaExpansion* aux, int izq, int medio, int der)
	{
		int i = izq;
		int j = medio + 1;
		int k = izq;
		
		while (i <= medio && j <= der)
		{
			if (a[i].duracion <= a[j].duracion)
				aux[k++] = a[i++];
			else
				aux[k++] = a[j++];
		}
		
		while (i <= medio)
			aux[k++] = a[i++];
		
		while (j <= der)
			aux[k++] = a[j++];
		
		for (int p = izq; p <= der; p++)
			a[p] = aux[p];
	}
	
	void ordenarPorDuracion(AristaExpansion* a, AristaExpansion* aux, int izq, int der)
	{
		if (izq >= der)
			return;
		
		int medio = izq + (der - izq) / 2;
		ordenarPorDuracion(a, aux, izq, medio);
		ordenarPorDuracion(a, aux, medio + 1, der);
		mezclar(a, aux, izq, medio, der);
	}
	
	int buscarRaiz(int* padre, int x)
	{
		int raiz = x;
		
		while (padre[raiz] != raiz)
			raiz = padre[raiz];
		
		while (padre[x] != raiz)
		{
			int siguiente = padre[x];
			padre[x] = raiz;
			x = siguiente;
		}
		
		return raiz;
	}
	
	void unir(int* padre, int* rango, int a, int b)
	{
		if (rango[a] < rango[b])
			padre[a] = b;
		else if (rango[a] > rango[b])
			padre[b] = a;
		else
		{
			padre[b] = a;
			rango[a]++;
		}
	}
}

bool Kruskal::calcular(Grafo& grafo, ResultadoExpansion& r)
{
	NodoArista** mejor = nullptr;
	
	if (!prepararGrafoNoDirigido(grafo, false, r, mejor))
		return false;
	
	int n = r.cantidadCiudades;
	r.indiceInicio = -1;
	
	int total = 0;
	
	for (int i = 0; i < n; i++)
	{
		for (int j = i + 1; j < n; j++)
		{
			if (mejor[i * n + j] != nullptr)
				total++;
		}
	}
	
	r.pasos = new AristaExpansion[total > 0 ? total : 1];
	r.cantidadPasos = 0;
	
	int cantidad = 0;
	
	for (int i = 0; i < n; i++)
	{
		for (int j = i + 1; j < n; j++)
		{
			NodoArista* vuelo = mejor[i * n + j];
			
			if (vuelo == nullptr)
				continue;
			
			AristaExpansion& a = r.pasos[cantidad++];
			a.origen = i;
			a.destino = j;
			a.codigoVuelo = vuelo->vuelo.getCodigo();
			a.costo = vuelo->vuelo.getCosto();
			a.duracion = vuelo->vuelo.getDuracion();
			a.aceptada = false;
		}
	}
	
	delete[] mejor;
	
	AristaExpansion* aux = new AristaExpansion[total > 0 ? total : 1];
	ordenarPorDuracion(r.pasos, aux, 0, total - 1);
	delete[] aux;
	
	int* padre = new int[n];
	int* rango = new int[n];
	
	for (int i = 0; i < n; i++)
	{
		padre[i] = i;
		rango[i] = 0;
	}
	
	int aceptadas = 0;
	int procesadas = 0;
	
	while (procesadas < total && aceptadas < n - 1)
	{
		AristaExpansion& a = r.pasos[procesadas];
		int raizA = buscarRaiz(padre, a.origen);
		int raizB = buscarRaiz(padre, a.destino);
		
		if (raizA != raizB)
		{
			unir(padre, rango, raizA, raizB);
			a.aceptada = true;
			aceptadas++;
		}
		
		procesadas++;
	}
	
	r.cantidadPasos = procesadas;
	
	delete[] padre;
	delete[] rango;
	
	finalizarResultado(r);
	return true;
}
