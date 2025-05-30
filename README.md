# Fixed Partition Memory Simulator

"Fixed Partition Memory Simulator" es un programa en C que simula un sistema de gestión de memoria con particiones fijas. El simulador permite crear procesos de tamaño aleatorio, asignarlos a particiones de memoria, gestionar la fragmentación interna y visualizar el estado del sistema de memoria en tiempo real.

---

## Requisitos

- **Compilador C** compatible con C11 o superior (GCC, Clang, etc.).
- Entorno Unix/Linux o Windows (MinGW/Cygwin).
- `make` (opcional, si deseas usar un Makefile).

---

## Estructura del proyecto
```
├── main.c    # Código fuente principal
├── README.md            # Documentación del proyecto
├── makefile             # Makefile para compilar el programa (opcional)
```

## Características

- **Memoria particionada fija**: Simula un sistema de memoria de 2048 bytes dividido en particiones de 10 bytes.
- **Gestión de procesos**: Creación y eliminación de procesos con tamaños aleatorios (1-20 bytes).
- **Algoritmo de asignación**: Busca particiones libres consecutivas para procesos que requieren múltiples particiones.
- **Cálculo de fragmentación**: Análisis detallado de fragmentación interna y desperdicio de memoria.
- **Interfaz interactiva**: Menú con 7 opciones para gestionar el sistema.
- **Visualización de memoria**: Muestra el estado actual de la memoria y las particiones.

## Compilación

1. **Sin Makefile**  
Desde el directorio del proyecto, ejecuta:
```bash
gcc -Wall -Wextra -O2 main.c -o memory_simulator
```

2. **Con Makefile**
Si tienes un `makefile`, simplemente ejecuta:
```bash
make
```

Esto generará un ejecutable llamado `memory_simulator`.

## Uso

```bash
./memory_simulator
```

El programa iniciará con un menú interactivo que ofrece las siguientes opciones:

- **a) Crear proceso**: Genera un proceso de tamaño aleatorio y lo asigna a memoria
- **b) Ver tabla de procesos**: Muestra todos los procesos activos y estadísticas de memoria
- **c) Ver particiones libres**: Información sobre particiones disponibles
- **d) Ver particiones en uso**: Análisis de fragmentación interna y uso de memoria
- **e) Eliminar proceso**: Elimina un proceso aleatorio y libera su memoria
- **f) Ver Memoria**: Visualiza el estado actual del vector de memoria
- **g) Apagar**: Cierra el simulador

## Configuración

El programa utiliza constantes definidas que pueden modificarse según las necesidades:

```c
#define MEMORIA_TOTAL 2048        // Tamaño total de memoria en bytes
#define TAMAÑO_PARTICION 10       // Tamaño de cada partición en bytes
```

## Recursos

- [Silberschatz, A., Galvin, P. B., & Gagne, G. (2018). *Operating System Concepts* (10th ed.). John Wiley & Sons.](https://www.os-book.com/)

- [Tanenbaum, A. S., & Bos, H. (2014). *Modern Operating Systems* (4th ed.). Pearson.](https://www.pearson.com/us/higher-education/program/Tanenbaum-Modern-Operating-Systems-4th-Edition/PGM80736.html)

- [GeeksforGeeks. (n.d.). *Fixed (or static) Partitioning in Operating System*](https://www.geeksforgeeks.org/fixed-or-static-partitioning-in-operating-system/)

- [Tutorialspoint. (n.d.). *Operating System - Memory Management*](https://www.tutorialspoint.com/operating_system/os_memory_management.htm)