/*
==============================================================
  ARCHIVO  : Capa.cpp
  DESCRIPCION: Implementacion del ABB de Capas.
               Arbol Binario de Busqueda ordenado por ID.
==============================================================
*/

#include "Capa.h"
#include <sstream>

/* ----------------------------------------------------------
   Constructor: arbol vacio
   ---------------------------------------------------------- */
ABBCapas::ABBCapas() : raiz(NULL) {}

/* ----------------------------------------------------------
   Destructor: libera todos los nodos del arbol recursivamente
   ---------------------------------------------------------- */
ABBCapas::~ABBCapas() {
    destruir(raiz);
}

void ABBCapas::destruir(NodoCapa* nodo) {
    if (nodo == NULL) return;
    destruir(nodo->izquierdo);
    destruir(nodo->derecho);
    delete nodo->capa;   /* Esto libera tambien la MatrizDispersa */
    delete nodo;
}

/* ----------------------------------------------------------
   insertar (privado): inserta recursivamente en el ABB.
   Si el ID ya existe, NO inserta (IDs son unicos).
   Menor ID va a la izquierda, mayor a la derecha.
   ---------------------------------------------------------- */
NodoCapa* ABBCapas::insertar(NodoCapa* nodo, Capa* capa) {
    if (nodo == NULL) return new NodoCapa(capa);

    if (capa->id < nodo->capa->id)
        nodo->izquierdo = insertar(nodo->izquierdo, capa);
    else if (capa->id > nodo->capa->id)
        nodo->derecho   = insertar(nodo->derecho,   capa);
    /* Si igual: ID duplicado, no se inserta */

    return nodo;
}

void ABBCapas::insertar(Capa* capa) {
    raiz = insertar(raiz, capa);
}

/* ----------------------------------------------------------
   buscar (privado): busca recursivamente por ID.
   O(log n) promedio, O(n) peor caso (arbol degenerado)
   ---------------------------------------------------------- */
NodoCapa* ABBCapas::buscar(NodoCapa* nodo, int id) const {
    if (nodo == NULL)       return NULL;
    if (id == nodo->capa->id) return nodo;
    if (id  < nodo->capa->id) return buscar(nodo->izquierdo, id);
    return buscar(nodo->derecho, id);
}

Capa* ABBCapas::buscar(int id) const {
    NodoCapa* n = buscar(raiz, id);
    return (n != NULL) ? n->capa : NULL;
}

/* ----------------------------------------------------------
   RECORRIDO INORDEN: izquierdo -> raiz -> derecho
   Resultado: IDs en orden ASCENDENTE
   ---------------------------------------------------------- */
void ABBCapas::inorden(NodoCapa* nodo, std::vector<int>& res) const {
    if (nodo == NULL) return;
    inorden(nodo->izquierdo, res);
    res.push_back(nodo->capa->id);
    inorden(nodo->derecho,   res);
}
std::vector<int> ABBCapas::recorridoInorden() const {
    std::vector<int> res;
    inorden(raiz, res);
    return res;
}

/* ----------------------------------------------------------
   RECORRIDO PREORDEN: raiz -> izquierdo -> derecho
   ---------------------------------------------------------- */
void ABBCapas::preorden(NodoCapa* nodo, std::vector<int>& res) const {
    if (nodo == NULL) return;
    res.push_back(nodo->capa->id);
    preorden(nodo->izquierdo, res);
    preorden(nodo->derecho,   res);
}
std::vector<int> ABBCapas::recorridoPreorden() const {
    std::vector<int> res;
    preorden(raiz, res);
    return res;
}

/* ----------------------------------------------------------
   RECORRIDO POSTORDEN: izquierdo -> derecho -> raiz
   ---------------------------------------------------------- */
void ABBCapas::postorden(NodoCapa* nodo, std::vector<int>& res) const {
    if (nodo == NULL) return;
    postorden(nodo->izquierdo, res);
    postorden(nodo->derecho,   res);
    res.push_back(nodo->capa->id);
}
std::vector<int> ABBCapas::recorridoPostorden() const {
    std::vector<int> res;
    postorden(raiz, res);
    return res;
}

/* ----------------------------------------------------------
   generarDOTRec: recorre el arbol generando el codigo DOT.
   Los nodos nulos se marcan como puntos para ver estructura.
   ---------------------------------------------------------- */
void ABBCapas::generarDOTRec(NodoCapa* nodo,
                              std::ostringstream& dot) const {
    if (nodo == NULL) return;

    dot << "  nodo" << nodo->capa->id
        << " [label=\"Capa " << nodo->capa->id
        << "\", shape=ellipse, style=filled, fillcolor=lightgreen];\n";

    if (nodo->izquierdo != NULL) {
        dot << "  nodo" << nodo->capa->id
            << " -> nodo" << nodo->izquierdo->capa->id
            << " [label=\"L\"];\n";
        generarDOTRec(nodo->izquierdo, dot);
    } else {
        dot << "  nullL" << nodo->capa->id
            << " [label=\"\", shape=point];\n";
        dot << "  nodo" << nodo->capa->id
            << " -> nullL" << nodo->capa->id << ";\n";
    }

    if (nodo->derecho != NULL) {
        dot << "  nodo" << nodo->capa->id
            << " -> nodo" << nodo->derecho->capa->id
            << " [label=\"R\"];\n";
        generarDOTRec(nodo->derecho, dot);
    } else {
        dot << "  nullR" << nodo->capa->id
            << " [label=\"\", shape=point];\n";
        dot << "  nodo" << nodo->capa->id
            << " -> nullR" << nodo->capa->id << ";\n";
    }
}

std::string ABBCapas::generarDOT() const {
    std::ostringstream dot;
    dot << "digraph ABBCapas {\n";
    dot << "  label=\"Arbol Binario de Busqueda - Capas\";\n";
    dot << "  node [fontsize=11];\n";

    if (raiz != NULL)
        generarDOTRec(raiz, dot);
    else
        dot << "  vacio [label=\"Arbol vacio\"];\n";

    dot << "}\n";
    return dot.str();
}
