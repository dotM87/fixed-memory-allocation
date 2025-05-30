#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <limits.h>
#include <string.h>
#include <unistd.h>

#define MEMORIA_TOTAL 2048
#define TAMAÑO_PARTICION 10
#define TOTAL_PARTICIONES (MEMORIA_TOTAL / TAMAÑO_PARTICION)

#define RESET   "\033[0m"
#define BOLD    "\033[1m"
#define RED     "\033[31m"
#define GREEN   "\033[32m"
#define YELLOW  "\033[33m"
#define BLUE    "\033[34m"
#define MAGENTA "\033[35m"
#define CYAN    "\033[36m"
#define WHITE   "\033[37m"
#define BG_BLACK "\033[40m"
#define BG_RED   "\033[41m"
#define BG_GREEN "\033[42m"
#define BG_BLUE  "\033[44m"

typedef struct {
    int numero;
    int posicion;
    int asignado;
    int proceso_id;
} Particion;

typedef struct Proceso {
    int id;
    int tamaño;
    int particion_inicio;
    int particiones_usadas;
    struct Proceso* siguiente;
} Proceso;

int memoria[MEMORIA_TOTAL];
Particion tabla_particiones[TOTAL_PARTICIONES];
Proceso* lista_procesos = NULL;
int contador_procesos = 1;

void limpiar_pantalla() {
    printf("\033[2J\033[H");
}

void imprimir_linea(char caracter, int longitud, const char* color) {
    printf("%s", color);
    for (int i = 0; i < longitud; i++) {
        printf("%c", caracter);
    }
    printf("%s\n", RESET);
}

void mostrar_header() {
    limpiar_pantalla();
    imprimir_linea('=', 80, CYAN);
    printf("%s%s", BOLD, CYAN);
    printf("                    SIMULADOR DE MEMORIA PARTICIONADA FIJA\n");
    printf("%s", RESET);
    imprimir_linea('=', 80, CYAN);
    printf("\n");
}

void mostrar_info_sistema() {
    printf("%s┌─ INFORMACIÓN DEL SISTEMA %s", BOLD BLUE, "─" RESET);
    for (int i = 0; i < 50; i++) printf("─");
    printf("\n");
    
    printf("%s│%s Memoria Total:     %s%d bytes%s\n", BLUE, RESET, GREEN, MEMORIA_TOTAL, RESET);
    printf("%s│%s Tamaño Partición:  %s%d bytes%s\n", BLUE, RESET, GREEN, TAMAÑO_PARTICION, RESET);
    printf("%s│%s Total Particiones: %s%d%s\n", BLUE, RESET, GREEN, TOTAL_PARTICIONES, RESET);
    
    printf("%s└", BLUE);
    for (int i = 0; i < 78; i++) printf("─");
    printf("%s\n\n", RESET);
}

void mostrar_estadisticas_memoria() {
    int memoria_en_uso = 0;
    int particiones_ocupadas = 0;
    
    for (int i = 0; i < TOTAL_PARTICIONES; i++) {
        if (tabla_particiones[i].asignado == 1) {
            particiones_ocupadas++;
        }
    }
    memoria_en_uso = particiones_ocupadas * TAMAÑO_PARTICION;
    
    float porcentaje_uso = ((float)memoria_en_uso / MEMORIA_TOTAL) * 100;
    
    printf("%s┌─ ESTADÍSTICAS DE MEMORIA %s", BOLD YELLOW, "─" RESET);
    for (int i = 0; i < 47; i++) printf("─");
    printf("\n");
    
    printf("%s│%s Memoria en Uso:  %s%4d bytes %s(%.1f%%)%s\n", 
           YELLOW, RESET, 
           (porcentaje_uso > 75) ? RED : (porcentaje_uso > 50) ? YELLOW : GREEN,
           memoria_en_uso, WHITE, porcentaje_uso, RESET);
    
    printf("%s│%s Memoria Libre:   %s%4d bytes %s(%.1f%%)%s\n", 
           YELLOW, RESET, GREEN, MEMORIA_TOTAL - memoria_en_uso, 
           WHITE, 100 - porcentaje_uso, RESET);
    
    printf("%s│%s Particiones:     %s%d/%d ocupadas%s\n", 
           YELLOW, RESET, CYAN, particiones_ocupadas, TOTAL_PARTICIONES, RESET);
    
    printf("%s│%s Barra de uso:    %s[", YELLOW, RESET, WHITE);
    int barras = (int)(porcentaje_uso / 2.5);
    for (int i = 0; i < 40; i++) {
        if (i < barras) {
            printf("%s█%s", (porcentaje_uso > 75) ? BG_RED : (porcentaje_uso > 50) ? BG_BLUE : BG_GREEN, RESET);
        } else {
            printf("░");
        }
    }
    printf("]%s\n", RESET);
    
    printf("%s└", YELLOW);
    for (int i = 0; i < 78; i++) printf("─");
    printf("%s\n\n", RESET);
}

