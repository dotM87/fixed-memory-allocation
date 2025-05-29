#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <limits.h>

#define MEMORIA_TOTAL 2048
#define TAMAÑO_PARTICION 10
#define TOTAL_PARTICIONES (MEMORIA_TOTAL / TAMAÑO_PARTICION)

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
    
    printf("Memoria Total: %d Bytes\n", MEMORIA_TOTAL);
    printf("Tamaño Partición: %d Bytes\n", TAMAÑO_PARTICION);
    printf("Total Particiones: %d\n\n", TOTAL_PARTICIONES);
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

void crear_proceso() {
    int tamaño = (rand() % 20) + 1;
    int particiones_necesarias = (tamaño + TAMAÑO_PARTICION - 1) / TAMAÑO_PARTICION;
    
    int particion_inicio = buscar_particiones_libres(particiones_necesarias);
    
    if (particion_inicio == -1) {
        printf("No hay particiones disponibles para el proceso de %d bytes\n", tamaño);
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
    
    printf("Proceso %d creado (tamaño: %d bytes, particiones: %d-%d)\n", 
           nuevo_proceso->id, tamaño, particion_inicio + 1, particion_inicio + particiones_necesarias);
}

void ver_tabla_procesos() {
    int memoria_en_uso = 0;
    int particiones_ocupadas = 0;
    
    for (int i = 0; i < TOTAL_PARTICIONES; i++) {
        if (tabla_particiones[i].asignado == 1) {
            particiones_ocupadas++;
        }
    }
    memoria_en_uso = particiones_ocupadas * TAMAÑO_PARTICION;
    
    printf("Memoria Total: %d Bytes\n", MEMORIA_TOTAL);
    printf("Memoria en Uso: %d Bytes\n", memoria_en_uso);
    printf("Memoria Libre: %d Bytes\n", MEMORIA_TOTAL - memoria_en_uso);
    printf("Porcentaje de Uso: %.2f%%\n", ((float)memoria_en_uso / MEMORIA_TOTAL) * 100);
    printf("\nTabla de Procesos:\n");
    printf("ID\tTamaño\tParticiones\tDesperdicio\n");
    printf("--\t------\t-----------\t-----------\n");
    
    Proceso* actual = lista_procesos;
    while (actual != NULL) {
        int desperdicio = (actual->particiones_usadas * TAMAÑO_PARTICION) - actual->tamaño;
        printf("%d\t%d\t%d-%d\t\t%d bytes\n", 
               actual->id, actual->tamaño, 
               actual->particion_inicio + 1, 
               actual->particion_inicio + actual->particiones_usadas,
               desperdicio);
        actual = actual->siguiente;
    }
}

void ver_particiones_libres() {
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
    
    printf("# de particiones libres: %d\n", libres);
    printf("# de particiones libres contiguas: %d\n", contiguas);
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
    
    printf("Número Total de Particiones: %d\n", TOTAL_PARTICIONES);
    printf("Número de Particiones en uso: %d\n", en_uso);
    
    if (procesos_count > 0) {
        printf("%% Fragmentación Interna: %.2f%%\n", fragmentacion_total / procesos_count);
        printf("Proceso con mayor desperdicio: %d (%d bytes)\n", proceso_max, max_desperdicio);
        printf("Proceso con menor desperdicio: %d (%d bytes)\n", proceso_min, min_desperdicio);
        printf("Total Memoria fragmentada: %d bytes\n", 
               (en_uso * TAMAÑO_PARTICION) - calcular_memoria_real_usada());
    } else {
        printf("%% Fragmentación Interna: 0\n");
        printf("Proceso con mayor desperdicio: 0\n");
        printf("Proceso con menor desperdicio: 0\n");
        printf("Total Memoria fragmentada: 0 bytes\n");
    }
}

void eliminar_proceso() {
    if (lista_procesos == NULL) {
        printf("No hay procesos para eliminar\n");
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
        printf("No existe el ID (%d) de proceso\n", proceso_aleatorio);
        return;
    }
    
    printf("Proceso eliminado: ID %d, tamaño %d bytes\n", actual->id, actual->tamaño);
    
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
    
    free(actual);
}

void ver_memoria() {
    int memoria_en_uso = 0;
    for (int i = 0; i < TOTAL_PARTICIONES; i++) {
        if (tabla_particiones[i].asignado == 1) {
            memoria_en_uso += TAMAÑO_PARTICION;
        }
    }
    
    printf("Memoria Total: %d Bytes\n", MEMORIA_TOTAL);
    printf("Memoria en Uso: %d Bytes\n", memoria_en_uso);
    printf("Memoria Libre: %d Bytes\n", MEMORIA_TOTAL - memoria_en_uso);
    
    printf("\nPrimeras 100 posiciones de memoria:\n");
    for (int i = 0; i < 100 && i < MEMORIA_TOTAL; i++) {
        if (i % 20 == 0) printf("\n%04d: ", i);
        printf("%d ", memoria[i]);
    }
    printf("\n");
}

void mostrar_menu() {
    printf("\n=== SIMULADOR DE MEMORIA PARTICIONADA FIJA ===\n");
    printf("a) Crear proceso\n");
    printf("b) Ver tabla de procesos\n");
    printf("c) Ver particiones libres\n");
    printf("d) Ver particiones en uso\n");
    printf("e) Eliminar proceso\n");
    printf("f) Ver Memoria\n");
    printf("g) Apagar\n");
    printf("Seleccione una opción: ");
}

int main() {
    srand(time(NULL));
    char opcion;
    
    inicializar_sistema();
    
    do {
        mostrar_menu();
        scanf(" %c", &opcion);
        
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
                printf("Apagando sistema...\n");
                break;
            default:
                printf("Opción no válida\n");
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