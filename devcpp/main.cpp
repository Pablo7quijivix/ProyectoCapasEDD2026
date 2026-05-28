/*
==============================================================
  ARCHIVO  : main.cpp
  PROYECTO : Generador de Imagenes por Capas
  AUTOR    : Estructuras de Datos
  IDE      : Dev-C++ 5.11 / Windows
  DESCRIPCION: Menu principal del sistema. Conecta todos los
               modulos: carga masiva, generacion de imagenes,
               CRUD de usuarios/imagenes y reportes graficos.
==============================================================
*/

#include <iostream>
#include <string>
#include <limits>
#include <windows.h>

/* Incluir todos los modulos del sistema */
#include "CargaMasiva.h"
#include "GeneradorImagenes.h"

/* Carpeta donde se guardan todos los reportes y HTML */
const std::string CARPETA_REPORTES = "reportes";

/* ----------------------------------------------------------
   Limpia el buffer de entrada para evitar problemas con cin
   ---------------------------------------------------------- */
void limpiarBuffer() {
    std::cin.ignore(
        std::numeric_limits<std::streamsize>::max(), '\n');
}

/* ----------------------------------------------------------
   Pausa hasta que el usuario presione Enter
   ---------------------------------------------------------- */
void pausar() {
    std::cout << "\nPresiona Enter para continuar...";
    std::cin.get();
}

/* ==========================================================
   MENU: CARGA MASIVA
   ========================================================== */
void menuCargaMasiva(ABBCapas& abbCapas,
                     ListaCircularImagenes& listaImagenes,
                     ABBUsuarios& abbUsuarios)
{
    int op;
    do {
        std::cout << "\n+----------------------------------+\n";
        std::cout << "|        CARGA MASIVA              |\n";
        std::cout << "+----------------------------------+\n";
        std::cout << "| 1. Cargar capas    (.cap)        |\n";
        std::cout << "| 2. Cargar imagenes (.im)         |\n";
        std::cout << "| 3. Cargar usuarios (.usr)        |\n";
        std::cout << "| 4. Carga completa  (1 -> 2 -> 3) |\n";
        std::cout << "| 0. Volver al menu principal      |\n";
        std::cout << "+----------------------------------+\n";
        std::cout << "Opcion: ";
        std::cin >> op;
        limpiarBuffer();

        std::string ruta;

        if (op == 1) {
            std::cout << "Ruta del archivo .cap\n";
            std::cout << "(ej: datos\\capas_ejemplo.cap): ";
            std::getline(std::cin, ruta);
            CargaMasiva::cargarCapas(ruta, abbCapas);

        } else if (op == 2) {
            std::cout << "Ruta del archivo .im\n";
            std::cout << "(ej: datos\\imagenes_ejemplo.im): ";
            std::getline(std::cin, ruta);
            CargaMasiva::cargarImagenes(ruta, listaImagenes, abbCapas);

        } else if (op == 3) {
            std::cout << "Ruta del archivo .usr\n";
            std::cout << "(ej: datos\\usuarios_ejemplo.usr): ";
            std::getline(std::cin, ruta);
            CargaMasiva::cargarUsuarios(ruta, abbUsuarios,
                                        listaImagenes);

        } else if (op == 4) {
            /* Orden obligatorio: capas -> imagenes -> usuarios */
            std::string rc, ri, ru;
            std::cout << "Archivo .cap  : "; std::getline(std::cin, rc);
            std::cout << "Archivo .im   : "; std::getline(std::cin, ri);
            std::cout << "Archivo .usr  : "; std::getline(std::cin, ru);
            CargaMasiva::cargarCapas   (rc, abbCapas);
            CargaMasiva::cargarImagenes(ri, listaImagenes, abbCapas);
            CargaMasiva::cargarUsuarios(ru, abbUsuarios, listaImagenes);
        }

        if (op != 0) pausar();

    } while (op != 0);
}

/* ==========================================================
   MENU: GENERACION DE IMAGENES
   ========================================================== */
