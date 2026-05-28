/*
==============================================================
  ARCHIVO  : Usuario.cpp
  DESCRIPCION: Implementacion del ABB de Usuarios y la
               Lista Simplemente Enlazada de Imagenes
               por usuario.
==============================================================
*/

#include "Usuario.h"
#include <sstream>
#include <algorithm>

/* ----------------------------------------------------------
   Destructor de Usuario:
   Libera la lista simple de imagenes.
   NO libera las imagenes (son punteros a la lista circular).
   ---------------------------------------------------------- */
Usuario::~Usuario() {
    NodoImagenUsuario* actual = listaImagenes;
    while (actual != NULL) {
        NodoImagenUsuario* tmp = actual;
        actual = actual->siguiente;
        delete tmp;   /* Solo borra el nodo, no la imagen */
    }
}

/* ----------------------------------------------------------
   agregarImagen: inserta al FINAL de la lista simple.
   Lista simple => solo hay puntero al siguiente.
   ---------------------------------------------------------- */
void Usuario::agregarImagen(int idImagen, Imagen* img) {
    NodoImagenUsuario* nuevo = new NodoImagenUsuario(idImagen, img);

    if (listaImagenes == NULL) {
        listaImagenes = nuevo;
    } else {
        /* Recorrer hasta el ultimo nodo */
        NodoImagenUsuario* actual = listaImagenes;
        while (actual->siguiente != NULL) {
            actual = actual->siguiente;
        }
        actual->siguiente = nuevo;
    }
    numImagenes++;
}

/* ----------------------------------------------------------
   eliminarImagen: busca el ID en la lista simple y lo borra.
   Reconecta el nodo anterior con el siguiente.
   ---------------------------------------------------------- */
bool Usuario::eliminarImagen(int idImagen) {
    if (listaImagenes == NULL) return false;

    NodoImagenUsuario* actual   = listaImagenes;
    NodoImagenUsuario* anterior = NULL;

    while (actual != NULL) {
        if (actual->idImagen == idImagen) {
            if (anterior != NULL)
                anterior->siguiente = actual->siguiente;
            else
                listaImagenes = actual->siguiente;  /* Era la cabeza */

            delete actual;
            numImagenes--;
            return true;
        }
        anterior = actual;
        actual   = actual->siguiente;
    }
    return false;
}

/* ----------------------------------------------------------
   obtenerIdsImagenes: recorre la lista y retorna los IDs
   ---------------------------------------------------------- */
std::vector<int> Usuario::obtenerIdsImagenes() const {
    std::vector<int> ids;
    NodoImagenUsuario* actual = listaImagenes;
    while (actual != NULL) {
        ids.push_back(actual->idImagen);
        actual = actual->siguiente;
    }
    return ids;
}

/* ----------------------------------------------------------
   tieneImagen: busca si el usuario tiene la imagen
   ---------------------------------------------------------- */
bool Usuario::tieneImagen(int id) const {
    NodoImagenUsuario* actual = listaImagenes;
    while (actual != NULL) {
        if (actual->idImagen == id) return true;
        actual = actual->siguiente;
    }
    return false;
}

/* ==========================================================
   ABBUsuarios - Implementacion
   ========================================================== */

ABBUsuarios::ABBUsuarios() : raiz(NULL) {}

ABBUsuarios::~ABBUsuarios() {
    destruir(raiz);
}

void ABBUsuarios::destruir(NodoUsuario* nodo) {
    if (nodo == NULL) return;
    destruir(nodo->izquierdo);
    destruir(nodo->derecho);
    delete nodo->usuario;
    delete nodo;
}

/* ----------------------------------------------------------
   insertar: insercion recursiva en el ABB por nombre.
   Comparacion lexicografica: < va a izquierda, > a derecha.
   ---------------------------------------------------------- */
NodoUsuario* ABBUsuarios::insertar(NodoUsuario* nodo, Usuario* usuario) {
    if (nodo == NULL) return new NodoUsuario(usuario);

    if (usuario->nombre < nodo->usuario->nombre)
        nodo->izquierdo = insertar(nodo->izquierdo, usuario);
    else if (usuario->nombre > nodo->usuario->nombre)
        nodo->derecho   = insertar(nodo->derecho,   usuario);
    /* Si igual: nombre duplicado, no inserta */

    return nodo;
}

void ABBUsuarios::insertar(Usuario* usuario) {
    raiz = insertar(raiz, usuario);
}

/* ----------------------------------------------------------
   buscar: busqueda recursiva por nombre en el ABB
   ---------------------------------------------------------- */
NodoUsuario* ABBUsuarios::buscar(NodoUsuario* nodo,
                                  const std::string& nombre) const {
    if (nodo == NULL) return NULL;
    if (nombre == nodo->usuario->nombre) return nodo;
    if (nombre  < nodo->usuario->nombre)
        return buscar(nodo->izquierdo, nombre);
    return buscar(nodo->derecho, nombre);
}

