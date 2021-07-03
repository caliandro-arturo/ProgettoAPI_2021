/**
 * API project 2020-2021
 * Author: Arturo Caliandro
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

int graphDimension;
int rankingLength;
int actualRankingSize = 0;

/**
 * Node struct for RB tree implementation for ranking.
 */
typedef struct treeNode {
    int graphId;
    int key;
    bool isRed;
    struct treeNode *father;
    struct treeNode *left;
    struct treeNode *right;
} rankingNode;

/*typedef struct {

} graphNode;*/

/*typedef struct {
    int id;
} graph;*/

/**
 * Node that represents a null value.
 */
rankingNode *nil;
/**
 * Node that represents the root of the ranking tree.
 */
rankingNode *root;

/**
 * Returns the lowest value.
 * @param val1 the first value
 * @param val2 the second value
 * @return the lowest value
 */
int minInt(int val1, int val2) {
    if (val1 <= val2)
        return val1;
    else return val2;
}

/**
 * Returns the higher value.
 * @param val1 the first value
 * @param val2 the second value
 * @return the higher value
 */
int maxInt(int val1, int val2) {
    if (val1 >= val2)
        return val1;
    else return val2;
}

/**
 * Returns the node containing the required key.
 * @param T the father to consider to do the research
 * @param key the value that identifies the searched node
 */
rankingNode *search(rankingNode *T, int key) {
    if (T == nil || T->key == key)
        return T;
    else if (T->key < key)
        return search(T->right, key);
    else return search(T->left, key);
}

/**
 * Returns the node with the lowest key, starting from a specified node.
 * @param subTree the root of the sub tree to use to start the research
 * @return the the node with the lowest key in the sub tree
 */
rankingNode *minNode(rankingNode *subTree) {
    rankingNode *current = subTree;
    while (current->left != nil)
        current = current->left;
    return current;
}

/**
 * Returns the node with the lowest key value that is higher than the node passed as parameter.
 * @param startNode the node to use as reference
 * @return the next node
 */
rankingNode *successor(rankingNode *startNode) {
    if (startNode == nil)
        return nil;
    if (startNode->right != nil)
        return minNode(startNode->right);
    rankingNode *startNodeFather = startNode->father;
    while (startNodeFather != nil && startNodeFather->right == startNode) {
        startNode = startNodeFather;
        startNodeFather = startNodeFather->father;
    }
    return startNodeFather;
}

/**
 * Applies a counterclockwise rotation between a node and his right son.
 * The son becomes the father of the node, and the node becomes his left son.
 * @param toRotate the father
 */
void leftRotate(rankingNode *toRotate) {
    rankingNode *rightSon = toRotate->right;
    toRotate->right = rightSon->left;
    if (rightSon->left != nil)
        rightSon->left->father = toRotate;
    rightSon->father = toRotate->father;
    if (toRotate->father == nil)
        root = rightSon;
    else if (toRotate == toRotate->father->left)
        toRotate->father->left = rightSon;
    else toRotate->father->right = rightSon;
    rightSon->left = toRotate;
    toRotate->father = rightSon;
}

/**
 * Applies a clockwise rotation between a node and his left son.
 * The son becomes the father of the node, and the node becomes his right son.
 * @param toRotate the father
 */
void rightRotate(rankingNode *toRotate) {
    rankingNode *leftSon = toRotate->left;
    toRotate->left = leftSon->right;
    if (leftSon->right != nil)
        leftSon->right->father = toRotate;
    leftSon->father = toRotate->father;
    if (toRotate->father == nil)
        root = leftSon;
    else if (toRotate == toRotate->father->left)
        toRotate->father->left = leftSon;
    else toRotate->father->right = leftSon;
    leftSon->right = toRotate;
    toRotate->father = leftSon;
}

/**
 * Fixes the RB Tree property after the specified node has been inserted.
 * @param newNode the new node added into the tree
 */