void menuGeneracion(ABBCapas& abbCapas,
                    ListaCircularImagenes& listaImagenes,
                    ABBUsuarios& abbUsuarios)
{
    int op;
    do {
        std::cout << "\n+----------------------------------+\n";
        std::cout << "|    GENERACION DE IMAGENES        |\n";
        std::cout << "+----------------------------------+\n";
        std::cout << "| 1. Por recorrido del ABB         |\n";
        std::cout << "| 2. Por ID de imagen              |\n";
        std::cout << "| 3. Por capa individual           |\n";
        std::cout << "| 4. Por usuario                   |\n";
        std::cout << "| 0. Volver                        |\n";
        std::cout << "+----------------------------------+\n";
        std::cout << "Opcion: ";
        std::cin >> op;
        limpiarBuffer();

        if (op == 1) {
            /* Recorrido limitado del ABB */
            std::string tipo;
            int numCapas;
            std::cout << "Tipo de recorrido\n";
            std::cout << "(inorden / preorden / postorden): ";
            std::getline(std::cin, tipo);
            std::cout << "Numero de capas a usar (0 = todas): ";
            std::cin >> numCapas;
            limpiarBuffer();
            GeneradorImagenes::generarPorRecorrido(
                tipo, numCapas, abbCapas, CARPETA_REPORTES);

        } else if (op == 2) {
            /* Por ID de imagen de la lista circular */
            int idImg;
            std::cout << "ID de la imagen: ";
            std::cin >> idImg;
            limpiarBuffer();
            GeneradorImagenes::generarPorImagen(
                idImg, listaImagenes, CARPETA_REPORTES);

        } else if (op == 3) {
            /* Por capa del ABB */
            int idCapa;
            std::cout << "ID de la capa: ";
            std::cin >> idCapa;
            limpiarBuffer();
            GeneradorImagenes::generarPorCapa(
                idCapa, abbCapas, CARPETA_REPORTES);

        } else if (op == 4) {
            /* Por usuario */
            std::string nombre;
            std::cout << "Nombre del usuario: ";
            std::getline(std::cin, nombre);

            Usuario* usr = abbUsuarios.buscar(nombre);
            if (usr != NULL) {
                std::vector<int> ids = usr->obtenerIdsImagenes();
                if (ids.empty()) {
                    std::cout << "El usuario no tiene imagenes.\n";
                } else {
                    std::cout << "Imagenes del usuario: ";
                    for (size_t i = 0; i < ids.size(); i++) {
                        std::cout << ids[i];
                        if (i < ids.size()-1) std::cout << ", ";
                    }
                    std::cout << "\nID de imagen a graficar: ";
                    int idImg;
                    std::cin >> idImg;
                    limpiarBuffer();
                    GeneradorImagenes::generarPorUsuario(
                        nombre, idImg, abbUsuarios,
                        listaImagenes, CARPETA_REPORTES);
                }
            } else {
                std::cout << "[ERROR] Usuario no encontrado.\n";
            }
        }

        if (op != 0) pausar();

    } while (op != 0);
}

/* ==========================================================
   MENU: CRUD USUARIOS
   ========================================================== */
