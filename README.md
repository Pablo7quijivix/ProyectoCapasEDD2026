# ProyectoCapasEDD2026

#Generador de Imágenes por Capas
### Proyecto - Estructuras de Datos | Universidad Rafael Landívar 2026

---

##Descripción General

Sistema desarrollado en **C++** que genera imágenes mediante la superposición de capas de píxeles. Cada capa contiene elementos visuales definidos por coordenadas y colores en formato hexadecimal. Al combinar múltiples capas se forma una imagen completa que puede visualizarse en el navegador web.

El proyecto aplica de forma estricta los conceptos de **Estructuras de Datos** implementando cada módulo con la estructura más adecuada para su función.

---

##Estructuras de Datos Implementadas

| Estructura | Dónde se aplica |
|---|---|
| **Matriz Dispersa** | Almacena los píxeles de cada capa. Solo ocupa memoria para los píxeles que tienen color definido |
| **Lista Doblemente Enlazada** | Estructura interna de la matriz dispersa (filas y columnas) |
| **Árbol Binario de Búsqueda (ABB)** | Almacena todas las capas del sistema ordenadas por ID |
| **Lista Circular Doblemente Enlazada** | Almacena todas las imágenes del sistema ordenadas por ID |
| **Lista Doblemente Enlazada** | Lista de capas de cada imagen (punteros al ABB, no copias) |
| **Lista Simplemente Enlazada** | Lista de imágenes registradas por cada usuario |
| **Árbol Binario de Búsqueda (ABB)** | Almacena todos los usuarios del sistema ordenados por nombre |

---

##Estructura del Proyecto

```
devcpp/
├── main.cpp                    # Menú principal y punto de entrada
├── include/                    # Archivos de cabecera (.h)
│   ├── MatrizDispersa.h        # Estructura de la matriz dispersa
│   ├── Capa.h                  # ABB de capas
│   ├── Imagen.h                # Lista circular de imágenes
│   ├── Usuario.h               # ABB de usuarios
│   ├── CargaMasiva.h           # Analizador léxico y carga de archivos
│   └── GeneradorImagenes.h     # Generación de imágenes y reportes
├── src/                        # Implementaciones (.cpp)
│   ├── MatrizDispersa.cpp
│   ├── Capa.cpp
│   ├── Imagen.cpp
│   ├── Usuario.cpp
│   ├── CargaMasiva.cpp
│   └── GeneradorImagenes.cpp
├── datos/                      # Archivos de datos de prueba
│   ├── capas_ejemplo.cap       # Capas de ejemplo
│   ├── imagenes_ejemplo.im     # Imágenes de ejemplo
│   └── usuarios_ejemplo.usr    # Usuarios de ejemplo
└── reportes/                   # Carpeta donde se generan los resultados
```

---

##Requisitos

- **IDE:** Dev-C++ 5.11 o superior
- **Compilador:** TDM-GCC 4.9.2 64-bit o superior
- **Estándar:** C++11
- **Sistema Operativo:** Windows
- **Opcional:** [Graphviz](https://graphviz.org/download/) para generar reportes en PNG

---

##Instalación y Configuración

### 1. Clonar el repositorio
```bash
git clone https://github.com/Pablo7quijivix/ProyectoCapasEDD2026.git
```

### 2. Abrir en Dev-C++
- Abre Dev-C++
- Ve a **Archivo → Nuevo → Proyecto**
- Selecciona **Aplicación de consola** y marca **C++**
- Guarda el proyecto dentro de la carpeta `devcpp\`

### 3. Agregar los archivos al proyecto
Clic derecho sobre el proyecto → **Agregar al proyecto** y agrega:
```
main.cpp
src\MatrizDispersa.cpp
src\Capa.cpp
src\Imagen.cpp
src\Usuario.cpp
src\CargaMasiva.cpp
src\GeneradorImagenes.cpp
```

### 4. Configurar el compilador
IR a **Herramientas → Opciones del Compilador**
- Agregar en comandos adicionales: `-std=c++11`

### 5. Configurar los directorios de include
- IR a **Proyecto → Opciones del Proyecto → Directorios → Include Directories**
- Agregar la ruta completa de la carpeta `include\` del proyecto

### 6. Compilar y ejecutar
Presionar **F11**

---

##Uso del Sistema

### Orden de carga obligatorio
Siempre cargar los datos en este orden:
```
1. Capas    (.cap)
2. Imágenes (.im)
3. Usuarios (.usr)
```

### Menú Principal

```
1. Carga masiva de datos
2. Generación de imágenes
3. CRUD Usuarios
4. CRUD Imágenes
5. Reportes / Estado de memoria
0. Salir
```

---

##Formatos de Archivos

### Archivo de Capas (.cap)
```
id (
    fila,columna,#COLOR;
    fila,columna,#COLOR;
)
```
Ejemplo:
```
1 (
    0,0,#FF0000;
    0,1,#FF0000;
    1,0,#00FF00;
)
```

### Archivo de Imágenes (.im)
```
idImagen{idCapa,idCapa,idCapa}
```
Ejemplo:
```
1{1,2,3}
2{3,1}
3{}
```

### Archivo de Usuarios (.usr)
```
nombre:idImagen,idImagen;
```
Ejemplo:
```
userA:1,2,3;
userB:;
```

---

##Generación de Imágenes

El sistema ofrece 4 formas de generar imágenes:

**1. Por recorrido del ABB**
Recorre el árbol de capas en inorden, preorden o postorden y superpone las primeras N capas del recorrido.

**2. Por ID de imagen**
Busca la imagen en la lista circular y superpone sus capas en el orden definido.

**3. Por capa individual**
Grafica únicamente una capa del ABB mostrando sus píxeles.

**4. Por usuario**
Busca al usuario en el ABB y permite seleccionar una de sus imágenes para graficar.

Todos los resultados se guardan como archivos `.html` en la carpeta `reportes\` y pueden abrirse con cualquier navegador web.

---

##Reportes de Estado de Memoria

El sistema genera reportes visuales de todas las estructuras de datos en formato **DOT** (Graphviz):

- **Lista circular de imágenes** con sus capas enlazadas
- **Árbol ABB de capas** con su estructura completa
- **Matriz dispersa** de una capa específica
- **Imagen + árbol de capas** mostrando los punteros entre estructuras
- **Árbol ABB de usuarios** con sus imágenes registradas

Si Graphviz está instalado los reportes se generan automáticamente en PNG. Si no, los archivos `.dot` pueden visualizarse en [GraphvizOnline](https://dreampuf.github.io/GraphvizOnline).

---

##Analizador Léxico (Lexer)

El módulo de carga masiva implementa un **analizador léxico** propio que tokeniza los archivos de entrada de forma eficiente. Lee el archivo completo en memoria de una sola vez y reconoce los siguientes tokens:

| Token | Descripción |
|---|---|
| `NUMERO` | Enteros positivos para IDs, filas y columnas |
| `COLOR` | Colores hexadecimales formato `#RRGGBB` |
| `IDENTIFICADOR` | Nombres de usuario |
| `LPAREN / RPAREN` | Paréntesis `( )` |
| `LBRACE / RBRACE` | Llaves `{ }` |
| `COMA` | Separador `,` |
| `PUNTOYCOMA` | Fin de instrucción `;` |
| `DOSPUNTOS` | Separador `:` |

---

##Autor

**Pablo Quijivix** — Ingeniería en Informática y Sistemas.  
Universidad Rafael Landívar.  
3er Semestre 2026. 
Curso: Estructuras de Datos.
