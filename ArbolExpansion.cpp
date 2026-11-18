#include "ArbolExpansion.h"

ResultadoExpansion::ResultadoExpansion()
{
	ciudades = nullptr;
	cantidadCiudades = 0;
	pasos = nullptr;
	cantidadPasos = 0;
	cantidadAceptadas = 0;
	costoTotal = 0;
	duracionTotal = 0;
	componentes = 0;
	indiceInicio = -1;
}

ResultadoExpansion::~ResultadoExpansion()
{
	liberar();
}

void ResultadoExpansion::liberar()
{
	delete[] ciudades;
	delete[] pasos;
	ciudades = nullptr;
	pasos = nullptr;
	cantidadCiudades = 0;
	cantidadPasos = 0;
	cantidadAceptadas = 0;
	costoTotal = 0;
	duracionTotal = 0;
	componentes = 0;
	indiceInicio = -1;
}

int buscarIndiceCiudad(const ResultadoExpansion& r, const string& codigo)
{
	for (int i = 0; i < r.cantidadCiudades; i++)
	{
		if (r.ciudades[i].codigo == codigo)
			return i;
	}
	
	return -1;
}

bool prepararGrafoNoDirigido(Grafo& grafo, bool porCosto, ResultadoExpansion& r, NodoArista**& mejor)
{
	mejor = nullptr;
	r.liberar();
	
	int n = grafo.getCantidadVertices();
	
	if (n <= 0)
		return false;
	
	r.cantidadCiudades = n;
	r.ciudades = new CiudadExpansion[n];
	
	NodoVertice** vertices = new NodoVertice*[n];
	NodoVertice* actual = grafo.getPrimero();
	
	for (int i = 0; i < n; i++)
	{
		vertices[i] = actual;
		r.ciudades[i].codigo = actual->ciudad.getCodigo();
		r.ciudades[i].nombre = actual->ciudad.getNombre();
		r.ciudades[i].x = actual->ciudad.getPosicionX();
		r.ciudades[i].y = actual->ciudad.getPosicionY();
		actual = actual->siguiente;
	}
	
	mejor = new NodoArista*[n * n];
	
	for (int i = 0; i < n * n; i++)
		mejor[i] = nullptr;
	
	for (int i = 0; i < n; i++)
	{
		NodoArista* arista = vertices[i]->conexiones;
		
		while (arista != nullptr)
		{
			int j = buscarIndiceCiudad(r, arista->vuelo.getCodigoDestino());
			
			if (j >= 0 && j != i)
			{
				NodoArista* actualPar = mejor[i * n + j];
				float pesoNuevo = porCosto ? arista->vuelo.getCosto() : arista->vuelo.getDuracion();
				bool mejora = actualPar == nullptr;
				
				if (!mejora)
				{
					float pesoActual = porCosto ? actualPar->vuelo.getCosto() : actualPar->vuelo.getDuracion();
					mejora = pesoNuevo < pesoActual;
				}
				
				if (mejora)
				{
					mejor[i * n + j] = arista;
					mejor[j * n + i] = arista;
				}
			}
			
			arista = arista->siguiente;
		}
	}
	
	delete[] vertices;
	return true;
}

void finalizarResultado(ResultadoExpansion& r)
{
	r.costoTotal = 0;
	r.duracionTotal = 0;
	r.cantidadAceptadas = 0;
	
	for (int i = 0; i < r.cantidadPasos; i++)
	{
		if (r.pasos[i].aceptada)
		{
			r.costoTotal += r.pasos[i].costo;
			r.duracionTotal += r.pasos[i].duracion;
			r.cantidadAceptadas++;
		}
	}
	r.componentes = r.cantidadCiudades - r.cantidadAceptadas;
}

ArbolExpansion::ArbolExpansion()
{
	nodos = nullptr;
	cantidadNodos = 0;
	raices = nullptr;
	cantidadRaices = 0;
}

ArbolExpansion::~ArbolExpansion()
{
	limpiar();
}

void ArbolExpansion::limpiar()
{
	for (int i = 0; i < cantidadNodos; i++)
		delete nodos[i];
	
	delete[] nodos;
	delete[] raices;
	nodos = nullptr;
	raices = nullptr;
	cantidadNodos = 0;
	cantidadRaices = 0;
}