void menuCRUDUsuarios(ABBUsuarios& abbUsuarios)
{
    int op;
    do {
        std::cout << "\n+----------------------------------+\n";
        std::cout << "|         CRUD USUARIOS            |\n";
        std::cout << "+----------------------------------+\n";
        std::cout << "| 1. Agregar usuario               |\n";
        std::cout << "| 2. Eliminar usuario              |\n";
        std::cout << "| 3. Modificar (renombrar) usuario |\n";
        std::cout << "| 4. Listar todos los usuarios     |\n";
        std::cout << "| 0. Volver                        |\n";
        std::cout << "+----------------------------------+\n";
        std::cout << "Opcion: ";
        std::cin >> op;
        limpiarBuffer();

        std::string nombre;

        if (op == 1) {
            /* Agregar nuevo usuario */
            std::cout << "Nombre del nuevo usuario: ";
            std::getline(std::cin, nombre);

            if (nombre.empty()) {
                std::cout << "[ERROR] El nombre no puede ser vacio.\n";
            } else if (abbUsuarios.buscar(nombre) != NULL) {
                std::cout << "[ERROR] El usuario '"
                          << nombre << "' ya existe.\n";
            } else {
                abbUsuarios.insertar(new Usuario(nombre));
                std::cout << "[OK] Usuario '" << nombre
                          << "' creado correctamente.\n";
            }

        } else if (op == 2) {
            /* Eliminar usuario */
            std::cout << "Nombre del usuario a eliminar: ";
            std::getline(std::cin, nombre);

            if (abbUsuarios.eliminar(nombre))
                std::cout << "[OK] Usuario '" << nombre
                          << "' eliminado.\n";
            else
                std::cout << "[ERROR] Usuario no encontrado.\n";

        } else if (op == 3) {
            /* Renombrar usuario (eliminar + insertar con nuevo nombre) */
            std::string nuevoNombre;
            std::cout << "Nombre actual del usuario: ";
            std::getline(std::cin, nombre);

            Usuario* usr = abbUsuarios.buscar(nombre);
            if (usr == NULL) {
                std::cout << "[ERROR] Usuario no encontrado.\n";
            } else {
                std::cout << "Nuevo nombre: ";
                std::getline(std::cin, nuevoNombre);

                if (abbUsuarios.buscar(nuevoNombre) != NULL) {
                    std::cout << "[ERROR] Ya existe un usuario con ese nombre.\n";
                } else {
                    /* Crear nuevo usuario con los mismos datos */
                    Usuario* nuevo = new Usuario(nuevoNombre);
                    NodoImagenUsuario* img = usr->listaImagenes;
                    while (img != NULL) {
                        nuevo->agregarImagen(img->idImagen,
                                             img->imagenPuntero);
                        img = img->siguiente;
                    }
                    abbUsuarios.eliminar(nombre);
                    abbUsuarios.insertar(nuevo);
                    std::cout << "[OK] Usuario renombrado a '"
                              << nuevoNombre << "'.\n";
                }
            }

        } else if (op == 4) {
            /* Listar todos en orden alfabetico (inorden del ABB) */
            std::vector<std::string> lista =
                abbUsuarios.listarUsuarios();

            if (lista.empty()) {
                std::cout << "No hay usuarios registrados.\n";
            } else {
                std::cout << "\nUsuarios (" << lista.size() << "):\n";
                for (size_t i = 0; i < lista.size(); i++) {
                    Usuario* u = abbUsuarios.buscar(lista[i]);
                    std::cout << "  " << (i+1) << ". "
                              << lista[i]
                              << "  |  Imagenes: "
                              << u->numImagenes << "\n";
                }
            }
        }

        if (op != 0) pausar();

    } while (op != 0);
}

/* ==========================================================
   MENU: CRUD IMAGENES
   ========================================================== */
