/*
==============================================================
  ARCHIVO  : CargaMasiva.cpp
  DESCRIPCION: Implementacion del Lexer y los parsers para
               carga masiva de .cap / .im / .usr
==============================================================
*/

#include "CargaMasiva.h"
#include <cctype>
#include <cstdlib>

/* ==========================================================
   LEXER - Implementacion
   ========================================================== */

Lexer::Lexer(const std::string& contenido)
    : fuente(contenido), pos(0), linea(1) {}

/* ----------------------------------------------------------
   saltarEspacios: avanza la posicion saltando blancos.
   Cuenta saltos de linea para reportar numero de linea.
   ---------------------------------------------------------- */
void Lexer::saltarEspacios() {
    while (pos < fuente.size()) {
        char c = fuente[pos];
        if      (c == '\n') { linea++; pos++; }
        else if (c == ' ' || c == '\t' || c == '\r') { pos++; }
        else break;
    }
}

/* ----------------------------------------------------------
   leerNumero: consume todos los digitos desde pos actual
   ---------------------------------------------------------- */
Token Lexer::leerNumero() {
    std::string num;
    while (pos < fuente.size() && isdigit((unsigned char)fuente[pos])) {
        num += fuente[pos++];
    }
    return Token(TOKEN_NUMERO, num, linea);
}

/* ----------------------------------------------------------
   leerColor: consume '#' seguido de exactamente 6 hex.
   Convierte a mayusculas para uniformidad.
   ---------------------------------------------------------- */
Token Lexer::leerColor() {
    std::string color = "#";
    pos++;  /* Consumir el '#' */
    int count = 0;

    while (pos < fuente.size() && count < 6 &&
           isxdigit((unsigned char)fuente[pos])) {
        color += (char)toupper((unsigned char)fuente[pos++]);
        count++;
    }

    if (count < 6) {
        return Token(TOKEN_ERROR, "Color invalido: " + color, linea);
    }
    return Token(TOKEN_COLOR, color, linea);
}

/* ----------------------------------------------------------
   leerIdentificador: letras, digitos y guion bajo
   ---------------------------------------------------------- */
Token Lexer::leerIdentificador() {
    std::string id;
    while (pos < fuente.size() &&
           (isalnum((unsigned char)fuente[pos]) || fuente[pos] == '_')) {
        id += fuente[pos++];
    }
    return Token(TOKEN_IDENTIFICADOR, id, linea);
}

/* ----------------------------------------------------------
   siguienteToken: punto de entrada principal del Lexer.
   Salta espacios y retorna el siguiente token reconocido.
   ---------------------------------------------------------- */
Token Lexer::siguienteToken() {
    saltarEspacios();

    if (pos >= fuente.size())
        return Token(TOKEN_FIN, "", linea);

    char c = fuente[pos];

    if (c == '(') { pos++; return Token(TOKEN_LPAREN,       "(", linea); }
    if (c == ')') { pos++; return Token(TOKEN_RPAREN,       ")", linea); }
    if (c == '{') { pos++; return Token(TOKEN_LBRACE,       "{", linea); }
    if (c == '}') { pos++; return Token(TOKEN_RBRACE,       "}", linea); }
    if (c == ',') { pos++; return Token(TOKEN_COMA,         ",", linea); }
    if (c == ';') { pos++; return Token(TOKEN_PUNTOYCOMA,   ";", linea); }
    if (c == ':') { pos++; return Token(TOKEN_DOSPUNTOS,    ":", linea); }
    if (c == '#') return leerColor();
    if (isdigit ((unsigned char)c)) return leerNumero();
    if (isalpha ((unsigned char)c) || c == '_') return leerIdentificador();

    /* Caracter no reconocido: avanzar y reportar */
    pos++;
    return Token(TOKEN_ERROR, std::string(1, c), linea);
}

/* ==========================================================
   CARGAMASIVA - Implementacion
   ========================================================== */

/* ----------------------------------------------------------
   leerArchivo: lee el contenido completo del archivo
   en un solo std::string. Usa seekg/tellg para saber el
   tamanio exacto y reservar memoria de una vez (eficiente).
   ---------------------------------------------------------- */
