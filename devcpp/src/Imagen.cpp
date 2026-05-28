/*
==============================================================
  ARCHIVO  : Imagen.cpp
  DESCRIPCION: Implementacion de Imagen y
               Lista Circular Doblemente Enlazada de Imagenes.
==============================================================
*/

#include "Imagen.h"
#include <sstream>

/* ----------------------------------------------------------
   Destructor de Imagen:
   Libera la lista de NodoListaCapa.
   NO libera las Capas porque son punteros al ABB global.
   ---------------------------------------------------------- */
Imagen::~Imagen() {
    if (cabezaCapas == NULL) return;

    /* Romper la circularidad antes de recorrer */
    NodoListaCapa* actual = cabezaCapas;
    do {
        NodoListaCapa* tmp = actual;
        actual = actual->siguiente;
        delete tmp;   /* Solo borra el nodo, NO la capa */
    } while (actual != cabezaCapas);
}

/* ----------------------------------------------------------
   agregarCapa: inserta una capa al FINAL de la lista
   de capas de la imagen. Usa lista circular doble.
   El orden de insercion = orden de superposicion.
   ---------------------------------------------------------- */
void Imagen::agregarCapa(Capa* capa, int idCapa) {
    NodoListaCapa* nuevo = new NodoListaCapa(idCapa, capa);

    if (cabezaCapas == NULL) {
        /* Primera capa: el nodo apunta a si mismo */
        cabezaCapas      = nuevo;
        nuevo->siguiente = nuevo;
        nuevo->anterior  = nuevo;
    } else {
        /* Insertar al final: antes de la cabeza */
        NodoListaCapa* ultimo = cabezaCapas->anterior;
        ultimo->siguiente     = nuevo;
        nuevo->anterior       = ultimo;
        nuevo->siguiente      = cabezaCapas;
        cabezaCapas->anterior = nuevo;
    }
    numCapas++;
}

/* ----------------------------------------------------------
   generarDOTListaCapas: genera DOT de la lista de capas
   de esta imagen para visualizar con Graphviz.
   ---------------------------------------------------------- */
std::string Imagen::generarDOTListaCapas() const {
    std::ostringstream dot;
    dot << "digraph ListaCapasImg" << id << " {\n";
    dot << "  rankdir=LR;\n";
    dot << "  label=\"Lista de Capas - Imagen " << id << "\";\n";
    dot << "  node [shape=record, style=filled, fillcolor=lightyellow];\n";

    if (cabezaCapas == NULL) {
        dot << "  vacio [label=\"Sin capas\"];\n";
        dot << "}\n";
        return dot.str();
    }

    /* Recopilar los nodos para generar el DOT */
    std::vector<int> ids;
    NodoListaCapa* actual = cabezaCapas;
    do {
        ids.push_back(actual->idCapa);
        actual = actual->siguiente;
    } while (actual != cabezaCapas);

    /* Crear nodos */
    for (int i = 0; i < (int)ids.size(); i++) {
        dot << "  capN" << i
            << " [label=\"Capa " << ids[i] << "\"];\n";
    }

    /* Crear flechas dobles entre nodos */
    for (int i = 0; i < (int)ids.size() - 1; i++) {
        dot << "  capN" << i     << " -> capN" << (i+1) << ";\n";
        dot << "  capN" << (i+1) << " -> capN" << i
            << " [style=dashed];\n";
    }

    dot << "}\n";
    return dot.str();
}

/* ==========================================================
   LISTA CIRCULAR DOBLEMENTE ENLAZADA DE IMAGENES
   ========================================================== */

/* ----------------------------------------------------------
   Constructor: lista vacia
   ---------------------------------------------------------- */
ListaCircularImagenes::ListaCircularImagenes()
    : cabeza(NULL), cantidad(0) {}

/* ----------------------------------------------------------
   Destructor: libera todos los nodos e imagenes
   ---------------------------------------------------------- */
ListaCircularImagenes::~ListaCircularImagenes() {
    if (cabeza == NULL) return;

    NodoImagen* actual = cabeza;
    do {
        NodoImagen* tmp = actual;
        actual = actual->siguiente;
        delete tmp->imagen;
        delete tmp;
    } while (actual != cabeza);
}

/* ----------------------------------------------------------
   insertar: inserta una imagen ORDENADA por ID.
   Si el ID ya existe retorna false.
   Mantiene el enlace circular en todo momento.
   ---------------------------------------------------------- */