void menuCRUDImagenes(ListaCircularImagenes& listaImagenes,
                      ABBCapas& abbCapas,
                      ABBUsuarios& abbUsuarios)
{
    int op;
    do {
        std::cout << "\n+----------------------------------+\n";
        std::cout << "|         CRUD IMAGENES            |\n";
        std::cout << "+----------------------------------+\n";
        std::cout << "| 1. Agregar imagen a usuario      |\n";
        std::cout << "| 2. Eliminar imagen               |\n";
        std::cout << "| 0. Volver                        |\n";
        std::cout << "+----------------------------------+\n";
        std::cout << "Opcion: ";
        std::cin >> op;
        limpiarBuffer();

        if (op == 1) {
            /* Agregar imagen nueva */
            std::string nomUsr;
            int idImg;

            std::cout << "Nombre del usuario: ";
            std::getline(std::cin, nomUsr);

            Usuario* usr = abbUsuarios.buscar(nomUsr);
            if (usr == NULL) {
                std::cout << "[ERROR] Usuario no encontrado.\n";
                pausar();
                continue;
            }

            std::cout << "ID para la nueva imagen: ";
            std::cin >> idImg;
            limpiarBuffer();

            /* Verificar que el ID es globalmente unico */
            if (listaImagenes.buscar(idImg) != NULL) {
                std::cout << "[ERROR] La imagen " << idImg
                          << " ya existe en el sistema.\n";
                pausar();
                continue;
            }

            Imagen* nuevaImg = new Imagen(idImg);

            /* Agregar capas a la nueva imagen */
            std::cout << "Ingresa IDs de capas una por una.\n";
            std::cout << "Escribe 0 para terminar.\n";
            int idCapa;
            while (true) {
                std::cout << "  ID de capa: ";
                std::cin >> idCapa;
                if (idCapa == 0) break;
                Capa* cap = abbCapas.buscar(idCapa);
                if (cap != NULL) {
                    nuevaImg->agregarCapa(cap, idCapa);
                    std::cout << "  Capa " << idCapa
                              << " agregada.\n";
                } else {
                    std::cout << "  [WARN] Capa " << idCapa
                              << " no existe, omitiendo.\n";
                }
            }
            limpiarBuffer();

            listaImagenes.insertar(nuevaImg);
            usr->agregarImagen(idImg, nuevaImg);
            std::cout << "[OK] Imagen " << idImg
                      << " agregada al usuario '"
                      << nomUsr << "'.\n";

        } else if (op == 2) {
            /* Eliminar imagen */
            std::string nomUsr;
            int idImg;

            std::cout << "Nombre del usuario: ";
            std::getline(std::cin, nomUsr);

            Usuario* usr = abbUsuarios.buscar(nomUsr);
            if (usr == NULL) {
                std::cout << "[ERROR] Usuario no encontrado.\n";
                pausar();
                continue;
            }

            /* Mostrar sus imagenes */
            std::vector<int> ids = usr->obtenerIdsImagenes();
            if (ids.empty()) {
                std::cout << "El usuario no tiene imagenes.\n";
                pausar();
                continue;
            }
            std::cout << "Imagenes del usuario: ";
            for (size_t i = 0; i < ids.size(); i++) {
                std::cout << ids[i];
                if (i < ids.size()-1) std::cout << ", ";
            }
            std::cout << "\nID de imagen a eliminar: ";
            std::cin >> idImg;
            limpiarBuffer();

            if (!usr->tieneImagen(idImg)) {
                std::cout << "[ERROR] El usuario no tiene esa imagen.\n";
            } else {
                usr->eliminarImagen(idImg);       /* Quitar del usuario */
                listaImagenes.eliminar(idImg);    /* Quitar de la lista global */
                std::cout << "[OK] Imagen " << idImg
                          << " eliminada del usuario y del sistema.\n";
            }
        }

        if (op != 0) pausar();

    } while (op != 0);
}

/* ==========================================================
   MENU: REPORTES / ESTADO DE MEMORIA
   ========================================================== */
void menuReportes(ABBCapas& abbCapas,
                  ListaCircularImagenes& listaImagenes,
                  ABBUsuarios& abbUsuarios)
{
    int op;
    do {
        std::cout << "\n+--------------------------------------+\n";
        std::cout << "|     REPORTES / ESTADO DE MEMORIA    |\n";
        std::cout << "+--------------------------------------+\n";
        std::cout << "| 1. Ver lista circular de imagenes   |\n";
        std::cout << "| 2. Ver arbol ABB de capas           |\n";
        std::cout << "| 3. Ver capa especifica              |\n";
        std::cout << "| 4. Ver imagen y arbol de capas      |\n";
        std::cout << "| 5. Ver arbol ABB de usuarios        |\n";
        std::cout << "| 0. Volver                           |\n";
        std::cout << "+--------------------------------------+\n";
        std::cout << "Opcion: ";
        std::cin >> op;
        limpiarBuffer();

        if (op == 1) {
            GeneradorImagenes::reporteListaImagenes(
                listaImagenes, CARPETA_REPORTES);

        } else if (op == 2) {
            GeneradorImagenes::reporteArbolCapas(
                abbCapas, CARPETA_REPORTES);

        } else if (op == 3) {
            int id;
            std::cout << "ID de la capa: ";
            std::cin >> id; limpiarBuffer();
            GeneradorImagenes::reporteCapa(
                id, abbCapas, CARPETA_REPORTES);

        } else if (op == 4) {
            int id;
            std::cout << "ID de la imagen: ";
            std::cin >> id; limpiarBuffer();
            GeneradorImagenes::reporteImagenYArbol(
                id, listaImagenes, abbCapas, CARPETA_REPORTES);

        } else if (op == 5) {
            GeneradorImagenes::reporteArbolUsuarios(
                abbUsuarios, CARPETA_REPORTES);
        }

        if (op != 0) pausar();

    } while (op != 0);
}