std::string CargaMasiva::leerArchivo(const std::string& ruta) {
    std::ifstream archivo(ruta.c_str(),
                          std::ios::binary | std::ios::ate);
    if (!archivo.is_open()) {
        std::cerr << "[ERROR] No se pudo abrir: " << ruta << "\n";
        return "";
    }

    /* Obtener tamanio sin recorrer el archivo */
    std::streamsize tamanio = archivo.tellg();
    archivo.seekg(0, std::ios::beg);

    /* Reservar memoria exacta y leer de una sola vez */
    std::string contenido(static_cast<size_t>(tamanio), '\0');
    archivo.read(&contenido[0], tamanio);
    return contenido;
}

/* ----------------------------------------------------------
   cargarCapas: parsea archivo .cap con el Lexer.

   Sintaxis esperada:
     id (
       fila, columna, #COLOR;
       ...
     )
   ---------------------------------------------------------- */
int CargaMasiva::cargarCapas(const std::string& archivo,
                              ABBCapas& abbCapas) {
    std::string contenido = leerArchivo(archivo);
    if (contenido.empty()) return -1;

    Lexer lexer(contenido);
    int   capasInsertadas = 0;

    Token tok = lexer.siguienteToken();

    while (tok.tipo != TOKEN_FIN) {

        /* Esperar ID de capa (numero) */
        if (tok.tipo != TOKEN_NUMERO) {
            tok = lexer.siguienteToken();
            continue;
        }
        int idCapa = atoi(tok.valor.c_str());

        /* Esperar '(' */
        tok = lexer.siguienteToken();
        if (tok.tipo != TOKEN_LPAREN && tok.tipo != TOKEN_LBRACE) {
            std::cerr << "[WARN] Esperado '(' para capa "
                      << idCapa << " linea " << tok.linea << "\n";
            continue;
        }

        /* Crear capa (o usar existente si ya estaba) */
        Capa* capaExistente = abbCapas.buscar(idCapa);
        Capa* capa = (capaExistente != NULL)
                     ? capaExistente
                     : new Capa(idCapa);

        tok = lexer.siguienteToken();

        /* Leer pixeles: fila , columna , #COLOR ; */
        while (tok.tipo != TOKEN_RPAREN && tok.tipo != TOKEN_RBRACE && tok.tipo != TOKEN_FIN) {

            if (tok.tipo == TOKEN_NUMERO) {
                int fila = atoi(tok.valor.c_str());

                tok = lexer.siguienteToken();   /* coma */
                if (tok.tipo != TOKEN_COMA) break;

                tok = lexer.siguienteToken();   /* columna */
                if (tok.tipo != TOKEN_NUMERO) break;
                int columna = atoi(tok.valor.c_str());

                tok = lexer.siguienteToken();   /* coma */
                if (tok.tipo != TOKEN_COMA) break;

                tok = lexer.siguienteToken();   /* color */
                if (tok.tipo != TOKEN_COLOR) break;
                std::string color = tok.valor;

                tok = lexer.siguienteToken();   /* ; (permisivo) */

                capa->matriz->insertarPixel(fila, columna, color);
            } else {
                tok = lexer.siguienteToken();
            }
        }

        /* Insertar en el ABB solo si es nueva */
        if (capaExistente == NULL) {
            abbCapas.insertar(capa);
            capasInsertadas++;
        }

        tok = lexer.siguienteToken();
    }

    std::cout << "[INFO] Capas cargadas: " << capasInsertadas
              << " desde " << archivo << "\n";
    return capasInsertadas;
}

/* ----------------------------------------------------------
   cargarImagenes: parsea archivo .im con el Lexer.

   Sintaxis esperada:
     id { idCapa, idCapa, ... }
   ---------------------------------------------------------- */
