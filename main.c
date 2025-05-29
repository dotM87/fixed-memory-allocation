#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define mem_size 2048
#define partition_size 10
#define total_partitions (mem_size / partition_size)

typedef struct {
    int num;
    int position;
    int status;
    int process_id;
} Partition;

typedef struct Process {
    int id;
    int size;
    int partition_num;
    int used_partitions;
    struct Process *next;
} Process;

int* memory;
Partition* partitions_table;
Process* process_list = NULL;
int process_count = 0;

void initialize_memory() {
    memory = (int*)malloc(mem_size * sizeof(int));
    memset(memory, 0, mem_size * sizeof(int));
}

void initialize_partitions() {
    partitions_table = (Partition*)malloc(total_partitions * sizeof(Partition));
    for (int i = 0; i < total_partitions; i++) {
        partitions_table[i].num = i + 1;
        partitions_table[i].position = i * partition_size;
        partitions_table[i].status = 0; // 0: free, 1: occupied
        partitions_table[i].process_id = -1; // No process assigned
    }
}
