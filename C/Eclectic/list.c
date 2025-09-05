/****************************************************************************************************
 * Include
 ****************************************************************************************************/

#include "Eclectic/list.h"
#include "Eclectic/memory.h"
#include <stdbool.h>
#include <stddef.h>
#include <string.h>

/****************************************************************************************************
 * Function Prototype
 ****************************************************************************************************/

static bool list_pushSortedCompare(const list_list_t * const List, const void * const Data1, const void * const Data2, const bool Ascending);
static list_node_t *list_createNode(void * const data);

/****************************************************************************************************
 * Function Definition (Public)
 ****************************************************************************************************/

/*** Deinitialize ***/
void list_deinit(list_list_t * const list)
{
    /*** Deinitialize ***/
    /* Error Check */
    if(list != NULL)
    {
        /* Pop Until Empty */
        while(list->size > 0)
        {
            void *data = list_popHead(list);
            if(list->destroyCallback != NULL)
                list->destroyCallback(data);
        }

        /* Deinitialize */
        (void)memset(list, 0, sizeof(*list));
    }
}

/*** Find ***/
size_t list_find(const list_list_t * const List, const void * const Data)
{
    size_t foundIndex = LIST_FIND_NOT_FOUND_INDEX;

    /*** Find ***/
    /* Error Check */
    if((List != NULL) && (List->compareCallback != NULL))
    {
        /* Find */
        const list_node_t *Node = List->head;
        for(size_t i = 0; i < List->size; i++)
        {
            /* Compare */
            if(List->compareCallback(Data, Node->data) == 0)
            {
                foundIndex = i;
                break;
            }

            /* Go To Next Node */
            Node = Node->next;
        }
    }

    return foundIndex;
}

/*** Initialize ***/
void list_init(list_list_t * const list, const list_compareCallback_t CompareCallback, const list_destroyCallback_t DestroyCallback)
{
    /*** Initialize ***/
    /* Error Check */
    if(list != NULL)
    {
        /* Initialize */
        list->compareCallback = CompareCallback;
        list->destroyCallback = DestroyCallback;
        list->head = list->tail = NULL;
        list->size = 0;
    }
}

/*** Peek At ***/
void *list_peekAt(const list_list_t * const List, const size_t Index)
{
    void *data = NULL;

    /*** Peek At ***/
    /* Error Check */
    if((List != NULL) && (Index < List->size))
    {
        /* Peek At */
        const list_node_t *Node = List->head;
        for(size_t i = 0; i < Index; i++)
            Node = Node->next;
        data = Node->data;
    }

    return data;
}

/*** Pop At ***/
void *list_popAt(list_list_t * const list, const size_t Index)
{
    void *data = NULL;

    /*** Pop At ***/
    /* Error Check */
    if((list != NULL) && (Index < list->size))
    {
        /* Adjust List */
        if(Index == 0)
        {
            /* Node At Head */
            data = list_popHead(list);
        }
        else if(Index == (list->size - 1))
        {
            /* Node At Tail */
            data = list_popTail(list);
        }
        else
        {
            /* Find */
            list_node_t *node = list->head;
            for(size_t i = 0; i < Index; i++)
                node = node->next;
            data = node->data;

            /* Repair Hole */
            node->prev->next = node->next;
            node->next->prev = node->prev;

            /* Decrement Size */
            list->size--;

            /* Clean Up */
            memory_free((void **)&node);
        }
    }

    return data;
}

/*** Pop Head ***/
void *list_popHead(list_list_t * const list)
{
    void *data = NULL;

    /*** Pop Head ***/
    /* Error Check */
    if((list != NULL) && (list->size > 0))
    {
        /* Pop Head */
        list_node_t *oldHead = list->head;
        data = oldHead->data;

        /* Decrement Size */
        list->size--;

        /* Adjust List */
        if(list->size == 0)
        {
            /* List Is Empty */
            list->head = list->tail = NULL;
        }
        else
        {
            /* New Head Is Next Node */
            list->head = list->head->next;
            list->head->prev = NULL;
        }

        /* Clean Up */
        memory_free((void **)&oldHead);
    }

    return data;
}

/*** Pop Tail ***/
void *list_popTail(list_list_t * const list)
{
    void *data = NULL;

    /*** Pop Tail ***/
    /* Error Check */
    if((list != NULL) && (list->size > 0))
    {
        /* Pop Tail*/
        list_node_t *oldTail = list->tail;
        data = oldTail->data;

        /* Decrement Size */
        list->size--;

        /* Adjust List */
        if(list->size == 0)
        {
            /* List Is Empty */
            list->head = list->tail = NULL;
        }
        else
        {
            /* New Tail Is Previous Node */
            list->tail = list->tail->prev;
            list->tail->next = NULL;
        }

        /* Clean Up */
        memory_free((void **)&oldTail);
    }

    return data;
}