Usuario* ABBUsuarios::buscar(const std::string& nombre) const {
    NodoUsuario* n = buscar(raiz, nombre);
    return (n != NULL) ? n->usuario : NULL;
}

/* ----------------------------------------------------------
   minimoNodo: retorna el nodo con el menor nombre
   (el mas a la izquierda). Se usa en la eliminacion.
   ---------------------------------------------------------- */
NodoUsuario* ABBUsuarios::minimoNodo(NodoUsuario* nodo) const {
    while (nodo->izquierdo != NULL)
        nodo = nodo->izquierdo;
    return nodo;
}

/* ----------------------------------------------------------
   eliminar: elimina recursivamente del ABB.
   Tres casos:
   1. Sin hijos: borrar directamente
   2. Un hijo: reemplazar con el hijo
   3. Dos hijos: reemplazar con sucesor inorden
   ---------------------------------------------------------- */
NodoUsuario* ABBUsuarios::eliminar(NodoUsuario* nodo,
                                    const std::string& nombre) {
    if (nodo == NULL) return NULL;

    if (nombre < nodo->usuario->nombre)
        nodo->izquierdo = eliminar(nodo->izquierdo, nombre);
    else if (nombre > nodo->usuario->nombre)
        nodo->derecho   = eliminar(nodo->derecho,   nombre);
    else {
        /* Nodo encontrado */
        if (nodo->izquierdo == NULL) {
            NodoUsuario* tmp = nodo->derecho;
            delete nodo->usuario;
            delete nodo;
            return tmp;
        } else if (nodo->derecho == NULL) {
            NodoUsuario* tmp = nodo->izquierdo;
            delete nodo->usuario;
            delete nodo;
            return tmp;
        }
        /* Dos hijos: usar sucesor inorden para mantener ABB */
        NodoUsuario* sucesor = minimoNodo(nodo->derecho);
        std::swap(nodo->usuario, sucesor->usuario);
        nodo->derecho = eliminar(nodo->derecho,
                                  sucesor->usuario->nombre);
    }
    return nodo;
}

bool ABBUsuarios::eliminar(const std::string& nombre) {
    if (buscar(nombre) == NULL) return false;
    raiz = eliminar(raiz, nombre);
    return true;
}

/* ----------------------------------------------------------
   listarRec: recorrido inorden => orden alfabetico
   ---------------------------------------------------------- */
void ABBUsuarios::listarRec(NodoUsuario* nodo,
                              std::vector<std::string>& lista) const {
    if (nodo == NULL) return;
    listarRec(nodo->izquierdo, lista);
    lista.push_back(nodo->usuario->nombre);
    listarRec(nodo->derecho,   lista);
}

std::vector<std::string> ABBUsuarios::listarUsuarios() const {
    std::vector<std::string> lista;
    listarRec(raiz, lista);
    return lista;
}

/* ----------------------------------------------------------
   generarDOTRec: genera el DOT del ABB de usuarios
   ---------------------------------------------------------- */
void ABBUsuarios::generarDOTRec(NodoUsuario* nodo,
                                  std::ostringstream& dot) const {
    if (nodo == NULL) return;

    dot << "  usr_" << nodo->usuario->nombre
        << " [label=\"" << nodo->usuario->nombre
        << "\\nImagenes: " << nodo->usuario->numImagenes
        << "\", shape=ellipse, style=filled, fillcolor=lightsalmon];\n";

    if (nodo->izquierdo != NULL) {
        dot << "  usr_" << nodo->usuario->nombre
            << " -> usr_" << nodo->izquierdo->usuario->nombre
            << " [label=\"L\"];\n";
        generarDOTRec(nodo->izquierdo, dot);
    } else {
        dot << "  nullUL_" << nodo->usuario->nombre
            << " [label=\"\", shape=point];\n";
        dot << "  usr_" << nodo->usuario->nombre
            << " -> nullUL_" << nodo->usuario->nombre << ";\n";
    }

    if (nodo->derecho != NULL) {
        dot << "  usr_" << nodo->usuario->nombre
            << " -> usr_" << nodo->derecho->usuario->nombre
            << " [label=\"R\"];\n";
        generarDOTRec(nodo->derecho, dot);
    } else {
        dot << "  nullUR_" << nodo->usuario->nombre
            << " [label=\"\", shape=point];\n";
        dot << "  usr_" << nodo->usuario->nombre
            << " -> nullUR_" << nodo->usuario->nombre << ";\n";
    }
}

std::string ABBUsuarios::generarDOT() const {
    std::ostringstream dot;
    dot << "digraph ABBUsuarios {\n";
    dot << "  label=\"Arbol Binario de Busqueda - Usuarios\";\n";
    dot << "  node [fontsize=11];\n";

    if (raiz != NULL)
        generarDOTRec(raiz, dot);
    else
        dot << "  vacio [label=\"Sin usuarios\"];\n";

    dot << "}\n";
    return dot.str();
}
