/****************************************************************************************************
 * Include
 ****************************************************************************************************/

#include "list.h"
#include "mock_eclectic.h"
#include "tree.h"
#include "unity.h"

/****************************************************************************************************
 * Helper
 ****************************************************************************************************/

/*** Compare Callback ***/
static int treeTest_compareCallback(const void * const Data1, const void * const Data2)
{
    /*** Compare Callback ***/
    return *(const int *)Data1 - *(const int *)Data2;
}

void test_compareCallback_success(void)
{
    /*** Test Data ***/
    /* Type Definition */
    typedef struct testData_s
    {
        const int Integer1;
        const int Integer2;
        const int ExpectedResult;
    } testData_t;

    /* Test Data */
    const testData_t TestData[] =
    {
        /* Integer1 Smaller Than Integer2 */
        {-2, -1, -1},
        {-2, 1, -3},
        {1, 2, -1},

        /* Integer1 Equal To Integer2 */
        {-2, -2, 0},
        {0, 0, 0},
        {1, 1, 0},

        /* Integer1 Larger Than Integer2 */
        {2, 1, 1},
        {1, -2, 3},
        {-1, -2, 1},
    };
    const size_t TestDataCount = sizeof(TestData) / sizeof(TestData[0]);

    /* Variable */
    int actualResult;
    uint8_t i;

    /*** Compare Callback ***/
    for(i = 0; i < TestDataCount; i++)
    {
        actualResult = treeTest_compareCallback(&TestData[i].Integer1, &TestData[i].Integer2);
        TEST_ASSERT_EQUAL_INT(TestData[i].ExpectedResult, actualResult);
    }
}

