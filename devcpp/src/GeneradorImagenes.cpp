/*
==============================================================
  ARCHIVO  : GeneradorImagenes.cpp
  DESCRIPCION: Implementacion del generador de imagenes y
               reportes. 100% compatible con Windows/Dev-C++.
               No usa mkdir de Linux ni funciones POSIX.
               Usa solo la libreria estandar de C++.
==============================================================
*/

#include "GeneradorImagenes.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <cstdlib>
#include <cstring>

/* ----------------------------------------------------------
   Crea la carpeta en Windows usando el comando mkdir del SO.
   Solo se llama una vez al iniciar el programa.
   ---------------------------------------------------------- */
static void crearCarpeta(const std::string& carpeta) {
    /* En Windows: mkdir funciona en cmd sin opciones extra */
    std::string cmd = "mkdir \"" + carpeta + "\" 2>nul";
    system(cmd.c_str());
}

/* ----------------------------------------------------------
   escribirDOT: escribe el archivo .dot y trata de compilar
   con Graphviz si esta instalado (dot.exe en el PATH).
   Si no esta instalado, el .dot queda listo para usarse
   en https://dreampuf.github.io/GraphvizOnline
   ---------------------------------------------------------- */
void GeneradorImagenes::escribirDOT(const std::string& contenidoDOT,
                                     const std::string& archivoBase,
                                     const std::string& carpeta) {
    crearCarpeta(carpeta);
    std::string rutaDOT = carpeta + "\\" + archivoBase + ".dot";
    std::string rutaPNG = carpeta + "\\" + archivoBase + ".png";

    /* Escribir el archivo DOT */
    std::ofstream archivo(rutaDOT.c_str());
    if (!archivo.is_open()) {
        std::cerr << "[ERROR] No se pudo crear: " << rutaDOT << "\n";
        return;
    }
    archivo << contenidoDOT;
    archivo.close();

    /* Intentar compilar a PNG con Graphviz */
    std::string cmd = "dot -Tpng \"" + rutaDOT
                    + "\" -o \"" + rutaPNG + "\" 2>nul";
    int res = system(cmd.c_str());

    if (res == 0) {
        std::cout << "[OK] PNG generado: " << rutaPNG << "\n";
    } else {
        std::cout << "[OK] DOT generado: " << rutaDOT << "\n";
        std::cout << "     (Abre el .dot en https://dreampuf.github.io/GraphvizOnline)\n";
    }
}

/* ----------------------------------------------------------
   htmlDePixeles: construye la tabla HTML a partir del mapa
   de pixeles ya calculado. Usa colores de fondo por celda.
   ---------------------------------------------------------- */
std::string GeneradorImagenes::htmlDePixeles(
    const std::map<std::pair<int,int>, std::string>& pixeles,
    int maxFila, int maxCol,
    const std::string& titulo,
    const std::string& infoExtra)
{
    std::ostringstream html;

    html << "<!DOCTYPE html>\n<html>\n<head>\n";
    html << "<meta charset='UTF-8'>\n";
    html << "<title>" << titulo << "</title>\n";
    html << "<style>\n";
    html << "body { background:#1a1a2e; color:#eee; font-family:monospace; padding:20px; }\n";
    html << "h2   { color:#e94560; }\n";
    html << "p    { color:#aaa; margin:4px 0; }\n";
    html << "table{ border-collapse:collapse; margin-top:10px; }\n";
    html << "td   { width:20px; height:20px; border:1px solid #333; }\n";
    html << "</style>\n</head>\n<body>\n";
    html << "<h2>" << titulo << "</h2>\n";
    html << "<p>" << infoExtra << "</p>\n";
    html << "<p>Dimension: " << maxFila << " x " << maxCol << " px</p>\n";
    html << "<table>\n";

    if (maxFila == 0 || maxCol == 0) {
        /* Sin pixeles: mostrar pixel negro */
        html << "<tr><td style='background:#000000;'></td></tr>\n";
    } else {
        for (int f = 0; f < maxFila; f++) {
            html << "<tr>";
            for (int c = 0; c < maxCol; c++) {
                std::map<std::pair<int,int>, std::string>::const_iterator it =
                    pixeles.find(std::make_pair(f, c));

                std::string color = (it != pixeles.end())
                                    ? it->second
                                    : "#FFFFFF";

                html << "<td style='background:" << color
                     << ";' title='(" << f << "," << c
                     << ") " << color << "'></td>";
            }
            html << "</tr>\n";
        }
    }

    html << "</table>\n</body>\n</html>\n";
    return html.str();
}

/* ----------------------------------------------------------
   htmlDeCapa: genera el HTML de una sola capa
   ---------------------------------------------------------- */