/* ==========================================================
   PROGRAMA PRINCIPAL
   ========================================================== */
int main() {
    std::cout << "+=========================================+\n";
    std::cout << "|   SISTEMA GENERADOR DE IMAGENES        |\n";
    std::cout << "|       POR CAPAS  -  Dev-C++            |\n";
    std::cout << "|       Estructuras de Datos             |\n";
    std::cout << "+=========================================+\n\n";

    /* --------------------------------------------------
       Declaracion de las 3 estructuras principales:
       1. ABBCapas         -> Arbol Binario de Busqueda
       2. ListaCircular    -> Lista Circular Doble
       3. ABBUsuarios      -> Arbol Binario de Busqueda
       -------------------------------------------------- */
    ABBCapas               abbCapas;
    ListaCircularImagenes  listaImagenes;
    ABBUsuarios            abbUsuarios;

    /* Crear la carpeta de reportes al iniciar */
    SetCurrentDirectory("C:\\Ingeniera en Sistemas URL 2025\\3er semestre 2026\\Estructura_de_Datos\\GeneradorCapasProyecto\\GeneradorCapas_DevCPP\\devcpp");
    system("mkdir reportes 2>nul");

    std::cout << "Carpeta 'reportes' lista.\n";
    std::cout << "Carga primero los datos desde el menu 1.\n\n";

    /* --------------------------------------------------
       Menu principal del sistema
       -------------------------------------------------- */
    int op;
    do {
        std::cout << "\n+========================================+\n";
        std::cout << "|            MENU PRINCIPAL              |\n";
        std::cout << "+========================================+\n";
        std::cout << "| 1. Carga masiva de datos               |\n";
        std::cout << "| 2. Generacion de imagenes              |\n";
        std::cout << "| 3. CRUD Usuarios                       |\n";
        std::cout << "| 4. CRUD Imagenes                       |\n";
        std::cout << "| 5. Reportes / Estado de memoria        |\n";
        std::cout << "| 0. Salir                               |\n";
        std::cout << "+========================================+\n";
        std::cout << "Opcion: ";
        std::cin >> op;
        limpiarBuffer();

        switch (op) {
            case 1:
                menuCargaMasiva(abbCapas, listaImagenes, abbUsuarios);
                break;
            case 2:
                menuGeneracion(abbCapas, listaImagenes, abbUsuarios);
                break;
            case 3:
                menuCRUDUsuarios(abbUsuarios);
                break;
            case 4:
                menuCRUDImagenes(listaImagenes, abbCapas, abbUsuarios);
                break;
            case 5:
                menuReportes(abbCapas, listaImagenes, abbUsuarios);
                break;
            case 0:
                std::cout << "\nCerrando el sistema...\n";
                std::cout << "Los archivos generados estan en: reportes\\\n";
                break;
            default:
                std::cout << "Opcion invalida. Intenta de nuevo.\n";
        }

    } while (op != 0);

    /*
       Al salir del main, los destructores de ABBCapas,
       ListaCircularImagenes y ABBUsuarios se ejecutan
       automaticamente y liberan TODA la memoria usada.
    */
    return 0;
}
