/****************************************************************************************************
 * Include
 ****************************************************************************************************/

#include "tree.h"

/****************************************************************************************************
 * Function Prototype
 ****************************************************************************************************/

static tree_node_t *tree_createNode(void * const data);
static void tree_inorder(const tree_node_t * const Node, list_list_t * const list);
static void tree_postorder(const tree_node_t * const Node, list_list_t * const list);
static void tree_preorder(const tree_node_t * const Node, list_list_t * const list);

/****************************************************************************************************
 * Function Definition (Public)
 ****************************************************************************************************/

/*** Destroy ***/
eclectic_status_t tree_destroy(tree_tree_t * const tree)
{  
    /*** Error Check ***/
    /* NULL Pointer */
    if(tree == NULL)
        return ECLECTIC_STATUS_ERROR_NULL_POINTER;
    
    /*** Destroy ***/
    (void)tree_remove(tree, tree->root, TREE_POSITION_ROOT);
    
    return ECLECTIC_STATUS_SUCCESS;
}

/*** Find ***/
bool tree_find(const tree_tree_t * const Tree, const void * const Data)
{
    bool found = false;
    list_list_t list;
    
    /*** Find ***/
    if((Tree != NULL) && (Tree->compareCallback != NULL))
    {
        /* List */
        (void)list_init(&list, Tree->compareCallback, NULL);
        (void)tree_order(Tree, &list, TREE_ORDER_INORDER); // TREE_ORDER_POSTORDER And TREE_ORDER_PREORDER Should Produce The Same Result
        
        /* Find */
        found = (list_find(&list, Data) != LIST_FIND_NOT_FOUND_INDEX);
        
        /* Clean Up */
        (void)list_destroy(&list);
    }
    
    return found;
}

/*** Initialize ***/
eclectic_status_t tree_init(tree_tree_t * const tree, const tree_compareCallback_t CompareCallback, const tree_destroyCallback_t DestroyCallback)
{    
    /*** Error Check ***/
    /* NULL Pointer */
    if(tree == NULL)
        return ECLECTIC_STATUS_ERROR_NULL_POINTER;
    
    /*** Initialize ***/
    tree->compareCallback = CompareCallback;
    tree->destroyCallback = DestroyCallback;
    tree->root = NULL;
    tree->size = 0;
    
    return ECLECTIC_STATUS_SUCCESS;
}

/*** Insert ***/
eclectic_status_t tree_insert(tree_tree_t * const tree, tree_node_t * const node, void * const data, const tree_position_t Position)
{
    tree_node_t *newNode;
    
    /*** Error Check ***/
    /* NULL Pointer */
    if(tree == NULL)
        return ECLECTIC_STATUS_ERROR_NULL_POINTER;
    
    /*** Insert ***/
    /* Get Position And NULL Pointer/Not Empty/Memory Allocation/Invalid Error Check */
    switch(Position)
    {
        case TREE_POSITION_LEFT:
            if(node == NULL)
                return ECLECTIC_STATUS_ERROR_NULL_POINTER;
            else if(node->left != NULL)
                return ECLECTIC_STATUS_ERROR_NOT_EMPTY;
            else if((newNode = tree_createNode(data)) == NULL)
                return ECLECTIC_STATUS_ERROR_MEMORY_ALLOCATION;
            node->left = newNode;
            break;
        case TREE_POSITION_RIGHT:
            if(node == NULL)
                return ECLECTIC_STATUS_ERROR_NULL_POINTER;
            else if(node->right != NULL)
                return ECLECTIC_STATUS_ERROR_NOT_EMPTY;
            else if((newNode = tree_createNode(data)) == NULL)
                return ECLECTIC_STATUS_ERROR_MEMORY_ALLOCATION;
            node->right = newNode;
            break;
        case TREE_POSITION_ROOT:
            if(node != NULL)
                return ECLECTIC_STATUS_ERROR_NOT_EMPTY;
            else if((newNode = tree_createNode(data)) == NULL)
                return ECLECTIC_STATUS_ERROR_MEMORY_ALLOCATION;
            tree->root = newNode;
            break;
        default:
            /* Invalid */
            return ECLECTIC_STATUS_ERROR_INVALID;
            break;
    }
    
    /* Increment Size */
    tree->size++;
    
    return ECLECTIC_STATUS_SUCCESS;
}

