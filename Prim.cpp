#include "Prim.h"
#include <limits>

bool Prim::calcular(Grafo& grafo, string codigoInicio, ResultadoExpansion& r)
{
	NodoArista** mejor = nullptr;
	
	if (!prepararGrafoNoDirigido(grafo, true, r, mejor))
		return false;
	
	int n = r.cantidadCiudades;
	const float INFINITO = numeric_limits<float>::max();
	
	int inicio = buscarIndiceCiudad(r, codigoInicio);
	
	if (inicio < 0)
		inicio = 0;
	
	r.indiceInicio = inicio;
	r.pasos = new AristaExpansion[n];
	r.cantidadPasos = 0;
	
	bool* enArbol = new bool[n];
	float* menorCosto = new float[n];
	int* padre = new int[n];
	
	for (int i = 0; i < n; i++)
	{
		enArbol[i] = false;
		menorCosto[i] = INFINITO;
		padre[i] = -1;
	}
	
	int raiz = inicio;
	
	while (raiz != -1)
	{
		enArbol[raiz] = true;
		
		for (int j = 0; j < n; j++)
		{
			if (enArbol[j])
				continue;
			
			NodoArista* vuelo = mejor[raiz * n + j];
			menorCosto[j] = vuelo != nullptr ? vuelo->vuelo.getCosto() : INFINITO;
			padre[j] = vuelo != nullptr ? raiz : -1;
		}
		
		while (true)
		{
			int elegida = -1;
			
			for (int j = 0; j < n; j++)
			{
				if (!enArbol[j] && menorCosto[j] < INFINITO &&
					(elegida == -1 || menorCosto[j] < menorCosto[elegida]))
					elegida = j;
			}
			
			if (elegida == -1)
				break;
			
			NodoArista* vuelo = mejor[padre[elegida] * n + elegida];
			AristaExpansion& paso = r.pasos[r.cantidadPasos++];
			paso.origen = padre[elegida];
			paso.destino = elegida;
			paso.codigoVuelo = vuelo->vuelo.getCodigo();
			paso.costo = vuelo->vuelo.getCosto();
			paso.duracion = vuelo->vuelo.getDuracion();
			paso.aceptada = true;
			
			enArbol[elegida] = true;
			
			for (int k = 0; k < n; k++)
			{
				if (enArbol[k])
					continue;
				
				NodoArista* candidata = mejor[elegida * n + k];
				
				if (candidata != nullptr && candidata->vuelo.getCosto() < menorCosto[k])
				{
					menorCosto[k] = candidata->vuelo.getCosto();
					padre[k] = elegida;
				}
			}
		}
		raiz = -1;
		
		for (int i = 0; i < n; i++)
		{
			if (!enArbol[i])
			{
				raiz = i;
				break;
			}
		}
	}
	
	delete[] enArbol;
	delete[] menorCosto;
	delete[] padre;
	delete[] mejor;
	
	finalizarResultado(r);
	return true;
}
