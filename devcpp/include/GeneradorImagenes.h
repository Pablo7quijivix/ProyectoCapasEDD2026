#ifndef GENERADOR_IMAGENES_H
#define GENERADOR_IMAGENES_H

/*
==============================================================
  ARCHIVO  : GeneradorImagenes.h
  MODULO   : Generacion de Imagenes y Reportes
  QUE HACE : Combina capas en imagenes HTML.
             Genera reportes DOT de todas las estructuras.
             100% compatible con Windows / Dev-C++.
             NO usa funciones de Linux.
  COMPATIBLE: Dev-C++ 5.11 / MinGW / Windows
==============================================================
*/

#include "Capa.h"
#include "Imagen.h"
#include "Usuario.h"
#include <string>
#include <vector>
#include <map>
#include <utility>

/* ----------------------------------------------------------
   CLASE GENERADORIMAGENES
   Todos los metodos son estaticos: no necesita instanciarse.
   ---------------------------------------------------------- */
class GeneradorImagenes {
public:

    /* --------------------------------------------------------
       GENERACION POR RECORRIDO LIMITADO
       tipo: "inorden" / "preorden" / "postorden"
       numCapas: cuantas capas del recorrido usar (0 = todas)
       -------------------------------------------------------- */
    static std::string generarPorRecorrido(
        const std::string& tipo,
        int numCapas,
        ABBCapas& abbCapas,
        const std::string& carpeta
    );

    /* --------------------------------------------------------
       GENERACION POR IMAGEN (lista circular)
       Busca la imagen y superpone sus capas en orden.
       -------------------------------------------------------- */
    static std::string generarPorImagen(
        int idImagen,
        ListaCircularImagenes& listaImagenes,
        const std::string& carpeta
    );

    /* --------------------------------------------------------
       GENERACION POR CAPA INDIVIDUAL
       Grafica solo una capa del ABB como imagen HTML.
       -------------------------------------------------------- */
    static std::string generarPorCapa(
        int idCapa,
        ABBCapas& abbCapas,
        const std::string& carpeta
    );

    /* --------------------------------------------------------
       GENERACION POR USUARIO
       Busca al usuario y genera la imagen que seleccione.
       -------------------------------------------------------- */
    static std::string generarPorUsuario(
        const std::string& nombreUsuario,
        int idImagen,
        ABBUsuarios& abbUsuarios,
        ListaCircularImagenes& listaImagenes,
        const std::string& carpeta
    );

    /* ========================================================
       REPORTES DE ESTADO DE MEMORIA (archivos DOT/PNG)
       ======================================================== */

    /* Reporte de la lista circular de imagenes */
    static void reporteListaImagenes(
        ListaCircularImagenes& listaImagenes,
        const std::string& carpeta
    );

    /* Reporte del ABB de capas */
    static void reporteArbolCapas(
        ABBCapas& abbCapas,
        const std::string& carpeta
    );

    /* Reporte de la matriz dispersa de una capa */
    static void reporteCapa(
        int idCapa,
        ABBCapas& abbCapas,
        const std::string& carpeta
    );

    /* Reporte imagen + arbol de capas con enlaces */
    static void reporteImagenYArbol(
        int idImagen,
        ListaCircularImagenes& listaImagenes,
        ABBCapas& abbCapas,
        const std::string& carpeta
    );

    /* Reporte del ABB de usuarios */
    static void reporteArbolUsuarios(
        ABBUsuarios& abbUsuarios,
        const std::string& carpeta
    );

private:

    /* Superpone capas usando sus punteros directos */
    static std::string superponerCapasPorPunteros(
        const std::vector<Capa*>& capas,
        const std::string& nombreArchivo,
        const std::string& carpeta
    );

    /* Superpone capas buscandolas por ID en el ABB */
    static std::string superponerCapasPorIds(
        const std::vector<int>& ids,
        ABBCapas& abbCapas,
        const std::string& nombreArchivo,
        const std::string& carpeta
    );

    /* Escribe DOT e intenta compilar con Graphviz si existe */
    static void escribirDOT(
        const std::string& contenidoDOT,
        const std::string& archivoBase,
        const std::string& carpeta
    );

    /* Genera la tabla HTML de una sola capa */
    static std::string htmlDeCapa(Capa* capa, const std::string& titulo);

    /* Genera el HTML final a partir del mapa de pixeles */
    static std::string htmlDePixeles(
        const std::map<std::pair<int,int>, std::string>& pixeles,
        int maxFila, int maxCol,
        const std::string& titulo,
        const std::string& infoExtra
    );
};

#endif