/*** Order ***/
eclectic_status_t tree_order(const tree_tree_t * const Tree, list_list_t * const list, const tree_order_t Order)
{    
    /*** Error Check ***/
    /* NULL Pointer */
    if((Tree == NULL) || (list == NULL))
        return ECLECTIC_STATUS_ERROR_NULL_POINTER;
    
    /* Length */
    if(Tree->size == 0)
        return ECLECTIC_STATUS_ERROR_LENGTH;
    
    /*** Order ***/
    /* Get Position And Invalid Error Check */
    switch(Order)
    {
        case TREE_ORDER_INORDER:
            tree_inorder(Tree->root, list);
            break;
        case TREE_ORDER_POSTORDER:
            tree_postorder(Tree->root, list);
            break;
        case TREE_ORDER_PREORDER:
            tree_preorder(Tree->root, list);
            break;
        default:
            /* Invalid */
            return ECLECTIC_STATUS_ERROR_INVALID;
            break;
    }
    
    return ECLECTIC_STATUS_SUCCESS;
}

/*** Remove ***/
eclectic_status_t tree_remove(tree_tree_t * const tree, tree_node_t * const node, const tree_position_t Position)
{
    tree_node_t *position;
    
    /*** Error Check ***/
    /* NULL Pointer */
    if(tree == NULL)
        return ECLECTIC_STATUS_ERROR_NULL_POINTER;
    
    /*** Remove ***/
    if(Position == TREE_POSITION_ROOT)
    {
        /* NULL Pointer Error Check */
        if(tree->root == NULL)
            return ECLECTIC_STATUS_ERROR_NULL_POINTER;
        
        /* Remove Leaves */
        (void)tree_remove(tree, tree->root, TREE_POSITION_LEFT);
        (void)tree_remove(tree, tree->root, TREE_POSITION_RIGHT);
        
        /* Remove Root */
        if(tree->destroyCallback != NULL)
            tree->destroyCallback(tree->root->data);
        eclectic_free((void **)&tree->root);
        
        /* Reinitialize */
        (void)tree_init(tree, tree->compareCallback, tree->destroyCallback);
    }
    else
    {
        /* Get Position And NULL Pointer/Invalid Error Check */
        switch(Position)
        {
            case TREE_POSITION_LEFT:
                if((node == NULL) || (node->left == NULL))
                    return ECLECTIC_STATUS_ERROR_NULL_POINTER;
                else
                    position = node->left;
                    node->left = NULL;
                break;
            case TREE_POSITION_RIGHT:
                if((node == NULL) || (node->right == NULL))
                    return ECLECTIC_STATUS_ERROR_NULL_POINTER;
                else
                    position = node->right;
                node->right = NULL;
                break;
            default:
                /* Invalid */
                return ECLECTIC_STATUS_ERROR_INVALID;
                break;
        }
        
        /* Remove */
        (void)tree_remove(tree, position, TREE_POSITION_LEFT);
        (void)tree_remove(tree, position, TREE_POSITION_RIGHT);
        if(tree->destroyCallback != NULL)
            tree->destroyCallback(position->data);
        
        /* Clean Up */
        eclectic_free((void **)&position);
        
        /* Decrement Size */
        tree->size--;
    }
    
    return ECLECTIC_STATUS_SUCCESS;
}

/*** Size***/
extern size_t tree_size(const tree_tree_t * const Tree)
{
    size_t size = 0;
    
    /*** Size ***/
    /* Error Check */
    if(Tree != NULL)
        size = Tree->size;
    
    return size;
}

/****************************************************************************************************
 * Function Definition (Private)
 ****************************************************************************************************/

/*** Create Node ***/
static tree_node_t *tree_createNode(void * const data)
{
    tree_node_t *node;
    
    /*** Create Node ***/
    /* Allocate Memory */
    node = eclectic_malloc(sizeof(*node));
    
    /* Error Check */
    if(node != NULL)
    {
        /* Initialize */
        node->data = data;
        node->left = node->right = NULL;
    }
    
    return node;
}

/*** Inorder ***/
static void tree_inorder(const tree_node_t * const Node, list_list_t * const list)
{    
    /*** Inorder ***/
    if(Node->left != NULL)
        tree_inorder(Node->left, list);
    (void)list_pushTail(list, Node->data);
    if(Node->right != NULL)
        tree_inorder(Node->right, list);
}

/*** Postorder ***/
static void tree_postorder(const tree_node_t * const Node, list_list_t * const list)
{
    /*** Postorder ***/
    if(Node->left != NULL)
        tree_postorder(Node->left, list);
    if(Node->right != NULL)
        tree_postorder(Node->right, list);
    (void)list_pushTail(list, Node->data);
}

/*** Preorder ***/
static void tree_preorder(const tree_node_t * const Node, list_list_t * const list)
{
    /*** Postorder ***/
    (void)list_pushTail(list, Node->data);
    if(Node->left != NULL)
        tree_preorder(Node->left, list);
    if(Node->right != NULL)
        tree_preorder(Node->right, list);
}
