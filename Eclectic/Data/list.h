/****************************************************************************************************
 * Pragma
 ****************************************************************************************************/

#pragma once

/****************************************************************************************************
 * Define
 ****************************************************************************************************/

#define LIST_FIND_NOT_FOUND_INDEX (UINT_MAX)
#define LIST_NODE_MEMORY_SIZE     (sizeof(list_node_t))

/****************************************************************************************************
 * Include
 ****************************************************************************************************/

#include <limits.h>
#include <stdbool.h>
#include <stddef.h>

/****************************************************************************************************
 * Type Definition
 ****************************************************************************************************/

typedef int (*list_compareCallback_t)(const void * const Data1, const void * const Data2);
typedef void (*list_destroyCallback_t)(void *data);

typedef struct list_node_s
{
	void *data;
    struct list_node_s *next, *prev;
} list_node_t;

typedef struct list_list_s
{
    list_compareCallback_t compareCallback;
    list_destroyCallback_t destroyCallback;
    list_node_t *head, *tail;
    size_t size;
} list_list_t;

/****************************************************************************************************
 * Function Prototype
 ****************************************************************************************************/

extern void list_deinit(list_list_t * const list);
extern size_t list_find(const list_list_t * const List, const void * const Data);
extern void list_init(list_list_t * const list, const list_compareCallback_t CompareCallback, const list_destroyCallback_t DestroyCallback);
extern void *list_peekAt(const list_list_t * const List, const size_t Index);
extern void *list_popAt(list_list_t * const list, const size_t Index);
extern void *list_popHead(list_list_t * const list);
extern void *list_popTail(list_list_t * const list);
extern void list_pushAt(list_list_t * const list, void * const data, const size_t Index);
extern void list_pushHead(list_list_t * const list, void * const data);
extern void list_pushSorted(list_list_t * const list, void * const data, const bool Ascending);
extern void list_pushTail(list_list_t * const list, void * const data);