void mostrar_tabla_procesos_visual() {
    printf("%s┌─ TABLA DE PROCESOS %s", BOLD MAGENTA, "─" RESET);
    for (int i = 0; i < 53; i++) printf("─");
    printf("\n");
    
    if (lista_procesos == NULL) {
        printf("%s│%s %sNo hay procesos en memoria%s\n", MAGENTA, RESET, RED, RESET);
        printf("%s└", MAGENTA);
        for (int i = 0; i < 78; i++) printf("─");
        printf("%s\n\n", RESET);
        return;
    }
    
    printf("%s│%s %sID   Tamaño  Particiones    Desperdicio    Estado%s\n", 
           MAGENTA, RESET, BOLD WHITE, RESET);
    printf("%s│%s %s──   ──────  ───────────    ───────────    ──────%s\n", 
           MAGENTA, RESET, WHITE, RESET);
    
    Proceso* actual = lista_procesos;
    while (actual != NULL) {
        int desperdicio = (actual->particiones_usadas * TAMAÑO_PARTICION) - actual->tamaño;
        float porcentaje_desp = ((float)desperdicio / (actual->particiones_usadas * TAMAÑO_PARTICION)) * 100;
        
        printf("%s│%s %s%-3d%s  %s%-6d%s  %s%d-%d%s         %s%-3d bytes%s    %s%s%.1f%%%s\n",
               MAGENTA, RESET,
               GREEN, actual->id, RESET,
               CYAN, actual->tamaño, RESET,
               YELLOW, actual->particion_inicio + 1, actual->particion_inicio + actual->particiones_usadas, RESET,
               (desperdicio > 5) ? RED : GREEN, desperdicio, RESET,
               (porcentaje_desp > 50) ? RED : (porcentaje_desp > 25) ? YELLOW : GREEN,
               BOLD, porcentaje_desp, RESET);
        
        actual = actual->siguiente;
    }
    
    printf("%s└", MAGENTA);
    for (int i = 0; i < 78; i++) printf("─");
    printf("%s\n\n", RESET);
}

void mostrar_mapa_memoria() {
    printf("%s┌─ MAPA DE MEMORIA %s", BOLD RED, "─" RESET);
    for (int i = 0; i < 56; i++) printf("─");
    printf("\n");
    
    printf("%s│%s Particiones: [%sLibre%s] [%sOcupada%s]\n", 
           RED, RESET, BG_GREEN WHITE, RESET, BG_RED WHITE, RESET);
    printf("%s│%s ", RED, RESET);
    
    for (int i = 0; i < TOTAL_PARTICIONES; i++) {
        if (i % 20 == 0 && i != 0) {
            printf("\n%s│%s ", RED, RESET);
        }
        
        if (tabla_particiones[i].asignado == 1) {
            printf("%s%s%02d%s ", BG_RED, WHITE, tabla_particiones[i].proceso_id, RESET);
        } else {
            printf("%s%s▓▓%s ", BG_GREEN, WHITE, RESET);
        }
    }
    printf("\n");
    
    printf("%s└", RED);
    for (int i = 0; i < 78; i++) printf("─");
    printf("%s\n\n", RESET);
}