bool ListaCircularImagenes::insertar(Imagen* imagen) {
    NodoImagen* nuevo = new NodoImagen(imagen);

    if (cabeza == NULL) {
        /* Lista vacia: unico nodo apunta a si mismo */
        cabeza           = nuevo;
        nuevo->siguiente = nuevo;
        nuevo->anterior  = nuevo;
        cantidad++;
        return true;
    }

    /* Buscar posicion de insercion (ordenado por ID) */
    NodoImagen* actual = cabeza;
    do {
        if (actual->imagen->id == imagen->id) {
            delete nuevo;
            return false;   /* ID duplicado */
        }
        if (actual->imagen->id > imagen->id) break;
        actual = actual->siguiente;
    } while (actual != cabeza);

    /* Insertar antes de 'actual' */
    NodoImagen* ant  = actual->anterior;
    ant->siguiente   = nuevo;
    nuevo->anterior  = ant;
    nuevo->siguiente = actual;
    actual->anterior = nuevo;

    /* Si el nuevo ID es menor que la cabeza, actualizar cabeza */
    if (imagen->id < cabeza->imagen->id) {
        cabeza = nuevo;
    }

    cantidad++;
    return true;
}

/* ----------------------------------------------------------
   buscar: recorre la lista circular buscando el ID.
   Maximo recorre una vuelta completa => O(n)
   ---------------------------------------------------------- */
Imagen* ListaCircularImagenes::buscar(int id) const {
    if (cabeza == NULL) return NULL;

    NodoImagen* actual = cabeza;
    do {
        if (actual->imagen->id == id) return actual->imagen;
        actual = actual->siguiente;
    } while (actual != cabeza);

    return NULL;
}

/* ----------------------------------------------------------
   eliminar: elimina la imagen con el ID indicado.
   Actualiza los enlaces circulares correctamente.
   ---------------------------------------------------------- */
bool ListaCircularImagenes::eliminar(int id) {
    if (cabeza == NULL) return false;

    NodoImagen* actual = cabeza;
    do {
        if (actual->imagen->id == id) {
            if (cantidad == 1) {
                /* Unico nodo */
                delete actual->imagen;
                delete actual;
                cabeza    = NULL;
                cantidad  = 0;
                return true;
            }
            /* Reconectar vecinos */
            actual->anterior->siguiente = actual->siguiente;
            actual->siguiente->anterior = actual->anterior;

            if (actual == cabeza) {
                cabeza = actual->siguiente;
            }
            delete actual->imagen;
            delete actual;
            cantidad--;
            return true;
        }
        actual = actual->siguiente;
    } while (actual != cabeza);

    return false;
}

/* ----------------------------------------------------------
   generarDOT: genera DOT de la lista circular completa.
   Si mostrarCapas=true, muestra las capas de cada imagen
   y los punteros hacia el ABB de capas.
   ---------------------------------------------------------- */
std::string ListaCircularImagenes::generarDOT(bool mostrarCapas) const {
    std::ostringstream dot;
    dot << "digraph ListaCircularImagenes {\n";
    dot << "  rankdir=LR;\n";
    dot << "  label=\"Lista Circular Doblemente Enlazada de Imagenes\";\n";
    dot << "  node [shape=record, style=filled, fillcolor=lightcyan];\n";

    if (cabeza == NULL) {
        dot << "  vacio [label=\"Lista vacia\"];\n";
        dot << "}\n";
        return dot.str();
    }

    /* Recopilar IDs para generar nodos */
    std::vector<int> ids;
    NodoImagen* actual = cabeza;
    do {
        dot << "  img" << actual->imagen->id
            << " [label=\"{<prev>|Img "
            << actual->imagen->id
            << " | Capas: " << actual->imagen->numCapas
            << "|<next>}\"];\n";
        ids.push_back(actual->imagen->id);
        actual = actual->siguiente;
    } while (actual != cabeza);

    /* Flechas dobles circulares entre nodos */
    for (int i = 0; i < (int)ids.size(); i++) {
        int j = (i + 1) % (int)ids.size();
        dot << "  img" << ids[i] << ":next -> img"
            << ids[j] << ":prev [label=\"sig\"];\n";
        dot << "  img" << ids[j] << ":prev -> img"
            << ids[i] << ":next [label=\"ant\", style=dashed];\n";
    }

    /* Mostrar capas de cada imagen si se solicita */
    if (mostrarCapas) {
        actual = cabeza;
        do {
            NodoListaCapa* cap = actual->imagen->cabezaCapas;
            if (cap != NULL) {
                int capIdx = 0;
                NodoListaCapa* c = cap;
                std::string prevNode = "img"
                    + std::to_string(actual->imagen->id);

                do {
                    std::string capId = "img"
                        + std::to_string(actual->imagen->id)
                        + "_c" + std::to_string(capIdx);

                    dot << "  " << capId
                        << " [label=\"Capa " << c->idCapa
                        << "\", shape=box, fillcolor=lightyellow];\n";
                    dot << "  " << prevNode
                        << " -> " << capId << " [color=blue];\n";
                    dot << "  " << capId
                        << " -> nodo" << c->idCapa
                        << " [style=dotted, color=purple, label=\"ptr\"];\n";

                    prevNode = capId;
                    c        = c->siguiente;
                    capIdx++;
                } while (c != cap);
            }
            actual = actual->siguiente;
        } while (actual != cabeza);
    }

    dot << "}\n";
    return dot.str();
}
