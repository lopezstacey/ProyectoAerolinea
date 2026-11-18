#include "Grafo.h"
#include <cmath>
#include <cctype>

using namespace std;

static string limpiarTextoGrafo(const string& texto)
{
	size_t inicio = texto.find_first_not_of(" \t\r\n");
	
	if (inicio == string::npos)
		return "";
	
	size_t fin = texto.find_last_not_of(" \t\r\n");
	
	return texto.substr(inicio, fin - inicio + 1);
}

Grafo::Grafo()
{
	primero = nullptr;
	cantidadVertices = 0;
	arbolCiudades.limpiar();
}

ArbolABB* Grafo::getArbolCiudades()
{
	return &arbolCiudades;
}

void Grafo::reconstruirArbol()
{
	arbolCiudades.limpiar();
	
	NodoVertice* actual = primero;
	
	while (actual != nullptr)
	{
		arbolCiudades.insertar(actual->ciudad.getNombre(), actual->ciudad.getCodigo());
		actual = actual->siguiente;
	}
}

Grafo::~Grafo()
{
	limpiar();
}

bool Grafo::agregarCiudad(Ciudad ciudad)
{
	string codigo = limpiarTextoGrafo(ciudad.getCodigo());
	string nombre = limpiarTextoGrafo(ciudad.getNombre());
	
	if (codigo.empty() || nombre.empty())
		return false;
	
	if (!isfinite(ciudad.getPosicionX()) ||
		!isfinite(ciudad.getPosicionY()))
		return false;
	
	if (ciudad.getPosicionX() < 0 ||
		ciudad.getPosicionY() < 0)
		return false;
	
	if (existeCiudad(codigo))
		return false;
	
	if (arbolCiudades.contiene(nombre))
		return false;
	
	Ciudad ciudadValidada(codigo, nombre, ciudad.getPosicionX(), ciudad.getPosicionY());
	
	NodoVertice* nuevo = new NodoVertice(ciudadValidada);
	
	if (primero == nullptr)
		primero = nuevo;
	else
	{
		NodoVertice* actual = primero;
		
		while (actual->siguiente != nullptr)
			actual = actual->siguiente;
		
		actual->siguiente = nuevo;
	}
	
	cantidadVertices++;
	arbolCiudades.insertar(nombre, codigo);
	
	return true;
}

bool Grafo::agregarVuelo(Vuelo vuelo)
{
	string codigoOrigen = limpiarTextoGrafo(vuelo.getCodigoOrigen());
	string codigoDestino = limpiarTextoGrafo(vuelo.getCodigoDestino());
	
	NodoVertice* origen = buscarCiudad(codigoOrigen);
	NodoVertice* destino = buscarCiudad(codigoDestino);
	
	if (origen == nullptr || destino == nullptr)
		return false;
	
	if (codigoOrigen.empty() || codigoDestino.empty() ||
		codigoOrigen == codigoDestino)
		return false;
	
	if (vuelo.getCosto() <= 0 || vuelo.getDuracion() <= 0)
		return false;
	
	if (buscarVuelo(vuelo.getCodigo()) != nullptr)
		return false;
	
	Vuelo vueloValidado(vuelo.getCodigo(), codigoOrigen, codigoDestino, vuelo.getCosto(), vuelo.getDuracion());
	
	NodoArista* nuevo = new NodoArista(vueloValidado);
	
	if (origen->conexiones == nullptr)
		origen->conexiones = nuevo;
	else
	{
		NodoArista* actual = origen->conexiones;
		
		while (actual->siguiente != nullptr)
			actual = actual->siguiente;
		
		actual->siguiente = nuevo;
	}
	
	return true;
}

NodoArista* Grafo::buscarVuelo(string codigo) const
{
	NodoVertice* vertice = primero;
	
	while (vertice != nullptr)
	{
		NodoArista* arista = vertice->conexiones;
		
		while (arista != nullptr)
		{
			if (arista->vuelo.getCodigo() == codigo)
				return arista;
			
			arista = arista->siguiente;
		}
		
		vertice = vertice->siguiente;
	}
	
	return nullptr;
}

bool Grafo::eliminarVuelo(string codigo)
{
	NodoVertice* vertice = primero;
	
	while (vertice != nullptr)
	{
		NodoArista* anterior = nullptr;
		NodoArista* arista = vertice->conexiones;
		
		while (arista != nullptr)
		{
			if (arista->vuelo.getCodigo() == codigo)
			{
				if (anterior == nullptr)
					vertice->conexiones = arista->siguiente;
				else
					anterior->siguiente = arista->siguiente;
				
				delete arista;
				return true;
			}
			
			anterior = arista;
			arista = arista->siguiente;
		}
		
		vertice = vertice->siguiente;
	}
	
	return false;
}