void mostrar_menu_principal() {
    printf("%s┌─ MENÚ DE OPCIONES %s", BOLD WHITE, "─" RESET);
    for (int i = 0; i < 54; i++) printf("─");
    printf("\n");
    
    printf("%s│%s %s[a]%s Crear proceso          %s[e]%s Eliminar proceso\n", 
           WHITE, RESET, GREEN BOLD, RESET, RED BOLD, RESET);
    printf("%s│%s %s[b]%s Ver tabla de procesos  %s[f]%s Ver memoria detallada\n", 
           WHITE, RESET, CYAN BOLD, RESET, MAGENTA BOLD, RESET);
    printf("%s│%s %s[c]%s Ver particiones libres %s[g]%s Salir del programa\n", 
           WHITE, RESET, YELLOW BOLD, RESET, RED BOLD, RESET);
    printf("%s│%s %s[d]%s Ver particiones en uso\n", 
           WHITE, RESET, BLUE BOLD, RESET);
    
    printf("%s└", WHITE);
    for (int i = 0; i < 78; i++) printf("─");
    printf("%s\n\n", RESET);
    
    printf("%sSeleccione una opción: %s", BOLD CYAN, RESET);
}

void inicializar_sistema() {
    for (int i = 0; i < MEMORIA_TOTAL; i++) {
        memoria[i] = 0;
    }
    
    for (int i = 0; i < TOTAL_PARTICIONES; i++) {
        tabla_particiones[i].numero = i + 1;
        tabla_particiones[i].posicion = i * TAMAÑO_PARTICION;
        tabla_particiones[i].asignado = 0;
        tabla_particiones[i].proceso_id = 0;
    }
}

int buscar_particiones_libres(int particiones_necesarias) {
    int consecutivas = 0;
    int inicio = -1;
    
    for (int i = 0; i < TOTAL_PARTICIONES; i++) {
        if (tabla_particiones[i].asignado == 0) {
            if (consecutivas == 0) {
                inicio = i;
            }
            consecutivas++;
            if (consecutivas >= particiones_necesarias) {
                return inicio;
            }
        } else {
            consecutivas = 0;
        }
    }
    return -1;
}

void mostrar_mensaje_exito(const char* mensaje) {
    printf("%s%s✓ %s%s\n", BOLD GREEN, BG_GREEN WHITE, mensaje, RESET);
}

void mostrar_mensaje_error(const char* mensaje) {
    printf("%s%s✗ %s%s\n", BOLD RED, BG_RED WHITE, mensaje, RESET);
}

void crear_proceso() {
    mostrar_header();
    printf("%s%s CREANDO NUEVO PROCESO %s\n\n", BOLD GREEN, BG_GREEN WHITE, RESET);
    
    int tamaño = (rand() % 20) + 1;
    int particiones_necesarias = (tamaño + TAMAÑO_PARTICION - 1) / TAMAÑO_PARTICION;
    
    printf("Generando proceso de %s%d bytes%s (requiere %s%d particiones%s)...\n", 
           CYAN, tamaño, RESET, YELLOW, particiones_necesarias, RESET);
    
    int particion_inicio = buscar_particiones_libres(particiones_necesarias);
    
    if (particion_inicio == -1) {
        mostrar_mensaje_error("No hay particiones disponibles");
        printf("\nPresione Enter para continuar...");
        getchar();
        return;
    }
    
    Proceso* nuevo_proceso = (Proceso*)malloc(sizeof(Proceso));
    nuevo_proceso->id = contador_procesos++;
    nuevo_proceso->tamaño = tamaño;
    nuevo_proceso->particion_inicio = particion_inicio;
    nuevo_proceso->particiones_usadas = particiones_necesarias;
    nuevo_proceso->siguiente = lista_procesos;
    lista_procesos = nuevo_proceso;
    
    for (int i = particion_inicio; i < particion_inicio + particiones_necesarias; i++) {
        tabla_particiones[i].asignado = 1;
        tabla_particiones[i].proceso_id = nuevo_proceso->id;
        
        int pos_mem = tabla_particiones[i].posicion;
        for (int j = 0; j < TAMAÑO_PARTICION && (pos_mem + j) < MEMORIA_TOTAL; j++) {
            memoria[pos_mem + j] = 1;
        }
    }
    
    char mensaje[100];
    sprintf(mensaje, "Proceso %d creado exitosamente", nuevo_proceso->id);
    mostrar_mensaje_exito(mensaje);
    
    printf("\nPresione Enter para continuar...");
    getchar();
}

