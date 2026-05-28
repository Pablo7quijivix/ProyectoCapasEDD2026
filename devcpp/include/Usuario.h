#ifndef USUARIO_H
#define USUARIO_H

/*
==============================================================
  ARCHIVO  : Usuario.h
  ESTRUCTURA: ABB de Usuarios + Lista Simple de Imagenes
  QUE HACE : Los usuarios se guardan en un Arbol Binario de
             Busqueda ordenado por nombre (alfabetico).
             Cada usuario tiene lista simplemente enlazada
             de imagenes que ha registrado.
  COMPATIBLE: Dev-C++ 5.11 / MinGW / Windows
==============================================================
*/

#include "Imagen.h"
#include <string>
#include <vector>

/* ----------------------------------------------------------
   NODO DE LISTA SIMPLE DE IMAGENES DEL USUARIO
   Guarda puntero a la imagen en la lista circular global.
   ---------------------------------------------------------- */
struct NodoImagenUsuario {
    int    idImagen;
    Imagen* imagenPuntero;         /* Puntero a la lista global */
    NodoImagenUsuario* siguiente;

    NodoImagenUsuario(int id, Imagen* img)
        : idImagen(id), imagenPuntero(img), siguiente(NULL) {}
};

/* ----------------------------------------------------------
   USUARIO: nombre + lista simple enlazada de imagenes
   Se almacena en un ABB ordenado por nombre.
   ---------------------------------------------------------- */
struct Usuario {
    std::string nombre;
    NodoImagenUsuario* listaImagenes;  /* Lista simple de imagenes */
    int numImagenes;

    Usuario(const std::string& n)
        : nombre(n), listaImagenes(NULL), numImagenes(0) {}
    ~Usuario();

    /* Agrega imagen al final de la lista simple */
    void agregarImagen(int idImagen, Imagen* img);

    /* Elimina imagen de la lista simple por ID */
    bool eliminarImagen(int idImagen);

    /* Retorna vector de IDs de imagenes del usuario */
    std::vector<int> obtenerIdsImagenes() const;

    /* Verifica si el usuario tiene la imagen indicada */
    bool tieneImagen(int id) const;
};

/* ----------------------------------------------------------
   NODO DEL ABB DE USUARIOS
   Ordenado lexicograficamente por nombre
   ---------------------------------------------------------- */
struct NodoUsuario {
    Usuario*     usuario;
    NodoUsuario* izquierdo;
    NodoUsuario* derecho;

    NodoUsuario(Usuario* u)
        : usuario(u), izquierdo(NULL), derecho(NULL) {}
};

/* ----------------------------------------------------------
   CLASE ABBUsuarios
   Arbol Binario de Busqueda de usuarios, ordenado por nombre.
   Soporta CRUD completo (agregar, buscar, eliminar, modificar).
   ---------------------------------------------------------- */
class ABBUsuarios {
private:
    NodoUsuario* raiz;

    NodoUsuario* insertar  (NodoUsuario* nodo, Usuario* usuario);
    NodoUsuario* buscar    (NodoUsuario* nodo, const std::string& nombre) const;
    NodoUsuario* eliminar  (NodoUsuario* nodo, const std::string& nombre);
    NodoUsuario* minimoNodo(NodoUsuario* nodo) const;
    void         generarDOTRec(NodoUsuario* nodo, std::ostringstream& dot) const;
    void         destruir  (NodoUsuario* nodo);
    void         listarRec (NodoUsuario* nodo, std::vector<std::string>& lista) const;

public:
    ABBUsuarios();
    ~ABBUsuarios();

    void     insertar(Usuario* usuario);
    Usuario* buscar  (const std::string& nombre) const;
    bool     eliminar(const std::string& nombre);

    /* Lista todos los nombres en orden alfabetico (inorden) */
    std::vector<std::string> listarUsuarios() const;

    /* Genera DOT del ABB de usuarios para Graphviz */
    std::string generarDOT() const;

    NodoUsuario* getRaiz() const { return raiz; }
};

#endif
