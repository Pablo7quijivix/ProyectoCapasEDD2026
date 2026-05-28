#ifndef IMAGEN_H
#define IMAGEN_H

/*
==============================================================
  ARCHIVO  : Imagen.h
  ESTRUCTURA: Lista Circular Doblemente Enlazada de Imagenes
              + Lista Doblemente Enlazada de Capas por imagen
  QUE HACE : Las imagenes se guardan en lista circular doble
             ordenada por ID. Cada imagen tiene su propia
             lista de capas que SON PUNTEROS al ABB (no copias).
  COMPATIBLE: Dev-C++ 5.11 / MinGW / Windows
==============================================================
*/

#include "Capa.h"
#include <string>
#include <vector>

/* ----------------------------------------------------------
   NODO DE LISTA DE CAPAS DE UNA IMAGEN
   Apunta DIRECTAMENTE a la Capa en el ABB (no es copia).
   El orden de insercion define el orden de superposicion.
   ---------------------------------------------------------- */
struct NodoListaCapa {
    int   idCapa;           /* ID de la capa referenciada     */
    Capa* capaPuntero;      /* Puntero directo al ABB de capas*/
    NodoListaCapa* siguiente;
    NodoListaCapa* anterior;

    NodoListaCapa(int id, Capa* c)
        : idCapa(id), capaPuntero(c),
          siguiente(NULL), anterior(NULL) {}
};

/* ----------------------------------------------------------
   IMAGEN: resultado de superponer capas en orden.
   Tiene ID unico y lista doblemente enlazada de capas.
   ---------------------------------------------------------- */
struct Imagen {
    int id;
    NodoListaCapa* cabezaCapas; /* Lista doble de capas         */
    int numCapas;

    Imagen(int i) : id(i), cabezaCapas(NULL), numCapas(0) {}
    ~Imagen();

    /* Agrega capa al final de la lista (respeta orden) */
    void agregarCapa(Capa* capa, int idCapa);

    /* Genera DOT de la lista de capas de esta imagen */
    std::string generarDOTListaCapas() const;
};

/* ----------------------------------------------------------
   NODO DE LA LISTA CIRCULAR DOBLEMENTE ENLAZADA DE IMAGENES
   ---------------------------------------------------------- */
struct NodoImagen {
    Imagen*     imagen;
    NodoImagen* siguiente;
    NodoImagen* anterior;

    NodoImagen(Imagen* img)
        : imagen(img), siguiente(NULL), anterior(NULL) {}
};

/* ----------------------------------------------------------
   LISTA CIRCULAR DOBLEMENTE ENLAZADA DE IMAGENES
   - Ordenada por ID de imagen
   - Recorrido circular: llega al final y vuelve al inicio
   - Es la coleccion GLOBAL de todas las imagenes del sistema
   ---------------------------------------------------------- */
class ListaCircularImagenes {
private:
    NodoImagen* cabeza;    /* Nodo con menor ID (entrada) */
    int cantidad;

public:
    ListaCircularImagenes();
    ~ListaCircularImagenes();

    /* Inserta imagen manteniendo orden por ID.
       Retorna false si el ID ya existe. */
    bool insertar(Imagen* imagen);

    /* Busca y retorna imagen por ID; NULL si no existe */
    Imagen* buscar(int id) const;

    /* Elimina imagen por ID; retorna true si elimino */
    bool eliminar(int id);

    /* Genera DOT de la lista circular completa */
    std::string generarDOT(bool mostrarCapas) const;

    NodoImagen* getCabeza()   const { return cabeza;   }
    int         getCantidad() const { return cantidad; }
};

#endif