int CargaMasiva::cargarImagenes(const std::string& archivo,
                                 ListaCircularImagenes& listaImagenes,
                                 ABBCapas& abbCapas) {
    std::string contenido = leerArchivo(archivo);
    if (contenido.empty()) return -1;

    Lexer lexer(contenido);
    int   imagenesInsertadas = 0;

    Token tok = lexer.siguienteToken();

    while (tok.tipo != TOKEN_FIN) {

        /* Esperar ID de imagen */
        if (tok.tipo != TOKEN_NUMERO) {
            tok = lexer.siguienteToken();
            continue;
        }
        int idImagen = atoi(tok.valor.c_str());

        /* Esperar '{' */
        tok = lexer.siguienteToken();
        if (tok.tipo != TOKEN_LBRACE) {
            std::cerr << "[WARN] Esperado '{' para imagen "
                      << idImagen << "\n";
            continue;
        }

        /* Verificar que el ID es unico */
        if (listaImagenes.buscar(idImagen) != NULL) {
            std::cerr << "[WARN] Imagen " << idImagen
                      << " ya existe. Omitiendo.\n";
            /* Consumir hasta '}' */
            while (tok.tipo != TOKEN_RBRACE && tok.tipo != TOKEN_FIN)
                tok = lexer.siguienteToken();
            tok = lexer.siguienteToken();
            continue;
        }

        Imagen* imagen = new Imagen(idImagen);
        tok = lexer.siguienteToken();

        /* Leer IDs de capas separados por coma */
        while (tok.tipo != TOKEN_RBRACE && tok.tipo != TOKEN_FIN) {
            if (tok.tipo == TOKEN_NUMERO) {
                int idCapa = atoi(tok.valor.c_str());
                Capa* cap  = abbCapas.buscar(idCapa);
                if (cap != NULL) {
                    imagen->agregarCapa(cap, idCapa);
                } else {
                    std::cerr << "[WARN] Capa " << idCapa
                              << " no encontrada para imagen "
                              << idImagen << "\n";
                }
            }
            tok = lexer.siguienteToken();
        }

        listaImagenes.insertar(imagen);
        imagenesInsertadas++;
        tok = lexer.siguienteToken();
    }

    std::cout << "[INFO] Imagenes cargadas: " << imagenesInsertadas
              << " desde " << archivo << "\n";
    return imagenesInsertadas;
}

/* ----------------------------------------------------------
   cargarUsuarios: parsea archivo .usr con el Lexer.

   Sintaxis esperada:
     nombre : idImg, idImg, ... ;
     nombre : ;   (sin imagenes)
   ---------------------------------------------------------- */
int CargaMasiva::cargarUsuarios(const std::string& archivo,
                                 ABBUsuarios& abbUsuarios,
                                 ListaCircularImagenes& listaImagenes) {
    std::string contenido = leerArchivo(archivo);
    if (contenido.empty()) return -1;

    Lexer lexer(contenido);
    int   usuariosInsertados = 0;

    Token tok = lexer.siguienteToken();

    while (tok.tipo != TOKEN_FIN) {

        /* Esperar nombre de usuario */
        if (tok.tipo != TOKEN_IDENTIFICADOR) {
            tok = lexer.siguienteToken();
            continue;
        }
        std::string nombre = tok.valor;

        /* Esperar ':' */
        tok = lexer.siguienteToken();
        if (tok.tipo != TOKEN_DOSPUNTOS) {
            std::cerr << "[WARN] Esperado ':' para usuario "
                      << nombre << "\n";
            continue;
        }

        /* Si ya existe, saltar hasta ';' */
        if (abbUsuarios.buscar(nombre) != NULL) {
            std::cerr << "[WARN] Usuario " << nombre
                      << " ya existe. Omitiendo.\n";
            while (tok.tipo != TOKEN_PUNTOYCOMA && tok.tipo != TOKEN_FIN)
                tok = lexer.siguienteToken();
            tok = lexer.siguienteToken();
            continue;
        }

        Usuario* usuario = new Usuario(nombre);
        tok = lexer.siguienteToken();

        /* Leer IDs de imagenes hasta ';' */
        while (tok.tipo != TOKEN_PUNTOYCOMA && tok.tipo != TOKEN_FIN) {
            if (tok.tipo == TOKEN_NUMERO) {
                int idImg = atoi(tok.valor.c_str());
                Imagen* img = listaImagenes.buscar(idImg);
                if (img != NULL) {
                    usuario->agregarImagen(idImg, img);
                } else {
                    std::cerr << "[WARN] Imagen " << idImg
                              << " no encontrada para "
                              << nombre << "\n";
                }
            }
            tok = lexer.siguienteToken();
        }

        abbUsuarios.insertar(usuario);
        usuariosInsertados++;
        tok = lexer.siguienteToken();  /* Consumir ';' */
    }

    std::cout << "[INFO] Usuarios cargados: " << usuariosInsertados
              << " desde " << archivo << "\n";
    return usuariosInsertados;
}
