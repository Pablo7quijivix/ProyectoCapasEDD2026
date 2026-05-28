#ifndef MATRIZ_DISPERSA_H
#define MATRIZ_DISPERSA_H

/*
==============================================================
  ARCHIVO  : MatrizDispersa.h
  ESTRUCTURA: Lista Doblemente Enlazada de Listas Doblemente
              Enlazadas  =>  Matriz Dispersa
  QUE HACE : Guarda solo los pixeles que tienen color.
             Las posiciones sin color se muestran blancas
             al generar la imagen.
  COMPATIBLE: Dev-C++ 5.11 / MinGW / Windows
==============================================================
*/

#include <string>
#include <sstream>

/* ----------------------------------------------------------
   NODO COLUMNA
   Representa UN pixel dentro de una fila.
   Campos:
     columna   -> posicion horizontal del pixel
     color     -> color en hex, ej: #FF0000
     siguiente -> proximo pixel en la misma fila
     anterior  -> pixel anterior en la misma fila
   ---------------------------------------------------------- */
struct NodoColumna {
    int columna;
    std::string color;
    NodoColumna* siguiente;
    NodoColumna* anterior;

    NodoColumna(int c, const std::string& col)
        : columna(c), color(col), siguiente(NULL), anterior(NULL) {}
};

/* ----------------------------------------------------------
   NODO FILA
   Representa UNA fila de la matriz.
   Tiene su propia lista doblemente enlazada de columnas.
   ---------------------------------------------------------- */
struct NodoFila {
    int fila;
    NodoColumna* columnas;  // Primer pixel de esta fila
    NodoFila*    siguiente;
    NodoFila*    anterior;

    NodoFila(int f)
        : fila(f), columnas(NULL), siguiente(NULL), anterior(NULL) {}
};

/* ----------------------------------------------------------
   CLASE MATRIZDISPERSA
   Lista doble de NodoFila, cada uno con lista doble de
   NodoColumna. Solo ocupa memoria para pixeles existentes.
   ---------------------------------------------------------- */
class MatrizDispersa {
private:
    NodoFila* cabezaFilas;  // Primer nodo de la lista de filas
    int maxFila;            // Indice mayor de fila registrado
    int maxColumna;         // Indice mayor de columna registrado

    /* Busca la fila; si no existe la crea en orden ascendente */
    NodoFila* obtenerFila(int fila);

public:
    MatrizDispersa();
    ~MatrizDispersa();

    /* Inserta o actualiza un pixel en (fila, columna) */
    void insertarPixel(int fila, int columna, const std::string& color);

    /* Retorna el color del pixel, o "" si no existe */
    std::string obtenerPixel(int fila, int columna) const;

    /* Genera codigo DOT de la estructura para Graphviz */
    std::string generarDOT(int idCapa) const;

    int       getMaxFila()     const { return maxFila;     }
    int       getMaxColumna()  const { return maxColumna;  }
    NodoFila* getCabezaFilas() const { return cabezaFilas; }
};

#endif