void ver_tabla_procesos() {
    mostrar_header();
    mostrar_estadisticas_memoria();
    mostrar_tabla_procesos_visual();
    
    printf("Presione Enter para continuar...");
    getchar();
}

void ver_particiones_libres() {
    mostrar_header();
    
    int libres = 0;
    int contiguas = 0;
    int ultima_ocupada = -1;
    
    for (int i = 0; i < TOTAL_PARTICIONES; i++) {
        if (tabla_particiones[i].asignado == 0) {
            libres++;
        } else {
            ultima_ocupada = i;
        }
    }
    
    for (int i = ultima_ocupada + 1; i < TOTAL_PARTICIONES; i++) {
        if (tabla_particiones[i].asignado == 0) {
            contiguas++;
        } else {
            break;
        }
    }
    
    printf("%s┌─ ANÁLISIS DE PARTICIONES LIBRES %s", BOLD GREEN, "─" RESET);
    for (int i = 0; i < 39; i++) printf("─");
    printf("\n");
    
    printf("%s│%s Particiones libres totales:   %s%d%s\n", 
           GREEN, RESET, BOLD CYAN, libres, RESET);
    printf("%s│%s Particiones libres contiguas: %s%d%s\n", 
           GREEN, RESET, BOLD YELLOW, contiguas, RESET);
    printf("%s│%s Memoria disponible:           %s%d bytes%s\n", 
           GREEN, RESET, BOLD WHITE, libres * TAMAÑO_PARTICION, RESET);
    
    printf("%s└", GREEN);
    for (int i = 0; i < 78; i++) printf("─");
    printf("%s\n\n", RESET);
    
    mostrar_mapa_memoria();
    
    printf("Presione Enter para continuar...");
    getchar();
}

int calcular_memoria_real_usada() {
    int total = 0;
    Proceso* actual = lista_procesos;
    while (actual != NULL) {
        total += actual->tamaño;
        actual = actual->siguiente;
    }
    return total;
}

