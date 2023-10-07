//
// Created by Anhelina Modenko on 06.10.2023.
//

#ifndef MARCHING_CUBES_AVL_H
#define MARCHING_CUBES_AVL_H

#define MAX(a,b) (a > b ? a : b)

class AVL {
public:

    void* key;
    void* val;
    int height;

    AVL* left;
    AVL* right;
    
    int (*keycmp)(void* key, void* other);

public:
    AVL(int (*keycmp)(void* key, void* other)){
        this->key = nullptr;
        this->val = nullptr;
        this->keycmp = keycmp;
    }

    AVL* createRoot(void* key, void* val, int (*keycmp)(void* val1, void* val2)) {
        AVL* result = new AVL(keycmp);
        if (!result)
            return nullptr;
        result->key = key;
        result->val = val;
        result->height = 1;

        result->left = nullptr;
        result->right = nullptr;

        result->keycmp = keycmp;

        return result;
    }

    AVL* createEmptyRoot() {
        return createRoot(nullptr, nullptr, keycmp);
    }

    AVL* smallestKey(AVL* root) {
        return root->left ? smallestKey(root->left) : root;
    }


    int getHeight(AVL* avl) {
        return avl ? height : 0;
    }

    int balanceFactor(AVL* avl) {
        return avl ? getHeight(left) - getHeight(right) : 0;
    }

    AVL* insert(AVL* avl, void* key, void* val) {
        AVL* newAVL = bstInsert(avl, key, val);
        newAVL = rebalance(newAVL);

        return newAVL;
    }

    AVL* remove(AVL* root, void* key) {
        AVL* newRoot = bstRemove(root, key);
        newRoot = rebalance(newRoot);

        return newRoot;
    }

    void* get(AVL* root, void* key) {
        if (!root)
            return nullptr;

        char cmp = root->keycmp(key, root->key);
        if (!cmp)
            return root->val;
        else if (cmp>0)
            return root->right ? get(root->right, key) : nullptr;
        else
            return root->left ? get(root->left, key) : nullptr;
    }

    void inorderTraverse(AVL* root, void (*visit)(AVL* avl)) {
        if (root->left)
            inorderTraverse(root->left, visit);

        visit(root);
        if (root->right)
            inorderTraverse(root->right, visit);
    }

    void preorderTraverse(AVL* root, void (*visit)(AVL* AVL)) {
        visit(root);

        if (root->left)
            preorderTraverse(root->left, visit);
        if (root->right)
            preorderTraverse(root->right, visit);
    }

    void postorderTraverse(AVL* root, void (*visit)(AVL* AVL)) {
        if (root->left)
            postorderTraverse(root->left, visit);
        if (root->right)
            postorderTraverse(root->right, visit);
        visit(root);
    }

    static void deleteAVL(AVL* AVL) {
        delete AVL;
    }

    static void deleteAVLDeep(AVL* AVL) {
        AVL->key = nullptr;
        AVL->val = nullptr;
        delete AVL;
    }

    void free(AVL* root) {
        postorderTraverse(root, deleteAVL);
    }

    void freeDeep(AVL* root) {
        postorderTraverse(root, deleteAVLDeep);
    }

private:
    AVL* bstInsert(AVL* avl, void* key, void* val) {
        if (!avl) {
            avl->key = key;
            avl->val = val;
            avl->height = 1;
            return avl;
        }

        if (keycmp(key, avl->key) < 0) {
            if (avl->left)
                avl->left = insert(avl->left, key, val);
            else
                avl->left = createRoot(key, val, keycmp);
        } else {
            if (avl->right)
                avl->right = insert(avl->right, key, val);
            else
                avl->right = createRoot(key, val, keycmp);
        }
        recalcHeight(avl);
        return avl;
    }

    AVL* bstRemove(AVL* avl, void* key) {
        int cmp = avl->keycmp(key, avl->key);
        if (cmp < 0) {
            if (avl->left) {
                avl->left = avl->remove(avl->left, key);
                recalcHeight(avl->left);
            }
        } else if (cmp > 0) {
            if (avl->right) {
                avl->right = remove(avl->right, key);
                recalcHeight(avl->right);
            }
        } else {
                // leaf node
                if (!avl->left && !avl->right) {
                    free(avl);
                    return nullptr;
                }
                // has one child
                if (avl->left && !avl->right) {
                    AVL* result = avl->left;
                    free(avl);
                    return result;
                } else if (!avl->left && !avl->right) {
                    AVL* result = avl->right;
                    free(avl);
                    return result;
                }
                // has both children
                AVL* substitution = smallestKey(avl->right);
                avl->key = substitution->key;
                avl->val = substitution->val;
                avl = remove(avl->right, avl->key);
        }
        recalcHeight(avl);
        return avl;
    }

    AVL* rebalance(AVL* avl) {
        int bf = balanceFactor(avl);
        if (bf < -1 || bf > 1) {
            /*
            * left-left
            * - right rotation arount the root
            */
            if (bf > 1 && balanceFactor(avl->left) >= 0) {
                avl = rotateRight(avl);
            }
            /*
             * left-right
             * - left rotation around child
             * - rotate right arout root
             */
            else if (bf > 1) {
                avl->left = rotateLeft(avl->left);
                avl = rotateRight(avl);
            }
            /*
             * right-right
             * - left rotation around root
             */
            else if (bf < -1 && balanceFactor(avl->right) <= 0) {
                avl = rotateLeft(avl);
            }
            /*
             * right-left
             * - right rotation around child
             * - left rotation around root
             */
            else {
                avl->right = rotateRight(avl->right);
                avl = rotateLeft(avl);
            }
        }
        return avl;
    }

    AVL* rotateRight(AVL* avl) {
        if (!avl->left)
            return avl;

        AVL* newAvl = avl->left;
        avl->left = newAvl->right;
        newAvl->right = avl;

        recalcHeight(avl);
        recalcHeight(newAvl);

        return newAvl;
    }

    AVL* rotateLeft(AVL* avl) {
        if (!avl->right)
            return avl;

        AVL* newAvl = avl->right;
        avl->right = newAvl->left;
        newAvl->left = avl;

        recalcHeight(avl);
        recalcHeight(newAvl);

        return newAvl;
    }

    void recalcHeight(AVL* avl) {
        if (avl)
            avl->height = 1 + MAX(getHeight(avl->left), getHeight(avl->right));
    }
};


#endif //MARCHING_CUBES_AVL_H
