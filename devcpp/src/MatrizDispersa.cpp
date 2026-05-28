/*
==============================================================
  ARCHIVO  : MatrizDispersa.cpp
  DESCRIPCION: Implementacion de la Matriz Dispersa.
               Lista doble de filas con lista doble de columnas.
==============================================================
*/

#include "MatrizDispersa.h"

/* ----------------------------------------------------------
   Constructor: inicializa la matriz completamente vacia
   ---------------------------------------------------------- */
MatrizDispersa::MatrizDispersa()
    : cabezaFilas(NULL), maxFila(0), maxColumna(0) {}

/* ----------------------------------------------------------
   Destructor: recorre y libera TODA la memoria usada.
   Primero libera las columnas de cada fila, luego la fila.
   ---------------------------------------------------------- */
MatrizDispersa::~MatrizDispersa() {
    NodoFila* fila = cabezaFilas;
    while (fila != NULL) {
        /* Liberar todos los nodos columna de esta fila */
        NodoColumna* col = fila->columnas;
        while (col != NULL) {
            NodoColumna* tmp = col;
            col = col->siguiente;
            delete tmp;
        }
        NodoFila* tmpFila = fila;
        fila = fila->siguiente;
        delete tmpFila;
    }
}

/* ----------------------------------------------------------
   obtenerFila: busca el NodoFila con el numero indicado.
   Si no existe, lo crea e inserta en la lista ORDENADA
   de forma ascendente por numero de fila.
   ---------------------------------------------------------- */
NodoFila* MatrizDispersa::obtenerFila(int fila) {
    NodoFila* actual   = cabezaFilas;
    NodoFila* anterior = NULL;

    /* Avanzar hasta encontrar la posicion correcta */
    while (actual != NULL && actual->fila < fila) {
        anterior = actual;
        actual   = actual->siguiente;
    }

    /* Si ya existe esa fila, retornarla directamente */
    if (actual != NULL && actual->fila == fila) {
        return actual;
    }

    /* Crear nuevo NodoFila e insertarlo en la posicion correcta */
    NodoFila* nuevo  = new NodoFila(fila);
    nuevo->siguiente = actual;
    nuevo->anterior  = anterior;

    if (anterior != NULL) {
        anterior->siguiente = nuevo;
    } else {
        cabezaFilas = nuevo;   /* El nuevo nodo es la nueva cabeza */
    }

    if (actual != NULL) {
        actual->anterior = nuevo;
    }

    return nuevo;
}

/* ----------------------------------------------------------
   insertarPixel: inserta o actualiza un pixel en (fila, col).
   Mantiene la lista de columnas ordenada ascendentemente.
   ---------------------------------------------------------- */
void MatrizDispersa::insertarPixel(int fila, int columna,
                                   const std::string& color) {
    NodoFila* nFila = obtenerFila(fila);

    /* Actualizar dimensiones maximas de la capa */
    if (fila + 1     > maxFila)    maxFila    = fila + 1;
    if (columna + 1  > maxColumna) maxColumna = columna + 1;

    /* Buscar posicion correcta en la lista de columnas */
    NodoColumna* actual = nFila->columnas;
    NodoColumna* ant    = NULL;

    while (actual != NULL && actual->columna < columna) {
        ant    = actual;
        actual = actual->siguiente;
    }

    /* Si ya existe ese pixel, solo actualizar el color */
    if (actual != NULL && actual->columna == columna) {
        actual->color = color;
        return;
    }

    /* Crear nuevo NodoColumna e insertarlo */
    NodoColumna* nuevo  = new NodoColumna(columna, color);
    nuevo->siguiente    = actual;
    nuevo->anterior     = ant;

    if (ant != NULL) {
        ant->siguiente = nuevo;
    } else {
        nFila->columnas = nuevo;   /* Primer pixel de esta fila */
    }

    if (actual != NULL) {
        actual->anterior = nuevo;
    }
}

/* ----------------------------------------------------------
   obtenerPixel: busca y retorna el color de (fila, columna).
   Retorna "" si la posicion no tiene pixel definido.
   ---------------------------------------------------------- */
std::string MatrizDispersa::obtenerPixel(int fila, int columna) const {
    NodoFila* f = cabezaFilas;
    while (f != NULL && f->fila < fila) {
        f = f->siguiente;
    }
    if (f == NULL || f->fila != fila) return "";

    NodoColumna* c = f->columnas;
    while (c != NULL && c->columna < columna) {
        c = c->siguiente;
    }
    if (c == NULL || c->columna != columna) return "";

    return c->color;
}

/* ----------------------------------------------------------
   generarDOT: genera representacion Graphviz de la matriz.
   Muestra la estructura interna de filas y columnas.
   ---------------------------------------------------------- */
std::string MatrizDispersa::generarDOT(int idCapa) const {
    std::ostringstream dot;
    dot << "digraph MatrizCapa" << idCapa << " {\n";
    dot << "  rankdir=LR;\n";
    dot << "  label=\"Matriz Dispersa - Capa " << idCapa << "\";\n";
    dot << "  node [shape=record, fontsize=10];\n";

    NodoFila* fila = cabezaFilas;
    while (fila != NULL) {
        /* Nodo que representa la fila */
        dot << "  fila" << fila->fila
            << " [label=\"Fila " << fila->fila
            << "\", shape=box, style=filled, fillcolor=lightblue];\n";

        NodoColumna* col   = fila->columnas;
        std::string prevId = "fila" + std::to_string(fila->fila);

        while (col != NULL) {
            std::string nodeId = "f" + std::to_string(fila->fila)
                               + "c" + std::to_string(col->columna);

            dot << "  " << nodeId
                << " [label=\"[" << fila->fila << "," << col->columna
                << "]\\n" << col->color
                << "\", style=filled, fillcolor=\"" << col->color << "\"];\n";

            dot << "  " << prevId << " -> " << nodeId << ";\n";
            prevId = nodeId;
            col    = col->siguiente;
        }

        /* Flecha al siguiente nodo fila */
        if (fila->siguiente != NULL) {
            dot << "  fila" << fila->fila
                << " -> fila" << fila->siguiente->fila
                << " [color=red];\n";
        }
        fila = fila->siguiente;
    }
    dot << "}\n";
    return dot.str();
}