bool Grafo::modificarVuelo(string codigo, string codigoOrigen, string codigoDestino, float costo, float duracion)
{
	codigoOrigen = limpiarTextoGrafo(codigoOrigen);
	codigoDestino = limpiarTextoGrafo(codigoDestino);
	
	NodoArista* arista = buscarVuelo(codigo);
	
	if (arista == nullptr)
		return false;
	
	if (buscarCiudad(codigoOrigen) == nullptr ||
		buscarCiudad(codigoDestino) == nullptr)
		return false;
	
	if (codigoOrigen.empty() || codigoDestino.empty() ||
		codigoOrigen == codigoDestino || costo <= 0 || duracion <= 0)
		return false;
	
	if (arista->vuelo.getCodigoOrigen() == codigoOrigen)
	{
		arista->vuelo.setCodigoDestino(codigoDestino);
		arista->vuelo.setCosto(costo);
		arista->vuelo.setDuracion(duracion);
		return true;
	}
	
	Vuelo actualizado(codigo, codigoOrigen, codigoDestino, costo, duracion);
	
	if (!eliminarVuelo(codigo))
		return false;
	
	return agregarVuelo(actualizado);
}

bool Grafo::existeCiudad(string codigo) const
{
	return buscarCiudad(limpiarTextoGrafo(codigo)) != nullptr;
}

NodoVertice* Grafo::buscarCiudad(string codigo) const
{
	codigo = limpiarTextoGrafo(codigo);
	
	NodoVertice* actual = primero;
	
	while (actual != nullptr)
	{
		if (actual->ciudad.getCodigo() == codigo)
			return actual;
		
		actual = actual->siguiente;
	}
	
	return nullptr;
}

NodoVertice* Grafo::getPrimero() const
{
	return primero;
}

int Grafo::getCantidadVertices() const
{
	return cantidadVertices;
}

bool Grafo::modificarCiudad(string codigo, string nombre, float x, float y)
{
	codigo = limpiarTextoGrafo(codigo);
	nombre = limpiarTextoGrafo(nombre);
	
	if (codigo.empty() || nombre.empty())
		return false;
	
	if (!isfinite(x) || !isfinite(y) || x < 0 || y < 0)
		return false;
	
	NodoVertice* ciudad = buscarCiudad(codigo);
	
	if (ciudad == nullptr)
		return false;
	
	int pasos = 0;
	NodoABB* conMismoNombre = arbolCiudades.buscar(nombre, nullptr, pasos);
	
	if (conMismoNombre != nullptr && conMismoNombre->codigo != codigo)
		return false;
	
	ciudad->ciudad.setNombre(nombre);
	ciudad->ciudad.setPosicionX(x);
	ciudad->ciudad.setPosicionY(y);
	reconstruirArbol();
	
	return true;
}

bool Grafo::eliminarCiudad(string codigo)
{
	codigo = limpiarTextoGrafo(codigo);
	
	NodoVertice* anterior = nullptr;
	NodoVertice* actual = primero;
	
	while (actual != nullptr &&
		   actual->ciudad.getCodigo() != codigo)
	{
		anterior = actual;
		actual = actual->siguiente;
	}
	
	if (actual == nullptr)
		return false;
	
	NodoVertice* origen = primero;
	
	while (origen != nullptr)
	{
		NodoArista* arista = origen->conexiones;
		NodoArista* anteriorArista = nullptr;
		
		while (arista != nullptr)
		{
			if (arista->vuelo.getCodigoOrigen() == codigo ||
				arista->vuelo.getCodigoDestino() == codigo)
			{
				NodoArista* eliminar = arista;
				arista = arista->siguiente;
				
				if (anteriorArista == nullptr)
					origen->conexiones = arista;
				else
					anteriorArista->siguiente = arista;
				
				delete eliminar;
			}
			else
			{
				anteriorArista = arista;
				arista = arista->siguiente;
			}
		}
		
		origen = origen->siguiente;
	}
	
	if (anterior == nullptr)
		primero = actual->siguiente;
	else
		anterior->siguiente = actual->siguiente;
	
	NodoArista* arista = actual->conexiones;
	
	while (arista != nullptr)
	{
		NodoArista* eliminar = arista;
		arista = arista->siguiente;
		delete eliminar;
	}
	
	delete actual;
	cantidadVertices--;
	reconstruirArbol();
	
	return true;
}

void Grafo::limpiar()
{
	NodoVertice* actual = primero;
	
	while (actual != nullptr)
	{
		NodoVertice* eliminarVertice = actual;
		NodoArista* arista = actual->conexiones;
		
		while (arista != nullptr)
		{
			NodoArista* eliminarArista = arista;
			arista = arista->siguiente;
			
			delete eliminarArista;
		}
		
		actual = actual->siguiente;
		
		delete eliminarVertice;
	}
	
	primero = nullptr;
	cantidadVertices = 0;
}
