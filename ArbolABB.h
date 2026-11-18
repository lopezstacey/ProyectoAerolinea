#ifndef ARBOLABB_H
#define ARBOLABB_H
#include <string>

using namespace std;

struct NodoABB
{
	string nombre;
	string codigo;
	NodoABB* izquierdo;
	NodoABB* derecho;
	float x;	
	float y;
	
	NodoABB(string nombre, string codigo)
	{
		this->nombre = nombre;
		this->codigo = codigo;
		izquierdo = nullptr;
		derecho = nullptr;
		x = 0;
		y = 0;
	}
};

class ArbolABB
{
private:
	NodoABB* raiz;
	int cantidad;
	
	static string minusculas(const string& texto);
	static int comparar(const string& a, const string& b);
	NodoABB* insertar(NodoABB* actual, NodoABB* nuevo, bool& insertado);
	int altura(NodoABB* nodo) const;
	void liberar(NodoABB* nodo);
	void inorden(NodoABB* nodo, NodoABB** destino, int& n) const;
	void preorden(NodoABB* nodo, NodoABB** destino, int& n) const;
	void postorden(NodoABB* nodo, NodoABB** destino, int& n) const;
	
public:
	ArbolABB();
	~ArbolABB();
	bool insertar(string nombre, string codigo);
	NodoABB* buscar(const string& nombre, NodoABB** camino, int& pasos) const;
	bool contiene(const string& nombre) const;
	int getCantidad() const;
	int getAltura() const;
	NodoABB* getRaiz() const;
	int recorrerInorden(NodoABB** destino) const;
	int recorrerPreorden(NodoABB** destino) const;
	int recorrerPostorden(NodoABB** destino) const;
	void limpiar();
};

#endif