void ver_particiones_en_uso() {
    mostrar_header();
    
    int en_uso = 0;
    float fragmentacion_total = 0;
    int procesos_count = 0;
    int max_desperdicio = 0, min_desperdicio = INT_MAX;
    int proceso_max = 0, proceso_min = 0;
    
    for (int i = 0; i < TOTAL_PARTICIONES; i++) {
        if (tabla_particiones[i].asignado == 1) {
            en_uso++;
        }
    }
    
    Proceso* actual = lista_procesos;
    while (actual != NULL) {
        int desperdicio = (actual->particiones_usadas * TAMAÑO_PARTICION) - actual->tamaño;
        float porcentaje = ((float)desperdicio / (actual->particiones_usadas * TAMAÑO_PARTICION)) * 100;
        
        fragmentacion_total += porcentaje;
        procesos_count++;
        
        if (desperdicio > max_desperdicio) {
            max_desperdicio = desperdicio;
            proceso_max = actual->id;
        }
        if (desperdicio < min_desperdicio) {
            min_desperdicio = desperdicio;
            proceso_min = actual->id;
        }
        
        actual = actual->siguiente;
    }
    
    printf("%s┌─ ANÁLISIS DE FRAGMENTACIÓN %s", BOLD RED, "─" RESET);
    for (int i = 0; i < 46; i++) printf("─");
    printf("\n");
    
    printf("%s│%s Particiones en uso:           %s%d/%d%s\n", 
           RED, RESET, CYAN, en_uso, TOTAL_PARTICIONES, RESET);
    
    if (procesos_count > 0) {
        printf("%s│%s Fragmentación interna media:  %s%.2f%%%s\n", 
               RED, RESET, YELLOW, fragmentacion_total / procesos_count, RESET);
        printf("%s│%s Mayor desperdicio:            %sProceso %d (%d bytes)%s\n", 
               RED, RESET, MAGENTA, proceso_max, max_desperdicio, RESET);
        printf("%s│%s Menor desperdicio:            %sProceso %d (%d bytes)%s\n", 
               RED, RESET, GREEN, proceso_min, min_desperdicio, RESET);
        printf("%s│%s Memoria fragmentada total:    %s%d bytes%s\n", 
               RED, RESET, WHITE, (en_uso * TAMAÑO_PARTICION) - calcular_memoria_real_usada(), RESET);
    } else {
        printf("%s│%s %sNo hay procesos en memoria%s\n", RED, RESET, YELLOW, RESET);
    }
    
    printf("%s└", RED);
    for (int i = 0; i < 78; i++) printf("─");
    printf("%s\n\n", RESET);
    
    mostrar_tabla_procesos_visual();
    
    printf("Presione Enter para continuar...");
    getchar();
}

void eliminar_proceso() {
    mostrar_header();
    printf("%s%s ELIMINANDO PROCESO %s\n\n", BOLD RED, BG_RED WHITE, RESET);
    
    if (lista_procesos == NULL) {
        mostrar_mensaje_error("No hay procesos para eliminar");
        printf("\nPresione Enter para continuar...");
        getchar();
        return;
    }
    
    int count = 0;
    Proceso* temp = lista_procesos;
    while (temp != NULL) {
        count++;
        temp = temp->siguiente;
    }
    
    int proceso_aleatorio = (rand() % count) + 1;
    int actual_count = 1;
    
    Proceso* anterior = NULL;
    Proceso* actual = lista_procesos;
    
    while (actual != NULL && actual_count < proceso_aleatorio) {
        anterior = actual;
        actual = actual->siguiente;
        actual_count++;
    }
    
    if (actual == NULL) {
        char mensaje[100];
        sprintf(mensaje, "No existe el proceso ID %d", proceso_aleatorio);
        mostrar_mensaje_error(mensaje);
        printf("\nPresione Enter para continuar...");
        getchar();
        return;
    }
    
    printf("Eliminando proceso %s%d%s (tamaño: %s%d bytes%s)...\n", 
           CYAN, actual->id, RESET, YELLOW, actual->tamaño, RESET);
    
    for (int i = actual->particion_inicio; i < actual->particion_inicio + actual->particiones_usadas; i++) {
        tabla_particiones[i].asignado = 0;
        tabla_particiones[i].proceso_id = 0;
        
        int pos_mem = tabla_particiones[i].posicion;
        for (int j = 0; j < TAMAÑO_PARTICION && (pos_mem + j) < MEMORIA_TOTAL; j++) {
            memoria[pos_mem + j] = 0;
        }
    }
    
    if (anterior == NULL) {
        lista_procesos = actual->siguiente;
    } else {
        anterior->siguiente = actual->siguiente;
    }
    
    char mensaje[100];
    sprintf(mensaje, "Proceso %d eliminado exitosamente", actual->id);
    free(actual);
    mostrar_mensaje_exito(mensaje);
    
    printf("\nPresione Enter para continuar...");
    getchar();
}

