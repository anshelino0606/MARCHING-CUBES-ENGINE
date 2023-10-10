#ifndef AVL_H
#define AVL_H

// max function macro
#define MAX(a, b) (a > b ? a : b)

// avl structure
typedef struct AVL
{
    void* key;
    void* val;
    int height;

    struct AVL* left;
    struct AVL* right;

    int (*keycmp)(void* key1, void* key2);
} AVL;

/*
    allocation
*/
// create root with key and value
AVL* avl_createRoot(void* key, void* val, int (*keycmp)(void* val1, void* val2));
// create root with null key and value
AVL* avl_createEmptyRoot(int (*keycmp)(void* val1, void* val2));

/*
    accessor functions
*/
// get the smallest key in a subtree
AVL* avl_smallestKey(AVL* root);
// get the height of a root
int avl_height(AVL* root);
// get the balance factor of a root
int avl_balanceFactor(AVL* root);

/*
    operation functions
*/
// insert into avl
AVL* avl_insert(AVL* root, void* key, void* val);
// insert into bst
AVL* avl_bst_insert(AVL* root, void* key, void* val);
// remove from avl
AVL* avl_remove(AVL* root, void* key);
// remove from bst
AVL* avl_bst_remove(AVL* root, void* key);
// rebalance the avl
AVL* avl_rebalance(AVL* root);
// rotate right around a root
AVL* avl_rotateRight(AVL* root);
// rotate left around a root
AVL* avl_rotateLeft(AVL* root);
// calcualte the height of a root considering the children's heights
void avl_recalcHeight(AVL* root);
// get the value stored with a key
void* avl_get(AVL* root, void* key);

/*
    traversal methods
*/
// inorder traversal
void avl_inorderTraverse(AVL* root, void (*visit)(AVL* node));
// preorder traversal
void avl_preorderTraverse(AVL* root, void (*visit)(AVL* node));
// postorder traversal
void avl_postorderTraverse(AVL* root, void (*visit)(AVL* node));

/*
    freeing methods
*/
// free node memory
void avl_deleteNode(AVL* node);
// free key, value, and node memory
void avl_deleteNodeDeep(AVL* node);
// free subtree memory
void avl_free(AVL* root);
// free subtree memory, including keys and values
void avl_freeDeep(AVL* root);

/*
    default key comparisons
    format:
        int <name>cmp(void *key1, void *key2);
*/
// string keys
int strkeycmp(void* key1, void* key2);
// integer keys
int intkeycmp(void* key1, void* key2);

#endif