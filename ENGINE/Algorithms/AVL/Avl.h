//
// Created by Anhelina Modenko on 06.10.2023.
//

#ifndef MARCHING_CUBES_AVL_H
#define MARCHING_CUBES_AVL_H

#define MAX(a,b) (a > b ? a : b)

class AVL {
    struct Node {
        void* key;
        void* val;
        int height;
        Node* left;
        Node* right;
    };

    Node* root;
    std::function<int(void*, void*)> keycmp;

public:
    AVL(std::function<int(void*, void*)> keycmp) : root(nullptr), keycmp(keycmp) {}

    Node* createRoot(void* key, void* val) {
        // implementation
    }

    Node* createEmptyRoot() {
        // implementation
    }

    Node* smallestKey(Node* root) {
        // implementation
    }

    int height(Node* root) {
        // implementation
    }

    int balanceFactor(Node* root) {
        // implementation
    }

    Node* insert(Node* root, void* key, void* val) {
        // implementation
    }

    Node* remove(Node* root, void* key) {
        // implementation
    }

    void* get(Node* root, void* key) {
        // implementation
    }

    void inorderTraverse(Node* root, void (*visit)(Node* node)) {
        // implementation
    }

    void preorderTraverse(Node* root, void (*visit)(Node* node)) {
        // implementation
    }

    void postorderTraverse(Node* root, void (*visit)(Node* node)) {
        // implementation
    }

    void deleteNode(Node* node) {
        // implementation
    }

    void deleteNodeDeep(Node* node) {
        // implementation
    }

    void free(Node* root) {
        // implementation
    }

    void freeDeep(Node* root) {
        // implementation
    }

private:
    Node* bstInsert(Node* root, void* key, void* val) {
        // implementation
    }

    Node* bstRemove(Node* root, void* key) {
        // implementation
    }

    Node* rebalance(Node* root) {
        // implementation
    }

    Node* rotateRight(Node* root) {
        // implementation
    }

    Node* rotateLeft(Node* root) {
        // implementation
    }

    void recalcHeight(Node* root) {
        // implementation
    }
};


#endif //MARCHING_CUBES_AVL_H