void RBInsertFixup(rankingNode *newNode) {
    while (newNode != root && newNode->father->isRed) {
        rankingNode *newNodeUncle;
        if (newNode->father == newNode->father->father->left)
            newNodeUncle = newNode->father->father->right;
        else
            newNodeUncle = newNode->father->father->left;
        if (newNodeUncle->isRed) {
            newNodeUncle->isRed = false;
            newNode->father->isRed = false;
            newNode->father->father->isRed = true;
            newNode = newNode->father->father;
        } else {
            if (newNode->father == newNode->father->father->left &&
                newNode == newNode->father->left) {
                bool color = newNode->father->isRed;
                newNode->father->isRed = newNode->father->father->isRed;
                newNode->father->father->isRed = color;
                rightRotate(newNode->father->father);
            }
            if (newNode->father == newNode->father->father->left &&
                newNode == newNode->father->right) {
                bool color = newNode->isRed;
                newNode->isRed = newNode->father->father->isRed;
                newNode->father->father->isRed = color;
                leftRotate(newNode->father);
                rightRotate(newNode->father->father);
            }
            if (newNode->father == newNode->father->father->right &&
                newNode == newNode->father->right) {
                bool color = newNode->father->isRed;
                newNode->father->isRed = newNode->father->father->isRed;
                newNode->father->father->isRed = color;
                leftRotate(newNode->father->father);
            }
            if (newNode->father == newNode->father->father->right &&
                newNode == newNode->father->left) {
                bool color = newNode->isRed;
                newNode->isRed = newNode->father->father->isRed;
                newNode->father->father->isRed = color;
                rightRotate(newNode->father);
                leftRotate(newNode->father->father);
            }
        }
    }
    /* root must always be black */
    root->isRed = false;
}

/**
 * Inserts a new node inside the RB Tree.
 * @param toInsert the node to add
 */
void RBInsert(rankingNode *toInsert) {
    rankingNode *previousNode = nil;
    rankingNode *currentNode = root;
    while (currentNode != nil) {
        previousNode = currentNode;
        if (toInsert->key < currentNode->key)
            currentNode = currentNode->left;
        else currentNode = currentNode->right;
    }
    toInsert->father = previousNode;
    if (previousNode == nil)
        root = toInsert;
    else if (toInsert->key < previousNode->key)
        previousNode->left = toInsert;
    else previousNode->right = toInsert;
    toInsert->left = nil;
    toInsert->right = nil;
    toInsert->isRed = true;
    RBInsertFixup(toInsert);
}

/**
 * Auxiliary method that fixes the RB tree after a deletion of a node, if the the deleted node was black.
 * @param substitute the node that has taken the place of the old removed node
 */
void RBDeleteFixup(rankingNode *substitute) {
    rankingNode *w;
    while (substitute != root && !substitute->isRed) {
        if (substitute == substitute->father->left) {
            w = substitute->father->right;
            if (w->isRed) {
                w->isRed = false;
                substitute->father->isRed = true;
                leftRotate(substitute->father);
                w = substitute->father->right;
            }
            if (!w->left->isRed && !w->right->isRed) {
                w->isRed = true;
                substitute = substitute->father;
            } else if (!w->right->isRed) {
                w->left->isRed = false;
                w->isRed = true;
                rightRotate(w);
                w = substitute->father->right;
            }
            w->isRed = substitute->father->isRed;
            substitute->father->isRed = false;
            w->right->isRed = false;
            leftRotate(substitute->father);
            substitute = root;
        } else {
            w = substitute->father->left;
            if (w->isRed) {
                w->isRed = false;
                substitute->father->isRed = true;
                rightRotate(substitute->father);
                w = substitute->father->left;
            }
            if (!w->right->isRed && !w->left->isRed) {
                w->isRed = true;
                substitute = substitute->father;
            } else if (!w->left->isRed) {
                w->right->isRed = false;
                w->isRed = true;
                leftRotate(w);
                w = substitute->father->left;
            }
            w->isRed = substitute->father->isRed;
            substitute->father->isRed = false;
            w->left->isRed = false;
            rightRotate(substitute->father);
            substitute = root;
        }
    }
    substitute->isRed = false;
}

/**
 * Removes a node from the RB Tree.
 * @param node the node to remove
 */
void RBDelete(rankingNode *node) {
    rankingNode *toDelete, *subTree;
    if (node->left == nil || node->right == nil)
        toDelete = node;
    else toDelete = successor(node);
    if (toDelete->left != nil)
        subTree = toDelete->left;
    else subTree = toDelete->right;
    if (subTree != nil)
        subTree->father = toDelete->father;
    if (toDelete->father == nil)
        root = subTree;
    else if (toDelete == toDelete->father->left)
        toDelete->father->left = subTree;
    else toDelete->father->right = subTree;
    if (toDelete != node) {
        node->key = toDelete->key;
    }
    free(toDelete);
    if (!node->isRed)
        RBDeleteFixup(node);
}

/**
 * Reads commands from input.
 */
void inputHandler() {
    char command[13];
    while (scanf("%s", command)) {

    }
}

/**
 * Initializes the program, setting dimension of graphs to classify and length of the ranking.
 */
void initialize() {
    scanf("%d %d", &graphDimension, &rankingLength);
}

int main() {
    initialize();
    inputHandler();
    return 0;
}
