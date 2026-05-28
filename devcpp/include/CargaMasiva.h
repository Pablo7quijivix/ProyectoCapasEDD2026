#ifndef CARGA_MASIVA_H
#define CARGA_MASIVA_H

/*
==============================================================
  ARCHIVO  : CargaMasiva.h
  MODULO   : Analizador Lexico (Lexer) + Carga Masiva
  QUE HACE : Lee archivos .cap / .im / .usr y pobla las
             estructuras de datos del sistema.
             Usa un Lexer para tokenizar el archivo completo
             de una sola lectura, lo que permite procesar
             archivos grandes de forma eficiente.
  COMPATIBLE: Dev-C++ 5.11 / MinGW / Windows
==============================================================
*/

#include "Capa.h"
#include "Imagen.h"
#include "Usuario.h"
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

/* ----------------------------------------------------------
   TIPOS DE TOKEN que reconoce el Lexer
   ---------------------------------------------------------- */
enum TipoToken {
    TOKEN_NUMERO,       /* Entero positivo: IDs, filas, columnas */
    TOKEN_COLOR,        /* Color hex: #RRGGBB                    */
    TOKEN_IDENTIFICADOR,/* Nombre de usuario                     */
    TOKEN_LPAREN,       /* (                                     */
    TOKEN_RPAREN,       /* )                                     */
    TOKEN_LBRACE,       /* {                                     */
    TOKEN_RBRACE,       /* }                                     */
    TOKEN_COMA,         /* ,                                     */
    TOKEN_PUNTOYCOMA,   /* ;                                     */
    TOKEN_DOSPUNTOS,    /* :                                     */
    TOKEN_FIN,          /* Fin del archivo                       */
    TOKEN_ERROR         /* Caracter no reconocido                */
};

/* ----------------------------------------------------------
   ESTRUCTURA TOKEN: unidad lexica reconocida
   ---------------------------------------------------------- */
struct Token {
    TipoToken   tipo;
    std::string valor;
    int         linea;

    Token(TipoToken t, const std::string& v, int l)
        : tipo(t), valor(v), linea(l) {}
};

/* ----------------------------------------------------------
   CLASE LEXER: Analizador Lexico
   Lee el archivo COMPLETO en memoria de una sola vez
   y luego entrega tokens uno a uno al parser.
   Esto es eficiente para archivos de cualquier tamanio.
   ---------------------------------------------------------- */
class Lexer {
private:
    std::string fuente;  /* Contenido completo del archivo */
    size_t      pos;     /* Posicion actual en la cadena   */
    int         linea;   /* Linea actual (para errores)    */

    /* Avanza saltando espacios y saltos de linea */
    void saltarEspacios();

    /* Lee un numero entero desde la posicion actual */
    Token leerNumero();

    /* Lee un color hexadecimal (#RRGGBB) */
    Token leerColor();

    /* Lee un identificador de usuario (letras+digitos) */
    Token leerIdentificador();

public:
    /* Constructor: recibe el contenido ya leido del archivo */
    Lexer(const std::string& contenido);

    /* Retorna el siguiente token del flujo */
    Token siguienteToken();

    /* Hay mas contenido por leer? */
    bool haySiguiente() const { return pos < fuente.size(); }
};

/* ----------------------------------------------------------
   CLASE CARGAMASIVA: carga archivos de datos al sistema.
   Orden obligatorio: capas -> imagenes -> usuarios
   ---------------------------------------------------------- */
class CargaMasiva {
public:
    /* Lee archivo .cap y carga capas al ABB */
    static int cargarCapas(const std::string& archivo,
                           ABBCapas& abbCapas);

    /* Lee archivo .im y carga imagenes a la lista circular */
    static int cargarImagenes(const std::string& archivo,
                              ListaCircularImagenes& listaImagenes,
                              ABBCapas& abbCapas);

    /* Lee archivo .usr y carga usuarios al ABB */
    static int cargarUsuarios(const std::string& archivo,
                              ABBUsuarios& abbUsuarios,
                              ListaCircularImagenes& listaImagenes);

private:
    /* Lee el archivo completo en un string de una sola vez */
    static std::string leerArchivo(const std::string& ruta);
};

#endif