void ArbolExpansion::construir(const ResultadoExpansion& r, int indiceRaiz)
{
	limpiar();
	
	int n = r.cantidadCiudades;
	
	if (n <= 0)
		return;
	
	cantidadNodos = n;
	nodos = new NodoExpansion*[n];
	
	for (int i = 0; i < n; i++)
	{
		NodoExpansion* nodo = new NodoExpansion;
		nodo->ciudad = i;
		nodo->codigo = r.ciudades[i].codigo;
		nodo->nombre = r.ciudades[i].nombre;
		nodo->x = r.ciudades[i].x;
		nodo->y = r.ciudades[i].y;
		nodo->costoPadre = 0;
		nodo->duracionPadre = 0;
		nodo->primerHijo = nullptr;
		nodo->hermano = nullptr;
		nodo->px = 0;
		nodo->py = 0;
		nodos[i] = nodo;
	}
	
	bool* visitado = new bool[n];
	int* cola = new int[n];
	NodoExpansion** ultimoHijo = new NodoExpansion*[n];
	
	for (int i = 0; i < n; i++)
	{
		visitado[i] = false;
		ultimoHijo[i] = nullptr;
	}
	
	raices = new NodoExpansion*[n];
	cantidadRaices = 0;
	
	for (int intento = -1; intento < n; intento++)
	{
		int raiz = intento == -1 ? indiceRaiz : intento;
		
		if (raiz < 0 || raiz >= n || visitado[raiz])
			continue;
		
		raices[cantidadRaices++] = nodos[raiz];
		visitado[raiz] = true;
		
		int inicio = 0;
		int fin = 0;
		cola[fin++] = raiz;
		
		while (inicio < fin)
		{
			int u = cola[inicio++];
			
			for (int k = 0; k < r.cantidadPasos; k++)
			{
				const AristaExpansion& a = r.pasos[k];
				
				if (!a.aceptada)
					continue;
				
				int v = -1;
				
				if (a.origen == u)
					v = a.destino;
				else if (a.destino == u)
					v = a.origen;
				
				if (v < 0 || visitado[v])
					continue;
				
				visitado[v] = true;
				nodos[v]->costoPadre = a.costo;
				nodos[v]->duracionPadre = a.duracion;
				
				if (ultimoHijo[u] == nullptr)
					nodos[u]->primerHijo = nodos[v];
				else
					ultimoHijo[u]->hermano = nodos[v];
				
				ultimoHijo[u] = nodos[v];
				cola[fin++] = v;
			}
		}
	}
	
	delete[] visitado;
	delete[] cola;
	delete[] ultimoHijo;
}

int ArbolExpansion::getCantidadNodos() const
{
	return cantidadNodos;
}

int ArbolExpansion::getCantidadRaices() const
{
	return cantidadRaices;
}

NodoExpansion* ArbolExpansion::getRaiz(int i) const
{
	return (i >= 0 && i < cantidadRaices) ? raices[i] : nullptr;
}

NodoExpansion* ArbolExpansion::getNodo(int indiceCiudad) const
{
	return (indiceCiudad >= 0 && indiceCiudad < cantidadNodos) ? nodos[indiceCiudad] : nullptr;
}

int ArbolExpansion::altura(NodoExpansion* nodo) const
{
	if (nodo == nullptr)
		return 0;
	
	int maxima = 0;
	
	for (NodoExpansion* h = nodo->primerHijo; h != nullptr; h = h->hermano)
	{
		int a = altura(h);
		
		if (a > maxima)
			maxima = a;
	}
	
	return 1 + maxima;
}

int ArbolExpansion::getAltura() const
{
	int maxima = 0;
	
	for (int i = 0; i < cantidadRaices; i++)
	{
		int a = altura(raices[i]);
		
		if (a > maxima)
			maxima = a;
	}
	
	return maxima;
}

int ArbolExpansion::contarHojas() const
{
	int hojas = 0;
	
	for (int i = 0; i < cantidadNodos; i++)
	{
		if (nodos[i]->primerHijo == nullptr)
			hojas++;
	}
	
	return hojas;
}

void ArbolExpansion::preorden(NodoExpansion* nodo, NodoExpansion** destino, int& n) const
{
	destino[n++] = nodo;
	
	for (NodoExpansion* h = nodo->primerHijo; h != nullptr; h = h->hermano)
		preorden(h, destino, n);
}

void ArbolExpansion::postorden(NodoExpansion* nodo, NodoExpansion** destino, int& n) const
{
	for (NodoExpansion* h = nodo->primerHijo; h != nullptr; h = h->hermano)
		postorden(h, destino, n);
	
	destino[n++] = nodo;
}

int ArbolExpansion::recorrerPreorden(NodoExpansion** destino) const
{
	int n = 0;
	
	for (int i = 0; i < cantidadRaices; i++)
		preorden(raices[i], destino, n);
	
	return n;
}

int ArbolExpansion::recorrerPostorden(NodoExpansion** destino) const
{
	int n = 0;
	
	for (int i = 0; i < cantidadRaices; i++)
		postorden(raices[i], destino, n);
	
	return n;
}

int ArbolExpansion::recorrerAnchura(NodoExpansion** destino) const
{
	int n = 0;
	
	for (int i = 0; i < cantidadRaices; i++)
	{
		int cabeza = n;
		destino[n++] = raices[i];
		
		while (cabeza < n)
		{
			NodoExpansion* actual = destino[cabeza++];
			
			for (NodoExpansion* h = actual->primerHijo; h != nullptr; h = h->hermano)
				destino[n++] = h;
		}
	}
	
	return n;
}