/*** Push At ***/
void list_pushAt(list_list_t * const list, void * const data, const size_t Index)
{
    /*** Push At ***/
    /* Error Check */
    if((list != NULL) && (Index <= list->size))
    {
        /* Push At */
        if(Index == 0)
        {
            /* Node At Head */
            list_pushHead(list, data);
        }
        else if(Index == list->size)
        {
            /* Node At Tail */
            list_pushTail(list, data);
        }
        else
        {
            /* Memory Allocation */
            list_node_t *newNode;
            if((newNode = list_createNode(data)) != NULL)
            {
                /* Find */
                list_node_t *oldNode = list->head;
                for(size_t i = 0; i < Index; i++)
                    oldNode = oldNode->next;

                /* Adjust */
                oldNode->prev->next = newNode;
                newNode->prev = oldNode->prev;
                newNode->next = oldNode;
                oldNode->prev = newNode;

                /* Increment Size */
                list->size++;
            }
        }
    }
}

/*** Push Head ***/
void list_pushHead(list_list_t * const list, void * const data)
{
    /*** Push Head ***/
    /* Error Check */
    if(list != NULL)
    {
        /* Memory Allocation */
        list_node_t *node;
        if((node = list_createNode(data)) != NULL)
        {
            /* Push Head */
            if(list->size == 0)
            {
                /* New Node Is Both Head And Tail */
                list->head = list->tail = node;
            }
            else
            {
                /* New Node Is New Head */
                node->next = list->head;
                list->head->prev = node;
                list->head = node;
            }

            /* Increment Size */
            list->size++;
        }
    }
}

/*** Push Sorted ***/
void list_pushSorted(list_list_t * const list, void * const data, const bool Ascending)
{
    /*** Push Sorted ***/
    /* Error Check */
    if((list != NULL) && (list->compareCallback != NULL))
    {
        /* Push Sorted */
        if((list->size == 0) || list_pushSortedCompare(list, data, list->head->data, Ascending))
        {
            /* Node At Head */
            list_pushHead(list, data);
        }
        else
        {
            /* Find */
            list_node_t *oldNode = list->head;
            for(size_t i = 0; i < list->size; i++)
            {
                if(list_pushSortedCompare(list, data, oldNode->data, Ascending))
                    break;
                oldNode = oldNode->next;
            }

            /* Adjust */
            if(oldNode == NULL)
            {
                /* Node At Tail */
                list_pushTail(list, data);
            }
            else
            {
                list_node_t *newNode;

                /* Memory Allocation */
                if((newNode = list_createNode(data)) != NULL)
                {
                    /* Adjust */
                    newNode->prev = oldNode->prev;
                    newNode->prev->next = newNode;
                    newNode->next = oldNode;
                    oldNode->prev = newNode;

                    /* Increment Size */
                    list->size++;
                }
            }
        }
    }
}

/*** Push Tail ***/
void list_pushTail(list_list_t * const list, void * const data)
{
    /*** Push Tail ***/
    /* Error Check */
    if(list != NULL)
    {
        /* Memory Allocation */
        list_node_t *node;
        if((node = list_createNode(data)) != NULL)
        {
            /* Push Tail */
            if(list->size == 0)
            {
                /* New Node Is Both Head And Tail */
                list->head = list->tail = node;
            }
            else
            {
                /* New Node Is New Tail */
                list->tail->next = node;
                node->prev = list->tail;
                list->tail = node;
            }

            /* Increment Size */
            list->size++;
        }
    }
}

/****************************************************************************************************
 * Function Definition (Private)
 ****************************************************************************************************/

/*** Push Sorted Compare ***/
static bool list_pushSortedCompare(const list_list_t * const List, const void * const Data1, const void * const Data2, const bool Ascending)
{
    /*** Push Sorted Compare ***/
    if(Ascending)
        return (List->compareCallback(Data1, Data2) < 0);
    else
        return (List->compareCallback(Data1, Data2) > 0);
}

/*** Create Node ***/
static list_node_t *list_createNode(void * const data)
{
    /*** Create Node ***/
    /* Allocate Memory */
    list_node_t *node = memory_malloc(LIST_NODE_MEMORY_SIZE);

    /* Error Check */
    if(node != NULL)
    {
        /* Initialize */
        node->data = data;
        node->next = node->prev = NULL;
    }

    return node;
}
