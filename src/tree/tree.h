/****************************************************************************************************
 * Pragma
 ****************************************************************************************************/

#pragma once

/****************************************************************************************************
 * Include
 ****************************************************************************************************/

#include "eclectic.h"
#include "list.h"
#include <stdbool.h>
#include <stddef.h>

/****************************************************************************************************
 * Type Definition
 ****************************************************************************************************/

/*** Function Pointer ***/
typedef int (*tree_compareCallback_t)(const void * const Data1, const void * const Data2);
typedef void (*tree_destroyCallback_t)(void *data);

/*** Enumeration ***/
typedef enum tree_order_e
{
	TREE_ORDER_INORDER,
    TREE_ORDER_POSTORDER,
    TREE_ORDER_PREORDER,
    TREE_ORDER_COUNT
} tree_order_t;

typedef enum tree_position_e
{
	TREE_POSITION_LEFT,
    TREE_POSITION_RIGHT,
    TREE_POSITION_ROOT,
    TREE_POSITION_COUNT
} tree_position_t;

/*** Structure ***/
typedef struct tree_node_s
{
	void *data;
    struct tree_node_s *left, *right;
} tree_node_t;

typedef struct tree_tree_s
{
    tree_compareCallback_t compareCallback;
    tree_destroyCallback_t destroyCallback;
    tree_node_t *root;
    size_t size;
} tree_tree_t;

/****************************************************************************************************
 * Function Prototype
 ****************************************************************************************************/

extern eclectic_status_t tree_destroy(tree_tree_t * const tree);
extern bool tree_find(const tree_tree_t * const Tree, const void * const Data);
extern eclectic_status_t tree_init(tree_tree_t * const tree, const tree_compareCallback_t CompareCallback, const tree_destroyCallback_t DestroyCallback);
extern eclectic_status_t tree_insert(tree_tree_t * const tree, tree_node_t * const node, void * const data, const tree_position_t Position);
extern eclectic_status_t tree_order(const tree_tree_t * const Tree, list_list_t * const list, const tree_order_t Order);
extern eclectic_status_t tree_remove(tree_tree_t * const tree, tree_node_t * const node, const tree_position_t Position);
extern size_t tree_size(const tree_tree_t * const Tree);