std::string GeneradorImagenes::htmlDeCapa(Capa* capa,
                                           const std::string& titulo) {
    /* Construir el mapa de pixeles desde la matriz dispersa */
    std::map<std::pair<int,int>, std::string> pixeles;
    int maxF = 0, maxC = 0;

    NodoFila* nf = capa->matriz->getCabezaFilas();
    while (nf != NULL) {
        NodoColumna* nc = nf->columnas;
        while (nc != NULL) {
            pixeles[std::make_pair(nf->fila, nc->columna)] = nc->color;
            if (nf->fila + 1     > maxF) maxF = nf->fila + 1;
            if (nc->columna + 1  > maxC) maxC = nc->columna + 1;
            nc = nc->siguiente;
        }
        nf = nf->siguiente;
    }

    std::string info = "Capa ID: " + std::to_string(capa->id);
    return htmlDePixeles(pixeles, maxF, maxC, titulo, info);
}

/* ----------------------------------------------------------
   superponerCapasPorPunteros: recorre la lista de punteros
   de Capa en orden. La ultima capa con color en una posicion
   gana (sobreescribe). Posiciones sin color = blanco.
   ---------------------------------------------------------- */
std::string GeneradorImagenes::superponerCapasPorPunteros(
    const std::vector<Capa*>& capas,
    const std::string& nombreArchivo,
    const std::string& carpeta)
{
    std::map<std::pair<int,int>, std::string> pixeles;
    int maxF = 0, maxC = 0;

    /* Recorrer cada capa en orden de la lista */
    for (size_t i = 0; i < capas.size(); i++) {
        if (capas[i] == NULL) continue;

        NodoFila* nf = capas[i]->matriz->getCabezaFilas();
        while (nf != NULL) {
            NodoColumna* nc = nf->columnas;
            while (nc != NULL) {
                /* La ultima capa sobreescribe las anteriores */
                pixeles[std::make_pair(nf->fila, nc->columna)] = nc->color;
                if (nf->fila + 1    > maxF) maxF = nf->fila + 1;
                if (nc->columna + 1 > maxC) maxC = nc->columna + 1;
                nc = nc->siguiente;
            }
            nf = nf->siguiente;
        }
    }

    /* Construir info con los IDs de las capas superpuestas */
    std::string info = "Capas superpuestas: ";
    for (size_t i = 0; i < capas.size(); i++) {
        if (capas[i] != NULL) {
            if (i > 0) info += " -> ";
            info += "Capa " + std::to_string(capas[i]->id);
        }
    }

    crearCarpeta(carpeta);
    std::string html = htmlDePixeles(pixeles, maxF, maxC,
                                     "Imagen Generada", info);

    std::string ruta = carpeta + "\\" + nombreArchivo + ".html";
    std::ofstream archivo(ruta.c_str());
    if (archivo.is_open()) {
        archivo << html;
        archivo.close();
        std::cout << "[OK] Imagen generada: " << ruta << "\n";
    } else {
        std::cerr << "[ERROR] No se pudo crear: " << ruta << "\n";
    }
    return ruta;
}

/* ----------------------------------------------------------
   superponerCapasPorIds: busca cada ID en el ABB y superpone
   ---------------------------------------------------------- */
std::string GeneradorImagenes::superponerCapasPorIds(
    const std::vector<int>& ids,
    ABBCapas& abbCapas,
    const std::string& nombreArchivo,
    const std::string& carpeta)
{
    std::vector<Capa*> capas;
    for (size_t i = 0; i < ids.size(); i++) {
        Capa* c = abbCapas.buscar(ids[i]);
        capas.push_back(c);
        if (c == NULL) {
            std::cerr << "[WARN] Capa " << ids[i]
                      << " no encontrada.\n";
        }
    }
    return superponerCapasPorPunteros(capas, nombreArchivo, carpeta);
}

/* ==========================================================
   METODOS PUBLICOS DE GENERACION
   ========================================================== */

/* ----------------------------------------------------------
   generarPorRecorrido: recorre el ABB en el orden indicado,
   toma los primeros 'numCapas' IDs y los superpone.
   ---------------------------------------------------------- */
std::string GeneradorImagenes::generarPorRecorrido(
    const std::string& tipo,
    int numCapas,
    ABBCapas& abbCapas,
    const std::string& carpeta)
{
    std::vector<int> todos;

    if (tipo == "inorden")
        todos = abbCapas.recorridoInorden();
    else if (tipo == "preorden")
        todos = abbCapas.recorridoPreorden();
    else if (tipo == "postorden")
        todos = abbCapas.recorridoPostorden();
    else {
        std::cerr << "[ERROR] Recorrido desconocido: " << tipo << "\n";
        return "";
    }

    /* Limitar al numero de capas pedido */
    if (numCapas > 0 && (int)todos.size() > numCapas) {
        todos.resize(numCapas);
    }

    std::cout << "[INFO] Recorrido " << tipo
              << " usando " << todos.size() << " capas.\n";

    std::string nombre = "imagen_" + tipo + "_"
                       + std::to_string(numCapas);
    return superponerCapasPorIds(todos, abbCapas, nombre, carpeta);
}

/* ----------------------------------------------------------
   generarPorImagen: busca la imagen en la lista circular
   y superpone sus capas usando los punteros directos al ABB.
   ---------------------------------------------------------- */