/*** Create Tree ***/
static void *treeTest_createTree(tree_tree_t * const tree)
{
    static int data[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    uint8_t i = 0;
    tree_node_t *memory;
    
    /*** Create Tree ***/
    /* Memory */
    memory = malloc(11 * sizeof(tree_node_t));
    
    /* Initialize */
    TEST_ASSERT_EQUAL_INT(ECLECTIC_STATUS_SUCCESS, tree_init(tree, treeTest_compareCallback, NULL));
    
    /* Root (Level 0) */
    eclectic_malloc_ExpectAndReturn(sizeof(tree_node_t), &memory[0]);
    TEST_ASSERT_EQUAL_INT(ECLECTIC_STATUS_SUCCESS, tree_insert(tree, tree->root, &data[i++], TREE_POSITION_ROOT));
    
    /* Level 1 */
    eclectic_malloc_ExpectAndReturn(sizeof(tree_node_t), &memory[1]);
    TEST_ASSERT_EQUAL_INT(ECLECTIC_STATUS_SUCCESS, tree_insert(tree, tree->root, &data[i++], TREE_POSITION_LEFT));
    eclectic_malloc_ExpectAndReturn(sizeof(tree_node_t), &memory[2]);
    TEST_ASSERT_EQUAL_INT(ECLECTIC_STATUS_SUCCESS, tree_insert(tree, tree->root, &data[i++], TREE_POSITION_RIGHT));
    
    /* Level 2 */
    eclectic_malloc_ExpectAndReturn(sizeof(tree_node_t), &memory[3]);
    TEST_ASSERT_EQUAL_INT(ECLECTIC_STATUS_SUCCESS, tree_insert(tree, tree->root->left, &data[i++], TREE_POSITION_LEFT));
    eclectic_malloc_ExpectAndReturn(sizeof(tree_node_t), &memory[4]);
    TEST_ASSERT_EQUAL_INT(ECLECTIC_STATUS_SUCCESS, tree_insert(tree, tree->root->left, &data[i++], TREE_POSITION_RIGHT));
    eclectic_malloc_ExpectAndReturn(sizeof(tree_node_t), &memory[5]);
    TEST_ASSERT_EQUAL_INT(ECLECTIC_STATUS_SUCCESS, tree_insert(tree, tree->root->right, &data[i++], TREE_POSITION_LEFT));
    eclectic_malloc_ExpectAndReturn(sizeof(tree_node_t), &memory[6]);
    TEST_ASSERT_EQUAL_INT(ECLECTIC_STATUS_SUCCESS, tree_insert(tree, tree->root->right, &data[i++], TREE_POSITION_RIGHT));
    
    /* Level 3 */
    eclectic_malloc_ExpectAndReturn(sizeof(tree_node_t), &memory[7]);
    TEST_ASSERT_EQUAL_INT(ECLECTIC_STATUS_SUCCESS, tree_insert(tree, tree->root->left->right, &data[i++], TREE_POSITION_RIGHT));
    eclectic_malloc_ExpectAndReturn(sizeof(tree_node_t), &memory[8]);
    TEST_ASSERT_EQUAL_INT(ECLECTIC_STATUS_SUCCESS, tree_insert(tree, tree->root->right->left, &data[i++], TREE_POSITION_LEFT));
    eclectic_malloc_ExpectAndReturn(sizeof(tree_node_t), &memory[9]);
    TEST_ASSERT_EQUAL_INT(ECLECTIC_STATUS_SUCCESS, tree_insert(tree, tree->root->right->right, &data[i++], TREE_POSITION_LEFT));
    eclectic_malloc_ExpectAndReturn(sizeof(tree_node_t), &memory[10]);
    TEST_ASSERT_EQUAL_INT(ECLECTIC_STATUS_SUCCESS, tree_insert(tree, tree->root->right->right, &data[i++], TREE_POSITION_RIGHT));
    
    /* Size */
    TEST_ASSERT_EQUAL_UINT(11, tree_size(tree));
    
    return memory;
}

/****************************************************************************************************
 * Set Up/Tear Down
 ****************************************************************************************************/

void setUp(void)
{
}

void tearDown(void)
{
}

/****************************************************************************************************
 * Test (Public)
 ****************************************************************************************************/

/*** Destroy ***/
void test_destroy_error(void)
{
    /*** Destroy ***/
    /* NULL Pointer Error */
    TEST_ASSERT_EQUAL_INT(ECLECTIC_STATUS_ERROR_NULL_POINTER, tree_destroy(NULL));
}

void test_destroy_success(void)
{
    /*** Test Data ***/    
    /* Variable */
    uint8_t i;
    tree_node_t *memory;
    tree_tree_t tree;
    
    /* Set Up */
    memory = treeTest_createTree(&tree);
    
    /*** Destroy ***/
    /* Mock */
    for(i = 0; i < 11; i++)
        eclectic_free_ExpectAnyArgs();
    
    /* Destroy */
    TEST_ASSERT_EQUAL_INT(ECLECTIC_STATUS_SUCCESS, tree_destroy(&tree));
    
    /* Verify (Same State As When init'd) */
    TEST_ASSERT_EQUAL_PTR(treeTest_compareCallback, tree.compareCallback);
    TEST_ASSERT_EQUAL_PTR(NULL, tree.destroyCallback);
    TEST_ASSERT_NULL(tree.root);
    TEST_ASSERT_EQUAL_UINT(0, tree.size);
    
    /* Free */
    free(memory);
}

/*** Find ***/
void test_find_error(void)
{
    /*** Test Data ***/
    /* Test Data */
    const int Data = 7;
    
    /* Variable */
    int data;
    uint8_t i;
    tree_node_t *memory;
    tree_tree_t tree;
    
    /* Set Up */
    data = Data;
    
    /*** Find ***/
    /* Create */
    memory = treeTest_createTree(&tree);
    
    /* NULL Pointer Error */
    TEST_ASSERT_FALSE(tree_find(NULL, &data));
    tree.compareCallback = NULL;
    TEST_ASSERT_FALSE(tree_find(&tree, &data));
    
    /* Mock */
    for(i = 0; i < 11; i++)
        eclectic_free_ExpectAnyArgs();
    
    /* Destroy */
    TEST_ASSERT_EQUAL_INT(ECLECTIC_STATUS_SUCCESS, tree_destroy(&tree));
    
    /* Length Error */
    TEST_ASSERT_EQUAL_UINT(0, tree.size);
    TEST_ASSERT_FALSE(tree_find(&tree, &data));
    
    /* Free */
    free(memory);
}

void test_find_success(void)
{
    /*** Test Data ***/    
    /* Variable */
    int data, i;
    uint8_t j;
    list_node_t *listMemory;
    tree_tree_t tree;
    tree_node_t *treeMemory;
    
    /* Set Up */
    listMemory = malloc(11 * sizeof(list_node_t));
    treeMemory = treeTest_createTree(&tree);
    
    /*** Find ***/
    for(i = 0; i < 11; i++)
    {
        /* Mock */
        for(j = 0; j < 11; j++)
            eclectic_malloc_ExpectAndReturn(sizeof(list_node_t), &listMemory[j]);
        for(j = 0; j < 11; j++)
            eclectic_free_ExpectAnyArgs();
        
        /* True */
        data = i;
        TEST_ASSERT_TRUE(tree_find(&tree, &data));
        
        /* False */
        if(i != 0) // -0 == 0
        {
            /* Mock */
            for(j = 0; j < 11; j++)
                eclectic_malloc_ExpectAndReturn(sizeof(list_node_t), &listMemory[j]);
            for(j = 0; j < 11; j++)
                eclectic_free_ExpectAnyArgs();
            
            /* False */
            data = -i;
            TEST_ASSERT_FALSE(tree_find(&tree, &data));
        }
    }
    
    /* Mock */
    for(i = 0; i < 11; i++)
        eclectic_free_ExpectAnyArgs();
    
    /* Destroy */
    TEST_ASSERT_EQUAL_INT(ECLECTIC_STATUS_SUCCESS, tree_destroy(&tree));
    
    /* Free */
    free(listMemory);
    free(treeMemory);
}

/*** Initialize ***/
void test_init_error(void)
{
    /*** Initialize ***/
    /* NULL Pointer Error */
    TEST_ASSERT_EQUAL_INT(ECLECTIC_STATUS_ERROR_NULL_POINTER, tree_init(NULL, treeTest_compareCallback, free));
}

void test_init_success(void)
{
    /*** Test Data ***/
    /* Variable */
    tree_tree_t tree;
    
    /*** Initialize ***/
    /* Initialize */
    TEST_ASSERT_EQUAL_INT(ECLECTIC_STATUS_SUCCESS, tree_init(&tree, treeTest_compareCallback, free));
    
    /* Verify */
    TEST_ASSERT_EQUAL_PTR(treeTest_compareCallback, tree.compareCallback);
    TEST_ASSERT_EQUAL_PTR(free, tree.destroyCallback);
    TEST_ASSERT_NULL(tree.root);
    TEST_ASSERT_EQUAL_UINT(0, tree.size);
}

/*** Insert ***/
void test_insert_error(void)
{
    /*** Test Data ***/
    /* Test Data */
    const int Data = 7;
    
    /* Variable */
    int data;
    uint8_t i;
    tree_tree_t tree;
    tree_node_t *memory;
    
    /* Set Up */
    data = Data;
    memory = treeTest_createTree(&tree);
    
    /*** Insert ***/
    /* NULL Pointer Error */
    TEST_ASSERT_EQUAL_INT(ECLECTIC_STATUS_ERROR_NULL_POINTER, tree_insert(&tree, NULL, &data, TREE_POSITION_LEFT));
    TEST_ASSERT_EQUAL_INT(ECLECTIC_STATUS_ERROR_NULL_POINTER, tree_insert(&tree, NULL, &data, TREE_POSITION_RIGHT));
    TEST_ASSERT_EQUAL_INT(ECLECTIC_STATUS_ERROR_NULL_POINTER, tree_insert(NULL, tree.root, &data, TREE_POSITION_ROOT));
    
    /* Not Empty Error */
    TEST_ASSERT_EQUAL_INT(ECLECTIC_STATUS_ERROR_NOT_EMPTY, tree_insert(&tree, tree.root, &data, TREE_POSITION_LEFT));
    TEST_ASSERT_EQUAL_INT(ECLECTIC_STATUS_ERROR_NOT_EMPTY, tree_insert(&tree, tree.root, &data, TREE_POSITION_RIGHT));
    TEST_ASSERT_EQUAL_INT(ECLECTIC_STATUS_ERROR_NOT_EMPTY, tree_insert(&tree, tree.root, &data, TREE_POSITION_ROOT));
    
    /* Mock */
    eclectic_malloc_ExpectAndReturn(sizeof(tree_node_t), NULL);
    
    /* Memory Allocation Error */
    TEST_ASSERT_EQUAL_INT(ECLECTIC_STATUS_ERROR_MEMORY_ALLOCATION, tree_insert(&tree, tree.root->left->right, &data, TREE_POSITION_LEFT));
    
    /* Mock */
    eclectic_malloc_ExpectAndReturn(sizeof(tree_node_t), NULL);
    
    /* Memory Allocation Error */
    TEST_ASSERT_EQUAL_INT(ECLECTIC_STATUS_ERROR_MEMORY_ALLOCATION, tree_insert(&tree, tree.root->right->left, &data, TREE_POSITION_RIGHT));
    
    /* Mock */
    for(i = 0; i < 11; i++)
        eclectic_free_ExpectAnyArgs();
    
    /* Destroy */
    TEST_ASSERT_EQUAL_INT(ECLECTIC_STATUS_SUCCESS, tree_destroy(&tree));
    
    /* Free */
    free(memory);
    
    /* Initialize */
    TEST_ASSERT_EQUAL_INT(ECLECTIC_STATUS_SUCCESS, tree_init(&tree, treeTest_compareCallback, NULL));
    
    /* Mock */
    eclectic_malloc_ExpectAndReturn(sizeof(tree_node_t), NULL);
    
    /* Memory Allocation Error */
    TEST_ASSERT_EQUAL_INT(ECLECTIC_STATUS_ERROR_MEMORY_ALLOCATION, tree_insert(&tree, tree.root, &data, TREE_POSITION_ROOT));
    
    /* Invalid Error */
    TEST_ASSERT_EQUAL_INT(ECLECTIC_STATUS_ERROR_INVALID, tree_insert(&tree, tree.root, &data, TREE_POSITION_COUNT));
}

void test_insert_success(void)
{
    /*** Test Data ***/
    /* Test Data */
    const int Data = 7;
    
    /* Variable */
    int data;
    size_t i;
    list_node_t *listMemory;
    tree_tree_t tree;
    tree_node_t *treeMemory;
    
    /* Set Up */
    data = Data;
    
    /*** Insert ***/   
    /* Initialize */
    TEST_ASSERT_EQUAL_INT(ECLECTIC_STATUS_SUCCESS, tree_init(&tree, treeTest_compareCallback, NULL));
        
    /* Malloc */
    treeMemory = malloc(sizeof(tree_node_t));
    
    /* Mock */
    eclectic_malloc_ExpectAndReturn(sizeof(tree_node_t), treeMemory);
    
    /* Insert */
    TEST_ASSERT_EQUAL_INT(ECLECTIC_STATUS_SUCCESS, tree_insert(&tree, NULL, &data, TREE_POSITION_ROOT));
    
    /* Size */
    TEST_ASSERT_EQUAL_UINT(1, tree_size(&tree));
    
    /* Malloc */
    listMemory = malloc(sizeof(list_node_t));
    
    /* Mock */
    eclectic_malloc_ExpectAndReturn(sizeof(list_node_t), listMemory);
    eclectic_free_ExpectAnyArgs();
    
    /* Find */
    TEST_ASSERT_TRUE(tree_find(&tree, &data));
    
    /* Mock */
    eclectic_free_ExpectAnyArgs();
    
    /* Destroy */
    TEST_ASSERT_EQUAL_INT(ECLECTIC_STATUS_SUCCESS, tree_destroy(&tree));
    
    /* Free */
    free(listMemory);
    free(treeMemory);
}

/*** Order ***/
void test_order_error(void)
{
    /*** Test Data ***/
    /* Variable */
    int data;
    list_list_t list;
    tree_node_t *memory;
    tree_tree_t tree;
    
    /*** Order ***/
    /* Initialize */
    TEST_ASSERT_EQUAL_INT(ECLECTIC_STATUS_SUCCESS, tree_init(&tree, treeTest_compareCallback, NULL));
    
    /* NULL Pointer Error */
    TEST_ASSERT_EQUAL_INT(ECLECTIC_STATUS_ERROR_NULL_POINTER, tree_order(NULL, &list, TREE_ORDER_INORDER));
    TEST_ASSERT_EQUAL_INT(ECLECTIC_STATUS_ERROR_NULL_POINTER, tree_order(&tree, NULL, TREE_ORDER_INORDER));
    
    /* Length Error */
    TEST_ASSERT_EQUAL_INT(ECLECTIC_STATUS_ERROR_LENGTH, tree_order(&tree, &list, TREE_ORDER_INORDER));
    
    /* Malloc */
    memory = malloc(sizeof(tree_node_t));
    
    /* Mock */
    eclectic_malloc_ExpectAndReturn(sizeof(tree_node_t), &memory[0]);
    
    /* Insert */
    TEST_ASSERT_EQUAL_INT(ECLECTIC_STATUS_SUCCESS, tree_insert(&tree, NULL, &data, TREE_POSITION_ROOT));
    
    /* Invalid Error */
    TEST_ASSERT_EQUAL_INT(ECLECTIC_STATUS_ERROR_INVALID, tree_order(&tree, &list, TREE_POSITION_COUNT));
    
    /* Mock */
    eclectic_free_ExpectAnyArgs();
    
    /* Destroy */
    TEST_ASSERT_EQUAL_INT(ECLECTIC_STATUS_SUCCESS, tree_destroy(&tree));
    
    /* Free */
    free(memory);
}

void test_order_success(void)
{
    /*** Test Data ***/
    /* Type Definition */
    typedef struct testData_s
    {
        const tree_order_t Order;
        const int ExpectedList[11];
    } testData_t;
    
    /* Test Data */
    const testData_t TestData[] =
    {
        {
            TREE_ORDER_INORDER,
            {3, 1, 4, 7, 0, 8, 5, 2, 9, 6, 10}
        },
        {
            TREE_ORDER_POSTORDER,
            {3, 7, 4, 1, 8, 5, 9, 10, 6, 2, 0}
        },
        {
            TREE_ORDER_PREORDER,
            {0, 1, 3, 4, 7, 2, 5, 8, 6, 9, 10}
        }
    };
    const size_t TestDataCount = sizeof(TestData) / sizeof(TestData[0]);
    
    /* Variable */
    list_list_t actualList;
    uint8_t i, j;
    list_node_t *listMemory;
    tree_tree_t tree;
    tree_node_t *treeMemory;
    
    /* Set Up */
    listMemory = malloc(11 * sizeof(list_node_t));
    treeMemory = treeTest_createTree(&tree);
    
    /*** Order ***/
    for(i = 0; i < TestDataCount; i++)
    {
        /* Mock */
        for(j = 0; j < 11; j++)
            eclectic_malloc_ExpectAndReturn(sizeof(list_node_t), &listMemory[j]);
        
        /* Initialize */
        TEST_ASSERT_EQUAL_INT(ECLECTIC_STATUS_SUCCESS, list_init(&actualList, treeTest_compareCallback, NULL));
        
        /* Order */
        TEST_ASSERT_EQUAL_INT(ECLECTIC_STATUS_SUCCESS, tree_order(&tree, &actualList, TestData[i].Order));
        
        /* Verify */
        TEST_ASSERT_EQUAL_UINT(11, list_size(&actualList));
        for(j = 0; j < 11; j++)
            TEST_ASSERT_EQUAL_UINT(j, list_find(&actualList, &TestData[i].ExpectedList[j]));
        
        /* Mock */
        for(j = 0; j < 11; j++)
            eclectic_free_ExpectAnyArgs();
        
        /* Destroy */
        TEST_ASSERT_EQUAL_INT(ECLECTIC_STATUS_SUCCESS, list_destroy(&actualList));
    }
    
    /* Mock */
    for(j = 0; j < 11; j++)
        eclectic_free_ExpectAnyArgs();
    
    /* Destroy */
    TEST_ASSERT_EQUAL_INT(ECLECTIC_STATUS_SUCCESS, tree_destroy(&tree));
    
    /* Free */
    free(listMemory);
    free(treeMemory);
}

/*** Remove ***/
void test_remove_error(void)
{
    /*** Test Data ***/    
    /* Variable */
    tree_tree_t tree;
    
    /*** Remove ***/
    /* Initialize */
    TEST_ASSERT_EQUAL_INT(ECLECTIC_STATUS_SUCCESS, tree_init(&tree, treeTest_compareCallback, NULL));
    
    /* NULL Pointer Error */
    TEST_ASSERT_EQUAL_INT(ECLECTIC_STATUS_ERROR_NULL_POINTER, tree_remove(NULL, tree.root, TREE_POSITION_ROOT));
    TEST_ASSERT_EQUAL_INT(ECLECTIC_STATUS_ERROR_NULL_POINTER, tree_remove(&tree, tree.root, TREE_POSITION_ROOT));
    TEST_ASSERT_EQUAL_INT(ECLECTIC_STATUS_ERROR_NULL_POINTER, tree_remove(&tree, NULL, TREE_POSITION_LEFT));
    TEST_ASSERT_EQUAL_INT(ECLECTIC_STATUS_ERROR_NULL_POINTER, tree_remove(&tree, tree.root, TREE_POSITION_LEFT));
    TEST_ASSERT_EQUAL_INT(ECLECTIC_STATUS_ERROR_NULL_POINTER, tree_remove(&tree, NULL, TREE_POSITION_RIGHT));
    TEST_ASSERT_EQUAL_INT(ECLECTIC_STATUS_ERROR_NULL_POINTER, tree_remove(&tree, tree.root, TREE_POSITION_RIGHT));
    
    /* Invalid Error */
    TEST_ASSERT_EQUAL_INT(ECLECTIC_STATUS_ERROR_INVALID, tree_remove(&tree, tree.root, TREE_POSITION_COUNT));
    
    /* Destroy */
    TEST_ASSERT_EQUAL_INT(ECLECTIC_STATUS_SUCCESS, tree_destroy(&tree));
}

void test_remove_success(void)
{
    /*** Test Data ***/
    /* Test Data */
    const int Data = 7;
    
    /* Variable */
    int *data[3];
    size_t i;
    tree_tree_t tree;
    tree_node_t *treeMemory;
    
    /* Set Up */
    for(i = 0; i < 3; i++)
    {
        data[i] = malloc(sizeof(data[i]));
        *data[i] = Data + i;
    }
    
    /*** Remove ***/   
    /* Initialize */
    TEST_ASSERT_EQUAL_INT(ECLECTIC_STATUS_SUCCESS, tree_init(&tree, treeTest_compareCallback, free));
        
    /* Malloc */
    treeMemory = malloc(3 * sizeof(tree_node_t));
    
    /*
    /* Mock */
    eclectic_malloc_ExpectAndReturn(sizeof(tree_node_t), &treeMemory[0]);
    
    /* Insert */
    TEST_ASSERT_EQUAL_INT(ECLECTIC_STATUS_SUCCESS, tree_insert(&tree, NULL, data[0], TREE_POSITION_ROOT));
    
    /* Mock */
    eclectic_malloc_ExpectAndReturn(sizeof(tree_node_t), &treeMemory[1]);
    
    /* Insert */
    TEST_ASSERT_EQUAL_INT(ECLECTIC_STATUS_SUCCESS, tree_insert(&tree, tree.root, data[1], TREE_POSITION_LEFT));
    
    /* Mock */
    eclectic_malloc_ExpectAndReturn(sizeof(tree_node_t), &treeMemory[2]);
    
    /* Insert */
    TEST_ASSERT_EQUAL_INT(ECLECTIC_STATUS_SUCCESS, tree_insert(&tree, tree.root, data[2], TREE_POSITION_RIGHT));
    
    /* Size */
    TEST_ASSERT_EQUAL_UINT(3, tree_size(&tree));
    
    /* Mock */
    eclectic_free_ExpectAnyArgs();
    
    /* Remove */
    TEST_ASSERT_EQUAL_INT(ECLECTIC_STATUS_SUCCESS, tree_remove(&tree, tree.root, TREE_POSITION_LEFT));
    
    /* Mock */
    eclectic_free_ExpectAnyArgs();
    
    /* Size */
    TEST_ASSERT_EQUAL_UINT(2, tree_size(&tree));
    
    /* Remove */
    TEST_ASSERT_EQUAL_INT(ECLECTIC_STATUS_SUCCESS, tree_remove(&tree, tree.root, TREE_POSITION_RIGHT));
    
    /* Size */
    TEST_ASSERT_EQUAL_UINT(1, tree_size(&tree));
    
    /* Mock */
    eclectic_free_ExpectAnyArgs();
    
    /* Remove */
    TEST_ASSERT_EQUAL_INT(ECLECTIC_STATUS_SUCCESS, tree_remove(&tree, tree.root, TREE_POSITION_ROOT));
    
    /* Size */
    TEST_ASSERT_EQUAL_UINT(0, tree_size(&tree));
    
    /* Destroy */
    //TEST_ASSERT_EQUAL_INT(ECLECTIC_STATUS_SUCCESS, tree_destroy(&tree));
    
    /* Free */
    free(treeMemory);
}

/*** Size ***/
void test_size_error(void)
{
    /*** Size ***/
    /* NULL Pointer Error */
    TEST_ASSERT_EQUAL_UINT(0, tree_size(NULL));
}

void test_size_success(void)
{
    /*** Test Data ***/    
    /* Variable */
    uint8_t i;
    tree_node_t *memory;
    tree_tree_t tree;
    
    /* Set Up */
    memory = treeTest_createTree(&tree);
    
    /*** Size ***/
    /* Size */
    TEST_ASSERT_EQUAL_UINT(11, tree_size(&tree));
    
    /* Mock */
    for(i = 0; i < 11; i++)
        eclectic_free_ExpectAnyArgs();
    
    /* Destroy */
    TEST_ASSERT_EQUAL_INT(ECLECTIC_STATUS_SUCCESS, tree_destroy(&tree));
    
    /* Free */
    free(memory);
}
