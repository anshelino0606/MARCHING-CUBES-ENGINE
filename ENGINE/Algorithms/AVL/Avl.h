//
// Created by Anhelina Modenko on 06.10.2023.
//

#ifndef MARCHING_CUBES_AVL_H
#define MARCHING_CUBES_AVL_H

#define MAX(a,b) (a > b ? a : b)

class AVL {
public:

    static void* key;
    static void* val;
    static int height;

    static AVL* left;
    static AVL* right;
    
    static int (*keycmp)(void* key, void* other);

public:
    AVL();
    AVL(int (*keycmp)(void* key, void* other)){
        this->key = nullptr;
        this->val = nullptr;
        this->keycmp = keycmp;
    }

    static AVL* createRoot(void* key, void* val, int (*keycmp)(void* val1, void* val2)) {
        AVL* result = new AVL();
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

    static AVL* createEmptyRoot() {
        return createRoot(nullptr, nullptr, keycmp);
    }

    static AVL* smallestKey(AVL* root) {
        return root->left ? smallestKey(root->left) : root;
    }


    static int getHeight(AVL* avl) {
        return avl ? height : 0;
    }

    static int balanceFactor(AVL* avl) {
        return avl ? getHeight(left) - getHeight(right) : 0;
    }

    static AVL* insert(AVL* avl, void* key, void* val) {
        AVL* newAVL = bstInsert(avl, key, val);
        newAVL = rebalance(newAVL);

        return newAVL;
    }

    static AVL* remove(AVL* root, void* key) {
        AVL* newRoot = bstRemove(root, key);
        newRoot = rebalance(newRoot);

        return newRoot;
    }

    static void* get(AVL* root, void* key) {
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

    static void inorderTraverse(AVL* root, void (*visit)(AVL* avl)) {
        if (root->left)
            inorderTraverse(root->left, visit);

        visit(root);
        if (root->right)
            inorderTraverse(root->right, visit);
    }

    static void preorderTraverse(AVL* root, void (*visit)(AVL* AVL)) {
        visit(root);

        if (root->left)
            preorderTraverse(root->left, visit);
        if (root->right)
            preorderTraverse(root->right, visit);
    }

    static void postorderTraverse(AVL* root, void (*visit)(AVL* AVL)) {
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

    static void free(AVL* root) {
        postorderTraverse(root, deleteAVL);
    }

    static void freeDeep(AVL* root) {
        postorderTraverse(root, deleteAVLDeep);
    }

private:
    static AVL* bstInsert(AVL* avl, void* key, void* val) {
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

    static AVL* bstRemove(AVL* avl, void* key) {
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

    static AVL* rebalance(AVL* avl) {
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

    static AVL* rotateRight(AVL* avl) {
        if (!avl->left)
            return avl;

        AVL* newAvl = avl->left;
        avl->left = newAvl->right;
        newAvl->right = avl;

        recalcHeight(avl);
        recalcHeight(newAvl);

        return newAvl;
    }

    static AVL* rotateLeft(AVL* avl) {
        if (!avl->right)
            return avl;

        AVL* newAvl = avl->right;
        avl->right = newAvl->left;
        newAvl->left = avl;

        recalcHeight(avl);
        recalcHeight(newAvl);

        return newAvl;
    }

    static void recalcHeight(AVL* avl) {
        if (avl)
            avl->height = 1 + MAX(getHeight(avl->left), getHeight(avl->right));
    }
};


#endif //MARCHING_CUBES_AVL_H