std::string GeneradorImagenes::generarPorImagen(
    int idImagen,
    ListaCircularImagenes& listaImagenes,
    const std::string& carpeta)
{
    Imagen* img = listaImagenes.buscar(idImagen);
    if (img == NULL) {
        std::cerr << "[ERROR] Imagen " << idImagen
                  << " no encontrada.\n";
        return "";
    }

    /* Recopilar punteros de capas en el orden de la lista */
    std::vector<Capa*> capas;
    if (img->cabezaCapas != NULL) {
        NodoListaCapa* actual = img->cabezaCapas;
        do {
            capas.push_back(actual->capaPuntero);
            actual = actual->siguiente;
        } while (actual != img->cabezaCapas);
    }

    std::string nombre = "imagen_" + std::to_string(idImagen);
    return superponerCapasPorPunteros(capas, nombre, carpeta);
}

/* ----------------------------------------------------------
   generarPorCapa: muestra solo una capa del ABB
   ---------------------------------------------------------- */
std::string GeneradorImagenes::generarPorCapa(
    int idCapa,
    ABBCapas& abbCapas,
    const std::string& carpeta)
{
    Capa* capa = abbCapas.buscar(idCapa);
    if (capa == NULL) {
        std::cerr << "[ERROR] Capa " << idCapa << " no existe.\n";
        return "";
    }

    crearCarpeta(carpeta);
    std::string titulo  = "Capa " + std::to_string(idCapa);
    std::string html    = htmlDeCapa(capa, titulo);
    std::string ruta    = carpeta + "\\capa_"
                        + std::to_string(idCapa) + ".html";

    std::ofstream archivo(ruta.c_str());
    if (archivo.is_open()) {
        archivo << html;
        archivo.close();
        std::cout << "[OK] Capa " << idCapa
                  << " graficada: " << ruta << "\n";
    }
    return ruta;
}

/* ----------------------------------------------------------
   generarPorUsuario: verifica que el usuario tenga la imagen
   y delega en generarPorImagen.
   ---------------------------------------------------------- */
std::string GeneradorImagenes::generarPorUsuario(
    const std::string& nombreUsuario,
    int idImagen,
    ABBUsuarios& abbUsuarios,
    ListaCircularImagenes& listaImagenes,
    const std::string& carpeta)
{
    Usuario* usr = abbUsuarios.buscar(nombreUsuario);
    if (usr == NULL) {
        std::cerr << "[ERROR] Usuario '" << nombreUsuario
                  << "' no encontrado.\n";
        return "";
    }
    if (!usr->tieneImagen(idImagen)) {
        std::cerr << "[ERROR] El usuario '" << nombreUsuario
                  << "' no tiene la imagen " << idImagen << ".\n";
        return "";
    }
    return generarPorImagen(idImagen, listaImagenes, carpeta);
}

/* ==========================================================
   METODOS DE REPORTE (generan archivos DOT/PNG)
   ========================================================== */

void GeneradorImagenes::reporteListaImagenes(
    ListaCircularImagenes& listaImagenes,
    const std::string& carpeta)
{
    escribirDOT(listaImagenes.generarDOT(true),
                "reporte_lista_imagenes", carpeta);
}

void GeneradorImagenes::reporteArbolCapas(
    ABBCapas& abbCapas,
    const std::string& carpeta)
{
    escribirDOT(abbCapas.generarDOT(),
                "reporte_arbol_capas", carpeta);
}

void GeneradorImagenes::reporteCapa(
    int idCapa,
    ABBCapas& abbCapas,
    const std::string& carpeta)
{
    Capa* capa = abbCapas.buscar(idCapa);
    if (capa == NULL) {
        std::cerr << "[ERROR] Capa " << idCapa << " no existe.\n";
        return;
    }
    /* DOT de la matriz dispersa interna */
    escribirDOT(capa->matriz->generarDOT(idCapa),
                "reporte_capa_" + std::to_string(idCapa), carpeta);
    /* Tambien generar la imagen visual HTML */
    generarPorCapa(idCapa, abbCapas, carpeta);
}

void GeneradorImagenes::reporteImagenYArbol(
    int idImagen,
    ListaCircularImagenes& listaImagenes,
    ABBCapas& abbCapas,
    const std::string& carpeta)
{
    Imagen* img = listaImagenes.buscar(idImagen);
    if (img == NULL) {
        std::cerr << "[ERROR] Imagen " << idImagen << " no existe.\n";
        return;
    }
    /* Lista de capas de la imagen */
    escribirDOT(img->generarDOTListaCapas(),
                "reporte_img" + std::to_string(idImagen) + "_capas",
                carpeta);
    /* Arbol de capas completo */
    escribirDOT(abbCapas.generarDOT(),
                "reporte_img" + std::to_string(idImagen) + "_arbol",
                carpeta);
    std::cout << "[INFO] Los archivos DOT muestran la relacion "
              << "lista -> ABB de capas.\n";
}

void GeneradorImagenes::reporteArbolUsuarios(
    ABBUsuarios& abbUsuarios,
    const std::string& carpeta)
{
    escribirDOT(abbUsuarios.generarDOT(),
                "reporte_arbol_usuarios", carpeta);
}
