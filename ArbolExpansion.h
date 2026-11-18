#ifndef ARBOLEXPANSION_H
#define ARBOLEXPANSION_H
#include "Grafo.h"
#include <string>

using namespace std;

struct AristaExpansion
{
	int origen;			
	int destino;
	string codigoVuelo;	
	float costo;
	float duracion;
	bool aceptada;		
};

struct CiudadExpansion
{
	string codigo;
	string nombre;
	float x;
	float y;
};

struct ResultadoExpansion
{
	CiudadExpansion* ciudades;
	int cantidadCiudades;
	AristaExpansion* pasos;		
	int cantidadPasos;
	int cantidadAceptadas;
	float costoTotal;
	float duracionTotal;
	int componentes;			
	int indiceInicio;		
	
	ResultadoExpansion();
	~ResultadoExpansion();
	void liberar();
	
private:
	ResultadoExpansion(const ResultadoExpansion&);
	ResultadoExpansion& operator=(const ResultadoExpansion&);
};

bool prepararGrafoNoDirigido(Grafo& grafo, bool porCosto, ResultadoExpansion& r, NodoArista**& mejor);
void finalizarResultado(ResultadoExpansion& r);
int buscarIndiceCiudad(const ResultadoExpansion& r, const string& codigo);

struct NodoExpansion
{
	int ciudad;
	string codigo;
	string nombre;
	float x;				
	float y;
	float costoPadre;		
	float duracionPadre;
	NodoExpansion* primerHijo;
	NodoExpansion* hermano;
	float px;				
	float py;
};

class ArbolExpansion
{
private:
	NodoExpansion** nodos;	
	int cantidadNodos;
	NodoExpansion** raices;
	int cantidadRaices;
	
	void preorden(NodoExpansion* nodo, NodoExpansion** destino, int& n) const;
	void postorden(NodoExpansion* nodo, NodoExpansion** destino, int& n) const;
	int altura(NodoExpansion* nodo) const;
	ArbolExpansion(const ArbolExpansion&);
	ArbolExpansion& operator=(const ArbolExpansion&);
	
public:
	ArbolExpansion();
	~ArbolExpansion();
	void construir(const ResultadoExpansion& r, int indiceRaiz);
	void limpiar();
	int getCantidadNodos() const;
	int getCantidadRaices() const;
	NodoExpansion* getRaiz(int i) const;
	NodoExpansion* getNodo(int indiceCiudad) const;
	int getAltura() const;
	int contarHojas() const;
	int recorrerPreorden(NodoExpansion** destino) const;
	int recorrerPostorden(NodoExpansion** destino) const;
	int recorrerAnchura(NodoExpansion** destino) const;
};

#endif
