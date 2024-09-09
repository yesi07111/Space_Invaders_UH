#ifndef P_MEMORY_C
#define P_MEMORY_C

#include <stdio.h>
#include <stddef.h>

#include <stdlib.h>

#define USE_OS_API 0 // 1 para usar las funciones del sistema
#define MEMORY_SIZE (1 << 20) // Tamaño total de la memoria simulada

typedef struct Block
{
    size_t size;
    int is_free;
    struct Block *next;
    struct Block *prev;
} Block;

char memory[MEMORY_SIZE];
Block *head = NULL;
Block *last_fit = NULL;

void p_memory_init()
{
    head = (Block *)memory;
    head->size = MEMORY_SIZE - sizeof(Block);
    head->is_free = 1;
    head->next = head;
    head->prev = head;
    last_fit = head;
}

void *p_malloc(size_t size)
{
    if (USE_OS_API)
    {
        return malloc(size);
    }

    Block *current = last_fit;
    Block *suitable = NULL;

    // Buscar un bloque adecuado usando next-fit
    do
    {
        if (current->is_free && current->size >= size + sizeof(Block))
        {
            suitable = current;
            break;
        }
        current = current->next;
        if (current == head)
            current = head->next; // Skip head in circular list
    } while (current != last_fit);

    if ((char *)suitable + sizeof(Block) + size > memory + MEMORY_SIZE)
    {
        perror("Memory allocation would exceed bounds");
        return NULL;
    }

    if (suitable == NULL)
    {
        perror("Not enough space");
        return NULL;
    }

    // Dividir el bloque si es necesario
    if (suitable->size > size + sizeof(Block) + sizeof(Block))
    {
        Block *new_node = (Block *)((char *)suitable + sizeof(Block) + size);
        new_node->size = suitable->size - size - sizeof(Block);
        new_node->is_free = 1;
        new_node->next = suitable->next;
        new_node->prev = suitable;
        suitable->next->prev = new_node;
        suitable->next = new_node;
        suitable->size = size;
    }

    suitable->is_free = 0;
    last_fit = suitable->next;
    return (void *)((char *)suitable + sizeof(Block));
}

void p_free(void *ptr)
{
    if (USE_OS_API)
    {
        free(ptr);
        return;
    }

    if (!ptr || ptr < (void *)memory || ptr >= (void *)(memory + MEMORY_SIZE))
    {
        perror("Invalid pointer in p_free");
        return;
    }

    Block *node = (Block *)((char *)ptr - sizeof(Block));
    node->is_free = 1;

    // Fusionar con nodo siguiente si está libre
    if (node->next->is_free)
    {
        node->size += node->next->size + sizeof(Block);
        node->next = node->next->next;
        node->next->prev = node;
    }

    // Fusionar con nodo anterior si está libre
    if (node->prev->is_free)
    {
        node->prev->size += node->size + sizeof(Block);
        node->prev->next = node->next;
        node->next->prev = node->prev;
        node = node->prev;
    }

    last_fit = node;
}

typedef struct
{
    int free;
    int used;
} memory_state;

void print_memory_state()
{
    Block *current = head;
    printf(">>> ");
    while (current->next != head)
    {
        printf("[%s %zu] ", current->is_free ? "Free" : "Used", current->size);
        current = current->next;
    }
    printf("\n");
}

// Función auxiliar para imprimir el estado de la memoria
memory_state get_memory_state()
{
    Block *current = head->next;
    memory_state state = (memory_state){0, 0};

    while (current != head)
    {
        if (current->is_free)
            state.free += current->size;
        else
            state.used += current->size;

        current = current->next;
    }

    return state;
}

#endif