void ver_memoria() {
    mostrar_header();
    mostrar_estadisticas_memoria();
    mostrar_mapa_memoria();
    
    printf("%s┌─ VISTA DETALLADA DE MEMORIA %s", BOLD BLUE, "─" RESET);
    for (int i = 0; i < 44; i++) printf("─");
    printf("\n");
    
    printf("%s│%s Primeras 100 posiciones:\n", BLUE, RESET);
    
    for (int i = 0; i < 100 && i < MEMORIA_TOTAL; i++) {
        if (i % 20 == 0) {
            printf("%s│%s %04d: ", BLUE, RESET, i);
        }
        
        if (memoria[i] == 1) {
            printf("%s1%s ", BG_RED WHITE, RESET);
        } else {
            printf("%s0%s ", GREEN, RESET);
        }
        
        if ((i + 1) % 20 == 0) {
            printf("\n");
        }
    }
    
    printf("%s└", BLUE);
    for (int i = 0; i < 78; i++) printf("─");
    printf("%s\n\n", RESET);
    
    printf("Presione Enter para continuar...");
    getchar();
}

void mostrar_splash_screen() {
    limpiar_pantalla();
    printf("%s", CYAN);
    printf("  ███╗   ███╗███████╗███╗   ███╗ ██████╗ ██████╗ ██╗ █████╗ \n");
    printf("  ████╗ ████║██╔════╝████╗ ████║██╔═══██╗██╔══██╗██║██╔══██╗\n");
    printf("  ██╔████╔██║█████╗  ██╔████╔██║██║   ██║██████╔╝██║███████║\n");
    printf("  ██║╚██╔╝██║██╔══╝  ██║╚██╔╝██║██║   ██║██╔══██╗██║██╔══██║\n");
    printf("  ██║ ╚═╝ ██║███████╗██║ ╚═╝ ██║╚██████╔╝██║  ██║██║██║  ██║\n");
    printf("  ╚═╝     ╚═╝╚══════╝╚═╝     ╚═╝ ╚═════╝ ╚═╝  ╚═╝╚═╝╚═╝  ╚═╝\n");
    printf("%s", RESET);
    printf("\n%s          SIMULADOR DE MEMORIA FIJA PARTICIONADA v1.0%s\n", BOLD WHITE, RESET);
    printf("%s            Taller Sistemas Operativos - Práctica 2%s\n\n", YELLOW, RESET);
    
    printf("Inicializando sistema");
    for (int i = 0; i < 3; i++) {
        printf(".");
        fflush(stdout);
        usleep(500000);
    }
    printf("\n\n");
}

int main() {
    srand(time(NULL));
    char opcion;
    
    mostrar_splash_screen();
    inicializar_sistema();
    mostrar_mensaje_exito("Sistema inicializado correctamente");
    
    printf("\nPresione Enter para continuar...");
    getchar();
    
    do {
        mostrar_header();
        mostrar_info_sistema();
        mostrar_estadisticas_memoria();
        mostrar_menu_principal();
        
        scanf(" %c", &opcion);
        getchar();
        
        switch (opcion) {
            case 'a':
                crear_proceso();
                break;
            case 'b':
                ver_tabla_procesos();
                break;
            case 'c':
                ver_particiones_libres();
                break;
            case 'd':
                ver_particiones_en_uso();
                break;
            case 'e':
                eliminar_proceso();
                break;
            case 'f':
                ver_memoria();
                break;
            case 'g':
                limpiar_pantalla();
                printf("%s%s CERRANDO SISTEMA %s\n\n", BOLD RED, BG_RED WHITE, RESET);
                printf("Liberando memoria");
                for (int i = 0; i < 3; i++) {
                    printf(".");
                    fflush(stdout);
                    usleep(300000);
                }
                printf("\n\n%s¡Hasta luego!%s\n", GREEN, RESET);
                break;
            default:
                mostrar_mensaje_error("Opción no válida");
                printf("\nPresione Enter para continuar...");
                getchar();
        }
    } while (opcion != 'g');
    
    Proceso* actual = lista_procesos;
    while (actual != NULL) {
        Proceso* temp = actual;
        actual = actual->siguiente;
        free(temp);
    }
    
    return 0;
}
