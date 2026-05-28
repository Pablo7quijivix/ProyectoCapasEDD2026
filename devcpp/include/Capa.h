#ifndef CAPA_H
#define CAPA_H

/*
==============================================================
  ARCHIVO  : Capa.h
  ESTRUCTURA: Arbol Binario de Busqueda (ABB) de Capas
  QUE HACE : Almacena todas las capas del sistema ordenadas
             por su ID. Permite busqueda en O(log n) promedio.
             Soporta recorridos inorden/preorden/postorden
             para la generacion de imagenes por recorrido.
  COMPATIBLE: Dev-C++ 5.11 / MinGW / Windows
==============================================================
*/

#include "MatrizDispersa.h"
#include <string>
#include <vector>

/* ----------------------------------------------------------
   CAPA: contiene el ID unico y la matriz dispersa de pixeles
   ---------------------------------------------------------- */
struct Capa {
    int id;
    MatrizDispersa* matriz;   /* Pixeles de la capa */

    Capa(int i) : id(i), matriz(new MatrizDispersa()) {}
    ~Capa() { delete matriz; }
};

/* ----------------------------------------------------------
   NODO DEL ABB DE CAPAS
   Cada nodo guarda una Capa y dos punteros (izq / der)
   ---------------------------------------------------------- */
struct NodoCapa {
    Capa*     capa;
    NodoCapa* izquierdo;
    NodoCapa* derecho;

    NodoCapa(Capa* c)
        : capa(c), izquierdo(NULL), derecho(NULL) {}
};

/* ----------------------------------------------------------
   CLASE ABBCapas
   Arbol Binario de Busqueda de capas, ordenado por ID.
   - Insercion, busqueda: O(log n) promedio
   - Recorridos: inorden, preorden, postorden
   ---------------------------------------------------------- */
class ABBCapas {
private:
    NodoCapa* raiz;

    /* Insercion recursiva */
    NodoCapa* insertar(NodoCapa* nodo, Capa* capa);

    /* Busqueda recursiva */
    NodoCapa* buscar(NodoCapa* nodo, int id) const;

    /* Generacion DOT recursiva */
    void generarDOTRec(NodoCapa* nodo, std::ostringstream& dot) const;

    /* Libera toda la memoria del arbol */
    void destruir(NodoCapa* nodo);

    /* Recorridos internos que llenan el vector resultado */
    void inorden   (NodoCapa* nodo, std::vector<int>& res) const;
    void preorden  (NodoCapa* nodo, std::vector<int>& res) const;
    void postorden (NodoCapa* nodo, std::vector<int>& res) const;

public:
    ABBCapas();
    ~ABBCapas();

    /* Inserta una capa en el ABB (ignora si el ID ya existe) */
    void insertar(Capa* capa);

    /* Busca y retorna la capa por ID; NULL si no existe */
    Capa* buscar(int id) const;

    /* Genera el codigo DOT del arbol para Graphviz */
    std::string generarDOT() const;

    /* Retorna la raiz (para acceso externo) */
    NodoCapa* getRaiz() const { return raiz; }

    /* Recorridos que retornan vector de IDs */
    std::vector<int> recorridoInorden()    const;
    std::vector<int> recorridoPreorden()   const;
    std::vector<int> recorridoPostorden()  const;
};

#endif
